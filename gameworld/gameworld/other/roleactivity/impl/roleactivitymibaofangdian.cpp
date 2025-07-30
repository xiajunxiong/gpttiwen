#include "roleactivitymibaofangdian.hpp"
#include "gamelog.h"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "item/money.h"
#include "item/knapsack.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "protocol/randactivity/msgranationalday.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitymibaofangdianconfig.hpp"


RoleActivityMiBaoFangDian::RoleActivityMiBaoFangDian(int type)
	:RoleActivity(type)
{
}

RoleActivityMiBaoFangDian::~RoleActivityMiBaoFangDian()
{
}

void RoleActivityMiBaoFangDian::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAMiBaoFangDianParam));
}

void RoleActivityMiBaoFangDian::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAMiBaoFangDianParam));
}

void RoleActivityMiBaoFangDian::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_MI_BAO_FANG_DIAN))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_MI_BAO_FANG_DIAN);
	this->CheckPhase();
	this->SendInfo(true);
}

void RoleActivityMiBaoFangDian::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_MI_BAO_FANG_DIAN != rand_act_type)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		const RandActivityMiBaoFangDianConfig * config = static_cast<const RandActivityMiBaoFangDianConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_MI_BAO_FANG_DIAN));
		if (NULL == config)
		{
			return;
		}
		m_param.Reset();
		m_param.ra_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		this->CheckPhase();
		this->SetRoleDataChange();
	}
}

void RoleActivityMiBaoFangDian::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;
	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_MI_BAO_FANG_DIAN))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_MI_BAO_FANG_DIAN);
	m_param.is_open_today_flag = 0;
	memset(m_param.today_buy_times, 0, sizeof(m_param.today_buy_times));
	this->CheckPhase();
	this->SetRoleDataChange();
	this->SendInfo(true);
}

void RoleActivityMiBaoFangDian::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_MI_BAO_FANG_DIAN))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_MI_BAO_FANG_DIAN);
	switch (raor->opera_type)
	{
	case Protocol::RA_MI_BAO_FANG_DIAN_REQ_TYPE_INFO:
	{
		this->SendInfo();
	}
	break;
	case Protocol::RA_MI_BAO_FANG_DIAN_REQ_TYPE_BUY_ITEM:
	{
		this->BuyItem(raor->param_1);
	}
	break;
	case Protocol::RA_MI_BAO_FANG_DIAN_REQ_TYPE_OPEN_ACTIVITY:
	{
		this->SetOpenFlag();
	}
	break;
	}
}

