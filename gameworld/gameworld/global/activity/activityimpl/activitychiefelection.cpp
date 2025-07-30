#include<algorithm>

#include "world.h"
#include "gamelog.h"
#include "scene/scenemanager.hpp"
#include "obj/character/role.h"
#include "engineadapter.h"
#include "global/usercache/usercache.hpp"
#include "global/worldstatus/worldstatus.hpp"
#include "config/logicconfigmanager.hpp"
#include "servercommon/string/gameworldstr.h"
#include "battle/battle_manager_local.hpp"
#include "global/topicmanager/topicmanager.hpp"
#include "internalcomm.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "item/itembase.h"
#include "item/itempool.h"
#include "other/route/mailroute.hpp"
#include "other/funopen/funopenconfig.hpp"
#include "other/funopen/funopendef.hpp"
#include "servercommon/errornum.h"

#include "config/activityconfig/chiefelection/activitychiefelectionconfig.hpp"
#include "scene/speciallogicimpl/speciallogicchiefelection.hpp"
#include "protocol/msgchiefelection.h"
#include "activitychiefelection.hpp"
#include "global/activity/activitymanager.hpp"

int ActivityChiefElection::FIGHTER_NUMBER[ACTIVITY_CHIEF_ELECTION_FIGTHER_NUM] = { 0 };
int ActivityChiefElection::m_seq = 1;

ActivityChiefElection::ActivityChiefElection(ActivityManager * activity_manager) : Activity(activity_manager, ACTIVITY_TYPE_CHIEF_ELECTION)
{
	for (int i = 0; i < ARRAY_LENGTH(FIGHTER_NUMBER); ++i)
	{
		FIGHTER_NUMBER[i] = i + 1;
	}
}

ActivityChiefElection::~ActivityChiefElection()
{
}

void ActivityChiefElection::Init(const ActivityData & arg_data)
{
}

void ActivityChiefElection::GetInitParam(ActivityData & arg_data)
{
}

int ActivityChiefElection::GetMatchSeq()
{
	return m_seq++;
}

void ActivityChiefElection::OnUserLogin(Role * role)
{
	Activity::OnUserLogin(role);

	if (CrossConfig::Instance().IsHiddenServer())
	{
		std::map<int, ActivityChiefElectionPerProfMatchInfo>::iterator iter = m_prof_match_info.find(role->GetBaseProfession());
		if (iter != m_prof_match_info.end())
		{
			iter->second.SendMatchState(role);
			iter->second.SendNoticeToRole(role);			
		}
	}

	if (!CrossConfig::Instance().IsHiddenServer() && this->IsActivityOpen())
	{
		UniqueUserID uuid = role->GetUniqueUserID();
		crossgameprotocal::GameHiddenChiefElectionReq send_hidden_req;
		send_hidden_req.origin_plat_type = uuid.plat_type;
		send_hidden_req.origin_server_id = LocalConfig::Instance().GetMergeServerId();
		send_hidden_req.origin_role_id = UidToInt(uuid.user_id);
		send_hidden_req.base_prof = role->GetBaseProfession();
		send_hidden_req.req_type = Protocol::CHIEF_ELECTION_REQ_GET_INFO;
		send_hidden_req.p1 = 0;
		send_hidden_req.p2 = 0;
		InternalComm::Instance().SendToHiddenThroughCross((const char *)&send_hidden_req, sizeof(send_hidden_req));
	}
}

void ActivityChiefElection::OnUserLoginOriginGame(int role_id)
{
}

void ActivityChiefElection::OnActivityStatusChange(int from_status, int to_status)
{
	Activity::OnActivityStatusChange(from_status, to_status);

	switch (to_status)
	{
		case ACTIVITY_STATUS_CLOSE:
			{
				BattleManagerLocal::GetInstance().ForceFinishBattleMode(BATTLE_MODE_CHIEF_ELECTION);
				BattleManagerLocal::GetInstance().ForceFinishBattleMode(BATTLE_MODE_CHIEF_ELECTION_SEA);

				for (std::map<int, ActivityChiefElectionPerProfMatchInfo>::iterator iter = m_prof_match_info.begin(); iter != m_prof_match_info.end(); ++iter)
				{
					ActivityChiefElectionPerProfMatchInfo & match_info = iter->second;
					match_info.OnActivityClose();
				}

				this->ClearData();
			}
			break;
		case ACTIVITY_STATUS_STANDY:
			{
				this->ClearData();
			}
			break;

		case ACTIVITY_STATUS_OPEN:
			{
				WorldStatus::Instance().ClearChiefElectionInfo();
				this->ClearData();

				const ChiefElectionOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityChiefElectionConfig()->GetOhterCfg();
				for (int prof_num = 1; prof_num < PROFESSION_TYPE_NUM; ++prof_num)
				{			
					int which_prof_match = prof_num * PROFESSION_BASE;
					m_prof_match_info[which_prof_match].which_prof_match = which_prof_match;
					SpecialLogicChiefElectionSea * sea_special_lobgic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicChiefElectionSea(other_cfg.GetSeaElectionSceneid(which_prof_match), COMMON_FB_KEY);
					if (sea_special_lobgic != NULL)
					{
						sea_special_lobgic->SetSceneInfo(which_prof_match);
					}
				}
			}
			break;
	}

	this->SetDirty();
}

void ActivityChiefElection::Update(unsigned long interval, time_t now_second)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	Activity::Update(interval, now_second);

	this->UpdateMatchInfo(now_second);

	if (this->IsActivityOpen())
	{
		if (this->IsAllFinish())
		{
			m_next_status_switch_time = static_cast<unsigned int>(now_second);
		}
		else
		{
			m_next_status_switch_time = UINT_MAX;
		}
	}
}

