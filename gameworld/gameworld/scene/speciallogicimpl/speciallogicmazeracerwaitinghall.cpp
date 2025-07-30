#include "scene/speciallogicimpl/speciallogicmazeracerwaitinghall.hpp"
#include "world.h"
#include "gamelog.h"
#include "engineadapter.h"
#include "mazeracermanager.hpp"
#include "global/team/team.hpp"
#include "global/activity/activitymanager.hpp"
#include "global/activity/activityimpl/activitymazeracer.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/activityconfig/mazeracer/activitymazeracerconfig.hpp"
#include "scene/scene.h"
#include "obj/character/role.h"
#include "obj/otherobj/treasurechest.h"
#include "protocol/msgactivity.hpp"
#include "global/team/teammanager.hpp"
#include "servercommon/errornum.h"
#include "item/knapsack.h"
#include "other/rolemodulemanager.hpp"

SpecialLogicMazeRacerWaitingHall::SpecialLogicMazeRacerWaitingHall(Scene * scene) : SpecialLogic(scene)
{
	this->Reset();
}

SpecialLogicMazeRacerWaitingHall::~SpecialLogicMazeRacerWaitingHall()
{
}

void SpecialLogicMazeRacerWaitingHall::Reset()
{
	const MazeRacerWaitingHall * mr_wh_cfg = LOGIC_CONFIG->GetActivityMazeRacerConfig()->GetWaitingHallInfoBySceneID(m_scene->GetSceneID());
	if (NULL != mr_wh_cfg)
	{
		m_target_maze_id = mr_wh_cfg->target_maze_id;
	}
	else
	{
		m_target_maze_id = 0;
	}

	const MazeRacerOthersCfg & mr_other_cfg = LOGIC_CONFIG->GetActivityMazeRacerConfig()->GetOthersConfig();
	m_next_begin_timestamp = EngineAdapter::Instance().Time() + mr_other_cfg.waiting_interval;

	m_need_send_info = false;
	m_next_send_timestamp = 0;
	m_has_role_enter = false;

	this->ExtinctAllChest();
	m_chest_list.clear();
	next_box_refresh_tamp = 0;
	m_box_num = 0;
	is_refresh_box_flag = false;
}

bool SpecialLogicMazeRacerWaitingHall::CanDestroy()
{
	ActivityMazeRacer * act_maze_racer = (ActivityMazeRacer *)(ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_MAZE_RACER));
	return NULL == act_maze_racer || !act_maze_racer->IsActivityOpen();
}

void SpecialLogicMazeRacerWaitingHall::Update(unsigned long interval, time_t now_second)
{
	if (m_need_send_info && now_second >= m_next_send_timestamp)
	{
		m_need_send_info = false;
		m_next_send_timestamp = now_second + 3;
		this->SendWaitingHallInfo();
	}

	if (m_has_role_enter)
	{
		m_has_role_enter = false;

		// 如果场景里满人了，就传送
		const MazeRacerOthersCfg & mr_other_cfg = LOGIC_CONFIG->GetActivityMazeRacerConfig()->GetOthersConfig();
		if (m_scene->RoleNum() >= mr_other_cfg.waiting_num_max)
		{
			this->TransAllTeamIntoEctype();
		}
	}

	// 刷新宝箱
	if (is_refresh_box_flag && now_second >= next_box_refresh_tamp)
	{
		const MazeRacerOthersCfg & other_cfg = LOGIC_CONFIG->GetActivityMazeRacerConfig()->GetOthersConfig();
		if (m_box_num < other_cfg.box_max_num)
		{
			this->OnRefreshChest();
		}
		next_box_refresh_tamp = now_second + other_cfg.box_refresh_time;
	}

	if (now_second >= m_next_begin_timestamp) this->TransAllTeamIntoEctype();
}

void SpecialLogicMazeRacerWaitingHall::OnRoleEnterScene(Role * role)
{
	if (NULL == role) return;

	ActivityMazeRacer * act_ref = (ActivityMazeRacer *)(ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_MAZE_RACER));
	if (NULL == act_ref || !act_ref->IsActivityOpen())
	{
		this->DelayKickOutRole(role);
		return;
	}
	
	Team * team = role->GetMyTeam();
	if (NULL == team)
	{
		this->DelayKickOutRole(role);
		return;
	}

	m_need_send_info = true;
	m_has_role_enter = true;
	m_team_index_list.insert(team->GetTeamIndex());
	this->SendWaitingHallInfo(role);
}

void SpecialLogicMazeRacerWaitingHall::OnRoleLeaveScene(Role * role, bool is_logout)
{
	if (NULL == role) return;

	m_need_send_info = true;
}

void SpecialLogicMazeRacerWaitingHall::OnRoleLeaveTeam(Role * role)
{
	if (NULL == role) return;

	this->DelayKickOutRole(role);
}

