#include "medalitem.hpp"
#include "obj/character/role.h"
#include "servercommon/errornum.h"
#include "other/medal/medalconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"


MedalItem::MedalItem() : ItemBase(I_TYPE_MEDAL)
{
	m_attr_group = 0;
	m_sp_group = 0;
	m_initial_rank = 0;
}

int MedalItem::Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type /*= ITEM_PARAM_TYPE_INVAILD*/, void * param /*= NULL*/) const
{
	role->NoticeNum(errornum::EN_KNAPSACK_USE_NOT_DIRECT);

	return -1;
}

int MedalItem::GetSellPrice(NetValueItemParam* param) const
{
	const MedalMarketCfg* cfg = LOGIC_CONFIG->GetMedalConfig()->GetMedalMarketCfg(param->medal_param.grade);
	if (NULL != cfg)
	{
		return cfg->sold_price_coin;
	}

	return m_sell_price;
}

void MedalItem::RandomNetValueOnPutItem(Role * role, NetValueItemParam * param_data, int put_reason) const
{
	if (NULL == param_data)
	{
		return;
	}
	
	MedalItemParam & data = param_data->medal_param;
	if (0 != data.has_net_value)
	{
		return;
	}

	LOGIC_CONFIG->GetMedalConfig()->RandMedalParam(param_data, m_attr_group, m_sp_group, m_initial_rank);
}
