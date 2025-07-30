#ifndef __PET_GOD_FB_CONFIG_HPP__
#define __PET_GOD_FB_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "servercommon/petgodfbdef.hpp"

#include "gamedef.h"

//神印之地
class PetGodFbConfig : public ILogicConfig
{
public:
	//其他
	struct OtherCfg
	{
		OtherCfg()
		{
			day_base_times = 0;
		}

		int day_base_times;		//每日基础次数  小于0:不限制
	};

	//副本配置
	struct FBCfg
	{
		FBCfg()
		{
			fb_seq = 0;
			fb_type = 0;
			fb_level = 0;

			scene_id = 0;
			role_level_limit = 0;
			cost_power = 0;

			auto_limit_cap = 0;

			recover_npc_id = 0;
			recover_npc_dir = 0;

			boss_dir = 0;
			boss_npc_id = 0;
		}
		int fb_seq;				//副本seq
		int fb_type;			//副本类型 1开始
		int fb_level;			//副本等级 1开始

		int scene_id;			//场景id
		int role_level_limit;	//角色等级限制
		int cost_power;			//单次消耗体力

		int auto_limit_cap;		//扫荡所需综合战力

		//奖励组id[奖励组数量]
		std::vector<int> reward_group_list;
		int draw_num;


		//角色出生点
		Posi role_pos;

		//恢复npc
		int recover_npc_id;
		Posi recover_npc_pos;
		Dir recover_npc_dir;


		Posi boss_pos;
		Dir boss_dir;
		int boss_npc_id;

	};


	//奖励组 [奖励组id][等级][权重池]
	struct RewardItemInfo
	{
		RewardItemInfo()
		{
			group_id = 0;
			power = 0;
			role_min_level = 0;
			role_max_level = 0;
		}
		int group_id;		//奖励组id
		int power;

		//角色等级区间 闭区间
		int role_min_level;
		int role_max_level;

		ItemConfigData reward_item;
	};

	//奖励权重map <起始权重, 奖励>
	typedef std::map<int, RewardItemInfo> RewardPowerMap;
	struct RewardPool
	{
		RewardPool()
		{
			total_power = 0;
		}
		int total_power;
		RewardPowerMap power_pool;
	};

	//奖励组信息map<最低角色等级, 奖励组信息>
	typedef std::map<int, RewardPool> RewardPoolLevelMap;

	//奖励组ID map <奖励组id, 奖励组>
	typedef std::map<int, RewardPoolLevelMap> RewardGroupIDMap;

	//购买次数
	struct BuyTimeCfg
	{
		BuyTimeCfg()
		{
			buy_times = 0;
			cost_gold = 0;
		}

		int buy_times;		//总购买次数
		int cost_gold;		//本次消耗元宝
	};
	//奖励组ID map <总购买次数, 配置>
	typedef std::map<int, BuyTimeCfg> BuyTimeCfgMap;
public:
	PetGodFbConfig();
	virtual ~PetGodFbConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const OtherCfg& GetOtherCfg() const { return m_other_cfg; };
	const FBCfg* GetFBCfg(int fb_seq) const;

	bool GetFBRewardData(int fb_seq, int role_level, std::vector<ItemConfigData> &out_item_list) const;

	const BuyTimeCfg * GetBuyTimeCfg(int buy_times) const;

	long long GetRewardExp(int  role_level) const;

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitRewardGroupCfg(TiXmlElement *RootElement);
	int InitBuyTimesCfg(TiXmlElement *RootElement);
	int InitEXPRewardCfg(TiXmlElement *RootElement);
	int InitFBCfg(TiXmlElement *RootElement);

	const RewardItemInfo* GetGroupReward(int group_id, int role_level) const;


	OtherCfg m_other_cfg;
	//副本配置[副本seq]
	std::map<int, FBCfg> m_fb_cfg_map;

	//奖励组 [奖励组id][等级]
	RewardGroupIDMap m_reward_group_id_map;

	//购买次数
	BuyTimeCfgMap m_buy_times_map;
	//经验奖励[角色等级]
	std::vector<long long> m_exp_reward_list;
};

#endif