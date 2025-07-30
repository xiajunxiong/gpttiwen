#include "roleactivityluckydiscount.hpp"

#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityluckydiscountconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "protocol/randactivity/msgraluckydiscount.hpp"

#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"

RoleActivityLuckyDiscount::RoleActivityLuckyDiscount(int type)
	:RoleActivity(type)
{

}

RoleActivityLuckyDiscount::~RoleActivityLuckyDiscount()
{

}

void RoleActivityLuckyDiscount::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RALuckyDiscountParam));
}

void RoleActivityLuckyDiscount::GetInitParam(RoleActivityRoleData *p)
{
	memcpy( p->base_data, &m_param, sizeof(RALuckyDiscountParam));
}

void RoleActivityLuckyDiscount::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	memset(m_param.ra_lucky_discount_today_buy_count, 0, sizeof(m_param.ra_lucky_discount_today_buy_count));
	this->SetRoleDataChange();
	this->SendRALuckyDiscountInfo();
}

void RoleActivityLuckyDiscount::OnLoginSendInfo()
{
	this->SendRALuckyDiscountInfo();
}

void RoleActivityLuckyDiscount::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_LUCKY_DISCOUNT != rand_act_type)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_lucky_discount_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.ra_lucky_discount_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		m_param.ra_lucky_discount_discount = LUCKY_DISCOUNT_BEGIN_DISCOUNT;
		memset(m_param.ra_lucky_discount_buy_count, 0, sizeof(m_param.ra_lucky_discount_buy_count));
		memset(m_param.ra_lucky_discount_today_buy_count, 0, sizeof(m_param.ra_lucky_discount_today_buy_count));

		this->SetRoleDataChange();
	}
}

void RoleActivityLuckyDiscount::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_LUCKY_DISCOUNT))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_LUCKY_DISCOUNT);

	switch (raor->opera_type)
	{
	case Protocol::RA_LUCKY_DISCOUNT_OPERA_TYPE_INFO:
		{
			this->SendRALuckyDiscountInfo();
		}
		break;
	case Protocol::RA_LUCKY_DISCOUNT_OPERA_TYPE_GET_DISCOUNT:
		{
			this->OnRALuckyDiscountGetDiscount();
		}
		break;
	case Protocol::RA_LUCKY_DISCOUNT_OPERA_TYPE_BUY_ITEM:
		{
			this->OnRALuckyDiscountBuy(raor->param_1, raor->param_2);		// param1 - seq
		}
		break;
	}
}

void RoleActivityLuckyDiscount::SendRALuckyDiscountInfo()
{
	Protocol::SCRALuckyDiscountInfo info;
	info.this_discount = m_param.ra_lucky_discount_discount;
	memcpy(info.bug_count, m_param.ra_lucky_discount_buy_count, sizeof(info.bug_count));
	memcpy(info.today_buy_count, m_param.ra_lucky_discount_today_buy_count, sizeof(info.today_buy_count));
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityLuckyDiscount::OnRALuckyDiscountGetDiscount()
{
	if (LUCKY_DISCOUNT_BEGIN_DISCOUNT != m_param.ra_lucky_discount_discount)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_HAVE_DISCOUNT);
		return;
	}

	const RandActivityLuckyDiscountConfig * config = static_cast<const RandActivityLuckyDiscountConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_LUCKY_DISCOUNT));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	int discount = config->GetDiscountCfg();
	if (0 >= discount)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	m_param.ra_lucky_discount_discount = discount;
	this->SendRALuckyDiscountInfo();

	this->SetRoleDataChange();
}

void RoleActivityLuckyDiscount::OnRALuckyDiscountBuy(int seq, int count)
{
	if (0 >= count)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RandActivityLuckyDiscountConfig * config = static_cast<const RandActivityLuckyDiscountConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_LUCKY_DISCOUNT));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RALuckyDiscountRewardCfg * cfg = config->GetRewardCfg(seq, ramgr);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	switch (cfg->limit_type)
	{
	case RALuckyDiscountRewardCfg::LIMIT_TYPE_TOTAL:
		{
			if (count + m_param.ra_lucky_discount_buy_count[seq] > cfg->limit_count)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_BUY_PASS_ENOUGH);
				return;
			}
		}
		break;

	case RALuckyDiscountRewardCfg::LIMIT_TYPE_DAY:
		{
			if (count + m_param.ra_lucky_discount_today_buy_count[seq] > cfg->limit_count)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_BUY_PASS_ENOUGH);
				return;
			}
		}
		break;

	default:
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(cfg->item.item_id, cfg->item.num * count))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	double need_money = (double)m_param.ra_lucky_discount_discount / LUCKY_DISCOUNT_BEGIN_DISCOUNT * cfg->price;
	Int64 consume_money = static_cast<long long>(round(need_money) * count);

	//消耗
	switch (cfg->money_type)
	{
	case MONEY_TYPE_GOLD:
		{
			//消耗元宝
			if (cfg->price > 0)
			{
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
		}
		break;
	case MONEY_TYPE_COIN_BIND:
		{
			//消耗铜币
			if (cfg->price > 0)
			{
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
		}
		break;
	case MONEY_TYPE_SILVER_COIN:
		{
			//消耗灵玉
			if (cfg->price > 0)
			{
				if (!m_role_module_mgr->GetMoney()->OtherCoinMoreThan(consume_money, MONEY_TYPE_SILVER_COIN))
				{
					m_role_module_mgr->NoticeNum(errornum::EN_SILVER_COIN_NOT_ENOUGH);
					return;
				}
				if (!m_role_module_mgr->GetMoney()->UseOtherCoin(consume_money, MONEY_TYPE_SILVER_COIN, __FUNCTION__))
				{
					m_role_module_mgr->NoticeNum(errornum::EN_SILVER_COIN_NOT_ENOUGH);
					return;
				}
			}
		}
		break;
	default:
		{
			m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}
		break;
	}

	ItemConfigData node = cfg->item;
	node.num = cfg->item.num * count;

	//给予
	if (!m_role_module_mgr->GetKnapsack()->Put(node, PUT_REASON_RA_LUCKY_DISCOUNT))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	switch (cfg->limit_type)
	{
	case RALuckyDiscountRewardCfg::LIMIT_TYPE_TOTAL:
		m_param.ra_lucky_discount_buy_count[seq] += count;
		break;

	case RALuckyDiscountRewardCfg::LIMIT_TYPE_DAY:
		m_param.ra_lucky_discount_today_buy_count[seq] += count;
		break;
	}
	this->SendRALuckyDiscountInfo();

	this->SetRoleDataChange();

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnRALuckyDiscountBuy user[%d, %s], level[%d], type[%d], seq[%d] buy_count[%d] consume_type[%d] consume_num[%lld] buy_item[id:%d num:%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(),
		RAND_ACTIVITY_TYPE_LUCKY_DISCOUNT, seq, count, cfg->money_type, consume_money, cfg->item.item_id, cfg->item.num);

	this->OnParticipateRA(__FUNCTION__);
}
