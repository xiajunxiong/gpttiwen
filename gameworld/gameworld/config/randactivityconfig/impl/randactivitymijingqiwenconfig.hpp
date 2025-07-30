#ifndef __RA_MI_JING_QI_WEN_CONFIG_HPP__
#define __RA_MI_JING_QI_WEN_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "other/rolecrystalfb/rolecrystalfbconfig.hpp"

struct new_Event_NPC_cfg
{
	enum CRYSTALFB_NPC_TYPE
	{
		CRYSTALFB_NPC_TYPE_0 = 0,		//!< 未知物品
		CRYSTALFB_NPC_TYPE_1 = 1,		//!< 怪物
		CRYSTALFB_NPC_TYPE_2 = 2,		//!< npc
		CRYSTALFB_NPC_TYPE_3 = 3,		//!< Boss
		CRYSTALFB_NPC_TYPE_4 = 4,		//!< 篝火
		CRYSTALFB_NPC_TYPE_5 = 5,		//!< 铜币
		CRYSTALFB_NPC_TYPE_6 = 6,		//!< 采集物 - 木材(斧子)
		CRYSTALFB_NPC_TYPE_7 = 7,		//!< 采集物 - 矿石（锤子）
		CRYSTALFB_NPC_TYPE_8 = 8,		//!< 采集物 - 草（铲子）
		CRYSTALFB_NPC_TYPE_9 = 9,		//!< 宝箱
		CRYSTALFB_NPC_TYPE_10 = 10,		//!< 传说门
		CRYSTALFB_NPC_TYPE_11 = 11,		//!< 答题
	};
	new_Event_NPC_cfg() : event_group_id(0), npc_id(0), rate(0), npc_type(0) {}
	int event_group_id;
	int npc_id;
	int rate;
	int npc_type;
};

struct MiJingQiWenFbNPCRewardCfg
{
	enum REWARD_TYPE
	{
		REWARD_TYPE_0 = 1,//buff
		REWARD_TYPE_1 = 2,//奖励id
		REWARD_TYPE_2 = 3,//商店id
		REWARD_TYPE_3 = 4,//金钱奖励
		REWARD_TYPE_4 = 5,//交流
		REWARD_TYPE_5 = 6,//答题
		REWARD_TYPE_MAX,
	};
	MiJingQiWenFbNPCRewardCfg() :npc_id(0), option_index(0), reward_type(0), is_delete(0)
	{
	}

	int npc_id;
	int option_index;
	int reward_type;
	int is_delete;
	int reward_list[MAX_ROLE_CRYSTAL_FB_REWARD_COLUMN];//秘境奇闻修改为，0 = 答对buff，1答错buff
};

struct MiJingQiWenFbNPCRewardListCfg
{
	const static int MAX_OPERATION_INDEX = 2;//最大选择个数
	MiJingQiWenFbNPCRewardListCfg()
	{
	}
	MiJingQiWenFbNPCRewardCfg reward_list[MAX_OPERATION_INDEX];
};

struct MiJingQiWenOtherCfg
{
	MiJingQiWenOtherCfg()
	{
		item_num = 0;
		random_event_num = 0;
		question_num = 0;
		question_times = 0;
		mall_item_num = 0;
		scene_id = 0;
		x = 0;
		y = 0;
		start_time = 0;
		end_time = 0;
		question_id = 0;
	}

	int item_num;
	int	random_event_num;
	int	question_num;
	int	question_times;
	int	mall_item_num;
	int	scene_id;
	int	x;
	int	y;
	int	start_time;
	int	end_time;
	int question_id;
};

struct MiJingQiWenFbLevelTypeCfg
{
	MiJingQiWenFbLevelTypeCfg() : type(0), level_down(0), level_up(0) {}
	int type;
	int	level_down;
	int	level_up;
};

class RandActivityManager;
class RandActivityMiJingQiWenConfig : public RandActivityConfig
{
public:
	RandActivityMiJingQiWenConfig();
	virtual ~RandActivityMiJingQiWenConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RoleCrystalFbLevelCfg * GetRoleCrystalFbLevelCfg(int seq) const;
	const int GetLayerNum(int seq) const;
	const bool IsCrystalFbBoxGather(int gather_id) const;
	const CrystalFbBoxCfg::ItemCfg * GetBoxCfg(int group_id, int box_seq) const;
	//	int GetRandScene(int scene_group_id, std::set<int>  exclude_scene_id = std::set<int>());
	const CrystalFbSceneCfg::ItemCfg * GetRandSceneCfg(int scene_group_id, std::set<int>  exclude_scene_id = std::set<int>()) const;
	const Posi GetRandPosi(int pos_group_id) const;

