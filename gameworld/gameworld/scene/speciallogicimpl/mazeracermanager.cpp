#include "mazeracermanager.hpp"
#include "world.h"
#include "engineadapter.h"
#include "servercommon/string/gameworldstr.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/route/mailroute.hpp"
#include "protocol/msgactivity.hpp"
#include "global/guild/guild.hpp"
#include "global/guild/guildmanager.hpp"
#include "global/team/team.hpp"
#include "global/team/teammanager.hpp"
#include "scene/speciallogicimpl/mazeracermanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/activityconfig/mazeracer/activitymazeracerconfig.hpp"
#include "scene/speciallogicimpl/speciallogicmazeracer.hpp"
#include "battle/battle_manager_local.hpp"
#include "item/knapsack.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "internalcomm.h"
#include "gamelog.h"
#include "servercommon/errornum.h"

MazeRacerManager::MazeRacerManager() : m_give_rank_award_timestamp(0)
{
	m_scenc_key = MAZE_RACER_COPY_ID_BEGIN;
}

MazeRacerManager::~MazeRacerManager()
{
}

MazeRacerManager & MazeRacerManager::Instance()
{
	static MazeRacerManager mrm;
	return mrm;
}

void MazeRacerManager::Update(time_t now_second)
{
	for (std::set<SceneIndex>::iterator iter = m_need_del_scene_index.begin(); iter != m_need_del_scene_index.end(); )
	{
		if (!this->IsMazeRacerScene(*iter) || World::GetInstWorld()->GetSceneManager()->DestroyFB(*iter))
		{
			m_need_del_scene_index.erase(iter++);
		}
		else
		{
			++iter;
		}
	}


	UNSTD_STATIC_CHECK(4 == MAZE_AMULET_EFFECT_TYPE_MAX);
	for (std::map<int, TeamParamCommonData>::iterator loop = m_team_common_data_map.begin(); loop != m_team_common_data_map.end();)
	{
		int team_index = loop->first;
		TeamParamCommonData & info_list = loop->second;

		bool is_need_send = false;
		for (std::map<int, TeamAmuletInfo > ::iterator iter = info_list.team_mmulet_info.begin(); iter != info_list.team_mmulet_info.end();)
		{
			if (iter->first == MAZE_AMULET_EFFECT_SET_MOVES_PERMIT || iter->first == MAZE_AMULET_EFFECT_SET_ENCNT_PERMIT)
			{
				TeamAmuletInfo & info = iter->second;
				if (now_second > info.effect_time)		// 如果持续时间过了
				{
					is_need_send = true;
					info_list.team_mmulet_info.erase(iter++);
					continue;
				}
			}

			++iter;
		}

		if (is_need_send)
		{
			this->AmuletEffectMessage(team_index);
		}

		if (info_list.team_mmulet_info.empty())
		{
			m_team_common_data_map.erase(loop++);
		}
		else
		{
			++loop;
		}
	}
}

void MazeRacerManager::InitWaitingHall()
{
	std::set<int> tmp_waiting_hall_id_set;
	LOGIC_CONFIG->GetActivityMazeRacerConfig()->GetAllWaitingHallSceneID(tmp_waiting_hall_id_set);
	for (std::set<int>::iterator loop = tmp_waiting_hall_id_set.begin(); loop != tmp_waiting_hall_id_set.end(); ++loop)
	{
		SpecialLogicMazeRacerWaitingHall * spec = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicMazeRacerWaitingHall(*loop, COMMON_FB_KEY);
		if (spec != NULL)
		{
			spec->Reset();
			spec->OnActivityOpen();
		}
	}
}

void MazeRacerManager::KickWaitingHall()
{
	std::set<int> tmp_waiting_hall_id_set;
	LOGIC_CONFIG->GetActivityMazeRacerConfig()->GetAllWaitingHallSceneID(tmp_waiting_hall_id_set);
	for (std::set<int>::iterator loop = tmp_waiting_hall_id_set.begin(); loop != tmp_waiting_hall_id_set.end(); ++loop)
	{
		SpecialLogicMazeRacerWaitingHall * sp_logic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicMazeRacerWaitingHall(*loop, COMMON_FB_KEY);
		if (NULL != sp_logic) 
		{
			sp_logic->DelayKickOutAllRole();

		}
	}
}

bool MazeRacerManager::InitMazeCopy(int maze_id, int * copy_id, int * scene_id)
{
	time_t now = EngineAdapter::Instance().Time();
	*copy_id = m_scenc_key++;
	// 相同的 Key 所产生的场景ID 不能重复，否则会出问题
	std::set<int> scene_except_list;
	int max_tier = LOGIC_CONFIG->GetActivityMazeRacerConfig()->GetMaxTierByMazeID(maze_id);
	for (int layer = max_tier; layer > 0; --layer)
	{
		*scene_id = LOGIC_CONFIG->GetActivityMazeRacerConfig()->RandASceneIDByMazeAndTier(maze_id, layer, scene_except_list);
		if (0 == *scene_id) return false;

		SpecialLogicMazeRacer * copy_room = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicMazeRacer(*scene_id, *copy_id);
		if (NULL == copy_room) return false;
		
		Scene * scene = copy_room->GetScene();
		if (NULL == scene) return false;

		m_maze_list[*copy_id][layer] = scene->GetIndex();
		copy_room->InitMazeRacerScene(*copy_id, maze_id, layer,now);
		scene_except_list.insert(*scene_id);
	}

	m_begin_timestamp[*copy_id] = (UInt32)now;
	return true;
}