void ActivityChiefElection::GotoChiefElectionSea(Role * role)
{
	if (role == NULL)
	{
		return;
	}

	if (!this->IsActivityOpen())
	{
		return;
	}

	std::map<int, ActivityChiefElectionPerProfMatchInfo>::iterator iter = m_prof_match_info.find(role->GetBaseProfession());
	if (iter != m_prof_match_info.end())
	{
		ActivityChiefElectionPerProfMatchInfo & match_info = iter->second;
		if (match_info.match_state != ACTIVITY_CHIEF_ELECTION_STATE_SEA_ELECTION_READY)
		{
			role->NoticeNum(errornum::EN_CHIEF_ELECTION_SEA_CANT_ENTER);
			return;
		}

		const ChiefElectionOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityChiefElectionConfig()->GetOhterCfg();
		Posi pos = LOGIC_CONFIG->GetActivityChiefElectionConfig()->GetSeaElectionPos();
		WorldStatus::Instance().LogFunctionStats(role, "ActivityChiefElectionElectionSea");
		World::GetInstWorld()->GetSceneManager()->GoTo(role, other_cfg.GetSeaElectionSceneid(match_info.which_prof_match), COMMON_FB_KEY, pos);
	}
}

void ActivityChiefElection::GotoReadyScene(Role * role)
{
	if (role == NULL)
	{
		return;
	}

	std::map<int, ActivityChiefElectionPerProfMatchInfo>::iterator iter = m_prof_match_info.find(role->GetBaseProfession());
	if (iter != m_prof_match_info.end())
	{
		ActivityChiefElectionPerProfMatchInfo & match_info = iter->second;
		if (match_info.is_match_finish)
		{
			return;
		}

		for (int i = 0; i < ARRAY_LENGTH(match_info.figther_info); ++i)
		{
			if (role->GetUID() == match_info.figther_info[i].role_id)
			{
				gamelog::g_Log_chief_election.printf(LL_INFO, "%s line:%d | GotoReadyScene1 role[%d,%s] prof:%d", __FUNCTION__, __LINE__, role->GetUID(), role->GetName(), role->GetBaseProfession());

				if (match_info.figther_info[i].state == ACTIVITY_CHIEF_ELECTION_FIGHT_RESULT_LOSE || match_info.curr_match_round != 1 || match_info.round_state != ROUND_STATE_READY)
				{
					return;
				}

				match_info.GotoMatchScene(i);
				break;
			}
		}
	}
	else
	{
		gamelog::g_Log_chief_election.printf(LL_INFO, "%s line:%d | GotoReadyScene fail role[%d,%s] prof:%d", __FUNCTION__, __LINE__, role->GetUID(), role->GetName(), role->GetBaseProfession());
	}

}

void ActivityChiefElection::SendMatchInfo(Role * role)
{
	if (role == NULL)
	{
		return;
	}

	Protocol::SCChiefElectionMatchInfo protocol;
	protocol.match_info_count = 0;

	std::map<int, ActivityChiefElectionPerProfMatchInfo>::iterator iter = m_prof_match_info.find(role->GetBaseProfession());

	if (iter != m_prof_match_info.end())
	{
		const ActivityChiefElectionPerProfMatchInfo & match_info = iter->second;
		const ActivityChiefElectionFigtherInfo(&figther_info)[ACTIVITY_CHIEF_ELECTION_FIGTHER_NUM] = match_info.figther_info;
		protocol.curr_round = match_info.curr_match_round;
		protocol.round_state = match_info.round_state;
		protocol.end_time = (unsigned int)match_info.m_next_round_state_time;
		protocol.is_finish = match_info.is_match_finish;
		for (int i = 0; i < ARRAY_LENGTH(figther_info); ++i)
		{
			if (protocol.match_info_count < ARRAY_LENGTH(protocol.match_info))
			{
				protocol.match_info[protocol.match_info_count].role_id = figther_info[i].role_id;
				protocol.match_info[protocol.match_info_count].match_round = figther_info[i].match_round;
				protocol.match_info[protocol.match_info_count].state = figther_info[i].state;
				protocol.match_info[protocol.match_info_count].prof = iter->first;

				UserCacheNode * user_cache_node = UserCacheManager::Instance().GetUserNode(figther_info[i].role_id);
				if (NULL != user_cache_node)
				{
					user_cache_node->GetName(protocol.match_info[protocol.match_info_count].role_name);
					protocol.match_info[i].level = user_cache_node->level;
					protocol.match_info[i].avatar_type = user_cache_node->avatar_type;
					protocol.match_info[i].headshot_id = user_cache_node->appearance.headshot_id;
				}

				protocol.match_info_count += 1;
			}
		}
	}

	unsigned int send_length = sizeof(protocol) - (ARRAY_LENGTH(protocol.match_info) - protocol.match_info_count) * sizeof(protocol.match_info[0]);
	EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, send_length);
}

void ActivityChiefElection::HiddenToGameSendMatchInfo(int role_id, int base_prof)
{
	Protocol::SCChiefElectionMatchInfo protocol;
	protocol.match_info_count = 0;

	std::map<int, ActivityChiefElectionPerProfMatchInfo>::iterator iter = m_prof_match_info.find(base_prof);
	if (iter != m_prof_match_info.end())
	{
		const ActivityChiefElectionPerProfMatchInfo & match_info = iter->second;
		const ActivityChiefElectionFigtherInfo(&figther_info)[ACTIVITY_CHIEF_ELECTION_FIGTHER_NUM] = match_info.figther_info;
		protocol.curr_round = match_info.curr_match_round;
		protocol.round_state = match_info.round_state;
		protocol.end_time = (unsigned int)match_info.m_next_round_state_time;
		protocol.is_finish = match_info.is_match_finish;
		for (int i = 0; i < ARRAY_LENGTH(figther_info); ++i)
		{
			if (protocol.match_info_count < ARRAY_LENGTH(protocol.match_info))
			{
				protocol.match_info[protocol.match_info_count].role_id = figther_info[i].role_id;
				protocol.match_info[protocol.match_info_count].match_round = figther_info[i].match_round;
				protocol.match_info[protocol.match_info_count].state = figther_info[i].state;
				protocol.match_info[protocol.match_info_count].prof = iter->first;

				UserCacheNode * user_cache_node = UserCacheManager::Instance().GetUserNode(figther_info[i].role_id);
				if (NULL != user_cache_node)
				{
					user_cache_node->GetName(protocol.match_info[protocol.match_info_count].role_name);
					protocol.match_info[i].level = user_cache_node->level;
					protocol.match_info[i].avatar_type = user_cache_node->avatar_type;
					protocol.match_info[i].headshot_id = user_cache_node->appearance.headshot_id;
				}

				protocol.match_info_count += 1;
			}
		}
	}

	unsigned int send_length = sizeof(protocol) - (ARRAY_LENGTH(protocol.match_info) - protocol.match_info_count) * sizeof(protocol.match_info[0]);
	InternalComm::Instance().SendToGameUserThroughCross(role_id, &protocol, send_length);
}

