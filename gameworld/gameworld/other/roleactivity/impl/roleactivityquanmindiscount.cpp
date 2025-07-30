#include "roleactivityquanmindiscount.hpp"

#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityquanmindiscountconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "global/randactivity/randactivityimpl/raquanmindiscountmanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "protocol/randactivity/msgraquanmindiscount.hpp"

#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "internalcomm.h"
#include "other/rolecross/rolecross.hpp"

RoleActivityQuanMinDiscount::RoleActivityQuanMinDiscount(int type)
	:RoleActivity(type)
{

}

RoleActivityQuanMinDiscount::~RoleActivityQuanMinDiscount()
{

}

void RoleActivityQuanMinDiscount::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAQuanMinDiscountParam));
}

void RoleActivityQuanMinDiscount::GetInitParam(RoleActivityRoleData *p)
{
	memcpy(p->base_data, &m_param, sizeof(RAQuanMinDiscountParam));
}

void RoleActivityQuanMinDiscount::OnLoginSendInfo()
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		this->SendRAQuanMinDiscountInfo();
	}
	else
	{
		crossgameprotocal::HiddenGameQuanMinActivityInfoReq req;
		req.uid = m_role_module_mgr->GetRole()->GetUID();

		InternalComm::Instance().SendToGameThroughCross(
			m_role_module_mgr->GetRoleCross()->GetOriginPlatType(),
			m_role_module_mgr->GetRoleCross()->GetOriginServerId(),
			(const char*)&req, sizeof(req)
		);
	}
}

void RoleActivityQuanMinDiscount::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivityQuanMinDiscountConfig * config = static_cast<const RandActivityQuanMinDiscountConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_QUAN_MIN_DISCOUNT));
	if (NULL == config)
	{
		return;
	}

	//全民团购每日限购重置
	if (ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_QUAN_MIN_DISCOUNT))
	{
		for (int i = 0; i < QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM; ++i)
		{
			const RAQuanMinDiscountCfg * q_cfg = config->GetQuanMinDiscountCfg(ramgr, i);
			if (NULL == q_cfg) continue;

			if (ACTVITY_BUY_LIMIT_TYPE_DAY == q_cfg->buy_type)
			{
				m_param.ra_quan_min_discount_buy_count[i] = 0;
			}
		}
		if (!CrossConfig::Instance().IsHiddenServer())
		{
			this->SendRAQuanMinDiscountInfo();
		}
		else
		{
			crossgameprotocal::HiddenGameQuanMinActivityInfoReq req;
			req.uid = m_role_module_mgr->GetRole()->GetUID();

			InternalComm::Instance().SendToGameThroughCross(
				m_role_module_mgr->GetRoleCross()->GetOriginPlatType(),
				m_role_module_mgr->GetRoleCross()->GetOriginServerId(),
				(const char*)&req, sizeof(req)
			);
		}

		this->SetRoleDataChange();
	}
}

void RoleActivityQuanMinDiscount::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_QUAN_MIN_DISCOUNT != rand_act_type)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_quan_min_discount_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.ra_quan_min_discount_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

		this->SetRoleDataChange();
	}
}

void RoleActivityQuanMinDiscount::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_QUAN_MIN_DISCOUNT))
	{
		//m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_QUAN_MIN_DISCOUNT);

	switch (raor->opera_type)
	{
	case Protocol::RA_QUAN_MIN_DISCOUNT_OPERA_TYPE_GET_INFO:
		{
			if (!CrossConfig::Instance().IsHiddenServer())
			{
				this->SendRAQuanMinDiscountInfo();
			}
			else
			{
				crossgameprotocal::HiddenGameQuanMinActivityInfoReq req;
				req.uid = m_role_module_mgr->GetRole()->GetUID();

				InternalComm::Instance().SendToGameThroughCross(
					m_role_module_mgr->GetRoleCross()->GetOriginPlatType(),
					m_role_module_mgr->GetRoleCross()->GetOriginServerId(),
					(const char*)&req, sizeof(req)
				);
			}
		}
		break;
	case Protocol::RA_QUAN_MIN_DISCOUNT_OPERA_TYPE_BUY_ITEM:
		{
			this->OnRAQuanMinDiscountBuy(raor->param_1);		// param1 - seq
		}
		break;
	}
}

