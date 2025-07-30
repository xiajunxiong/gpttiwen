#include "robotmanager.hpp"
#include "robotconfig.hpp"
#include "servercommon/robotdef.hpp"
#include "obj/character/robot.h"
#include "scene/scene.h"
#include "config/logicconfigmanager.hpp"
#include "servercommon/serverconfig/nameconfig.h"
#include "servercommon/serverconfig/localconfig.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "gamelog.h"
#include "global/team/team.hpp"
#include "global/team/teammanager.hpp"
#include <assert.h>
#include "other/pet/petconfig.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/serverconfig/appearanceconfig.hpp"
#include "ai/robotai/robotaimission.hpp"
#include "task/taskpool.h"
#include "config/otherconfig/professionconfig.hpp"
#include "scene/map.h"

RobotManager* gRobotMgr = NULL;
RobotManager& RobotManager::Instance() 
{
	if (NULL == gRobotMgr)
	{
		gRobotMgr = new RobotManager();
	}
	return *gRobotMgr;
}

bool RobotManager::IsRobot(int role_id)
{
	UserID user_id = IntToUid(role_id);
	return ROBOT_DB_INDEX == user_id.db_index;
}

void RobotManager::Update(time_t now_second)
{
	if (m_next_recalc_mission_robot_time < now_second)
	{
		this->RecalcMissionRobotCreat(now_second);
		m_next_recalc_mission_robot_time = now_second + 300;
	}

	this->CheckMissionRobot(now_second);
	this->CheckRangerRobot(now_second);
	this->CheckStaticRobot(now_second);
}

Robot* RobotManager::CreateRobotToScene(Scene* scene, const Posi& pos, int level, bool has_pet, const char* caller_func, int creator_sid /* = 0 */, ARG_IN int* min_level /* = NULL */, ARG_IN int* max_level /* = NULL */)
{
	static RobotParam param;
	param.Reset();

	if (!this->ConstructRobotParamByLevel(level, ROBOT_CONFIG_USE_TYPE_ZHUZHAN, has_pet, &param, creator_sid))
	{
		return NULL;
	}
	
	if (NULL != min_level && level < *min_level)
	{
		param.level = *min_level;
		param.role_battle_data.role_character_data.level = *min_level;

		if (param.role_battle_data.pet_num > 0)
		{
			param.role_battle_data.pet_character_data[0].battle_data.level = *min_level;
		}
	}

	if (NULL != max_level && level > *max_level)
	{
		param.level = *max_level;
		param.role_battle_data.role_character_data.level = *max_level;

		if (param.role_battle_data.pet_num > 0)
		{
			param.role_battle_data.pet_character_data[0].battle_data.level = *max_level;
		}
	}

	return this->CreateRobotToScene(scene, pos, param, caller_func, creator_sid);
}

Robot* RobotManager::CreateRobotToScene(Scene* scene, const Posi& pos, const RobotParam& param, const char* caller_func, int creator_sid /* = 0 */)
{
	Robot* robot = new Robot();
	robot->Init(param);
	robot->SetPos(pos);

	//if (creator_sid > 0 && CrossConfig::Instance().IsHiddenServer())
	//{
	//	GameName new_name;
	//	SNPRINTF(new_name, sizeof(new_name), "%s_s%d", param.robot_name, creator_sid);
	//	robot->SetName(new_name);
	//}

	scene->AddObj(robot);
	this->AddRobotToLocalMap(robot->GetRobotID(), robot->GetId(), scene->GetIndex());

	gamelog::g_log_robot.printf(LL_INFO, "RobotManager::CreateRobotToScene, create robot by caller[%s] scene_id[%d] robot_id[%d]",
		caller_func, scene->GetSceneID(), robot->GetRobotID());
	return robot;
}

