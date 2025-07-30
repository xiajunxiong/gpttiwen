#include "roleactivityqixitehui.hpp"
#include "gamelog.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityqixitehuiconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/rolecommonsave/rolecommonsavemanager.hpp"
#include "global/cmdbuyrecordmanager/cmdbuyrecordmanager.hpp"

#include "protocol/randactivity/msgraqixitehui.hpp"

#include "servercommon/errornum.h"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/uniqueidgenerator.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"


RoleActivityQiXiTeHui::RoleActivityQiXiTeHui(int type)
	:RoleActivity(type)
{
}

RoleActivityQiXiTeHui::~RoleActivityQiXiTeHui()
{
}

void RoleActivityQiXiTeHui::OnFetchPhaseReward(int seq)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivityQiXiTeHuiConfig * config = static_cast<const RandActivityQiXiTeHuiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	const RAQiXiTeHuiPhaseRewardCfg * phase_cfg = config->GetPhaseRewardCfg(ramgr, seq);
	if (NULL == phase_cfg)
	{
		return;
	}

	if (m_param.phase_reward_flag.IsBitSet(seq))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	if (m_param.activity_buy_count < phase_cfg->buy_times)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_NOT_ELIGIBLE);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(phase_cfg->reward.item_id, phase_cfg->reward.num, false))
	{
		m_role_module_mgr->GetKnapsack()->SendErrorNotice();

		return;
	}
	if (!m_role_module_mgr->GetKnapsack()->Put(phase_cfg->reward, PUT_REASON_RA_QI_XI_TE_HUI))
	{
		m_role_module_mgr->GetKnapsack()->SendErrorNotice();
		return;
	}

	m_param.phase_reward_flag.SetBit(seq);
	this->SetRoleDataChange();
	this->OnSendOtherInfo();
	gamelog::g_log_qi_xi_te_hui.printf(LL_INFO, "%s activity_type[%d] role[%d %s] fetch_seq[%d]", __FUNCTION__, m_role_activity_type, 
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), seq);
}

void RoleActivityQiXiTeHui::OnCancelRedDot()
{
	m_param.is_init_open_interface_flag = 1;
	this->SetRoleDataChange();
	this->OnSendOtherInfo();
}

void RoleActivityQiXiTeHui::OnBuyItemCheck(int seq)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return ;
	}

	const RandActivityQiXiTeHuiConfig * config = static_cast<const RandActivityQiXiTeHuiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return ;
	}

	const RAQiXiTeHuiBaseCfg * base_cfg = config->GetBaseCfg(ramgr, seq);
	if (NULL == base_cfg || base_cfg->price <= 0)
	{
		return;
	}

	int act_real_open_day = config->GetActOpenDay(ramgr, m_role_activity_type);
	if (act_real_open_day < base_cfg->activity_day)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_SHOP_LOCK_NOT_CAN_BUY_ITEM);
		return;
	}
	
	//seq已经在配置读取判断过了
	short & buy_times_today = m_param.today_buy_times[seq];
	short & buy_times_activity = m_param.activity_buy_times[seq];
	switch (base_cfg->buy_limit)
	{
		case ACTVITY_BUY_LIMIT_TYPE_ALL:
		{
			if (buy_times_activity > 0 && buy_times_activity >= base_cfg->limit_times)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT);
				return;
			}
		}
		break;
		case ACTVITY_BUY_LIMIT_TYPE_DAY:
		{
			if (buy_times_today > 0 && buy_times_today >= base_cfg->limit_times)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT);
				return;
			}
		}
		break;
		default:
		return;
	}

	const RAQiXiTeHuiRewardCfg * reward_cfg = config->GetRewardCfg(base_cfg->reward_group);
	if (NULL == reward_cfg)
	{
		return;
	}

	time_t now = EngineAdapter::Instance().Time();
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetNewCmdBuyRecord<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE);
	if (buy_item == NULL)
	{
		return;
	}
	if (!UniqueIDGenerator::Instance()->GenUniqueID(&buy_item->unique_id))
	{
		return;
	}

	buy_item->cfg_vesion = config->GetConfigVer();
	buy_item->need_gold = base_cfg->price;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_qi_xi_te_hui_begin_timestamp;
	buy_item->param1 = seq;
	buy_item->param2 = base_cfg->buy_limit;
	buy_item->param3 = (int)GetZeroTime(EngineAdapter::Instance().Time());
	::ItemConfigDataVecToBuyItem(reward_cfg->item_list, buy_item->rewards_item, &buy_item->rewards_num);
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_log_qi_xi_te_hui, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_qi_xi_te_hui.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	Protocol::SCCmdBuyItemCheckRet info;
	info.buy_type = BUY_TYPE;
	info.cfg_ver = buy_item->cfg_vesion;
	info.unique_id = buy_item->unique_id;
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