void ActivityChiefElection::HiddenToGameSendNoticeToRole(int role_id, int base_prof)
{
	std::map<int, ActivityChiefElectionPerProfMatchInfo>::iterator iter = m_prof_match_info.find(base_prof);
	if (iter != m_prof_match_info.end())
	{
		const ActivityChiefElectionPerProfMatchInfo & match_info = iter->second;
		match_info.HiddenToGameSendNoticeToRoleById(role_id);
	}
}

void ActivityChiefElection::HiddenToGameSendMatchState(int role_id)
{
	for (std::map<int, ActivityChiefElectionPerProfMatchInfo>::iterator iter = m_prof_match_info.begin(); iter != m_prof_match_info.end(); ++iter)
	{
		ActivityChiefElectionPerProfMatchInfo & match_info = iter->second;
		match_info.HiddenToGameSendMatchState(role_id);
	}
}

void ActivityChiefElection::SetFightResult(int role_id, bool is_win, int which_prof_match)
{
	std::map<int, ActivityChiefElectionPerProfMatchInfo>::iterator iter = m_prof_match_info.find(which_prof_match);

	if (iter != m_prof_match_info.end())
	{
		ActivityChiefElectionPerProfMatchInfo & match_info = iter->second;
		match_info.SetFightResult(role_id, is_win, true);
	}
}

void ActivityChiefElection::OnRoleLeaveMatch(int role_id, int which_prof_match)
{
	std::map<int, ActivityChiefElectionPerProfMatchInfo>::iterator iter = m_prof_match_info.find(which_prof_match);

	if (iter != m_prof_match_info.end())
	{
		ActivityChiefElectionPerProfMatchInfo & match_info = iter->second;
		match_info.OnRoleLeaveMatch(role_id);
	}
}

void ActivityChiefElection::OnSeaElectionFinish(int which_prof_match, std::vector<int> sea_election_win_role)
{
	std::map<int, ActivityChiefElectionPerProfMatchInfo>::iterator iter = m_prof_match_info.find(which_prof_match);

	if (iter != m_prof_match_info.end())
	{
		ActivityChiefElectionPerProfMatchInfo & match_info = iter->second;
		match_info.OnSeaElectionFinish(sea_election_win_role);
	}
}

void ActivityChiefElection::SetSeaElectionFinish(int which_prof_match)
{
	std::map<int, ActivityChiefElectionPerProfMatchInfo>::iterator iter = m_prof_match_info.find(which_prof_match);

	if (iter != m_prof_match_info.end())
	{
		ActivityChiefElectionPerProfMatchInfo & match_info = iter->second;
		match_info.SetSeaElectionFinish();
	}
}

bool ActivityChiefElection::GetScene(int role_id, int base_prof, int now_scene_id, int * scene_id, int * scene_key)
{
	if (!LOGIC_CONFIG->GetActivityChiefElectionConfig()->IsElectionChiefScene(now_scene_id))
	{
		return false;
	}

	if (!this->IsActivityOpen() || scene_id == NULL || scene_key == NULL)
	{
		return false;
	}

	std::map<int, ActivityChiefElectionPerProfMatchInfo>::iterator iter = m_prof_match_info.find(base_prof);
	if (iter != m_prof_match_info.end())
	{
		ActivityChiefElectionPerProfMatchInfo & match_info = iter->second;
		return match_info.GetSceneInfo(role_id, scene_id, scene_key);
	}

	return false;
}

bool ActivityChiefElection::CanEnter(int role_id, int base_prof)
{
	std::map<int, ActivityChiefElectionPerProfMatchInfo>::iterator iter = m_prof_match_info.find(base_prof);
	if (iter != m_prof_match_info.end())
	{
		ActivityChiefElectionPerProfMatchInfo & match_info = iter->second;
		if (match_info.is_match_finish)
		{
			return false;
		}

		for (int i = 0; i < ARRAY_LENGTH(match_info.figther_info); ++i)
		{
			if (match_info.figther_info[i].role_id == role_id)
			{
				if (match_info.figther_info[i].match_round >= match_info.curr_match_round && match_info.figther_info[i].state != ACTIVITY_CHIEF_ELECTION_FIGHT_RESULT_LOSE)
				{
					return true;
				}

				break;
			}
		}
	}

	return false;
}

void ActivityChiefElection::GmNextMatchState(int which_prof_match)
{
	std::map<int, ActivityChiefElectionPerProfMatchInfo>::iterator iter = m_prof_match_info.find(which_prof_match);

	if (iter != m_prof_match_info.end())
	{
		ActivityChiefElectionPerProfMatchInfo & match_info = iter->second;

		if (match_info.match_state < ACTIVITY_CHIEF_ELECTION_STATE_ELIMINATION)
		{
			match_info.next_match_state_time = EngineAdapter::Instance().Time();
		}		
	}
}

