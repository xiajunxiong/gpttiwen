#ifndef __RAND_ACTIVITY_PET_LOOT_CONFIG_HPP__
#define __RAND_ACTIVITY_PET_LOOT_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/rapetlootparam.hpp"
#include "item/itembase.h"
#include <vector>

class RandActivityManager;

struct PetLootReward
{
	PetLootReward() {}
	enum PET_TYPE
	{
		PET_TYPE_1 = 1,		// 普卡宠物
		PET_TYPE_2,			// 银卡宠物
		PET_TYPE_3,			// 金卡宠物
		PET_TYPE_4,			// 稀有宠物
		PET_TYPE_5,			// 神兽宠物
		PET_TYPE_MAX
	};
	enum INT_TYPE
	{
		INT_TYPE_0 = 1,		// 强化次数
		INT_TYPE_1,			// 强化等级
		INT_TYPE_MAX
	};
	int type;
	int section_start;
	int section_end;
	int	seq;
	std::vector<int> pet_type;
	int	int_type;
	int	parameter;
	ItemConfigData	reward_item;
	int	reward_times;
};

struct TaskGiftConfigure
{
	TaskGiftConfigure() {}
	int type;
	int task_num;
	int group_id;
	ItemConfigData reward_item;
};

struct RamdGroup
{
	RamdGroup() {}
	int group_id;
	int rate;
	std::vector<ItemConfigData> reward_list;
};

class RandActivityPetLootConfig : public RandActivityConfig
{
public:
	RandActivityPetLootConfig();
	virtual ~RandActivityPetLootConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const PetLootReward * GetRewardCfg(int open_day,int seq) const;
	const TaskGiftConfigure * GetTaskGiftCfg(int type) const;
	const RamdGroup * GetRandRewardCfg(int group_id) const;
	const int GetTaskGiftSize()const { return (int)m_gift_map.size(); }
private:
	int InitPetLootCfg(TiXmlElement *RootElement);
	int InitTaskGiftConfigureCfg(TiXmlElement *RootElement);
	int InitRewardGroupCfg(TiXmlElement *RootElement);

	std::map<int, PetLootReward> m_cfg_map;
	std::map<int, TaskGiftConfigure> m_gift_map;

	std::map<int, int> m_rand_weight;
	std::map<int, std::vector<RamdGroup> > m_rand_group;
};

#endif	//__RAND_ACTIVITY_PET_LOOT_CONFIG_HPP__