#include "roleactivityshanhaimeet.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityshanhaimeetconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/funopen/funopen.hpp"
#include "other/route/mailroute.hpp"
#include "servercommon/uniqueidgenerator.hpp"

#include "global/cmdbuyrecordmanager/cmdbuyrecordmanager.hpp"
#include "other/rolecommonsave/rolecommonsavemanager.hpp"
#include "protocol/randactivity/msgrashanhaimeet.h"

#include "servercommon/errornum.h"
#include "gamelog.h"
#include "engineadapter.h"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/serverconfig/crossconfig.hpp"


RoleActivityShanHaiMeet::RoleActivityShanHaiMeet(int type)
	:RoleActivity(type)
{

}

RoleActivityShanHaiMeet::~RoleActivityShanHaiMeet()
{

}

void RoleActivityShanHaiMeet::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAShanHaiMeetParam));
}

void RoleActivityShanHaiMeet::GetInitParam(RoleActivityRoleData *p)
{
	memcpy(p->base_data, &m_param, sizeof(RAShanHaiMeetParam));
}

void RoleActivityShanHaiMeet::OnLogin()
{
}

void RoleActivityShanHaiMeet::OnLoginSendInfo()
{
	this->SendAllInfo();
}

void RoleActivityShanHaiMeet::Update(time_t now_second)
{
}

void RoleActivityShanHaiMeet::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
}

void RoleActivityShanHaiMeet::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_SHAN_HAI_MEET != rand_act_type)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

		this->SetRoleDataChange();
	}
}

void RoleActivityShanHaiMeet::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SHAN_HAI_MEET))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_MEET);

	switch (raor->opera_type)
	{
	case Protocol::RA_SHAN_HAI_MEET_TYPE_GET_INFO:
		this->SendAllInfo();
		break;

	case Protocol::RA_SHAN_HAI_MEET_OPERA_TYPE_FETCH:
		this->OnFetch(raor->param_1);
		break;

	case Protocol::RA_SHAN_HAI_MEET_OPERA_TYPE_POKER:
		this->OnPoker(raor->param_1);
		break;

	case Protocol::RA_SHAN_HAI_MEET_OPERA_TYPE_BUY:
		{
			long long unique_id = 0;
			int ret_num = this->OnBuyItemCheck(unique_id);
			if (ret_num == 0)
			{
				Protocol::SCCmdBuyItemCheckRet protocol;
				protocol.unique_id = unique_id;
				protocol.cfg_ver = raor->cfg_ver;
				protocol.buy_type = BUY_TYPE;
				m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
			}
		}
		break;
	case Protocol::RA_SHAN_HAI_MEET_OPERA_RESET_POKER:
		this->OnResetPoker();
		break;

	default:
		break;
	}
}

int RoleActivityShanHaiMeet::OnBuyItemCheck(long long ARG_OUT & unique_key)
{
	const RandActivityShanHaiMeetConfig * config = static_cast<const RandActivityShanHaiMeetConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -__LINE__;
	}

	unique_key = 0;
	if (!UniqueIDGenerator::Instance()->GenUniqueID(&unique_key))
	{
		unique_key = 0;
		return -__LINE__;
	}

	time_t now = EngineAdapter::Instance().Time();
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetNewCmdBuyRecord<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	buy_item->cfg_vesion = config->GetConfigVer();
	buy_item->need_gold = config->GetOtherCfg().buy_money;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.begin_timestamp;
	buy_item->unique_id = unique_key;
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_log_rand_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	return 0;
}

int RoleActivityShanHaiMeet::OnCmdBuyItem(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.begin_timestamp, ::GetString(*buy_item).c_str());

	if (buy_item->unique_id != unique_id)
	{
		return -__LINE__;
	}

	if (chong_zhi_gold != buy_item->need_gold)
	{
		return -__LINE__;
	}

	m_param.is_buy = 1;

	this->SendAllInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_log_rand_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	return 0;
}

int RoleActivityShanHaiMeet::OnBuyGiftItem(ARG_OUT int * price)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return 1;
	}
	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return 1;
	}
	if (1 == m_param.is_buy)
	{
		return 2;
	}
	const RandActivityShanHaiMeetConfig * config = static_cast<const RandActivityShanHaiMeetConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return 1;
	}
	if (NULL != price)
	{
		*price = config->GetOtherCfg().buy_money;
		return 0;
	}

	m_param.is_buy = 1;

	this->SendAllInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.begin_timestamp);

	return 0;
}