void ActivityChiefElection::ClearData()
{
	m_prof_match_info.clear();
}

void ActivityChiefElection::UpdateMatchInfo(time_t now_second)
{
	if (!this->IsActivityOpen())
	{
		return;
	}

	for (std::map<int, ActivityChiefElectionPerProfMatchInfo>::iterator iter = m_prof_match_info.begin(); iter != m_prof_match_info.end(); ++iter)
	{
		ActivityChiefElectionPerProfMatchInfo & match_info = iter->second;
		match_info.Update(now_second);
	}
}

bool ActivityChiefElection::IsAllFinish()
{
	for (std::map<int, ActivityChiefElectionPerProfMatchInfo>::iterator iter = m_prof_match_info.begin(); iter != m_prof_match_info.end(); ++iter)
	{
		ActivityChiefElectionPerProfMatchInfo & match_info = iter->second;
		if (!match_info.is_match_finish)
		{
			return false;
		}
	}

	return true;
}

bool ActivityChiefElectionPerProfMatchInfo::SortFuncFinish(const ActivityChiefElectionFigtherInfo & item1, const ActivityChiefElectionFigtherInfo & item2)
{
	if (item1.role_id == 0 && item2.role_id != 0)
		return false;
	if (item1.role_id != 0 && item2.role_id == 0)
		return true;

	if (item1.match_round > item2.match_round)
		return true;
	if (item1.match_round < item2.match_round)
		return false;

	if (item1.state != ACTIVITY_CHIEF_ELECTION_FIGHT_RESULT_LOSE && item2.state == ACTIVITY_CHIEF_ELECTION_FIGHT_RESULT_LOSE)
		return true;
	if (item1.state == ACTIVITY_CHIEF_ELECTION_FIGHT_RESULT_LOSE && item2.state != ACTIVITY_CHIEF_ELECTION_FIGHT_RESULT_LOSE)
		return false;

	if (item1.role_id > item2.role_id)
		return true;
	if (item1.role_id < item2.role_id)
		return false;

	return false;
}

ActivityChiefElectionPerProfMatchInfo::ActivityChiefElectionPerProfMatchInfo()
{
	curr_match_round = 0;
	round_state = ROUND_STATE_INVALID;
	m_next_round_state_time = 0;
	is_match_finish = false;

	which_prof_match = 0;
	figther_info_num = 0;

	match_state = ACTIVITY_CHIEF_ELECTION_STATE_SEA_ELECTION_READY;
	next_match_state_time = 0;

	MAX_MATCH_ROUND = 0;
	int remain = ACTIVITY_CHIEF_ELECTION_FIGTHER_NUM;
	while (remain > 1)
	{
		MAX_MATCH_ROUND += 1;
		remain /= 2;
	}
}

void ActivityChiefElectionPerProfMatchInfo::Update(time_t now_second)
{
	if (match_state < ACTIVITY_CHIEF_ELECTION_STATE_ELIMINATION)
	{
		this->UpdateSeaElection(now_second);
	}
	else
	{
		this->UpdateMatchInfo(now_second);
	}
}

void ActivityChiefElectionPerProfMatchInfo::UpdateSeaElection(time_t now_second)
{
	const ChiefElectionOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityChiefElectionConfig()->GetOhterCfg();

	bool send = false;
	if (next_match_state_time == 0)
	{
		send = true;

		SpecialLogicChiefElectionSea * special_lobgic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicChiefElectionSea(other_cfg.GetSeaElectionSceneid(which_prof_match), COMMON_FB_KEY, false);
		if (special_lobgic == NULL)
		{
			match_state = ACTIVITY_CHIEF_ELECTION_STATE_ELIMINATION;
		}

		if (match_state == ACTIVITY_CHIEF_ELECTION_STATE_SEA_ELECTION_READY)
		{
			next_match_state_time = now_second + other_cfg.sea_election_ready_time;
			special_lobgic->OnReady();
		}

		if (match_state == ACTIVITY_CHIEF_ELECTION_STATE_SEA_ELECTION_FIGHT)
		{
			next_match_state_time = now_second + other_cfg.sea_election_fight_time;
			special_lobgic->OnStart();
		}

		if (match_state == ACTIVITY_CHIEF_ELECTION_STATE_SEA_ELECTION_FINISH)
		{
			next_match_state_time = now_second + 30;
			special_lobgic->OnFinish(true);
		}
	}

	if (now_second >= next_match_state_time && match_state < ACTIVITY_CHIEF_ELECTION_STATE_ELIMINATION) 
	{
		match_state += 1;
		next_match_state_time = 0;
	}

	if (match_state == ACTIVITY_CHIEF_ELECTION_STATE_ELIMINATION)
	{
		send = true;
		m_next_round_state_time = EngineAdapter::Instance().Time();
	}

	if (send)
	{
		this->SendMatchState();
	}
}


void ActivityChiefElectionPerProfMatchInfo::UpdateMatchInfo(time_t now_second)
{
	if (is_match_finish)
	{
		return;
	}

	if (m_next_round_state_time != 0 && now_second >= m_next_round_state_time)
	{
		round_state = (round_state + 1) % ROUND_STATE_MAX;

		if (round_state == ROUND_STATE_READY)
		{
			const ChiefElectionOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityChiefElectionConfig()->GetOhterCfg();
			int ready_duration = curr_match_round == 0 ? other_cfg.first_ready_time : other_cfg.ready_time;

			curr_match_round += 1;
			m_next_round_state_time = now_second + ready_duration;

			gamelog::g_Log_chief_election.printf(LL_INFO, "%s line:%d | prof:%d start_round_ready:%d", __FUNCTION__, __LINE__, which_prof_match, curr_match_round);
		}
		else
		{
			gamelog::g_Log_chief_election.printf(LL_INFO, "%s line:%d | prof:%d start_round_fight:%d", __FUNCTION__, __LINE__, which_prof_match, curr_match_round);
			m_next_round_state_time = now_second + ACTIVITY_CHIEF_ELECTION_BATTLE_TIME;
			this->StartRoundFight();
		}

		this->CheckFinish();
		this->NoticeRoundInfo();
		this->SendMatchInfoUpdate();
	}
}

