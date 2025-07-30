#ifndef __ROLE_CRYSTAL_FB_CONFIG_HPP__
#define __ROLE_CRYSTAL_FB_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include <vector>
#include <map>
#include <set>
#include "gameworld/obj/character/npc_chosed.h"


const static int   MAX_ROLE_CRYSTAL_FB_REWARD_COLUMN = 3;

enum ROLE_CRYSTAL_FB_TYPE
{
	ROLE_CRYSTAL_FB_TYPE_NORMAL = 0,		//普通(玉虚镜)
	ROLE_CRYSTAL_FB_TYPE_XI_YOU = 1,		//稀有(精英试炼)
	ROLE_CRYSTAL_FB_TYPE_JING_YING = 2,		//精英(上古遗迹)
	ROLE_CRYSTAL_FB_TYPE_SPECIAL = 3,		//特殊

	ROLE_CRYSTAL_FB_TYPE_MAX,
};

struct RoleCrystalFbLevelCfg
{
	RoleCrystalFbLevelCfg()
	{
		seq = 0;
		type = 0;
		fb_index = 0;
		limit_times = 0;
		need_role_level = 0;
		comsume_item_id = 0;
		comsume_item_num = 1;
		recommend_score = 0;
		boss_npc_id = 0;
		monster_dir = 0;
		last_layer_scene_id = 0;
		boss_layer_sceneborn_group_id = 0;
		box_group_id = 0;
		box_num_group_id = 0;
		monster_id_group_id = 0;
		monster_num_group_id = 0;
		use_item_limit_group_id = 0;
		event_num = 0;
		event_group_id = 0;
		formula_drop_group = 0;
		first_poker_reward_group_id = 0;
		recover_npc_id = 0;
		recover_npc_pos = Posi(0, 0);
		recover_npc_dir = 0;
		is_fly_up_fb = false;
	}

	struct LayerCfg
	{
		LayerCfg()
		{
			scene_id_group = 0;
		}

		int scene_id_group;
	};

	const LayerCfg * GetLayerCfg(int layer) const
	{
		int index = layer - 1;

		if (index < 0 || index >= (int)layer_cfg.size())
		{
			return NULL;
		}

		return &layer_cfg[index];
	}

	int seq;
	int type;
	int fb_index;
	int limit_times;
	int need_role_level;
	int comsume_item_id;
	int comsume_item_num;
	int recommend_score;
	int boss_layer_sceneborn_group_id;
	int boss_npc_id;
	Posi boss_pos;
	Dir monster_dir;
	int last_layer_scene_id;
	int box_group_id;
	int box_num_group_id;
	int monster_id_group_id;
	int monster_num_group_id;
	int use_item_limit_group_id;
	int event_num;
	int event_group_id;
	int formula_drop_group;
	int first_poker_reward_group_id;
	std::vector<int> drop_list_vec;
	std::vector<int> poker_reward_group_vec;

	std::vector<LayerCfg> layer_cfg;

	std::vector<ItemConfigData> first_kill_reward_vec;

	int recover_npc_id;
	Posi recover_npc_pos;
	Dir recover_npc_dir;
	bool is_fly_up_fb;
};

struct CrystalFbBoxCfg
{
	const static int MAX_RATE_COUNT = 10000;

	struct ItemCfg
	{
		ItemCfg() :treasure_chest_posi(0, 0)
		{
			rate = 0;
			seq = 0;
			open_need_item_id = 0;
			open_need_item_num = 0;
			treasure_chest_id = 0;
			encounter_monster_group_id = 0;
		}

		int rate;
		int seq;
		int open_need_item_id;
		int open_need_item_num;
		Posi treasure_chest_posi;
		int treasure_chest_id;
		int encounter_monster_group_id;
		std::vector <int> add_coin;
		std::vector<int> captain_coin;
		std::vector<std::vector<int> > reward_column;
		std::vector<std::vector<int> > captain_reward_column;
	};

	CrystalFbBoxCfg()
	{
	}

	std::vector<ItemCfg> group_cfg;
};

struct CrystalFbOtherCfg
{
	CrystalFbOtherCfg()
	{
		all_level_limit_times = 0;
		initial_power = 0;
		use_power = 0;
		max_power = 0;
		recovery_power = 0;
		recovery_time = 0;
		extra_power_limit = 0;
		limit_type = 0;
		buy_times = 0;
		normal_fb_day_pass_time_max = 0;

		pass_time_max = 0;
		normal_reward_time = 0;
		free_reward_time = 0;
		extra_buy_times = 0;
		extra_buy_consume = 0;
	}

	int all_level_limit_times;
	int initial_power;
	int use_power;
	int max_power;
	int recovery_power;
	int recovery_time;
	int extra_power_limit;
	int limit_type;
	int buy_times;
	int normal_fb_day_pass_time_max;

