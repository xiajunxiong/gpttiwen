#include "world.h"
#include "gamelog.h"
#include "internalcomm.h"
#include "global/rmibackobjdef.h"

#include "servercommon/string/gameworldstr.h"
#include "global/randactivity/randactivitymanager.hpp"
#include "global/activity/activitymanager.hpp"
#include "global/colosseum/colosseummanager.hpp"

#include "worldstatus.hpp"
#include "obj/character/role.h"
#include "global/usercache/usercache.hpp"
#include "global/offlineregister/offlineregistermanager.hpp"
#include "global/wildbossmanager/wildbossmanager.h"
#include "global/colosseum/colosseummanager.hpp"
#include "global/kuafuyingxionglumanager/kuafuyingxionglumanager.hpp"
#include "protocol/msgcouragechallenge.h"
#include "other/title/title.hpp"
#include "other/rolemodulemanager.hpp"
#include "protocol/msgother.h"
#include "protocol/msgchiefelection.h"
#include "protocol/msgchiefpeakbattle.h"
#include "config/logicconfigmanager.hpp"
#include "other/schooltask/schooltaskconfig.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include <set>
#include "other/roleattribute/roleexpconfig.hpp"
#include "other/rolegatherfb/dropgather.hpp"
#include "other/route/mailroute.hpp"
#include "other/title/titleconfig.hpp"
#include "other/funopen/funopenconfig.hpp"
#include "other/world_question/worldquestionconfig.hpp"
#include "item/knapsack.h"
#include "other/funopen/funopen.hpp"
#include "servercommon/noticenum.h"
#include "config/activityconfig/onlyfight/activityonlyfightconfig.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "global/shanhaibossmanager/shanhaibossmanager.hpp"
#include "other/lyjz/lingyaojiuzhuconfig.hpp"
#include "obj/character/npc_gather.h"
#include "other/event/eventhandler.hpp"
#include "other/yyyxq/yingyuanyixianqianconfig.hpp"
#include "global/trademarket/trademarketmanager.hpp"
#include "protocol/msgworldstatus.h"
#include "config/sharedconfig/sharedconfig.h"
#include "global/cross/crossmanager.h"
#include "other/roleattribute/roleexpconfig.hpp"
#include "other/lunhuiwangchuan/lunhuiwangchuanconfig.hpp"
#include "global/rank/rankmanager.hpp"
#include "other/bigdipper/bigdippermanager.hpp"
#include "global/datasynccheck/datasynccheck.hpp"
#include "servercommon/errornum.h"
#include "other/cycleactivity/qingyuanduiduipeng/qingyuanduiduipengmanager.hpp"
#include "global/randactivity/randactivityimpl/randactivitygodbeastadvent.hpp"
#include "chat/chatmanager.h"
#include "other/workshop/godequipupconfig.hpp"
#include "other/robot/robotconfig.hpp"
#include "servercommon/serverconfig/appearanceconfig.hpp"
#include "config/otherconfig/entryconfig.hpp"
#include "config/otherconfig/sundryconfig.hpp"
#include "other/pet/pet.hpp"
#include "other/workshop/suitlistconfig.hpp"
#include "other/rolecross/rolecross.hpp"

void WorldStatus::SetCourageChallengeFirstKill(int level, Role * role)
{
	if (role == NULL)
	{
		return;
	}

	if (level < 0 || level >= COURAGE_CHALLENGE_MAX_LEVEL)
	{
		return;
	}

	if (m_courage_challenge_data.first_pass_role_id[level] != 0)
	{
		return;
	}

	m_courage_challenge_data.first_pass_role_id[level] = role->GetUID();

	this->SendCourageChallengeFirstKillInfo();
}

void WorldStatus::SendCourageChallengeFirstKillInfo(Role * role)
{
	/*
	Protocol::SCCourageChallengeFirstKillInfo protocol;
	protocol.count = 0;

	for (int i = 0; i < ARRAY_LENGTH(m_courage_challenge_data.first_pass_role_id) && protocol.count<ARRAY_LENGTH(protocol.first_kill_list); ++i)
	{
		if (m_courage_challenge_data.first_pass_role_id[i] == 0)
		{
			break;
		}

		UserCacheNode *user_node = UserCacheManager::Instance().GetUserNode(IntToUid(m_courage_challenge_data.first_pass_role_id[i]));
		if (NULL != user_node)
		{
			user_node->GetName(protocol.first_kill_list[i].role_name);
			protocol.count += 1;
		}
	}

	unsigned int length = sizeof(protocol) - (ARRAY_LENGTH(protocol.first_kill_list) - protocol.count) * sizeof(protocol.first_kill_list[0]);

	if (role != NULL)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&protocol, length);
	}
	else
	{
		World::GetInstWorld()->SendToAllGateway((const char*)&protocol, length);
	}	
	*/
}

int WorldStatus::GetWorldLevel(bool is_cal)
{
	if (is_cal)
	{
		this->OnCalcCrossOpenDay();
	}

	return LOGIC_CONFIG->GetRoleExpConfig()->GetWorldLevel(cross_server_open_day);
}

int WorldStatus::GetBraveGroundWorldLevel()
{
	return m_common_data.brave_ground_world_level;
}

void WorldStatus::SendWorldLevelInfo(Role * role)
{
	//跨服没连接
	if (!CrossManager::Instance().IsCrossServerConnected())
	{
		return;
	}

	Protocol::SCWorldLevelInfo info;
	info.open_day = cross_server_open_day;
	info.today_zero_timestamp = static_cast<unsigned int>(GetZeroTime(EngineAdapter::Instance().Time()));

	if (NULL == role)
	{
		World::GetInstWorld()->SendToAllGateway((const char*)&info, sizeof(info));
	}
	else
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &info, sizeof(info));
	}
}

void WorldStatus::OnCalcCrossOpenDay()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		cross_server_open_day = World::GetInstWorld()->GetCurRealOpenDayByOpenTimestamp((unsigned int)SHAREDCONFIG->GetOpenServerConfig().GetOpenServerZeroTime());
	}
	else
	{
		if(m_common_data.open_cross_server_zero_timestamp <= 0) return;

		cross_server_open_day = World::GetInstWorld()->GetCurRealOpenDayByOpenTimestamp(m_common_data.open_cross_server_zero_timestamp);
	}
}

bool WorldStatus::IsChiefElectionWinner(int role_id, int base_prof)
{
	for (int i = 0; i < ARRAY_LENGTH(m_chief_election_data.winner); ++i)
	{
		if (m_chief_election_data.winner[i].role_id == role_id && m_chief_election_data.winner[i].prof == base_prof)
		{
			return true;
		}
	}

	return false;
}

bool WorldStatus::OnChiefElectionFinish(int role_id, int prof)
{
	int free_index = -1;
	for (int i = 0; i < ARRAY_LENGTH(m_chief_election_data.winner); ++i)
	{
		if (m_chief_election_data.winner[i].role_id == role_id || (m_chief_election_data.winner[i].role_id != 0 && m_chief_election_data.winner[i].prof == prof))
		{
			free_index = -1;
			return false;
		}

		if (m_chief_election_data.winner[i].role_id == 0 && free_index == -1)
		{
			free_index = i;
		}
	}

	if (free_index == -1)
	{
		return false;
	}

	m_chief_election_data.winner[free_index].role_id = role_id;
	m_chief_election_data.winner[free_index].prof = prof;

	Role * tmp = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
	if (tmp != NULL)
	{
		tmp->GetRoleModuleManager()->GetTitle()->CalChiefElectionTitle();
		EventHandler::Instance().OnWinChiefElection(tmp, prof);

		Protocol::SCChiefElectionChampion protocol;
		EngineAdapter::Instance().NetSend(tmp->GetNetId(), (const char *)&protocol, sizeof(protocol));
	}
	else
	{
		if (CrossConfig::Instance().IsHiddenServer())
		{
			crossgameprotocal::HiddenGameRoleWinChiefElectionSyn hgrwces;
			hgrwces.role_id = role_id;
			hgrwces.prof = prof;

			InternalComm::Instance().SendToGameThroughCross2((const char *)&hgrwces, sizeof(hgrwces), IntToUid(role_id).db_index);
		}
		else
		{
			OfflineRegisterManager::Instance().OnWinChiefElection(role_id, prof);
		}
	}

	this->SendChiefElectionInfo();
	this->SyncWorldStatusHiddenToGameworld();
	return true;
}

bool WorldStatus::GmsetChiefElection(int role_id, int prof)
{
	return true;
}

void WorldStatus::ClearChiefElectionInfo(bool notice)
{
	for (int i = 0; i < ARRAY_LENGTH(m_chief_election_data.winner); ++i)
	{
		int role_id = m_chief_election_data.winner[i].role_id;
		int base_prof = m_chief_election_data.winner[i].prof;
		m_chief_election_data.winner[i].role_id = 0;

		Role * tmp = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
		if (tmp != NULL)
		{
			tmp->GetRoleModuleManager()->GetTitle()->CalChiefElectionTitle(base_prof);
		}
	}

	if (notice)
	{
		this->SendChiefElectionInfo();
		this->SyncWorldStatusHiddenToGameworld();
	}	
}

void WorldStatus::SetChiefElectionInfo(const ChiefElectionDataParam & chief_election_data)
{
	ChiefElectionDataParam old_param = m_chief_election_data;
	m_chief_election_data = chief_election_data;

	for (int i = 0; i < ARRAY_LENGTH(old_param.winner); ++i)
	{
		int role_id = old_param.winner[i].role_id;
		int base_prof = old_param.winner[i].prof;

		if (m_chief_election_data.HasRole(role_id))
		{
			continue;
		}		

		Role * tmp = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
		if (tmp != NULL)
		{
			tmp->GetRoleModuleManager()->GetTitle()->CalChiefElectionTitle(base_prof);
		}
	}

	for (int i = 0; i < ARRAY_LENGTH(m_chief_election_data.winner); ++i)
	{
		int role_id = m_chief_election_data.winner[i].role_id;

		if (old_param.HasRole(role_id))
		{
			continue;
		}

		Role * tmp = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
		if (tmp != NULL)
		{
			tmp->GetRoleModuleManager()->GetTitle()->CalChiefElectionTitle();
		}
	}

	this->SendChiefElectionInfo();
}

void WorldStatus::SendChiefElectionInfo(Role * role)
{
	Protocol::SCChiefElectionWinRoleInfo protocol;
	for (int i = 0; i < ACTIVITY_CHIEF_ELECTION_JOIN_PROF_NUM; ++i)
	{
		protocol.winner_info[i] = m_chief_election_data.winner[i];
	}

	if (role != NULL)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&protocol, sizeof(protocol));
	}
	else
	{
		World::GetInstWorld()->SendToAllGateway((const char*)&protocol, sizeof(protocol));
	}
}

std::vector<int> WorldStatus::GetChiefElectionTopRole()
{
	std::vector<int> ret;
	for (int i = 0; i < ARRAY_LENGTH(m_chief_election_data.winner) ; ++i)
	{
		if (m_chief_election_data.winner[i].role_id != 0)
		{
			UserCacheNode * temp_user = UserCacheManager::Instance().GetUserNode(m_chief_election_data.winner[i].role_id);
			if (temp_user == NULL || temp_user->GetBaseProfession() != m_chief_election_data.winner[i].prof)
			{
				continue;
			}

			ret.push_back(m_chief_election_data.winner[i].role_id);
		}
	}

	return ret;
}

