#include "endlesstower.hpp"
#include "other/funopen/funopen.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/endlesstower/endlesstowerconfig.hpp"
#include "protocol/msgother.h"
#include "servercommon/errornum.h"
#include "battle/battle_business_local.hpp"
#include "gameworld/battle/battle_manager_local.hpp"
#include "item/knapsack.h"
#include "global/rank/rankmanager.hpp"
#include "gameworld/engineadapter.h"
#include "other/route/mailroute.hpp"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/logdef.h"

EndlessTower::EndlessTower()
{
}

EndlessTower::~EndlessTower()
{
}

void EndlessTower::Init(RoleModuleManager * module_mgr, const int & param, const unsigned int & battle_tamp_param)
{
	m_module_mgr = module_mgr;
	battle_seq = param;
	battle_tamp = battle_tamp_param;
}

void EndlessTower::GetInitParam(int * param, unsigned int * battle_tamp_param)
{
	*param = battle_seq;
	*battle_tamp_param = battle_tamp;
}

void EndlessTower::OnRoleLogin()
{
	//检查超过150关的一律重置回150
	//int old_battle_seq = battle_seq;
	if (m_module_mgr->GetCommonData().endless_refresh_flag == 0)
	{
		if (battle_seq > 150)
		{
			battle_seq = 150;
			battle_tamp = (unsigned int)EngineAdapter::Instance().Time();
		}
		m_module_mgr->GetCommonData().endless_refresh_flag = 1;
	}

	if(m_module_mgr->GetCommonData().endless_refresh_flag == 1)
	{
		if (battle_seq > 250)
		{
			battle_seq = 250;
			battle_tamp = (unsigned int)EngineAdapter::Instance().Time();
		}
		m_module_mgr->GetCommonData().endless_refresh_flag = 2;
	}
	if (battle_seq > 0)
	{
		if (battle_tamp == 0)battle_tamp = (unsigned int)EngineAdapter::Instance().Time();
		RankManager::Instance().SyncPersonRankInfo(m_module_mgr->GetRole(), PERSON_RANK_TYPE_ENDLESS_TOWER);
	}
	this->SendInfo();
}

void EndlessTower::OnBattleFinishWin(BattleData_Business * business_data, int round_num)
{
	if (NULL != business_data)
	{
		const EndlessTowerChallengeCfg * challenge_cfg = LOGIC_CONFIG->GetEndlessTowerConfig()->GetEndlessTowerChallengeCfg(business_data->param2);
		if (NULL == challenge_cfg)return;
		
		const EndlessTowerRewardCfg * reward_cfg = LOGIC_CONFIG->GetEndlessTowerConfig()->GetEndLessTowerRewardCfg(challenge_cfg->reward_id);
		if (NULL != reward_cfg)
		{
			std::vector<ItemConfigData> real_item_vec;
			real_item_vec.clear();
			const GodRewardCfg * god_cfg = LOGIC_CONFIG->GetEndlessTowerConfig()->GetGodRewardCfg(reward_cfg->god_reward_id);
			if (NULL != god_cfg)
			{
				real_item_vec.insert(real_item_vec.end(), god_cfg->god_reward.begin(), god_cfg->god_reward.end());
			}

			if ((int)reward_cfg->reward_vec.size() > 0)
			{
				real_item_vec.insert(real_item_vec.end(), reward_cfg->reward_vec.begin(), reward_cfg->reward_vec.end());
				if (!m_module_mgr->GetKnapsack()->PutList(real_item_vec.size(), &real_item_vec[0], PUT_REASON_ENDLESS_TOWER_REWARD, false))
				{
					//发邮件
					m_module_mgr->GetKnapsack()->SendMail(&real_item_vec[0], (int)real_item_vec.size(), SNED_MAIL_TYPE_DEFAULT, true);
				}
				m_module_mgr->GetKnapsack()->NotifyGetItemList((short)real_item_vec.size(), &real_item_vec[0], GET_REASON_ENDLESS_TOWER);
			}
		}

		battle_seq = business_data->param2;
		battle_tamp = (unsigned int)EngineAdapter::Instance().Time();
		this->SendInfo();
		RankManager::Instance().SyncPersonRankInfo(m_module_mgr->GetRole(), PERSON_RANK_TYPE_ENDLESS_TOWER);
	
		m_module_mgr->GetRole()->LogActive(LOG_ACTIVE_TYPE_ENDLESS_TOWER);
	}
}

void EndlessTower::OnOpreat(int req_type, int param1, int param2)
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_ENDLESS_TOWER, true))return;
	
	switch (req_type)
	{
	case Protocol::CSEndlessTowerReq::REQ_TYPE_INFO:
		this->SendInfo();
		break;
	case Protocol::CSEndlessTowerReq::REQ_TYPE_BATTLE:
		this->StarBattle();
		break;
	default:
		break;
	}
}

void EndlessTower::StarBattle()
{
	if (m_module_mgr->GetRole()->GetMyTeam() != NULL)
	{
		m_module_mgr->NoticeNum(errornum::EN_ROLE_IN_TEAM);
		return;
	}
	//获取配置
	const EndlessTowerChallengeCfg * challenge_cfg = LOGIC_CONFIG->GetEndlessTowerConfig()->GetEndlessTowerChallengeCfg(battle_seq + 1);
	if (NULL == challenge_cfg)return;
	
	if (BattleManagerLocal::GetInstance().StartBattleModeFightReq(m_module_mgr->GetRole(), BATTLE_MODE_ENDLESS_TOWER, challenge_cfg->monster_group, challenge_cfg->seq, 0, true))
	{
		return;
	}
}

void EndlessTower::SendInfo()
{
	Protocol::SCEndlessTowerInfo info;
	info.battle_seq = battle_seq;
	m_module_mgr->NetSend((const char*)&info, sizeof(info));
}
