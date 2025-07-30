#include "netherworldpurgatory.hpp"
#include "other/funopen/funopen.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/netherworldpurgatory/netherworldpurgatoryconfig.hpp"
#include "protocol/msgother.h"
#include "scene/scene.h"
#include "scene/speciallogic.hpp"
#include "servercommon/errornum.h"
#include "global/team/team.hpp"
#include "battle/battle_business_local.hpp"
#include "gameworld/battle/battle_manager_local.hpp"
#include "item/knapsack.h"
#include "global/rank/rankmanager.hpp"
#include "other/activesystem/activesystem.hpp"
#include "gameworld/engineadapter.h"
#include "other/route/mailroute.hpp"
#include "other/event/eventhandler.hpp"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/logdef.h"
#include "global/worldstatus/worldstatus.hpp"

NetherWorldPurgatory::NetherWorldPurgatory() : send_check(false)
{
}

NetherWorldPurgatory::~NetherWorldPurgatory()
{
}

void NetherWorldPurgatory::Init(RoleModuleManager * module_mgr, const NetherWorldGatoryParam & param)
{
	m_module_mgr = module_mgr;
	m_param = param;
}

void NetherWorldPurgatory::OnRoleUplevel()
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_NETHER_WORLD_PURGATORY))return;
	if (!send_check)this->SendAllInfo();
}

void NetherWorldPurgatory::GetInitParam(NetherWorldGatoryParam * param)
{
	*param = m_param;
}

void NetherWorldPurgatory::OnRoleLogin()
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_NETHER_WORLD_PURGATORY))return;
	if (!send_check)this->SendAllInfo();
	if(this->GetRewardNum() > 0)RankManager::Instance().SyncPersonRankInfo(m_module_mgr->GetRole(), PERSON_RANK_TYPE_NEHER_WORLD_PURGATORY);
}

void NetherWorldPurgatory::OnDayChange()
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_NETHER_WORLD_PURGATORY))return;
	m_param.today_pass_times = 0;
	if (m_param.next_day_buff_up != 0)
	{
		const NetherWorldPurgatoryOtherCfg & other_cfg = LOGIC_CONFIG->GetNetherWorldPurgatoryConfig()->GetOtherCfg();
		if (m_param.buff_level < other_cfg.debuff_level)
		{
			m_param.buff_level += 1;
			m_param.next_day_buff_up = 0;
			}
						}
	this->SendInfo();
}

