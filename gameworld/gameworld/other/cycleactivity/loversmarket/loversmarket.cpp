#include "loversmarket.hpp"
#include "loversmarketconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/rolemodulemanager.hpp"
#include "servercommon/errornum.h"
#include "item/knapsack.h"
#include "protocol/cycleactivity/msgqingyuanduiduipeng.hpp"
#include "gamelog.h"
#include "obj/character/role.h"
#include "global/zhouqimanager/zhouqimanager.h"

LoversMarket::LoversMarket()
{
}

LoversMarket::~LoversMarket()
{
}

void LoversMarket::Init(RoleModuleManager * mgr, const LoversMarketParam & param)
{
	m_mgr = mgr;
	m_param = param;
}

void LoversMarket::GetInitParam(LoversMarketParam * param)
{
	*param = m_param;
}

void LoversMarket::OnClientReq(void * msg)
{
	if (NULL == msg) return;

	Protocol::CSLoversMarketReq * req = (Protocol::CSLoversMarketReq *)msg;

	switch (req->type)
	{
	case 0:				//请求信息
		{
			this->SendInfo();
		}
		break;
	case 1:				//购买商品
		{
			this->OnBuyStore(req->param);
		}
		break;
	}
}

void LoversMarket::OnDayChange()
{
	int zhouqi_type = ZhouQiManager::Instance().GetZhouQiNum();
	if (zhouqi_type != 1)
	{
		m_param.Reset();
	}
}

void LoversMarket::SendInfo()
{
	Protocol::SCLoversMarketInfo info;
	memcpy(info.store_list, m_param.store_list, sizeof(info.store_list));

	m_mgr->NetSend(&info, sizeof(info));
}

void LoversMarket::OnBuyStore(int seq)
{
	if (seq < 0 || seq >= MAX_LOVERS_MARKET_STORE_COUNT)
		return;

	const LoversMarketStoreCfg * cfg = LOGIC_CONFIG->GetLoversMarketConfig()->GetStoreCfg(seq);
	if (NULL == cfg)
		return;

	int count = m_mgr->GetKnapsack()->Count(LOGIC_CONFIG->GetLoversMarketConfig()->GetOtherCfg().play_coin);
	if (count < cfg->price)
	{
		m_mgr->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
		return;
	}

	if (cfg->limit_convert_count > 0)
	{
		if (m_param.store_list[seq] >= cfg->limit_convert_count)
		{
			m_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_BUY_NOT_ENOUGH);
			return;
		}
	}

	if (!m_mgr->GetKnapsack()->CheckForPut(cfg->item_id, cfg->num, false))
	{
		m_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	if (m_mgr->GetKnapsack()->ConsumeItem(LOGIC_CONFIG->GetLoversMarketConfig()->GetOtherCfg().play_coin, cfg->price, __FUNCTION__))
	{
		ItemConfigData item(cfg->item_id, cfg->is_bind, cfg->num);
		m_mgr->GetKnapsack()->Put(item, PUT_REASON_LOVERS_MARKET);

		if (cfg->limit_convert_count > 0)
		{
			m_param.store_list[seq] += 1;
			this->SendInfo();
		}
		//log
		gamelog::g_log_cycleactivity.printf(LL_INFO, "%s user[%d, %s], buy_index[%d]", __FUNCTION__,
			m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), seq);
	}
}