void WorldStatus::OnChiefPeakBattleFinish(int role_id, int match_round)
{
	for (int i = 0; i < ARRAY_LENGTH(m_chief_peak_battle_data.role_info); ++i)
	{
		if (0 == m_chief_peak_battle_data.role_info[i].role_id)
		{
			m_chief_peak_battle_data.role_info[i].role_id = role_id;
			m_chief_peak_battle_data.role_info[i].match_round = match_round;
			break;
		}
	}
	
	Role * tmp = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
	if (tmp != NULL)
	{
		tmp->GetRoleModuleManager()->GetTitle()->CalPeakBattleTitle();
	}

	if (m_chief_peak_battle_data.champion_role_id == 0 && match_round >= ACTIVITY_CHIEF_PEAK_BATTLE_FIGTHER_NUM)
	{
		m_chief_peak_battle_data.champion_role_id = role_id;
		this->SendChiefPeakBattleInfo();
	}

	this->SyncWorldStatusHiddenToGameworld();
}

void WorldStatus::ClearPeakBattleInfo(bool notice)
{
	m_chief_peak_battle_data.champion_role_id = 0;

	for (int i = 0; i < ARRAY_LENGTH(m_chief_peak_battle_data.role_info); ++i)
	{
		int role_id = m_chief_peak_battle_data.role_info[i].role_id;
		m_chief_peak_battle_data.role_info[i].role_id = 0;

		Role * tmp = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
		if (tmp != NULL)
		{
			tmp->GetRoleModuleManager()->GetTitle()->CalPeakBattleTitle(m_chief_peak_battle_data.role_info[i].match_round);
		}
	}

	if (notice)
	{
		this->SendChiefPeakBattleInfo();
		this->SyncWorldStatusHiddenToGameworld();
	}
}

void WorldStatus::SetChiefPeakBattleInfo(const ChiefPeakBattleDataParam & chief_peak_battle_data)
{
	ChiefPeakBattleDataParam old_param = m_chief_peak_battle_data;
	m_chief_peak_battle_data = chief_peak_battle_data;

	for (int i = 0; i < ARRAY_LENGTH(old_param.role_info); ++i)
	{
		int role_id = old_param.role_info[i].role_id;
		int last_round = old_param.role_info[i].role_id;

		if (m_chief_peak_battle_data.HasRole(role_id))
		{
			continue;
		}

		Role * tmp = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
		if (tmp != NULL)
		{
			tmp->GetRoleModuleManager()->GetTitle()->CalPeakBattleTitle(last_round);
		}
	}

	for (int i = 0; i < ARRAY_LENGTH(m_chief_peak_battle_data.role_info); ++i)
	{
		int role_id = m_chief_peak_battle_data.role_info[i].role_id;

		if (old_param.HasRole(role_id))
		{
			continue;
		}

		Role * tmp = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
		if (tmp != NULL)
		{
			tmp->GetRoleModuleManager()->GetTitle()->CalPeakBattleTitle();
		}
	}

	this->SendChiefPeakBattleInfo();
}

int WorldStatus::GetChiefPeakBattleMatchRound(int role_id)
{
	for (int i = 0; i < ARRAY_LENGTH(m_chief_peak_battle_data.role_info); ++i)
	{
		if (role_id == m_chief_peak_battle_data.role_info[i].role_id)
		{
			return m_chief_peak_battle_data.role_info[i].match_round;
		}		
	}

	return 0;
}

void WorldStatus::SendChiefPeakBattleInfo(Role * role)
{
	Protocol::SCChiefPeakBattleInfo protocol;
	protocol.champion_role_id = m_chief_peak_battle_data.champion_role_id;
	
	if (role != NULL)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&protocol, sizeof(protocol));
	}
	else
	{
		World::GetInstWorld()->SendToAllGateway((const char*)&protocol, sizeof(protocol));
	}
}

void WorldStatus::GetServerSchoolTask(int* out_list, int daily_num)
{
	for (int i = 0; i < SCHOOL_TASK_NUM && i < daily_num; ++i)
	{
		out_list[i] = m_common_data.today_school_task_list[i];
	}
}

void WorldStatus::OnConnectCrossServerSucc()
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenReqSyncWorldStatus msg;
		msg.server_id = LocalConfig::Instance().GetMergeServerId();

		InternalComm::Instance().SendToHiddenThroughCross((const char *)&msg, sizeof(msg));
		DataSyncCheck::Instance().AddCheck(crossgameprotocal::DATA_SYNC_CHECK_TYPE_WORLD_STATUS);
	}
	else
	{
		this->SyncWorldStatusHiddenToGameworld();
	}
}

void WorldStatus::SyncWorldStatusHiddenToGameworld(int server_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::HiddenGameSyncWorldStatus msg;
	msg.chief_election_data = m_chief_election_data;
	msg.chief_peak_battle_data = m_chief_peak_battle_data;
	msg.world_wst_group_id = m_common_data.now_question_group_id;
	msg.world_wst_answer_subject = m_common_data.now_answer_subject;
	msg.world_wst_answer = m_common_data.now_answer;
	msg.world_wst_answer_stamp = m_common_data.answer_tamp;
	msg.world_wst_show_answer_stamp = m_common_data.show_answer_tamp;
	msg.world_wst_next_subject_refresh_stamp = m_common_data.next_subject_refresh_tamp;
	msg.answer_end_stamp = m_common_data.answer_end_tamp;
	msg.next_answer_stamp = m_common_data.next_answer_tamp;
	memcpy(msg.can_find_end_timestamp_list, m_common_data.can_find_end_timestamp_list, sizeof(msg.can_find_end_timestamp_list));
	msg.guild_fight_info = m_guild_fight_param;
	msg.open_cross_server_zero_timestamp = (unsigned int)SHAREDCONFIG->GetOpenServerConfig().GetOpenServerZeroTime();
	if (server_id == 0)
	{
		InternalComm::Instance().SendToAllGameThroughCross(&msg, sizeof(msg));
	}
	else
	{
		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), server_id, (const char *)&msg, sizeof(msg));
	}
}

void WorldStatus::SetWorldStatus(const crossgameprotocal::HiddenGameSyncWorldStatus * msg)
{
	this->SetChiefElectionInfo(msg->chief_election_data);
	this->SetChiefPeakBattleInfo(msg->chief_peak_battle_data);
	m_common_data.now_question_group_id = msg->world_wst_group_id;
	m_common_data.now_answer_subject = msg->world_wst_answer_subject;
	m_common_data.now_answer = msg->world_wst_answer;
	m_common_data.answer_tamp = msg->world_wst_answer_stamp;
	m_common_data.show_answer_tamp = msg->world_wst_show_answer_stamp;
	m_common_data.next_subject_refresh_tamp = msg->world_wst_next_subject_refresh_stamp;
	m_common_data.answer_end_tamp = msg->answer_end_stamp;
	m_common_data.next_answer_tamp = msg->next_answer_stamp;
	memcpy(m_common_data.can_find_end_timestamp_list, msg->can_find_end_timestamp_list, sizeof(m_common_data.can_find_end_timestamp_list));
	m_common_data.open_cross_server_zero_timestamp = msg->open_cross_server_zero_timestamp;
	this->OnCalcCrossOpenDay();
	this->SendWorldLevelInfo();
}

void WorldStatus::RefreshWorldQuestion(time_t now_time, bool is_begin)
{
	m_world_answer_role_map.clear();//清除上一题答题玩家
	if (is_begin)
	{
		const WorldQuestionOther * other_cfg = LOGIC_CONFIG->GetWorldQuestionConfig()->GetOhterCfg();
		if (other_cfg == NULL)
		{
			gamelog::g_log_world.printf(LL_ERROR, "%s ERROR No OtherConfig!", __FUNCTION__);
			return;
		}
		
		{
			int next_day = EngineAdapter::Instance().NextDayInterval(other_cfg->start_time, 0, 0);
			int world_question_end_tamp = EngineAdapter::Instance().NextDayInterval(other_cfg->end_time, 0, 0);
			m_common_data.next_answer_tamp = next_day + now_time;//设置下次开启时间
			m_common_data.answer_end_tamp = world_question_end_tamp + now_time;//设置活动结束时间
			if (m_common_data.next_answer_tamp < m_common_data.answer_end_tamp)
			{
				m_common_data.next_answer_tamp += SECOND_PER_DAY;//防止前后时间戳乱序，必须下次开启时间大于活动结束时间
			}
			gamelog::g_log_world.printf(LL_INFO, "day begin %s succ next_answer_tamp[%lld] answer_end_tamp[%lld] ", __FUNCTION__, m_common_data.next_answer_tamp, m_common_data.answer_end_tamp);
		}
	}
	WorldQuestionBank question_cfg;
	bool is_has_new_question = LOGIC_CONFIG->GetWorldQuestionConfig()->RandQuestionCdg(m_common_data.now_question_group_id + 1, &question_cfg);	//下一题
	const WorldQuestionOther * other_cfg = LOGIC_CONFIG->GetWorldQuestionConfig()->GetOhterCfg();
	if (is_has_new_question && other_cfg != NULL)//还有下一题
	{
		m_common_data.now_answer_subject = question_cfg.question_id;
		m_common_data.now_answer = question_cfg.answer;
		m_common_data.now_question_group_id += 1;
		m_common_data.next_subject_refresh_tamp = now_time + EngineAdapter::Instance().NextHourInterval(0, 0);
		m_common_data.answer_tamp = now_time + other_cfg->question_time;
		gamelog::g_log_world.printf(LL_INFO, "%s succ subject[%d] answer[%d] group_id[%d] refresh_stamp[%lld] answer_stamp[%lld] ", __FUNCTION__, m_common_data.now_answer_subject,
			m_common_data.now_answer, m_common_data.now_question_group_id, m_common_data.next_subject_refresh_tamp, m_common_data.answer_tamp);
	}
	else
	{
		m_common_data.answer_end_tamp = 20 + now_time;//设置活动结束时间,没有题目了，20秒后活动关闭
		gamelog::g_log_world.printf(LL_ERROR, "%s RefreshWorldQuestion END!", __FUNCTION__);
	}
}