void SpecialLogicMazeRacerWaitingHall::OnTeamDismiss(Role * leader_role, Team * team)
{
	if (NULL == team) return;

	int team_index = team->GetTeamIndex();
	m_team_index_list.erase(team_index);
	int member_num = team->GetMemberCount();
	for (int i = 0; i < member_num; ++i)
	{
		this->DelayKickOutRole(team->GetMemberRoleByIndex(i));
	}
}

bool SpecialLogicMazeRacerWaitingHall::CanTeamMemberGotoLeaderFromOtherScene(Role* member, Role* leader)
{
	return true;
}

bool SpecialLogicMazeRacerWaitingHall::CanInvitationRole(Role * role, int uid)
{
	return false;
}

bool SpecialLogicMazeRacerWaitingHall::OnOpenTreasureChest(Role * role, TreasureChest * chest_obj)
{
	if (NULL == role || NULL == chest_obj) return false;
	Team * team = role->GetMyTeam();
	if (NULL != team)
	{
		if (role->GetUID() != team->GetLeaderUID())
		{
			role->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
			return false;
		}

		// 所有队员随机一次奖励
		int mem_count = team->GetMemberCount();
		for (int mem_idx = 0; mem_idx < mem_count; ++mem_idx)
		{
			Role * mem_role = team->GetMemberRoleByIndex(mem_idx);
			if (NULL != mem_role) this->RandAndPutChestAwardInBag(mem_role, chest_obj->GetTreasureChestid(), team->IsLeader(mem_role));
		}
	}
	else	// 如果是单人开箱，只自己随机一次奖励
	{
		if (!this->RandAndPutChestAwardInBag(role, chest_obj->GetTreasureChestid()))
		{
			return false;
		}
	}
	if (m_box_num > 0)m_box_num -= 1;
	return true;
}

void SpecialLogicMazeRacerWaitingHall::OnActivityOpen()
{
	//活动正式开始才刷宝箱
	is_refresh_box_flag = true;
}

void SpecialLogicMazeRacerWaitingHall::OnActivityEnd()
{
	//活动结束清除宝箱
	this->ExtinctAllChest();
	is_refresh_box_flag = false;
}

void SpecialLogicMazeRacerWaitingHall::GmTransportImmidiately()
{
	m_next_begin_timestamp = EngineAdapter::Instance().Time();
}

void MazeRacerTeamMemberGoToFunction(int role_id, int target_scene_id, int target_scene_key, bool go_ret, void * param);
void SpecialLogicMazeRacerWaitingHall::TransAllTeamIntoEctype()
{
	const MazeRacerOthersCfg & mr_other_cfg = LOGIC_CONFIG->GetActivityMazeRacerConfig()->GetOthersConfig();
	m_next_begin_timestamp = EngineAdapter::Instance().Time() + mr_other_cfg.waiting_interval;

	if (m_team_index_list.empty()) return;		// 如果大厅里没人，就不管了；

	int scene_key = 0, scene_id = 0;
	if (MazeRacerManager::Instance().InitMazeCopy(m_target_maze_id, &scene_key, &scene_id))
	{
		SpecialLogicMazeRacer * sp_mr = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicMazeRacer(scene_id, scene_key, false);
		if (NULL == sp_mr) return;
		
		for ( std::set<int>::iterator loop = m_team_index_list.begin(); loop != m_team_index_list.end(); ++loop)
		{
			Team * team = TeamManager::Instance().GetTeamByTeamIndex(*loop);
			if (team == NULL)
			{
				continue;
			}

			Role * leader = team->GetLeaderRole();
			if (leader == NULL || leader->GetScene()->GetSceneType() != this->GetSceneType())
			{
				continue;
			}

			Posi team_birth = sp_mr->GetScene()->GetARandPosi();		// 队伍传送目标地点，每个队伍都不一样
			TeamManager::Instance().TeamGotoScene(team, scene_id, scene_key, team_birth, false, MazeRacerTeamMemberGoToFunction, NULL);
		}			
	}

	m_team_index_list.clear();
	this->DelayKickOutAllRole();		// 没有成功传入副本的 队伍/玩家 直接踢出大厅
}

void SpecialLogicMazeRacerWaitingHall::SendWaitingHallInfo(Role * role)
{
	int role_num = m_scene->RoleNum();		// 如果场景里没人，那就没必要发了
	if (role_num <= 0) return;

	static Protocol::SCActivityMazeRacerWaitsInfo amrwi;

	amrwi.next_begin_timestamp = (UInt32)m_next_begin_timestamp;
	amrwi.cur_waiting_role_num = role_num;
	amrwi.min_level = 0;
	amrwi.max_level = 0;

	const MazeRacerWaitingHall * mr_wh_cfg = LOGIC_CONFIG->GetActivityMazeRacerConfig()->GetWaitingHallInfoBySceneID(m_scene->GetSceneID());
	if (NULL != mr_wh_cfg)
	{

		amrwi.min_level = mr_wh_cfg->min_level_limit;
		amrwi.max_level = mr_wh_cfg->max_level_limit;
	}

	if (role == NULL)
	{
		m_scene->SendToRole((const char *)&amrwi, sizeof(amrwi));
	}
	else
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &amrwi, sizeof(amrwi));
	}
}