void RoleActivityShanHaiMeet::OnFetch(int seq)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivityShanHaiMeetConfig * config = static_cast<const RandActivityShanHaiMeetConfig*>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SHAN_HAI_MEET));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAShanHaiMeetCfg* cfg = config->GetBaseCfg(ramgr, seq);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (1 != m_param.is_buy)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_SHAN_HAI_MEET_NOT_BUY);
		return;
	}

	if (m_param.fetch_flag.IsBitSet(seq))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GIVE_SECRET_KEY_HAS_GET_REWARD);
		return;
	}

	std::vector<ItemConfigData> reward_list;
	if (m_param.finish_count[cfg->task_type] < cfg->param_1)
	{
		int act_id = 0;
		switch (cfg->task_type)
		{
		case RA_SHAN_HAI_MEET_ACT_TYPE_0:
			act_id = 2122;
			break;
		case RA_SHAN_HAI_MEET_ACT_TYPE_1:
			act_id = 2134;
			break;
		case RA_SHAN_HAI_MEET_ACT_TYPE_2:
			act_id = 2124;
			break;
		case RA_SHAN_HAI_MEET_ACT_TYPE_3:
			act_id = 2142;
			break;
		case RA_SHAN_HAI_MEET_ACT_TYPE_4:
			act_id = 2140;
			break;
		case RA_SHAN_HAI_MEET_ACT_TYPE_5:
			act_id = 2122;
			break;
		case RA_SHAN_HAI_MEET_ACT_TYPE_6:
			act_id = 2125;
			break;
		case RA_SHAN_HAI_MEET_ACT_TYPE_7:
		case RA_SHAN_HAI_MEET_ACT_TYPE_8:
			act_id = 2156;
			break;
		default:
			break;
		}
		UNSTD_STATIC_CHECK(RA_SHAN_HAI_MEET_ACT_TYPE_END == 9);

		if (ramgr->IsRandActivityOpen(act_id))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_CONDITIONS_NOT_REACHED);
			return;
		}

		reward_list.assign(cfg->unfinish_item.begin(), cfg->unfinish_item.end());
	}
	else
	{
		reward_list.assign(cfg->finish_item.begin(), cfg->finish_item.end());
	}

	if (!reward_list.empty())
	{
		bool is_put_succ = false;
		if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)reward_list.size(), &reward_list[0]))
		{
			if (m_role_module_mgr->GetKnapsack()->PutList((short)reward_list.size(), &reward_list[0], PUT_REASON_RA_SHAN_HAI_MEET))
			{
				is_put_succ = true;
			}
		}
		if (!is_put_succ)
		{
			m_role_module_mgr->GetKnapsack()->SendMail(&reward_list[0], (short)reward_list.size(), PUT_REASON_RA_SHAN_HAI_MEET, true, RAND_ACTIVITY_TYPE_SHAN_HAI_MEET);
		}
		this->OnSendResultNotice(Protocol::SCRAResultNotice::RAND_ACTIVITY_RESULT_NOTICE_TYPE_GET_REWARD, &reward_list);
		//m_role_module_mgr->GetKnapsack()->NotifyGetItemList((short)reward_list.size(), &reward_list[0], GET_REASON_ACTVITIY_REWARD, RAND_ACTIVITY_TYPE_SHAN_HAI_MEET);
	}

	m_param.fetch_flag.SetBit(seq);
	this->SetRoleDataChange();
	this->SendAllInfo();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_shan_hai_meet.printf(LL_INFO, "%s user[%d, %s], seq[%d], activity_type[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), seq, RAND_ACTIVITY_TYPE_SHAN_HAI_MEET);
}