int MazeRacerManager::GetNextTiersScene(int copy_id, int layer)
{
	SpecialLogicMazeRacer * sp_logic = this->GetSPMazeRacer(copy_id, layer + 1);
	if (NULL != sp_logic && NULL != sp_logic->GetScene()) return sp_logic->GetScene()->GetSceneID();

	return 0;
}

void MazeRacerManager::OnEncounterMonster(int team_index)
{
	++m_encounter_times[team_index];
	
	// 刷新符咒效果
	std::map<int, TeamParamCommonData>::iterator f_it = m_team_common_data_map.find(team_index);
	if (m_team_common_data_map.end() != f_it)
	{
		bool is_need_send = false;
		TeamParamCommonData & info=f_it->second;
		{
			TeamAmuletInfo * amulet_info = info.GetAmuletInfo(MAZE_AMULET_EFFECT_SET_BATTLE_SPEED);
			if (amulet_info != NULL)
			{
				amulet_info->effect_time -= 1;

				if (amulet_info->effect_time <= 0)
				{
					is_need_send = true;
					info.team_mmulet_info.erase(MAZE_AMULET_EFFECT_SET_BATTLE_SPEED);
				}
			}
		}
		{
			TeamAmuletInfo * amulet_info = info.GetAmuletInfo(MAZE_AMULET_EFFECT_SET_WAR_INTERVAL);
			if (amulet_info != NULL)
			{
				amulet_info->effect_time -= 1;

				if (amulet_info->effect_time <= 0)
				{
					is_need_send = true;
					info.team_mmulet_info.erase(MAZE_AMULET_EFFECT_SET_WAR_INTERVAL);
				}
			}
		}

		if (is_need_send)
		{
			this->AmuletEffectMessage(team_index);
		}		
	}
}

void MazeRacerManager::OnTeamEnter(int copy_id, int layer, int team_index)
{
	m_location_map[team_index] = TeamLocation(copy_id, layer);
}

void MazeRacerManager::AmuletEffectMessage(int team_index)
{
	Team * team = TeamManager::Instance().GetTeamByTeamIndex(team_index);
	if (NULL == team) return;

	Role * leader = team->GetLeaderRole();
	if (NULL == leader) return;

	if (leader->GetScene()->GetSpecialLogic()->GetSceneType() != Scene::SCENE_TYPE_MAZE_RACER) return;

	Protocol::SCActivityMazeRacerCurseInfo amrci;
	amrci.obj_id = leader->GetId();		// 只发队长
	amrci.curse_type[0] = -1;

	TeamParamCommonData * suffer_amulet = this->GetTeamParamCommonData(team->GetTeamIndex());
	if (suffer_amulet != NULL)
	{
		amrci.curse_type[0] = suffer_amulet->GetFirstAmuletId();//目前修改为只能有一个效果
	}
	
	std::map<UInt32, TeamLocation>::iterator locate_it = m_location_map.find(team_index);
	if (m_location_map.end() == locate_it) return;

	SpecialLogicMazeRacer * sp_logic = MazeRacerManager::Instance().GetSPMazeRacer(locate_it->second.copy_id, locate_it->second.layer);
	if (NULL == sp_logic || sp_logic->GetScene()->GetIndex() != leader->GetScene()->GetIndex()) return;

	sp_logic->GetScene()->GetZoneMatrix()->NotifyAreaMsg(leader->GetObserHandle(), (void *)&amrci, sizeof(amrci));
}

void MazeRacerManager::AddJoinTimes(int role_id)
{	
	MazeRacerRoleJoinInfo & role_info = this->GetRoleJoinInfo(role_id);
	role_info.join_times += 1;
}

bool MazeRacerManager::CanJoin(int role_id)
{
	return true;  //改成不限制次数了
}

MazeRacerRoleJoinInfo & MazeRacerManager::GetRoleJoinInfo(int role_id)
{
	MazeRacerRoleJoinInfo & ret = m_role_to_join_info[role_id];
	ret.role_id = role_id;
	return ret;
}

void MazeRacerManager::GmSetActivityRankTimestamp()
{
	m_give_rank_award_timestamp = EngineAdapter::Instance().Time();
}