void RobotManager::RemoveRobot(Robot* robot, const char* caller_func, 
	bool also_delete_on_scene /* = true */, bool is_return_id /* = true */, 
	bool is_leave_team /* = true */)
{
	int robot_id = robot->GetRobotID();
	Team* my_team = robot->GetTeam();
	if (NULL != my_team)
	{
#ifdef _DEBUG
		Team* team = TeamManager::Instance().GetTeamByTeamIndex(robot->GetTeamIndex());
		assert(team == my_team);
#endif

		if (is_leave_team)
		{
			my_team->LeaveTeam(robot->GetRobotUserID());
			if (1 == my_team->GetMemberCount() && ROBOT_TYPE_TASK_ROBOT == robot->GetRobotType())
			{
				my_team->DismissTeamHelper();
			}
		}
		else
		{
			TeamMember* member = my_team->GetMemberInfoByUID(robot_id);
			if (NULL != member && member->IsRobot())
			{
			//	member->robot = NULL;
			}
		}
		robot->SetTeam(NULL);
	}
	
	this->RemoveRobotFromLocalMap(robot_id, caller_func);
					
	if (is_return_id)
	{
		this->ReturnRobotID(robot_id);
	}

	robot->SetRobotInvalid();
	robot->SetDelayDelete();
}

void RobotManager::RemoveRobotFromLocalMap(int robot_id, const char* caller_func)
{
	m_robot_local_map.erase(robot_id);
}

void RobotManager::AddRobotToLocalMap(int robot_id, ObjID obj_id, SceneIndex scene_idx)
{
	RobotLocal robot_local;
	robot_local.obj_id = obj_id;
	robot_local.scene_index = scene_idx;
	robot_local.create_time = (unsigned int)EngineAdapter::Instance().Time();
	m_robot_local_map.insert(RobotLocalMap::value_type(robot_id, robot_local));
}

Robot* RobotManager::GetRobot(int robot_id)
{
	RobotLocalMap::iterator it = m_robot_local_map.find(robot_id);
	if (it != m_robot_local_map.end())
	{
		const RobotLocal& robot_local = it->second;
		Scene* scene = World::GetInstWorld()->GetSceneManager()->GetSceneByIndex(robot_local.scene_index);
		if (NULL != scene)
		{
			Obj* obj = scene->GetObj(robot_local.obj_id);
			if (NULL != obj && Obj::OBJ_TYPE_ROBOT == obj->GetObjType())
			{
				Robot* robot = (Robot*)obj;
#ifdef _DEBUG
				assert(robot->GetRobotID() == robot_id);
#endif
				return robot;
			}
		}
	}
	return NULL;
}

bool RobotManager::RobotChangeScene(Robot* robot, int target_scene_id, int target_scene_key, const Posi& target_pos, const char* caller_func)
{
	Scene* old_scene = robot->GetScene();
	if (NULL == old_scene || 
		(old_scene->GetSceneID() == target_scene_id && old_scene->GetSceneKey() == target_scene_key)
		)
	{
		return false;
	}

	Scene* new_scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(target_scene_id, target_scene_key);
	if (NULL == new_scene)
	{
		return false;
	}

	Team* team = robot->GetTeam();
	int robot_id = robot->GetRobotID();
	TeamMember* member = NULL;
	if (NULL != team)
	{
		member = team->GetMemberInfoByUID(robot_id);
		//member->robot = NULL;
	}

	static RobotParam param;
	param.Reset();
	robot->GetInitParam(&param);
	this->RemoveRobot(robot, __FUNCTION__, true, false, false);

	Robot* new_robot = this->CreateRobotToScene(new_scene, target_pos, param, caller_func);
	if (NULL != member)
	{
		//member->robot = new_robot;
		new_robot->SetTeam(team);

		static Protocol::SCAddFollow msg;
		msg.reserve_sh = 0;
		msg.follow_team_id = team->GetTeamIndex();
		msg.add_objid = new_robot->GetId();

		new_scene->GetZoneMatrix()->NotifyAreaMsg(new_robot->GetObserHandle(), &msg, sizeof(msg));
		
		//team->SendUpdateFollowListMsg(new_robot);
	}

	return true;
}

int RobotManager::CalcTitleByProfession(int profession, int level)
{
	int advance_times = RobotConfig::Instance()->GetRobotAdvanceTimes(level);
	int small_times = RobotConfig::Instance()->GetRobotSmallAdvanceTimes(level);
	const CareerAdvanceCfg* cfg = LOGIC_CONFIG->GetProfessionConfig()->GetCareerAdvanceCfg(profession, small_times - 1);
	if (NULL == cfg)
	{
		// 找不到配置就用原来那套
		int base_prof = profession / PROFESSION_BASE;
		int prof_grade = profession % PROFESSION_BASE;
		std::vector<int> prof_title_list;	// 新职业称号表
		LOGIC_CONFIG->GetProfessionConfig()->GetAllTitleByBaseProf(base_prof, prof_title_list);	// 新职业称号
		if (0 < prof_grade && prof_title_list.size() >= prof_grade)
		{
			return prof_title_list.at(prof_grade - 1);
		}
	}
	else
	{
		return cfg->reward_title;
	}
	
	return 0;
}

