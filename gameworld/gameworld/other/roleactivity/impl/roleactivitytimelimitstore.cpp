#include "roleactivitytimelimitstore.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitytimelimitstoreconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

#include "protocol/randactivity/msgratimelimitstore.hpp"

#include "servercommon/errornum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"

RoleActivityTimeLimitStore::RoleActivityTimeLimitStore(int rand_type)
	: RoleActivity(rand_type)
{

}

RoleActivityTimeLimitStore::~RoleActivityTimeLimitStore()
{

}

void RoleActivityTimeLimitStore::Init(RoleModuleManager * mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RATimeLimitStoreParam));
}

void RoleActivityTimeLimitStore::GetInitParam(RoleActivityRoleData *p)
{
	memcpy( p->base_data, &m_param, sizeof(RATimeLimitStoreParam));
}

void RoleActivityTimeLimitStore::OnLoginSendInfo()
{
	this->SendRATimeLimitStoreInfo();
}

void RoleActivityTimeLimitStore::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_TIME_LIMIT_STORE != rand_act_type)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_time_limit_store_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.ra_time_limit_store_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		this->SetRoleDataChange();
	}
}

void RoleActivityTimeLimitStore::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(raor->rand_activity_type))
	{
		//m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return;
	}
	this->CheckInitRandActivity(raor->rand_activity_type);

	switch (raor->opera_type)
	{
	case Protocol::RA_TIME_LIMIT_STORE_OPERA_TYPE_GET_INFO:
		{
			this->SendRATimeLimitStoreInfo();
		}
		break;
	case Protocol::RA_TIME_LIMIT_STORE_OPERA_TYPE_BUY:
		{
			this->OnRATimeLimitStoreBuy(raor->param_1, raor->param_2);
		}
		break;
	}
}

void RoleActivityTimeLimitStore::SendRATimeLimitStoreInfo()
{
	Protocol::SCRandActivityTimeLimitStoreInfo info;

	UNSTD_STATIC_CHECK(sizeof(info.ra_time_limit_store_buy_count) == sizeof(m_param.ra_time_limit_store_buy_count))
	memcpy(info.ra_time_limit_store_buy_count, m_param.ra_time_limit_store_buy_count, sizeof(info.ra_time_limit_store_buy_count));

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityTimeLimitStore::OnRATimeLimitStoreBuy(int seq, int count)
{
	if (seq < 0 || seq >= RA_TIME_LIMIT_STORE_MAX_NUM)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityTimeLimitStoreConfig * config = static_cast<const RandActivityTimeLimitStoreConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_TIME_LIMIT_STORE));
	if (NULL == config)
	{
		return;
	}

	const TimeLimitStoreCfg *cfg = config->GetTimeLimitStoreItemCfg(ramgr, seq);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (cfg->limit_times > 0)
	{
		if (m_param.ra_time_limit_store_buy_count[seq] + count > cfg->limit_times)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_WST_NO_BUY_EXP_COUNT);
			return;
		}
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(cfg->item.item_id, cfg->item.num*count))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_HAS_NO_SPACING);
		return;
	}

	int price = cfg->price * count;
	switch (cfg->money_type)
	{
	case RA_TIME_LIMIT_STORE_MONEY_TYPE_GOLD:
		{
			if (!m_role_module_mgr->GetMoney()->GoldMoreThan(price))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
				return;
			}
		}
		break;
	case RA_TIME_LIMIT_STORE_MONEY_TYPE_IMMORTAL_COIN:
		{
			if (!m_role_module_mgr->GetMoney()->ImmortalCoinMoreThan(price))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_IMMORTAL_COIN_NOT_ENOUGH);
				return;
			}
		}
		break;
	case RA_TIME_LIMIT_STORE_MONEY_TYPE_COIN:
		{
			if (!m_role_module_mgr->GetMoney()->AllCoinMoreThan(price))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
				return;
			}
		}
		break;
	}

	///////////////////////////////判断完毕/////////////////////////////////////

	switch (cfg->money_type)
	{
	case RA_TIME_LIMIT_STORE_MONEY_TYPE_GOLD:
		m_role_module_mgr->GetMoney()->UseGold(price, __FUNCTION__);
		break;
	case RA_TIME_LIMIT_STORE_MONEY_TYPE_IMMORTAL_COIN:
		m_role_module_mgr->GetMoney()->UseImmortalCoin(price, __FUNCTION__);
		break;
	case RA_TIME_LIMIT_STORE_MONEY_TYPE_COIN:
		m_role_module_mgr->GetMoney()->UseAllCoin(price, __FUNCTION__);
		break;
	}
	ItemConfigData item;
	item.item_id = cfg->item.item_id;
	item.is_bind = cfg->item.is_bind;
	item.num = cfg->item.num*count;

	if (!m_role_module_mgr->GetKnapsack()->Put(item, PUT_REASON_RA_TIME_LIMIT_STORE))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	m_param.ra_time_limit_store_buy_count[seq] += count;
	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityTimeLimitStore::OnRATimeLimitStoreBuy user[%d, %s], level[%d], type[%d] item[id:%d num:%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_TIME_LIMIT_STORE, item.item_id, item.num);

	this->SendRATimeLimitStoreInfo();

	this->SetRoleDataChange();

	this->OnParticipateRA(__FUNCTION__);
}