void MazeRacerManager::OnTeamPass(int copy_id, int maze_id, Team * team)
{
	if (NULL == team) return;
	
	std::map<UInt32, UInt32>::iterator begin_it = m_begin_timestamp.find(copy_id);
	if (m_begin_timestamp.end() == begin_it) return;
	
	UInt32 now_sec = (UInt32)EngineAdapter::Instance().Time();
	UInt32 pass_time = now_sec - begin_it->second;
	if (pass_time <= 0) return;
	
	int pass_id = MAZE_RACER_PASS_ID_BEGIN;
	if (!m_pass_id_list.empty())
	{
		UInt32 last_id = *m_pass_id_list.rbegin();
		if (last_id < UINT_MAX)
		{
			pass_id = last_id + 1;
		}
		else
		{
			return;		// 队伍通关上限爆了，返回
		}
	}

	m_pass_id_list.insert(pass_id);

	MazeRacerTeamInfo tmp_mrti(pass_id, 0, team->GetTeamIndex());
	int count = team->GetMemberCount();
	for (int mem_idx = 0; mem_idx < count; ++mem_idx)
	{
		if (tmp_mrti.mem_count >= MAX_TEAM_MEMBER_NUM) break;

		Role * mem_role = team->GetMemberRoleByIndex(mem_idx);
		if (NULL == mem_role) continue;		// 忽略离线玩家

		ActivityRoleInform & info_con = tmp_mrti.pass_member[tmp_mrti.mem_count];
		info_con.role_uid = mem_role->GetUID();
		info_con.level = mem_role->GetLevel();
		info_con.prof = mem_role->GetBaseProfession();
		info_con.avatar = mem_role->GetAvatarType();
		info_con.headshot_id = mem_role->GetHeadshotID();
		mem_role->GetName(info_con.role_name);

		Guild * guild = mem_role->GetGuild();
		if (NULL != guild) memcpy(info_con.guild_name, guild->GetName(), sizeof(GuildName));

		// 队长放在第一位
		if (mem_role->GetUID() == team->GetLeaderUID() && 0 != tmp_mrti.mem_count) std::swap(tmp_mrti.pass_member[0], info_con);
		++tmp_mrti.mem_count;
	}
	
	// 副本排名
	std::map<UInt32, std::map<UInt32, std::vector<MazeRacerTeamInfo> > >::iterator rank_copy_it = m_ectype_rank_list.find(copy_id);
	if (m_ectype_rank_list.end() != rank_copy_it)
	{
		std::map<UInt32, std::vector<MazeRacerTeamInfo> >::iterator copy_time_it = rank_copy_it->second.find(pass_time);
		if (rank_copy_it->second.end() != copy_time_it)
		{
			copy_time_it->second.push_back(tmp_mrti);
		}
		else
		{
			std::vector<MazeRacerTeamInfo> tmp_pass_team_list; tmp_pass_team_list.push_back(tmp_mrti);
			m_ectype_rank_list[copy_id][pass_time] = tmp_pass_team_list;
		}
	}
	else
	{
		std::vector<MazeRacerTeamInfo> tmp_pass_team_list; tmp_pass_team_list.push_back(tmp_mrti);
		m_ectype_rank_list[copy_id][pass_time] = tmp_pass_team_list;
	}

	this->SendEctypeRankInfo(copy_id);		// 下发副本排名

	// 发放副本排名奖励
	std::map<UInt32, std::map<UInt32, std::vector<MazeRacerTeamInfo> > >::iterator copy_it = m_ectype_rank_list.find(copy_id);
	if (m_ectype_rank_list.end() == copy_it) return;

	int rank_posi = (int)copy_it->second.size();

	Protocol::SCActivityMazeRacerPassdInfo amrpi;	// 通关信息协议
	amrpi.pass_time = pass_time;
	amrpi.rank_posi = rank_posi;
	amrpi.total_encounter = m_encounter_times[team->GetTeamIndex()];
	amrpi.award_num = 0;
	
	int mem_count = team->GetMemberCount();
	for (int mem_idx = 0; mem_idx < mem_count; ++mem_idx)		// 每个成员单独随机
	{
		Role * mem_role = team->GetMemberRoleByIndex(mem_idx);
		if (NULL == mem_role) continue;

		MazeRacerRoleJoinInfo & role_info = this->GetRoleJoinInfo(mem_role->GetUID());

		if (role_info.best_pass_time == 0)
		{
			role_info.best_pass_time = pass_time;
		}

		if (pass_time < role_info.best_pass_time)
		{
			role_info.best_pass_time = pass_time;
			mem_role->NoticeNum(errornum::EN_MAZE_RACER_NEW_PASS_TIME);
		}

		std::vector<ItemConfigData> tmp_award_list;
		if (role_info.get_fb_pass_reward_times != 0)
		{
			mem_role->NoticeNum(errornum::EN_MAZE_RACER_ALREADY_GET_REWARD);
		}
		else
		{
			role_info.get_fb_pass_reward_times += 1;
			LOGIC_CONFIG->GetActivityMazeRacerConfig()->RandCopyRankAward(maze_id, rank_posi, mem_role->GetLevel(), tmp_award_list);
			if (!tmp_award_list.empty())
			{
				mem_role->GetRoleModuleManager()->GetKnapsack()->PutList((short)tmp_award_list.size(), &tmp_award_list[0], PUT_REASON_MAZERACER);
			}
		}
			
		amrpi.award_num = 0;
		for (std::vector<ItemConfigData>::iterator loop = tmp_award_list.begin(); loop != tmp_award_list.end(); ++loop)
		{
			if (amrpi.award_num >= MAX_ATTACHMENT_ITEM_NUM || amrpi.award_num >= ACTIVITY_AWARD_NUM_MAX) break;
			amrpi.award_list[amrpi.award_num++] = Protocol::PtcItemInfo(loop->item_id, loop->num);
		}

		int ptc_len = sizeof(amrpi) - (ACTIVITY_AWARD_NUM_MAX - amrpi.award_num) * sizeof(amrpi.award_list[0]);
		if (ptc_len > 0) mem_role->GetRoleModuleManager()->NetSend((const void *)&amrpi, ptc_len);
	}
	
	// 插入活动排名
	std::map<int, std::map<UInt32, std::vector<MazeRacerTeamInfo> > >::iterator rank_maze_it = m_pass_rank_list.find(maze_id);
	if (m_pass_rank_list.end() != rank_maze_it)
	{
		std::map<UInt32, std::vector<MazeRacerTeamInfo> >::iterator rank_time_it = rank_maze_it->second.find(pass_time);
		if (rank_maze_it->second.end() != rank_time_it)
		{
			rank_time_it->second.push_back(tmp_mrti);
		}
		else
		{
			std::vector<MazeRacerTeamInfo> tmp_pass_team_list; tmp_pass_team_list.push_back(tmp_mrti);
			m_pass_rank_list[maze_id][pass_time] = tmp_pass_team_list;
		}
	}
	else
	{
		std::vector<MazeRacerTeamInfo> tmp_pass_team_list; tmp_pass_team_list.push_back(tmp_mrti);
		m_pass_rank_list[maze_id][pass_time] = tmp_pass_team_list;
	}

	// 搜索列表位置
	std::map<int, std::map<UInt32, std::vector<MazeRacerTeamInfo> > >::iterator after_rank_maze_it = m_pass_rank_list.find(maze_id);
	if (m_pass_rank_list.end() == after_rank_maze_it) return;		// 上面插入了排名，这里不会为空

	Protocol::SCActivityMazeRacerActInsert amrai;
	amrai.maze_id = maze_id;
	amrai.reserve_sh = 0;
	amrai.rank_info.pass_time = pass_time;
	amrai.rank_info.mem_count = tmp_mrti.mem_count;
	for (int mem_idx = 0; mem_idx < tmp_mrti.mem_count; ++mem_idx)
	{
		if (mem_idx >= MAX_TEAM_MEMBER_NUM) break;
		amrai.rank_info.mem_list[mem_idx] = tmp_mrti.pass_member[mem_idx];
	}

	amrai.rank_info.rank_posi = 0; bool find_flag = false;
	for (std::map<UInt32, std::vector<MazeRacerTeamInfo> >::iterator timer_it = after_rank_maze_it->second.begin(); timer_it != after_rank_maze_it->second.end(); ++timer_it)
	{
		for (std::vector<MazeRacerTeamInfo>::iterator f_loop = timer_it->second.begin(); f_loop != timer_it->second.end(); ++f_loop)
		{
			++amrai.rank_info.rank_posi;
			if (f_loop->pass_id == tmp_mrti.pass_id)
			{
				find_flag = true;
				break;
			}
		}

		if (find_flag) break;
	}

	if (find_flag)
	{
		World::GetInstWorld()->SendToAllGateway((const char *)&amrai, sizeof(amrai));
		InternalComm::Instance().SendToAllGameUserThroughCross((const char *)&amrai, sizeof(amrai));
	}
	this->RefreshTeamStatus(team->GetTeamIndex());
}