void RoleActivityQuanMinDiscount::SendRAQuanMinDiscountInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());

	const RandActivityQuanMinDiscountConfig * config = static_cast<const RandActivityQuanMinDiscountConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_QUAN_MIN_DISCOUNT));
	if (NULL == config)
	{
		return;
	}

	Protocol::SCRAQuanMinDiscountInfo info;

	int count = config->GetMaxSeq(ramgr);
	info.count = count;

	for (int i = 0; i < count && i < QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM; ++i)
	{
		info.commodity_info[i].discount = RAQuanMinDiscountManager::Instance().GetCommodityDiscout(i);
		info.commodity_info[i].buy_count = m_param.ra_quan_min_discount_buy_count[i];
		info.commodity_info[i].all_buy_count = RAQuanMinDiscountManager::Instance().GetBuyCount(i);
	}
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityQuanMinDiscount::OnRAQuanMinDiscountBuy(int seq)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivityQuanMinDiscountConfig * config = static_cast<const RandActivityQuanMinDiscountConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_QUAN_MIN_DISCOUNT));
	if (NULL == config)
	{
		return;
	}

	const RAQuanMinDiscountCfg * cfg = config->GetQuanMinDiscountCfg(ramgr, seq);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	//购买次数限制
	if (ACTVITY_BUY_LIMIT_TYPE_ALL == cfg->buy_type)
	{
		if (m_param.ra_quan_min_discount_buy_count[seq] >= cfg->buy_limit_all)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_BUY_NOT_ENOUGH);
			return;
		}
	}
	else if (ACTVITY_BUY_LIMIT_TYPE_DAY == cfg->buy_type)
	{
		if (m_param.ra_quan_min_discount_buy_count[seq] >= cfg->buy_limit_day)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_BUY_NOT_ENOUGH_TODAY);
			return;
		}
	}
	else
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(cfg->item.item_id, cfg->item.num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	double max_discount = config->GetMaxDiscountCfg(cfg->discount_id);
	if (max_discount < 1e-6)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	int buy_count = RAQuanMinDiscountManager::Instance().GetBuyCount(seq);
	if (-1 == buy_count)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	double buy_discount = config->GetDiscountCfg(cfg->discount_id, buy_count);

	double all_money = (double)buy_discount / 10.0 * cfg->price;
	Int64 consume_money = static_cast<long long>(round(all_money));

	//消耗
	if (MONEY_TYPE_GOLD == cfg->money_type)
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
	else if (MONEY_TYPE_COIN_BIND == cfg->money_type)
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

	//给予
	if (!m_role_module_mgr->GetKnapsack()->Put(cfg->item, PUT_REASON_RA_QUAN_MIN_DISCOUNT))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	m_param.ra_quan_min_discount_buy_count[seq]++;

	bool is_max_discount = fabs(max_discount - buy_discount) < 1e-6;
	RAQuanMinDiscountManager::Instance().OnBuyCommodity(seq, m_role_module_mgr->GetUid(), is_max_discount);

	this->SendRAQuanMinDiscountInfo();

	this->SetRoleDataChange();

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnRAQuanMinDiscountBuy user[%d, %s], level[%d], type[%d], seq[%d] consume_type[%d] consume_num[%lld] buy_item[id:%d num:%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(),
		RAND_ACTIVITY_TYPE_QUAN_MIN_DISCOUNT, seq, cfg->money_type, consume_money, cfg->item.item_id, cfg->item.num);

	this->OnParticipateRA(__FUNCTION__);
}
