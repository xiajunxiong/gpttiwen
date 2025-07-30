#include "roleactivityyunzewish.hpp"
#include "config/randactivityconfig/impl/randactivityyunzewishconfig.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "global/randactivity/randactivityimpl/randactivityyunzewish.hpp"

#include "servercommon/errornum.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "gamelog.h"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/uniqueidgenerator.hpp"

RoleActivityYunZeWish::RoleActivityYunZeWish(int type)
	:RoleActivity(type)
{

}

RoleActivityYunZeWish::~RoleActivityYunZeWish()
{

}

void RoleActivityYunZeWish::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityYunZeWish::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityYunZeWish::Update(time_t now_second)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUN_ZE_WISH))
	{
		return;
	}	

	RandActivityYunZeWish * rand_activity = (RandActivityYunZeWish *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_YUN_ZE_WISH);
	if (NULL == rand_activity) return;

	if (m_param.lottery_lock == 0 && m_param.is_can_lottery == 0 && m_param.end_lock_timestamp != 0 && m_param.end_lock_timestamp < (m_param.next_refresh_timestamp + 60) &&
		(m_param.next_refresh_timestamp <= now_second && (m_param.next_refresh_timestamp + 60) > now_second))
	{

		m_param.next_refresh_timestamp = rand_activity->GetNextTimeStamp();
		m_param.is_can_lottery = 1;
		m_param.is_send_timestamp = 0;
		
		this->RASendInfo();
		this->SetRoleDataChange();
	}

	if ((m_param.next_refresh_timestamp + 60) <= now_second)
	{
		m_param.next_refresh_timestamp = rand_activity->GetNextTimeStamp();
		if (m_param.is_can_lottery != 1)
		{
			m_param.is_can_lottery = 0;
		}
		m_param.lottery_lock = 0;
		m_param.is_send_timestamp = 0;

		this->RASendInfo();
		this->SetRoleDataChange();
	}

	if (m_param.is_send_timestamp == 0 &&(m_param.lottery_lock != 0 || m_param.is_can_lottery != 0 || m_param.end_lock_timestamp == 0) &&
		(m_param.next_refresh_timestamp < now_second && (m_param.next_refresh_timestamp + 60) > now_second))
	{
		m_param.is_send_timestamp = 1;
		m_param.lottery_lock = 0;

		this->RASendInfo();
	}

}

void RoleActivityYunZeWish::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUN_ZE_WISH))
	{
		return;
	}

	this->RASendInfo();
}

void RoleActivityYunZeWish::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_YUN_ZE_WISH != rand_act_type)
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

		RandActivityYunZeWish * rand_activity = (RandActivityYunZeWish *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_YUN_ZE_WISH);
		if (NULL == rand_activity) return;

		m_param.next_refresh_timestamp = rand_activity->GetNextTimeStamp();

		this->RASendInfo();
		this->SetRoleDataChange();
	}
}

void RoleActivityYunZeWish::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUN_ZE_WISH))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_YUN_ZE_WISH);

	switch (raor->opera_type)
	{
	case 0:
		{
			this->RASendInfo();					// 发协议
		}
		break;
	case 1:
		{
			this->LockReward(raor->param_1);	// 锁定奖池，param1--奖励组seq
		}
		break;
	case 2:
		{
			//this->UnlockReward(raor->param_1);	// 解锁奖池，param1--奖励组seq
		}
		break;
	case 3:
		{
			this->Lottery();					 // 抽奖
		}
		break;
	default:
		break;
	}

}

void RoleActivityYunZeWish::RASendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	RandActivityYunZeWish * rand_activity = (RandActivityYunZeWish *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_YUN_ZE_WISH);
	if (NULL == rand_activity) return;

	Protocol::SCRAYunZeWishInfo msg;
	msg.next_refresh_timestamp = rand_activity->GetNextTimeStamp();
	if (msg.next_refresh_timestamp >= (unsigned int)ramgr->GetRandActivityEndTime(RAND_ACTIVITY_TYPE_YUN_ZE_WISH))
	{
		msg.next_refresh_timestamp = 0;
	}

	msg.is_can_lottery = m_param.is_can_lottery;
	msg.is_lock_reward = m_param.is_lock_reward;

	m_role_module_mgr->NetSend(&msg,sizeof(msg));
}

