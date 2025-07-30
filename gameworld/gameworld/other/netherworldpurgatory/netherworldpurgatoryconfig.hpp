#ifndef _NETHER_WORLD_PURGATORY_CONFIG_HPP_
#define _NETHER_WORLD_PURGATORY_CONFIG_HPP_

#include "servercommon/servercommon.h"
#include "servercommon/netherworldgatorydef.h"
#include "servercommon/configcommon.h"

struct NetherWorldPurgatoryMonsterCfg
{
	NetherWorldPurgatoryMonsterCfg() : monster_group_id(0)
	{
		memset(condition_id, 0, sizeof(condition_id));
	}
	int monster_group_id;
	int	condition_id[MAX_NETHER_WORLD_GATORY_TASK_NUM];
	std::vector<ItemConfigData> condition1_reward;
	std::vector<ItemConfigData> condition2_reward;
	std::vector<ItemConfigData> condition3_reward;
};

struct NetherWorldPurgatoryMonsterPassRewardCfg
{
	NetherWorldPurgatoryMonsterPassRewardCfg() : chapters(0), role_level(0), challenge_id(0), unlock_chapter(0), unlock_challenge(0) {}

	int chapters;
	int role_level;
	int challenge_id;
	int	unlock_chapter;
	int	unlock_challenge;

	NetherWorldPurgatoryMonsterCfg monster_group;
	std::vector<ItemConfigData> final_reward;
};

struct NetherWorldPurgatoryRewardCfg
{
	NetherWorldPurgatoryRewardCfg() : chapters(0), lattice(0) {}
	int chapters;
	int lattice;
	std::vector<ItemConfigData> reward;
};

struct NetherWorldPurgatoryRewardListCfg
{
	NetherWorldPurgatoryRewardListCfg() {}

	NetherWorldPurgatoryRewardCfg silver_vec[MAX_NETHER_WORLD_GATORY_INDEX];
	NetherWorldPurgatoryRewardCfg gold_vec[MAX_NETHER_WORLD_GATORY_INDEX];
};

struct NetherWorldPurgatoryOtherCfg
{
	NetherWorldPurgatoryOtherCfg() : times(0), debuff_id(0), debuff_level(0) {}

	int times;
	int debuff_id;
	int debuff_level;
};

class RoleModuleManager;
class NetherWorldPurgatoryConfig : public ILogicConfig
{
public:
	NetherWorldPurgatoryConfig();
	virtual ~NetherWorldPurgatoryConfig();
	enum BOX_REWARD_TYPE
	{
		BOX_REWARD_TYPE_SILVER = 1,
		BOX_REWARD_TYPE_GOLD = 2,
		BOX_REWARD_TYPE_MAX,
	};
	virtual bool Init(const std::string &configname, std::string *err);
	const NetherWorldPurgatoryMonsterCfg * GetMonsterConditionCfg(int chapter_id, int challenge_id, int monster_group_id);
	const NetherWorldPurgatoryMonsterPassRewardCfg * PassRewardCfg(int challenge_id, int role_level);
	const NetherWorldPurgatoryRewardListCfg * GetRewardCfg(int chapter_id);
	const NetherWorldPurgatoryOtherCfg & GetOtherCfg() { return other_cfg; }
private:
	int InitMonsterPassRewardCfg(TiXmlElement * RootElement);
	int InitSilverRewardCfg(TiXmlElement * RootElement);
	int InitGoldRewardCfg(TiXmlElement * RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);

	NetherWorldPurgatoryOtherCfg other_cfg;

	NetherWorldPurgatoryMonsterPassRewardCfg * pass_reward[MAX_NETHER_WORLD_GATORY_CHAPTER_NUM][MAX_NETHER_WORLD_GATORY_INDEX];
	std::vector<NetherWorldPurgatoryMonsterPassRewardCfg> pass_reward_vec;

	NetherWorldPurgatoryRewardListCfg reward_vec[MAX_NETHER_WORLD_GATORY_CHAPTER_NUM];
};
#endif
