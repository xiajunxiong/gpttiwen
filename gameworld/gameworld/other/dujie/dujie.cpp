#include "dujie.hpp"
#include "dujieconfig.hpp"

#include "engineadapter.h"
#include "gamelog.h"
#include "servercommon/errornum.h"
#include "other/route/mailroute.hpp"
#include "servercommon/string/gameworldstr.h"

#include "item/knapsack.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/event/eventhandler.hpp"
#include "other/funopen/funopen.hpp"
#include "other/dreamnoteschallenge/dreamnoteschallenge.hpp"
#include "other/couragechallenge/couragechallenge.h"
#include "other/prestige/prestige.hpp"
#include "other/title/title.hpp"
#include "other/lunhuiwangchuan/lunhuiwangchuan.hpp"
#include "other/salary/salary.hpp"
#include "other/courseofgrowth/courseofgrowth.hpp"
#include "other/capability/capability.hpp"

#include "config/logicconfigmanager.hpp"
#include "protocol/msgdujie.hpp"
#include "other/advancecareer/advancecareer.hpp"


DuJie::DuJie()
{
	m_module_mgr = NULL;
}

DuJie::~DuJie()
{
}

void DuJie::Init(RoleModuleManager * module_mgr, const DuJieParam & param)
{
	m_module_mgr = module_mgr;
	m_param = param;
}

void DuJie::GetInitParam(DuJieParam * param)
{
	*param = m_param;
}

void DuJie::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid)
	{
		return;
	}
	if (0 == m_param.task_param_list_[DU_JIE_TASK_PARAM_SHAN_HAI_BOSS_TODAY_PASS])
	{//间断
		m_param.task_param_list_[DU_JIE_TASK_PARAM_SHAN_HAI_BOSS_DAYS] = 0;
	}
	else
	{
		m_param.task_param_list_[DU_JIE_TASK_PARAM_SHAN_HAI_BOSS_TODAY_PASS] = 0;
	}

	memset(m_param.day_assist_times_, 0, sizeof(m_param.day_assist_times_));
}

void DuJie::OnReq(int req_type, int param_1)
{
	switch (req_type)
	{
	case Protocol::CSDuJieReq::REQ_TYPE_ROLE_INFO:
		{
			this->SendInfo();
		}
		break;
	default:
		return;
	}
}

bool DuJie::CanEnterFB(int fb_seq, int is_assist)
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_DU_JIE))
	{
		return false;
	}

	if (!is_assist)
	{//非助战必须未通关
		if (m_param.max_pass_fb_seq + 1 != fb_seq)
		{
			return false;
		}
	}
	else
	{//助战需要已通关
		if (m_param.max_pass_fb_seq >= fb_seq)
		{//已通关
			return true;
		}
		else
		{//未通过
			return false;
		}
	}

	const DuJieFbCfg* fb_cfg = LOGIC_CONFIG->GetDuJieConfig()->GetFbCfg(fb_seq);
	if (NULL == fb_cfg)
	{
		return false;
	}

	switch (fb_cfg->front_task_type)
	{
	//这几种特殊处理.
	case DU_JIE_TASK_TYPE_MONSTER_GROUP:
	case DU_JIE_TASK_TYPE_COUESE_OF_GROWTH:
		{
			if (1 != this->GetTaskParam(fb_seq))
			{
				return false;
			}
		}
		break;
	default:
		{
			if (fb_cfg->front_task_param_1 > this->GetTaskParam(fb_seq))
			{
				return false;
			}
		}
		break;
	}

	return true;
}