bool WorldStatus::RoleAnswerQuestion(Role * role, int answer_id)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_WORLD_ANSWER))
	{
		return false;
	}
	if (role == NULL) return false;

	if (!this->IsAnswerTime())
	{
		return false;
	}

	if (role->GetRoleModuleManager()->GetCommonData().world_ans_tamp > 0 && role->GetRoleModuleManager()->GetCommonData().world_ans_tamp > EngineAdapter::Instance().Time())
	{
		return false;
	}
	else
	{
		role->GetRoleModuleManager()->GetCommonData().world_ans = 0;
		role->GetRoleModuleManager()->GetCommonData().world_ans_tamp = 0;
	}

	//if (!FunOpen::IsFunOpen(role, FUN_TYPE_WORLD_ANSWER))return false;
	int talk_level = GLOBALCONFIG->GetTalkLevel();
	if (talk_level > role->GetLevel()) return false;

	if (role->GetRoleModuleManager()->GetCommonData().world_ans == 1)return false;

	const WorldAnswerGroup * answer_cfg = LOGIC_CONFIG->GetWorldQuestionConfig()->GetAnswerGroupCfg(m_common_data.now_answer);
	if (answer_cfg == NULL)return false;

	bool world_ans = false;
	for (int an_idx = 0; an_idx < 4; an_idx++)
	{
		if (answer_cfg->answer_id[an_idx] != 48 && answer_cfg->answer_id[an_idx] == answer_id)//48 == '0'
		{
			world_ans = true;//符合世界答题选项之一，才算参加世界答题
			role->GetRoleModuleManager()->GetCommonData().world_answer_idx = an_idx + 1;
		}
	}
	if (!world_ans)return false;

	bool an_true = false;
	if (answer_cfg->answer_true > 0 && answer_cfg->answer_true <= WorldAnswerGroup::MAX_ANSWER_ID_NUM)
	{
		if (answer_cfg->answer_id[answer_cfg->answer_true - 1] != 48 && answer_cfg->answer_id[answer_cfg->answer_true - 1] == answer_id)//48 == '0'
		{
			an_true = true;
		}
	}
	role->GetRoleModuleManager()->GetCommonData().world_ans = 1;
	role->GetRoleModuleManager()->GetCommonData().world_ans_tamp = static_cast<unsigned int>(m_common_data.next_subject_refresh_tamp - 10);
	if (!an_true)
	{
		const WorldQuestionReward * reward_cfg = LOGIC_CONFIG->GetWorldQuestionConfig()->GetReward(role->GetLevel());
		if (reward_cfg != NULL)
		{
			role->GetRoleModuleManager()->GetMoney()->AddCoinBind(reward_cfg->coin, __FUNCTION__);
		}
		role->GetRoleModuleManager()->NoticeNum(errornum::EN_WORLD_QUESTION_ANS_ERR);
	}
	else
	{
		std::map<int, bool>::iterator it = m_world_answer_role_map.find(role->GetUID());
		if (it == m_world_answer_role_map.end())
		{
			//奖励
			const WorldQuestionReward * reward_cfg = LOGIC_CONFIG->GetWorldQuestionConfig()->GetReward(role->GetLevel());
			std::vector<ItemConfigData> item_list;
			if (reward_cfg != NULL)
			{
				for (std::vector<int>::const_iterator reward_it = reward_cfg->item_list.begin(); reward_it != reward_cfg->item_list.end(); reward_it++)
				{
					const WorldQuestionRewardGroup * reward_node_cfg = LOGIC_CONFIG->GetWorldQuestionConfig()->RandRewardGroup(*reward_it);
					if (reward_node_cfg != NULL)
					{
						item_list.push_back(reward_node_cfg->reward_item);
					}
				}

				role->AddExp(reward_cfg->exp);
				role->GetRoleModuleManager()->GetMoney()->AddCoinBind(reward_cfg->coin, __FUNCTION__);
				role->GetRoleModuleManager()->GetKnapsack()->PutList((short)item_list.size(), &item_list[0], PUT_REASON_WORLD_QUESTION_REWARD);
			}
			m_world_answer_role_map[role->GetUID()] = true;
		}
		role->GetRoleModuleManager()->NoticeNum(noticenum::NT_WORLD_QUESTION_ANSWER_SUCC);
	}

	this->SendWorldAnswerInfo(role);
	this->LogFunctionStats(role, "World_Question");
	return true;
}

void WorldStatus::OnRefreshAnswerSubject(time_t now_time)
{
	const WorldQuestionOther * other_cfg = LOGIC_CONFIG->GetWorldQuestionConfig()->GetOhterCfg();
	int show_time = 0;
	if (other_cfg != NULL)
	{
		show_time = other_cfg->answer_time;
	}
	m_common_data.show_answer_tamp = now_time + show_time;
}

void WorldStatus::SendWorldAnswerInfo(Role * role)
{
	Protocol::SCWorldQuestionNotice msg;
	msg.answer_end_tamp = m_common_data.answer_end_tamp;
	msg.answer_tamp = m_common_data.answer_tamp;
	msg.next_subject_refresh_tamp = m_common_data.next_subject_refresh_tamp;
	msg.show_answer_tamp = m_common_data.show_answer_tamp;
	msg.now_answer_subject = m_common_data.now_answer_subject;
	msg.role_answer_idx = 0;
	const FunOpenCfg* fun_open_cfg = LOGIC_CONFIG->GetFunOpenConfig()->GetFunOpenCfg(FUN_TYPE_WORLD_ANSWER);
	if (NULL != fun_open_cfg)
	{
		msg.level_limit = fun_open_cfg->limit_level;
	}
	else
	{
		msg.level_limit = GLOBALCONFIG->GetTalkLevel();
	}
	if (role != NULL)
	{
		msg.is_broadcast = 0;
		msg.reserve_ch = 0;
		msg.role_answer_idx = role->GetRoleModuleManager()->GetCommonData().world_answer_idx;

		if (role->GetRoleModuleManager()->GetCommonData().world_ans_tamp > 0 && role->GetRoleModuleManager()->GetCommonData().world_ans_tamp <= EngineAdapter::Instance().Time())
		{
			role->GetRoleModuleManager()->GetCommonData().world_ans = 0;
			role->GetRoleModuleManager()->GetCommonData().world_ans_tamp = 0;
			role->GetRoleModuleManager()->GetCommonData().world_answer_idx = 0;
		}
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&msg, sizeof(msg));
	}
	else
	{
		msg.is_broadcast = 1;
		World::GetInstWorld()->SendToAllGateway((const char*)&msg, sizeof(msg));
	}
}

void WorldStatus::SendTitleToRole(int title_id, int uid, const char * reason)
{
	const TitleCfg * title_cfg = LOGIC_CONFIG->GetTitleConfig()->GetTitleCfg(title_id);
	if(NULL == title_cfg) return;

	//先在该服查看玩家是否在线
	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(uid);
	if (NULL != role)
	{
		role->GetRoleModuleManager()->GetTitle()->AddTitle(title_id, reason);
		return;
	}
	//玩家不在线此时查看是否在隐藏服
	if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameRoleAddTitle msg;
		msg.title_id = title_id;
		msg.uid = uid;
		F_STRNCPY(msg.reason, reason, sizeof(msg.reason));

		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(uid).db_index, (const char*)&msg, sizeof(msg));
		return;
	}
	//玩家处于离线状态
	OfflineRegisterManager::Instance().OnTitleSendReq(IntToUid(uid), title_id);
}

void WorldStatus::OnlyFightStatusChange(int to_status, bool is_enter)
{
	const std::vector<OnlyFightSceneCfg> * scene_cfg_vec = LOGIC_CONFIG->GetActivityOnlyFightConfig()->GetOnlyFightSceneCfgVec();
	if(NULL == scene_cfg_vec) return;

	for (int i = 0; i < (int)scene_cfg_vec->size(); i++)
	{
		SpecialLogicOnlyFight * fight_hall = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicOnlyFight((*scene_cfg_vec)[i].scene_id, COMMON_FB_KEY);
		if(NULL == fight_hall) continue;

		switch (to_status)
		{
		case ACTIVITY_STATUS_STANDY:
			fight_hall->OnActivityStandy();
			break;
		case ACTIVITY_STATUS_OPEN:		// 初始化战斗大厅
			{
				if (is_enter)
				{
					fight_hall->OnActivityEnterTime();
				}
				else
				{
					fight_hall->OnActivityOpen();
				}
			}
			break;
		case ACTIVITY_STATUS_CLOSE:
			fight_hall->OnActivityEnd();
			break;
		}
	}
}

void WorldStatus::CheckOnlyFightSceneRoleNum()
{
	const std::vector<OnlyFightSceneCfg> * scene_cfg_vec = LOGIC_CONFIG->GetActivityOnlyFightConfig()->GetOnlyFightSceneCfgVec();
	if (NULL == scene_cfg_vec) return;

	bool is_close = true;
	for (int i = 0; i < (int)scene_cfg_vec->size(); i++)
	{
		SpecialLogicOnlyFight * fight_hall = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicOnlyFight((*scene_cfg_vec)[i].scene_id, COMMON_FB_KEY);
		if (NULL == fight_hall) continue;

		if (!fight_hall->SceneRoleNumEmpty())
		{
			is_close = false;
			break;
		}
	}

	if (is_close)
	{
		ActivityManager::Instance().OnForceToNextState(ACTIVITY_TYPE_ONLY_FIGHT);
	}
}

int WorldStatus::GetOnlyFightRoleRank(int role_uid)
{
	std::map<int, int>::iterator it = m_only_fight_rank_map.find(role_uid);
	if (it != m_only_fight_rank_map.end())
	{
		return it->second;
	}

	return 0;
}

int WorldStatus::GetOnlyFightRankTitleId(int rank)
{
	if (rank <= 0 || rank > ONLY_FIGHT_RANK_TITLE_MAX_NUM) return 0;

	return m_only_fight_data.title_id_list[rank - 1];
}

void WorldStatus::GetOnlyFightRankTitleList(std::vector<int>* title_list)
{
	if(NULL == title_list) return;

	for (int i = 0; i < ONLY_FIGHT_RANK_TITLE_MAX_NUM; i++)
	{
		title_list->push_back(m_only_fight_data.title_id_list[i]);
	}
}

void WorldStatus::AddOnlyFightRecord(int length, int * id_list)
{
	if (!CrossConfig::Instance().IsHiddenServer() || NULL == id_list || length <= 0 || m_only_fight_data.count >= ONLY_FIGHT_SCNEE_MAX_NUM) return;

	for (int i = 0; i < length && i < ONLY_FIGHT_RANK_TITLE_MAX_NUM; i++)
	{
		if(id_list[i] == 0) continue;

		m_only_fight_data.rank[m_only_fight_data.count][i] = id_list[i];
		m_only_fight_rank_map[id_list[i]] = i + 1;
	}
	m_only_fight_data.count++;
	this->HiddenSynOnlyFightDataToAllGame();
}

void WorldStatus::HiddenSynOnlyFightDataToAllGame()
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::HiddenGameSynOnlyFightInfo hgsofi;
	hgsofi.param = m_only_fight_data;

	InternalComm::Instance().SendToAllGameThroughCross((const void *)&hgsofi, sizeof(hgsofi));
}

void WorldStatus::HiddenGameOnlyFightDataSyn(OnlyFightDataParam * param)
{
	if (CrossConfig::Instance().IsHiddenServer() || NULL == param) return;

	m_only_fight_data = *param;
	this->OnReCalOnlyFightMap();
}

void WorldStatus::GameHiddenOnlyFightDataSynReq(int server_id)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::HiddenGameSynOnlyFightInfo hgsofi;
	hgsofi.param = m_only_fight_data;

	InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), server_id, (const char *)&hgsofi, sizeof(hgsofi));
}

