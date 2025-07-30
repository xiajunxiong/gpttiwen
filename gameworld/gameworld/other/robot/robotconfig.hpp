#ifndef __ROBOT_CONFIG_HPP__
#define __ROBOT_CONFIG_HPP__

#include "gamedef.h"
#include "servercommon/configcommon.h"
#include "servercommon/facescoredef.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/partnerdef.h"
#include "servercommon/teamdef.hpp"
#include "servercommon/serverconfig/nameconfig.h"
#include "servercommon/robotdef.hpp"

#include <set>
#include <map>
#include <vector>
#include<functional>

static const int ROBOT_ROLE_SKILL_MAX = 13;
UNSTD_STATIC_CHECK(ROBOT_ROLE_SKILL_MAX <= MAX_BATTLE_SKILL_NUM)

struct PlatformBattleRobotSkillEntry
{
	PlatformBattleRobotSkillEntry()
	{
		skill_id = 0;
		skill_level = 0;
	}

	int skill_id;
	int skill_level;
};

struct PlatformBattlePartnerRobotCfg
{
	const int static PARTNER_MAX_SKILL_NUM = 3;

	PlatformBattlePartnerRobotCfg()
	{
		id = 0;
		partner_id = 0;
		level = 0;
		memset(name, 0, sizeof(name));
		prof = 0;
		weapon_type = 0;

		for (int i = 0; i < ARRAY_LENGTH(attr_list); ++i)
		{
			attr_list[i] = 0;
		}
	}


	int GetCapability() const;

	int id;
	int partner_id;
	int level;
	GameName name;
	int prof;
	int weapon_type;
	Attribute attr_list[BATTLE_ATTR_MAX];
	
	std::vector<PlatformBattleRobotSkillEntry> skill_list;

};


/* ================================================================ */
struct RobotSkillCon
{
	RobotSkillCon() : skill_id(0), skill_lv(0) {}

	UInt16 skill_id;
	UInt16 skill_lv;
};

struct RobotAttrCon
{
	RobotAttrCon() { this->Reset(); }

	void Reset()
	{
		strengthen_level = 0;
		max_hp = 0;
		max_mp = 0;
		memset(basic_attr_list, 0, sizeof(basic_attr_list));
		memset(amend_attr_list, 0, sizeof(amend_attr_list));
		memset(resis_attr_list, 0, sizeof(resis_attr_list));
		memset(element_list, 0, sizeof(element_list));
		memset(skill_list, 0, sizeof(skill_list));
	}

	void GetAttrList(BattleAttr* out_list) const
	{
		out_list[BATTLE_ATTR_MAX_HP] = max_hp;
		out_list[BATTLE_ATTR_MAX_MP] = max_mp;
		memcpy(out_list, basic_attr_list, sizeof(basic_attr_list));
		memcpy(&out_list[BATTLE_ATTR_CORRECTED_VALUE_BEGIN], amend_attr_list, sizeof(amend_attr_list));
		memcpy(&out_list[BATTLE_ATTR_ANTI_VALUE_BEGIN], resis_attr_list, sizeof(resis_attr_list));
		memcpy(&out_list[BATTLE_ATTR_ELEMENT_BEGIN], element_list, sizeof(element_list));
	}

	int strengthen_level;
	Attribute max_hp;
	Attribute max_mp;
	Attribute basic_attr_list[BATTLE_BASE_ATTR_NUM];
	Attribute amend_attr_list[BATTLE_AMEN_ATTR_NUM];
	Attribute resis_attr_list[BATTLE_RESI_ATTR_NUM];
	Attribute element_list[BATTLE_ELEMENT_ATTR_NUM];
	RobotSkillCon skill_list[ROBOT_ROLE_SKILL_MAX];

	UNSTD_STATIC_CHECK(BATTLE_ATTR_MAX >= BATTLE_BASE_ATTR_NUM);
};

struct RobotRoleCfg
{
	RobotRoleCfg() : use_way(0), level(0), base_prof(0), advance_times(0), small_advance_times(0),
		weapon_type(0), weapon_id(0), surface_id(0), surface_arms_id(0),
		mount_id(0)
	{
		attr_con.Reset();
	}

	int use_way;
	int level;
	int base_prof;
	int advance_times;
	int small_advance_times;