bool RobotManager::ConstructRobotParamByRealRoleBattleData(int real_role_uid, const RoleBattleData & rbd, RobotParam * out_param)
{
	GameName name;
	F_STRNCPY(name, rbd.role_character_data.name, sizeof(GameName));
	std::string name_fenshen = this->GetRealRoleRobotName(name);
	F_STRNCPY(name, name_fenshen.c_str(), sizeof(GameName));

	int robot_id = this->GetRobotIDFromPool();
	out_param->plat_type = LocalConfig::Instance().GetPlatType();
	out_param->robot_id = robot_id;
	out_param->real_role_uid = real_role_uid;
	out_param->robot_type = ROBOT_TYPE_NORMAL;
	F_STRNCPY(out_param->robot_name, name, sizeof(GameName));
	out_param->level = rbd.role_character_data.level;
	out_param->profession = rbd.appearance.prof_base * PROFESSION_BASE + rbd.appearance.advance_times;
	out_param->avatar_type = rbd.appearance.avatar_type;
	out_param->role_battle_data = rbd;
	F_STRNCPY(out_param->role_battle_data.role_character_data.name, name, sizeof(GameName));

	out_param->role_battle_data.is_robot = 1;
	out_param->role_battle_data.role_character_data.character_id = robot_id;
	return true;
}

RobotManager::RobotManager() : m_robot_id_generator(NORMAL_ROBOT_ID_START)
{
	m_next_auto_creat_robot_check_time = 0;

	m_next_creat_mission_robot_time = 0;
	m_need_creat_mission_robot = 0;
	m_next_recalc_mission_robot_time = 0;

	m_first_check_ranger_robot_time = 0;
	m_next_check_ranger_robot_time = 0;


	m_first_check_static_robot_time = 0;
	m_next_check_static_robot_time = 0;
}

RobotManager::~RobotManager()
{

}

void RobotManager::GenerateSomeRobotID()
{
	for (int i = 0; i < 200; ++i)
	{
		if (m_robot_id_generator + 1 >= MAX_NORMAL_ROBOT_ID)
		{
			m_robot_id_generator = NORMAL_ROBOT_ID_START;
		}

		int id = ++m_robot_id_generator;
		int robot_id = (ROBOT_DB_INDEX << DB_INDEX_MARK_BIT) + id;
		if (m_robot_local_map.find(robot_id) == m_robot_local_map.end())
		{
			m_robot_id_pool.push(robot_id);
		}
	}
}

void RobotManager::ReturnRobotID(int robot_id)
{
	m_robot_id_pool.push(robot_id);
}

int RobotManager::GetRobotIDFromPool()
{
	if (m_robot_id_pool.empty())
	{
		this->GenerateSomeRobotID();
		if (m_robot_id_pool.empty())
		{
			return 0;
		}
	}

	int ret_id = m_robot_id_pool.front();
	m_robot_id_pool.pop();

	return ret_id;
}

