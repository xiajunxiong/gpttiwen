#ifndef __WORLD_BOSS_3_CONFIG_HPP__
#define __WORLD_BOSS_3_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"

#include <vector>
#include <map>

static const int MAX_WORLD_BOSS_3_MONSTER_CFG_NUM = 20;
static const int MAX_WORLD_BOSS_3_REWARD_CFG_NUM = 30;
static const int MAX_WORLD_BOSS_3_REWARD_GROUP_CFG_NUM = 100;
struct WorldBoss3MonsterCfg
{
	WorldBoss3MonsterCfg()
	{
		min_open_day = 0;
		max_open_day = 0;
		scene_id = 0;
		ncp_monster_id = 0;
		coordinate_group_id = 0;
		boss_max_hp = 0;
	}

	int min_open_day;
	int max_open_day;
	int scene_id;
	int ncp_monster_id;
	int coordinate_group_id;
	int boss_max_hp;
};

enum WORLD_BOSS_3_REWARD_TYPE
{
	WORLD_BOSS_3_REWARD_TYPE_KILL,	//击杀奖
	WORLD_BOSS_3_REWARD_TYPE_HURT,	//输出奖
	WORLD_BOSS_3_REWARD_TYPE_HEAL,	//治疗奖
	WORLD_BOSS_3_REWARD_TYPE_LUCK,	//幸运奖
	WORLD_BOSS_3_REWARD_TYPE_JOIN,	//参与奖
	WORLD_BOSS_3_REWARD_TYPE_MAX,
};

struct WorldBoss3RankRewardCfg
{
	WorldBoss3RankRewardCfg()
	{
		min_level = 0;
		max_level = 0;
	}

	int min_level;
	int max_level;
	std::map<int, std::vector<int> > reward_list;
};

struct WorldBoss3OtherCfg
{
	WorldBoss3OtherCfg()
	{
		need_role_level = 0;
		limit_play_times = 0;
		need_role_num = 0;
		world_boss_3_heal_rank_num = 0;
		world_boss_3_hurt_rank_num = 0;
		world_boss_3_luck_rank_num = 0;
		need_world_level = 0;
	}

	int need_role_level;
	int limit_play_times;
	int need_role_num;
	int world_boss_3_heal_rank_num;
	int world_boss_3_hurt_rank_num;
	int world_boss_3_luck_rank_num;
	int need_world_level;	//活动开启所需世界等级
};

struct WorldBoss3RandPosCfg
{
	const static int MAX_RATE_COUNT = 10000;

	struct ItemCfg
	{
		ItemCfg() :rate(0)
		{
		}

		int rate;
		Posi pos;
	};

	WorldBoss3RandPosCfg()
	{
	}

	std::vector<ItemCfg> group_pos;
};

struct WorldBoss3RankRewardGroupCfg
{
	WorldBoss3RankRewardGroupCfg()
	{
		bind_coin = 0;
	}

	WorldBoss3RankRewardGroupCfg  operator+(const WorldBoss3RankRewardGroupCfg & other)
	{
		WorldBoss3RankRewardGroupCfg temp;
		temp.bind_coin = bind_coin + other.bind_coin;
		temp.rewards = rewards;
		temp.rewards.insert(temp.rewards.end(), other.rewards.begin(), other.rewards.end());

		return temp;
	}

	int bind_coin;
	std::vector<ItemConfigData> rewards;
};

// -----------------------------------天神降临------------------------------------------------
class WorldBoss3Config : public ILogicConfig
{
public:
	WorldBoss3Config();
	virtual ~WorldBoss3Config();

	virtual bool Init(const std::string &configname, std::string *err);

	const WorldBoss3MonsterCfg * GetWorldBossMonsterCfg(int open_day);
	WorldBoss3RankRewardGroupCfg GetWorldBossRankRewardCfg(int role_level, int reward_type);

	//-----------------------------------------------------------------------------------
	Posi GetRandPosByPosGroupID(int rand_pos_id);
	const WorldBoss3RankRewardGroupCfg * GetGroupReward(int reward_group_id);
	const WorldBoss3OtherCfg & GetOtherCfg();
private:
	int InitMonsterCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);

	int InitOtherCfg(TiXmlElement *RootElement);
	int InitPosCfg(TiXmlElement *RootElement);
	int InitRewardGroupCfg(TiXmlElement *RootElement);
	
	int m_monster_count;
	WorldBoss3MonsterCfg m_monster_list[MAX_WORLD_BOSS_3_MONSTER_CFG_NUM];
	int m_reward_count;
	WorldBoss3RankRewardCfg m_reward_list[MAX_WORLD_BOSS_3_REWARD_CFG_NUM];
	
	//-----------------------------------------------------------------------------------
	WorldBoss3OtherCfg m_other_cfg;
	std::map<int, WorldBoss3RandPosCfg> m_rand_pos_cfg;
	int m_reward_group_count;
	WorldBoss3RankRewardGroupCfg m_reward_group_list[MAX_WORLD_BOSS_3_REWARD_GROUP_CFG_NUM];
};

#endif