	int weapon_type;
	int weapon_id;
	RobotAttrCon attr_con;

	int surface_id;
	int surface_arms_id;
	int mount_id;
};

struct RobotPetCfg
{
	RobotPetCfg() : use_way(0), level(0), pet_id(0), pet_race(RACE_TYPE_HUMAN), pet_attr_type(0), is_long_range_unit(0)
	{
		memset(name, 0, sizeof(name));
		attr_con.Reset();
	}

	void GetAttr(BattleAttr(ARG_OUT &attr_list)[BATTLE_ATTR_MAX]) const
	{
		attr_list[BATTLE_ATTR_MAX_HP] =attr_con.max_hp;
		attr_list[BATTLE_ATTR_MAX_MP] =attr_con.max_mp;
		attr_list[BATTLE_ATTR_CUR_HP] = attr_con.max_hp;
		attr_list[BATTLE_ATTR_CUR_MP] =attr_con.max_mp;
		memcpy(attr_list, attr_con.basic_attr_list, sizeof(attr_con.basic_attr_list));
		memcpy(&attr_list[BATTLE_ATTR_CORRECTED_VALUE_BEGIN],attr_con.amend_attr_list, sizeof(attr_con.amend_attr_list));
		memcpy(&attr_list[BATTLE_ATTR_ANTI_VALUE_BEGIN], attr_con.resis_attr_list, sizeof(attr_con.resis_attr_list));
	}

	int GetCapability() const
	{
		BattleAttr attrs[BATTLE_ATTR_MAX] = { 0 };
		this->GetAttr(attrs);

		double gongjixishu_1 = 1 + attrs[BATTLE_ATTR_CRITICAL_INC_VALUE] * 0.00081 + (attrs[BATTLE_ATTR_CRITICAL] + attrs[BATTLE_ATTR_CRITICAL_DEC_VALUE]) *
			0.000242 + attrs[BATTLE_ATTR_COUNTER_ATTACK] * 0.0054;
		double gongjixishu_2 = 1 + (attrs[BATTLE_ATTR_HIT] + attrs[BATTLE_ATTR_DODGE]) * 0.001;
		double jingshenxishu_1 = 1 + 1.2 * attrs[BATTLE_ATTR_MAGIC_ATTACK] / (attrs[BATTLE_ATTR_MAGIC_ATTACK] + 220) + 0.8 * attrs[BATTLE_ATTR_MAGIC_DEFENSE] /
			(attrs[BATTLE_ATTR_MAGIC_DEFENSE] + 300);
		int capability = (int)(attrs[BATTLE_ATTR_MAX_MP] * 0.5 + attrs[BATTLE_ATTR_MAX_HP] * 0.125 + attrs[BATTLE_ATTR_ATTACK] * gongjixishu_1 * gongjixishu_2 +
				       attrs[BATTLE_ATTR_DEFENSE] + attrs[BATTLE_ATTR_MENTAL] * 2 * jingshenxishu_1 + attrs[BATTLE_ATTR_RECOVERY] * 2.27 +
				       attrs[BATTLE_ATTR_AGILITY] * 1.75);
		return capability;
	}

	int use_way;
	int level;
	int pet_id;
	int pet_race;
	int pet_attr_type;	// 宠物属性类型， 根据该类型读取属性
	int is_long_range_unit;
	GameName name;
	RobotAttrCon attr_con;
};

struct RobotPartnerCfg
{
	RobotPartnerCfg() : use_way(0), level(0), partner_id(0), profession(0)
	{
		memset(name, 0, sizeof(name));
		attr_con.Reset();
	}

	void GetAttr(Attribute(ARG_OUT &attr_list)[BATTLE_ATTR_MAX]) const
	{
		attr_list[BATTLE_ATTR_MAX_HP] = attr_con.max_hp;
		attr_list[BATTLE_ATTR_MAX_MP] = attr_con.max_mp;
		attr_list[BATTLE_ATTR_CUR_HP] = attr_con.max_hp;
		attr_list[BATTLE_ATTR_CUR_MP] = attr_con.max_mp;
		memcpy(attr_list, attr_con.basic_attr_list, sizeof(attr_con.basic_attr_list));
		memcpy(&attr_list[BATTLE_ATTR_CORRECTED_VALUE_BEGIN], attr_con.amend_attr_list, sizeof(attr_con.amend_attr_list));
		memcpy(&attr_list[BATTLE_ATTR_ANTI_VALUE_BEGIN], attr_con.resis_attr_list, sizeof(attr_con.resis_attr_list));
	}

