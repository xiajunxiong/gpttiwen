#include "config/logicconfigmanager.hpp"
#include "other/rolemodulemanager.hpp"
#include "servercommon/errornum.h"
#include "item/knapsack.h"
#include "gamelog.h"
#include "obj/character/role.h"

#include "global/zhouqimanager/zhouqimanager.h"
#include "other/cycleactivity/zhouqishop/zhouqishopconfig.hpp"
#include "protocol/cycleactivity/msgzhouqishop.hpp"
#include "zhouqishop.hpp"

ZhouQiShop::ZhouQiShop()
{
}

ZhouQiShop::~ZhouQiShop()
{
}

void ZhouQiShop::Init(RoleModuleManager * mgr, const ZhouQiShopParam & param)
{
	m_mgr = mgr;
	m_param = param;
}

void ZhouQiShop::GetInitParam(ZhouQiShopParam * param)
{
	*param = m_param;
}

void ZhouQiShop::OnZhouQiInfoChange(int old_zhou_qi, unsigned int old_zhou_end_time)
{
	m_param.ResetShopInfo();
	this->SendInfo();
}

void ZhouQiShop::Buy(int seq)
{
	int zhou_qi_num = ZhouQiManager::Instance().GetZhouQiNum();

	const ZhouQiShopItemCfg * cfg = LOGIC_CONFIG->GetZhouQiShopConfig()->GetZhouQiShopItemCfg(zhou_qi_num, seq);
	if (!cfg)
	{
		return;
	}

	if (cfg->seq < 0 || cfg->seq >= ZHOU_QI_SHOP_ITEM_MAX_NUM)
	{
		return;
	}

	if (!m_mgr->GetKnapsack()->CheckForPutList2(cfg->rewards))
	{
		m_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	ZhouQiShopItemParam & item_info = m_param.item_info[cfg->seq];

	if (cfg->limit_buy_num != 0 && item_info.buy_num >= cfg->limit_buy_num)
	{
		m_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_BUY_NOT_ENOUGH);
		return;
	}

	if (!m_mgr->GetKnapsack()->ConsumeItem(cfg->consume_item_id, cfg->consume_item_num, __FUNCTION__))
	{		
		return;
	}

	m_mgr->GetKnapsack()->PutVec(cfg->rewards, PUT_REASON_ZHOU_QI_SHOP);

	if (cfg->limit_buy_num != 0)
	{
		item_info.buy_num += 1;
		this->SendInfo();
	}
	//log
	gamelog::g_log_cycleactivity.printf(LL_INFO, "%s user[%d, %s],zhou_qi_num:%d seq[%d]", __FUNCTION__,
		m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), zhou_qi_num, seq);
}

void ZhouQiShop::SendInfo()
{
	Protocol::SCZhouQiShopInfo protocol;
	
	for (int i = 0; i < ZHOU_QI_SHOP_ITEM_MAX_NUM; ++i)
	{
		protocol.store_list[i] = m_param.item_info[i].buy_num;
	}

	m_mgr->NetSend(&protocol, sizeof(protocol));
}
