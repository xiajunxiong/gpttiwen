#include "roleactivitydiscountcarnival.hpp"

#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitydiscountcarnivalconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "protocol/randactivity/msgradiscountcarnival.hpp"

#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"


RoleActivityDiscountCarnival::RoleActivityDiscountCarnival(int type)
	:RoleActivity(type)
{

}

RoleActivityDiscountCarnival::~RoleActivityDiscountCarnival()
{

}


void RoleActivityDiscountCarnival::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RADiscountCarnivalParam));
}

void RoleActivityDiscountCarnival::GetInitParam(RoleActivityRoleData *p)
{
	memcpy( p->base_data, &m_param, sizeof(RADiscountCarnivalParam));
}

void RoleActivityDiscountCarnival::OnLoginSendInfo()
{
	this->SendRADiscountCarniavalInfo();
}

void RoleActivityDiscountCarnival::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	//折扣狂欢每日限购重置
	if (NULL != ramgr && ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_DISCOUNT_CARNIVAL))
	{
		for (int i = 0; i < DISCOUNT_CARNIVAL_COMMODITY_MAX_NUM; ++i)
		{
			const RandActivityDiscountCarnivalConfig * config = static_cast<const RandActivityDiscountCarnivalConfig *>
				(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_DISCOUNT_CARNIVAL));
			if (NULL == config)
			{
				return;
			}

			const RADiscountCarnivalCfg* d_cfg = config->GetDiscountCarnivalCfg(ramgr, i);
			if (NULL == d_cfg) continue;

			if (ACTVITY_BUY_LIMIT_TYPE_DAY == d_cfg->buy_type)
			{
				m_param.ra_discount_carnival_buy_count[i] = 0;
			}
		}
		this->SendRADiscountCarniavalInfo();

		this->SetRoleDataChange();
	}
}

void RoleActivityDiscountCarnival::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_DISCOUNT_CARNIVAL != rand_act_type)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_discount_carnival_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.ra_discount_carnival_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		m_param.ra_discount_carnival_buy_flag.Reset();
		memset(m_param.ra_discount_carnival_buy_count, 0, sizeof(m_param.ra_discount_carnival_buy_count));

		this->SetRoleDataChange();
	}
}

void RoleActivityDiscountCarnival::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_DISCOUNT_CARNIVAL))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_DISCOUNT_CARNIVAL);

	switch (raor->opera_type)
	{
	case Protocol::RA_DISCOUNT_CARNIVAL_OPERA_TYPE_INFO:
		{
			this->SendRADiscountCarniavalInfo();
		}
		break;
	case Protocol::RA_DISCOUNT_CARNIVAL_OPERA_TYPE_BUY_ITEM:
		{
			this->OnRADiscountCarniavalBuy(raor->param_1, raor->param_2);	
		}
		break;
	}
}

void RoleActivityDiscountCarnival::SendRADiscountCarniavalInfo()
{
	Protocol::SCRADiscountCarnivalInfo info;
	memcpy(info.buy_count, m_param.ra_discount_carnival_buy_count, sizeof(info.buy_count));
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityDiscountCarnival::OnRADiscountCarniavalBuy(int seq, int item_id)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityDiscountCarnivalConfig * config = static_cast<const RandActivityDiscountCarnivalConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_DISCOUNT_CARNIVAL));
	if (NULL == config)
	{
		return;
	}

	const RADiscountCarnivalCfg* cfg = config->GetDiscountCarnivalCfg(ramgr, seq);
	if (NULL == cfg) return;

	int cur_discount = 0;
	if (0 == item_id)
	{
		cur_discount = 10;
	}
	else
	{
		const int discount = config->GetCouponCfg(item_id);
		if (-1 == discount)return;

		cur_discount = discount;
	}

	//购买次数限制
	if (ACTVITY_BUY_LIMIT_TYPE_ALL == cfg->buy_type)
	{
		if (m_param.ra_discount_carnival_buy_count[seq] >= cfg->buy_limit_all)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_BUY_NOT_ENOUGH);
			return;
		}
	}
	else if (ACTVITY_BUY_LIMIT_TYPE_DAY == cfg->buy_type)
	{
		if (m_param.ra_discount_carnival_buy_count[seq] >= cfg->buy_limit_day)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_BUY_NOT_ENOUGH_TODAY);
			return;
		}
	}
	else
	{
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(cfg->item.item_id, cfg->item.num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	if (0 != item_id)
	{
		if (!m_role_module_mgr->GetKnapsack()->HasItem(item_id, 1))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
			return;
		}
	}

	double need_monry = (double)cur_discount / 10.0 * cfg->price;
	Int64 consume_money = static_cast<long long>(round(need_monry));

	//消耗
	if (MONEY_TYPE_GOLD == cfg->price_type)
	{
		//消耗元宝
		if (m_role_module_mgr->GetMoney()->GetGold() < consume_money)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
			return;
		}
		if (!m_role_module_mgr->GetMoney()->UseGold(consume_money, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
			return;
		}
	}
	else if (MONEY_TYPE_COIN_BIND == cfg->price_type)
	{
		//消耗铜币
		if (!m_role_module_mgr->GetMoney()->AllCoinMoreThan(consume_money))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return;
		}
		if (!m_role_module_mgr->GetMoney()->UseAllCoin(consume_money, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return;
		}
	}
	else
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	//消耗折扣券
	if (0 != item_id)
	{
		if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(item_id, 1, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
			return;
		}
	}

	//给予
	if (!m_role_module_mgr->GetKnapsack()->Put(cfg->item, PUT_REASON_RA_DISCOUNT_CARNIVAL))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	m_param.ra_discount_carnival_buy_flag.SetBit(seq);
	m_param.ra_discount_carnival_buy_count[seq]++;
	this->SendRADiscountCarniavalInfo();

	this->SetRoleDataChange();

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnRADiscountCarniavalBuy user[%d, %s], level[%d], type[%d], seq[%d] consume_gold[%lld] consume_item[id:%d num:%d] buy_item[id:%d num:%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(),
		RAND_ACTIVITY_TYPE_DISCOUNT_CARNIVAL, seq, consume_money, item_id, 1, cfg->item.item_id, cfg->item.num);

	this->OnParticipateRA(__FUNCTION__);
}
