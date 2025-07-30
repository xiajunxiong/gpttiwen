#include "roleactivitysaleshop.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitysaleshopconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/funopen/funopen.hpp"

#include "protocol/randactivity/msgsaleshop.hpp"

#include "servercommon/errornum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"

RoleActivitySaleShop::RoleActivitySaleShop(int type)
	:RoleActivity(type)
{

}

RoleActivitySaleShop::~RoleActivitySaleShop()
{

}

void RoleActivitySaleShop::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RASaleShopParam));
}

void RoleActivitySaleShop::GetInitParam(RoleActivityRoleData *p)
{
	memcpy(p->base_data, &m_param, sizeof(RASaleShopParam));
}

void RoleActivitySaleShop::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (old_dayid == now_dayid || !ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SALE_SHOP))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_SALE_SHOP);

	const RandActivitySaleShopConfig * config = static_cast<const RandActivitySaleShopConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SALE_SHOP));
	if (NULL == config)
	{
		return;
	}

	const RASaleShopSectionCfg * section_cfg = config->GetSaleShopSectionCfg(ramgr);
	if (NULL == section_cfg) return;

	for (int k = 0; k < SALE_SHOP_ITEM_TYPE_MAX; k++)
	{
		for (int i = 0; i < (int)section_cfg->node_cfg[k].item_list.size() && i < MAX_SALE_SHOP_TYPE_SEQ_NUM; i++)
		{
			if (section_cfg->node_cfg[k].item_list[i].limit_type == SALE_SHOP_LIMIT_TYPE_TODAY)
			{
				m_param.ra_sale_shop_buy_count[k][i] = 0;

				gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivitySaleShop::OnDayChange role[%d,%s] k[%d] i[%d] item[%d]",
					m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(), k, i, section_cfg->node_cfg[k].item_list[i].item.item_id);
			}
		}
		this->SendRASaleShopInfo(k);
	}

	this->SetRoleDataChange();
}

void RoleActivitySaleShop::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_SALE_SHOP!= rand_act_type)
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
	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_sale_shop_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.ra_sale_shop_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		memset(m_param.ra_sale_shop_buy_count, 0, sizeof(m_param.ra_sale_shop_buy_count));

		gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivitySaleShop::CheckInitRandActivity role[%d,%s] memset ra_sale_shop_buy_count",
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName());

		this->SetRoleDataChange();
	}
}

void RoleActivitySaleShop::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
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
	case Protocol::SALE_SHOP_TYPE_INFO:
		{
			this->SendRASaleShopInfo(raor->param_1);					//param1 -- item_type
		}
		break;
	case Protocol::SALE_SHOP_TYPE_BUY:
		{
			this->OnRASaleShopBuy(raor->param_1, raor->param_2, raor->param_3);		//param1 --item_type  param2 -- item_seq  param3 count
		}
		break;
	}
	return;
}

void RoleActivitySaleShop::SendRASaleShopInfo(int item_type)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivitySaleShopConfig * config = static_cast<const RandActivitySaleShopConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SALE_SHOP));
	if (NULL == config)
	{
		return;
	}

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SALE_SHOP)) return;
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_SALE_SHOP);

	Protocol::SCRASaleShopInfo info;
	info.count = config->GetSaleShopTypeItemCount(ramgr, item_type);
	if (info.count == 0) return;
	memcpy(info.sale_shop_buy_count, m_param.ra_sale_shop_buy_count[item_type], sizeof(info.sale_shop_buy_count));

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivitySaleShop::OnRASaleShopBuy(int item_type, int item_seq, int count)
{
	if (count <= 0) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());

	const RandActivitySaleShopConfig * config = static_cast<const RandActivitySaleShopConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SALE_SHOP));
	if (NULL == config)
	{
		return;
	}

	const RASaleShopCfg * cfg = config->GetSaleShopItemCfg(ramgr, item_type, item_seq);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	int max_count = 0;
	switch (cfg->limit_type)
	{
	case SALE_SHOP_LIMIT_TYPE_BUY_COUNT:
		{
			max_count = cfg->buy_times;
		}
		break;
	case SALE_SHOP_LIMIT_TYPE_TODAY:
		{
			max_count = cfg->today_times;
		}
		break;
	default:
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (count + m_param.ra_sale_shop_buy_count[item_type][item_seq] > max_count)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_TRADE_MARKET_ITEM_NOT_ENOUGH);
		return;
	}
	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(cfg->item.item_id, cfg->item.num * count))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}
	if (cfg->price > 0)
	{
		switch (cfg->price_type)
		{
		case MONEY_TYPE_COIN_BIND:
		{
			if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(cfg->price * count))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
				return;
			}
			m_role_module_mgr->GetMoney()->UseCoinBind(cfg->price * count, __FUNCTION__);
		}
		break;
		case MONEY_TYPE_GOLD:
		{
			if (!m_role_module_mgr->GetMoney()->GoldMoreThan(cfg->price * count))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
				return;
			}
			m_role_module_mgr->GetMoney()->UseGold(cfg->price * count, __FUNCTION__);
		}
		break;
		}
	}
	ItemConfigData buy_item = cfg->item;
	buy_item.num *= count;

	if (!m_role_module_mgr->GetKnapsack()->Put(buy_item, PUT_REASON_SALE_SHOP_BUY))
	{
		//失败返还货币  上面已经检查了背包,理应不会出现这种情况(加个保险)
		if (cfg->price > 0)
		{
			switch (cfg->price_type)
			{
			case MONEY_TYPE_COIN_BIND:
			{
				m_role_module_mgr->GetMoney()->AddCoinBind(cfg->price * count, __FUNCTION__);
			}
			break;
			case MONEY_TYPE_GOLD:
			{
				m_role_module_mgr->GetMoney()->AddGold(cfg->price * count, __FUNCTION__);
			}
			break;
			}
		}
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}
	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnRASaleShopBuy user[%d, %s], level[%d], type[%d] item[id:%d num:%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_SALE_SHOP, buy_item.item_id, buy_item.num);

	m_param.ra_sale_shop_buy_count[item_type][item_seq] += count;

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivitySaleShop::OnRASaleShopBuy role[%d,%s] item_type[%d] item_seq[%d] add_count[%d] buy_count[%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), item_type, item_seq, count, m_param.ra_sale_shop_buy_count[item_type][item_seq]);

	this->SendRASaleShopInfo(item_type);

	this->SetRoleDataChange();

	this->OnParticipateRA(__FUNCTION__);
}
