#include "roleactivitycijiuyingxin.hpp"

#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitycijiuyingxinconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "protocol/randactivity/msgragivepetdraw.hpp"

#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"

RoleActivityCiJiuYingXin::RoleActivityCiJiuYingXin(int type)
	:RoleActivity(type)
{

}

RoleActivityCiJiuYingXin::~RoleActivityCiJiuYingXin()
{

}

void RoleActivityCiJiuYingXin::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RACiJiuYingXinParam));
}

void RoleActivityCiJiuYingXin::GetInitParam(RoleActivityRoleData *p)
{
	memcpy(p->base_data, &m_param, sizeof(RACiJiuYingXinParam));
}

void RoleActivityCiJiuYingXin::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_CI_JIU_YING_XIN))
	{
		return;
	}

	int rand_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_CI_JIU_YING_XIN);

	if (m_param.open_day == rand_open_day)
	{
		//return;//屏蔽让GM命令能直接刷新题目
	}
	m_param.open_day = rand_open_day;
	m_param.OnDayReset();
	const RandActivityCiJiuYingXinConfig * config = static_cast<const RandActivityCiJiuYingXinConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_CI_JIU_YING_XIN));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	int id = config->RandSubject();
	m_param.id = id;
	this->SendInfo();
	this->SetRoleDataChange();
}

void RoleActivityCiJiuYingXin::OnLoginSendInfo()
{
	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_CI_JIU_YING_XIN))
	{
		return;
	}

	this->SendInfo();
}

void RoleActivityCiJiuYingXin::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_CI_JIU_YING_XIN != rand_act_type)
	{
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(rand_act_type))
	{
		return;
	}

	// 防止误启动活动，不在跨服启动的活动会导致重置
	if (CrossConfig::Instance().IsHiddenServer())
	{
		RandActivity *activity = ramgr->ForceGetRandActivity(rand_act_type);
		if (NULL != activity && !activity->CanSyncActivityDataToCross())
		{
			return;
		}
	}

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.begin_time);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.begin_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		int rand_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_CI_JIU_YING_XIN);
		const RandActivityCiJiuYingXinConfig * config = static_cast<const RandActivityCiJiuYingXinConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_CI_JIU_YING_XIN));
		if (NULL == config)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}
		int id = config->RandSubject();
		m_param.id = id;
		m_param.open_day = rand_open_day;
		this->SetRoleDataChange();
		this->SendInfo();
	}
}

void RoleActivityCiJiuYingXin::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
{
	if (NULL == raor)
	{
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_CI_JIU_YING_XIN))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_CI_JIU_YING_XIN);

	switch (raor->opera_type)
	{
	case 0:
		{
			this->SendInfo();
		}
		break;
	case 1:
		{
			this->Answer(raor->param_1);
		}
		break;
	}
}

void RoleActivityCiJiuYingXin::SendInfo()
{
	Protocol::SCRandActivityCJYXInfo info;
	info.id = m_param.id;
	info.today_answer = m_param.today_answer;
	info.has_answer_count = m_param.has_answer_count;
	memcpy(info.has_answer_list, m_param.has_answer_list, sizeof(info.has_answer_list));
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityCiJiuYingXin::Answer(int answer_id)
{
	const RandActivityCiJiuYingXinConfig * config = static_cast<const RandActivityCiJiuYingXinConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_CI_JIU_YING_XIN));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (0 == m_param.id)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (0 < m_param.today_answer)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}
	const RACiJiuYingXinSubjectCfg * subject = config->GetCiJiuYingXinSubjectCfg(m_param.id);
	if (subject == NULL)return;

	//检查是否选择过
	for (int check_ans = 0; check_ans < 2 && check_ans < m_param.has_answer_count; check_ans++)
	{
		if (answer_id == m_param.has_answer_list[check_ans])
		{
			return;
		}
	}

	if (subject->answer_id != answer_id)
	{
		if (m_param.has_answer_count < 0 || m_param.has_answer_count >= 2)return;
		
		m_param.has_answer_list[m_param.has_answer_count++] = answer_id;
	}
	else
	{
		const RACiJiuYingXinRewardListCfg * reward_cfg = config->GetCiJiuYingXinRewardListCfg(m_param.id);
		if (reward_cfg != NULL)
		{
			if (!m_role_module_mgr->GetKnapsack()->CheckForPutList(reward_cfg->item_count, reward_cfg->item_list))
			{
				//放入背包失败，改发邮件
				static MailContentParam contentparam; contentparam.Reset();
				for (int i = 0; i < MAX_ATTACHMENT_ITEM_NUM && i < ARRAY_ITEM_COUNT(reward_cfg->item_list); i++)
				{
					const ItemBase * item_base = ITEMPOOL->GetItem(reward_cfg->item_list[i].item_id);

					if (item_base != NULL)
					{
						contentparam.item_list[i].item_id = reward_cfg->item_list[i].item_id;
						contentparam.item_list[i].num = reward_cfg->item_list[i].num;
						contentparam.item_list[i].is_bind = (reward_cfg->item_list[i].is_bind) ? 1 : 0;
						contentparam.item_list[i].invalid_time = item_base->CalInvalidTime();
					}
				}
				int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_reward_subject);
				int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_reward_content, RAND_ACTIVITY_TYPE_CI_JIU_YING_XIN);
				if (lensub > 0 && length > 0)
				{
					MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
				}
			}
			else
			{
				//给予
				if (!m_role_module_mgr->GetKnapsack()->PutList(reward_cfg->item_count, reward_cfg->item_list, PUT_REASON_RA_CI_JIU_YING_XIN))
				{
					m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
					return;
				}
			}
			
		}
		m_param.today_answer = 1;
	}
	this->SendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityCiJiuYingXin::Answer user[%d, %s], level[%d], type[%d], today_answer[%d] has_count[%d] has_list_0[%d] has_list_1[%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(),
		PUT_REASON_RA_CI_JIU_YING_XIN, m_param.today_answer, m_param.has_answer_count, m_param.has_answer_list[0], m_param.has_answer_list[1]);
}