void RoleActivityYunZeWish::LockReward(int reward_seq)
{
	if (reward_seq < 0 || reward_seq >= YZ_WISH_MAX_REWARD_NUM)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	const RandActivityYunZeWishConfig * config = static_cast<const RandActivityYunZeWishConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUN_ZE_WISH));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RAYZWishRewardSectionCfg * reward_cfg = config->GetRewardCfg(ramgr);
	if (NULL == reward_cfg) return;

	{
		int i = 0;
		for (; i < (int)reward_cfg->reward_list.size(); ++i)
		{
			if (reward_cfg->reward_list[i].seq == reward_seq)  break;
		}

		if (i >= (int)reward_cfg->reward_list.size())
		{
			return;
		}
	}

	if (m_param.is_lock_reward.IsBitSet(reward_seq))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_CAN_NOT_REPEAT_LOCK);
		return;
	}

	{
		int lock_num = 0;
		for (int j = 0; j < YZ_WISH_MAX_REWARD_NUM; ++j)
		{
			if (m_param.is_lock_reward.IsBitSet(j))
			{
				lock_num++;
			}
		}

		if (lock_num >= config->GetOtherCfg().reward_choice)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_LOCK_UPPER_LIMIT);
			return;
		}
	}

	if (!m_param.is_lock_reward.SetBit(reward_seq))
	{
		return;
	}

	int mp_lock_num = 0;
	for (int i = 0; i < YZ_WISH_MAX_REWARD_NUM; i++)
	{
		if (m_param.is_lock_reward.IsBitSet(i))
		{
			mp_lock_num++;
		}
	}

	if (mp_lock_num == config->GetOtherCfg().reward_choice)
	{
		m_param.end_lock_timestamp = EngineAdapter::Instance().Time();
	}

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityYunZeWish::UnlockReward(int reward_seq)
{
	if (reward_seq < 0 || reward_seq >= YZ_WISH_MAX_REWARD_NUM)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	const RandActivityYunZeWishConfig * config = static_cast<const RandActivityYunZeWishConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUN_ZE_WISH));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RAYZWishRewardSectionCfg * reward_cfg = config->GetRewardCfg(ramgr);
	if (NULL == reward_cfg) return;

	{
		int i = 0;
		for (; i < (int)reward_cfg->reward_list.size(); ++i)
		{
			if (reward_cfg->reward_list[i].seq == reward_seq)  break;
		}

		if (i >= (int)reward_cfg->reward_list.size())
		{
			return;
		}
	}

	if (!m_param.is_lock_reward.IsBitSet(reward_seq))
	{
		return;
	}

	if (!m_param.is_lock_reward.UnSetBit(reward_seq))
	{
		return;
	}

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityYunZeWish::Lottery()
{
	if (m_param.is_can_lottery != 1 && m_param.lottery_lock == 1) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityYunZeWishConfig * config = static_cast<const RandActivityYunZeWishConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUN_ZE_WISH));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	int lock_num = 0;
	for (int j = 0; j < YZ_WISH_MAX_REWARD_NUM; ++j)
	{
		if (m_param.is_lock_reward.IsBitSet(j))
		{
			lock_num++;
		}
	}

	if (lock_num != config->GetOtherCfg().reward_choice)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_PLEASE_LOCK_ALL_REWARD);
		return;
	}

	const RAYZWishRewardSectionCfg * reward_cfg = config->GetRewardCfg(ramgr);
	if (NULL == reward_cfg) return;

	std::vector<RAYZWishRewardCfg> reward_list;
	std::vector<int> respective_rate;

	int total_rate = 0;
	for (int i = 0; i < YZ_WISH_MAX_REWARD_NUM; i++)
	{
		if (m_param.is_lock_reward.IsBitSet(i))
		{
			reward_list.push_back(reward_cfg->reward_list[i]);
			respective_rate.push_back(reward_cfg->reward_list[i].rate);
			total_rate += reward_cfg->reward_list[i].rate;
			reward_list[reward_list.size() - 1].rate = total_rate;
		}
	}

	if (reward_list.size() != config->GetOtherCfg().reward_choice)
	{
		return;
	}

	std::vector<ItemConfigData> finally_reward;
	for (int i = 0; i < config->GetOtherCfg().reward_get; i++)
	{
		int rand_rate = RandomNum(total_rate);

		int j = 0;
		for (; j < (int)reward_list.size(); ++j)
		{
			if (reward_list[j].rate > rand_rate)
			{
				finally_reward.push_back(reward_list[j].reward);
				break;
			}
		}

		if (j == (int)reward_list.size()) return;

		reward_list.erase(reward_list.begin() + j);
		respective_rate.erase(respective_rate.begin() + j);

		for (int y = 0; y < (int)reward_list.size(); ++y)
		{
			for (int x = 0; x <= y; x++)
			{
				reward_list[y].rate += respective_rate[x];
			}			
		}

		total_rate = reward_list[reward_list.size()-1].rate;
	}

	if (finally_reward.empty()) return;

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)finally_reward.size(), &finally_reward[0]))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)finally_reward.size(), &finally_reward[0], PUT_REASON_RA_YUNZE_WISH);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&finally_reward[0], (int)finally_reward.size(), SEND_MAIL_TYPE_YUNZE_WISH, true);
	}

	m_param.is_can_lottery = 0;		// 消耗标记
	m_param.end_lock_timestamp = 0;
	m_param.lottery_lock = 1;
	m_param.is_lock_reward.Reset();


	std::string gamelog_str;
	for (int i = 0; i < (int)finally_reward.size(); ++i)
	{
		gamelog_str = STRING_SPRINTF("{item[%d, %d]} ", finally_reward[i].item_id,finally_reward[i].num);
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityYunZeWish::%s role[%d, %s] get_reward[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), gamelog_str.c_str());

	this->RASendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);
}