void NetherWorldPurgatory::OnBattleFinishWin(BattleData_Business * business_data, int round_num)
{
	if (business_data == NULL)return;
	if (business_data->param3 - 1 < 0 && business_data->param3 - 1 >= MAX_NETHER_WORLD_GATORY_INDEX)return;
	if (business_data->param2 - 1 < 0 && business_data->param2 - 1 >= MAX_NETHER_WORLD_GATORY_CHAPTER_NUM)return;
	NetherWorldGatoryChapters &chapter = m_param.chapter_list[business_data->param2 - 1];
	NetherWorldGatoryLevel &level = chapter.challenge_info[business_data->param3 - 1];
	bool has_new_reward = false;
	for (int i = 0; i < business_data->condition_num && i < BATTLE_CONDITION_MAX_NUM && i < MAX_NETHER_WORLD_GATORY_TASK_NUM; i++)
	{
		if ((business_data->condition_flag & (1 << i)) > 0)
		{
			if (!level.reward_map.IsBitSet(i + 1) && !level.pass_map.IsBitSet(i + 1))
			{
				level.pass_map.SetBit(i + 1);
				has_new_reward = true;
			}
		}
	}

	if (round_num > 0)
	{
		if (level.round_num <= 0 || level.round_num < round_num)
		{
			level.round_num = round_num;
		}
	}

	if (business_data->param2 > m_param.pass_challenge_chapters)
	{
		if (business_data->param2 == m_param.pass_challenge_chapters + 1)
		{
			m_param.pass_challenge_chapters += 1;
			m_param.pass_challenge_id = 1;
			m_param.buff_level = 0;
			m_param.next_day_buff_up = 0;
		}
	}
	else if (business_data->param2 == m_param.pass_challenge_chapters)
	{
		if (business_data->param3 == m_param.pass_challenge_id + 1)
		{
			m_param.pass_challenge_id += 1;
			m_param.buff_level = 0;
			m_param.next_day_buff_up = 0;
		}
	}

	if (has_new_reward)
	{
		m_param.today_pass_times += 1;
	}
	
	Protocol::SCNetherWorldPurgatorySigleInfo info;
	info.challenge_info = level;
	info.send_reason = Protocol::SCNetherWorldPurgatorySigleInfo::SEND_REASON_FINISH_BATTLE;
	info.pass_challenge_chapters = business_data->param2;
	info.pass_challenge_id = business_data->param3;
	info.new_pass_flag = business_data->condition_flag;
	info.today_play_times = m_param.today_pass_times;
	m_module_mgr->NetSend((const char*)&info, sizeof(info));
	
	RankManager::Instance().SyncPersonRankInfo(m_module_mgr->GetRole(), PERSON_RANK_TYPE_NEHER_WORLD_PURGATORY);
	EventHandler::Instance().OnPassNetherWorldPurgatory(m_module_mgr->GetRole(), business_data->param2, business_data->param3);
	m_module_mgr->GetActiveSystem()->AddActiveDegree(ACTIVE_SYSTEM_TYPE_NETHER_WORLD_PURGATORY, 1, __FUNCTION__);

	m_module_mgr->GetRole()->LogActive(LOG_ACTIVE_TYPE_WORLD_PURGATORY);
}

void NetherWorldPurgatory::OnBattleFinishLose(BattleData_Business * business_data)
{
	if (business_data == NULL)return;
	if (business_data->param2 > m_param.pass_challenge_chapters)
	{
		if (business_data->param2 == m_param.pass_challenge_chapters + 1)
		{
			m_param.next_day_buff_up = 1;
		}
	}
	else if (business_data->param2 == m_param.pass_challenge_chapters)
	{
		if (business_data->param3 == m_param.pass_challenge_id + 1)m_param.next_day_buff_up = 1;
	}
}

void NetherWorldPurgatory::OnNetherWorldPurgatoryOp(int req_type, int param1, int param2, int param3)
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_NETHER_WORLD_PURGATORY))return;
	switch (req_type)
	{
	case Protocol::CSNetherWorldPurgatoryReq::REQ_OP_TYPE_0:
		this->SendAllInfo();
		break;
	case Protocol::CSNetherWorldPurgatoryReq::REQ_OP_TYPE_1:
		this->StarBattle(param1, param2);
		break;
	case Protocol::CSNetherWorldPurgatoryReq::REQ_OP_TYPE_2:
		this->GetChallengeReward(param1, param2, param3);
		break;
	case Protocol::CSNetherWorldPurgatoryReq::REQ_OP_TYPE_3:
		this->GetMiLingReward(param1, param2, param3);
		break;
	case Protocol::CSNetherWorldPurgatoryReq::REQ_OP_TYPE_4:
		this->SetVideoShow();
		break;
	default:
		break;
	}
}

int NetherWorldPurgatory::GetRewardNum()
{
	int reward_num = 0;

	for (int chapter_id = 0; chapter_id < MAX_NETHER_WORLD_GATORY_CHAPTER_NUM; chapter_id++)
	{
		NetherWorldGatoryChapters & chapter = m_param.chapter_list[chapter_id];
		for (int i = 0; i < MAX_NETHER_WORLD_GATORY_INDEX; i++)
		{
			NetherWorldGatoryLevel & level = chapter.challenge_info[i];
		for (int j = 0; j < MAX_NETHER_WORLD_GATORY_TASK_NUM; j++)
		{
			if (level.pass_map.IsBitSet(j + 1))
			{
				reward_num += 1;
			}
		}
	}
	}

	return reward_num;
}