void WorldStatus::ClearOnlyFightRankInfo()
{
	for (int i = 0; i < m_only_fight_data.count && i < ONLY_FIGHT_SCNEE_MAX_NUM; i++)
	{
		for (int j = 0; j < ONLY_FIGHT_RANK_TITLE_MAX_NUM; j++)
		{
			if (m_only_fight_data.rank[i][j] == 0) continue;

			Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(m_only_fight_data.rank[i][j]);
			if (NULL != role)
			{
				m_only_fight_rank_map.erase(m_only_fight_data.rank[i][j]);
				//重新计算称号
				role->GetRoleModuleManager()->GetTitle()->CalOnlyFightTitle();
			}
		}
	}

	m_only_fight_data.Reset();
	m_only_fight_rank_map.clear();

	const std::vector<int> * title_list = LOGIC_CONFIG->GetActivityOnlyFightConfig()->GetRankTitleListCfg();
	if (NULL != title_list)
	{
		for (int i = 0; i < ONLY_FIGHT_RANK_TITLE_MAX_NUM && i < (int)(*title_list).size(); i++)
		{
			m_only_fight_data.title_id_list[i] = (*title_list)[i];
		}
	}
}

void WorldStatus::OnReCalOnlyFightMap()
{
	m_only_fight_rank_map.clear();

	//添加映射
	for (int i = 0; i < m_only_fight_data.count && i < ONLY_FIGHT_SCNEE_MAX_NUM; i++)
	{
		for (int j = 0; j < ONLY_FIGHT_RANK_TITLE_MAX_NUM; j++)
		{
			if (m_only_fight_data.rank[i][j] == 0) continue;

			m_only_fight_rank_map[m_only_fight_data.rank[i][j]] = j + 1;
		}
	}
}

void WorldStatus::OnCalcFakeAnnouncement(unsigned int now_second)
{
	m_fake_announcement_list.clear();
	m_fake_announcement_time_map.clear();
	m_next_calc_fake_announcement_timestamp = EngineAdapter::Instance().NextHourInterval(0, 0) + now_second;

	int times_count = 0;
	std::vector<FakeAnnouncementCfg> cfg_list;
	LOGIC_CONFIG->GetSundryConfig()->GetFakeAnnouncement(times_count, cfg_list);
	if (times_count <= 0 || cfg_list.empty())
	{
		return;
	}
	int time_interval = SECOND_PER_HOUR / times_count;
	if (time_interval <= 0)
	{
		return;
	}

	//时间默认先放入(防止顺序未打乱的去除)
	unsigned int hour_zero_timestamp = GetZeroMinuteTime(now_second);
	int start_time = RandomNum(time_interval) + hour_zero_timestamp;
	std::vector<unsigned int> release_time_list;
	for (int i = 0; i < times_count; ++i)
	{
		unsigned int start_release_time = start_time + time_interval * i;
		release_time_list.push_back(start_release_time);
	}

	std::random_shuffle(release_time_list.begin(), release_time_list.end());

	int release_index = 0;
	for (int i = 0; i < (int)cfg_list.size(); ++i)
	{
		FakeAnnouncementInfo info;
		info.SetData(&cfg_list[i]);
		m_fake_announcement_list.push_back(info);

		for (int k = 0; k < cfg_list[i].random_times && release_index < (int)release_time_list.size(); ++k)
		{
			m_fake_announcement_time_map[release_time_list[release_index]] = (int)(m_fake_announcement_list.size()) - 1;
			release_index++;
		}
	}

	//去除一些已经超过的时间
	for (std::map<unsigned int, int>::iterator it = m_fake_announcement_time_map.begin(); it != m_fake_announcement_time_map.end();)
	{
		if (it->first >= now_second)
		{
			break;
		}
		else
		{
			m_fake_announcement_time_map.erase(it++);
		}
	}
}

void WorldStatus::OnCheckFakeAnnouncement(unsigned int now_second)
{
	if (m_fake_announcement_time_map.empty())
	{
		return;
	}

	for (std::map<unsigned int, int>::iterator it = m_fake_announcement_time_map.begin(); it != m_fake_announcement_time_map.end();)
	{
		if (it->first > now_second)
		{
			break;
		}

		if (it->second < 0 || it->second >= (int)m_fake_announcement_list.size())
		{
			m_fake_announcement_time_map.erase(it++);
			break;
		}

		FakeAnnouncementInfo & info = m_fake_announcement_list[it->second];
		int length = 0;
		if (info.is_random_name)
		{
			GameName robot_name;
			ROBOTCONFIG->GetRandomName(SEX_ARRAY[RandomNum(ARRAY_LENGTH(SEX_ARRAY))], robot_name, true, true, true, RobotConfig::ROBOT_NAME_TYPE_ROLE);

			length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_fake_announcement_content_1, 
				info.descripte_1.c_str(), 0, robot_name, info.descripte_2.c_str(), info.item_id, info.descripte_3.c_str());
		}
		else
		{
			length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_fake_announcement_content_2,
				info.descripte_1.c_str(), info.descripte_2.c_str(), info.item_id, info.descripte_3.c_str());
		}
		SysMsgType msg_type;
		if (FAKE_ANNOUNCEMENT_TYPE_ROLLING_NOTICE == info.announcement_type)
		{
			msg_type = GetMsgType(SYS_MSG_ROLLING_NOTICE);
		}
		else if(FAKE_ANNOUNCEMENT_TYPE_SYSTEM_CHAT == info.announcement_type)
		{
			msg_type = GetMsgType(SYS_MSG_SYSTEM_CHAT);
		}
		else
		{
			msg_type = GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT);
		}

		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, msg_type, 0, 0,
				info.min_level, info.max_level);
		}
		m_fake_announcement_time_map.erase(it++);
	}
}

void WorldStatus::AddHuanShouAnnInfo(Role * role, int egg_item_id, int huanshou_id)
{
	if(NULL == role) return;

	HuanShouAnnouncementInfo ann_info;
	ann_info.role_uid = role->GetUID();
	role->GetName(ann_info.role_name);
	ann_info.egg_item_id = egg_item_id;
	ann_info.huanshou_id = huanshou_id;

	this->AddHuanShouAnnInfo(ann_info);
	//把记录同步到跨服/原服
	if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameHuanShouAnnInfoSync hghsais;
		hghsais.ann_info = ann_info;

		InternalComm::Instance().SendToGameThroughCross(role->GetRoleModuleManager()->GetRoleCross()->GetOriginPlatType(), 
			role->GetRoleModuleManager()->GetRoleCross()->GetOriginServerId(), (const char *)&hghsais, sizeof(hghsais));
	}
	else
	{
		crossgameprotocal::GameHiddenHuanShouAnnInfoSync ghhsais;
		ghhsais.ann_info = ann_info;

		InternalComm::Instance().SendToHiddenThroughCross((const char *)&ghhsais, sizeof(ghhsais));
	}
	//先同步后发,防止此时下发的并不是最新的
	this->OnSendHuanShouAnnInfo(role);
}

void WorldStatus::AddHuanShouAnnInfo(HuanShouAnnouncementInfo ann_info)
{
	m_huanshou_announcement_list.push_back(ann_info);
	if ((int)m_huanshou_announcement_list.size() > MAX_HUAN_SHOU_ANNOUNCEMENT_INFO_NUM)
	{
		m_huanshou_announcement_list.erase(m_huanshou_announcement_list.begin());
	}
}

void WorldStatus::OnSendHuanShouAnnInfo(Role * role)
{
	if(NULL == role) return;

	//ps.策划允许二个不同版本,所以不需要同步别的服的记录到所有服
	//1.玩家在跨服
	//2.跨服没连接,直接使用原服公告
	if (CrossConfig::Instance().IsHiddenServer() || !CrossManager::Instance().IsCrossServerConnected())
	{
		Protocol::SCHuanShouAnnouncementInfo info;
		this->GetHuanShouAnnInfo(info);
		
		int length = sizeof(info) - (ARRAY_LENGTH(info.ann_list) - info.ann_count) * sizeof(info.ann_list[0]);
		EngineAdapter::Instance().NetSend(role->GetNetId(), &info, length);
	}
	else
	{
		crossgameprotocal::GameHiddenHuanShouAnnInfoReq ghhsair;
		ghhsair.role_uid = role->GetUID();

		InternalComm::Instance().SendToHiddenThroughCross((const char *)&ghhsair, sizeof(ghhsair));
	}
}

void WorldStatus::OnHiddenToGameHuanShouAnnInfo(int uid)
{
	Protocol::SCHuanShouAnnouncementInfo info;
	this->GetHuanShouAnnInfo(info);

	int length = sizeof(info) - (ARRAY_LENGTH(info.ann_list) - info.ann_count) * sizeof(info.ann_list[0]);
	InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(uid).db_index,
		(char *)&info, length, uid);
}

void WorldStatus::GetHuanShouAnnInfo(Protocol::SCHuanShouAnnouncementInfo & info)
{
	info.reserve_sh = 0;
	info.ann_count = 0;

	if ((int)m_huanshou_announcement_list.size() > ARRAY_LENGTH(info.ann_list))
	{
		std::list<HuanShouAnnouncementInfo>::const_reverse_iterator it = m_huanshou_announcement_list.rbegin();
		for (; info.ann_count < ARRAY_LENGTH(info.ann_list) && it != m_huanshou_announcement_list.rend(); ++it)
		{
			info.ann_count++;
			info.ann_list[ARRAY_LENGTH(info.ann_list) - info.ann_count] = *it;
		}

	}
	else
	{
		std::list<HuanShouAnnouncementInfo>::const_iterator it = m_huanshou_announcement_list.begin();
		for (; info.ann_count < ARRAY_LENGTH(info.ann_list) && it != m_huanshou_announcement_list.end(); ++it)
		{
			info.ann_list[info.ann_count++] = *it;
		}
	}
}

void WorldStatus::SendNoticeLog(int uid, const GameName role_name, const PlatName plat_name, int activity_type, int param1, const char * reason_str)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameNoticeLog msg;
		msg.uid = uid;
		msg.activity_type = activity_type;
		msg.param1 = param1;
		F_STRNCPY(msg.role_name, role_name, sizeof(msg.role_name));
		F_STRNCPY(msg.plat_name, plat_name, sizeof(msg.plat_name));
		F_STRNCPY(msg.reason_str, reason_str, sizeof(msg.reason_str));

		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(uid).db_index, (const char*)&msg, sizeof(msg));
		return;
	}
	if (plat_name == NULL)
	{
		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(uid);
		if (NULL != role)
		{
			ROLE_LOG_QUICK14(IntToUid(uid).db_index, LOG_TYPE_ACTIVITY_QUICK, uid, role_name, role->GetPlatName(), activity_type, param1, reason_str, NULL);
		}
	}
	else
	{
		ROLE_LOG_QUICK14(IntToUid(uid).db_index, LOG_TYPE_ACTIVITY_QUICK, uid, role_name, plat_name, activity_type, param1, reason_str, NULL);
	}
}

