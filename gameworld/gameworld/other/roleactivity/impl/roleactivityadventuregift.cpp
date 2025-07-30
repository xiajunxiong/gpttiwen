#include "roleactivityadventuregift.hpp"

#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityadventuregiftconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "protocol/randactivity/msgraadventuregift.hpp"

#include "servercommon/string/gameworldstr.h"
#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "gamelog.h"


RoleActivityAdventureGift::RoleActivityAdventureGift(int type)
	:RoleActivity(type)
{

}

RoleActivityAdventureGift::~RoleActivityAdventureGift()
{

}

void RoleActivityAdventureGift::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAAdventureGiftParam));
}

void RoleActivityAdventureGift::GetInitParam(RoleActivityRoleData *p)
{
	memcpy( p->base_data, &m_param, sizeof(RAAdventureGiftParam));
}

void RoleActivityAdventureGift::OnLoginSendInfo()
{
	this->RaAdventureGiftSendInfo();
}

void RoleActivityAdventureGift::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_ADVENTURE_GIFT != rand_act_type)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_adventure_gift_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.ra_adventure_gift_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		m_param.ra_adventure_gift_seq = 0;
		m_param.ra_adventure_gift_end_time = 0;

		this->SetRoleDataChange();
	}
}

void RoleActivityAdventureGift::OnRoleLevelUp(int old_level, int to_level)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivityAdventureGiftConfig * config = static_cast<const RandActivityAdventureGiftConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ADVENTURE_GIFT));
	if (NULL == config)
	{
		return;
	}

	if (ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_ADVENTURE_GIFT))
	{
		this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_ADVENTURE_GIFT);

		const RAAdventureGiftItemCfg * cfg = config->CheckAdventureGift(ADVENTURE_GIFT_CONDITION_TYPE_ROLE_LEVEL, old_level, to_level);
		if (cfg != NULL)
		{
			m_param.ra_adventure_gift_seq = cfg->seq;
			m_param.ra_adventure_gift_end_time = (unsigned int)(EngineAdapter::Instance().Time() + cfg->during_time);

			this->RaAdventureGiftSendInfo();

			this->SetRoleDataChange();
		}
	}
}

void RoleActivityAdventureGift::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_ADVENTURE_GIFT))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_ADVENTURE_GIFT);

	switch (raor->opera_type)
	{
	case Protocol::RA_ADVENTURE_GIFT_OPERA_TYPE_INFO:
		{
			this->RaAdventureGiftSendInfo();
		}
		break;
	case Protocol::RA_ADVENTURE_GIFT_OPERA_TYPE_BUY:
		{
			this->RaAdventureGiftBuy();
		}
		break;
	}
}

void RoleActivityAdventureGift::OnPassCourageChallenge(int old_layer, int layer)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL != ramgr && ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_ADVENTURE_GIFT))
	{
		this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_ADVENTURE_GIFT);

		const RandActivityAdventureGiftConfig * config = static_cast<const RandActivityAdventureGiftConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ADVENTURE_GIFT));
		if (NULL != config)
		{
			const RAAdventureGiftItemCfg * cfg = config->CheckAdventureGift(ADVENTURE_GIFT_CONDITION_TYPE_COURAGE_CHALLENGE, old_layer, layer);
			if (cfg != NULL)
			{
				m_param.ra_adventure_gift_seq = cfg->seq;
				m_param.ra_adventure_gift_end_time = (unsigned int)(EngineAdapter::Instance().Time() + cfg->during_time);

				this->RaAdventureGiftSendInfo();

				this->SetRoleDataChange();
			}
		}
	}
}

void RoleActivityAdventureGift::OnChangeCourseOfGrowthProg(int old_prog, int prog)
{
	if (prog >= old_prog)
	{
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL != ramgr && ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_ADVENTURE_GIFT))
	{
		this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_ADVENTURE_GIFT);

		const RandActivityAdventureGiftConfig * config = static_cast<const RandActivityAdventureGiftConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ADVENTURE_GIFT));
		if (NULL != config)
		{
			const RAAdventureGiftItemCfg * cfg = config->CheckAdventureGift(ADVENTURE_GIFT_CONDITION_TYPE_COURSE_OF_GROWTH_PROG, old_prog, prog);
			if (cfg != NULL)
			{
				m_param.ra_adventure_gift_seq = cfg->seq;
				m_param.ra_adventure_gift_end_time = (unsigned int)(EngineAdapter::Instance().Time() + cfg->during_time);

				this->RaAdventureGiftSendInfo();

				this->SetRoleDataChange();
			}
		}
	}
}

void RoleActivityAdventureGift::RaAdventureGiftSendInfo()
{
	Protocol::SCRaAdventureGiftInfo info;

	info.seq = m_param.ra_adventure_gift_seq;
	info.end_time = m_param.ra_adventure_gift_end_time;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityAdventureGift::RaAdventureGiftBuy()
{
	const RandActivityAdventureGiftConfig * config = static_cast<const RandActivityAdventureGiftConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ADVENTURE_GIFT));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAAdventureGiftItemCfg * cfg = config->GetAdventureGiftCfg(m_param.ra_adventure_gift_seq);
	if (cfg == NULL)
	{
		return;
	}

	if (EngineAdapter::Instance().Time() > m_param.ra_adventure_gift_end_time)
	{
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut2(cfg->reward))
	{
		return;
	}

	if (cfg->consume_type == 0)
	{
		if (!m_role_module_mgr->GetMoney()->UseGold(cfg->consume_value, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
			return;
		}
	}
	else
	{
		if (!m_role_module_mgr->GetMoney()->UseCoinBind(cfg->consume_value, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return;
		}
	}

	m_role_module_mgr->GetKnapsack()->Put(cfg->reward, PUT_REASON_RA_ADVENTURE_GIFT);

	m_param.ra_adventure_gift_end_time = 0;
	this->RaAdventureGiftSendInfo();

	if (cfg->is_broadcast != 0 && !CrossConfig::Instance().IsHiddenServer())
	{
		// 传闻
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_randactivity_adventure_gift_broadcast, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(), cfg->reward.item_id);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT));
		}
	}

	this->SetRoleDataChange();

	this->OnParticipateRA(__FUNCTION__);
}
