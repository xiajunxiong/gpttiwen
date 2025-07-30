#include "roleactivityxinfuqianghua.hpp"
#include "config/randactivityconfig/impl/randactivityxinfuqianghuaconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "protocol/randactivity/msgraxinfuqianghua.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "engineadapter.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "servercommon/errornum.h"

RoleActivityXinFuQiangHua::RoleActivityXinFuQiangHua(int type)
	:RoleActivity(type)
{
}

RoleActivityXinFuQiangHua::~RoleActivityXinFuQiangHua()
{
}

void RoleActivityXinFuQiangHua::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAXinFuQiangHuaParam));
}

void RoleActivityXinFuQiangHua::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAXinFuQiangHuaParam));
}

void RoleActivityXinFuQiangHua::CheckInitRandActivity(int rand_act_type, bool is_force)
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

		this->RASendInfo();
		this->SetRoleDataChange();
	}
}

void RoleActivityXinFuQiangHua::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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
	case 0:
		{
			this->RASendInfo();
		}
		break;
	case 1:		// 领取每日免费奖励
		{
			this->ReceiveFreeReward();
		}
		break;
	case 2:		// 领取强化奖励 p1-宠物品质, p2-索引(配置表seq1)
		{
			this->ReceiveStrengthenReward(raor->param_1, raor->param_2);
		}
		break;
	default:
		break;
	}
}

void RoleActivityXinFuQiangHua::OnLogin()
{
	this->RASendInfo();
}

void RoleActivityXinFuQiangHua::OnLoginCheck()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	if (!ramgr->IsRandActivityOpen(m_role_activity_type) && m_param.ra_begin_timestamp != 0 && (unsigned int)EngineAdapter::Instance().Time() > m_param.ra_begin_timestamp)
	{
		this->ReissueAllReward();
	}
}

void RoleActivityXinFuQiangHua::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	m_param.DayReset();
	this->RASendInfo();
}

void RoleActivityXinFuQiangHua::StrengthenTaskFinish(int pet_quality, int str_level)
{
	if(pet_quality <= PET_QUALITY_INVALID || pet_quality >= PET_QUALITY_MAX || pet_quality >= MAX_PET_QUALITY_NUM)return;

	const RandActivityXinFuQiangHuaConfig * config = static_cast<const RandActivityXinFuQiangHuaConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	int seq = -1;
	const std::vector<ItemConfigData>* reward_list = config->GetRewardCfg(pet_quality, str_level, &seq);
	if (seq < 0 || seq >= PET_STRENGTHEN_MAX_LEVEL || NULL == reward_list) return;

	if (m_param.task_finish[pet_quality - 1].IsBitSet(seq)) return;

	m_param.task_finish[pet_quality - 1].SetBit(seq);
	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityXinFuQiangHua::ReceiveFreeReward()
{
	if (m_param.free_reward_flag == 1) return;

	const RandActivityXinFuQiangHuaConfig * config = static_cast<const RandActivityXinFuQiangHuaConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(config->GetOtherCfg().free_reward_list))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->PutVec(config->GetOtherCfg().free_reward_list, PUT_REASON_RA_XIN_FU_QIANG_HUA))
	{
		m_role_module_mgr->GetKnapsack()->SendMail(config->GetOtherCfg().free_reward_list, SEND_MAIL_TYPE_ACTIVITY_REWARD_BAG_FULL, false, RAND_ACTIVITY_TYPE_XIN_FU_QIANG_HUA);
	}

	m_param.free_reward_flag = 1;
	this->RASendInfo();
	this->SetRoleDataChange();

	{	//  记录日志
		std::string log_str;
		for (size_t i = 0; i < config->GetOtherCfg().free_reward_list.size(); i++)
		{
			log_str += STRING_SPRINTF("[%d, %d] ", config->GetOtherCfg().free_reward_list[i].item_id, config->GetOtherCfg().free_reward_list[i].num);
		}

		gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] free_reward_flag[%d] reward_list[%s]", __FUNCTION__,
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), m_param.free_reward_flag, log_str.c_str());
	}
}

