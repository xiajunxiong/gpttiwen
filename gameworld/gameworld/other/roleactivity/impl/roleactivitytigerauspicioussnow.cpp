#include "roleactivitytigerauspicioussnow.hpp"
#include "config/randactivityconfig/impl/randactivitytigerauspicioussnowconfig.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "global/randactivity/randactivityimpl/randactivitytigerauspicioussnow.hpp"

#include "servercommon/errornum.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "gamelog.h"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "gameworld/item/knapsack.h"


RoleActivityTigerAuspiciousSnow::RoleActivityTigerAuspiciousSnow(int type)
	:RoleActivity(type)
{
}

RoleActivityTigerAuspiciousSnow::~RoleActivityTigerAuspiciousSnow()
{
}

void RoleActivityTigerAuspiciousSnow::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityTigerAuspiciousSnow::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityTigerAuspiciousSnow::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_TIGER_AUSPICIOUS_SNOW != rand_act_type)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.ra_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());


		this->RASendInfo();
		this->SetRoleDataChange();
	}


}

void RoleActivityTigerAuspiciousSnow::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TIGER_AUSPICIOUS_SNOW))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_TIGER_AUSPICIOUS_SNOW);

	switch (raor->opera_type)
	{
	case 0:
		{
			this->RASendInfo();					// 发协议
		}
		break;
	case 1:
		{
			this->SnowBallLottery();			// 雪球奖励
		}
		break;
	case 2:
		{
			this->Exchange();					// 集字奖励
		}
		break;
	default:
		break;
	}

}

void RoleActivityTigerAuspiciousSnow::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TIGER_AUSPICIOUS_SNOW))
	{
		return;
	}

	m_param.DayReset();
	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityTigerAuspiciousSnow::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TIGER_AUSPICIOUS_SNOW))
	{
		return;
	}

	this->RASendInfo();
}

void RoleActivityTigerAuspiciousSnow::RATigerSpSnowAddTimes()
{
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_TIGER_AUSPICIOUS_SNOW);

	m_param.times++;

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityTigerAuspiciousSnow::RASendInfo()
{
	Protocol::SCRATigerSpiciousSnowInfo msg;
	msg.times = m_param.times;
	msg.seq = -1;

	m_role_module_mgr->NetSend(&msg,sizeof(msg));
}

void RoleActivityTigerAuspiciousSnow::SnowBallLottery()
{
	const RandActivityTigerAuspiciousSnowConfig * config = static_cast<const RandActivityTigerAuspiciousSnowConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_TIGER_AUSPICIOUS_SNOW));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RATigerSpSnowOtherCfg & other_cfg = config->GetOtherCfg();

	if (m_role_module_mgr->GetKnapsack()->Count(other_cfg.snowball_id) == 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	int seq = -1;
	const ItemConfigData * reward_cfg = config->GetRandRewardCfg(ramgr, seq);
	if (NULL == reward_cfg || seq == -1) return;

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(other_cfg.snowball_id, 1, __FUNCTION__))
	{
		return;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPut(reward_cfg->item_id,reward_cfg->num,false))
	{
		m_role_module_mgr->GetKnapsack()->Put(*reward_cfg, PUT_REASON_RA_TIGER_AUSPICIOUS_SNOW);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(reward_cfg,1, SEND_MAIL_TYPE_TIGER_AUSPICIOUS_SNOW);
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityTigerAuspiciousSnow::%s role[%d, %s] consume_item[%d-%d] get_reward[%d-%d]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), other_cfg.snowball_id, 1, reward_cfg->item_id, reward_cfg->num);

	Protocol::SCRATigerSpiciousSnowInfo msg;
	msg.times = m_param.times;
	msg.seq = seq;

	m_role_module_mgr->NetSend(&msg, sizeof(msg));
	this->SetRoleDataChange();
}

void RoleActivityTigerAuspiciousSnow::Exchange()
{
	const RandActivityTigerAuspiciousSnowConfig * config = static_cast<const RandActivityTigerAuspiciousSnowConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_TIGER_AUSPICIOUS_SNOW));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RATigerSpSnowOtherCfg & other_cfg = config->GetOtherCfg();

	bool is_all_collect = true;

	if (m_role_module_mgr->GetKnapsack()->Count(other_cfg.consume_item_id_1) == 0) is_all_collect = false;
	if (m_role_module_mgr->GetKnapsack()->Count(other_cfg.consume_item_id_2) == 0) is_all_collect = false;
	if (m_role_module_mgr->GetKnapsack()->Count(other_cfg.consume_item_id_3) == 0) is_all_collect = false;
	if (m_role_module_mgr->GetKnapsack()->Count(other_cfg.consume_item_id_4) == 0) is_all_collect = false;

	if (!is_all_collect)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RATigerSpSonwRechargeSectionCfg * recharge_cfg = config->GetRechargeCfg(ramgr);
	if (NULL == recharge_cfg) return;

	if (!this->ConsumeAllItem())
	{
		return;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPutList((short)recharge_cfg->reward.size(), &recharge_cfg->reward[0], false))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)recharge_cfg->reward.size(), &recharge_cfg->reward[0], PUT_REASON_RA_TIGER_AUSPICIOUS_SNOW);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&recharge_cfg->reward[0], (int)recharge_cfg->reward.size(), SEND_MAIL_TYPE_TIGER_AUSPICIOUS_SNOW);
	}

	std::string gamelog_str;
	for (int i = 0; i < (int)recharge_cfg->reward.size(); ++i)
	{
		gamelog_str += STRING_SPRINTF("{item[%d, %d]} ", recharge_cfg->reward[i].item_id, recharge_cfg->reward[i].num);
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityTigerAuspiciousSnow::%s role[%d, %s] get_reward[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), gamelog_str.c_str());
}

bool RoleActivityTigerAuspiciousSnow::ConsumeAllItem()
{
	const RandActivityTigerAuspiciousSnowConfig * config = static_cast<const RandActivityTigerAuspiciousSnowConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_TIGER_AUSPICIOUS_SNOW));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return false;
	}

	const RATigerSpSnowOtherCfg & other_cfg = config->GetOtherCfg();
	std::vector<ItemID> consume_list;

	consume_list.push_back(other_cfg.consume_item_id_1);
	consume_list.push_back(other_cfg.consume_item_id_2);
	consume_list.push_back(other_cfg.consume_item_id_3);
	consume_list.push_back(other_cfg.consume_item_id_4);

	int success_consume = 0;
	for (; success_consume < (int)consume_list.size(); ++success_consume)
	{
		if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(consume_list[success_consume], other_cfg.consume_num, __FUNCTION__))
		{
			break;
		}
	}

	if (success_consume == 4)
	{
		std::string gamelog_str;
		for (int i = 0; i < (int)consume_list.size(); ++i)
		{
			gamelog_str += STRING_SPRINTF("{item[%d, %d]} ",consume_list[i],other_cfg.consume_num );
		}

		gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityTigerAuspiciousSnow::%s role[%d, %s] consume_item[%s]", __FUNCTION__,
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), gamelog_str.c_str());

		return true;
	}
	else
	{
		for (int i = 0; i < success_consume; ++i)
		{
			if (m_role_module_mgr->GetKnapsack()->CheckForPut(consume_list[i], other_cfg.consume_num, false))
			{
				m_role_module_mgr->GetKnapsack()->Put(ItemConfigData(consume_list[i],true,other_cfg.consume_num), PUT_REASON_RA_TIGER_AUSPICIOUS_SNOW, NULL, NULL, 0, 0, false);
			}
		}
	}

	return false;
}