	int GetCapability(Attribute(ARG_IN &attr_list)[BATTLE_ATTR_MAX]) const;

	int use_way;
	int level;
	int partner_id;
	int profession;
	GameName name;
	RobotAttrCon attr_con;
};

/* ================================================================ */

struct RobotPetAttrCfg
{
	int attr_type;
	RobotAttrCon attr_con_list[MAX_ROLE_LEVEL + 1];
};

struct TeamTypeRobotCfg
{
	TeamTypeRobotCfg() : is_run_robot(false), is_user_create_team(false), 
		battle_mode(0), team_type(0), add_robot_seconds(0), level_group_id(0), 
		min_add_robot_interval_s(0), max_add_robot_interval_s(0), robot_limit(0),
		use_real_role_robot(false) {}

	bool is_run_robot;
	bool is_user_create_team;
	int battle_mode;		// 对应的战斗玩法
	int team_type;			// 对应的组队类型
	int add_robot_seconds;
	int level_group_id;
	int min_add_robot_interval_s;
	int max_add_robot_interval_s;
	int robot_limit;
	bool use_real_role_robot;
};

struct RobotOtherCfg
{
	RobotOtherCfg() : robot_pet_mutate_rate(0), reserved(0), jjc_robot_weaken_attr_percent(0)
	{
		start_capacity_target_task_id = 0;
		capacity_target_time = 0;
		capacity_target_robot_level = 0;
		off_auto_talk_level = 0;
		match_level_down = 0;
		match_level_up = 0;
		match_real_role_robot_level = 0;
	}

	int robot_pet_mutate_rate;
	int reserved;
	int jjc_robot_weaken_attr_percent;

	int start_capacity_target_task_id;
	int capacity_target_time;
	int capacity_target_robot_value;
	int capacity_target_robot_level;	
	std::vector<ItemConfigData> capacity_target_win_reward;
	std::vector<ItemConfigData> capacity_target_lose_reward;
	int off_auto_talk_level;			//自动对话关闭等级

	int match_level_down;
	int match_level_up;
	int match_real_role_robot_level;
	std::string real_role_robot_suffix;

	std::vector<ItemConfigData> real_role_robot_rewards;
};


static const int ROBOT_PET_ATTR_NUM_MAX = 3;
static const int ROBOT_LEVEL_GROUP_NUM_MAX = 5;
static const int ROBOT_TEAM_TYPE_CFG_NUM = 10;

struct PlatformBattlePlatformInfo;
struct PlatformBattleRobotInfo;
//主线机器人
struct MissionRobotCreatCfg
{
	MissionRobotCreatCfg()
	{
		robot_show_days = 0;
		task_robot_refresh_cd = 0;
		final_task_id = 0;

		scene_id = 0;
		pos_x = 0;
		pos_y = 0;
	}
	int robot_show_days;	//开服前n天需要创建跑图机器人 0 代表不创建
	
	int task_robot_refresh_cd;	//创建事件间隔(s)
	int final_task_id;		//最后任务id
	
	//创建位置
	int scene_id;
	int pos_x;				
	int pos_y;
};

//巡逻(刷怪)机器人创建配置
struct RangerRobotCreatCfg
{
	RangerRobotCreatCfg()
	{
		scene_id = 0;
		level = 0;
		base_interval_time_s = 0;
		range_interval_time_s = SECOND_PER_MIN * 30;

		robot_num = 0;
	}

	int scene_id;		//场景id
	int level;			//等级(等级范围写死了)
	int base_interval_time_s;	//替换时间(s)
	int range_interval_time_s;	//替换时间浮动(s)(写死了)

	int robot_num;		//场景机器人数量
};

//静态机器人创建配置
struct StaticRobotCreatCfg
{
	StaticRobotCreatCfg()
	{
		scene_id = 0;
		robot_num = 0;
		level = 0;
		range_interval_time_s = 0;

	}
	int scene_id;		//场景id
	int robot_num;		//静态机器人
	int level;			//基础等级(等级范围写死了)
	int range_interval_time_s;	//替换时间浮动(s)
};

