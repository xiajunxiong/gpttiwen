#include "roleactivitydanbifanli.hpp"
#include "gamelog.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitydanbifanliconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/rolecommonsave/rolecommonsavemanager.hpp"
#include "global/cmdbuyrecordmanager/cmdbuyrecordmanager.hpp"

#include "protocol/randactivity/msgradanbifanli.hpp"

#include "servercommon/errornum.h"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/uniqueidgenerator.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"

RoleActivityDanBiFanLi::RoleActivityDanBiFanLi(int type)
	:RoleActivity(type)
{
}

RoleActivityDanBiFanLi::~RoleActivityDanBiFanLi()
{
}

void RoleActivityDanBiFanLi::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RADanBiFanLiParam));
}

void RoleActivityDanBiFanLi::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RADanBiFanLiParam));
}

void RoleActivityDanBiFanLi::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}
	this->CheckInitRandActivity(m_role_activity_type);
	this->OnSendAllInfo();
}

void RoleActivityDanBiFanLi::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (m_role_activity_type != rand_act_type)
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
		m_param.Reset();
		m_param.ra_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

		this->SetRoleDataChange();
	}
}

void RoleActivityDanBiFanLi::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;
	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}
	this->CheckInitRandActivity(m_role_activity_type);

	memset(m_param.today_buy_times_pool_list, 0, sizeof(m_param.today_buy_times_pool_list));
	this->SetRoleDataChange();
	this->OnSendAllInfo();
}

void RoleActivityDanBiFanLi::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}
	this->CheckInitRandActivity(m_role_activity_type);
	switch (raor->opera_type)
	{
		case Protocol::RA_DAN_BI_FAN_LI_OPERA_TYPE_INFO:
		{
			this->OnSendAllInfo();
		}
		break;
		case Protocol::RA_DAN_BI_FAN_LI_OPERA_TYPE_BUY_CHECK:
		{
			long long unique_id = 0;
			if (!this->OnBuyItemCheck(raor->param_1, raor->param_2, unique_id))
			{
				unique_id = 0;
			}
			//客户端要求无论成功与失败都要下发,失败唯一ID发0
			Protocol::SCCmdBuyItemCheckRet protocol;
			protocol.unique_id = unique_id;
			protocol.cfg_ver = raor->cfg_ver;
			protocol.buy_type = BUY_TYPE;

			m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
		}
		break;
	}
}

bool RoleActivityDanBiFanLi::OnBuyItemCheck(int pool_seq, int seq, long long ARG_OUT & unique_key)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return false;
	}

	const RandActivityDanBiFanLiConfig * config = static_cast<const RandActivityDanBiFanLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return false;
	}
	const RADanBiFanLiRewardRewardCfg * base_cfg = config->GetRewardCfg(ramgr, pool_seq, seq);
	if (NULL == base_cfg || base_cfg->buy_money <= 0)
	{
		return false;
	}
	const RADanBiFanLiPoolCfg * pool_cfg = config->GetPoolCfg(ramgr, pool_seq);
	if (NULL == pool_cfg || pool_seq < 0 || pool_seq >= MAX_RA_DAN_BI_FAN_LI_POOL_NUM)
	{
		return false;
	}

	short & buy_times_today = m_param.today_buy_times_pool_list[pool_seq];
	short & buy_times_activity = m_param.activity_buy_times_pool_list[pool_seq];
	switch (pool_cfg->limit_type)
	{
		case ACTVITY_BUY_LIMIT_TYPE_ALL:
		{
			if (buy_times_activity > 0 && buy_times_activity >= pool_cfg->times)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT);
				return false;
			}
		}
		break;
		case ACTVITY_BUY_LIMIT_TYPE_DAY:
		{
			if (buy_times_today > 0 && buy_times_today >= pool_cfg->times)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT);
				return false;
			}
		}
		break;
		default:
		return false;
	}

	time_t now = EngineAdapter::Instance().Time();
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetNewCmdBuyRecord<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE);
	if (buy_item == NULL)
	{
		return false;
	}
	if (!UniqueIDGenerator::Instance()->GenUniqueID(&buy_item->unique_id))
	{
		return false;
	}

	buy_item->cfg_vesion = config->GetConfigVer();
	buy_item->need_gold = base_cfg->buy_money;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_begin_timestamp;
	buy_item->param1 = pool_seq;
	buy_item->param2 = seq;
	buy_item->param3 = base_cfg->gold_give + base_cfg->gold_num;
	buy_item->param4 = pool_cfg->limit_type;
	buy_item->param5 = (int)GetZeroTime(EngineAdapter::Instance().Time());
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_log_rand_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	unique_key = buy_item->unique_id;
	return true;
}