bool RobotManager::ConstructRobotParam(const RobotRoleCfg* cfg, const RobotPetCfg* pet_cfg, RobotParam * out_param, int creator_sid /* = 0 */, int name_type /*= ROBOT_NAME_TYPE_DEF*/)
{
	int robot_id = this->GetRobotIDFromPool();
	GameName robot_name;

	// 先计算职业（要根据职业随机模型）
	out_param->profession = cfg->base_prof * PROFESSION_BASE + cfg->advance_times;

	//先模型确定性别
	out_param->avatar_type = AppearanceConfig::Instance().RandAvatarByProfession(out_param->profession);

	//根据性别获得名字
	ROBOTCONFIG->GetRandomName(SEX_ARRAY[out_param->avatar_type], 
		robot_name, true, true, true, name_type);
	if (creator_sid > 0 && CrossConfig::Instance().IsHiddenServer())
	{
		GameName new_name = { 0 };
		SNPRINTF(new_name, sizeof(new_name), "%s_s%d", robot_name, creator_sid);
		F_STRNCPY(robot_name, new_name, sizeof(robot_name));
	}

	out_param->plat_type = LocalConfig::Instance().GetPlatType();
	out_param->robot_id = robot_id;
	F_STRNCPY(out_param->robot_name, robot_name, sizeof(GameName));
	out_param->level = cfg->level;
	
	out_param->role_battle_data.appearance.avatar_type = out_param->avatar_type;
	out_param->role_battle_data.appearance.prof_base = cfg->base_prof;
	out_param->role_battle_data.appearance.advance_times = cfg->advance_times;
	out_param->role_battle_data.appearance.color = rand() % INIT_APPREARANCE_COLOR_NUM;
	out_param->role_battle_data.appearance.weapon_id = cfg->weapon_id;
	out_param->role_battle_data.appearance.sub_weapon_id = 0;
	out_param->role_battle_data.appearance.wear_surface[SURFACE_TYPE_BODY] = cfg->surface_id;
	out_param->role_battle_data.appearance.wear_surface[SURFACE_TYPE_ARM] = cfg->surface_arms_id;
	out_param->role_battle_data.appearance.wear_surface[SURFACE_TYPE_MOUNT] = cfg->mount_id;

	out_param->role_battle_data.role_stand_row = rand() % 2;

	BattleCharacterData& bcd = out_param->role_battle_data.role_character_data;
	bcd.character_id = robot_id;
	bcd.character_type = BATTLE_CHARACTER_TYPE_ROLE_SHADOW;
	F_STRNCPY(bcd.name, robot_name, sizeof(GameName));
	bcd.is_use_cur_hp_mp = 1;
	bcd.level = cfg->level;
	bcd.profession = out_param->profession;
	bcd.race = RACE_TYPE_HUMAN;
	bcd.is_long_range_unit = 0;
	bcd.weapon_type = cfg->weapon_type;
	bcd.weapon_id = cfg->weapon_id;
	for (int i = 0; i < ARRAY_ITEM_COUNT(cfg->attr_con.skill_list); ++i)
	{
		if (cfg->attr_con.skill_list[i].skill_id > 0)
		{
			bcd.skill_list[bcd.skill_num].skill_id = cfg->attr_con.skill_list[i].skill_id;
			bcd.skill_list[bcd.skill_num].skill_level = cfg->attr_con.skill_list[i].skill_lv;
			bcd.skill_num += 1;
		}
	}
	cfg->attr_con.GetAttrList(bcd.attr_list);
	bcd.attr_list[BATTLE_ATTR_CUR_HP] = bcd.attr_list[BATTLE_ATTR_MAX_HP];
	bcd.attr_list[BATTLE_ATTR_CUR_MP] = bcd.attr_list[BATTLE_ATTR_MAX_MP];

	// 添加机器人宠物
	if (NULL != pet_cfg)
	{
			out_param->role_battle_data.pet_num = 1;
			out_param->role_battle_data.fight_pet_idx = 0;
			PetBattleData& pbd = out_param->role_battle_data.pet_character_data[0];

			pbd.auto_move_type = 0;
			pbd.auto_skill_id = 0;
			pbd.auto_skill_level = 0;
			pbd.exclusive_passive_skill_OBSOLETE = 0;
			memset(pbd.passive_list, 0, sizeof(pbd.passive_list));
			memset(pbd.other_sp_list, 0, sizeof(pbd.other_sp_list));

			pbd.battle_data.character_id = pet_cfg->pet_id;
			pbd.battle_data.id_param = 0;
			pbd.battle_data.character_type = BATTLE_CHARACTER_TYPE_PET_SHADOW;
			F_STRNCPY(pbd.battle_data.name, pet_cfg->name, sizeof(pbd.battle_data.name));
			pbd.battle_data.is_use_cur_hp_mp = 1;
			pbd.battle_data.level = pet_cfg->level;
			pbd.battle_data.param = pet_cfg->attr_con.strengthen_level;
			pbd.battle_data.profession = 0;
			pbd.battle_data.race = static_cast<char>(pet_cfg->pet_race);
			pbd.battle_data.is_long_range_unit = static_cast<char>(pet_cfg->is_long_range_unit);
			pbd.battle_data.weapon_type = WEAPON_TYPE_NONE;
			pbd.battle_data.weapon_id = 0;
			for (int i = 0; i < ARRAY_ITEM_COUNT(pet_cfg->attr_con.skill_list); ++i)
			{
				if (pet_cfg->attr_con.skill_list[i].skill_id > 0)
				{
					pbd.battle_data.skill_list[pbd.battle_data.skill_num].skill_id = pet_cfg->attr_con.skill_list[i].skill_id;
					pbd.battle_data.skill_list[pbd.battle_data.skill_num].skill_level = pet_cfg->attr_con.skill_list[i].skill_lv;
					pbd.battle_data.skill_num += 1;
				}
			}
			pet_cfg->attr_con.GetAttrList(pbd.battle_data.attr_list);
			pbd.battle_data.attr_list[BATTLE_ATTR_CUR_HP] = pbd.battle_data.attr_list[BATTLE_ATTR_MAX_HP];
			pbd.battle_data.attr_list[BATTLE_ATTR_CUR_MP] = pbd.battle_data.attr_list[BATTLE_ATTR_MAX_MP];
		
	}

	return true;
}

