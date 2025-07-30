#include "roleactivitysecrettreasure.hpp"
#include "gamelog.h"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "item/money.h"
#include "item/knapsack.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "protocol/randactivity/msgrasecrettreasure.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitysecrettreasureconfig.hpp"

RoleActivitySecretTreasure::RoleActivitySecretTreasure(int type)
	:RoleActivity(type)
{
}

RoleActivitySecretTreasure::~RoleActivitySecretTreasure()
{
}

void RoleActivitySecretTreasure::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RASecretTreasureParam));
}

void RoleActivitySecretTreasure::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RASecretTreasureParam));
}

void RoleActivitySecretTreasure::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SPECIAL_SECRET_TREASURE))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_SPECIAL_SECRET_TREASURE);
	this->SendInfo();
}

void RoleActivitySecretTreasure::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_SPECIAL_SECRET_TREASURE != rand_act_type)
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
		const RandActivitySecretTreasureConfig * config = static_cast<const RandActivitySecretTreasureConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SPECIAL_SECRET_TREASURE));
		if (NULL == config)
		{
			return;
		}
		m_param.Reset();
		m_param.ra_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		m_param.phase = config->GetPhase(ramgr);
		this->SetRoleDataChange();
	}
}

void RoleActivitySecretTreasure::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if(old_dayid == now_dayid) return;
	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SPECIAL_SECRET_TREASURE))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_SPECIAL_SECRET_TREASURE);
	const RandActivitySecretTreasureConfig * config = static_cast<const RandActivitySecretTreasureConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SPECIAL_SECRET_TREASURE));
	if (NULL == config) return;

	int phase = config->GetPhase(ramgr);
	if (phase > 0 && phase != m_param.phase)
	{
		m_param.phase = phase;
	}
	memset(m_param.today_buy_times, 0, sizeof(m_param.today_buy_times));
	m_param.is_open_today_flag = 0;
	this->SetRoleDataChange();
	this->SendInfo();
}

void RoleActivitySecretTreasure::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SPECIAL_SECRET_TREASURE))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_SPECIAL_SECRET_TREASURE);

	switch (raor->opera_type)
	{
		case Protocol::RA_SECRET_TREASURE_REQ_TYPE_INFO:
		{
			this->SendInfo();
		}
		break;
		case Protocol::RA_SECRET_TREASURE_REQ_TYPE_BUY_ITEM:
		{
			this->BuyItem(raor->param_1, raor->param_2);
		}
		break;
		case Protocol::RA_SECRET_TREASURE_REQ_TYPE_OPEN_ACTIVITY:
		{
			this->SetOpenFlag();
		}
		break;
	default:
		break;
	}
}

bool RoleActivitySecretTreasure::OnCmdBuyItem(int type, int chong_zhi_gold)
{
	const RandActivitySecretTreasureConfig * config = static_cast<const RandActivitySecretTreasureConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SPECIAL_SECRET_TREASURE));
	if (NULL == config)
	{
		gamelog::g_log_serious_error.printf(LL_INFO, "%s Err!user[%d %s] type[%d] chongzhi_gold[%d] reason[config NULL]", __FUNCTION__,
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(), type, chong_zhi_gold);
		return false;
	}
	
	const RASecretTreasureShopCfg * buy_item_cfg = config->GetShopCfg(type);
	if (NULL == buy_item_cfg)
	{
		gamelog::g_log_serious_error.printf(LL_INFO, "%s Err!user[%d %s] type[%d] chongzhi_gold[%d] reason[buy_item_cfg NULL]", __FUNCTION__,
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(), type, chong_zhi_gold);
		return false;
	}

	if (chong_zhi_gold != buy_item_cfg->price)
	{
		gamelog::g_log_serious_error.printf(LL_INFO, "%s Err!user[%d %s] type_id[%d] chongzhi_gold[%d] reason[chong_zhi_gold != buy_item_cfg->price]", __FUNCTION__,
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(), type, chong_zhi_gold);
		return false;
	}

	short & today_buy_times = m_param.today_buy_times[buy_item_cfg->seq];
	short & activity_buy_times = m_param.activity_buy_times[buy_item_cfg->type];
	switch (buy_item_cfg->limit_type)
	{
	case ACTVITY_BUY_LIMIT_TYPE_ALL:
		{
			if (activity_buy_times > 0 && activity_buy_times >= buy_item_cfg->buy_times)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT);
				return false;
			}
		}
		break;
	case ACTVITY_BUY_LIMIT_TYPE_DAY:
		{
			if (today_buy_times > 0 && today_buy_times >= buy_item_cfg->today_times)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT);
				return false;
			}
		}
		break;
	default:
		break;
	}

	bool is_put_succ = false;
	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary(1, &buy_item_cfg->item))
	{
		if (m_role_module_mgr->GetKnapsack()->Put(buy_item_cfg->item, PUT_REASON_SECRET_TREASURE))
		{
			is_put_succ = true;
		}
	}
	if (!is_put_succ)
	{
		if (!m_role_module_mgr->GetKnapsack()->SendMail(&buy_item_cfg->item, 1, SEND_MAIL_TYPE_SECRET_TREASURE, true))
		{
			gamelog::g_log_serious_error.printf(LL_INFO, "%s Err!user[%d %s] type_id[%d] chongzhi_gold[%d] reason[send mail error]", __FUNCTION__,
				m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(), type, chong_zhi_gold);
			return false;
		}
	}

	++today_buy_times;
	++activity_buy_times;

	this->SendInfo();
	this->SetRoleDataChange();
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d], type[%d], chong_zhi_gold[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(),
		RAND_ACTIVITY_TYPE_SPECIAL_SECRET_TREASURE, type, chong_zhi_gold);
	return true;
}

