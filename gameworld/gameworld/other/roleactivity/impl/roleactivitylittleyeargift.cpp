#include "roleactivitylittleyeargift.hpp"

#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitylittleyeargiftconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "npc/npcmanager.h"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"

RoleActivityLittleYearGift::RoleActivityLittleYearGift(int type)
	:RoleActivity(type)
{

}

RoleActivityLittleYearGift::~RoleActivityLittleYearGift()
{

}

void RoleActivityLittleYearGift::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RALittleYearGiftParam));
}

void RoleActivityLittleYearGift::GetInitParam(RoleActivityRoleData *p)
{
	memcpy(p->base_data, &m_param, sizeof(RALittleYearGiftParam));
}

void RoleActivityLittleYearGift::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_LITTLE_YEAR_GIFT))
	{
		return;
	}

	int rand_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_LITTLE_YEAR_GIFT);

	if (m_param.open_day == rand_open_day)
	{
		//return;//屏蔽让GM命令能直接刷新题目
	}
	m_param.DayReset();
	const RandActivityLittleYearGiftConfig * config = static_cast<const RandActivityLittleYearGiftConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_LITTLE_YEAR_GIFT));
	if (NULL != config)
	{
		const RALittleYearGiftThreadBankCfg * cfg = config->RandThreadCfg();
		if (NULL != cfg)
		{
			m_param.thread_id = cfg->thread_id;
		}
	}
	m_param.open_day = rand_open_day;
	this->SendInfo();
	this->SetRoleDataChange();
}

void RoleActivityLittleYearGift::OnLoginSendInfo()
{
	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_LITTLE_YEAR_GIFT))
	{
		return;
	}

	this->SendInfo();
}

void RoleActivityLittleYearGift::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_LITTLE_YEAR_GIFT != rand_act_type)
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
		int rand_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_LITTLE_YEAR_GIFT);
		const RandActivityLittleYearGiftConfig * config = static_cast<const RandActivityLittleYearGiftConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_LITTLE_YEAR_GIFT));
		if (NULL == config)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}
		m_param.open_day = rand_open_day;

		const RALittleYearGiftThreadBankCfg * cfg = config->RandThreadCfg();
		if (NULL != cfg)
		{
			m_param.thread_id = cfg->thread_id;
		}
		this->SendInfo();
		this->SetRoleDataChange();
	}
}

void RoleActivityLittleYearGift::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_LITTLE_YEAR_GIFT))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_LITTLE_YEAR_GIFT);

	switch (raor->opera_type)//0,下发，1，对话
	{
	case 0:
		{
			this->SendInfo();
		}
		break;
	case 1://和NPC对话 param1 = npc_id
		{
			this->TalkToNpc(raor->param_1);
		}
		break;
	}
}

void RoleActivityLittleYearGift::SendInfo()
{
	Protocol::SCRALittleYearGiftInfo info;
	
	info.thread_id = m_param.thread_id;
	info.has_get_reward = m_param.has_get_reward;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityLittleYearGift::TalkToNpc(int npc_id)
{
	const RandActivityLittleYearGiftConfig * config = static_cast<const RandActivityLittleYearGiftConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_LITTLE_YEAR_GIFT));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (0 == m_param.open_day)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RALittleYearGiftCfg *check_item_cfg = config->GetLittleYearGiftCfgByActivityDay(m_param.open_day, m_param.thread_id);
	if (NULL == check_item_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (npc_id != check_item_cfg->npc_id)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_LITTLE_YEAR_GIFT_FAIL_NPC);
		return;
	}

	if (m_param.has_get_reward == 1)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_COW_HAS_REWARD_ERR);
		return;
	}

	if (static_cast<short>(check_item_cfg->reward_item_vec.size()) <= 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList(static_cast<short>(check_item_cfg->reward_item_vec.size()), &check_item_cfg->reward_item_vec[0]))
	{
		//放入背包失败，改发邮件
		static MailContentParam contentparam; contentparam.Reset();
		for (int i = 0; i < MAX_ATTACHMENT_ITEM_NUM && i < (short)check_item_cfg->reward_item_vec.size(); i++)
		{
			const ItemBase * item_base = ITEMPOOL->GetItem(check_item_cfg->reward_item_vec[i].item_id);

			if (item_base != NULL)
			{
				contentparam.item_list[i].item_id = check_item_cfg->reward_item_vec[i].item_id;
				contentparam.item_list[i].num = check_item_cfg->reward_item_vec[i].num;
				contentparam.item_list[i].is_bind = (check_item_cfg->reward_item_vec[i].is_bind) ? 1 : 0;
				contentparam.item_list[i].invalid_time = item_base->CalInvalidTime();
			}
		}
		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_reward_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_reward_content, RAND_ACTIVITY_TYPE_LITTLE_YEAR_GIFT);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}
	else
	{
		//给予
		if (!m_role_module_mgr->GetKnapsack()->PutList(static_cast<short>(check_item_cfg->reward_item_vec.size()), &check_item_cfg->reward_item_vec[0], PUT_REASON_RA_LITTLE_YEAR_GIFT))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		}
	}

	m_param.has_get_reward = 1;

	this->SendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityLittleYearGift::TalkToNpc user[%d, %s], level[%d], type[%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(),
		RAND_ACTIVITY_TYPE_LITTLE_YEAR_GIFT);
}