void MazeRacerManager::RefreshTeamStatus(int team_index)
{
	m_encounter_times.erase(team_index);
	m_location_map.erase(team_index);
	m_team_amulet_map.erase(team_index);
	m_team_common_data_map.erase(team_index);
}

void MazeRacerManager::OnSendRoleInfo(Role * role)
{
	if (role==NULL)
	{
		return;
	}

	MazeRacerRoleJoinInfo & role_info = this->GetRoleJoinInfo(role->GetUID());

	 Protocol::SCActivityMazeRacerRoleInfo protocol;
	 protocol.get_pass_times = role_info.get_fb_pass_reward_times;
	EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
}

void MazeRacerManager::SendAllRankInfo(Role * role)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		for (std::map<int, std::map<UInt32, std::vector<MazeRacerTeamInfo> > >::iterator loop = m_pass_rank_list.begin(); loop != m_pass_rank_list.end(); ++loop)
		{
			this->SendMazeRacerActivityRank(loop->first, role == NULL ? 0 : role->GetUID());
		}
	}
	else
	{
		if (role == NULL)
		{
			return;
		}

		UniqueUserID uuid = role->GetUniqueUserID();
		crossgameprotocal::GameHiddenMazerAcerReq send_hidden_req;
		send_hidden_req.origin_plat_type = uuid.plat_type;
		send_hidden_req.origin_server_id = LocalConfig::Instance().GetMergeServerId();
		send_hidden_req.origin_role_id = UidToInt(uuid.user_id);
		send_hidden_req.req_type = crossgameprotocal::GAME_HIDDEN_MAZE_RACER_REQ_TYPE_RANK;
		InternalComm::Instance().SendToHiddenThroughCross((const char *)&send_hidden_req, sizeof(send_hidden_req));

	}
}

void MazeRacerManager::SendAllRankInfoHiddenToGame(int role_id)
{
	for (std::map<int, std::map<UInt32, std::vector<MazeRacerTeamInfo> > >::iterator loop = m_pass_rank_list.begin(); loop != m_pass_rank_list.end(); ++loop)
	{
		this->SendMazeRacerActivityRank(loop->first, role_id);
	}
}

void MazeRacerManager::CheckIssueActivityAward(time_t now_second, int next_open_day)
{
	if (now_second <= m_give_rank_award_timestamp || m_give_rank_award_timestamp == 0) return;  //开启活动才会计算这个发奖励时间,发完赋值为0

	this->ForceStopMazeRacerBattle();
	this->KickWaitingHall();

	this->SendAllRankInfo();
	this->MazeRacerIssueActivityAward();

	this->Clear();

	m_give_rank_award_timestamp = 0;
}