int RoleActivityQiXiTeHui::OnBuyItem(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	gamelog::g_log_qi_xi_te_hui.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.ra_qi_xi_te_hui_begin_timestamp, ::GetString(*buy_item).c_str());

	if (buy_item->unique_id != unique_id)
	{
		return -__LINE__;
	}

	if (chong_zhi_gold != buy_item->need_gold)
	{
		return  -__LINE__;
	}

	std::vector<ItemConfigData> rewards = ::BuyItemToItemConfigDataVec(buy_item->rewards_num, buy_item->rewards_item);
	if (rewards.empty())
	{
		return  -__LINE__;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(rewards))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(rewards, PUT_REASON_RA_QI_XI_TE_HUI);
	}
	else
	{
		// 发邮件
		m_role_module_mgr->GetKnapsack()->SendMail(rewards, SEND_MAIL_TYPE_ACTIVITY_BUY_SHOP, false, m_role_activity_type);
	}

	int & seq = buy_item->param1;
	int & buy_limit = buy_item->param2;
	unsigned int buy_zero_timestamp = (unsigned int)buy_item->param3;
	if (m_param.ra_qi_xi_te_hui_begin_timestamp == buy_item->ra_begin_timestamp)
	{
		if (seq >= 0 && seq < RA_QI_XI_TE_HUI_MAX_BASE_NUM)
		{
			if (ACTVITY_BUY_LIMIT_TYPE_DAY == buy_limit && (unsigned int)GetZeroTime(EngineAdapter::Instance().Time()) == buy_zero_timestamp)
			{
				m_param.today_buy_times[buy_item->param1] += 1;
			}
			m_param.activity_buy_times[buy_item->param1] += 1;
		}
		m_param.activity_buy_count += 1;
		this->OnSendSingleInfo(seq);
		this->SetRoleDataChange();
	}
	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_log_qi_xi_te_hui, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	this->OnParticipateRA(__FUNCTION__);
	return 0;
}

int RoleActivityQiXiTeHui::OnBuyGiftItem(int seq, ARG_OUT int * price)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return 1;
	}

	const RandActivityQiXiTeHuiConfig * config = static_cast<const RandActivityQiXiTeHuiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return 1;
	}

	const RAQiXiTeHuiBaseCfg * base_cfg = config->GetBaseCfg(ramgr, seq);
	if (NULL == base_cfg || base_cfg->price <= 0)
	{
		return 1;
	}

	int act_real_open_day = config->GetActRealOpenDay(ramgr, m_role_activity_type);
	if (act_real_open_day < base_cfg->activity_day)
	{
		return 1;
	}

	//seq已经在配置读取判断过了
	short & buy_times_today = m_param.today_buy_times[seq];
	short & buy_times_activity = m_param.activity_buy_times[seq];
	switch (base_cfg->buy_limit)
	{
		case ACTVITY_BUY_LIMIT_TYPE_ALL:
		{
			if (buy_times_activity > 0 && buy_times_activity >= base_cfg->limit_times)
			{
				return 2;
			}
		}
		break;
		case ACTVITY_BUY_LIMIT_TYPE_DAY:
		{
			if (buy_times_today > 0 && buy_times_today >= base_cfg->limit_times)
			{
				return 2;
			}
		}
		break;
		default:
		return 1;
	}

	const RAQiXiTeHuiRewardCfg * reward_cfg = config->GetRewardCfg(base_cfg->reward_group);
	if (NULL == reward_cfg)
	{
		return 1;
	}
	
	if (NULL != price)
	{
		*price = base_cfg->price;
		return 0;
	}


	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(reward_cfg->item_list))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(reward_cfg->item_list, PUT_REASON_RA_QI_XI_TE_HUI);
	}
	else
	{
		// 发邮件
		m_role_module_mgr->GetKnapsack()->SendMail(reward_cfg->item_list, SEND_MAIL_TYPE_ACTIVITY_BUY_SHOP, false, m_role_activity_type);
	}


	m_param.today_buy_times[seq] += 1;
	m_param.activity_buy_times[seq] += 1;
	m_param.activity_buy_count += 1;
	this->OnSendSingleInfo(seq);
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	return 0;
}