std::string RobotManager::GetRealRoleRobotName(GameName name)
{
	std::string name_str = name;
	size_t _s_pos = name_str.rfind("_s");
	if (_s_pos != std::string::npos)
	{
		std::string name_sub = name_str.substr(0, _s_pos);
		name_str = name_sub;
	}
	
	return name_str + ROBOTCONFIG->GetOtherCfg().real_role_robot_suffix;
}

void RobotManager::CheckMissionRobot(time_t now_second)
{
	if (1 != m_need_creat_mission_robot)
	{
		return;
	}

	if (m_next_creat_mission_robot_time > now_second)
	{
		return;
	}

	if (CrossConfig::Instance().IsHiddenServer() && !World::GetInstWorld()->RandCanCreatAutoRobotServerID(NULL))
	{
		// 隐藏服，且暂未有母服接入，不生成机器人，因为机器人名字后缀无法拼接
		return;
	}

	const MissionRobotCreatCfg& cfg_info = ROBOTCONFIG->GetMissionCreatCfg();

	if (0 == m_next_creat_mission_robot_time)
	{
		m_next_creat_mission_robot_time = now_second + cfg_info.task_robot_refresh_cd;
		return;
	}
	m_next_creat_mission_robot_time = now_second + cfg_info.task_robot_refresh_cd;
	RobotParam robot_param;
	robot_param.robot_type = ROBOT_TYPE_MISSION_ROBOT;

	UNSTD_STATIC_CHECK(sizeof(RobotAIMission::InitParam) <= sizeof(robot_param.robot_ai_param));
	RobotAIMission::InitParam * ai_init_param = (RobotAIMission::InitParam *)(&robot_param.robot_ai_param);
	ai_init_param->m_last_task_id = cfg_info.final_task_id;
	ai_init_param->m_cur_task_id = TASKPOOL->GetRobotMissionBeginTask();
	ai_init_param->m_force_next_task_status = now_second + RobotAIMission::TASK_STATUS_OUT_TIME;

	ai_init_param->m_task_status = RobotAIMission::TASK_STATUS_INVALID;
	ai_init_param->m_task_show_action = RobotAIMission::TASK_SHOW_ACTION_NONE;
	ai_init_param->m_task_show_action_param = 0;
	ai_init_param->m_action_target_posi = Posi(-1, -1);

	if (!this->ConstructRobotParamByLevel(1, ROBOT_CONFIG_USE_TYPE_MISSION_ROBOT, true, &robot_param, 0, RobotConfig::ROBOT_NAME_TYPE_ROLE))
	{
		return;
	}

	robot_param.role_battle_data.wear_title_id = this->CalcTitleByProfession(robot_param.profession, robot_param.level);

	Posi creat_posi(cfg_info.pos_x, cfg_info.pos_y);

	Scene * target_scene = World::GetInstWorld()->GetSceneManager()
		->GetSceneById(cfg_info.scene_id, COMMON_SCENE_KEY);
	if (NULL == target_scene)
	{
		return;
	}

	this->CreateRobotToScene(target_scene, creat_posi, robot_param, __FUNCTION__);
// 	Robot * temp_robot = 
// 	if (NULL == temp_robot)
// 	{
// 		return;
// 	}

}