void RoleActivityMiBaoFangDian::SendInfo(bool is_recal_phase)
{
	Protocol::SCRAMiBaoFangDianInfo info;
	info.is_need_recal_phase_flag = is_recal_phase ? 1 : 0;
	info.is_open_today_flag = m_param.is_open_today_flag;
	info.cur_use_open_day = m_param.cur_use_open_day;
	memcpy(info.today_buy_times, m_param.today_buy_times, sizeof(info.today_buy_times));
	memcpy(info.activity_buy_times, m_param.cur_phase_activity_buy_times, sizeof(info.activity_buy_times));

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

//索引安全由外部调用保证
void RoleActivityMiBaoFangDian::SendSingleInfo(short seq, short index)
{
	Protocol::SCRAMiBaoFangDianSingleInfo info;
	info.seq = seq;
	info.index = index;
	info.today_buy_times = m_param.today_buy_times[seq];
	info.activity_buy_times = m_param.cur_phase_activity_buy_times[index];

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityMiBaoFangDian::BuyItem(int seq)
{
	if (seq < 0 || seq >= MAX_RA_MI_BAO_FANG_DIAN_PHASE_SHOP_ITEM_NUM)
	{
		return;
	}
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivityMiBaoFangDianConfig * config = static_cast<const RandActivityMiBaoFangDianConfig*>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_MI_BAO_FANG_DIAN));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	const RAMiBaoFangDianShopCfg *buy_item_cfg = config->GetShopCfg(ramgr, seq, m_param.cur_use_open_day);
	if (NULL == buy_item_cfg)
	{
		return;
	}
	short & buy_times_today = m_param.today_buy_times[buy_item_cfg->seq];
	short & buy_times_activity = m_param.cur_phase_activity_buy_times[buy_item_cfg->seq];
	switch (buy_item_cfg->limit_type)
	{
	case ACTVITY_BUY_LIMIT_TYPE_ALL:
	{
		if (buy_times_activity > 0 && buy_times_activity >= buy_item_cfg->buy_times)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT);
			return;
		}
	}
	break;
	case ACTVITY_BUY_LIMIT_TYPE_DAY:
	{
		if (buy_times_today > 0 && buy_times_today >= buy_item_cfg->today_times)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT);
			return;
		}
	}
	break;
	default:
		break;
	}
	//检查消耗
	switch (buy_item_cfg->price_type)
	{
	case RAMiBaoFangDianShopCfg::PRICE_TYPE_COIN:
	{
		if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(buy_item_cfg->price))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return;
		}
		m_role_module_mgr->GetMoney()->UseCoinBind(buy_item_cfg->price, __FUNCTION__);
	}
	break;
	case RAMiBaoFangDianShopCfg::PRICE_TYPE_SILVER_COIN:
	{
		if (!m_role_module_mgr->GetMoney()->OtherCoinMoreThan(buy_item_cfg->price, MONEY_TYPE_SILVER_COIN))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_SILVER_COIN_NOT_ENOUGH);
			return;
		}
		m_role_module_mgr->GetMoney()->UseOtherCoin(buy_item_cfg->price, MONEY_TYPE_SILVER_COIN, __FUNCTION__);
	}
	break;
	case RAMiBaoFangDianShopCfg::PRICE_TYPE_GOLD:
	{
		if (!m_role_module_mgr->GetMoney()->GoldMoreThan(buy_item_cfg->price))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
			return;
		}
		m_role_module_mgr->GetMoney()->UseGold(buy_item_cfg->price, __FUNCTION__);
	}
	break;
	default:
		break;
	}

	bool is_put_succ = false;
	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary(1, &buy_item_cfg->reward_item))
	{
		if (m_role_module_mgr->GetKnapsack()->Put(buy_item_cfg->reward_item, PUT_REASON_MI_BAO_FANG_DIAN))
		{
			is_put_succ = true;
		}
	}
	if (!is_put_succ)
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&buy_item_cfg->reward_item, 1, SEND_MAIL_TYPE_ACTIVITY_BUY_SHOP, true, RAND_ACTIVITY_TYPE_MI_BAO_FANG_DIAN);
	}
	++buy_times_activity;
	++buy_times_today;
	this->SendSingleInfo(buy_item_cfg->seq, buy_item_cfg->seq);
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_national_day.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d], index[%d], seq[%d] cur_use_open_day[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(),
		RAND_ACTIVITY_TYPE_MI_BAO_FANG_DIAN, buy_item_cfg->index, seq, m_param.cur_use_open_day);
}

void RoleActivityMiBaoFangDian::SetOpenFlag()
{
	m_param.is_open_today_flag = 1;
	this->SendInfo();
}

void RoleActivityMiBaoFangDian::CheckPhase()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivityMiBaoFangDianConfig * config = static_cast<const RandActivityMiBaoFangDianConfig*>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_MI_BAO_FANG_DIAN));
	if (NULL == config)
	{
		return;
	}

	int use_open_day = 0, cur_phase = 0;;
	config->GetPhase(ramgr, use_open_day, cur_phase);
	if (m_param.phase != cur_phase)
	{
		memset(m_param.today_buy_times, 0, sizeof(m_param.today_buy_times));
		m_param.phase = cur_phase;
		m_param.cur_use_open_day = use_open_day;
		memset(m_param.cur_phase_activity_buy_times, 0, sizeof(m_param.cur_phase_activity_buy_times));
	}
}
