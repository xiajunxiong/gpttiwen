#ifndef __RA_ZAI_ZHAN_YUN_ZE_CONFIG_HPP__
#define __RA_ZAI_ZHAN_YUN_ZE_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itembase.h"
#include "servercommon/roleactivity/razaizhanyunzeparam.hpp"

namespace RAZaiZhanYunZe
{
	//其他
	struct OtherCfg
	{
		OtherCfg()
		{
			battle_cost_item_id = 0;
		}
		ItemID battle_cost_item_id;		//挑战消耗物品id
	};

	struct BossCfg
	{
		BossCfg()
		{
			boss_index = 0;

			battle_cost_item_num = 0;

			act_open_day = 0;
			monster_group_id = 0;

			pass_rand_id = 0;
			first_pass_id = 0;
		}

		int boss_index;

		int battle_cost_item_num;

		int act_open_day;		//活动开启天数
		int monster_group_id;	//怪物组id

		int pass_rand_id;	//通关随机奖励组id(随机组)

		int first_pass_id;	//首通奖励组id(固定组)

		//通关额外奖励组map<通关次数, 奖励组id(固定组)>
		typedef std::map<int, int> PassNumIDMap;
		PassNumIDMap pass_num_id_map;
	};

	//奖励vec
	typedef std::vector<ItemConfigData> RewardVec;
	
	typedef std::map<int, RewardVec> DefSectionDayMap;
	//固定奖励组
	struct DefRewardGroupCfg
	{
		DefRewardGroupCfg()
		{
			group_id = 0;
		}
		int group_id;

		//固定奖励阶段配置map<活动开启时的开服天数, 配置>
		DefSectionDayMap section_cfg_map;
	};
	//固定奖励组map<奖励组id, 固定奖励组>
	typedef std::map<int, DefRewardGroupCfg> DefGroupIdMap;


	//随机奖励
	struct RandCfg
	{
		RandCfg()
		{
			index = 0;
		}
		int index;				//奖励index
		RewardVec reward_vec;	//奖励
	};
	//奖池<权重end, 奖励>
	typedef std::map<int, RandCfg> RandPool;
	//随机奖励阶段配置
	struct RandSectionCfg
	{
		RandSectionCfg()
		{
			game_open_day_begin = 0;
			total_power = 0;
		}
		int game_open_day_begin;	//活动开启时的开服天数

		int total_power;			//总权重							
		RandPool rand_pool;
	};
	//阶段配置<活动开启时的开服天数, 配置>
	typedef std::map<int, RandSectionCfg> RandSectionDayMap;
	//随机奖励组
	struct RandRewardGroupCfg
	{
		RandRewardGroupCfg()
		{
			group_id = 0;
		}
		int group_id;

		RandSectionDayMap section_day_map;
	};
	//随机奖励组map<奖励组id, 随机奖励组>
	typedef std::map<int, RandRewardGroupCfg> RandGroupIdMap;
};

class RAZaiZhanYunZeConfig : public RandActivityConfig
{
public:
	RAZaiZhanYunZeConfig();
	virtual ~RAZaiZhanYunZeConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAZaiZhanYunZe::OtherCfg& GetOtherCfg() const { return m_other_cfg; };

	const RAZaiZhanYunZe::BossCfg* GetBossCfg(int boss_index) const;
	const RAZaiZhanYunZe::RewardVec* GetDefRewardGroupCfg(RandActivityManager* ramgr, int def_group_id) const;
	const RAZaiZhanYunZe::RewardVec* GetRandRewardGroupCfg(RandActivityManager* ramgr, int rand_group_id) const;

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitDefGroupIdMap(TiXmlElement *RootElement);
	int InitRandGroupIdMap(TiXmlElement *RootElement);
	int InitBossCfg(TiXmlElement *RootElement);

	//查看奖励组配置
	bool CheckDefGroupID(int group_id) const;
	bool CheckRandGroupID(int group_id) const;

	RAZaiZhanYunZe::OtherCfg m_other_cfg;	//其他

	RAZaiZhanYunZe::DefGroupIdMap m_def_group_id_map;	//固定奖励组
	RAZaiZhanYunZe::RandGroupIdMap m_rand_group_id_map;	//随机奖励组

	std::vector<RAZaiZhanYunZe::BossCfg> m_boss_cfg_vec;//boss配置[boss_index]

};



#endif