void RobotManager::RecalcMissionRobotCreat(time_t now_second)
{
	int server_open_day_index = GetDayIndex(World::GetInstWorld()->GetMaxServerOpenTime(), now_second);
	
	const MissionRobotCreatCfg& cfg_info = ROBOTCONFIG->GetMissionCreatCfg();
	if (0 == cfg_info.robot_show_days
		|| cfg_info.robot_show_days < server_open_day_index + 1)
	{
		m_need_creat_mission_robot = 0;
	}
	else
	{
		m_need_creat_mission_robot = 1;
	}
}

void RobotManager::CheckRangerRobot(time_t now_second)
{
	if (m_next_check_ranger_robot_time > now_second)
	{
		return;
	}
	m_next_check_ranger_robot_time = now_second + 60;
	if (0 == m_first_check_ranger_robot_time)
	{
		m_first_check_ranger_robot_time = now_second;
		return;
	}

	if (CrossConfig::Instance().IsHiddenServer() && !World::GetInstWorld()->RandCanCreatAutoRobotServerID(NULL))
	{
		// 隐藏服，且暂未有母服接入，不生成机器人，因为机器人名字后缀无法拼接
		return;
	}

	int cnt = 1000; // 避免无限循环
	while (--cnt)
	{
		RobotTimeMultiMap::iterator out_time_it = m_ranger_scene_out_time_map.begin();
		if (out_time_it == m_ranger_scene_out_time_map.end())
		{
			break;
		}
		if (out_time_it->first > now_second)
		{
			break;
		}
		RobotQueryParam robot_query_param = out_time_it->second;
		m_ranger_scene_out_time_map.erase(out_time_it);

		SceneRobotMap::iterator it = m_ranger_scene_map.find(robot_query_param.scene_id);
		if (it == m_ranger_scene_map.end())
		{
			continue;
		}
		it->second.erase(robot_query_param.robot_id);
		
		Robot* temp_robot = this->GetRobot(robot_query_param.robot_id);
		if (NULL == temp_robot)
		{
			continue;
		}
		this->RemoveRobot(temp_robot, __FUNCTION__);
	}

	if (1 != m_need_creat_mission_robot)
	{
		return;
	}

	//创建
	const RobotConfig::RangerCreatMap &creat_map = ROBOTCONFIG->GetRangerCreatCfg();
	RobotConfig::RangerCreatMap::const_iterator scene_creat_cit = creat_map.begin();
	for (; scene_creat_cit != creat_map.end(); scene_creat_cit++)
	{
		//检测创建时间
// 		if (scene_creat_cit->second.base_interval_time_s + m_first_check_ranger_robot_time > now_second)
// 		{
// 			continue;
// 		}

		//检测创建数量
		RobotIDMap& scene_now_robot_map = m_ranger_scene_map[scene_creat_cit->second.scene_id];

		int creat_num = scene_creat_cit->second.robot_num - scene_now_robot_map.size();
		for (int i = 0; i < creat_num; i++)
		{
			static RobotParam robot_param;
			robot_param.Reset();
			robot_param.robot_type = ROBOT_TYPE_RANGER_ROBOT;
			int temp_level = scene_creat_cit->second.level + RandomNum(3);

			if (!this->ConstructRobotParamByLevel(temp_level, ROBOT_CONFIG_USE_TYPE_RANGER_ROBOT, true, &robot_param, 0, RobotConfig::ROBOT_NAME_TYPE_ROLE))
			{
				continue;
			}
			robot_param.role_battle_data.wear_title_id = this->CalcTitleByProfession(robot_param.profession, robot_param.level);

			Scene * target_scene = World::GetInstWorld()->GetSceneManager()
				->GetSceneById(scene_creat_cit->second.scene_id, COMMON_SCENE_KEY);
			if (NULL == target_scene)
			{
				continue;
			}

			Posi creat_posi;
			creat_posi = target_scene->GetMap()->GetARandWalkablePoint();
			if (creat_posi.x == 0 && creat_posi.y == 0)
			{
				target_scene->GetTownPoint(&creat_posi);
			}

			Robot * temp_robot = this->CreateRobotToScene(target_scene, creat_posi, robot_param, __FUNCTION__);
			if (NULL == temp_robot)
			{
				continue;
			}

			RobotQueryParam temp_query_param;
			temp_query_param.robot_id = temp_robot->GetRobotID();
			temp_query_param.scene_id = scene_creat_cit->second.scene_id;
			
			scene_now_robot_map[temp_robot->GetRobotID()] = temp_query_param;

			time_t time_out = now_second + scene_creat_cit->second.base_interval_time_s + RandomNum(scene_creat_cit->second.range_interval_time_s);
			
			RobotTimeMultiMap::value_type  temp_value(time_out, temp_query_param);
			m_ranger_scene_out_time_map.insert(temp_value);
		}
	}
}