void RoleActivitySecretTreasure::SendInfo()
{
	Protocol::SCRASecretTreasureInfo info;
	info.phase = m_param.phase;
	info.is_open_today_flag = m_param.is_open_today_flag;
	memcpy(info.today_buy_times, m_param.today_buy_times, sizeof(info.today_buy_times));
	memcpy(info.activity_buy_times, m_param.activity_buy_times, sizeof(info.activity_buy_times));

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivitySecretTreasure::BuyItem(int type, int seq)
{
	if (seq < 0 || seq >= MAX_SECRET_TREASURE_PHASE_SHOP_ITEM_NUM)
	{
		return;
	}
	const RandActivitySecretTreasureConfig * config = static_cast<const RandActivitySecretTreasureConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SPECIAL_SECRET_TREASURE));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	if (m_param.phase < 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RASecretTreasureShopCfg * buy_item_cfg = config->GetShopCfg(type);
	if (NULL == buy_item_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	const RASecretTreasureShopCfg * check_item_cfg = config->GetShopCfg(m_param.phase, seq);
	if (NULL == check_item_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	if (buy_item_cfg->seq != check_item_cfg->seq || buy_item_cfg->type != check_item_cfg->type)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	short has_buy_times_today = m_param.today_buy_times[buy_item_cfg->seq];
	short has_buy_times_activity = m_param.activity_buy_times[buy_item_cfg->type];
	switch (buy_item_cfg->limit_type)
	{
	case ACTVITY_BUY_LIMIT_TYPE_ALL:
		{
			if (has_buy_times_activity > 0 && has_buy_times_activity >= buy_item_cfg->buy_times)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT);
				return;
			}
		}
		break;
	case ACTVITY_BUY_LIMIT_TYPE_DAY:
		{
			if (has_buy_times_today > 0 && has_buy_times_today >= buy_item_cfg->today_times)
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
	case RASecretTreasureShopCfg::PRICE_TYPE_COIN:
	{
		if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(buy_item_cfg->price))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return;
		}
		m_role_module_mgr->GetMoney()->UseCoinBind(buy_item_cfg->price, __FUNCTION__);
	}
	break;
	case RASecretTreasureShopCfg::PRICE_TYPE_GOLD:
	{
		if (!m_role_module_mgr->GetMoney()->GoldMoreThan(buy_item_cfg->price))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
			return;
		}
		m_role_module_mgr->GetMoney()->UseGold(buy_item_cfg->price, __FUNCTION__);
	}
	break;
	case RASecretTreasureShopCfg::PRICE_TYPE_MONEY:
	{
		return;
	}
	break;
	default:
		break;
	}
	bool is_put_succ = false;
	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary(1, &buy_item_cfg->item))
	{
		if (m_role_module_mgr->GetKnapsack()->Put(buy_item_cfg->item, PUT_REASON_SECRET_TREASURE))
		{
			is_put_succ = true;
		}
	}
	if (!is_put_succ)
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&buy_item_cfg->item, 1, SEND_MAIL_TYPE_SECRET_TREASURE, true);
	}

	++m_param.activity_buy_times[buy_item_cfg->type];
	++m_param.today_buy_times[seq];
	this->SendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d], type[%d], seq[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(),
		RAND_ACTIVITY_TYPE_SPECIAL_SECRET_TREASURE, type, seq);
}

void RoleActivitySecretTreasure::SetOpenFlag()
{
	m_param.is_open_today_flag = 1;
	this->SetRoleDataChange();
	this->SendInfo();
}