int RoleActivityDanBiFanLi::OnBuyItem(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.ra_begin_timestamp, ::GetString(*buy_item).c_str());

	if (buy_item->unique_id != unique_id)
	{
		return -__LINE__;
	}

	if (chong_zhi_gold != buy_item->need_gold)
	{
		return  -__LINE__;
	}


	int & pool_seq = buy_item->param1;
	int & seq = buy_item->param2;
	int & add_gold_num = buy_item->param3;
	int & buy_limit = buy_item->param4;
	unsigned int buy_zero_timestamp = (unsigned int)buy_item->param5;
	if (m_param.ra_begin_timestamp == buy_item->ra_begin_timestamp)
	{
		if (pool_seq >= 0 && pool_seq < MAX_RA_DAN_BI_FAN_LI_POOL_NUM)
		{
			if (ACTVITY_BUY_LIMIT_TYPE_DAY == buy_limit && (unsigned int)GetZeroTime(EngineAdapter::Instance().Time()) == buy_zero_timestamp)
			{
				m_param.today_buy_times_pool_list[pool_seq] += 1;
			}
			m_param.activity_buy_times_pool_list[pool_seq] += 1;
		}
		m_param.m_total_chongzhi += chong_zhi_gold;
		this->OnSendSingleInfo(pool_seq);
		this->SetRoleDataChange();
	}
	if (add_gold_num > 0)
	{
		m_role_module_mgr->GetMoney()->AddGold(add_gold_num, __FUNCTION__);
	}
	int today_buy_times = 0;
	int activity_buy_times = 0;
	if (pool_seq >= 0 && pool_seq < MAX_RA_DAN_BI_FAN_LI_POOL_NUM)
	{
		today_buy_times = m_param.today_buy_times_pool_list[pool_seq];
		activity_buy_times = m_param.activity_buy_times_pool_list[pool_seq];
	}
	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_log_rand_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	this->OnParticipateRA(__FUNCTION__);
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] pool_seq[%d] seq[%d] limit_type[%d] buy_times[today:%d, activity:%d] add_gold_num[%d", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), pool_seq, seq, buy_limit, today_buy_times, activity_buy_times, add_gold_num);
	return 0;
}

int RoleActivityDanBiFanLi::OnBuyGiftItem(int pool_seq, int seq, ARG_OUT int * price)
{
	if (pool_seq < 0 || pool_seq >= MAX_RA_DAN_BI_FAN_LI_POOL_NUM)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}
	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	const RandActivityDanBiFanLiConfig * config = static_cast<const RandActivityDanBiFanLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	const RADanBiFanLiRewardRewardCfg * base_cfg = config->GetRewardCfg(ramgr, pool_seq, seq);
	if (NULL == base_cfg || base_cfg->buy_money <= 0)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}
	const RADanBiFanLiPoolCfg * pool_cfg = config->GetPoolCfg(ramgr, pool_seq);
	if (NULL == pool_cfg || pool_seq < 0 || pool_seq >= MAX_RA_DAN_BI_FAN_LI_POOL_NUM)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}
	short & buy_times_today = m_param.today_buy_times_pool_list[pool_seq];
	short & buy_times_activity = m_param.activity_buy_times_pool_list[pool_seq];
	switch (pool_cfg->limit_type)
	{
		case ACTVITY_BUY_LIMIT_TYPE_ALL:
		{
			if (buy_times_activity > 0 && buy_times_activity >= pool_cfg->times)
			{
				return BUY_GIFT_ITEM_RET_TYPE_2;
			} 
		}
		break;
		case ACTVITY_BUY_LIMIT_TYPE_DAY:
		{
			if (buy_times_today > 0 && buy_times_today >= pool_cfg->times)
			{
				return BUY_GIFT_ITEM_RET_TYPE_2;
			}
		}
		break;
		default:
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	if (NULL != price)
	{
		*price = base_cfg->buy_money;
		return 0;
	}

	int add_gold_num = base_cfg->gold_give + base_cfg->gold_num;
	if (add_gold_num > 0)
	{
		m_role_module_mgr->GetMoney()->AddGold(add_gold_num, __FUNCTION__);
	}

	buy_times_today += 1;
	buy_times_activity += 1;
	m_param.m_total_chongzhi += base_cfg->buy_money;
	this->OnSendSingleInfo(pool_seq);
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] pool_seq[%d] seq[%d] buy_times[today:%d, activity:%d] gold[%d, give:%d]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), pool_seq, seq, buy_times_today, buy_times_activity, base_cfg->gold_num, base_cfg->gold_give);
	return 0;
}

void RoleActivityDanBiFanLi::OnSendAllInfo()
{
	Protocol::SCRADanBiFanLiInfo info;
	UNSTD_STATIC_CHECK(sizeof(info.today_buy_times_pool_list) == sizeof(m_param.today_buy_times_pool_list));
	UNSTD_STATIC_CHECK(sizeof(info.activity_buy_times_pool_list) == sizeof(m_param.activity_buy_times_pool_list));
	memcpy(info.today_buy_times_pool_list, m_param.today_buy_times_pool_list, sizeof(info.today_buy_times_pool_list));
	memcpy(info.activity_buy_times_pool_list, m_param.activity_buy_times_pool_list, sizeof(info.activity_buy_times_pool_list));

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityDanBiFanLi::OnSendSingleInfo(int pool_seq)
{
	if (pool_seq < 0 || pool_seq >= ARRAY_ITEM_COUNT(m_param.today_buy_times_pool_list))
	{
		return;
	}

	Protocol::SCRADanBiFanLiSingleInfo info;
	info.pool_seq = pool_seq;
	info.today_buy_times = m_param.today_buy_times_pool_list[pool_seq];
	info.activity_buy_times = m_param.activity_buy_times_pool_list[pool_seq];

	m_role_module_mgr->NetSend(&info, sizeof(info));
}
