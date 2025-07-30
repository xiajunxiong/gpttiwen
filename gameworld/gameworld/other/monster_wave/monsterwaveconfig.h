#ifndef _MONSTER_WAVE_CONFIG_HPP_
#define _MONSTER_WAVE_CONFIG_HPP_

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"

#include <map>
#include<vector>

struct MonsterWaveLevelCfg
{
	struct ItemCfg
	{
		ItemCfg():min_level(0), max_Level(0)
		{
		}

		int min_level;
		int max_Level;
		std::vector<int> wave_reward;
	};

	MonsterWaveLevelCfg()
	{
		monster_group_id = 0;
	}

	int need_role_level;
	int monster_group_id;

	std::map<int, std::vector<ItemCfg> > all_wave_reward;  //以当前波数为key
};

struct MonsterWaveRewardCfg
{
	const static int MAX_RATE_COUNT = 10000;

	struct ItemCfg
	{
		ItemCfg():rate(0)
		{			
		}

		int rate;
		ItemConfigData reward;
	};

	MonsterWaveRewardCfg()
	{
	}

	std::vector<ItemCfg> group_reward;
};

struct MonsterWaveOtherCfg
{
	MonsterWaveOtherCfg() : max_round_limit(0), reserve_int(0) {}

	int max_round_limit;
	int reserve_int;
};

class MonsterWaveConfig : public ILogicConfig
{
public:
	MonsterWaveConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const MonsterWaveLevelCfg * GetLevelCfg(int fb_level);
	bool CanPlay(int role_level, int & fb_level);
	void GetRandReward(int reward_id, std::vector<ItemConfigData> & out_list);
	inline const MonsterWaveOtherCfg& GetOtherCfg() const { return m_other_cfg; }
private:
	int InitLevelSceneCfg(TiXmlElement *RootElement);
	int InitGroupReward(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	std::map<int, MonsterWaveLevelCfg > m_level_cfg;			//以万妖谷所需等级为key
	std::map<int, MonsterWaveRewardCfg > m_rewards_cfg;

	MonsterWaveOtherCfg m_other_cfg;
};



#endif // _WUSHUANGDIANCONFIG_HPP_