void WorldStatus::SetWorldQuestionStatus(int type)
{
	switch (type)
	{
	case 1://直接刷新下一题
		m_common_data.next_subject_refresh_tamp = EngineAdapter::Instance().Time();
		break;
	case 2://直接开启活动
		m_common_data.WorldQuestionNextTimesReset();
		m_common_data.next_answer_tamp = EngineAdapter::Instance().Time();
		break;
	case 3://结束活动
		m_common_data.answer_end_tamp = EngineAdapter::Instance().Time();
		break;
	default:
		break;
	}
}

int WorldStatus::GetRAQuanMinDiscountFirstMark()
{
	return m_common_data.ra_quan_min_discount_first_mark;
}

void WorldStatus::SetRAQuanMinDiscountFirstMark()
{
	m_common_data.ra_quan_min_discount_first_mark = 1;
}

void WorldStatus::SetFindEndTimestamp(int special_logic_activity_type)
{
	if(special_logic_activity_type < 0 || special_logic_activity_type >= DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_MAX) return;

	unsigned int now_second = (unsigned int)EngineAdapter::Instance().Time();
	m_common_data.can_find_end_timestamp_list[special_logic_activity_type] = now_second + (unsigned int)EngineAdapter::Instance().NextDayInterval(0, 0, 0) + SECOND_PER_DAY;
	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->SyncWorldStatusHiddenToGameworld();
	}
	gamelog::g_log_activity.printf(LL_INFO, "%s type[%d] end_timestamp[%u]", __FUNCTION__, special_logic_activity_type, m_common_data.can_find_end_timestamp_list[special_logic_activity_type]);
}

unsigned int WorldStatus::GetFindEndTimestamp(int special_logic_activity_type)
{
	if (special_logic_activity_type < 0 || special_logic_activity_type >= DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_MAX) return 0;

	return m_common_data.can_find_end_timestamp_list[special_logic_activity_type];
}

void WorldStatus::GmSetFindEndTimestamp(int special_logic_activity_type, int end_day)
{
	if(end_day < 0 || special_logic_activity_type < 0 || special_logic_activity_type >= DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_MAX) return;

	unsigned int now_second = (unsigned int)EngineAdapter::Instance().Time();
	m_common_data.can_find_end_timestamp_list[special_logic_activity_type] = now_second + (unsigned int)EngineAdapter::Instance().NextDayInterval(0, 0, 0) + end_day * SECOND_PER_DAY;
	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->SyncWorldStatusHiddenToGameworld();	
	}
	gamelog::g_log_activity.printf(LL_INFO, "%s type[%d] end_day[%d] end_timestamp[%u]", __FUNCTION__, special_logic_activity_type, end_day, m_common_data.can_find_end_timestamp_list[special_logic_activity_type]);
}

void WorldStatus::LogFunctionStats(Role * role, const char* function_name, int param1, int param2, int param3)
{
	if (NULL == role)
	{
		return;
	}

	gamelog::g_log_function_stats.printf(LL_INFO, "[Join][user[%s %d] function_name:%s times:1 param1:%d param2:%d param3:%d]",
		role->GetName(), role->GetUID(), function_name, param1, param2, param3);
}

unsigned int WorldStatus::GetTopLevelRefreshTimestamp()
{	
	return m_common_data.next_top_level_refresh_timestamp;
}

void WorldStatus::OnReCalcTopLevelRefreshTimestamp(bool is_reset)
{
	unsigned int now_second = (unsigned int)EngineAdapter::Instance().Time();
	int refresh_time_interval = TOP_LEVEL_REFRESH_DAY_TIEMS * SECOND_PER_DAY;
	if (now_second < TOP_LEVEL_REFRESH_BASE_TIMESTAMP)
	{
		m_common_data.next_top_level_refresh_timestamp = TOP_LEVEL_REFRESH_BASE_TIMESTAMP;
		m_common_data.top_level_season_start_timestamp = m_common_data.next_top_level_refresh_timestamp - refresh_time_interval;
	}
	else
	{
		UNSTD_STATIC_CHECK(TOP_LEVEL_REFRESH_DAY_TIEMS > 0);
		int after_times = (now_second - TOP_LEVEL_REFRESH_BASE_TIMESTAMP) / refresh_time_interval;
		m_common_data.next_top_level_refresh_timestamp = (after_times + 1) * refresh_time_interval + TOP_LEVEL_REFRESH_BASE_TIMESTAMP;
		m_common_data.top_level_season_start_timestamp = m_common_data.next_top_level_refresh_timestamp - refresh_time_interval;
		if (is_reset && !CrossConfig::Instance().IsHiddenServer())
		{
			//本服才重置usercache
			UserCacheManager::Instance().OnTopLevelReset(m_common_data.next_top_level_refresh_timestamp);
		}
	}
}

void WorldStatus::GmSendEndlessRankReward()
{
	if (!CrossConfig::Instance().IsHiddenServer())return;
	crossgameprotocal::HiddenCrossEndlessRankReward req;
	InternalComm::Instance().SendToCross((const char *)&req, sizeof(req));
}

void WorldStatus::RandBroadcastInfo()
{
	const RobotRoleCfg * robot_role = ROBOTCONFIG->RandRobotByLevel(32, ROBOT_CONFIG_USE_TYPE_STATIC_ROBOT);
	if (NULL == robot_role)	return;

	int m_profession = robot_role->base_prof * PROFESSION_BASE + robot_role->advance_times;
	int m_avatar_type = AppearanceConfig::Instance().RandAvatarByProfession(m_profession);

	GameName robot_name;
	ROBOTCONFIG->GetRandomName(SEX_ARRAY[m_avatar_type], robot_name, false);

	ItemID godequip_id = GODEQUIP_CONFIG->RandGodEquipId();

	const ItemBase * p_ib = ITEMPOOL->GetItem(godequip_id);
	if (NULL == p_ib) return;

	
	ItemDataWrapper compound_equip; compound_equip.Reset();
	compound_equip.item_id = godequip_id;
	compound_equip.num = 1;
	compound_equip.is_bind = 0;
	static NetValueItemParam p_nvip; p_nvip.Reset();
	if (I_TYPE_EQUIPMENT == p_ib->GetItemType())
	{
		Equipment * c_equip = (Equipment *)p_ib;
		if (NULL == c_equip) return;

		c_equip->RandomEquipmentAttr(&p_nvip);

		c_equip->RandomSpecialEffect(&p_nvip);
		F_STRNCPY(p_nvip.equip_param.creator_name, robot_name, sizeof(GameName));
		p_nvip.equip_param.has_random_net_value = 1;

		p_nvip.equip_param.inc_num = 0;
		int entry_num = 0;// LOGIC_CONFIG->GetEntryConfig()->GetEntryNumConfig(c_equip->GetEntryNum());
		if (c_equip->IsNeedReCalSmeltEntryNum())
		{
			const GodEquipAttrNumConfig* max_num_cfg = LOGIC_CONFIG->GetEntryConfig()->GetGodEquipmentNumCfg(p_nvip.equip_param.strengthen_level, c_equip->GetEquipColor());
			if (max_num_cfg != NULL && max_num_cfg->max_num <= EquipmentParam::EXA_ATTR_NUM_MAX)p_nvip.equip_param.inc_num = p_nvip.equip_param.inc_num >= max_num_cfg->max_num ? p_nvip.equip_param.inc_num : max_num_cfg->max_num;
		}
		else
		{
			//词条解锁数
			const MaxAttrNumCfg *attr_max_info = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrMaxNumConfig(c_equip->GetEquipLevel(), c_equip->GetEquipColor());
			if (attr_max_info != NULL)
			{
				entry_num = attr_max_info->max_num;
			}
		}
		for (int voc_idx = 0; voc_idx < entry_num && voc_idx < EquipmentParam::EXA_ATTR_NUM_MAX; voc_idx++)
		{
			p_nvip.equip_param.attr_extra_inc[voc_idx].un_lock = 0;//所有词条修改为不开放，等玩家自己解锁
		}

		if (entry_num > 0)
		{
			RandGroupCfg entry_type_list[EquipmentParam::EXA_ATTR_NUM_MAX];
			int real_entry_num = LOGIC_CONFIG->GetEntryConfig()->GetEntryRandConfig(c_equip->GetEntryGroup(), entry_num, entry_type_list);
			if (real_entry_num > 0)
			{
				for (int attr_type_index = 0; attr_type_index < EquipmentParam::EXA_ATTR_NUM_MAX; attr_type_index++)
				{
					if (entry_type_list[attr_type_index].att_type < 0)continue;
					{
						const AttValueCfg *attr_info = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfig3(c_equip->GetEquipLevel(), entry_type_list[attr_type_index].quality, entry_type_list[attr_type_index].att_type);
						if (attr_info != NULL)
						{
							if (p_nvip.equip_param.inc_num >= EquipmentParam::EXA_ATTR_NUM_MAX) break;
							int attr_value = rand() % ((attr_info->max - attr_info->min) + 1) + attr_info->min;
							if (attr_value <= 0)
							{
								continue;
							}
							EquipmentParam & attr_param = p_nvip.equip_param;
							AttrVocInfo & attr_inc = attr_param.attr_extra_inc[attr_type_index];
							if (attr_inc.un_lock != 1)continue;
							attr_inc.attr_type = entry_type_list[attr_type_index].att_type;
							attr_inc.attr_value = attr_value;
						}
					}
				}
				p_nvip.equip_param.inc_num = entry_num;
			}

		}

		//检查是否有重复词条
		for (int same_index = 0; same_index < EquipmentParam::EXA_ATTR_NUM_MAX; same_index++)
		{
			AttrVocInfo &voc_info = p_nvip.equip_param.attr_extra_inc[same_index];
			if (voc_info.attr_type >= 0 && voc_info.attr_value > 0)
			{
				for (int same_index2 = 0; same_index2 < EquipmentParam::EXA_ATTR_NUM_MAX; same_index2++)
				{
					AttrVocInfo &voc_info2 = p_nvip.equip_param.attr_extra_inc[same_index2];
				}
			}
		}

		//套装
		const SuitListGroup * suit_cfg = LOGIC_CONFIG->GetSuitListConfig()->GetSuitListGroup(c_equip->GetSuitRandomGroup());
		if (suit_cfg != NULL)
		{
			int suit_rad = rand() % suit_cfg->rate_weight;
			for (std::vector<SuitGroup>::const_iterator suit_it = suit_cfg->m_suit_group.begin(); suit_it != suit_cfg->m_suit_group.end(); suit_it++)
			{
				if (suit_it->rate > suit_rad)
				{
					p_nvip.equip_param.new_suit_effect_id = suit_it->id;
					break;
				}
			}
		}

		p_nvip.equip_param.is_treasure = (char)c_equip->CheckEquipmentIsTreasure(p_nvip.equip_param);
		if (c_equip->GetColor() >= ItemBase::I_COLOR_STAR && c_equip->GetColor() <= ItemBase::I_COLOR_GOD)p_nvip.equip_param.strengthen_level += 1;
		p_nvip.GetData(compound_equip.param_data);
		compound_equip.has_param = 1;
	}
	else if (I_TYPE_JEWELRY == p_ib->GetItemType())
	{
		Jewelry * c_jewelry = (Jewelry *)p_ib;
		if (NULL == c_jewelry) return;

		F_STRNCPY(p_nvip.equip_param.creator_name, robot_name, sizeof(GameName));

		if (p_nvip.jewelry_param.attr_list[0].attr_value <= 0 && p_nvip.jewelry_param.attr_list[1].attr_value <= 0)
		{
			c_jewelry->RandomNetValueParam(&p_nvip.jewelry_param);
		}

		if (c_jewelry->GetColor() >= ItemBase::I_COLOR_STAR && c_jewelry->GetColor() <= ItemBase::I_COLOR_GOD)
		{
			p_nvip.jewelry_param.strengthen_level += 1;
		}

		//额外词条 
		int entry_num = 0;// LOGIC_CONFIG->GetEntryConfig()->GetEntryNumConfig(this->GetEntryNum());
						  //词条解锁数
		if (c_jewelry->IsNeedReCalSmeltEntryNum())
		{
			const GodEquipAttrNumConfig* attr_cfg = LOGIC_CONFIG->GetEntryConfig()->GetGodEquipmentNumCfg(p_nvip.jewelry_param.strengthen_level, c_jewelry->GetEquipColor());
			if (attr_cfg != NULL)entry_num = entry_num >= attr_cfg->max_num ? entry_num : attr_cfg->max_num;
		}
		else
		{
			const MaxAttrNumCfg *attr_max_info = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrMaxNumConfig(c_jewelry->GetJewelryLevel(), c_jewelry->GetEquipColor());
			if (attr_max_info != NULL)
			{
				entry_num = attr_max_info->max_num;
			}
		}

		for (int voc_idx = 0; voc_idx < entry_num && voc_idx < JewelryParam::EXA_ATTR_NUM_MAX; voc_idx++)
		{
			p_nvip.jewelry_param.attr_extra_inc[voc_idx].un_lock = 0;
		}
		if (entry_num > 0)//是否有词条
		{
			RandGroupCfg entry_type_list[JewelryParam::EXA_ATTR_NUM_MAX];
			int real_entry_num = LOGIC_CONFIG->GetEntryConfig()->GetEntryRandConfig(c_jewelry->GetEntryGroup(), entry_num, entry_type_list);
			if (real_entry_num > 0)
			{
				for (int attr_type_index = 0; attr_type_index < JewelryParam::EXA_ATTR_NUM_MAX; attr_type_index++)
				{
					if (entry_type_list[attr_type_index].att_type < 0)continue;
					{
						const AttValueCfg *attr_info = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfig4(c_jewelry->GetJewelryLevel(), entry_type_list[attr_type_index].quality, entry_type_list[attr_type_index].att_type);
						if (attr_info != NULL)
						{
							int attr_value = rand() % ((attr_info->max - attr_info->min) + 1) + attr_info->min;
							if (attr_value <= 0)continue;
							JewelryParam & attr_param = p_nvip.jewelry_param;
							AttrVocInfo & attr_inc = attr_param.attr_extra_inc[attr_type_index];
							if (attr_inc.un_lock != 1)continue;
							attr_inc.attr_type = entry_type_list[attr_type_index].att_type;
							attr_inc.attr_value = attr_value;
						}
					}
				}
				p_nvip.jewelry_param.inc_num = entry_num;
			}
		}

		p_nvip.jewelry_param.has_random_net_value = 1;
		p_nvip.GetData(compound_equip.param_data);
		compound_equip.has_param = 1;
	}

	int sign_id1 = 0;
	int sign_id2 = 0;
	ChatManager::Instance().AddChatItemInfo(&sign_id1, &sign_id2, compound_equip);

	Protocol::SCSystemMsg sm;
	int sendlen = 0;
	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_god_equip_create_notice, robot_name, sign_id1, sign_id2, compound_equip.item_id);
	if (length > 0)
	{
		if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
		{
			World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);
		}
	}
}