void RoleActivityQiXiTeHui::OnSendAllInfo()
{
	Protocol::SCRAQiXiTeHuiInfo info;
	info.is_init_open_interface_flag = m_param.is_init_open_interface_flag;
	info.reserve_ch = 0;
	info.activity_buy_count = m_param.activity_buy_count;
	info.phase_reward_flag = m_param.phase_reward_flag;
	UNSTD_STATIC_CHECK(sizeof(info.activity_buy_times) == sizeof(m_param.activity_buy_times));
	UNSTD_STATIC_CHECK(sizeof(info.today_buy_times) == sizeof(m_param.today_buy_times));
	memcpy(info.activity_buy_times, m_param.activity_buy_times, sizeof(info.activity_buy_times));
	memcpy(info.today_buy_times, m_param.today_buy_times, sizeof(info.today_buy_times));

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityQiXiTeHui::OnSendSingleInfo(int seq)
{
	if(seq < 0 || seq >= RA_QI_XI_TE_HUI_MAX_BASE_NUM) return ;

	Protocol::SCRAQiXiTeHuiSingleInfo info;
	info.activity_buy_count = m_param.activity_buy_count;
	info.seq = seq;
	info.activity_buy_times = m_param.activity_buy_times[seq];
	info.today_buy_times = m_param.today_buy_times[seq];

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityQiXiTeHui::OnSendOtherInfo()
{
	Protocol::SCRAQiXiTeHuiOtherInfo info;
	info.is_init_open_interface_flag = m_param.is_init_open_interface_flag;
	info.reserve_ch = 0;
	info.reserve_sh = 0;
	info.phase_reward_flag = m_param.phase_reward_flag;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityQiXiTeHui::GiveUnFetchReward()
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}
	const RandActivityQiXiTeHuiConfig * config = static_cast<const RandActivityQiXiTeHuiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}
	const RAQiXiTeHuiPhaseRewardSectionCfg * phase_section_cfg = config->GetPhaseSectionCfg(ramgr);
	if (NULL == phase_section_cfg)
	{
		return;
	}

	std::vector<ItemConfigData> reward_list;
	for (int i = 0; i < (int)phase_section_cfg->reward_list.size() && i < RA_QI_XI_TE_HUI_MAX_PHASE_NUM; ++i)
	{
		if (m_param.phase_reward_flag.IsBitSet(i))
		{
			continue;
		}

		if (m_param.activity_buy_count < phase_section_cfg->reward_list[i].buy_times)
		{
			continue;
		}

		reward_list.push_back(phase_section_cfg->reward_list[i].reward);
		m_param.phase_reward_flag.SetBit(i);
		this->SetRoleDataChange();
		gamelog::g_log_qi_xi_te_hui.printf(LL_INFO, "%s role[%d %s] seq[%d]", __FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), 
			m_role_module_mgr->GetRole()->GetName(), i);

	}

	if (reward_list.empty())
	{
		return;
	}

	m_role_module_mgr->GetKnapsack()->SendMail(reward_list, SEND_MAIL_TYPE_ACTIVITY_END, false, m_role_activity_type);
	gamelog::g_log_qi_xi_te_hui.printf(LL_INFO, "%s role[%d %s] succ", __FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), 
		m_role_module_mgr->GetRole()->GetName());
}

void RoleActivityQiXiTeHui::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAQiXiTeHuiParam));
}

void RoleActivityQiXiTeHui::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAQiXiTeHuiParam));

}

void RoleActivityQiXiTeHui::OnLoginSendInfo()
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

void RoleActivityQiXiTeHui::CheckInitRandActivity(int rand_act_type, bool is_force)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_qi_xi_te_hui_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.ra_qi_xi_te_hui_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

		this->SetRoleDataChange();
	}
}

void RoleActivityQiXiTeHui::OnRAClose()
{
	this->GiveUnFetchReward();
}

void RoleActivityQiXiTeHui::OnLoginCheck()
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	// 防止误启动活动，不在跨服启动的活动会导致重置
	if (CrossConfig::Instance().IsHiddenServer())
	{
		RandActivity * activity = ramgr->ForceGetRandActivity(this->GetRoleActivityType());
		if (NULL != activity && !activity->CanSyncActivityDataToCross())
		{
			return;
		}
	}

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(this->GetRoleActivityType(), m_param.ra_qi_xi_te_hui_begin_timestamp);
	if (in_activity_time)
	{
		return;
	}

	this->GiveUnFetchReward();
}

void RoleActivityQiXiTeHui::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
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

	memset(m_param.today_buy_times, 0, sizeof(m_param.today_buy_times));
	this->SetRoleDataChange();
	this->OnSendAllInfo();
}

void RoleActivityQiXiTeHui::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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
		case Protocol::RA_QI_XI_TE_HUI_OPERA_TYPE_INFO:
		{
			this->OnSendAllInfo();
		}
		break;
		case Protocol::RA_QI_XI_TE_HUI_OPERA_TYPE_BUY_ITEM:
		{
			this->OnBuyItemCheck(raor->param_1);
		}
		break;
		case Protocol::RA_QI_XI_TE_HUI_OPERA_TYPE_CANCEL_RED_DOT:
		{
			this->OnCancelRedDot();
		}
		break;
		case Protocol::RA_QI_XI_TE_HUI_OPERA_TYPE_FETCH_PHASE_REWARD:
		{
			this->OnFetchPhaseReward(raor->param_1);
		}
		break;
	}
}
