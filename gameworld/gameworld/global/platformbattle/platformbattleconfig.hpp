#ifndef _PLATFORM_BATTLE_CONFIG_HPP_
#define _PLATFORM_BATTLE_CONFIG_HPP_

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/platformbattledef.hpp"

#include <vector>
#include <map>

struct PlatformBattleOtherCfg
{
	PlatformBattleOtherCfg()
	{
		settle_account_time = 1300;
		robot_show_level = 0;
	}

	int settle_account_time;			//每天13：00结算
	int robot_show_level;
};

struct PlatformBattleRewardCfg
{
	PlatformBattleRewardCfg()
	{
	}

	struct RewardItemCfg
	{
		int min_rank;
		int max_rank;

		int coin;
		int credits;
		std::vector<ItemConfigData> reward_item;
	}; 

	std::vector<RewardItemCfg> rank_reward;
};

struct PlatformBattleRobotLevelCfg
{
	PlatformBattleRobotLevelCfg()
	{
		min_robot_level = 0;
		max_robot_level = 0;
	}

	int min_robot_level;
	int max_robot_level;
};

struct PlatformBattlePlatformCfg
{
	PlatformBattlePlatformCfg()
	{
		type = 0;
		reward_id = 0;
		position = 0;
		platform_level = 0;
	}

	int type;			//0 :3v3擂台  ,1: 5v5擂台
	int reward_id;
	int position;
	int platform_level;

	std::vector<PlatformBattleRobotLevelCfg> robot_level_cfg;
};


struct PlatformBattleSceneCfg
{
	PlatformBattleSceneCfg()
	{
		seq = 0;
		scene_id = 0;
		enter_pos = Posi(0, 0);
	}

	int seq;
	int scene_id;
	Posi enter_pos;
};

struct PlatformBattleAchievementItemItemCfg
{
	PlatformBattleAchievementItemItemCfg()
	{
		seq = 0;
		param = 0;
		jifen_type = 0;
		jifen = 0;
	}
	
	int seq;
	int param;
	int jifen_type;
	int jifen;
	std::vector<ItemConfigData> rewards;
};

struct PlatformBattleAchievementCfg
{
	PlatformBattleAchievementCfg()
	{
		is_all_platform = false;
	}

	bool IsInPlatformList(int platform_number) const;
	const PlatformBattleAchievementItemItemCfg  *  GetPlatformBattleAchievementItemItemCfg(int seq) const;

	bool is_all_platform;
	std::set<int> platform_list;
	std::map<int, PlatformBattleAchievementItemItemCfg> item_cfg;
};

class PlatformBattleConfig : public ILogicConfig
{
public:
	PlatformBattleConfig();
	~PlatformBattleConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const PlatformBattleOtherCfg &  GetOtherCfg();

	int GetMaxAllowableNum(int platform_number);
	int GetRandRobotRoleLevel(int platform_number,int robot_level_index);
	const PlatformBattleRewardCfg::RewardItemCfg * GetRankReward(int platform_number, int rank);
	const PlatformBattleSceneCfg * GetSceneCfg(int seq);
	const PlatformBattlePlatformCfg * GetPlatformBattlePlatformCfg(int platform_number);
	const PlatformBattleAchievementCfg * GetPlatformBattleAchievementCfg(int type);
	const PlatformBattleAchievementItemItemCfg * GetPlatformBattleAchievementItemItemCfg(int type, int seq);
private:
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitPlatformCfg(TiXmlElement *RootElement);
	int InitSceneCfg(TiXmlElement *RootElement);
	int InitRobotLevelCfg(TiXmlElement * RootElement);
	int InitAchievementCfg(TiXmlElement * RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);
	 
	PlatformBattleOtherCfg m_other_cfg;
	std::map<int, PlatformBattlePlatformCfg> m_platform_cfg;
	std::map<int, PlatformBattleRewardCfg> m_reward_cfg;
	std::map<int,PlatformBattleSceneCfg> m_scene_cfg;
	std::map<int, PlatformBattleAchievementCfg> m_achievement_cfg;
};

#endif