void MazeRacerManager::OnActivityStandy()
{
	this->ForceStopMazeRacerBattle();
	this->KickWaitingHall();
	this->Clear();
	this->SendAllRankInfo();
}

void MazeRacerManager::OnActivityOpen()
{
	this->InitWaitingHall();

	{	// 计算下次发放奖励的时间戳
		const MazeRacerOthersCfg mr_other_cfg = LOGIC_CONFIG->GetActivityMazeRacerConfig()->GetOthersConfig();
		int tmp_interval = EngineAdapter::Instance().NextDayInterval(mr_other_cfg.act_reward_time / 100, mr_other_cfg.act_reward_time % 100, 0);
		m_give_rank_award_timestamp = EngineAdapter::Instance().Time() + tmp_interval;
	}
}

void MazeRacerManager::OnActivityEnd()
{
	std::set<int> tmp_waiting_hall_id_set;
	LOGIC_CONFIG->GetActivityMazeRacerConfig()->GetAllWaitingHallSceneID(tmp_waiting_hall_id_set);
	for (std::set<int>::iterator loop = tmp_waiting_hall_id_set.begin(); loop != tmp_waiting_hall_id_set.end(); ++loop)
	{
		SpecialLogicMazeRacerWaitingHall * sp_logic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicMazeRacerWaitingHall(*loop, COMMON_FB_KEY);
		if (NULL != sp_logic)
		{
			sp_logic->OnActivityEnd();
		}
	}
	this->SendAllRankInfo();
}

void MazeRacerManager::Clear()
{
	ReleaseSpLogicMap();
	m_role_to_join_info.clear();
	m_begin_timestamp.clear();
	m_encounter_times.clear();
	m_location_map.clear();
	m_maze_list.clear();
	m_pass_rank_list.clear();
	m_ectype_rank_list.clear();
	m_pass_id_list.clear();
	m_team_amulet_map.clear();
	m_team_common_data_map.clear();
}

SpecialLogicMazeRacer * MazeRacerManager::GetSPMazeRacer(int copy_id, int layer)
{
	std::map<UInt32, std::map<int, SceneIndex> >::iterator f_it = m_maze_list.find(copy_id);
	if (m_maze_list.end() == f_it) return NULL;

	std::map<int, SceneIndex>::iterator layer_it = f_it->second.find(layer);
	if (f_it->second.end() == layer_it) return NULL;

	Scene * scene = World::GetInstWorld()->GetSceneManager()->GetSceneByIndex(layer_it->second);
	if (NULL == scene) return NULL;

	SpecialLogic * sp_logic = scene->GetSpecialLogic();
	if (NULL != sp_logic && Scene::SCENE_TYPE_MAZE_RACER == sp_logic->GetSceneType())
	{
		return (SpecialLogicMazeRacer *)sp_logic;
	}

	return NULL;
}

bool MazeRacerManager::IsMazeRacerScene(SceneIndex scene_index)
{
	Scene * scene = World::GetInstWorld()->GetSceneManager()->GetSceneByIndex(scene_index);
	if (NULL != scene && Scene::SCENE_TYPE_MAZE_RACER == scene->GetSceneType())
	{
		return true;
	}

	return false;
}

int & MazeRacerManager::GetTeamResiAmulet(int team_index, int amulet_id)
{
	return m_team_amulet_map[team_index][amulet_id];
}

void MazeRacerManager::AddAmuletToTeam(int team_index, int amulet_id, int num)
{
	int & resi_num = m_team_amulet_map[team_index][amulet_id];
	resi_num += num;
	if (resi_num <= 0) resi_num = 0;
	this->SendAmuletNumToTeam(team_index);
}

