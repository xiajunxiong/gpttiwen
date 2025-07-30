#include "randactivityduihuanlingyuconfig.hpp"
#include "servercommon/struct/moneyotherdef.h"
#include "servercommon/roleconvertshopparam.hpp"
#include "obj/character/role.h"
#include "servercommon/errornum.h"
#include "item/knapsack.h"
#include "other/rolemodulemanager.hpp"
#include "other/partner/partner.hpp"

#include "gamelog.h"

RandActivityDuiHuanLingYuConfig::RandActivityDuiHuanLingYuConfig()
{
}

RandActivityDuiHuanLingYuConfig::~RandActivityDuiHuanLingYuConfig()
{
}

bool RandActivityDuiHuanLingYuConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("gift_configure", InitCfg);

	return true;
}

const RADuiHuanLingYuCfg * RandActivityDuiHuanLingYuConfig::GetMarketItemCfg(int index)const
{
	std::map<int, RADuiHuanLingYuCfg>::const_iterator it = m_market_item_map.find(index);
	if (it != m_market_item_map.end()) return &it->second;

	return NULL;
}

void RandActivityDuiHuanLingYuConfig::OnSaleItem(Role * role, int index)const
{
	if (NULL == role) return;
	const RADuiHuanLingYuCfg *cfg = this->GetMarketItemCfg(index);
	if (NULL == cfg)
	{
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	const ItemBase * cost_item_base = ITEMPOOL->GetItem(cfg->item.item_id);
	if (NULL == cost_item_base)
	{
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	switch (cost_item_base->GetItemType())
	{
	case I_TYPE_PARTNER_PACT:
		{
			if (!role->GetRoleModuleManager()->GetPartner()->PactGridTakeOut(cfg->item.item_id, cfg->item.num, PUT_REASON_PARTNER_PACT_GET))
			{
				role->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
				return;
			}
		}
		break;
	default:
		if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(cfg->item.item_id, cfg->item.num, __FUNCTION__))
		{
			role->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
			return;
		}
		break;
	}
	switch (cfg->price_type)
	{
	case MONEY_TYPE_GOLD:
		{
			role->GetRoleModuleManager()->GetMoney()->AddGold(cfg->price, __FUNCTION__);
		}
		break;
	case MONEY_TYPE_IMMORTAL_COIN:
		{
			role->GetRoleModuleManager()->GetMoney()->AddImmortalCoin(cfg->price, __FUNCTION__);
		}
		break;
	case MONEY_TYPE_COIN_BIND:
		{
			role->GetRoleModuleManager()->GetMoney()->AddCoinBind(cfg->price, __FUNCTION__);
		}
		break;
	default:
		role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(cfg->price, cfg->price_type, __FUNCTION__);
		break;
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s succ ! role[%d, %s] index[%d]", __FUNCTION__, role->GetUID(), role->GetName(), index);
}

int RandActivityDuiHuanLingYuConfig::InitCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -888;

	while (NULL != dataElement)
	{
		int index = 0;
		if (!GetSubNodeValue(dataElement, "index", index) || m_market_item_map.end() != m_market_item_map.find(index))
		{
			return -1;
		}
		RADuiHuanLingYuCfg & cfg = m_market_item_map[index];

		if (!GetSubNodeValue(dataElement, "item_id", cfg.item.item_id) || NULL == ITEMPOOL->GetItem(cfg.item.item_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "num", cfg.item.num))
		{
			return -3;
		}
		if (!GetSubNodeValue(dataElement, "price_type", cfg.price_type) || cfg.price_type < 0 || cfg.price_type >= MONEY_TYPE_MAX)
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "price", cfg.price) || cfg.price <= 0)
		{
			return -5;
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}
