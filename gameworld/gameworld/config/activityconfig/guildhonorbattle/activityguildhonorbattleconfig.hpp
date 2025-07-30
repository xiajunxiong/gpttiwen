#ifndef __ACTIVITY_GUILD_HONOR_BATTLE_CONFIG__
#define __ACTIVITY_GUILD_HONOR_BATTLE_CONFIG__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/activitydef.hpp"

#include <set>
#include <map>
#include <vector>

struct ActivityGuildHonorBattleOtherCfg
{
	ActivityGuildHonorBattleOtherCfg()
	{
		scene_id = 0;
		first_ready_duration = 10 * 60;
		ready_duration = 5 * 60;
		first_battle_duration = 20 * 60;
		battle_duration = 25 * 60;
		max_battle_rounds = 100;

		settlement_list_time_wday = 0;
		settlement_list_time_hour = 0;
		settlement_list_time_min = 0;
		settlement_list_day = 0;
	}

	Posi GetAckPos() const;
	Posi GetDefPos() const;

	bool IsVaildDay(int wday) const;
	int GetLastVaildDay() const;

	int scene_id;
	int first_ready_duration;
	int ready_duration;
	int first_battle_duration;
	int battle_duration;
	int max_battle_rounds;

	int settlement_list_time_wday;
	int settlement_list_time_hour;
	int settlement_list_time_min;
	unsigned char settlement_list_day;			//用哪些天的积分排序
	std::vector<Posi> ack_pos;
	std::vector<Posi> def_pos;
};

struct ActivityGuildHonorBattleRewardCfg
{
	ActivityGuildHonorBattleRewardCfg()
	{
		gong_xian = 0;
		coin = 0;
		guild_coin = 0;
		title_id = 0;
	}

	int gong_xian;
	int coin;
	int guild_coin;
	int title_id;
	std::vector<ItemConfigData> items;

};

struct ActivityGuildHonorBattleGuessCfg
{
	ActivityGuildHonorBattleGuessCfg()
	{
		right_coin = 0;
		right_gong_xian = 0;
		wrong_coin = 0;
		wrong_gong_xian = 0;
	}

	int right_coin;
	int right_gong_xian;
	int wrong_coin;
	int wrong_gong_xian;
}; 

struct ActivityGuildHonorBattleRankRewardCfg
{
	ActivityGuildHonorBattleRankRewardCfg()
	{
		rank = 0;
		title_id = 0;
		coin = 0;
		low_winner_coin = 0;
	}

	int rank;
	int title_id;
	int coin;
	int low_winner_coin;		// 未参与铜币奖励
	std::vector<int> buff_id;
	std::vector<ItemConfigData> rewards;
	std::vector<ItemConfigData> low_rewards;	// 未参与物品奖励
};


class ActivityGuildHonorBattleConfig : public ILogicConfig
{
public:
	ActivityGuildHonorBattleConfig();
	virtual ~ActivityGuildHonorBattleConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const ActivityGuildHonorBattleOtherCfg & GetOtherCfg();
	const ActivityGuildHonorBattleRewardCfg * GetActivityReward(int reward_type);
	const ActivityGuildHonorBattleGuessCfg * GetActivityGuildHonorBattleGuessCfg(int type);
	const ActivityGuildHonorBattleRankRewardCfg * GetActivityGuildHonorBattleRankRewardCfg(int rank);
	std::vector<int> GetGuildHonorRankkbuff(int rank);
private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitGuessCfg(TiXmlElement * RootElement);
	int InitBirthPosCfg(TiXmlElement * RootElement);
	int InitRankRewardCfg(TiXmlElement * RootElement);

	ActivityGuildHonorBattleOtherCfg m_other_cfg;
	std::map<int, ActivityGuildHonorBattleRewardCfg> m_reward_cfg;
	std::map<int, ActivityGuildHonorBattleGuessCfg> m_guess_cfg;
	std::map<int, ActivityGuildHonorBattleRankRewardCfg> m_rank_reward_cfg;
};

#endif 