void MazeRacerManager::AddAmuletBuffToTeam(int user_team_index, int target_team_index, int amulet_id)
{
	const MazeRacerAumletCfg * amulet_cfg = LOGIC_CONFIG->GetActivityMazeRacerConfig()->GetAmuletConfig(amulet_id);
	if (NULL == amulet_cfg) return;

	bool is_war = true;		//添加遇怪咒符之前是否处于遇怪咒符效果中，不是的话才要刷新遇暗怪间隔
	switch (amulet_cfg->effect_type)
	{
	case MAZE_AMULET_EFFECT_SET_BATTLE_SPEED:		// 生效次数类： effect_times - 剩余生效次数
		{
			TeamAmuletInfo & amulet_info = m_team_common_data_map[target_team_index].team_mmulet_info[amulet_cfg->effect_type];
			amulet_info.effect_type = amulet_cfg->effect_type;

			if (amulet_info.amulet_id == -1)
			{
				amulet_info.amulet_id = amulet_id;
				amulet_info.effect_time = amulet_cfg->duration;
				amulet_info.effect_value = amulet_cfg->effect_val;
				break;
			}

			if (amulet_info.effect_value > 0 && amulet_cfg->effect_type > 0)
			{
				amulet_info.effect_time += amulet_cfg->duration;
			}
			else if (amulet_info.effect_value < 0 && amulet_cfg->effect_type < 0)
			{
				amulet_info.effect_time += amulet_cfg->duration;
			}
			else
			{				
				if (amulet_info.effect_time < amulet_cfg->duration)
				{
					amulet_info.amulet_id = amulet_id;
					amulet_info.effect_value = amulet_cfg->effect_val;
					amulet_info.effect_time = amulet_cfg->duration - amulet_info.effect_time;
				}
				else
				{
					amulet_info.effect_time -= amulet_cfg->duration;
				}
			}
		}
		break;
	case MAZE_AMULET_EFFECT_SET_WAR_INTERVAL:
		{
			TeamAmuletInfo & amulet_info = m_team_common_data_map[target_team_index].team_mmulet_info[amulet_cfg->effect_type];

			if (amulet_info.effect_time == 0)
			{
				is_war = false;
			}

			amulet_info.amulet_id = amulet_id;
			amulet_info.effect_type = amulet_cfg->effect_type;
			amulet_info.effect_time += amulet_cfg->duration;
			amulet_info.effect_value = amulet_cfg->effect_val;
		}
		break;

	case MAZE_AMULET_EFFECT_SET_MOVES_PERMIT:		// 持续时间类： effect_times - 失效时间戳
	case MAZE_AMULET_EFFECT_SET_ENCNT_PERMIT:
		{
			TeamAmuletInfo & amulet_info = m_team_common_data_map[target_team_index].team_mmulet_info[amulet_cfg->effect_type];
			amulet_info.amulet_id = amulet_id;
			amulet_info.effect_type = amulet_cfg->effect_type;
			amulet_info.effect_value = amulet_cfg->effect_val;

			time_t now = EngineAdapter::Instance().Time();
			if (now > amulet_info.effect_time)
			{
				amulet_info.effect_time = now + amulet_cfg->duration;
			}
			else
			{
				amulet_info.effect_time += amulet_cfg->duration;
			}
		}
		break;
	}
	UNSTD_STATIC_CHECK(4 == MAZE_AMULET_EFFECT_TYPE_MAX);


	if (!is_war && MAZE_AMULET_EFFECT_SET_WAR_INTERVAL == amulet_cfg->effect_type)		// 如果是激战符，立刻刷新一次遇怪间隔
	{
		Team * team = TeamManager::Instance().GetTeamByTeamIndex(target_team_index);
		if (NULL != team)
		{
			Role * leader = team->GetLeaderRole();
			if (NULL != leader)
			{
				leader->RefreshEncounterInterval();
			}
		}
	}

	this->SendAmuletNumToTeam(user_team_index);
	this->AmuletEffectMessage(target_team_index);
}

TeamParamCommonData * MazeRacerManager::GetTeamParamCommonData(int team_index)
{
	std::map<int, TeamParamCommonData>::iterator f_it = m_team_common_data_map.find(team_index);
	if (m_team_common_data_map.end() != f_it)
	{
		return &f_it->second;
	}

	return NULL;
}

TeamAmuletInfo * MazeRacerManager::GetTeamSufferAmulet(int team_index, int effect_type)
{
	TeamParamCommonData *  info=this->GetTeamParamCommonData(team_index);
	if (info != NULL)
	{
		return info->GetAmuletInfo(effect_type);
	}

	return NULL;
}

bool MazeRacerManager::TeamHasSufferAmuletBuff(int team_index)
{
	TeamParamCommonData * info = this->GetTeamParamCommonData(team_index);
	if (info != NULL && !info->team_mmulet_info.empty())
	{
		return true;
	}

	return false;
}

void MazeRacerManager::SendAmuletNumToTeam(int team_index)
{
	Team * team = TeamManager::Instance().GetTeamByTeamIndex(team_index);
	if (NULL == team) return;

	Protocol::SCActivityMazeRacerAmuletNum amran;
	amran.type_count = 0;

	std::map<int, std::map<int, int> >::iterator f_it = m_team_amulet_map.find(team_index);
	if (m_team_amulet_map.end() != f_it)
	{
		for (std::map<int, int>::iterator loop = f_it->second.begin(); loop != f_it->second.end(); ++loop)
		{
			if (amran.type_count >= ACTIVITY_MAZE_RACER_AMULET_MAX_TYPE) break;
			if (loop->second > 0) amran.amulet_num[amran.type_count++] = Protocol::PtcItemInfo(loop->first, loop->second);
		}
	}

	team->SendToMember((const char *)&amran, sizeof(amran));
}

void MazeRacerManager::SendMazeRacerActivityRank(int maze_id, int role_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}


	Protocol::SCActivityMazeRacerRankLists mrrl;

	mrrl.maze_id = maze_id;
	mrrl.list_num = 0;

	std::map<int, std::map<UInt32, std::vector<MazeRacerTeamInfo> > >::iterator maze_it = m_pass_rank_list.find(maze_id);
	if (m_pass_rank_list.end() != maze_it)
	{
		for (std::map<UInt32, std::vector<MazeRacerTeamInfo> >::iterator timer_it = maze_it->second.begin(); timer_it != maze_it->second.end(); ++timer_it)
		{
			if (mrrl.list_num >= ACTIVITY_RANK_SHORTER_NUM_MAX) break;
			
			for (std::vector<MazeRacerTeamInfo>::iterator team_it = timer_it->second.begin(); team_it != timer_it->second.end(); ++team_it)
			{
				if (mrrl.list_num >= ACTIVITY_RANK_SHORTER_NUM_MAX) break;

				Protocol::MazeRacerRankItem & list_item = mrrl.rank_list[mrrl.list_num++];
				list_item.rank_posi = mrrl.list_num;			// 排名逻辑修改，顺位向下，没有并列
				list_item.pass_time = timer_it->first;
				list_item.mem_count = team_it->mem_count;
				for (int mem_idx = 0; mem_idx < list_item.mem_count && mem_idx < MAX_TEAM_MEMBER_NUM; ++mem_idx)
				{
					list_item.mem_list[mem_idx] = team_it->pass_member[mem_idx];
				}
			}
		}
	}
	
	if (role_id != 0)
	{
		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
		if (role != NULL)
		{
			EngineAdapter::Instance().NetSendSerializeMsg(role->GetNetId(), &mrrl);
		}
		else
		{
			InternalComm::Instance().SendSerializeMsgToGameUserThroughCross(role_id, &mrrl);
		}		
	}
	else
	{
		World::GetInstWorld()->SendSerializeToAllGateway(&mrrl);
		InternalComm::Instance().SendSerializeMsgToAllGameUserThroughCross(&mrrl);
	}
}

