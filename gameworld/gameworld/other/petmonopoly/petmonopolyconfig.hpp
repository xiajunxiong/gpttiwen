#ifndef __PET_MONOPOLY_CONFIG_HPP__
#define __PET_MONOPOLY_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/petmonopolydef.hpp"
#include "servercommon/struct/itemlistparam.h"
#include "other/pet/petconfig.hpp"


struct PetMonopolyRewardGroupItem
{
	PetMonopolyRewardGroupItem() : rate(0) {}

	int rate;
	ItemConfigData item;
};

struct PetMonopolyWishCard
{
	PetMonopolyWishCard() : reward_group_id(0), reward_type(0){}

	int reward_group_id;
	int reward_type;
	ItemConfigData item;
};


struct PetMonopolyRewardGroup
{
	PetMonopolyRewardGroup() : reward_group_id(0), total_rate(0), reward_num(0), special_item(0){}

	int reward_group_id;
	int total_rate;
	int reward_num;
	int special_item;
	PetMonopolyRewardGroupItem reward_list[10];
};

struct PetMonopolyOtherCfg
{
	PetMonopolyOtherCfg() : dice_cost_energy(0), expect_pet_rate_silver(0), expect_pet_rate_gold(0), guide_pet_monster_group(0) {}

	int dice_cost_energy;
	int expect_pet_rate_silver;
	int expect_pet_rate_gold;
	int guide_pet_monster_group;
};

struct PetMonopolyPetGroupItem
{
	PetMonopolyPetGroupItem() : pet_id(0), quality(0), monster_group_id(0), fragment_item_id(0), fragment_item_num(0) {}

	int pet_id;
	int quality;
	int monster_group_id;
	int fragment_item_id;
	int fragment_item_num;
};

struct PetMonopolyPetGroup
{
	static const int PET_GROUP_MAX_PET_NUM = 40;

	PetMonopolyPetGroup()
	{
		memset(quality_pet_num, 0, sizeof(quality_pet_num));
	}

	int quality_pet_num[PET_QUALITY_MAX];
	PetMonopolyPetGroupItem quality_pet_list[PET_QUALITY_MAX][PET_GROUP_MAX_PET_NUM];
};

struct PetMonopolyTurnTableSector
{
	PetMonopolyTurnTableSector() : reward_type(0), param(0), rate(0) {}

	int reward_type;
	int param;
	ItemConfigData item;
	int rate;
};

struct PetMonopolyTurnTableGroup
{
	PetMonopolyTurnTableGroup() : group_id(0), total_rate(0), sector_num(0) {}

	int group_id;
	int total_rate;
	int sector_num;
	PetMonopolyTurnTableSector sector_list[PET_MONOPOLY_TURNTABLE_SECTOR_NUM];
};

struct PetMonopolyLayerGridCfg
{
	PetMonopolyLayerGridCfg() : layer(0), grid_idx(0), reward_type(0), param(0), param2(0), grid_type(0), version(0) {}

	int layer;
	int grid_idx;
	int reward_type;
	int param;
	int param2;
	int grid_type;
	int version;
};

struct PetMonopolyLayerCfg
{
	PetMonopolyLayerCfg() : layer(0) {}

	int layer;
	PetMonopolyLayerGridCfg grid_list[PET_MONOPOLY_GRID_NUM];
};

class PetMonopolyConfig : public ILogicConfig
{
public:
	static const int MAX_LAYER_NUM = 25;
	static const int MAX_REWARD_GROUP_NUM = 50;
	static const int MAX_TURNTABLE_GROUP_NUM = 10;
	static const int MAX_WISH_CARD_NUM = 10;

public:
	PetMonopolyConfig();
	virtual ~PetMonopolyConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const PetMonopolyOtherCfg & GetOtherCfg() { return m_other_cfg; }
	const PetMonopolyLayerCfg* GetLowerBoundLayerCfg(int layer);
	const PetMonopolyRewardGroup* GetRewardGroup(int group_id);
	const PetMonopolyTurnTableGroup* GetTurntableGroup(int group_id);
	const PetMonopolyWishCard* GetWishCardReward(int wish_card_seq);
	bool IsPetInPool(int pet_id, int quality);
	bool RandItemByRewardGroupId(int reward_group_id, ARG_OUT PetMonopolyGridItem* item);
	bool RandPetFragmentByPetQuality(int pet_quality, int specific_pet, int expect_silver_pet_id, int expect_gold_pet_id, ARG_OUT PetMonopolyGridItem* item);
	bool RandPetMonsterByPetQuality(int silver_pet_chance, int specific_pet, int expect_silver_pet_id, int expect_gold_pet_id, ARG_OUT PetMonopolyGridPet* pet);
	int RandPetQuality(int silver_pet_chance);
	bool RandSpecialItem(ARG_OUT PetMonopolyGridSpecialItem * special_item);
	int GetAddExpNum(int level);
	int GetExpectPetRate(int pet_quality);
	int GetSpecialItemParam(int special_item_id);	
	
private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitRewardGroup(TiXmlElement *RootElement);
	int InitPetGroup(TiXmlElement* RootElement);
	int InitTurnTableGroup(TiXmlElement* RootElement);
	int InitExpCfg(TiXmlElement* RootElement);
	int InitExpectPetRateUpCfg(TiXmlElement* RootElement);
	int InitLayerCfg(TiXmlElement* RootElement);
	int InitSpecialItemCfg(TiXmlElement* RootElement);
	int InitWishCade(TiXmlElement* RootElement);

	PetMonopolyRewardGroup* MutableRewardGroupCfg(int group_id);
	PetMonopolyTurnTableGroup* MutableTurnTableGroupCfg(int group_id);
	PetMonopolyLayerCfg* MutableLayerCfg(int layer);
	const PetMonopolyRewardGroupItem* RandItemFromRewardGroup(const PetMonopolyRewardGroup* reward_group);
	const PetMonopolyPetGroupItem* RandPetItemFromPetGroup(int expect_pet_id, int specific_pet_id, int pet_quality);
	bool IsPetInPoolImpl(int pet_id, int pool_pet_num, PetMonopolyPetGroupItem* pool_pet_list);

	int m_reward_group_num;
	PetMonopolyRewardGroup m_reward_group_list[MAX_REWARD_GROUP_NUM];

	PetMonopolyOtherCfg m_other_cfg;

	PetMonopolyPetGroup m_pet_group;

	int m_turn_table_group_num;
	PetMonopolyTurnTableGroup m_turn_table_group_list[MAX_TURNTABLE_GROUP_NUM];

	int m_level_exp_list[MAX_ROLE_LEVEL + 1];

	int m_layer_cfg_num;
	PetMonopolyLayerCfg m_layer_cfg_list[MAX_LAYER_NUM];

	int m_special_item_param[PET_MONOPOLY_SPECIAL_ITEM_MAX];		//道具参数

	int m_fast_move_num;		//标记神行移动次数
	PetMonopolyWishCard m_wish_card[MAX_WISH_CARD_NUM];		//心愿卡道具组
};


#endif