int NetherWorldPurgatory::GetMaxRoundNum(int chapter_id)
{
	if (chapter_id - 1 < 0 || chapter_id - 1 >= MAX_NETHER_WORLD_GATORY_CHAPTER_NUM)return 0;
	NetherWorldGatoryChapters & chapter = m_param.chapter_list[chapter_id - 1];
	int round_num = 0;
	for (int i = 0; i < MAX_NETHER_WORLD_GATORY_INDEX; i++)
	{
		NetherWorldGatoryLevel & level = chapter.challenge_info[i];
		round_num += level.round_num;
	}
	return round_num;
}

void NetherWorldPurgatory::StarBattle(int chapter_id, int challenge_id)
{
	if (!m_module_mgr->GetRole()->GetScene()->GetSpecialLogic()->CanJoinOtherActivity())
	{
		m_module_mgr->NoticeNum(errornum::EN_PLAY_ACTIVITY_NOW_ERR);
		return;
	}
	const NetherWorldPurgatoryOtherCfg & other_cfg = LOGIC_CONFIG->GetNetherWorldPurgatoryConfig()->GetOtherCfg();
	if (m_param.today_pass_times >= other_cfg.times)
	{
		m_module_mgr->NoticeNum(errornum::EN_NETHER_TODAY_TIMES_LIMIT);
		return;
	}

	if (m_module_mgr->GetRole()->InTeam() && m_module_mgr->GetRole()->GetMyTeam()->IsRoleMemberFollow(m_module_mgr->GetUid()))
	{
		m_module_mgr->NoticeNum(errornum::EN_ROLE_IN_TEAM);
		return;
	}

	const NetherWorldPurgatoryMonsterPassRewardCfg * challenge_cfg = LOGIC_CONFIG->GetNetherWorldPurgatoryConfig()->PassRewardCfg(chapter_id, challenge_id);
	if (challenge_cfg == NULL)return;

	if (m_param.pass_challenge_chapters < challenge_cfg->unlock_chapter ||
		(m_param.pass_challenge_chapters == challenge_cfg->unlock_chapter
			&& m_param.pass_challenge_id < challenge_cfg->unlock_challenge))
	{
		m_module_mgr->NoticeNum(errornum::EN_FB_NOT_PASS);
		return;
	}
	if (challenge_id - 1 < 0 || challenge_id - 1 >= MAX_NETHER_WORLD_GATORY_INDEX)return;
	if (chapter_id - 1 < 0 || chapter_id - 1 >= MAX_NETHER_WORLD_GATORY_INDEX)return;

	if (BattleManagerLocal::GetInstance().StartBattleModeFightReq(m_module_mgr->GetRole(), BATTLE_MODE_NETHER_WORLD_PURGATORY, challenge_cfg->monster_group.monster_group_id, chapter_id, challenge_id, true))
	{
		return;
	}
}