void RoleActivityXinFuQiangHua::ReceiveStrengthenReward(int pet_quality, int seq)
{
	if (pet_quality <= PET_QUALITY_INVALID || pet_quality >= PET_QUALITY_MAX || pet_quality >= MAX_PET_QUALITY_NUM)return;
	if (seq < 0 || seq >= PET_STRENGTHEN_MAX_LEVEL) return;

	if (!m_param.task_finish[pet_quality - 1].IsBitSet(seq))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_NETHER_TASK_IS_NOT_COMPLETED);
		return;
	}

	if (m_param.reward_receive[pet_quality - 1].IsBitSet(seq))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	const RandActivityXinFuQiangHuaConfig * config = static_cast<const RandActivityXinFuQiangHuaConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	if (!config->IsOpen(pet_quality, m_role_module_mgr->GetRole()->GetLevel())) return;

	int st_level = config->GetStLevelBySeq(pet_quality, seq);
	if (st_level <= 0 || st_level > PET_STRENGTHEN_MAX_LEVEL) return;

	const std::vector<ItemConfigData>* reward_list = config->GetRewardCfg(pet_quality, st_level);
	if (NULL == reward_list) return;

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(*reward_list))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(*reward_list, PUT_REASON_RA_XIN_FU_QIANG_HUA);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(*reward_list, SEND_MAIL_TYPE_ACTIVITY_REWARD_BAG_FULL, false, RAND_ACTIVITY_TYPE_XIN_FU_QIANG_HUA);
	}
	
	m_param.reward_receive[pet_quality - 1].SetBit(seq);

	this->RASendInfo();
	this->SetRoleDataChange();

	{	//  记录日志
		std::string log_str;
		for (size_t i = 0; i < reward_list->size(); i++)
		{
			log_str += STRING_SPRINTF("[%d, %d] ", (*reward_list)[i].item_id, (*reward_list)[i].num);
		}

		gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] receive[%d][%d] reward_list[%s]", __FUNCTION__,
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), pet_quality - 1, seq, log_str.c_str());
	}
}

void RoleActivityXinFuQiangHua::ReissueAllReward()
{
	const RandActivityXinFuQiangHuaConfig * config = static_cast<const RandActivityXinFuQiangHuaConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	std::vector<ItemConfigData> all_reward_list;
	for (size_t i = 0; i < MAX_PET_QUALITY_NUM; i++)
	{
		if(!config->IsOpen(i + 1, m_role_module_mgr->GetRole()->GetLevel())) continue;

		for (size_t j = 0; j < MAX_XIN_FU_QIANG_HUA_BITMAP; j++)
		{
			if (!m_param.task_finish[i].IsBitSet(j)) continue;
			if (m_param.reward_receive[i].IsBitSet(j))continue;

			int st_level = config->GetStLevelBySeq(i + 1, j);
			if (st_level <= 0 || st_level > PET_STRENGTHEN_MAX_LEVEL) continue;

			const std::vector<ItemConfigData>* reward_list = config->GetRewardCfg(i + 1, st_level);
			if (NULL == reward_list) continue;

			all_reward_list.insert(all_reward_list.end(), reward_list->begin(), reward_list->end());
			m_param.reward_receive[i].SetBit(j);
		}
	}
	if (all_reward_list.empty()) return;

	m_role_module_mgr->GetKnapsack()->SendMail(all_reward_list, SEND_MAIL_TYPE_ACTIVITY_REWARD_BAG_FULL, false, RAND_ACTIVITY_TYPE_XIN_FU_QIANG_HUA);

	{	//  记录日志
		std::string log_str;
		for (size_t i = 0; i < all_reward_list.size(); i++)
		{
			log_str += STRING_SPRINTF("[%d, %d] ", all_reward_list[i].item_id, all_reward_list[i].num);
		}

		gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] all_reward_list[%s]", __FUNCTION__,
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), log_str.c_str());
	}

	this->SetRoleDataChange();
}

void RoleActivityXinFuQiangHua::RASendInfo()
{
	Protocol::SCRAXinFuQiangHua msg;
	msg.free_reward_flag = m_param.free_reward_flag;
	memcpy(msg.task_finish, m_param.task_finish, sizeof(msg.task_finish));
	memcpy(msg.reward_receive, m_param.reward_receive, sizeof(msg.reward_receive));

	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}