void MazeRacerManager::MazeRacerIssueActivityAward()
{
	std::set<int> get_reward_set;
	for (std::map<int, std::map<UInt32, std::vector<MazeRacerTeamInfo> > >::iterator maze_it = m_pass_rank_list.begin(); maze_it != m_pass_rank_list.end(); ++maze_it)
	{
		int team_num = 0;		// 已发放奖励的队伍数量
		for (std::map<UInt32, std::vector<MazeRacerTeamInfo> >::iterator timer_it = maze_it->second.begin(); timer_it != maze_it->second.end(); ++timer_it)
		{
			//int rank_posi = team_num + 1;		// 最高位排名，由已发放奖励的队伍数量向后顺延。时间相同，排名并列。
			for (std::vector<MazeRacerTeamInfo>::iterator team_it = timer_it->second.begin(); team_it != timer_it->second.end(); ++team_it)
			{
				int rank_posi = team_num + 1;	// 修改排名逻辑：排名没有并列，时间相同的，排名也分先后
				for (int mem_idx = 0; mem_idx < team_it->mem_count && mem_idx < MAX_TEAM_MEMBER_NUM; ++mem_idx)		// 每个成员单独随机
				{
					if (get_reward_set.find(team_it->pass_member[mem_idx].role_uid) != get_reward_set.end())
					{
						continue;
					}
					get_reward_set.insert(team_it->pass_member[mem_idx].role_uid);

					std::vector<ItemConfigData> tmp_award_list;
					if (!LOGIC_CONFIG->GetActivityMazeRacerConfig()->RandActivityAward(maze_it->first, rank_posi, team_it->pass_member[mem_idx].level, tmp_award_list)) break;

					static MailContentParam contentparam; contentparam.Reset(); int award_num = 0;
					for (std::vector<ItemConfigData>::iterator loop = tmp_award_list.begin(); loop != tmp_award_list.end(); ++loop)
					{
						if (award_num >= MAX_ATTACHMENT_ITEM_NUM) break;
						contentparam.item_list[award_num++] = ItemDataWrapper(loop->item_id, loop->num, loop->is_bind ? 1 : 0);
					}

					int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_maze_racer_reward_subj);
					int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_maze_racer_rank_reward, rank_posi);
					if (lensub > 0 && length > 0)
					{
						MailRoute::MailToUser(IntToUid(team_it->pass_member[mem_idx].role_uid), MAIL_REASON_DEFAULT, contentparam);
						gamelog::g_log_maze_racer.printf(LL_INFO, "%s Send Mail Succ user[%d,%s] Rank[%d] Level[%d] maze_id[%d] ", __FUNCTION__, team_it->pass_member[mem_idx].role_uid, team_it->pass_member[mem_idx].role_name,
							rank_posi, team_it->pass_member[mem_idx].level, maze_it->first);
						// TODO: 打 log，后续补充
					}

				}

				++team_num;
			}
		}
	}
}

