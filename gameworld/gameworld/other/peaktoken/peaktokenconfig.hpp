#ifndef __PEAK_TOKEN_CONFIG_HPP__
#define __PEAK_TOKEN_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "servercommon/peaktokendef.hpp"

#include "gamedef.h"



class PeakTokenConfig : public ILogicConfig
{
public:
	//其他
	struct OtherCfg
	{
		OtherCfg()
		{
			act_duration_s = 0;
			notice_interval_s = 0;

			game_time_s = 0;
			role_reward_times = 0;

			item_can_use_times = 0;
		}

		int act_duration_s;		//活动持续时间(s)
		int notice_interval_s;	//公告时间间隔(s)

		int game_time_s;		//游戏时间(s)
		int role_reward_times;	//玩家奖励次数

		int item_can_use_times;	//物品可使用次数
	};

	//奖励组
	struct RewardCfg
	{
		RewardCfg()
		{
			group_id = 0;
			power = 0;
			is_broadcast = 0;
		}
		int group_id;

		int power;				//权重
		ItemConfigData item;	//奖励
		int is_broadcast;		//广播
	};

	//奖励组 奖池map<权重_begin, 奖励组物品>
	typedef std::map<int, RewardCfg>  RewardPool;
	struct GroupCfg
	{
		GroupCfg()
		{
			group_id = 0;

			total_power = 0;
		}
		int group_id;

		int total_power;	//奖池总权重
		RewardPool item_pool;
	};
	//奖励组id map<奖励组id, 奖励组>
	typedef std::map<int, GroupCfg>  GroupIDMap;

public:
	PeakTokenConfig();
	virtual ~PeakTokenConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const OtherCfg & GetOtherCfg() const { return m_other_cfg; };

	const RewardCfg * GetGroupRandCfg(int group_id) const;

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitRewardGroupCfg(TiXmlElement *RootElement);


	OtherCfg m_other_cfg;

	GroupIDMap m_group_id_map;
};
#endif