void DuJie::OnFBFinish(int fb_seq)
{
	//助战奖励
	this->AssistFinish(fb_seq);
	

	if (!this->CanEnterFB(fb_seq, false))
	{
		return;
	}

	const DuJieFbCfg* fb_cfg = LOGIC_CONFIG->GetDuJieConfig()->GetFbCfg(fb_seq);
	if (NULL == fb_cfg)
	{
		return;
	}

	//记录
	m_param.max_pass_fb_seq++;
	m_param.next_front_gate_pass = 0;

	//奖励
	if (m_module_mgr->GetKnapsack()->CheckForPutList((short)fb_cfg->reward_num, fb_cfg->reward_list_))
	{
		m_module_mgr->GetKnapsack()->PutList((short)fb_cfg->reward_num, fb_cfg->reward_list_, PUT_REASON_DU_JIE);
	}
	else
	{
		UNSTD_STATIC_CHECK(DuJieFbCfg::MAX_REWARD_NUM <= MAX_ATTACHMENT_ITEM_NUM);
		static MailContentParam contentparam;
		contentparam.Reset();
		int len_1 = SNPRINTF(contentparam.subject,
			sizeof(contentparam.subject),
			gamestring::g_knapsack_put_subject);
		int len_2 = SNPRINTF(contentparam.contenttxt,
			sizeof(contentparam.contenttxt),
			gamestring::g_knapsack_put_content);

		for (int i = 0; i < fb_cfg->reward_num && i < DuJieFbCfg::MAX_REWARD_NUM; i++)
		{
			FillMailContentParam(contentparam, fb_cfg->reward_list_[i]);
		}
		if (len_1 > 0 && len_2 > 0)
		{
			MailRoute::MailToUser(IntToUid(m_module_mgr->GetUid()), MAIL_REASON_DEFAULT, contentparam);
		}
	}
	m_module_mgr->GetRole()->GetRoleModuleManager()->GetKnapsack()->NotifyGetItemList((short)fb_cfg->reward_num, fb_cfg->reward_list_, GET_REASON_DU_JIE);

	m_module_mgr->GetTitle()->AddTitle(fb_cfg->reward_title_id, "DuJie::OnFBFinish");
	m_module_mgr->GetAdvanceCareer()->CheckAdvanceCond(ADVANCE_COND_TYPE_DU_JIE, m_param.max_pass_fb_seq, 0);

	//日志
	gamelog::g_log_fb_other.printf(LL_INFO, "%s role[%d %s] max_pass_fb_level:%d.",
		__FUNCTION__,
		m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
		m_param.max_pass_fb_seq);
}

void DuJie::AssistFinish(int fb_index)
{
	if (!this->CanEnterFB(fb_index, true))
	{
		return;
	}

	const DuJieOtherCfg& other_cfg = LOGIC_CONFIG->GetDuJieConfig()->GetOtherCfg();

	//可领取
	if (other_cfg.day_assist_times <= m_param.day_assist_times_[0])
	{
		return;
	}

	//记录
	m_param.day_assist_times_[0]++;
	this->SendInfo();
	m_module_mgr->NoticeNum(errornum::EN_ASSIST_REWARD);

	//奖励
	if (!other_cfg.assist_reward.empty())
	{
		if (m_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)other_cfg.assist_reward.size(), &other_cfg.assist_reward[0]))
		{
			m_module_mgr->GetKnapsack()->PutList((short)other_cfg.assist_reward.size(), &other_cfg.assist_reward[0], PUT_REASON_RA_CHAO_ZHI_GIFT);
		}
		else
		{
			m_module_mgr->GetKnapsack()->SendMail(&other_cfg.assist_reward[0], (short)other_cfg.assist_reward.size(), SNED_MAIL_TYPE_DEFAULT, true);
		}
	}

	//日志
	gamelog::g_log_fb_other.printf(LL_INFO, "%s role[%d %s] fb_index:%d day_assist_times_:%d.",
		__FUNCTION__,
		m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
		fb_index, m_param.day_assist_times_[0]);
}

void DuJie::ChangeTaskParam(int task_type, int task_param)
{
	bool need_send = false;
	int save_param = 0;
	switch (task_type)
	{
	case DU_JIE_TASK_TYPE_SHAN_HAI_BOSS:
		{//task_param 任务难度
			if (LOGIC_CONFIG->GetDuJieConfig()->GetTaskShanHaiBossHard() > task_param)
			{
				return;
			}
			m_param.task_param_list_[DU_JIE_TASK_PARAM_SHAN_HAI_BOSS_DAYS]++;
			m_param.task_param_list_[DU_JIE_TASK_PARAM_SHAN_HAI_BOSS_TODAY_PASS]++;

			save_param = m_param.task_param_list_[DU_JIE_TASK_PARAM_SHAN_HAI_BOSS_DAYS];
			if (save_param
				> m_param.task_param_list_[DU_JIE_TASK_PARAM_SHAN_HAI_BOSS_MAX_DAYS])
			{
				m_param.task_param_list_[DU_JIE_TASK_PARAM_SHAN_HAI_BOSS_MAX_DAYS]
					= save_param;
				
				need_send = true;
			}
		}
		break;
	case DU_JIE_TASK_TYPE_MONSTER_GROUP:
		{
			if (0 != m_param.next_front_gate_pass)
			{
				return;
			}

			const DuJieFbCfg* fb_cfg = LOGIC_CONFIG->GetDuJieConfig()->GetFbCfg(m_param.max_pass_fb_seq + 1);
			if (NULL == fb_cfg)
			{
				return;
			}
			if (DU_JIE_TASK_TYPE_MONSTER_GROUP != fb_cfg->front_task_type
				|| task_param != fb_cfg->front_task_param_1)
			{
				return;
			}
			m_param.next_front_gate_pass = 1;
			save_param = m_param.next_front_gate_pass;
			need_send = true;
		}
		break;
	case DU_JIE_TASK_TYPE_LUN_HUI_WANG_CHUAN:
		{//task_param 最大关卡seq
			task_param = m_module_mgr->GetLunHuiWangChuan()->GetPassIDX();

			if (m_param.task_param_list_[DU_JIE_TASK_PARAM_LUN_HUI_WANG_CHUAN]
				< task_param)
			{
				m_param.task_param_list_[DU_JIE_TASK_PARAM_LUN_HUI_WANG_CHUAN]
					= task_param;

				need_send = true;
			}
			save_param = m_param.task_param_list_[DU_JIE_TASK_PARAM_LUN_HUI_WANG_CHUAN];
		}
		break;
	default:
		return;
	}

	if (need_send)
	{
		this->SendInfo();
	}

	//日志
	gamelog::g_log_fb_other.printf(LL_INFO, "%s role[%d %s] task_type:%d. save_param:%d.",
		__FUNCTION__,
		m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
		task_type, save_param);
}