	int pass_time_max;
	int	normal_reward_time;
	int	free_reward_time;
	int	extra_buy_times;
	int	extra_buy_consume;
	std::vector<int> item_notice_vec;
};

struct CrystalFbRewardCfg
{
	const static int MAX_RATE_COUNT = 10000;

	struct ItemCfg
	{
		ItemCfg() :rate(0)
		{
		}

		int rate;
		ItemConfigData reward;
	};

	CrystalFbRewardCfg()
	{
	}

	std::vector<ItemCfg> group_cfg;
};

struct CrystalFbPosCfg
{
	const static int MAX_RATE_COUNT = 10000;

	struct ItemCfg
	{
		ItemCfg()
		{
			rate = 0;
			birth_pos = Posi(0, 0);
			transport_pos = Posi(0, 0);
		}

		int rate;
		Posi birth_pos;
		Posi transport_pos;
	};

	CrystalFbPosCfg()
	{
	}

	std::vector<ItemCfg> group_cfg;
};

struct CrystalFbSceneCfg
{
	const static int MAX_RATE_COUNT = 10000;

	struct ItemCfg
	{
		ItemCfg()
		{
			rate = 0;
			scene_id = 0;
			sceneborn_group_id = 0;
			birth_pos = Posi(0, 0);
			transport_pos = Posi(0, 0);
		}

		int rate;
		int scene_id;
		int sceneborn_group_id;
		Posi birth_pos;
		Posi transport_pos;
	};

	CrystalFbSceneCfg()
	{
	}

	std::vector<ItemCfg> group_cfg;
};

struct CrystalFbUseItemLimitCfg
{
	CrystalFbUseItemLimitCfg()
	{
	}

	std::map<int, int> use_type_to_limit_times;
};

struct CrystalFbRandNumCfg
{
	const static int MAX_RATE_COUNT = 10000;

	struct ItemCfg
	{
		ItemCfg() :rate(0)
		{
			box_num = 0;
		}

		int rate;
		int box_num;
	};

	CrystalFbRandNumCfg()
	{
	}

	std::vector<ItemCfg> group_cfg;
};

struct CrystalFbMonsterCfg
{
	const static int MAX_RATE_COUNT = 10000;

	struct ItemCfg
	{
		ItemCfg() :rate(0)
		{
			monster_npc_id = 0;
		}

		int rate;
		int monster_npc_id;
	};

	CrystalFbMonsterCfg()
	{
	}

	std::vector<ItemCfg> group_cfg;
};

struct CrystalFbMonsterDropCfg
{
	CrystalFbMonsterDropCfg():monster_npc_id(0),village_id(0),prestige_num(0), exp(0)
	{
	}
	int monster_npc_id;
	int village_id;
	int prestige_num;
	std::vector<std::vector<int> > drop_id_list_group;

	int exp;
	std::vector<ItemConfigData> reward_item;
};

struct CrystalFbNPCRewardCfg
{
	enum REWARD_TYPE
	{
		REWARD_TYPE_0 = 1,//buff
		REWARD_TYPE_1 = 2,//奖励id
		REWARD_TYPE_2 = 3,//商店id
		REWARD_TYPE_3 = 4,//金钱奖励
		REWARD_TYPE_4 = 5,//交流
		REWARD_TYPE_MAX,
	};
	CrystalFbNPCRewardCfg():npc_id(0), option_index(0), reward_type(0), is_delete(0)
	{
	}

	int npc_id;
	int option_index;
	int reward_type;
	int is_delete;
	int reward_list[MAX_ROLE_CRYSTAL_FB_REWARD_COLUMN];
};

struct CrystalFbNPCRewardListCfg
{
	const static int MAX_OPERATION_INDEX = 2;//最大选择个数
	CrystalFbNPCRewardListCfg()
	{
	}
	CrystalFbNPCRewardCfg reward_list[MAX_OPERATION_INDEX];
};

struct Event_NPC_cfg
{
	enum CRYSTALFB_NPC_TYPE
	{
		CRYSTALFB_NPC_TYPE_0 = 0,		//!< 未知物品
		CRYSTALFB_NPC_TYPE_1 = 1,		//!< 怪物
		CRYSTALFB_NPC_TYPE_2 = 2,		//!< npc
		CRYSTALFB_NPC_TYPE_3 = 3,		//!< Boss
		CRYSTALFB_NPC_TYPE_4 = 4,		//!< 篝火
		CRYSTALFB_NPC_TYPE_5 = 5,		//!< 铜币
		CRYSTALFB_NPC_TYPE_6 = 6,		//!< 采集物 - 木材
		CRYSTALFB_NPC_TYPE_7 = 7,		//!< 采集物 - 矿石
		CRYSTALFB_NPC_TYPE_8 = 8,		//!< 采集物 - 草
		CRYSTALFB_NPC_TYPE_9 = 9,		//!< 宝箱
		CRYSTALFB_NPC_TYPE_10 = 10,		//!< 传说门
	};
	Event_NPC_cfg(): event_group_id(0), npc_id(0), rate(0), npc_type(0){}
	int event_group_id;
	int npc_id;
	int rate;
	int npc_type;
};

