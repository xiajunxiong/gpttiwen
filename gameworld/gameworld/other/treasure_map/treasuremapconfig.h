#ifndef _TREASURE_MAP_CONFIG_HPP_
#define _TREASURE_MAP_CONFIG_HPP_
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"

#include <vector>
#include <map>

struct TreasureMapOtherCfg
{
	TreasureMapOtherCfg() : limit_get_reward_times(0), max_play_times(0), high_map_id(0)
	{}

	int limit_get_reward_times;
	int max_play_times;					//保底次数
	ItemID high_map_id;	// 高级藏宝图(隐)id
};

struct TreasureMapTaskCfg
{
	const static int MAX_RATE_COUNT = 10000;

	TreasureMapTaskCfg()
	{
		task_id = 0;
		min_level = 0;
		max_level = 0;
		scene_group_id = 0;
		monster_group_id = 0;
		reward_rate = 5000;
		exp = 0;
		coin = 0;

	}

	int task_id;
	int min_level;
	int max_level;
	int scene_group_id;
	int monster_group_id;
	int reward_rate;
	std::vector<ItemConfigData> reward_list;
	int exp;
	int coin;
};

struct TreasureMapSceneCfg
{
	const static int MAX_RATE_COUNT = 10000;

	struct ItemCfg
	{
		ItemCfg() :rate(0), scene_id(0), pos_group_id(0) {}

		int rate;
		int scene_id;
		int pos_group_id;
	};

	TreasureMapSceneCfg() {}

	std::vector<ItemCfg> group_item_cfg;
};

struct TreasureMapMonsterCfg
{
	const static int MAX_RATE_COUNT = 10000;

	struct ItemCfg
	{
		ItemCfg() :rate(0), monster_id(0) {}

		int rate;
		int monster_id;
	};

	TreasureMapMonsterCfg() {}

	std::vector<ItemCfg> group_item_cfg;
};


enum TREASURE_MAP_ITEM_TYPE
{
	TREASURE_MAP_ITEM_TYPE_MONSTER = 0,			// 怪物
	TREASURE_MAP_ITEM_TYPE_COIN = 1,			// 铜币
	TREASURE_MAP_ITEM_TYPE_ITEM = 2,			// 物品
	TREASURE_MAP_ITEM_TYPE_RAND_ITEM = 3,		// 随机物品

	TREASURE_MAP_ITEM_TYPE_MAX,
};

enum TREASURE_MAP_ITEM_RANK_TYPE
{
	TREASURE_MAP_ITEM_RANK_TYPE_HIGH = 1,			// 高级藏宝图
	TREASURE_MAP_ITEM_RANK_TYPE_LOW = 2,			// 低级藏宝图
	TREASURE_MAP_ITEM_RANK_TYPE_STAR = 3,			// 星辰藏宝图
	TREASURE_MAP_ITEM_RANK_TYPE_HALLOW = 4,			// 圣器藏宝图

	TREASURE_MAP_ITEM_RANK_TYPE_MAX,
};

struct TreasureMapItemCfg			// 藏宝图配置
{
	const static int MAX_RATE_COUNT = 10000;

	TreasureMapItemCfg() {}

	struct ItemCfg
	{
		ItemCfg()
		{
			rate = 0;
			seq = 0;
			item_type = 0;
			monster_id = 0;
			coin = 0;
			reward_grounp_id = 0;
			map_type = 0;
			is_notice = 0;
		}

		int rate;
		int seq;
		int index;
		int item_type;
		int monster_id;
		int coin;
		int reward_grounp_id;
		int map_type;
		int is_notice;				// 0: 不公告 1:发布公告1 2:发布公告2
		ItemConfigData item;
	};

	struct RoleLevelItemCfg
	{
		RoleLevelItemCfg()
		{
			min_role_level = 0;
			max_role_level = 0;
		}

		int min_role_level;
		int max_role_level;

		std::vector<ItemCfg> group_item_cfg;
	};

	std::vector<RoleLevelItemCfg> role_level_item_cfg;
};

struct TreasureMapRewardGroupCfg			//藏宝图奖励组
{
	const static int MAX_RATE_COUNT = 10000;

	TreasureMapRewardGroupCfg() {}

	struct ItemCfg
	{
		ItemCfg() : rate(0) {}

		int rate;
		ItemConfigData item;
	};