void ActivityChiefElectionPerProfMatchInfo::NoticeRoundInfo()
{
	if (is_match_finish)
	{
		return;
	}

	Protocol::SCChiefElectionNotice protocol;
	protocol.curr_round = curr_match_round;
	protocol.round_state = round_state;
	protocol.end_time = (unsigned int)m_next_round_state_time;

	for (int i = 0; i < ARRAY_LENGTH(figther_info); ++i)
	{
		if (figther_info[i].match_round == curr_match_round && figther_info[i].state != ACTIVITY_CHIEF_ELECTION_FIGHT_RESULT_LOSE)
		{
			int role_id = figther_info[i].role_id;
			if (role_id == 0)
			{
				continue;
			}


			Role * tmp_role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);

			bool send_ret = false;
			if (tmp_role != NULL)
			{
				send_ret = EngineAdapter::Instance().NetSend(tmp_role->GetNetId(), &protocol, sizeof(protocol));
			}
			else
			{
				send_ret = InternalComm::Instance().SendToGameUserThroughCross(role_id, &protocol, sizeof(protocol));
			}

			gamelog::g_Log_chief_election.printf(LL_INFO, "%s line:%d | prof:%d start_round_fight:%d round_state:%d role_id:%d send_ret:%d is_null:%d", __FUNCTION__, __LINE__, which_prof_match, curr_match_round, round_state,
				role_id, (int)send_ret, (int)(tmp_role == NULL));
		}
	}
}

void ActivityChiefElectionPerProfMatchInfo::SendNoticeToRole(Role * role)
{
	if (role == NULL)
	{
		return;
	}

	if (is_match_finish)
	{
		return;
	}

	Protocol::SCChiefElectionNotice protocol;
	protocol.curr_round = curr_match_round;
	protocol.round_state = round_state;
	protocol.end_time = (unsigned int)m_next_round_state_time;

	for (int i = 0; i < ARRAY_LENGTH(figther_info); ++i)
	{
		if (figther_info[i].role_id == role->GetUID())
		{
			bool send_ret = false;
			if (figther_info[i].match_round == curr_match_round && figther_info[i].state != ACTIVITY_CHIEF_ELECTION_FIGHT_RESULT_LOSE)
			{
				send_ret = EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
			}


			gamelog::g_Log_chief_election.printf(LL_INFO, "%s line:%d | prof:%d start_round_fight:%d round_state:%d role_id:%d send_ret:%d", __FUNCTION__, __LINE__, which_prof_match, curr_match_round, round_state,
				role->GetUID(), (int)send_ret);

			break;
		}
	}
}

void ActivityChiefElectionPerProfMatchInfo::HiddenToGameSendNoticeToRoleById(int role_id) const
{
	if (is_match_finish)
	{
		return;
	}

	Protocol::SCChiefElectionNotice protocol;
	protocol.curr_round = curr_match_round;
	protocol.round_state = round_state;
	protocol.end_time = (unsigned int)m_next_round_state_time;

	for (int i = 0; i < ARRAY_LENGTH(figther_info); ++i)
	{
		if (figther_info[i].role_id == role_id)
		{
			if (figther_info[i].match_round == curr_match_round && figther_info[i].state != ACTIVITY_CHIEF_ELECTION_FIGHT_RESULT_LOSE && curr_match_round == 1)
			{
				InternalComm::Instance().SendToGameUserThroughCross(role_id, &protocol, sizeof(protocol));
			}

			break;
		}
	}
}

void ActivityChiefElectionPerProfMatchInfo::CheckFinish()
{
	int win_num = 0;				//比赛中获胜的人还有多少个，小于等于 1 就能结束这场比赛
	int round_role_num = 0;			//当前回合还没打完的人数
	for (int i = 0; i < ARRAY_LENGTH(figther_info); ++i)
	{
		const ActivityChiefElectionFigtherInfo & info = figther_info[i];

		if (info.role_id == 0)
		{
			continue;
		}

		if (info.match_round > curr_match_round && info.state != ACTIVITY_CHIEF_ELECTION_FIGHT_RESULT_LOSE)
		{
			win_num += 1;
		}

		if (info.match_round == curr_match_round && info.state != ACTIVITY_CHIEF_ELECTION_FIGHT_RESULT_LOSE)
		{
			round_role_num += 1;
		}
	}

	if (win_num <= 1 && round_role_num == 0)
	{
		this->OnMatchFinish();
	}

	if (round_role_num <= 0)
	{
		m_next_round_state_time = EngineAdapter::Instance().Time();
	}
}

void ActivityChiefElectionPerProfMatchInfo::OnActivityClose()
{
	const ChiefElectionOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityChiefElectionConfig()->GetOhterCfg();
	SpecialLogicChiefElectionSea * sea_special_lobgic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicChiefElectionSea(other_cfg.GetSeaElectionSceneid(which_prof_match), COMMON_FB_KEY, false);
	if (sea_special_lobgic != NULL)
	{
		sea_special_lobgic->OnActivityClose();
	}

	for (int i = 0; i < ARRAY_LENGTH(figther_info); ++i)
	{
		ActivityChiefElectionFigtherInfo & figher_item = figther_info[i];

		if (figher_item.role_id == 0 || figher_item.state == ACTIVITY_CHIEF_ELECTION_FIGHT_RESULT_LOSE)
		{
			continue;
		}

		SpecialLogicChiefElection * special_lobgic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicChiefElection(figher_item.scene_info.scene_id, figher_item.scene_info.scene_key, false);
		if (special_lobgic != NULL)
		{
			special_lobgic->OnActivityClose();
		}

		figher_item.state = ACTIVITY_CHIEF_ELECTION_FIGHT_RESULT_LOSE;
	}

	this->OnMatchFinish();
}

