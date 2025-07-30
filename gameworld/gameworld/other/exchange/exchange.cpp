#include "exchange.hpp"
#include "exchangeconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/rolemodulemanager.hpp"
#include "servercommon/errornum.h"
#include "item/money.h"
#include "item/knapsack.h"
#include "gamelog.h"
#include "obj/character/role.h"
#include "protocol/msgshop.h"

Exchange::Exchange()
{
}

Exchange::~Exchange()
{
}

void Exchange::Init(RoleModuleManager * role_module_manager, const ExchangeParam & param)
{
	m_role_module_mgr = role_module_manager;
	
	for (int i = 0; i < LIMIT_DATA_NUM; i++)
	{
		const ExchangeLimitData& data = param.limit_list[i];
		if (data.index <= 0) break;
		const ExchangeItemCfg *item_cfg = LOGIC_CONFIG->GetExchangeConfig()->GetItemCfg(data.index);
		if (NULL == item_cfg) continue;

		m_limit_buy_count[item_cfg->limit_type][item_cfg->seq] = data;
	}

}

void Exchange::GetOtherInitParam(ExchangeParam * param)
{
	int count = 0;
	param->ClearLimitData();
	for (int i = 0; i < SHOP_BUY_LIMIT_TYPE_MAX; ++i)
	{
		for (std::map<int, ExchangeLimitData>::const_iterator it = m_limit_buy_count[i].begin(); it != m_limit_buy_count[i].end(); ++it)
		{
			if (it->second.buy_count <= 0) continue;

			param->limit_list[count] = it->second;
			if (++count >= LIMIT_DATA_NUM) return;
		}
	}
}

void Exchange::OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid)
{
	m_limit_buy_count[SHOP_BUY_LIMIT_TYPE_DAILY].clear();
}

void Exchange::OnResetModuleDataWeek()
{
	m_limit_buy_count[SHOP_BUY_LIMIT_TYPE_WEEKLY].clear();
}

void Exchange::OnExchange(int seq, int num)
{
	const ExchangeItemCfg *item_cfg = LOGIC_CONFIG->GetExchangeConfig()->GetItemCfg(seq);
	if (NULL == item_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	// 判断次数
	ExchangeLimitData* data = NULL;
	if (item_cfg->limit_type > 0)
	{
		data = &m_limit_buy_count[item_cfg->limit_type][item_cfg->seq];
		if (data->buy_count + num > item_cfg->limit_times)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_SHOP_BUY_LIMIT);
			return;
		}
	}

	// 判断消耗
	Int64 need_price = item_cfg->score_price * num;
	if (item_cfg->score_price >= 0)
	{
		if (!this->IsEnough(item_cfg->score_type, need_price))return;
	}

	//////////////////////////////给予&消耗 在这之后不做return///////////////////////////////////

	if (!this->ConsumeScore(item_cfg->score_type, need_price))
	{
		gamelog::g_log_exchange.printf(LL_ERROR, "%s ConsumeScore Error user[%d %s] tpye[%d] price[%d]", __FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName()
									   , item_cfg->limit_type, item_cfg->score_price);
	}

	ItemConfigData item;
	item.item_id = item_cfg->item_id;
	item.is_bind = (0 != item_cfg->is_bind);
	item.num = num;
	m_role_module_mgr->GetKnapsack()->Put(item, PUT_REASON_EXCHANGE);
	
	if (NULL != data)
	{
		data->buy_count += num;
	}

	if (item_cfg->limit_type > 0)
	{
		this->SendSingelInfo(item_cfg->limit_type, seq);
	}
}

void Exchange::SendAllInfo()
{
	Protocol::SCExchangeAllInfo info;
	info.count = 0;
	for (int i = 0; i < SHOP_BUY_LIMIT_TYPE_MAX; ++i)
	{
		for (std::map<int, ExchangeLimitData>::const_iterator it = m_limit_buy_count[i].begin(); it != m_limit_buy_count[i].end(); ++it)
		{
			if (it->second.buy_count <= 0) continue;

			info.limit_list[info.count].seq = it->second.index;
			info.limit_list[info.count].buy_count = it->second.buy_count;

			if (++info.count >= LIMIT_DATA_NUM) return;
		}
	}

	int len = sizeof(info) - sizeof(info.limit_list) + (info.count * sizeof(Protocol::SCExchangeAllInfo::LimitData));
	m_role_module_mgr->NetSend(&info, len);
}

void Exchange::SendSingelInfo(int limit_type, int seq)
{
	ExchangeLimitData& data = m_limit_buy_count[limit_type][seq];

	Protocol::SCExchangeSingleInfo info;
	info.seq = data.index;
	info.buy_count = data.buy_count;
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

bool Exchange::IsEnough(int score_type, int score_num)
{
	switch (score_type)
	{
	case SCORE_TYPE_CHIVALROUS:
		{
			if (!m_role_module_mgr->GetMoney()->OtherCoinMoreThan(score_num, MONEY_TYPE_CHIVALROUS))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_CHIVALROUS_NOT_ENOUGH);
				return false;
			}
			else
			{
				return true;
			}
		}
		break;
	}

	return false;
}

bool Exchange::ConsumeScore(int score_type, Int64 score_num)
{
	switch (score_type)
	{
	case SCORE_TYPE_CHIVALROUS:
		{
			return m_role_module_mgr->GetMoney()->UseOtherCoin(score_num, MONEY_TYPE_CHIVALROUS, __FUNCTION__);
		}
		break;
	}
	return false;
}