void NetherWorldPurgatory::GetChallengeReward(int chapter_id, int challenge_id, int task_id)
{
	const NetherWorldPurgatoryMonsterPassRewardCfg * pass_reward_cfg = LOGIC_CONFIG->GetNetherWorldPurgatoryConfig()->PassRewardCfg(chapter_id, challenge_id);
	if (pass_reward_cfg == NULL)return;

	if (m_param.pass_challenge_chapters < pass_reward_cfg->unlock_chapter ||
		(m_param.pass_challenge_chapters == pass_reward_cfg->unlock_chapter
			&& m_param.pass_challenge_id < pass_reward_cfg->unlock_challenge))
	{
		m_module_mgr->NoticeNum(errornum::EN_FB_NOT_PASS);
		return;
	}
	if (chapter_id - 1 < 0 || chapter_id - 1 >= MAX_NETHER_WORLD_GATORY_CHAPTER_NUM)return;
	if (challenge_id - 1 < 0 || challenge_id - 1 >= MAX_NETHER_WORLD_GATORY_INDEX)return;
	if (task_id < 0 || task_id >= MAX_NETHER_WORLD_GATORY_TASK_NUM)return;

	NetherWorldGatoryLevel &level = m_param.chapter_list[chapter_id - 1].challenge_info[challenge_id - 1];

	if (task_id != 0 && !level.pass_map.IsBitSet(task_id))
	{
		m_module_mgr->NoticeNum(errornum::EN_NETHER_TASK_IS_NOT_COMPLETED);
		return;
	}

	if (task_id != 0 && level.reward_map.IsBitSet(task_id))
	{
		m_module_mgr->NoticeNum(errornum::EN_NETHER_TASK_REWARD_HAS_GET);
		return;
	}

	{
		switch (task_id)
		{
		case 0://最终奖励
		{
			if (!level.pass_map.IsBitSet(1))//第一个任务的通关
			{
				m_module_mgr->NoticeNum(errornum::EN_NETHER_TASK_IS_NOT_COMPLETED);
				return;
			}

			if (level.reward_map.IsBitSet(0))//最终奖励
			{
				m_module_mgr->NoticeNum(errornum::EN_NETHER_TASK_REWARD_HAS_GET);
				return;
			}
			if (pass_reward_cfg->final_reward.size() <= 0)return;
			if (!m_module_mgr->GetKnapsack()->CheckForPutList((short)pass_reward_cfg->final_reward.size(), &pass_reward_cfg->final_reward[0]))
			{
				return;
			}
			m_module_mgr->GetKnapsack()->PutList((short)pass_reward_cfg->final_reward.size(), &pass_reward_cfg->final_reward[0], PUT_REASON_NETHER_WORLD_PURGATORY_REWARD);
		}
		break;
		case 1:
		{
			m_param.low_key += 1;
			const NetherWorldPurgatoryMonsterCfg * condition_reward_cfg = LOGIC_CONFIG->GetNetherWorldPurgatoryConfig()->GetMonsterConditionCfg(chapter_id, challenge_id, pass_reward_cfg->monster_group.monster_group_id);
			if (NULL != condition_reward_cfg)
			{
				if ((short)condition_reward_cfg->condition1_reward.size() > 0 && m_module_mgr->GetKnapsack()->CheckForPutList((short)condition_reward_cfg->condition1_reward.size(), &condition_reward_cfg->condition1_reward[0]))
				{
					m_module_mgr->GetKnapsack()->PutList((short)condition_reward_cfg->condition1_reward.size(), &condition_reward_cfg->condition1_reward[0], PUT_REASON_NETHER_WORLD_PURGATORY_REWARD);
				}
			}
		}
		break;
		case 2:
		{
			m_param.mid_key += 1;
			const NetherWorldPurgatoryMonsterCfg * condition_reward_cfg = LOGIC_CONFIG->GetNetherWorldPurgatoryConfig()->GetMonsterConditionCfg(chapter_id, challenge_id, pass_reward_cfg->monster_group.monster_group_id);
			if (NULL != condition_reward_cfg)
				if ((short)condition_reward_cfg->condition2_reward.size() > 0 && m_module_mgr->GetKnapsack()->CheckForPutList((short)condition_reward_cfg->condition2_reward.size(), &condition_reward_cfg->condition2_reward[0]))
				{
					m_module_mgr->GetKnapsack()->PutList((short)condition_reward_cfg->condition2_reward.size(), &condition_reward_cfg->condition2_reward[0], PUT_REASON_NETHER_WORLD_PURGATORY_REWARD);
				}
		}
		break;
		case 3:
		{
			m_param.high_key += 1;
			const NetherWorldPurgatoryMonsterCfg * condition_reward_cfg = LOGIC_CONFIG->GetNetherWorldPurgatoryConfig()->GetMonsterConditionCfg(chapter_id, challenge_id, pass_reward_cfg->monster_group.monster_group_id);
			if (NULL != condition_reward_cfg)
				if ((short)condition_reward_cfg->condition3_reward.size() > 0 && m_module_mgr->GetKnapsack()->CheckForPutList((short)condition_reward_cfg->condition3_reward.size(), &condition_reward_cfg->condition3_reward[0]))
				{
					m_module_mgr->GetKnapsack()->PutList((short)condition_reward_cfg->condition3_reward.size(), &condition_reward_cfg->condition3_reward[0], PUT_REASON_NETHER_WORLD_PURGATORY_REWARD);
				}
		}
		break;
		default:
			break;
		}
	}

	level.reward_map.SetBit(task_id);
	this->SendSigleChallengeInfo(chapter_id, challenge_id);
	this->SendMiLingInfo();
}