void RoleActivityShanHaiMeet::OnPoker(int index)
{
	if (0 > index || index >= RA_SHAN_HAI_MEET_POKER_MAX_INDEX)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (1 != m_param.is_buy)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_SHAN_HAI_MEET_CAN_NOT_POKER);
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivityShanHaiMeetConfig * config = static_cast<const RandActivityShanHaiMeetConfig*>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SHAN_HAI_MEET));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAShanHaiMeetPokerCfg* poker_cfg = config->GetPokerCfg(ramgr);
	if (NULL == poker_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAShanHaiMeetRewardCfg* cfg = config->GetRewardCfg(poker_cfg->reward_group);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAShanHaiMeetOtherCfg& other_cfg = config->GetOtherCfg();

	if (other_cfg.poker_cost_num > m_role_module_mgr->GetKnapsack()->Count(other_cfg.poker_cost_id, false))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
		return;
	}

	m_role_module_mgr->GetKnapsack()->ConsumeItem(other_cfg.poker_cost_id, other_cfg.poker_cost_num, __FUNCTION__);

	std::vector<ItemConfigData> reward_list;
	reward_list.push_back(cfg->item);

	if (!reward_list.empty())
	{
		bool is_put_succ = false;
		if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)reward_list.size(), &reward_list[0]))
		{
			if (m_role_module_mgr->GetKnapsack()->PutList((short)reward_list.size(), &reward_list[0], PUT_REASON_RA_SHAN_HAI_MEET))
			{
				is_put_succ = true;
			}
		}
		if (!is_put_succ)
		{
			m_role_module_mgr->GetKnapsack()->SendMail(&reward_list[0], (short)reward_list.size(), SEND_MAIL_TYPE_SHAN_HAI_MEET, true, RAND_ACTIVITY_TYPE_SHAN_HAI_MEET);
		}
		this->OnSendResultNotice(Protocol::SCRAResultNotice::RAND_ACTIVITY_RESULT_NOTICE_TYPE_GET_REWARD, &reward_list);
		//m_role_module_mgr->GetKnapsack()->NotifyGetItemList((short)reward_list.size(), &reward_list[0], GET_REASON_ACTVITIY_REWARD, RAND_ACTIVITY_TYPE_SHAN_HAI_MEET);
	}

	m_param.poker_idx_flag.SetBit(index);
	m_param.poker_item_seq[index] = cfg->seq;

	this->SetRoleDataChange();
	this->SendAllInfo();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_shan_hai_meet.printf(LL_INFO, "%s user[%d, %s], index[%d], reward_seq[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), index, cfg->seq);
}

void RoleActivityShanHaiMeet::OnResetPoker()
{
	bool is_all_poker = true;
	for (int i = 0; i < RA_SHAN_HAI_MEET_POKER_MAX_INDEX; ++i)
	{
		if (!m_param.poker_idx_flag.IsBitSet(i))
		{
			is_all_poker = false;
			break;
		}
	}

	if (!is_all_poker)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_SHAN_HAI_MEET_NOT_POKER_ALL);
		return;
	}

	m_param.poker_idx_flag.Reset();
	memset(m_param.poker_item_seq, 0, sizeof(m_param.poker_item_seq));

	this->SetRoleDataChange();
	this->SendAllInfo();
	gamelog::g_log_shan_hai_meet.printf(LL_INFO, "%s user[%d, %s] ResetPoker!!", __FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName());
}

void RoleActivityShanHaiMeet::OnFinishType(int type, int count)
{
	if (RA_SHAN_HAI_MEET_ACT_TYPE_0 > type || type >= RA_SHAN_HAI_MEET_ACT_TYPE_END) return;
	if (0 >= count) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SHAN_HAI_MEET))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_MEET);

	m_param.finish_count[type] += count;
	this->SendAllInfo();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_shan_hai_meet.printf(LL_INFO, "%s user[%d, %s], type[%d], count[%d] all_count[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), type, count, m_param.finish_count[type]);
}

void RoleActivityShanHaiMeet::SendAllInfo()
{
	Protocol::SCRAShanHaiMeetInfo info;
	info.is_buy = m_param.is_buy;
	info.sh_server = 0;
	info.ch_server = 0;
	info.poker_idx_flag = m_param.poker_idx_flag;
	info.fetch_flag = m_param.fetch_flag;

	memset(info.poker_item_seq, -1, sizeof(info.poker_item_seq));
	memcpy(info.poker_item_seq, m_param.poker_item_seq, sizeof(info.poker_item_seq));

	info.count = RA_SHAN_HAI_MEET_ACT_TYPE_END;
	memset(info.finish_count, 0, sizeof(info.finish_count));
	for (int i = 0; i < RA_SHAN_HAI_MEET_ACT_TYPE_END; ++i)
	{
		info.finish_count[i] = m_param.finish_count[i];
	}

	int len = sizeof(info) - (RA_SHAN_HAI_MEET_ACT_TYPE_MAX - info.count) * (info.finish_count[0]);
	m_role_module_mgr->NetSend(&info, len);
}
