#include "roleactivityyunzehaoli.hpp"
#include "config/randactivityconfig/impl/randactivityyunzehaoliconfig.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"

#include "servercommon/errornum.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "gamelog.h"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/uniqueidgenerator.hpp"

RoleActivityYunZeHaoLi::RoleActivityYunZeHaoLi(int type)
	:RoleActivity(type)
{

}

RoleActivityYunZeHaoLi::~RoleActivityYunZeHaoLi()
{

}

void RoleActivityYunZeHaoLi::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityYunZeHaoLi::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityYunZeHaoLi::Update(time_t now_second)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUN_ZE_HAO_LI))
	{
		return;
	}

	const RandActivityYunZeHaoLiConfig * config = static_cast<const RandActivityYunZeHaoLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUN_ZE_HAO_LI));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	for (int i = 0; i < (int)config->GetOtherCfg().time.size(); ++i)
	{
		if(m_param.prop_free_timestamp == 0)
		{
			if (this->ForHourMinuteGetTimestamp(config->GetOtherCfg().time[i]) <= now_second &&
				(this->ForHourMinuteGetTimestamp(config->GetOtherCfg().time[i]) + 60) > now_second)
			{
				m_param.prop_free_num++;
				m_param.prop_free_timestamp = this->ForHourMinuteGetTimestamp(config->GetOtherCfg().time[i]) + 60;

				this->RASendInfo();
				this->SetRoleDataChange();
				break;
			}
		}
	}

	if (m_param.prop_free_timestamp != 0 && m_param.prop_free_timestamp <= now_second)
	{
		m_param.prop_free_timestamp = 0;
	}
}

void RoleActivityYunZeHaoLi::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUN_ZE_HAO_LI))
	{
		return;
	}
	this->RASendInfo();
}

void RoleActivityYunZeHaoLi::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_YUN_ZE_HAO_LI != rand_act_type)
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

		this->SetRoleDataChange();
	}
}

void RoleActivityYunZeHaoLi::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;

	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUN_ZE_HAO_LI))
	{
		return;
	}

	const RandActivityYunZeHaoLiConfig * config = static_cast<const RandActivityYunZeHaoLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUN_ZE_HAO_LI));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (config->GetOtherCfg().buy_limit == RAYZ_HAO_LI_LIMIT_BUY_TYPE_TODAY)
	{
		m_param.DayReset();

		this->RASendInfo();
		this->SetRoleDataChange();
	}
}

void RoleActivityYunZeHaoLi::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUN_ZE_HAO_LI))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_YUN_ZE_HAO_LI);

	switch (raor->opera_type)
	{
	case 0:
		{
			this->RASendInfo();			// 发协议
		}
		break;
	case 1:
		{
			this->SmashEgg(raor->param_1);		// 砸蛋，param1：0-银锤子，1-金锤子
		}
		break;
	case 2:
		{
			this->BuyGoldHammer();				// 购买金锤
		}
		break;
	default:
		break;
	}


}

void RoleActivityYunZeHaoLi::RASendInfo()
{
	Protocol::SCRAYunZeHaoLiInfo msg;
	msg.buy_times = m_param.buy_times;
	msg.prop_free_num = m_param.prop_free_num;

	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}

/**
* 输入格式：1100
* 输出时间戳
*/
time_t RoleActivityYunZeHaoLi::ForHourMinuteGetTimestamp(int hour_minute)
{
	int t_hour = hour_minute / 100;
	int t_minute = hour_minute % 100;

	if (0 > t_hour || t_hour >= 24) return 0;
	if (0 > t_minute || t_minute >= 60) return 0;

	time_t now_time = EngineAdapter::Instance().Time();
	int next_day_zeor_time = EngineAdapter::Instance().NextDayInterval(0, 0, 0);

	int cut_time = ((24 - t_hour) * 3600) - (t_minute * 60);

	time_t r_timestamp = now_time + next_day_zeor_time - cut_time;

	return r_timestamp;
}

void RoleActivityYunZeHaoLi::SmashEgg(int is_advanced_item)
{
	if (is_advanced_item != 0 && is_advanced_item != 1)
	{
		return;
	}

	if (is_advanced_item == 0 && m_param.prop_free_num <= 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_NOT_ENOUGH);
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityYunZeHaoLiConfig * config = static_cast<const RandActivityYunZeHaoLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUN_ZE_HAO_LI));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAYZHaoLiRewardCfg * reward_cfg = config->GetRandRewardCfg(ramgr, is_advanced_item == 1 ? true : false);
	if (NULL == reward_cfg) return;

	if (is_advanced_item == 1)
	{
		if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(config->GetOtherCfg().prop_added_id, 1, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
			return;
		}
	}
	else
	{
		m_param.prop_free_num--;

	}
	
	if (m_role_module_mgr->GetKnapsack()->CheckForPut(reward_cfg->reward.item_id, reward_cfg->reward.num, false))
	{
		m_role_module_mgr->GetKnapsack()->Put(reward_cfg->reward, PUT_REASON_RA_YUNZE_HAO_LI);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&reward_cfg->reward, 1, SEND_MAIL_TYPE_YUNZE_HAO_LI, true);
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityYunZeHaoLi::%s role[%d, %s] activity_type[%d] reward[%d-%d] ", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), RAND_ACTIVITY_TYPE_YUN_ZE_HAO_LI, reward_cfg->reward.item_id,reward_cfg->reward.num);

	this->RASendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityYunZeHaoLi::BuyGoldHammer()
{
	const RandActivityYunZeHaoLiConfig * config = static_cast<const RandActivityYunZeHaoLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUN_ZE_HAO_LI));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (m_param.buy_times >= config->GetOtherCfg().quantity)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_SHOP_BUY_LIMIT);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(config->GetOtherCfg().limit_item.item_id, config->GetOtherCfg().limit_item.num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	if (config->GetOtherCfg().price_type == RAYZ_HAO_LI_BUY_TYPE_GOLD)
	{
		if (!m_role_module_mgr->GetMoney()->GoldMoreThan(config->GetOtherCfg().price))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
			return;
		}

		if (!m_role_module_mgr->GetMoney()->UseGold(config->GetOtherCfg().price, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}
	}
	else if (config->GetOtherCfg().price_type == RAYZ_HAO_LI_BUY_TYPE_JADE)
	{
		if (!m_role_module_mgr->GetMoney()->OtherCoinMoreThan(config->GetOtherCfg().price, MONEY_TYPE_SILVER_COIN))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_SILVER_COIN_NOT_ENOUGH);
			return;
		}

		if (!m_role_module_mgr->GetMoney()->UseOtherCoin(config->GetOtherCfg().price, MONEY_TYPE_SILVER_COIN, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}
	}

	if (!m_role_module_mgr->GetKnapsack()->Put(config->GetOtherCfg().limit_item, PUT_REASON_RA_YUNZE_HAO_LI_BUY_GOLD_HAMMER))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityYunZeHaoLi::%s role[%d, %s] buy_times[%d]  use_money[type:%d, %d] surplus_item[%d-%d]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), m_param.buy_times, config->GetOtherCfg().price_type, config->GetOtherCfg().price,
		config->GetOtherCfg().prop_added_id, m_role_module_mgr->GetKnapsack()->Count(config->GetOtherCfg().prop_added_id));

	m_param.buy_times++;

	this->RASendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);
}