void NetherWorldPurgatory::GetMiLingReward(int chapter_id, int miling_type, int times)
{
	if (chapter_id - 1 < 0 || chapter_id - 1 >= MAX_NETHER_WORLD_GATORY_CHAPTER_NUM)return;
	NetherWorldGatoryChapters &chapther = m_param.chapter_list[chapter_id - 1];
	switch (miling_type)
	{
	case 1:
		if (m_param.low_key < times)
		{
			return;
		}
		break;
	case 2:
	{
		if (m_param.mid_key < times)
		{
			m_module_mgr->NoticeNum(errornum::EN_NETHER_WORLD_PURGATORY_MILING_NO_TIMES);
			return;
		}
		const NetherWorldPurgatoryRewardListCfg * reward_cfg = LOGIC_CONFIG->GetNetherWorldPurgatoryConfig()->GetRewardCfg(chapter_id);
		if (reward_cfg == NULL)return;
		std::vector<NetherWorldPurgatoryRewardCfg> real_silver_vec;
		for (int i = 0; i < MAX_NETHER_WORLD_GATORY_INDEX; i++)
		{
			if (chapther.silver_reward_flag.IsBitSet(i) || reward_cfg->silver_vec[i].lattice <= 0)continue;
			real_silver_vec.push_back(reward_cfg->silver_vec[i]);
		}

		for (int i = 0; i < times && i < MAX_NETHER_WORLD_GATORY_INDEX && m_param.mid_key > 0; i++)
		{
			if (real_silver_vec.size() > 0)
			{
				int rand_idx = rand() % real_silver_vec.size();
				NetherWorldPurgatoryRewardCfg &node_cfg = real_silver_vec[rand_idx];
				if (node_cfg.lattice - 1 < 0 || node_cfg.lattice - 1 >= MAX_NETHER_WORLD_GATORY_INDEX)continue;
				if (!m_module_mgr->GetKnapsack()->Put(node_cfg.reward[0], PUT_REASON_NETHER_WORLD_PURGATORY_REWARD))
				{
					return;
				}
				chapther.silver_reward_flag.SetBit(node_cfg.lattice - 1);
				m_param.mid_key -= 1;
				real_silver_vec.erase(real_silver_vec.begin() + rand_idx);
			}
		}
	}
	break;
	case 3:
	{
		if (m_param.high_key < times)
		{
			m_module_mgr->NoticeNum(errornum::EN_NETHER_WORLD_PURGATORY_MILING_NO_TIMES);
			return;
		}
		const NetherWorldPurgatoryRewardListCfg * reward_cfg = LOGIC_CONFIG->GetNetherWorldPurgatoryConfig()->GetRewardCfg(chapter_id);
		if (reward_cfg == NULL)return;
		std::vector<NetherWorldPurgatoryRewardCfg> real_gold_vec;
		for (int i = 0; i < MAX_NETHER_WORLD_GATORY_INDEX; i++)
		{
			if (chapther.gold_reward_flag.IsBitSet(i) || reward_cfg->gold_vec[i].lattice <= 0)continue;
			real_gold_vec.push_back(reward_cfg->gold_vec[i]);
		}

		for (int i = 0; i < times && i < MAX_NETHER_WORLD_GATORY_INDEX && m_param.high_key > 0; i++)
		{
			if (real_gold_vec.size() > 0)
			{
				int rand_idx = rand() % real_gold_vec.size();
				NetherWorldPurgatoryRewardCfg &node_cfg = real_gold_vec[rand_idx];
				if (node_cfg.lattice - 1 < 0 || node_cfg.lattice - 1 >= MAX_NETHER_WORLD_GATORY_INDEX)continue;
				if (!m_module_mgr->GetKnapsack()->Put(node_cfg.reward[0], PUT_REASON_NETHER_WORLD_PURGATORY_REWARD))
				{
					return;
				}
				chapther.gold_reward_flag.SetBit(node_cfg.lattice - 1);
				m_param.high_key -= 1;
				real_gold_vec.erase(real_gold_vec.begin() + rand_idx);
			}
		}
	}
	break;
	default:
		return;
	}

	this->SendMiLingInfo();
}

