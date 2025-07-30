#include "roleactivitydiscountstore.hpp"

#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitydiscountstoreconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

#include "protocol/randactivity/msgradiscountstore.hpp"

#include "servercommon/errornum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"

RoleActivityDiscountStore::RoleActivityDiscountStore(int type)
	:RoleActivity(type)
{

}

RoleActivityDiscountStore::~RoleActivityDiscountStore()
{

}

void RoleActivityDiscountStore::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RADiscountStoreParam));
}

void RoleActivityDiscountStore::GetInitParam(RoleActivityRoleData *p)
{
	memcpy( p->base_data, &m_param, sizeof(RADiscountStoreParam));
}

void RoleActivityDiscountStore::OnLoginSendInfo()
{
	this->SendRADiscountStoreInfo();
}

void RoleActivityDiscountStore::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (old_dayid == now_dayid || !ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_DISCOUNT_STORE))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_DISCOUNT_STORE);

	const RandActivityDiscountStoreConfig * config = static_cast<const RandActivityDiscountStoreConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_DISCOUNT_STORE));
	if (NULL == config)
	{
		return;
	}

	const RADiscountStoreSectionCfg * section_cfg = config->GetDiscountStoreSectionCfg(ramgr);
	if (section_cfg == NULL)
	{
		return;
	}

	for (int i = 0; i < section_cfg->count && i < RA_DISCOUNT_STORE_MAX_NUM; i++)
	{
		if (section_cfg->node_cfg[i].buy_times > 0)
		{
			m_param.ra_discount_store_buy_count[i] = 0;
		}
	}
	this->SendRADiscountStoreInfo();

	this->SetRoleDataChange();
}

void RoleActivityDiscountStore::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_DISCOUNT_STORE != rand_act_type)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_discount_store_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.ra_discount_store_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		this->SetRoleDataChange();
	}
}

void RoleActivityDiscountStore::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
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
	case Protocol::RA_DISCOUNT_STORE_OPERA_TYPE_GET_INFO:
		{
			this->SendRADiscountStoreInfo();
		}
		break;
	case Protocol::RA_DISCOUNT_STORE_OPERA_TYPE_BUY:
		{
			this->OnRADiscountStoreBuy(raor->param_1, raor->param_2);
		}
		break;
	}
}

void RoleActivityDiscountStore::SendRADiscountStoreInfo()
{
	Protocol::SCRandActivityDiscountStoreInfo info;

	UNSTD_STATIC_CHECK(sizeof(info.ra_discount_store_buy_count) == sizeof(m_param.ra_discount_store_buy_count));
	memcpy(info.ra_discount_store_buy_count, m_param.ra_discount_store_buy_count, sizeof(info.ra_discount_store_buy_count));
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityDiscountStore::OnRADiscountStoreBuy(int seq, int count)
{
	if (seq < 0 || seq >= RA_DISCOUNT_STORE_MAX_NUM)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityDiscountStoreConfig * config = static_cast<const RandActivityDiscountStoreConfig * >
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_DISCOUNT_STORE));
	if (NULL == config)
	{
		return;
	}

	const RADiscountStoreCfg *cfg = config->GetDiscountStoreItemCfg(ramgr, seq);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (cfg->buy_times > 0)
	{
		if (m_param.ra_discount_store_buy_count[seq] + count > cfg->buy_times)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_WST_NO_BUY_EXP_COUNT);
			return;
		}
	}
	else if (cfg->total_exchange > 0)
	{
		if (m_param.ra_discount_store_buy_count[seq] + count > cfg->total_exchange)
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

	int price = cfg->need_gold * count;

	if (!m_role_module_mgr->GetMoney()->GoldMoreThan(price))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return;
	}

	///////////////////////////////判断完毕/////////////////////////////////////


	if (!m_role_module_mgr->GetMoney()->UseGold(price, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return;
	}

	ItemConfigData item;
	item.item_id = cfg->item.item_id;
	item.is_bind = cfg->item.is_bind;
	item.num = cfg->item.num*count;
	if (!m_role_module_mgr->GetKnapsack()->Put(item, PUT_REASON_RA_DISCOUNT_STORE))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}
	m_param.ra_discount_store_buy_count[seq] += count;
	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnRADiscountStoreBuy user[%d, %s], level[%d], type[%d] item[id:%d num:%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_DISCOUNT_STORE, item.item_id, item.num);
	this->SendRADiscountStoreInfo();

	this->SetRoleDataChange();

	this->OnParticipateRA(__FUNCTION__);
}