	const CrystalFbBoxCfg::ItemCfg * GetRandBoxCfg(int rand_group_id) const;
	const std::vector<ItemConfigData>   GetRandBoxReward(std::vector<int> reward_group_ids) const;

	const ItemConfigData * GetRandNpcReward(int reward_group_ids) const;

	const int GetMaxRewardNum(int reward_group_id)const;
	const int GetRandNum(int num_group_id)const;
	const MiJingQiWenOtherCfg & GetCrystalFbOtherCfg()const;
	const int GetUseItemLimitTimes(int use_limit_group_id, int use_type)const;
	const int IsLimitTimesUseType(int use_limit_group_id, int use_type)const;
	const CrystalFbMonsterCfg::ItemCfg * GetCrystalFbMonsterCfg(int monster_group_id)const;
	const std::vector<int>  GetMonsterDropId(int monster_npc_id, int play_times = 0, int * village_id = NULL, int * prestige_num = NULL, int *add_exp = NULL, std::vector<ItemConfigData> * reward_list = NULL) const;
	const int GetRewardColumn(int play_times)const;
	const MiJingQiWenFbNPCRewardCfg * GetNpcRewardCfg(int npc_id, int option_index)const ;
	const MiJingQiWenFbNPCRewardListCfg * GetNpcEventCfg(int npc_id) const;
	const int GetEventNpcCfg(int group_id, int rand_num, std::vector<new_Event_NPC_cfg>*get_npc_list, std::vector<new_Event_NPC_cfg>*has_create_npc_vec)const;
	const BuffListConfig* GetBuffId(int buff_id)const;
	const int RandNPCShopItemList(int npc_id, NpcShopItem but_list[MAX_ROLE_CRYSTALFB_BUY_ITEM_COUNT])const;
	const GatherNpcConfig * GetGatherNpcCfg(int npc_id) const;

	const int GetWorkshopItemWeightCfg(int group_id)const;
	const std::vector<WorkshopItemConfig> * GetWorkshopItemCfg(int group_id)const;

	const MiJingQiWenFbLevelTypeCfg * GetLevelToCfg(int role_level) const;
private:
	int InitLevelCfg(TiXmlElement *RootElement);
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
	int InitMiJingQiWenLevelTypeCfg(TiXmlElement *RootElement);

	//-----------------------
	void RefreshPos(CrystalFbSceneCfg::ItemCfg ARG_OUT & cfg) const;


	std::vector<RoleCrystalFbLevelCfg> m_level_cfg;
	std::map<int, CrystalFbBoxCfg> m_box_cfg;
	std::map<int, CrystalFbRandNumCfg> m_rand_num_cfg;
	std::map<int, CrystalFbRewardCfg> m_rewards_cfg;
	std::map<int, CrystalFbSceneCfg> m_scene_cfg;
	std::map<int, CrystalFbPosCfg> m_pos_cfg;
	std::map<int, CrystalFbUseItemLimitCfg> m_use_item_limit_cfg;
	std::map<int, CrystalFbMonsterCfg> m_monster_cfg;
	std::set<int> m_all_box_gather_id;
	MiJingQiWenOtherCfg m_other_cfg;

	std::map<int, int> m_play_times_to_reward_column;
	std::map<int, CrystalFbMonsterDropCfg> m_monster_drop_cfg;

	std::map<int, int> m_event_npc_cfg_weight_map;//npc对应总权重
	std::map<int, std::vector<new_Event_NPC_cfg> > m_event_npc_cfg_map;
	std::map<int, MiJingQiWenFbNPCRewardListCfg> m_npc_reward_cfg_map;
	std::map<int, BuffListConfig> m_buff_id_list_map;
	std::map<int, std::vector<BuffListConfig> > m_buff_list_group;
	std::map<int, std::vector<NpcShopConfig> > m_npc_shop_list_map;
	std::map<int, GatherNpcConfig> m_gather_npc_cfg_map;

	std::map<int, int> m_workshop_wight_map;
	std::map<int, std::vector<WorkshopItemConfig> > m_workshop_item_cfg_map;

	std::vector<MiJingQiWenFbLevelTypeCfg> m_level_to_seq_vec;
};

#endif