void NetherWorldPurgatory::SetVideoShow()
{
	m_param.show_video = 1;
	this->SendInfo();
}

void NetherWorldPurgatory::SendAllInfo()
{
	this->SendInfo();
	for (int i = 0; i < MAX_NETHER_WORLD_GATORY_CHAPTER_NUM; i++)
	{
		this->SendChapterInfo(i);
	}
	this->SendMiLingInfo();
	send_check = true;
}

void NetherWorldPurgatory::SendInfo()
{
	Protocol::SCNetherWorldPurgatoryInfo info;
	info.low_key = m_param.low_key;
	info.mid_key = m_param.mid_key;
	info.high_key = m_param.high_key;
	info.show_video = m_param.show_video;
	info.pass_challenge_id = m_param.pass_challenge_id;
	info.pass_challenge_chapters = m_param.pass_challenge_chapters;
	info.today_pass_times = m_param.today_pass_times;

	m_module_mgr->NetSend((const char*)&info, sizeof(info));
}

void NetherWorldPurgatory::SendChapterInfo(int chapter_id)
{
	if (chapter_id < 0 || chapter_id >= MAX_NETHER_WORLD_GATORY_CHAPTER_NUM)return;
	Protocol::SCNetherWorldPurgatoryChapterInfo info;
	info.chapter_id = chapter_id;
	memcpy(&info.chapter_list, &m_param.chapter_list[chapter_id], sizeof(NetherWorldGatoryChapters));
	m_module_mgr->NetSend((const char*)&info, sizeof(info));
}

void NetherWorldPurgatory::SendSigleChallengeInfo(int chapter_id, int challenge_id)
{
	if (chapter_id <= 0 || chapter_id > MAX_NETHER_WORLD_GATORY_CHAPTER_NUM)return;
	if (challenge_id <= 0 || challenge_id > MAX_NETHER_WORLD_GATORY_INDEX)return;
	Protocol::SCNetherWorldPurgatorySigleInfo info;
	info.challenge_info = m_param.chapter_list[chapter_id - 1].challenge_info[challenge_id - 1];
	info.send_reason = Protocol::SCNetherWorldPurgatorySigleInfo::SEND_REASON_GET_REWARD;
	info.pass_challenge_id = challenge_id;
	info.pass_challenge_chapters = chapter_id;
	info.new_pass_flag = 0;
	info.today_play_times = m_param.today_pass_times;
	m_module_mgr->NetSend((const char*)&info, sizeof(info));
}

void NetherWorldPurgatory::SendMiLingInfo()
{
	Protocol::SCNetherWorldPurgatoryMiLingInfo info;

	info.low_key = m_param.low_key;
	info.mid_key = m_param.mid_key;
	info.high_key = m_param.high_key;

	for (int i = 0; i < MAX_NETHER_WORLD_GATORY_CHAPTER_NUM; i++)
	{
		info.mi_ling_list[i].silver_reward_flag = m_param.chapter_list[i].silver_reward_flag;
		info.mi_ling_list[i].gold_reward_flag = m_param.chapter_list[i].gold_reward_flag;
	}

	m_module_mgr->NetSend((const char*)&info, sizeof(info));
}