struct NpcShopConfig
{
	enum MONEY_TYPE
	{
		MONEY_TYPE_0 = 0,//元宝
		MONEY_TYPE_1 = 1,//仙币
		MONEY_TYPE_2 = 2,//铜钱
	};
	NpcShopConfig():shop_type(0),index(0),money_type(0),price(0), buy_times(0), rate(0), discount(0){}
	int shop_type;
	int index;
	int money_type;
	int price;
	int buy_times;
	int rate;
	int discount;
	ItemConfigData commodity;
};

struct BuffListConfig
{
	enum REWARDBUFTYPE
	{
		REWARDBUFTYPE_0 = 0,
		REWARDBUFTYPE_1 = 1,//每回合对地方造成固定伤害  param1 伤害值
		REWARDBUFTYPE_2 = 2,//回复魔力					parma1 百分比
		REWARDBUFTYPE_3 = 3,//回复生命值				param1 百分比
		REWARDBUFTYPE_4 = 4,//增加固定属性				param1 属性类型 param2 属性值
		REWARDBUFTYPE_5 = 5,//增加百分比属性			param1 属性类型 param2 百分比
		REWARDBUFTYPE_6 = 6,//全员回复魔力与生命值		param1 百分比
	};
	BuffListConfig() :buff_id(0), type(0), param_1(0), param_2(0) {}
	int buff_id;
	int type;
	int param_1;
	int param_2;
};

struct GatherNpcConfig
{
	enum REWARD_TYPE
	{
		REWARD_TYPE_0 = 1,//buff
		REWARD_TYPE_1 = 2,//奖励id
		REWARD_TYPE_2 = 3,//铜币
		REWARD_TYPE_MAX,
	};
	GatherNpcConfig() :npc_id(0), reward_type(0){}
	int npc_id;
	int reward_type;
	int reward_list[MAX_ROLE_CRYSTAL_FB_REWARD_COLUMN];
};
const static int MAX_ROLE_CRYSTALFB_BUY_ITEM_COUNT = 20;//npc商人商店物品个数

struct WorkshopItemConfig
{
	const static int MAX_RAND_WEIGHT_NUM = 10000;//总权重
	WorkshopItemConfig() :group_id(0), item_id(0), rate(0){}
	int group_id;
	int item_id;
	int rate;
};

struct BuyEnergyCfg
{
	BuyEnergyCfg()
	{}

	int price;			//元宝价格
	int get_power;		//体力数量
};

struct PowerUpCfg
{
	PowerUpCfg() :job_num(0), power_up(0) {}
	int job_num;
	int power_up;
};

class RoleCrystalFbConfig : public ILogicConfig
{
public:
	RoleCrystalFbConfig();
	virtual ~RoleCrystalFbConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const RoleCrystalFbLevelCfg * GetRoleCrystalFbLevelCfg(int seq);
	int GetLayerNum(int seq);
	bool IsCrystalFbBoxGather(int gather_id);
	const CrystalFbBoxCfg::ItemCfg * GetBoxCfg(int group_id, int box_seq);
//	int GetRandScene(int scene_group_id, std::set<int>  exclude_scene_id = std::set<int>());
	const CrystalFbSceneCfg::ItemCfg * GetRandSceneCfg(int scene_group_id, std::set<int>  exclude_scene_id = std::set<int>());
	Posi GetRandPosi(int pos_group_id);

	const CrystalFbBoxCfg::ItemCfg * GetRandBoxCfg(int rand_group_id);
	std::vector<ItemConfigData>   GetRandBoxReward(std::vector<int> reward_group_ids);

	const ItemConfigData * GetRandNpcReward(int reward_group_ids);
	