void ActivityChiefElectionPerProfMatchInfo::GotoMatchScene(int figther_index)
{
	if (figther_index < 0 || figther_index >= ARRAY_LENGTH(figther_info))
	{
		return;
	}

	ActivityChiefElectionFigtherInfo & self_info = figther_info[figther_index];
	if (self_info.state == ACTIVITY_CHIEF_ELECTION_FIGHT_RESULT_LOSE)
	{
		return;
	}


	self_info.scene_info.scene_key = ActivityChiefElection::GetMatchSeq();
	self_info.scene_info.scene_id = LOGIC_CONFIG->GetActivityChiefElectionConfig()->GetReadySceneId(self_info.match_round);

	ActivityChiefElectionFigtherInfo * opponent_info = this->GetOpponentInfo(figther_index);
	if (opponent_info != NULL)
	{
		self_info.opponent_number = opponent_info->number;
		opponent_info->opponent_number = self_info.number;

		if (opponent_info->scene_info.scene_id != 0)
		{
			self_info.scene_info = opponent_info->scene_info;
		}
	}

	gamelog::g_Log_chief_election.printf(LL_INFO, "%s line:%d | prof:%d role_id:%d scene_id:%d,scene_key:%d", __FUNCTION__, __LINE__,
		which_prof_match, self_info.role_id, self_info.scene_info.scene_id, self_info.scene_info.scene_key);

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(self_info.role_id);
	if (role != NULL)
	{
		WorldStatus::Instance().LogFunctionStats(role, "ActivityChiefMatchSea");
		World::GetInstWorld()->GetSceneManager()->GoTo(role, self_info.scene_info.scene_id, self_info.scene_info.scene_key);
	}
}

bool ActivityChiefElectionPerProfMatchInfo::GetSceneInfo(int role_id, int * scene_id, int * scene_key)
{
	for (int i = 0; i < ARRAY_LENGTH(figther_info); ++i)
	{
		if (role_id == figther_info[i].role_id)
		{
			if (is_match_finish || match_state != ACTIVITY_CHIEF_ELECTION_STATE_ELIMINATION ||
			    figther_info[i].state == ACTIVITY_CHIEF_ELECTION_FIGHT_RESULT_LOSE || figther_info[i].match_round < curr_match_round)
			{
				return false;
			}

			*scene_id = figther_info[i].scene_info.scene_id;
			*scene_key = figther_info[i].scene_info.scene_key;
			return true;
		}
	}

	return false;
}

void ActivityChiefElectionPerProfMatchInfo::StartRoundFight()
{
	for (int i = 0; i < ARRAY_LENGTH(figther_info); ++i)
	{
		ActivityChiefElectionFigtherInfo & figher_item = figther_info[i];

		if (figher_item.role_id == 0 || figher_item.match_round != curr_match_round || figher_item.state == ACTIVITY_CHIEF_ELECTION_FIGHT_RESULT_LOSE)
		{
			continue;
		}

		gamelog::g_Log_chief_election.printf(LL_INFO, "%s  line:%d | prof:%d role_id:%d scene_id:%d scene_key:%d", __FUNCTION__, __LINE__, which_prof_match, figher_item.role_id, figher_item.scene_info.scene_id, figher_item.scene_info.scene_key);

		SpecialLogicChiefElection * special_lobgic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicChiefElection(figher_item.scene_info.scene_id, figher_item.scene_info.scene_key);
		if (special_lobgic != NULL)
		{
			special_lobgic->StartRoundFight(figher_item.role_id, this->GetOpponentRoleId(i), which_prof_match);
		}
		else
		{
			gamelog::g_Log_chief_election.printf(LL_INFO, "%s  line:%d | prof:%d scene_id:%d scene_key:%d is NULL", __FUNCTION__, __LINE__, which_prof_match, figher_item.scene_info.scene_id, figher_item.scene_info.scene_key);
			this->SetFightResult(figher_item.role_id, false);
		}
	}

	this->CheckFinish();
}

void ActivityChiefElectionPerProfMatchInfo::SetFightResult(int role_id, bool is_win, bool is_check_finish)
{
	if (is_match_finish || role_id == 0)
	{
		return;
	}

	gamelog::g_Log_chief_election.printf(LL_INFO, "%s line:%d | prof:%d role_id:%d is_win:%d curr_match_round:%d", __FUNCTION__, __LINE__, 
					     which_prof_match, role_id, (int)is_win, curr_match_round);

	for (int i = 0; i < ARRAY_LENGTH(figther_info); ++i)
	{
		ActivityChiefElectionFigtherInfo & info = figther_info[i];
		if (info.role_id == role_id)
		{
			if (info.state == ACTIVITY_CHIEF_ELECTION_FIGHT_RESULT_LOSE)
			{
				break;
			}

			if (info.match_round < curr_match_round)
			{
				is_win = false;

				int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_chief_election_sea_tip4);
				World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(role_id, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT));
			}

			gamelog::g_Log_chief_election.printf(LL_INFO, "%s line:%d | prof:%d role_id:%d is_win:%d role_match_round:%d curr_match_round:%d", __FUNCTION__, __LINE__,
							     which_prof_match, role_id, (int)is_win, info.match_round, curr_match_round);

			SpecialLogicChiefElection * special_lobgic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicChiefElection(info.scene_info.scene_id, info.scene_info.scene_key, false);
			if (special_lobgic != NULL)
			{
				special_lobgic->OnFinish(role_id);
			}

			if (is_win)
			{
				info.match_round += 1;
				this->GotoMatchScene(i);
			}
			else
			{
				info.state = ACTIVITY_CHIEF_ELECTION_FIGHT_RESULT_LOSE;
			}

			this->SendBattleReward(role_id, is_win, curr_match_round);
			this->SendBattleResultNotice(role_id, is_win);
			this->SendMatchInfoUpdate();

			break;
		}
	}
	if (is_check_finish)
	{
		this->CheckFinish();
	}
}