int DuJie::GetTaskParam(int fb_seq)
{
	int ret_param = -1;
	const DuJieFbCfg* fb_cfg = LOGIC_CONFIG->GetDuJieConfig()->GetFbCfg(fb_seq);
	if (NULL == fb_cfg)
	{
		return ret_param;
	}

	UNSTD_STATIC_CHECK(11 == DU_JIE_TASK_TYPE_MAX);
	switch (fb_cfg->front_task_type)
	{
	case DU_JIE_TASK_TYPE_DREAM_NOTES_CHALLENGE:
		{
			ret_param = m_module_mgr->GetDreamNotesChallenge()->GetPassLevel();
		}
		break;
	case DU_JIE_TASK_TYPE_COURAGE_CHALLENGE:
		{
			ret_param = m_module_mgr->GetCourageChallenge()->GetPassMaxLayerLevel();
		}
		break;
	case DU_JIE_TASK_TYPE_MONSTER_GROUP:
		{
			ret_param = m_param.next_front_gate_pass;
		}
		break;
	case DU_JIE_TASK_TYPE_LUN_HUI_WANG_CHUAN:
		{
			ret_param = m_param.task_param_list_[DU_JIE_TASK_PARAM_LUN_HUI_WANG_CHUAN];
		}
		break;
	case DU_JIE_TASK_TYPE_SHAN_HAI_BOSS:
		{
			ret_param = m_param.task_param_list_[DU_JIE_TASK_PARAM_SHAN_HAI_BOSS_MAX_DAYS];
		}
		break;
	case DU_JIE_TASK_TYPE_PRESTIGE:
		{
			ret_param = m_module_mgr->GetPrestige()->GetVillageCountByLevelLower(fb_cfg->front_task_param_2);
		}
		break;
	case DU_JIE_TASK_TYPE_TOTAL_CAPABILITY:
		{
			ret_param = m_module_mgr->GetRole()->GetCapabilityWithFightPetAndPartner();
		}
		break;
	case DU_JIE_TASK_TYPE_GET_CHIVALROUS:
		{
			ret_param = m_module_mgr->GetSalary()->GetHistoryChivalrous();
		}
		break;
	case DU_JIE_TASK_TYPE_COURSE_LEVEL:
		{
			ret_param = m_module_mgr->GetCourseOfGrowth()->GetCourseLevel();
		}	
		break;
	case DU_JIE_TASK_TYPE_COUESE_OF_GROWTH:
		{
			ret_param = m_module_mgr->GetCourseOfGrowth()->IsReachCourseOfGrowth(fb_cfg->front_task_param_1, fb_cfg->front_task_param_2) ? 1 : 0;
		}
		break;
	case DU_JIE_TASK_TYPE_ROLE_CAPABILITY:
		{
			ret_param = m_module_mgr->GetCapability()->GetRoleCapability();
		}
		break;
	default:
		break;
	}

	return ret_param;
}

void DuJie::SendInfo()
{
	Protocol::SCDuJieRoleInfo info;
	info.max_pass_fb_seq = m_param.max_pass_fb_seq;
	info.next_front_gate_pass = this->GetTaskParam(m_param.max_pass_fb_seq + 1);

	info.day_assist_times = m_param.day_assist_times_[0];
	
	m_module_mgr->NetSend(&info, sizeof(info));
}

