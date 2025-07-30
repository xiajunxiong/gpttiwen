#include "roleactivityniuxingdayun.hpp"

#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityniuxingdayunconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "protocol/randactivity/msgragivepetdraw.hpp"

#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/route/mailroute.hpp"
#include "servercommon/string/gameworldstr.h"

RoleActivityNiuXingDaYun::RoleActivityNiuXingDaYun(int type)
	:RoleActivity(type)
{

}

RoleActivityNiuXingDaYun::~RoleActivityNiuXingDaYun()
{

}

void RoleActivityNiuXingDaYun::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RANiuXingDaYunParam));
}

void RoleActivityNiuXingDaYun::GetInitParam(RoleActivityRoleData *p)
{
	memcpy(p->base_data, &m_param, sizeof(RANiuXingDaYunParam));
}

void RoleActivityNiuXingDaYun::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_NIU_XING_DA_YUN))
	{
		return;
	}
	int rand_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_NIU_XING_DA_YUN);
	if (rand_open_day == m_param.open_day)
	{
		//return;//屏蔽让GM命令能直接刷新题目
	}
	m_param.today_reward = 0;
	m_param.type_id = 0;
	const RandActivityNiuXingDaYunConfig * config = static_cast<const RandActivityNiuXingDaYunConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_NIU_XING_DA_YUN));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RANiuXingDaYunConfig * subject_cfg = config->RandSubject(rand_open_day);
	if (subject_cfg != NULL)
	{
		m_param.type_id = subject_cfg->type;
	}
	this->SendInfo();
	this->SetRoleDataChange();
}

void RoleActivityNiuXingDaYun::OnLoginSendInfo()
{
	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_NIU_XING_DA_YUN))
	{
		return;
	}

	this->SendInfo();
}

void RoleActivityNiuXingDaYun::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_NIU_XING_DA_YUN != rand_act_type)
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
		m_param.begin_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		m_param.today_reward = 0;
		const RandActivityNiuXingDaYunConfig * config = static_cast<const RandActivityNiuXingDaYunConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_NIU_XING_DA_YUN));
		if (NULL == config)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}
		int rand_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_NIU_XING_DA_YUN);
		const RANiuXingDaYunConfig * subject_cfg = config->RandSubject(rand_open_day);
		if (subject_cfg != NULL)
		{
			m_param.type_id = subject_cfg->type;
			m_param.open_day = rand_open_day;
		}
		this->SetRoleDataChange();
		this->SendInfo();
	}
}

void RoleActivityNiuXingDaYun::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_NIU_XING_DA_YUN))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_NIU_XING_DA_YUN);

	switch (raor->opera_type)
	{
	case 0:
		{
			this->SendInfo();
		}
		break;
	case 1:
		{
			this->GetReward();
		}
		break;
	}
}

void RoleActivityNiuXingDaYun::SendInfo()
{
	Protocol::SCRandActivityNXDYInfo info;
	info.id = m_param.type_id;
	info.today_reward = m_param.today_reward;
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityNiuXingDaYun::GetReward()
{
	const RandActivityNiuXingDaYunConfig * config = static_cast<const RandActivityNiuXingDaYunConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_NIU_XING_DA_YUN));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (0 == m_param.type_id)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (0 < m_param.today_reward)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}
	const RANiuXingDaYunConfig * subject = config->GetNXDYSubjectCfg(m_param.type_id);
	if (subject == NULL)return;

	if (static_cast<short>(subject->reward_vec.size()) <= 0)return;

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList(static_cast<short>(subject->reward_vec.size()), &subject->reward_vec[0]))
	{
		//放入背包失败，改发邮件
		static MailContentParam contentparam; contentparam.Reset();
		for (int i = 0; i < MAX_ATTACHMENT_ITEM_NUM && i < static_cast<short>(subject->reward_vec.size()); i++)
		{
			const ItemBase * item_base = ITEMPOOL->GetItem(subject->reward_vec[i].item_id);

			if (item_base != NULL)
			{
				contentparam.item_list[i].item_id = subject->reward_vec[i].item_id;
				contentparam.item_list[i].num = subject->reward_vec[i].num;
				contentparam.item_list[i].is_bind = (subject->reward_vec[i].is_bind) ? 1 : 0;
				contentparam.item_list[i].invalid_time = item_base->CalInvalidTime();
			}
		}
		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_reward_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_reward_content, RAND_ACTIVITY_TYPE_NIU_XING_DA_YUN);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}
	else
	{
		//给予
		if (!m_role_module_mgr->GetKnapsack()->PutList(static_cast<short>(subject->reward_vec.size()), &subject->reward_vec[0], PUT_REASON_RA_NIU_XING_DA_YUN))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}
	}

	m_param.today_reward = 1;
	this->SendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityCiJiuYingXin::Answer user[%d, %s], level[%d], type[%d], today_reward[%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(),
		PUT_REASON_RA_NIU_XING_DA_YUN, m_param.today_reward);
}