void ActivityChiefElectionPerProfMatchInfo::SendBattleReward(int role_id, bool is_win, int match_round)
{
	int role_level = 1;

	Role * tmp_role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
	if (tmp_role != NULL)
	{
		role_level = tmp_role->GetLevel();
	}
	else
	{
		UserCacheNode * user_cache_node = UserCacheManager::Instance().GetUserNode(role_id);
		if (NULL != user_cache_node)
		{
			role_level = user_cache_node->level;
		}
	}


	// 发邮件
	static MailContentParam contentparam; contentparam.Reset();

	std::vector<ItemConfigData> rewards_vec;
	const ActivityChiefElectionBattleRewardCfg * cfg = LOGIC_CONFIG->GetActivityChiefElectionConfig()->GetPerBattleRewardCfg(role_level, match_round);
	if (cfg != NULL)
	{
		contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = is_win ? cfg->winner_coin : cfg->loser_coin;
		contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_EXP] = is_win ? cfg->winner_exp : cfg->loser_exp;
		if (!is_win)
		{
			rewards_vec.insert(rewards_vec.end(), cfg->loser_rewards.begin(), cfg->loser_rewards.end());
		}
	}

	int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_chief_election_battle_reward_subject);
	int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_chief_election_battle_reward_content, match_round, is_win);
	if (match_round >= MAX_MATCH_ROUND && is_win)
	{
		// 发冠军物品邮件
		const ChiefElectionRewardCfg * reward_cfg = LOGIC_CONFIG->GetActivityChiefElectionConfig()->GetRewardCfg(which_prof_match);
		if (reward_cfg != NULL)
		{
			rewards_vec.insert(rewards_vec.end(), reward_cfg->rewards.begin(), reward_cfg->rewards.end());
		}

		lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_chief_election_reward_subject);
		length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_chief_election_reward_content, which_prof_match);
	}

	FillMailContentParam(contentparam, rewards_vec);

	if (lensub > 0 && length > 0)
	{
		MailRoute::MailToUser(role_id, MAIL_REASON_DEFAULT, contentparam);
	}
}

void ActivityChiefElectionPerProfMatchInfo::SendBattleResultNotice(int role_id, int is_win)
{
	Protocol::SCChiefElectionFighterFinishNotice protocol;
	protocol.role_id = role_id;
	protocol.is_win = is_win;

	for (int i = 0; i < ARRAY_LENGTH(figther_info); ++i)
	{
		if (figther_info[i].match_round >= curr_match_round || figther_info[i].role_id == role_id)
		{
			Role * tmp_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(figther_info[i].role_id));
			if (tmp_role != NULL)
			{
				EngineAdapter::Instance().NetSend(tmp_role->GetNetId(), &protocol, sizeof(protocol));
			}
		}
	}
}

void ActivityChiefElectionPerProfMatchInfo::SendMatchInfoUpdate() const
{
	Protocol::SCChiefElectionMatchInfo protocol;
	protocol.match_info_count = 0;

	protocol.curr_round = curr_match_round;
	protocol.round_state = round_state;
	protocol.end_time = (unsigned int)m_next_round_state_time;
	protocol.is_finish = is_match_finish;
	for (int i = 0; i < ARRAY_LENGTH(figther_info); ++i)
	{
		if (protocol.match_info_count < ARRAY_LENGTH(protocol.match_info))
		{
			protocol.match_info[protocol.match_info_count].role_id = figther_info[i].role_id;
			protocol.match_info[protocol.match_info_count].match_round = figther_info[i].match_round;
			protocol.match_info[protocol.match_info_count].state = figther_info[i].state;
			protocol.match_info[protocol.match_info_count].prof = which_prof_match;

			UserCacheNode * user_cache_node = UserCacheManager::Instance().GetUserNode(figther_info[i].role_id);
			if (NULL != user_cache_node)
			{
				user_cache_node->GetName(protocol.match_info[protocol.match_info_count].role_name);
				protocol.match_info[i].level = user_cache_node->level;
				protocol.match_info[i].avatar_type = user_cache_node->avatar_type;
				protocol.match_info[i].headshot_id = user_cache_node->appearance.headshot_id;
			}

			protocol.match_info_count += 1;
		}
	}

	unsigned int send_length = sizeof(protocol) - (ARRAY_LENGTH(protocol.match_info) - protocol.match_info_count) * sizeof(protocol.match_info[0]);
	TopicManager::Instance()->OnTopicUpdate(TOPIC_TYPE_CHIEF_ELECTION_START + (which_prof_match / PROFESSION_BASE - 1), (const char *)&protocol, send_length);
}