WeddingID WorldStatus::CreatWeddingID()
{
	m_wedding_global_param.m_creat_wedding_end++;
	if (1000 > m_wedding_global_param.m_creat_wedding_end)
	{
		m_wedding_global_param.m_creat_wedding_end = 1000;
	}

	return m_wedding_global_param.m_creat_wedding_end;
}

unsigned int WorldStatus::GetHiddenOpenServerTime()
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return m_common_data.hidden_open_server_time;
	}
	else
	{
		return (unsigned int)SHAREDCONFIG->GetOpenServerConfig().GetServerRealStartTime();
	}
	
}

void WorldStatus::SetSyncHiddenOpenServerTime(unsigned int hidden_open_time)
{
	gamelog::g_log_world.printf(LL_INFO, "%s line:%d | m_common_data.hidden_open_server_time:%u hidden_open_time:%u", __FUNCTION__, __LINE__,
		m_common_data.hidden_open_server_time, hidden_open_time);

	if (m_common_data.hidden_open_server_time == hidden_open_time)
	{
		return;
	}

	m_common_data.hidden_open_server_time = hidden_open_time;
	this->SendHiddenInfo();

	EventHandler::Instance().OnHiddenOpenServerTimeChange();
}

void WorldStatus::HiddenToGameSyncHiddenOpenServerTime(int server_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	gamelog::g_log_world.printf(LL_INFO, "%s line:%d |server_id:%d m_common_data.hidden_open_server_time:%u", __FUNCTION__,__LINE__,
		server_id,m_common_data.hidden_open_server_time);

	crossgameprotocal::HiddenGameSyncHiddenOpenServerTime msg;
	msg.hidden_open_server_time = this->GetHiddenOpenServerTime();
	InternalComm::Instance().SendToGameThroughCross2(&msg, sizeof(msg), server_id);
}

void WorldStatus::SendHiddenInfo(Role * role)
{
	Protocol::SCWorldStatusHiddenInfo info;
	info.hidden_open_server_time = GetZeroTime(this->GetHiddenOpenServerTime());

	if (NULL == role)
	{
		World::GetInstWorld()->SendToAllGateway((const char *)&info, sizeof(info));
	}
	else
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &info, sizeof(info));
	}
}

void WorldStatus::GetInitHuanShouAnnParam(HuanShouAnnouncementParam * param)
{
	if(NULL == param) return;

	param->ann_count = 0;
	std::list<HuanShouAnnouncementInfo>::const_iterator it = m_huanshou_announcement_list.begin();
	for (; it != m_huanshou_announcement_list.end() && param->ann_count < ARRAY_ITEM_COUNT(param->ann_list); ++it)
	{
		param->ann_list[param->ann_count++] = *it;
	}
}

WorldStatus::WorldStatus()
	: m_data_state(WORLDSTATE_DATA_STATUS_INVALID), m_activity_data_state(WORLDSTATE_DATA_STATUS_INVALID), m_activity_role_data_state(WORLDSTATE_DATA_STATUS_INVALID), 
	m_start_time(0), m_world_time(0), m_last_update_dayid(0), m_last_pet_broadcast_timestamp(0), m_last_godequip_broadcast_timestamp(0), 
	world_gather_init(false), cross_server_open_day(1), m_next_calc_fake_announcement_timestamp(0)
{
}

WorldStatus::~WorldStatus()
{

}

void WorldStatus::Init(const WorldStatusParam &worldstatus_param)
{
	m_start_time = worldstatus_param.start_time;
	m_world_time = worldstatus_param.world_time;
	m_last_update_dayid = worldstatus_param.last_update_dayid;
	m_common_data = worldstatus_param.common_data;

	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	if (0 == m_start_time) m_start_time = now;
	if (0 == m_last_update_dayid) m_last_update_dayid = static_cast<unsigned int>(EngineAdapter::Instance().DayID());
	if (0 == m_common_data.last_reset_role_module_dayid) m_common_data.last_reset_role_module_dayid = static_cast<unsigned int>(EngineAdapter::Instance().DayID());
	if (0 == m_common_data.last_reset_role_modlue_timestamp) m_common_data.last_reset_role_modlue_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	
	if (0 == m_common_data.next_week_timestamp) m_common_data.next_week_timestamp = now + EngineAdapter::Instance().NextWeekInterval(1, 0, 0, 0);
	if (0 == m_common_data.next_month_timestamp) m_common_data.next_month_timestamp = now + EngineAdapter::Instance().NextMouthInterval(1, 0, 0, 0);
	if (0 == m_common_data.next_week_6_OClock) m_common_data.next_week_6_OClock = now + EngineAdapter::Instance().NextWeekInterval(1, 6, 0, 0);
	if (0 == m_common_data.next_week_6_OClock) m_common_data.next_month_6_OClock = now + EngineAdapter::Instance().NextMouthInterval(1, 6, 0, 0);
	
	m_honor_roll_data = worldstatus_param.honor_roll_data;
	m_courage_challenge_data = worldstatus_param.courage_challenge_data;
	m_chief_election_data = worldstatus_param.chief_election_data;
	m_chief_peak_battle_data = worldstatus_param.chief_peak_battle_data;
	ColosseumManager::Instance().InitDefenderBuffId(worldstatus_param.colosseum_manager_data.buff_num, worldstatus_param.colosseum_manager_data.buff_id_list);

	m_only_fight_data = worldstatus_param.only_fight_data;
	this->OnReCalOnlyFightMap();
	if (2 != m_common_data.is_recalc_top_level_refresh_time_flag)
	{
		m_common_data.is_recalc_top_level_refresh_time_flag = 2;
		this->OnReCalcTopLevelRefreshTimestamp();	
	}
	if (1 != m_common_data.is_calc_top_level_season_start_timestamp_flag)
	{
		m_common_data.is_calc_top_level_season_start_timestamp_flag = 1;
		m_common_data.top_level_season_start_timestamp = m_common_data.next_top_level_refresh_timestamp - TOP_LEVEL_REFRESH_DAY_TIEMS * SECOND_PER_DAY;
	}

	int school_task_count = 0;
	for (int i = 0; i < SCHOOL_TASK_NUM; ++i)
	{
		if (m_common_data.today_school_task_list[i] > 0)
		{
			++school_task_count;
		}
	}
	if (0 == school_task_count)
	{
		LOGIC_CONFIG->GetSchoolTaskConfig()->RandTaskListForServer(m_common_data.today_school_task_list);
	}

	int huanshou_ann_count = GetMin(MAX_HUAN_SHOU_ANNOUNCEMENT_INFO_NUM, (int)worldstatus_param.huan_shou_ann_data.ann_count);
	for (int i = 0; i < huanshou_ann_count && i < ARRAY_ITEM_COUNT(worldstatus_param.huan_shou_ann_data.ann_list); ++i)
	{
		m_huanshou_announcement_list.push_back(worldstatus_param.huan_shou_ann_data.ann_list[i]);
	}

	WildBossManager::Instance().Init(worldstatus_param.wild_boss_data);
	if (0 == m_common_data.is_reset_shan_hai_boss_mark)
	{
		ShanHaiBossParam data; data.Reset();
		ShanHaiBossManager::Instance().Init(data);
		m_common_data.is_reset_shan_hai_boss_mark = 1;
	}
	else
	{
		ShanHaiBossManager::Instance().Init(worldstatus_param.shan_hai_boss_data);
	}
	DropGather::Instance().OnStarDropGather();
	TradeMarketManager::Instance().InitHistoryTopPrice(worldstatus_param.history_top_price_data);
	BigDipperManager::Instance().Init(worldstatus_param.common_data.big_dipper_param);
	QingYuanDuiDuiPengManager::Instance().Init(worldstatus_param.qingyuan_duiduipeng_data);
	KuaFuYingXiongLuManager::Instance().Init(worldstatus_param.kua_fu_ying_xiong_lu_data);

	m_wedding_global_param = worldstatus_param.wedding_global_data;

	{
		RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
		if (NULL != ramgr)
		{
			RandActivityGodBeastAdvent * activity = (RandActivityGodBeastAdvent *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT);
			if (NULL != activity)
			{
				activity->InitBuy(worldstatus_param.god_beast_advent_data);
			}
		}
	}


	this->OnCalcCrossOpenDay();
	this->LoadWorldStatusSucc();
}