void SpecialLogicMazeRacerWaitingHall::OnRefreshChest()
{
	std::vector<MazeRaceDeleteData> delete_vec;
	for (std::map<int, std::set<ObjID> >::iterator chest_obj_it = m_chest_list.begin();chest_obj_it != m_chest_list.end(); chest_obj_it++)
	{
		for (std::set<ObjID>::iterator delete_obj_id_it = chest_obj_it->second.begin(); delete_obj_id_it != chest_obj_it->second.end(); ++delete_obj_id_it)
		{
			Obj * target_obj = m_scene->GetObj(*delete_obj_id_it);
			if (target_obj != NULL)
			{
				if (target_obj->GetObjType() == Obj::OBJ_TYPE_TREASURE_CHEST)
				{
					TreasureChest *chest_obj = (TreasureChest *)target_obj;
					if (chest_obj->IsOpen())
					{
						m_scene->DeleteObj(*delete_obj_id_it);
						delete_vec.push_back(MazeRaceDeleteData(chest_obj_it->first, *delete_obj_id_it));
					}
				}
			}
		}
	}

	if (!delete_vec.empty())
	{
		for (int i = 0; i < (int)delete_vec.size(); i++)
		{
			std::map<int, std::set<ObjID> >::iterator chest_obj_it = m_chest_list.find(delete_vec[i].box_id);
			if (chest_obj_it != m_chest_list.end())
			{
				std::set<ObjID>::iterator it = chest_obj_it->second.find(delete_vec[i].obj_id);
				if (it != chest_obj_it->second.end())
				{
					chest_obj_it->second.erase(*it);
				}
			}
		}
	}

	// 再生成宝箱
	const MazeRacerOthersCfg & other_cfg = LOGIC_CONFIG->GetActivityMazeRacerConfig()->GetOthersConfig();
	for (int i = 0; i < other_cfg.box_refresh_num && m_box_num < other_cfg.box_max_num; i++)
	{
		const MazeRaceBoxCfg* cfg = LOGIC_CONFIG->GetActivityMazeRacerConfig()->GetRandMazeRaceBoxCfg();
		if (cfg == NULL) return;

		TreasureChest * chest_obj = new TreasureChest(cfg->box_id);
		chest_obj->SetPos(m_scene->GetARandPosi());
		m_scene->AddObj(chest_obj);
		m_chest_list[cfg->box_id].insert(chest_obj->GetId());
		m_box_num += 1;
	}
}

void SpecialLogicMazeRacerWaitingHall::ExtinctAllChest()
{
	for (std::map<int, std::set<ObjID> >::iterator ext_loop = m_chest_list.begin(); ext_loop != m_chest_list.end(); ++ext_loop)
	{
		for (std::set<ObjID>::iterator obj_id_it = ext_loop->second.begin(); obj_id_it != ext_loop->second.end(); ++obj_id_it)
		{
			Obj * target_obj = m_scene->GetObj(*obj_id_it);
			if (target_obj != NULL)
			{
				if (target_obj->GetObjType() == Obj::OBJ_TYPE_TREASURE_CHEST)
				{
					m_scene->DeleteObj(*obj_id_it);
				}
			}
		}
	}

	m_chest_list.clear();
	m_box_num = 0;
}

bool SpecialLogicMazeRacerWaitingHall::RandAndPutChestAwardInBag(Role * role, int chest_id, bool is_leader)
{
	std::vector<ItemConfigData> tmp_award_list;
	LOGIC_CONFIG->GetActivityMazeRacerConfig()->GetMazeRaceBoxReward(chest_id, tmp_award_list, is_leader);
	if (!tmp_award_list.empty())
	{
		if (!role->GetRoleModuleManager()->GetKnapsack()->PutList((short)tmp_award_list.size(), &tmp_award_list[0], PUT_REASON_MAZERACER))
		{
			role->NoticeNum(errornum::EN_HAS_NO_SPACING);
			return false;
		}
		gamelog::g_log_maze_racer.printf(LL_INFO, "open treasure chest: user[%d, %s] item[id:%d, num:%d] now_time:%lld reason[%s]", role->GetUID(), role->GetName(), tmp_award_list[0].item_id,
			tmp_award_list[0].num, (long long)EngineAdapter::Instance().Time(), __FUNCTION__);
	}
	return true;
}


void MazeRacerTeamMemberGoToFunction(int role_id, int target_scene_id, int target_scene_key, bool go_ret, void * param)
{
	if (!go_ret)
	{
		TeamManager::Instance().ExitTeam2(role_id);
		return;
	}

	MazeRacerManager::Instance().AddJoinTimes(role_id);
}