void RobotManager::CheckStaticRobot(time_t now_second)
{
	if (m_next_check_static_robot_time > now_second)
	{
		return;
	}
	m_next_check_static_robot_time = now_second + 60;
	if (0 == m_first_check_static_robot_time)
	{
		m_first_check_static_robot_time = now_second;
		return;
	}

	if (CrossConfig::Instance().IsHiddenServer() && !World::GetInstWorld()->RandCanCreatAutoRobotServerID(NULL))
	{
		// 隐藏服，且暂未有母服接入，不生成机器人，因为机器人名字后缀无法拼接
		return;
	}

	//已占用的坐标点index   场景id  坐标点index_set
	std::map<int, std::set<int> > temp_use_point_info;
	
	int cnt = 1000; // 避免死循环
	while (--cnt)
	{
		RobotTimeMultiMap::iterator out_time_it = m_static_scene_out_time_map.begin();
		if (out_time_it == m_static_scene_out_time_map.end())
		{
			break;
		}
		if (out_time_it->first > now_second)
		{
			break;
		}
		RobotQueryParam robot_query_param = out_time_it->second;
		m_static_scene_out_time_map.erase(out_time_it);
		//删除后一般会新创 占用坐标点
		temp_use_point_info[robot_query_param.scene_id].insert(robot_query_param.creat_param);

		SceneRobotMap::iterator it = m_static_scene_map.find(robot_query_param.scene_id);
		if (it == m_static_scene_map.end())
		{
			continue;
		}
		it->second.erase(robot_query_param.robot_id);
		Robot* temp_robot = this->GetRobot(robot_query_param.robot_id);
		if (NULL == temp_robot)
		{
			continue;
		}
		this->RemoveRobot(temp_robot, __FUNCTION__);
	}

	if (1 != m_need_creat_mission_robot)
	{
		return;
	}

	//创建
	const RobotConfig::StaticCreatMap &creat_map = ROBOTCONFIG->GetStaticCreatCfg();
	RobotConfig::StaticCreatMap::const_iterator scene_creat_cit = creat_map.begin();
	for (; scene_creat_cit != creat_map.end(); scene_creat_cit++)
	{
		//检测创建时间
// 		if (scene_creat_cit->second.range_interval_time_s + m_first_check_static_robot_time > now_second)
// 		{
// 			continue;
// 		}

		//检测创建数量
		RobotIDMap & scene_now_robot_map = m_static_scene_map[scene_creat_cit->second.scene_id];

		int creat_num = scene_creat_cit->second.robot_num - scene_now_robot_map.size();
		if (0 >= creat_num)
		{
			continue;
		}
		//占用坐标点
		{
			RobotIDMap::iterator scene_now_robot_it = scene_now_robot_map.begin();
			for (; scene_now_robot_it != scene_now_robot_map.end(); scene_now_robot_it++)
			{
				temp_use_point_info[scene_now_robot_it->second.scene_id].insert(scene_now_robot_it->second.creat_param);
			}
		}

		std::vector<StaticRobotPosiCfg> can_use_posi;
		{
			const RobotConfig::StaticCreatPosiVec *scene_point_vec = ROBOTCONFIG->GetStaticCreatPosiCfg(scene_creat_cit->second.scene_id);
			if (NULL == scene_point_vec)
			{
				continue;
			}

			std::set<int>& used_posi_set = temp_use_point_info[scene_creat_cit->second.scene_id];

			StaticRobotPosiCfg temp_info;
			temp_info.scene_id = scene_creat_cit->second.scene_id;

			int total_posi_num = scene_point_vec->size();
			for (int index = 0; index < total_posi_num; index++)
			{
				if (used_posi_set.end() == used_posi_set.find(index))
				{
					temp_info.pos_x = scene_point_vec->at(index).pos_x;
					temp_info.pos_y = scene_point_vec->at(index).pos_y;
					temp_info.posi_index = scene_point_vec->at(index).posi_index;

					can_use_posi.push_back(temp_info);
				}
			}
		}
		int can_use_posi_num = can_use_posi.size();
		std::random_shuffle(can_use_posi.begin(), can_use_posi.end());

		for (int i = 0; i < creat_num && i < can_use_posi_num; i++)
		{
			static RobotParam robot_param;
			robot_param.Reset();
			robot_param.robot_type = ROBOT_TYPE_STATIC_ROBOT;
			int temp_level = scene_creat_cit->second.level + RandomNum(3);

			if (!this->ConstructRobotParamByLevel(temp_level, ROBOT_CONFIG_USE_TYPE_STATIC_ROBOT, true, &robot_param, 0, RobotConfig::ROBOT_NAME_TYPE_ROLE))
			{
				continue;
			}
			robot_param.role_battle_data.wear_title_id = this->CalcTitleByProfession(robot_param.profession, robot_param.level);

			Scene * target_scene = World::GetInstWorld()->GetSceneManager()
				->GetSceneById(scene_creat_cit->second.scene_id, COMMON_SCENE_KEY);
			if (NULL == target_scene)
			{
				continue;
			}

			Posi creat_posi(can_use_posi[i].pos_x, can_use_posi[i].pos_y);

			Robot * temp_robot = this->CreateRobotToScene(target_scene, creat_posi, robot_param, __FUNCTION__);
			if (NULL == temp_robot)
			{
				continue;
			}
			//随机个方向  不用太精确
			Dir rand_dir = RandomNum(31) / (Dir)10 * (0 == RandomNum(2) ? 1 : -1);
			temp_robot->SetDir(rand_dir);

			RobotQueryParam temp_query_param;
			temp_query_param.robot_id = temp_robot->GetRobotID();
			temp_query_param.scene_id = scene_creat_cit->second.scene_id;
			temp_query_param.creat_param = can_use_posi[i].posi_index;

			scene_now_robot_map[temp_robot->GetRobotID()] = temp_query_param;

			time_t time_out = now_second + RandomNum(scene_creat_cit->second.range_interval_time_s);

			RobotTimeMultiMap::value_type  temp_value(time_out, temp_query_param);
			m_static_scene_out_time_map.insert(temp_value);
		}
	}
}

