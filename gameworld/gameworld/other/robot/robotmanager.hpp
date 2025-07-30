#ifndef __ROBOT_MANAGER_HPP__
#define __ROBOT_MANAGER_HPP__

#include "servercommon/servercommon.h"
#include <queue>

#define ROBOTMANAGER RobotManager::Instance()

struct RobotLocal
{
	int obj_id;
	SceneIndex scene_index;
	unsigned int create_time;
};

struct RobotParam;
struct RobotRoleCfg;
struct RobotPetCfg;
class Robot;
class Scene;
struct RoleBattleData;
class RobotManager
{
public:
	typedef std::map<int, RobotLocal> RobotLocalMap; // robot_id 对应在哪个场景
	typedef std::queue<int> RobotIDPool;			 // 可用的robotid池

	struct RobotQueryParam
	{
		RobotQueryParam()
		{
			robot_id = 0;
			scene_id = 0;
			creat_param = 0;
		}
		int robot_id;
		int scene_id;
		int creat_param;
	};
	//机器人id查询<机器人id, param>
	typedef std::map<int, RobotQueryParam> RobotIDMap;
	//场景机器人id查询map<scene_id, 机器人id map>
	typedef std::map<int, RobotIDMap> SceneRobotMap;
	//机器人失效时间<失效时间, (机器人id, scene_id)>
	typedef std::multimap<time_t, RobotQueryParam> RobotTimeMultiMap;

public:
	static RobotManager& Instance();
	static bool IsRobot(int role_id);

	void Update(time_t now_second);

	Robot* CreateRobotToScene(Scene* scene, const Posi& pos, int level, bool has_pet, const char* caller_func, int creator_sid = 0, ARG_IN int* min_level = NULL, ARG_IN int* max_level = NULL);
	Robot* CreateRobotToScene(Scene* scene, const Posi& pos, const RobotParam& param, const char* caller_func, int creator_sid = 0);
	void RemoveRobot(Robot* robot, const char* caller_func, bool also_delete_on_scene = true, bool is_return_id = true, bool is_leave_team = true);
	void RemoveRobotFromLocalMap(int robot_id, const char* caller_func);
	void AddRobotToLocalMap(int robot_id, ObjID obj_id, SceneIndex scene_idx);
	Robot* GetRobot(int robot_id);
	bool ConstructRobotParamByLevel(int level, int use_way, bool has_pet, RobotParam* out_param, int creator_sid = 0, int name_type = 0);
	bool ConstructRobotParamByLevelAndProf(int level, int base_prof, int use_way, bool has_pet, RobotParam* out_param, int creator_sid = 0);
	bool RobotChangeScene(Robot* robot, int target_scene_id, int target_scene_key, const Posi& target_pos, const char* caller_func);

	int CalcTitleByProfession(int profession, int level);

	bool ConstructRobotParamByRealRoleBattleData(int real_role_uid, const RoleBattleData& rbd, RobotParam* out_param);
private:
	RobotManager();
	~RobotManager();
	RobotManager& operator = (const RobotManager&);
	RobotManager(const RobotManager&);

	void GenerateSomeRobotID();
	void ReturnRobotID(int robot_id);
	int GetRobotIDFromPool();
	bool ConstructRobotParam(const RobotRoleCfg* cfg, const RobotPetCfg* pet_cfg, RobotParam* out_param, int creator_sid = 0, int name_type = 0);
	std::string GetRealRoleRobotName(GameName name);

	int m_robot_id_generator;

	RobotLocalMap m_robot_local_map;
	RobotIDPool m_robot_id_pool;


	//下次 检测 自动创建机器人时间
	time_t m_next_auto_creat_robot_check_time;

	//主线机器人
	void CheckMissionRobot(time_t now_second);
	time_t m_next_creat_mission_robot_time;

	//改为控制全部机器人
	void RecalcMissionRobotCreat(time_t now_second);
	int m_need_creat_mission_robot;
	time_t m_next_recalc_mission_robot_time;

	//巡逻机器人场景map
	void CheckRangerRobot(time_t now_second);
	SceneRobotMap m_ranger_scene_map;
	RobotTimeMultiMap m_ranger_scene_out_time_map;
	time_t m_first_check_ranger_robot_time;

	time_t m_next_check_ranger_robot_time;

	//静态机器人场景map
	void CheckStaticRobot(time_t now_second);
	SceneRobotMap m_static_scene_map;
	RobotTimeMultiMap m_static_scene_out_time_map;
	time_t m_first_check_static_robot_time;
	time_t m_next_check_static_robot_time;
	

};

#endif