//静态机器人创建坐标
struct StaticRobotPosiCfg
{
	StaticRobotPosiCfg()
	{
		scene_id = 0;
		posi_index = 0;

		pos_x = 0;
		pos_y = 0;
	}
	int scene_id;
	int posi_index;

	int pos_x;
	int pos_y;
};

static const int MAX_AUTO_TALK_TALK_GROUPS_BASE_NUM = 10000;
static const int MAX_AUTO_TALK_TALK_GROUPS_NUM = 5;
struct AutoTalkCfg
{
	AutoTalkCfg() : talk_groups_id(0), talk_id(0), interval(0)
	{}

	int talk_groups_id;
	int talk_id;
	int interval;			//间隔(秒)
};

#define ROBOTCONFIG RobotConfig::Instance()

class RobotConfig
{
public:
	typedef std::vector<int> PartnerRandomGroup;
	typedef std::map<int, PartnerRandomGroup> RandomGroupMap;
	typedef std::map<int, RobotPartnerCfg> RobotPartnerMap;
	typedef std::map<int, int, std::greater<int> > RobotLevelMap;
	typedef std::map<int, RobotPetCfg*> RobotPetPtrMap;

	//巡逻(刷怪)机器人创建配置<场景id, 场景配置>
	typedef std::map<int, RangerRobotCreatCfg> RangerCreatMap;

	//静态机器人<场景id, 场景配置>
	typedef std::map<int, StaticRobotCreatCfg> StaticCreatMap;
	typedef std::vector<StaticRobotPosiCfg> StaticCreatPosiVec;
	typedef std::map<int, StaticCreatPosiVec> StaticCreatPosiMap;


	enum RobotNameType
	{
		ROBOT_NAME_TYPE_DEF = 0,		//机器人名字普通
		ROBOT_NAME_TYPE_ROLE,			//模拟玩家名字
	};

public:
	RobotConfig();
	~RobotConfig();

	static RobotConfig * Instance();

	bool Init(const std::string &configname, std::string *err);
	bool Reload(const std::string &configname, std::string *err);


	const RobotPetCfg* RandRobotPetByLevel(int level, int use_way);
	const RobotPetCfg* GetRobotPetByLevel(int pet_id, int level, int use_way);
	const RobotPartnerCfg* GetPartner(int partner_id, int level, int use_way);
	const RobotAttrCon* GetPetAttrByTypeAndLevel(int attr_type, int level, int use_way);
	const RobotRoleCfg* RandRobotByLevel(int level, int use_way);
	const RobotRoleCfg* RandRobotByLevelAndProf(int level, int prof, int use_way);
	RoleBattleData GetRobotBattleData(int level, int use_way);

	const NameCfg & GetNameCfg() const { return m_name_cfg; }
	bool GetRandomName(int sex, GameName out_name, bool need_prefix = true, bool need_infix = true, bool need_suffix = true, int name_type = ROBOT_NAME_TYPE_DEF);

	int RandRobotLevelByGroup(int group_id, int level);
	const TeamTypeRobotCfg* GetAutoTeamRobotCfg(int team_type);

	int GetRobotAdvanceTimes(int level);
	int GetRobotSmallAdvanceTimes(int level);
	void RandRobotPartnerList(int level, int max_num, int* out_num, int* out_list);
	int GetRandAutoGroupsId(int key_hash, int account_role_num);
	const AutoTalkCfg * GetAutoTalkCfg(int auto_talk_group_id, int talk_id);
	
	inline const RobotOtherCfg& GetOtherCfg() const { return m_other_cfg; }

	//主线机器人创建配置
	const MissionRobotCreatCfg &GetMissionCreatCfg() const { return m_mission_creat_cfg; };
	//巡逻(刷怪)机器人创建配置
	const RangerCreatMap & GetRangerCreatCfg() const { return m_ranger_creat_map; };
	//静态机器人创建配置
	const StaticCreatMap & GetStaticCreatCfg() const { return m_static_creat_map; };
	//静态机器人创建位置
	const StaticCreatPosiVec * GetStaticCreatPosiCfg(int scene_id) const;

