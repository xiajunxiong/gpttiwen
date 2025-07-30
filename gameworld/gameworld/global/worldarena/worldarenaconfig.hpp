#ifndef __WORLD_ARENA_CONFIG_HPP__
#define __WORLD_ARENA_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/serverdef.h"

static const int WORLD_ARENA_ROBOT_ATTR_ADJUST_PERCENT = 10000;
static const int WORLD_ARENA_ROBOT_ATTR_ADJUST_GROUP_NUM = 4;

struct WorldArenaOtherCfg
{
	WorldArenaOtherCfg() : standby_scene_id(0), max_limit_round(0), wait_enter_scene_minute(0), 
		win_score(0), lose_score(0), draw_score(0), give_up_score(0) {}

	int standby_scene_id;
	int max_limit_round;
	int wait_enter_scene_minute;
	int win_score;
	int lose_score;
	int draw_score;
	int give_up_score;
	Posi attacker_posi;
	Posi defender_posi;

	std::vector<ItemConfigData> day_reward_list;
	std::vector<ItemConfigData> signup_reward_list;
};

struct WorldArenaWinBoxRewardCfg
{
	WorldArenaWinBoxRewardCfg() : sn(0), win_num(0) {}

	int sn;
	int win_num;
	std::vector<ItemConfigData> reward_list;
};

struct WorldArenaSeasonRankRewardCfg
{
	WorldArenaSeasonRankRewardCfg() : sn(0), rank(0) {}

	int sn;
	int rank;
	std::vector<ItemConfigData> reward_list;
};

struct WorldArenaRobotCfg
{
	WorldArenaRobotCfg() : id(0), prof_base(0), level(0), attr_adjust_group(0), pet_helper_num(0) {}

	int id;
	int prof_base;
	int level;

	int attr_adjust_group;
	int pet_helper_num;
	
};

struct WorldArenaRobotAttrAdjustCfg
{
	WorldArenaRobotAttrAdjustCfg() : group_id(0), role_attack_adjust(0), role_mental_adjust(0), role_recover_adjust(0),
		role_maxhp_adjust(0), pet_attack_adjust(0), pet_mental_adjust(0), pet_recover_adjust(0),
		pet_maxhp_adjust(0), partner_attack_adjust(0), partner_mental_adjust(0), partner_recover_adjust(0),
		partner_maxhp_adjust(0) {}

	int group_id;
	int role_attack_adjust;
	int role_mental_adjust;
	int role_recover_adjust;
	int role_maxhp_adjust;
	int pet_attack_adjust;
	int pet_mental_adjust;
	int pet_recover_adjust;
	int pet_maxhp_adjust;
	int partner_attack_adjust;
	int partner_mental_adjust;
	int partner_recover_adjust;
	int partner_maxhp_adjust;
};

class WorldArenaConfig : public ILogicConfig
{
public:
	typedef std::vector<WorldArenaWinBoxRewardCfg> WinBoxRewardList;
	typedef std::vector<WorldArenaSeasonRankRewardCfg> SeasonRankRewardList;
	typedef std::pair<int, int> ProfLevelKey;
	typedef std::map<ProfLevelKey, WorldArenaRobotCfg> ProfLevelRobotMap;

	static const int MAX_ATTR_ADJUST_GROUP_NUM = 800;

public:
	WorldArenaConfig();
	~WorldArenaConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const WorldArenaOtherCfg& GetOtherCfg() const;
	const WorldArenaWinBoxRewardCfg* GetWinBoxRewardCfg(int sn);
	const WorldArenaSeasonRankRewardCfg* GetSeasonRankRewardCfg(int rank);

	const WorldArenaRobotCfg* GetRobotCfgByProfLevel(int prof_base, int level);
	const WorldArenaRobotAttrAdjustCfg* GetRobotAttrAdjustCfg(int group_id);
private:
	int InitOtherCfg(TiXmlElement *RootElement, std::string& err);
	int InitWinBoxRewardList(TiXmlElement *RootElement, std::string& err);
	int InitSeasonRankRewardList(TiXmlElement* RootElement, std::string& err);
	int InitRobotCfg(TiXmlElement* RootElement);
	int InitRobotAttrAdjustCfg(TiXmlElement* RootElement);

	WorldArenaOtherCfg m_other_cfg;
	WinBoxRewardList m_win_box_reward_list;
	SeasonRankRewardList m_season_rank_reward_list;

	ProfLevelRobotMap m_prof_level_robot_map;

	int m_robot_attr_adjust_cfg_num;
	WorldArenaRobotAttrAdjustCfg m_robot_attr_adjust_list[MAX_ATTR_ADJUST_GROUP_NUM];
};


#endif