	std::vector<ItemCfg> group_item_cfg;
};

struct TreasureMapBackItemsCon
{
	TreasureMapBackItemsCon() : prob(0) {}

	int prob;
	ItemConfigData item;
};

struct TreasureMapAwardBackCon
{
	TreasureMapAwardBackCon() : experience(0), coin_bind(0), family_contribution(0), back_item_group_id(0), find_type(0){}

	int experience;
	int coin_bind;
	int family_contribution;
	int back_item_group_id;
	int find_type;
	int seq;
};

struct TreasureMapObtainCfg
{
	TreasureMapObtainCfg():map_type(0), min_level(0), max_level(0)
	{}

	int map_type;
	ItemConfigData item;
	int min_level;
	int max_level;
};

struct TreasureMapCoordinateGroupCfg
{
	const static int MAX_RATE_COUNT = 10000;

	struct ItemCfg
	{
		ItemCfg() :rate(0) { pos = Posi(0, 0); }

		Posi pos;
		int rate;
	};

	TreasureMapCoordinateGroupCfg()
	{}

	std::vector<ItemCfg> group_item_cfg;
};


class TreasureMapConfig : public ILogicConfig
{
public:
	TreasureMapConfig();
	~TreasureMapConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const TreasureMapTaskCfg  * GetTreasureMapTaskCfg(int task_id);
	const TreasureMapSceneCfg  * GetTreasureMapSceneCfg(int scene_group_id, int seq);
	const TreasureMapMonsterCfg * GetTreasureMapMonsterCfg(int monster_group_id, int seq);
	const TreasureMapOtherCfg & GetOtherCfg();
	const TreasureMapItemCfg::ItemCfg * GetRandItemCfg(int seq, int role_level);
	const ItemConfigData * GetRandReward(int reward_grounp_id);
	const TreasureMapAwardBackCon * GetRewardBack(int role_level, int back_type);
	const TreasureMapAwardBackCon* GetFindType(int _role_level);
	const ItemConfigData * RandBackItem(int group_id);
	const ItemConfigData * GetMapItem(int map_type, int level);
	const TreasureMapItemCfg::ItemCfg * GetItemDetailCfg(int seq, int role_level, int index);

	void GetRandMonsterAndScene(int role_level, int ARG_OUT  * task_id, int ARG_OUT  * scene_id, int ARG_OUT * monster_id, Posi ARG_OUT * pos);
	int GetRandScene(int scene_group_id, Posi ARG_OUT * pos, const char * reason = NULL);
	int GetRandMonster(int monster_group_id);
	Posi GetRandPos(int pos_group_id, const char * reason = NULL);
private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitTaskCfg(TiXmlElement *RootElement);
	int InitSceneCfg(TiXmlElement *RootElement);
	int InitMonsterCfg(TiXmlElement *RootElement);
	int InitItemCfg(TiXmlElement *RootElement);
	int InitRewardGroupCfg(TiXmlElement *RootElement);
	int InitBackItemsCfg(TiXmlElement *RootElement);
	int InitAwardBackCfg(TiXmlElement *RootElement);
	int InitMapObtainCfg(TiXmlElement *RootElement);
	int InitCoordinateGroupCfg(TiXmlElement *RootElement);

	TreasureMapOtherCfg m_other_cfg;
	std::map<int, TreasureMapTaskCfg> m_task_cfg;
	std::map<int, TreasureMapSceneCfg> m_rand_scene_cfg;
	std::map<int, TreasureMapMonsterCfg> m_rand_monster_cfg;
	std::map<int, TreasureMapItemCfg> m_item_cfg;
	std::map<int, TreasureMapRewardGroupCfg> m_reward_group_cfg;

	// 奖励找回
	std::map<int, std::vector<TreasureMapBackItemsCon> > m_tmt_back_item_map;
	std::map<ParamSection, std::map<int, TreasureMapAwardBackCon> > m_tmt_find_back_map;
	std::map<ParamSection, TreasureMapAwardBackCon> m_ps_to_tmb;

	//宝图获取,消耗未使用的宝图获取有净值的宝图
	std::vector<TreasureMapObtainCfg> m_map_obtain_cfg;

	std::map<int, TreasureMapCoordinateGroupCfg> m_coordinate_group_cfg;		//场景坐标组

};

#endif 