void WorldStatus::OnServerStart()
{
	this->LoadWorldStatus(0);
	this->LoadActivityData(0);
	this->LoadActivityRoleData(0);
}

void WorldStatus::OnServerStop()
{
	this->SaveWorldStatus(); 
	this->SaveActivityData(true);
	this->SaveActivityRoleData();
}

void WorldStatus::OnUserLogin(Role * role)
{
	this->SendHiddenInfo(role);
}

void WorldStatus::OnGameServerConnect(std::set<int> server_id)
{
	if (server_id.empty())
	{
		return;
	}

	this->HiddenToGameSyncHiddenOpenServerTime(*server_id.begin());
}

void WorldStatus::Update(unsigned long interval, time_t now_second)
{
	if (WORLDSTATE_DATA_STATUS_LOAD_FINISH != m_data_state) return;

	static time_t last_save_time = now_second;

	if (!CrossConfig::Instance().IsHiddenServer())	//隐藏服不参与这个检测,此时以原服为准,跨服等待原服同步过来
	{
		if (m_common_data.clear_person_rank_flag == 0)
		{
			RankManager::Instance().ClearPersonRankType(PERSON_RANK_TYPE_ENDLESS_TOWER);
			m_common_data.clear_person_rank_flag = 1;
			gamelog::g_log_world.printf(LL_INFO, "WorldStatus::Update ClearPersonRankType !");
		}
	}
	if (0 != m_common_data.next_top_level_refresh_timestamp && (unsigned int)now_second >= m_common_data.next_top_level_refresh_timestamp)
	{
		this->OnReCalcTopLevelRefreshTimestamp(true);
	}
	if ((unsigned int)now_second >= m_common_data.next_quick_log_top_level_timestamp)
	{
		m_common_data.next_quick_log_top_level_timestamp = now_second + TOP_LEVEL_QUICK_LOG_INTERVAL * SECOND_PER_HOUR;
		UserCacheManager::Instance().OnTopLevelLog(m_common_data.top_level_season_start_timestamp, m_common_data.next_top_level_refresh_timestamp);
	}


	{
		bool need_send_all = false;
		if (m_common_data.answer_end_tamp != 0 && m_common_data.answer_end_tamp <= now_second)
		{
			m_common_data.WorldQuestionNextTimesReset();//清除所有信息，除了下次开启活动的时间戳
			need_send_all = true;
		}

		if (m_common_data.answer_tamp != 0 && m_common_data.answer_tamp <= now_second)
		{
			this->OnRefreshAnswerSubject(now_second);
			m_common_data.answer_tamp = 0;
			need_send_all = true;
		}

		if (m_common_data.show_answer_tamp != 0 && m_common_data.show_answer_tamp <= now_second)
		{
			m_common_data.show_answer_tamp = 0;
			m_world_answer_role_map.clear();//清除上一题答题玩家
			need_send_all = true;
		}

		if (m_common_data.next_subject_refresh_tamp != 0 && m_common_data.next_subject_refresh_tamp <= now_second)
		{
			m_common_data.next_subject_refresh_tamp = 0;
			this->RefreshWorldQuestion(now_second);
			need_send_all = true;
		}

		if (m_common_data.next_answer_tamp == 0 || m_common_data.next_answer_tamp <= now_second)
		{
			m_common_data.next_answer_tamp = now_second + 10;//以防找不到对应的10秒后再次尝试
			this->RefreshWorldQuestion(now_second,true);
			need_send_all = true;
		}
		if(need_send_all)this->SendWorldAnswerInfo();
	}
	
	if (now_second >= last_save_time + WORLDSTATUS_SAVE_INTERVAL)
	{
		m_world_time += static_cast<int>(now_second - last_save_time);	

		this->SaveWorldStatus(); 
		this->SaveActivityData();

		last_save_time = now_second;
	}

	if (!world_gather_init)
	{
		bool has_succ_lyjz = false;
		const LYJZOtherConfig * other_cfg = LOGIC_CONFIG->GetLingYaoJiuZhuFbConfig()->GetOtherCfg();
		if (other_cfg != NULL)
		{
			Scene *  gather_scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(other_cfg->scene_ID, COMMON_SCENE_KEY);
			if (gather_scene != NULL)
			{
				NPCGather * gather = NpcManager::Instance().CreateNPCGather(other_cfg->yjcnpc_ID, gather_scene, Posi(other_cfg->x, other_cfg->y));
				if (gather != NULL)
				{
					const int param1 = -100;//常驻采集物
					gather->SetParam(&param1);
					gather->SetRemoveAfterGatherFlag(false);
					has_succ_lyjz = true;
				}
			}
			
		}
		if (!has_succ_lyjz)gamelog::g_log_world.printf(LL_INFO, "WorldStatus::Update world_gather_init fail! LYJZO");

		bool has_succ_yixianqian1 = false;
		const YYYXQOtherConfig * yxq_other_cfg = LOGIC_CONFIG->GetYYYXQConfig()->GetOtherCfg();
		if (other_cfg != NULL)
		{
			Scene *  gather_scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(yxq_other_cfg->scene_ID, COMMON_SCENE_KEY);
			if (gather_scene != NULL)
			{
				NPCGather * gather = NpcManager::Instance().CreateNPCGather(yxq_other_cfg->yjcnpc_ID, gather_scene, Posi(yxq_other_cfg->x, yxq_other_cfg->y));
				if (gather != NULL)
				{
					const int param1 = -101;//常驻采集物
					gather->SetParam(&param1);
					gather->SetRemoveAfterGatherFlag(false);
					has_succ_yixianqian1 = true;
				}
			}

		}
		if (!has_succ_yixianqian1)gamelog::g_log_world.printf(LL_INFO, "WorldStatus::Update world_gather_init fail! YXQ");

		world_gather_init = true;
	}

	{
		//请求跨服开服天数，如果没有则返回等待
		unsigned int real_tamp = 1;
		if (CrossConfig::Instance().IsHiddenServer())
		{
			real_tamp = (unsigned int)SHAREDCONFIG->GetOpenServerConfig().GetOpenServerZeroTime();
		}
		else
		{
			unsigned int cross_open_tamp = CrossManager::Instance().GetCrossOpenTamp();
			real_tamp = cross_open_tamp;
		}

		if (real_tamp > 0)
		{
			const LunHuiWangChuanOtherCfg & other_cfg = LOGIC_CONFIG->GetLunHuiWangChuanConfig()->GetLunHuiWangChuanOhterCfg();
			//检查成就刷新时间
			unsigned int refresh_tamp = WorldStatus::Instance().GetLunHuiWangChuanRefreshTamp();
			if (refresh_tamp <= 0)
			{
				if (other_cfg.refresh_day != 0)
				{
					unsigned int next_zero_tamp = now_second + EngineAdapter::Instance().NextDayInterval(0, 0, 0);
					unsigned int real_refresh_tamp = static_cast<unsigned int>(next_zero_tamp + ((other_cfg.refresh_day - 1) * PER_DAY_SECOND));
					gamelog::g_lun_hui_wang_chuan.printf(LL_INFO, "WorldStatus::Refresh LunHuiWangchuanTamp old_tamp[%d] new_tamp[%d]", refresh_tamp, real_refresh_tamp);
					this->SetLunHuiWangChuanRefreshTamp(real_refresh_tamp);
				}
				
			}

			if (refresh_tamp != 0 && refresh_tamp < now_second)
			{
				unsigned int new_tamp = now_second + EngineAdapter::Instance().NextDayInterval(0, 0, 0) + (PER_DAY_SECOND * (other_cfg.refresh_day - 1));
				if ((new_tamp - now_second) < (PER_DAY_SECOND * (other_cfg.refresh_day - 1)))//如果有刷新时间小于2天的，应该是错误的刷新时间点，修复一下，延迟到下一天
				{
					new_tamp += PER_DAY_SECOND;
				}
				RankManager::Instance().ClearPersonRankType(PERSON_RANK_TYPE_LUN_HUI_WANG_CHUAN_RANK);
				gamelog::g_lun_hui_wang_chuan.printf(LL_INFO, "WorldStatus::Refresh LunHuiWangchuanTamp refresh_tamp[%d] other_cfg.refresh_day[%d] new_tamp[%d]", refresh_tamp, other_cfg.refresh_day, new_tamp);
				this->SetLunHuiWangChuanRefreshTamp(new_tamp);
			}
		}
	}

	if (CrossConfig::Instance().IsHiddenServer())
	{
		if (m_common_data.endless_tower_rank_reward_tamp == 0 || m_common_data.endless_tower_rank_reward_tamp <= now_second)
		{
			m_common_data.endless_tower_rank_reward_tamp = EngineAdapter::Instance().Time() + EngineAdapter::Instance().NextWeekInterval(MONDAY, 0, 0, 0);

			crossgameprotocal::HiddenCrossEndlessRankReward req;
			InternalComm::Instance().SendToCross((const char *)&req, sizeof(req));
		}
	}

	if (!CrossConfig::Instance().IsHiddenServer())
	{		// 条件满足时发一条假洗练/神装打造公告
		time_t open_server_timestamp = World::GetInstWorld()->GetRealServerOpenTime(LocalConfig::Instance().GetUniqueServerID());
		if (now_second >= (open_server_timestamp + LOGIC_CONFIG->GetPetConfig()->GetOtherCfg()->start_broadcast_time) && this->GetWorldLevel() < LOGIC_CONFIG->GetPetConfig()->GetOtherCfg()->world_rank)
		{
			if (now_second >= m_last_pet_broadcast_timestamp)
			{
				Pet::RandRefinedBroadcast();
				m_last_pet_broadcast_timestamp = now_second + LOGIC_CONFIG->GetPetConfig()->GetOtherCfg()->broadcast_interval;
			}
		}

		if (now_second >= (open_server_timestamp + GODEQUIP_CONFIG->GetOtherCfg().start_broadcast_time) && this->GetWorldLevel() < GODEQUIP_CONFIG->GetOtherCfg().world_rank)
		{
			if (now_second >= m_last_godequip_broadcast_timestamp)
			{
				this->RandBroadcastInfo();
				m_last_godequip_broadcast_timestamp = now_second + LOGIC_CONFIG->GetPetConfig()->GetOtherCfg()->broadcast_interval;
			}
		}
	}

	if (0 == m_next_calc_fake_announcement_timestamp || now_second >= (unsigned int)m_next_calc_fake_announcement_timestamp)
	{
		this->OnCalcFakeAnnouncement((unsigned int)now_second);
	}
	this->OnCheckFakeAnnouncement((unsigned int)now_second);
}

