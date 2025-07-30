#ifndef __WORLD_BOSS_CONFIG_HPP__
#define __WORLD_BOSS_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"

#include <vector>
#include <map>

//����boss ����
struct WorldBossMonsterCfg
{
	WorldBossMonsterCfg()
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

enum WORLD_BOSS_REWARD_TYPE
{
	WORLD_BOSS_REWARD_TYPE_KILL,	 //��ɱ��
	WORLD_BOSS_REWARD_TYPE_HURT, //�����
	WORLD_BOSS_REWARD_TYPE_HEAL,	 //���ƽ�
	WORLD_BOSS_REWARD_TYPE_LUCK,	 //���˽�
	WORLD_BOSS_REWARD_TYPE_JOIN,	 //���뽱
	WORLD_BOSS_REWARD_TYPE_MAX,
};

struct WorldBossRankRewardCfg
{
	WorldBossRankRewardCfg()
	{
		min_level = 0;
		max_level = 0;
	}

	int min_level;
	int max_level;
	std::map<int, std::vector<int> > reward_list;
};

//����boss2 ����
struct WorldBoss2MonsterCfg
{
	WorldBoss2MonsterCfg()
	{
		min_open_day = 0;
		max_open_day= 0;
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

enum WORLD_BOSS_2_REWARD_TYPE
{
	WORLD_BOSS_2_REWARD_TYPE_KILL,	 //��ɱ��
	WORLD_BOSS_2_REWARD_TYPE_HURT, //�����
	WORLD_BOSS_2_REWARD_TYPE_HEAL,	 //���ƽ�
	WORLD_BOSS_2_REWARD_TYPE_LUCK,	 //���˽�
	WORLD_BOSS_2_REWARD_TYPE_JOIN,	 //���뽱
	WORLD_BOSS_2_REWARD_TYPE_MAX,
};

struct WorldBoss2RankRewardCfg
{
	WorldBoss2RankRewardCfg()
	{
		min_level = 0;
		max_level = 0;
	}

	int min_level;
	int max_level;
	std::map<int, std::vector<int> > reward_list;
};


//---------------------����boss1������boss2-------------
struct WorldBossOtherCfg
{
	WorldBossOtherCfg()
	{
		need_role_level = 0;
		limit_play_times = 0;
		need_role_num = 0;
		world_boss_heal_rank_num = 0;
		world_boss_hurt_rank_num = 0;
		world_boss_luck_rank_num = 0;
		world_boss2_heal_rank_num = 0;
		world_boss2_hurt_rank_num = 0;
		world_boss2_luck_rank_num = 0;
	}

	int need_role_level;
	int limit_play_times;
	int need_role_num;
	//---����boss--
	int world_boss_heal_rank_num;
	int world_boss_hurt_rank_num;
	int world_boss_luck_rank_num;
	//---����boss--
	int world_boss2_heal_rank_num;
	int world_boss2_hurt_rank_num;
	int world_boss2_luck_rank_num;
};

struct WorldBossRandPosCfg
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

	WorldBossRandPosCfg()
	{
	}

	std::vector<ItemCfg> group_pos;
};

struct WorldBossRankRewardGroupCfg
{
	WorldBossRankRewardGroupCfg()
	{
		bind_coin = 0;
	}

	WorldBossRankRewardGroupCfg  operator+(const WorldBossRankRewardGroupCfg & other)
	{
		WorldBossRankRewardGroupCfg temp;
		temp.bind_coin = bind_coin + other.bind_coin;
		temp.rewards = rewards;
		temp.rewards.insert(temp.rewards.end(), other.rewards.begin(), other.rewards.end());

		return temp;
	}

	int bind_coin;
	std::vector<ItemConfigData> rewards;
};

// -----------------------------------------------------------------------------------
class WorldBossConfig : public ILogicConfig
{
public:
	WorldBossConfig();
	virtual ~WorldBossConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	//--------------------------����boss����---------------------------
	const WorldBossMonsterCfg * GetWorldBossMonsterCfg(int open_day);
	WorldBossRankRewardGroupCfg GetWorldBossRankRewardCfg(int role_level, int reward_type);
	//----------------------- ����boss���� ----------------------------------
	const WorldBoss2MonsterCfg * GetWorldBoss2MonsterCfg(int open_day);
	WorldBossRankRewardGroupCfg GetWorldBoss2RankRewardCfg(int role_level, int reward_type);

	//-----------------------------------------------------------------------------------
	Posi GetRandPosByPosGroupID(int rand_pos_id);
	const WorldBossRankRewardGroupCfg * GetGroupReward(int reward_group_id);
	const WorldBossOtherCfg & GetOtherCfg();
private:
	int InitMonsterCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitMonsterCfg2(TiXmlElement *RootElement);
	int InitRewardCfg2(TiXmlElement *RootElement);

	int InitOtherCfg(TiXmlElement *RootElement);
	int InitPosCfg(TiXmlElement *RootElement);
	int InitRewardGroupCfg(TiXmlElement *RootElement);

	//--------------------------����boss����---------------------------
	std::vector<WorldBossMonsterCfg> m_monster_cfg;
	std::vector<WorldBossRankRewardCfg> m_reward_cfg;

	//----------------------- ����boss���� ----------------------------------
	std::vector<WorldBoss2MonsterCfg> m_monster_cfg2;
	std::vector<WorldBoss2RankRewardCfg> m_reward_cfg2;

	//-----------------------------------------------------------------------------------
	WorldBossOtherCfg m_other_cfg;
	std::map<int, WorldBossRandPosCfg> m_rand_pos_cfg;
	std::map<int, WorldBossRankRewardGroupCfg> m_reward_group_cfg;
};

#endif