	//-------------擂台战---------------------------
	void GetPlatformBattleRotbotCfg(int platform_number, int robot_index, PlatformBattleRobotInfo ARG_OUT * robot_info_item);
	std::vector<int >RandRobotPartnerList2(int level, int max_num);
	//-----------------------------------------------------------------------

	void AddBaseWeaponID(int weapon_type, ItemID weapon_id);
	ItemID GetBaseWeaponID(int weapon_type);
private:
	
	int InitRobotBaseWeaponID(TiXmlElement* RootElement);
	int InitRoleRobotCfg(TiXmlElement* RootElement);
	int InitPetRobotCfg(TiXmlElement* RootElement);
	int InitPetAttrCfg(TiXmlElement* RootElement);
	int InitRobotAdvanceCfg(TiXmlElement* RootElement);
	int InitRobotPartnerRandomGroupCfg(TiXmlElement* RootElement);
	int InitRobotPartnerRandomCfg(TiXmlElement* RootElement);
	int InitPartnerRobotCfg(TiXmlElement* RootElement);
	int InitRobotLevelMap(TiXmlElement* RootElement);
	int InitTeamTypeRobotCfg(TiXmlElement* RootElement);
	int InitOtherCfg(TiXmlElement* RootElement);
	int InitNameCfg(TiXmlElement* RootElement);
	int InitNameCfgImpl(TiXmlElement* RootElement, NameCfg::NameList& name_list, const char* sheet_name);
	int InitRoleNameCfg(TiXmlElement* RootElement);
	int InitAutoTalkCfg(TiXmlElement* RootElement);

	int InitMissionCreatCfg(TiXmlElement* RootElement);
	int InitRangerCreatCfg(TiXmlElement* RootElement);
	int InitStaticCfg(TiXmlElement* RootElement);
	int InitStaticCreatPosiCfg(TiXmlElement* RootElement);
	int InitCapacityTargetValueCfg(TiXmlElement * RootElement);

	RobotRoleCfg m_robot_role_list[ROBOT_CONFIG_USE_TYPE_MAX][MAX_ROLE_LEVEL + 1][PROFESSION_TYPE_NUM];
	std::vector<RobotPetCfg> m_robot_pet_level_vec[ROBOT_CONFIG_USE_TYPE_MAX][MAX_ROLE_LEVEL + 1];
	std::vector<RobotPetCfg> m_mutate_pet_level_vec[ROBOT_CONFIG_USE_TYPE_MAX][MAX_ROLE_LEVEL + 1];
	RobotPetAttrCfg m_pet_attr_type_cfg_list[ROBOT_CONFIG_USE_TYPE_MAX][ROBOT_PET_ATTR_NUM_MAX];
	RobotPetPtrMap m_pet_cfg_ptr_map[ROBOT_CONFIG_USE_TYPE_MAX][MAX_ROLE_LEVEL + 1];

	int m_robot_advance_times_list[MAX_ROLE_LEVEL + 1];
	int m_robot_small_advance_times_list[MAX_ROLE_LEVEL + 1];
	int m_robot_partner_random_group_list[MAX_ROLE_LEVEL + 1];
	RandomGroupMap m_partner_random_group_map;

	RobotPartnerCfg m_partner_robot_list[ROBOT_CONFIG_USE_TYPE_MAX][MAX_ROLE_LEVEL + 1][PARTNER_NUM_MAX];

	RobotLevelMap m_robot_level_map[ROBOT_LEVEL_GROUP_NUM_MAX];
	
	TeamTypeRobotCfg* m_team_type_robot_cfg[TEAM_TYPE_MAX];

	RobotOtherCfg m_other_cfg;

	MissionRobotCreatCfg m_mission_creat_cfg;
	RangerCreatMap m_ranger_creat_map;
	StaticCreatMap m_static_creat_map;
	StaticCreatPosiMap m_static_creat_posi_map;

	std::set<int> m_auto_talk_groups_set;		//自动对话组ID
	std::map<int, AutoTalkCfg> m_auto_talk_map;	//自动对话配置 key:auto_talk_group_id * MAX_AUTO_TALK_TALK_GROUPS_BASE_NUM + talk_id

	NameCfg m_name_cfg;

	NameCfg m_role_name_cfg;		//模拟玩家名字

	ItemID m_base_weapon_id_list[WEAPON_TYPE_MAX];//机器人基础武器
};

#endif