	int GetMaxRewardNum(int reward_group_id);
	int GetRandNum(int num_group_id);
	const CrystalFbOtherCfg & GetCrystalFbOtherCfg();
	int GetUseItemLimitTimes(int use_limit_group_id, int use_type);
	int IsLimitTimesUseType(int use_limit_group_id, int use_type);
	const CrystalFbMonsterCfg::ItemCfg * GetCrystalFbMonsterCfg(int monster_group_id);
	std::vector<int>  GetMonsterDropId(int monster_npc_id, int play_times = 0,int * village_id = NULL,int * prestige_num = NULL, int *add_exp = NULL, std::vector<ItemConfigData> * reward_list = NULL);
	int GetRewardColumn(int play_times);
	int GetReawrdAttColumn(int ARG_OUT * out_play_times = NULL);
	const CrystalFbNPCRewardCfg * GetNpcRewardCfg(int npc_id,int option_index);
	const CrystalFbNPCRewardListCfg * GetNpcEventCfg(int npc_id);
	int GetEventNpcCfg(int group_id,int rand_num,std::vector<Event_NPC_cfg>*get_npc_list, std::vector<Event_NPC_cfg>*has_create_npc_vec);
	const BuffListConfig* GetBuffId(int buff_id);
	int RandNPCShopItemList(int npc_id,NpcShopItem but_list[MAX_ROLE_CRYSTALFB_BUY_ITEM_COUNT]);
	const GatherNpcConfig * GetGatherNpcCfg(int npc_id);

	const int GetWorkshopItemWeightCfg(int group_id);
	const std::vector<WorkshopItemConfig> * GetWorkshopItemCfg(int group_id);
	const BuyEnergyCfg * GetBuyEnergyCfg(int buy_times);

	int GetExpCfg(int role_level);
	const bool IsItemNotice(int item_id);
	const PowerUpCfg* GetPowerUpCfg(int adv_level);

	int GetFindExpCfg(int role_level);
private:
	int InitLevelCfg(TiXmlElement *RootElement);
	int InitExpCfg(TiXmlElement *RootElement);
	int InitBoxCfg(TiXmlElement *RootElement);
	int InitRandNumCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitPosCfg(TiXmlElement *RootElement);
	int InitSceneCfg(TiXmlElement *RootElement);	
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitUseLimitCfg(TiXmlElement *RootElement);
	int InitMonsterCfg(TiXmlElement *RootElement);
	int InitMonsterDropCfg(TiXmlElement *RootElement);
	int InitPlayTimesToRewardColumnCfg(TiXmlElement *RootElement);
	int InitEventGroupCfg(TiXmlElement *RootElement);
	int InitEventListCfg(TiXmlElement *RootElement);
	int InitShopListCfg(TiXmlElement *RootElement);
	int InitBuffListCfg(TiXmlElement *RootElement);
	int InitGatherNpcCfg(TiXmlElement *RootElement);
	int InitWrokShopItemCfg(TiXmlElement *RootElement);
	int InitBuyEnergyCfg(TiXmlElement *RootElement);
	int InitPowerUpCfg(TiXmlElement *RootElement);
	int InitEnergyFindCfg(TiXmlElement *RootElement);

	//-----------------------
	void RefreshPos(CrystalFbSceneCfg::ItemCfg ARG_OUT & cfg);

	std::vector<RoleCrystalFbLevelCfg> m_level_cfg;
	int m_exp_cfg[MAX_ROLE_LEVEL];							//经验配置 对应等级配置 *　所消耗体力
	//std::map<int, int> m_exp_cfg;							//经验配置 key:min_level * 1000 + max_level
	std::map<int,CrystalFbBoxCfg> m_box_cfg;
	std::map<int,CrystalFbRandNumCfg> m_rand_num_cfg;
	std::map<int,CrystalFbRewardCfg> m_rewards_cfg;
	std::map<int,CrystalFbSceneCfg> m_scene_cfg;
	std::map<int, CrystalFbPosCfg> m_pos_cfg;
	std::map<int, CrystalFbUseItemLimitCfg> m_use_item_limit_cfg;
	std::map<int, CrystalFbMonsterCfg> m_monster_cfg;
	std::set<int> m_all_box_gather_id;
	CrystalFbOtherCfg m_other_cfg;

	std::map<int, int> m_play_times_to_reward_column;
	std::map<int, CrystalFbMonsterDropCfg> m_monster_drop_cfg;

	std::map<int,int> m_event_npc_cfg_weight_map;//npc对应总权重
	std::map<int, std::vector<Event_NPC_cfg> > m_event_npc_cfg_map;
	std::map<int, CrystalFbNPCRewardListCfg> m_npc_reward_cfg_map;
	std::map<int, BuffListConfig> m_buff_id_list_map;
	std::map<int, std::vector<NpcShopConfig> > m_npc_shop_list_map;
	std::map<int, GatherNpcConfig> m_gather_npc_cfg_map;

	std::map<int, int> m_workshop_wight_map;
	std::map<int, std::vector<WorkshopItemConfig> > m_workshop_item_cfg_map;

	std::map<int, BuyEnergyCfg> m_buy_energy_cfg;

	PowerUpCfg * power_up_list[PROFESSION_BASE];
	std::vector<PowerUpCfg> m_power_up_cfg;

	std::map<int, int> m_find_energy;	// key-玩家等级，val-每点体力可找回经验
};

#endif