#ifndef _WILD_BOSS_CONFIG_HPP_
#define _WILD_BOSS_CONFIG_HPP_

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"

#include <vector>
#include <map>
#include <set>

struct WildBossItemCfg
{
static const int MAX_WILD_BOSS_RATE_COUNT = 10000;
	WildBossItemCfg()
	{
		seq = 0;
		scene_id = 0;
		level_group_id = 0;
		ordinary_number = 0;
		wild_number = 0;
		wild_rate = 0;
		refresh_time_group_id = 0;
	}

	int seq;
	int scene_id;
	int level_group_id;
	int ordinary_number;			//普通BOSS编号
	int wild_number;				//狂暴BOSS编号
	int wild_rate;					//狂暴概率
	int refresh_time_group_id;		// 刷新时间组
};

struct WildBossMonsterCfg
{
	WildBossMonsterCfg() : npc_monster_id(0), poker_reward(0), dec_poker_reward(0), alive_time(0)
	{}

	int npc_monster_id;
	std::vector<int >  reward_id_vec;			// 正常奖励
	std::vector<int >  dec_reward_id_vec;		// 超过次数的奖励
	int poker_reward;							// 翻牌奖励
	int dec_poker_reward;						// 超过次数的翻牌奖励
	int alive_time;								// 存活多久
};

struct WildBossOtherCfg
{
	WildBossOtherCfg()
	{
		get_reward_times = 0;
		check_item_boss_notice_list.clear();
	}

	int get_reward_times;
	std::set<int> check_item_boss_notice_list;	//翻牌奖励中检测到对应道具则发公告
};

struct WildBossRewardCfg
{
	const static int MAX_RATE_COUNT = 10000;

	struct ItemCfg
	{
		ItemCfg() :rate(0), coin(0), luck_coin(0)
		{
		}

		int rate;
		int coin;
		int luck_coin;
		ItemConfigData reward;
	};

	WildBossRewardCfg()
	{
	}

	std::vector<ItemCfg> group_reward;
};

struct WildBossPosCfg
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

	WildBossPosCfg()
	{
	}

	std::vector<ItemCfg> group_cfg;
};

struct WildBossRefresgTimeCfg
{
	const static int MAX_RATE_COUNT = 10000;

	struct ItemCfg
	{
		ItemCfg() :rate(0)
		{

		}

		int rate;
		int refresh_interval;
	};

	WildBossRefresgTimeCfg()
	{
	}

	std::vector<ItemCfg> group_cfg;
};

class WildBossConfig : public ILogicConfig
{
public:
	WildBossConfig();
	~WildBossConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const std::vector<WildBossItemCfg> & GetAllWildBossItemCfg();
	const WildBossItemCfg * GetWildBossItemCfg(int seq);
	const WildBossOtherCfg & GetOtherCfg();
	bool IsItemNotice(int item_id);
	const WildBossMonsterCfg * GetMonsterCfg(int npc_number);
	const WildBossMonsterCfg * GetMonsterCfgByNpcId(int npc_monster_id);

	void GetRandRewardItemByRewardIdList(std::vector <int > reward_id, int *coin, int* luck_coin, std::vector<ItemConfigData> &out_reward);
	void GetRandRewardItem(int scene_id, int monster_npc_id, int *coin, int* luck_coin, std::vector<ItemConfigData> &out_reward, bool use_dec_reward = false);
	void GetPokerRewardItem(int scene_id, int monster_npc_id, int *coin, int* luck_coin, std::vector<ItemConfigData> &out_reward, bool use_dec_reward = false);
	void GetPokerRewardItemByRewardId(int &reward_id, int *coin, int *luck_coin, std::vector<ItemConfigData> &out_reward);

	int GetMonsterIdBySceneId(int scene_id);
	void GetRandReward(int reward_id, std::vector<ItemConfigData> & out_list, int *coin, int *luck_coin);
	Posi GetRandPosi(int rand_pos_group_id);
	int GetRefreshInterval(int group_id);

	int GetLevelGroupCfg(int level_group_id);
private:
	int InitWildBossCfg(TiXmlElement *RootElement, std::string & errormsg);
	int InitMonsterCfg(TiXmlElement *RootElement, std::string & errormsg);
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitPosCfg(TiXmlElement *RootElement);
	int InitRefreshTimeCfg(TiXmlElement *RootElement);
	int InitLevelGroupCfg(TiXmlElement* RootElement);

	std::vector<WildBossItemCfg> m_boss_item_cfg;
	std::map<int, WildBossMonsterCfg> m_monster_list;			//怪物列表 key:npc_number
	WildBossOtherCfg m_other_cfg;
	std::map<int, WildBossRewardCfg > m_rewards_cfg;
	std::map<int, WildBossPosCfg > m_pos_cfg;
	std::map<int, WildBossRefresgTimeCfg > m_refresh_time_cfg;
	std::map<int, int> m_level_group_map;				//等级组 key:level_group_id second:1000 * level_min + level_max

	int m_scens_to_npc[65535];	// 场景id  --> 是否是NPCid所在场景 0 or 1
};

#endif