void WorldStatus::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid)
	{
		return;
	}
	this->OnCalcCrossOpenDay();
	this->SendWorldLevelInfo();
}

void WorldStatus::OnWeekChange()
{
	m_common_data.open_server_week_num += 1;
	m_common_data.guild_fight_open_times = 0;

	RankManager::Instance().ClearPersonRankType(PERSON_RANK_TYPE_CHIVALROUS); // 每周清空

	static battlegameprotocol::GameBattleForceFinishBattleModeReq req;//强制停止，玩法重置时间
	req.battle_mode = BATTLE_MODE_FALLEN_GOD;
	World::GetInstWorld()->SendMsgToAllBattle((const char*)&req, sizeof(req));

	RankManager::Instance().ClearPersonRankType(PERSON_RANK_TYPE_FALLEN_GOD);
}

void WorldStatus::OnTwoWeekChange()
{
	RankManager::Instance().ClearPersonRankType(PERSON_RANK_TYPE_NEHER_WORLD_PURGATORY);
}

void WorldStatus::OnResetData(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;

	memset(m_common_data.today_school_task_list, 0, sizeof(m_common_data.today_school_task_list));

	LOGIC_CONFIG->GetSchoolTaskConfig()->RandTaskListForServer(m_common_data.today_school_task_list);
}

bool WorldStatus::LoadWorldStatus(long long id_fram)
{
	if (WORLDSTATE_DATA_STATUS_LOAD_FINISH == m_data_state) return false;

	m_data_state = WORLDSTATE_DATA_STATUS_LOADING;

	RMIInitWorldStatusBackObjectImpl *impl = new RMIInitWorldStatusBackObjectImpl();

	RMIGlobalClient gc;
	gc.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	return gc.InitWorldStatusAsyn(id_fram, impl);	
}

void RMIInitWorldStatusBackObjectImpl::InitWorldStatusRet(int ret, const WorldStatusParam &worldstatus_param)
{
	if (ret < 0)
	{
		printf(" RMIInitWorldStatusBackObjectImpl::InitWorldStatusRet Failed \n");
		EngineAdapter::Instance().StopGame();
		return;
	}

	WorldStatus::Instance().Init(worldstatus_param);
}

void WorldStatus::LoadWorldStatusSucc()
{
	m_data_state = WORLDSTATE_DATA_STATUS_LOAD_FINISH;
}

bool WorldStatus::SaveWorldStatus()
{
	static WorldStatusParam worldstate_param; 

	worldstate_param.start_time = m_start_time;
	worldstate_param.world_time = m_world_time;
	worldstate_param.last_update_dayid = m_last_update_dayid;
	worldstate_param.common_data = m_common_data;
	worldstate_param.honor_roll_data = m_honor_roll_data;
	worldstate_param.courage_challenge_data = m_courage_challenge_data;
	worldstate_param.chief_election_data = m_chief_election_data;
	worldstate_param.chief_peak_battle_data = m_chief_peak_battle_data;
	ColosseumManager::Instance().GetDefenderBuffid(COLOSSEUM_DEFENDER_BUFF_NUM, 
		&worldstate_param.colosseum_manager_data.buff_num, 
		worldstate_param.colosseum_manager_data.buff_id_list);

	worldstate_param.only_fight_data = m_only_fight_data;

	WildBossManager::Instance().GetInitParam(&worldstate_param.wild_boss_data);
	ShanHaiBossManager::Instance().GetInitParam(&worldstate_param.shan_hai_boss_data);
	TradeMarketManager::Instance().GetHistoryTopPriceInitParam(worldstate_param.history_top_price_data);
	BigDipperManager::Instance().GetInitParam(&worldstate_param.common_data.big_dipper_param);
	QingYuanDuiDuiPengManager::Instance().GetInitParam(&worldstate_param.qingyuan_duiduipeng_data);
	KuaFuYingXiongLuManager::Instance().GetInitParam(&worldstate_param.kua_fu_ying_xiong_lu_data);
	this->GetInitHuanShouAnnParam(&worldstate_param.huan_shou_ann_data);

	worldstate_param.wedding_global_data = m_wedding_global_param;

	{
		RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
		if (NULL != ramgr)
		{
			RandActivityGodBeastAdvent * activity = (RandActivityGodBeastAdvent *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT);
			if (NULL != activity)
			{
				activity->GetInitBuyParam(worldstate_param.god_beast_advent_data);
			}
		}
	}

	RMISaveWorldStatusBackObjectImpl *impl = new RMISaveWorldStatusBackObjectImpl();

	RMIGlobalClient gc;
	gc.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	return gc.SaveWorldStatusAsyn(worldstate_param, impl);
}

bool WorldStatus::LoadActivityData(long long id_from)
{
	if (WORLDSTATE_DATA_STATUS_LOAD_FINISH == m_activity_data_state) return false;

	m_activity_data_state = WORLDSTATE_DATA_STATUS_LOADING;

	RMIInitActivityDataBackImpl* impl = new RMIInitActivityDataBackImpl();

	RMIGlobalClient gc;
	gc.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	return gc.InitActivityDataAsyn(id_from, impl);
}

void WorldStatus::LoadActivityDataSucc()
{
	m_activity_data_state = WORLDSTATE_DATA_STATUS_LOAD_FINISH;
}

bool WorldStatus::SaveActivityData(bool is_save_all)
{
	static ActivityDataListParam list_param;
	list_param.count = 0;
	list_param.next_id = -1;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
	if (!is_save_all)
	{	
		if (NULL != ramgr)
		{
			ramgr->GetChangedActivityData(&list_param);
		}
		ActivityManager::Instance().GetChangedActivityData(&list_param);
	}
	else
	{
		if (NULL != ramgr)
		{
			ramgr->GetActivityData(&list_param);
		}	
		ActivityManager::Instance().GetActivityData(&list_param);
	}

	if (NULL != ramgr)
	{
		ramgr->ClearDirtyMark();
	}
	
	ActivityManager::Instance().ClearDirtyMark();

	RMISaveActivityDataBackImpl* impl = new RMISaveActivityDataBackImpl();

	RMIGlobalClient gc;
	gc.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	gc.SaveActivityDataAsyn(list_param, impl);

	return true;
}

bool WorldStatus::LoadActivityRoleData(long long inc_id_from)
{
	if (m_activity_role_data_state == WORLDSTATE_DATA_STATUS_LOAD_FINISH)
	{
		return false;
	}

	m_activity_role_data_state = WORLDSTATE_DATA_STATUS_LOADING;

	RMIInitActivityRoleDataBackObjectImpl *impl = new RMIInitActivityRoleDataBackObjectImpl();

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	return ac.InitActivityRoleDataAsyn(inc_id_from, impl);
}

void WorldStatus::LoadActivityRoleDataSucc()
{
	m_activity_role_data_state = WORLDSTATE_DATA_STATUS_LOAD_FINISH;
}

void WorldStatus::SaveActivityRoleData()
{
	ActivityManager::Instance().SaveRoleData();
}

WorldStatus & WorldStatus::Instance()
{
	static WorldStatus ws;
	return ws;
}

unsigned int WorldStatus::GetLastUpdateDayId()
{
	return m_last_update_dayid;
}

void WorldStatus::SetLastUpdateDayId( unsigned int dayid )
{
	m_last_update_dayid = dayid;
}

bool WorldStatus::IsLoadFinish() const
{
	return WORLDSTATE_DATA_STATUS_LOAD_FINISH == m_data_state &&
		WORLDSTATE_DATA_STATUS_LOAD_FINISH == m_activity_data_state &&
		WORLDSTATE_DATA_STATUS_LOAD_FINISH == m_activity_role_data_state;
}

void WorldStatus::InitLoadFinish()
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		UserCacheManager::Instance().OnTopLevelReset(m_common_data.next_top_level_refresh_timestamp);
	}
}


//活动数据init rmi对象
void RMIInitActivityDataBackImpl::InitActivityDataRet(int ret, const ActivityDataListParam& list_param)
{
	gamelog::g_log_debug.printf(LL_INFO, "RMIInitActivityDataBackImpl::InitActivityDataRet ret[%d]", ret);

	if (0 == ret)
	{
		RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());

		for (int i = 0; i < list_param.count && i < ActivityDataListParam::LIST_COUNT; ++i)
		{
			int activity_type = list_param.data_list[i].activity_type;
			if (activity_type >= RAND_ACTIVITY_TYPE_BEGIN && activity_type < RAND_ACTIVITY_TYPE_END) // 属于随机活动
			{
				if (NULL != ramgr)
				{
					ramgr->InitActivity(activity_type, list_param.data_list[i].activity_data);
				}
			}

			if (IsNormalActivity(activity_type))
			{
				ActivityManager::Instance().InitActivity(activity_type, list_param.data_list[i].activity_data);
			}			
		}

		if (list_param.count > 0)
		{
			WorldStatus::Instance().LoadActivityData(list_param.next_id);
		}
		else
		{
			WorldStatus::Instance().LoadActivityDataSucc();
		}
	}
	else
	{
		printf("RMIInitActivityDataBackImpl::InitActivityDataRet failed, ret[%d]\n", ret);
		EngineAdapter::Instance().StopGame();
	}
}

void RMISaveActivityDataBackImpl::SaveActivityDataRet(int ret)
{
	gamelog::g_log_debug.printf(LL_INFO, "RMISaveActivityDataBackImpl::SaveActivityDataRet ret[%d]", ret);
}


void RMIInitActivityRoleDataBackObjectImpl::InitActivityRoleDataRet(int ret, const ActivityRoleDataParam & role_data_param)
{
	if (ret < 0)
	{
		printf("RMIInitActivityRoleDataBackObjectImpl::InitActivityRoleDataRet Failed ret=%d", ret);
		EngineAdapter::Instance().StopGame();
		return;
	}

	long long next_id_from = 0;

	for (int i = 0; i < role_data_param.count; i++)
	{
		const ActivityRoleDataParam::DBActivityRoleDataItem & role_data_item = role_data_param.data_list[i];
		next_id_from = GetMax(next_id_from, role_data_item.inc_id);

		ActivityManager::Instance().InitActivityRoleData(role_data_item);
	}

	if (role_data_param.count > 0)
	{
		WorldStatus::Instance().LoadActivityRoleData(next_id_from);
	}
	else
	{
		WorldStatus::Instance().LoadActivityRoleDataSucc();
	}
}

void FakeAnnouncementInfo::SetData(const FakeAnnouncementCfg * data)
{
	if(NULL == data) return;

	min_level = data->min_level;
	max_level = data->max_level;
	announcement_type = data->announcement_type;
	is_random_name = data->is_random_name;
	item_id = data->item_id;
	descripte_1 = data->descripte_1;
	descripte_2 = data->descripte_2;
	descripte_3 = data->descripte_3;
}