void MazeRacerManager::SendEctypeRankInfo(int copy_id)
{
	std::map<UInt32, UInt32>::iterator time_it = m_begin_timestamp.find(copy_id);
	if (m_begin_timestamp.end() == time_it) return;
	
	std::map<UInt32, std::map<int, SceneIndex> >::iterator unfin_copy_it = m_maze_list.find(copy_id);
	if (m_maze_list.end() == unfin_copy_it) return;

	Protocol::SCActivityMazeRacerCopysInfo amrci;
	amrci.begin_time = time_it->second;
	amrci.reserve_ush = 0;
	amrci.list_num = 0;
	
	std::set<int> pass_team_leader_set;
	std::map<UInt32, std::map<UInt32, std::vector<MazeRacerTeamInfo> > >::iterator copy_it = m_ectype_rank_list.find(copy_id);
	if (m_ectype_rank_list.end() != copy_it)
	{
		for (std::map<UInt32, std::vector<MazeRacerTeamInfo> >::iterator time_it = copy_it->second.begin(); time_it != copy_it->second.end(); ++time_it)
		{
			if (amrci.list_num >= ACTIVITY_RANK_SHORTER_NUM_MAX) break;
			for (std::vector<MazeRacerTeamInfo>::iterator fin_team_it = time_it->second.begin(); fin_team_it != time_it->second.end(); ++fin_team_it)
			{
				if (amrci.list_num >= ACTIVITY_RANK_SHORTER_NUM_MAX) break;
				Protocol::SCActivityMazeRacerCopysInfo::MazeRacerCopyRankItem & fin_rank_item = amrci.rank_list[amrci.list_num++];
				fin_rank_item.cur_layer = fin_team_it->team_index;
				fin_rank_item.cur_layer = 0;
				fin_rank_item.leader_uid = fin_team_it->pass_member[0].role_uid;
				memcpy(fin_rank_item.leader_name, fin_team_it->pass_member[0].role_name, sizeof(GameName));
				pass_team_leader_set.insert(fin_rank_item.leader_uid);
			}
		}
	}
	
	for (std::map<int, SceneIndex>::reverse_iterator layer_it = unfin_copy_it->second.rbegin(); layer_it != unfin_copy_it->second.rend(); ++layer_it)
	{
		if (amrci.list_num >= ACTIVITY_RANK_SHORTER_NUM_MAX) break;

		SpecialLogicMazeRacer * sp_logic = this->GetSPMazeRacer(copy_id, layer_it->first);
		if (NULL == sp_logic) continue;

		std::vector<int> tmp_team_index; sp_logic->GetTeamIndexList(tmp_team_index);
		for (std::vector<int>::iterator team_index_it = tmp_team_index.begin(); team_index_it != tmp_team_index.end(); ++team_index_it)
		{
			if (amrci.list_num >= ACTIVITY_RANK_SHORTER_NUM_MAX) break;

			Team * team = TeamManager::Instance().GetTeamByTeamIndex(*team_index_it);
			if (NULL == team || pass_team_leader_set.end() != pass_team_leader_set.find(team->GetLeaderUID())) continue;

			Role * leader = team->GetLeaderRole();
			if (NULL == leader) continue;

			Protocol::SCActivityMazeRacerCopysInfo::MazeRacerCopyRankItem & layer_rank_item = amrci.rank_list[amrci.list_num++];
			layer_rank_item.team_index = team->GetTeamIndex();
			layer_rank_item.cur_layer = layer_it->first;
			layer_rank_item.leader_uid = leader->GetUID();
			leader->GetName(layer_rank_item.leader_name);
		}
	}
	
	int send_len = sizeof(amrci) - (ACTIVITY_RANK_SHORTER_NUM_MAX - amrci.list_num) * sizeof(amrci.rank_list[0]);
	if (send_len <= 0) return;
	
	for (std::map<int, SceneIndex>::iterator scene_it = unfin_copy_it->second.begin(); scene_it != unfin_copy_it->second.end(); ++scene_it)
	{
		SpecialLogicMazeRacer * sp_logic = this->GetSPMazeRacer(copy_id, scene_it->first);
		if (NULL != sp_logic) sp_logic->SendEctypeInfoToAllRole(&amrci, send_len);
	}
}

void MazeRacerManager::ForceStopMazeRacerBattle()
{
	BattleManagerLocal::GetInstance().ForceFinishBattleMode(BATTLE_MODE_MAZE_RACER);
}

void MazeRacerManager::ReleaseSpLogicMap()
{
	for (std::map<UInt32, std::map<int, SceneIndex> >::iterator loop = m_maze_list.begin(); loop != m_maze_list.end(); ++loop)
	{
		for (std::map<int, SceneIndex>::iterator maze_it = loop->second.begin(); maze_it != loop->second.end(); ++maze_it)
		{
			SpecialLogicMazeRacer * sp_logic = this->GetSPMazeRacer(loop->first, maze_it->first);
			if (NULL != sp_logic) sp_logic->OnMRActivityFinish();
		}
	}
}

TeamParamCommonData::TeamParamCommonData()
{
}

int TeamParamCommonData::GetAmuletIdByEffectType(int effect_type)
{
	TeamAmuletInfo * amulet_info_ptr = this->GetAmuletInfo(effect_type);

	if (amulet_info_ptr == NULL)
	{
		return -1;
	}

	TeamAmuletInfo & amulet_info = *amulet_info_ptr;

	switch (effect_type)
	{
		case MAZE_AMULET_EFFECT_SET_BATTLE_SPEED:		// 生效次数类： effect_times - 剩余生效次数
		case MAZE_AMULET_EFFECT_SET_WAR_INTERVAL:
			{
				if (amulet_info.effect_time <= 0)
				{
					return -1;
				}
			}
			break;

		case MAZE_AMULET_EFFECT_SET_MOVES_PERMIT:		// 持续时间类： effect_times - 失效时间戳
		case MAZE_AMULET_EFFECT_SET_ENCNT_PERMIT:
			{
				time_t now = EngineAdapter::Instance().Time();
				if (now > amulet_info.effect_time)
				{
					return -1;
				}
			}
			break;
	}

	return amulet_info.amulet_id;
}

int TeamParamCommonData::GetFirstAmuletId()
{
	if (team_mmulet_info.empty())
	{
		return -1;
	}

	return team_mmulet_info.begin()->second.amulet_id;
}

TeamAmuletInfo * TeamParamCommonData::GetAmuletInfo(int effect_type)
{
	std::map<int, TeamAmuletInfo >::iterator iter = team_mmulet_info.find(effect_type);
	if (iter == team_mmulet_info.end())
	{
		return NULL;
	}

	return &iter->second;
}