bool RobotManager::ConstructRobotParamByLevel(int level, int use_way, bool has_pet, RobotParam* out_param, int creator_sid /* = 0 */, int name_type /*= ROBOT_NAME_TYPE_DEF*/)
{
	if (NULL == out_param) return false;

	const RobotRoleCfg* cfg = ROBOTCONFIG->RandRobotByLevel(level, use_way);
	if (NULL == cfg) return false;

	const RobotPetCfg* pet_cfg = NULL;
	if (has_pet)
	{
		int pet_level = GetMax(1, level - RandomNum(5));
		pet_cfg = ROBOTCONFIG->RandRobotPetByLevel(pet_level, use_way);
	}

	return this->ConstructRobotParam(cfg, pet_cfg, out_param, creator_sid, name_type);
}

bool RobotManager::ConstructRobotParamByLevelAndProf(int level, int base_prof, int use_way, bool has_pet, RobotParam * out_param, int creator_sid /* = 0 */)
{
	if (NULL == out_param) return false;

	const RobotRoleCfg* cfg = ROBOTCONFIG->RandRobotByLevelAndProf(level, base_prof, use_way);
	if (NULL == cfg) return false;

	const RobotPetCfg* pet_cfg = NULL;
	if (has_pet)
	{
		int pet_level = GetMax(1, level - RandomNum(5));
		pet_cfg = ROBOTCONFIG->RandRobotPetByLevel(pet_level, use_way);
	}

	return this->ConstructRobotParam(cfg, pet_cfg, out_param, creator_sid);
}