void ActivityChiefElectionPerProfMatchInfo::OnMatchFinish()
{
	if (is_match_finish)
	{
		return;
	}
	is_match_finish = true;
	match_state = ACTIVITY_CHIEF_ELECTION_STATE_FINISH;
	this->SendMatchState();

	std::vector<ActivityChiefElectionFigtherInfo> temp_figther_info(figther_info, figther_info + ARRAY_LENGTH(figther_info));		//复制一下，原本那个客户端还要显示
	std::sort(temp_figther_info.begin(), temp_figther_info.end(), &ActivityChiefElectionPerProfMatchInfo::SortFuncFinish);

	ActivityChiefElection * activity = (ActivityChiefElection *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_CHIEF_ELECTION);
	if (NULL == activity)
	{
		return;
	}

	const ActivityChiefElectionFigtherInfo & top_info = temp_figther_info[0];
	if (top_info.role_id != 0 && top_info.state != ACTIVITY_CHIEF_ELECTION_FIGHT_RESULT_LOSE)
	{
		SpecialLogicChiefElection * special_lobgic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicChiefElection(top_info.scene_info.scene_id, top_info.scene_info.scene_key);
		if (special_lobgic != NULL)
		{
			special_lobgic->OnFinish(top_info.role_id, true);
		}

		bool ret = WorldStatus::Instance().OnChiefElectionFinish(top_info.role_id, which_prof_match);
		if (ret)
		{
			UserCacheNode * uc = UserCacheManager::Instance().GetUserNode(top_info.role_id);
			if (NULL != uc)
			{
				int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_chief_election_champion_broadcast, which_prof_match, top_info.role_id, uc->GetNamePtr(), which_prof_match);
				if (length > 0)
				{
					World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT),
											       0, 0, activity->GetActivityLimitLevel(activity->GetActivityType()), 0);
				}
			}
		}

		//胜利者会下发剩余战斗场次奖励，2个人打的时候，可能经过1轮就结束，正常来说要打多轮
		for (int role_match_round = top_info.match_round; role_match_round <= MAX_MATCH_ROUND; ++role_match_round)
		{
			this->SendBattleReward(top_info.role_id, true, role_match_round);
		}
	}
	else
	{
		// 传闻
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_chief_election_no_winner_broadcast, which_prof_match);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT),
									       0, 0, activity->GetActivityLimitLevel(activity->GetActivityType()), 0);
		}
	}
}

void ActivityChiefElectionPerProfMatchInfo::OnRoleLeaveMatch(int role_id)
{

}

void ActivityChiefElectionPerProfMatchInfo::SendMatchState(Role * role)
{
	Protocol::SCChiefElectionStateInfo protocol;
	protocol.which_prof_match = which_prof_match;
	protocol.match_state = match_state;
	protocol.next_match_state_time = (unsigned int)next_match_state_time;

	if (role == NULL)
	{
		World::GetInstWorld()->SendToAllGateway((char *)&protocol, sizeof(protocol));
		InternalComm::Instance().SendToAllGameUserThroughCross((char *)&protocol, sizeof(protocol));
	}
	else
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
	}
}

void ActivityChiefElectionPerProfMatchInfo::HiddenToGameSendMatchState(int role_id)
{
	Protocol::SCChiefElectionStateInfo protocol;
	protocol.which_prof_match = which_prof_match;
	protocol.match_state = match_state;
	protocol.next_match_state_time = (unsigned int)next_match_state_time;

	InternalComm::Instance().SendToGameUserThroughCross(role_id, (const char *)&protocol, sizeof(protocol));
}

void ActivityChiefElectionPerProfMatchInfo::OnSeaElectionFinish(std::vector<int> sea_election_win_role)
{
	//打乱1~16的号码数组
	std::random_shuffle(ActivityChiefElection::FIGHTER_NUMBER, ActivityChiefElection::FIGHTER_NUMBER + ARRAY_LENGTH(ActivityChiefElection::FIGHTER_NUMBER));

	figther_info_num = (int)sea_election_win_role.size();
	for (int i = 0; i < figther_info_num; ++i)
	{
		int number = ActivityChiefElection::FIGHTER_NUMBER[i];
		ActivityChiefElectionFigtherInfo & info = figther_info[number - 1];
		info.role_id = sea_election_win_role[i];
		info.number = number;

		gamelog::g_Log_chief_election.printf(LL_INFO, "%s line:%d | %d prof:%d role_id:%d number:%d", __FUNCTION__, __LINE__, i + 1, which_prof_match, info.role_id, info.number);
	}	
}

void ActivityChiefElectionPerProfMatchInfo::SetSeaElectionFinish()
{
	if (match_state >= ACTIVITY_CHIEF_ELECTION_STATE_SEA_ELECTION_FINISH)
	{
		return;
	}
	match_state = ACTIVITY_CHIEF_ELECTION_STATE_SEA_ELECTION_FINISH;
	next_match_state_time = 0;
}

ActivityChiefElectionFigtherInfo * ActivityChiefElectionPerProfMatchInfo::GetOpponentInfo(int figther_index)
{
	if (figther_index < 0 || figther_index >= ARRAY_LENGTH(figther_info))
	{
		return NULL;
	}

	const ActivityChiefElectionFigtherInfo & info = figther_info[figther_index];
	for (int opponent_index = 0; opponent_index < ARRAY_LENGTH(figther_info); ++opponent_index)
	{
		ActivityChiefElectionFigtherInfo & opponent_info = figther_info[opponent_index];

		if (opponent_info.role_id == 0 || opponent_info.role_id == info.role_id || opponent_info.match_round != info.match_round)
		{
			continue;
		}

		unsigned int flag = ~((unsigned int)0) << info.match_round;
		if ((figther_index & flag) == (opponent_index & flag))
		{
			return &opponent_info;
		}
	}

	return NULL;
}

int ActivityChiefElectionPerProfMatchInfo::GetOpponentRoleId(int figther_index)
{
	if (figther_index < 0 || figther_index >= ARRAY_LENGTH(figther_info))
	{
		return 0;
	}

	const ActivityChiefElectionFigtherInfo & info = figther_info[figther_index];
	for (int opponent_index = 0; opponent_index < ARRAY_LENGTH(figther_info); ++opponent_index)
	{
		ActivityChiefElectionFigtherInfo & opponent_info = figther_info[opponent_index];

		if (opponent_info.role_id == 0 || opponent_info.role_id == info.role_id || opponent_info.match_round != info.match_round)
		{
			continue;
		}

		unsigned int flag = ~((unsigned int)0) << info.match_round;
		if ((figther_index & flag) == (opponent_index & flag))
		{
			return opponent_info.role_id;
		}
	}

	return 0;
}
