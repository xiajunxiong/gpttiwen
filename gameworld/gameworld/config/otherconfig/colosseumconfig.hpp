#ifndef __COLOSSEUM_CONFIG_HPP__
#define __COLOSSEUM_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/serverdef.h"
#include "servercommon/colosseumdef.hpp"
#include "servercommon/struct/itemlistparam.h"

#include <set>
#include <map>
#include <vector>

static const int MAX_COLOSSEUM_ROBOT_LEVEL_CFG_NUM = 320;
static const int COLOSSEUM_ROBOT_ATTR_ADJUST_PERCENT = 10000;

struct ColosseumOtherCfg
{
	ColosseumOtherCfg() : challenge_times(0), battle_max_round(0), win_price(0), lose_price(0), reward_pool_coin(0), copy_attr_perc(0), client_refresh_interval_s(0)
	{
		max_colosseum_pool_credits = 0;
		min_colosseum_pool_credits = 0;
		top_1_title_id = 0;
		top_10_title_id = 0;
		refresh_intervar = 0;
		buff_open_server_days = 0;
		battle_reward = 0;
		raiding_battle = 0;
		open_level = 0;
	}

	int challenge_times;
	int battle_max_round;
	int	win_price;
	int lose_price;
	int reward_pool_coin;			// 铜币池的标准奖励
	int copy_attr_perc;				// 玩家进入宝库后，属性竞技场属性的保留比例
	int min_pool_num;				// 当人数不足这个数字时，要改变铜币池奖励
	int bottom_pool_coin;			// 当人数不足时，铜币池的奖励
	int client_refresh_interval_s;	// 客户端刷新CD（秒）
	int max_colosseum_pool_credits;
	int min_colosseum_pool_credits;

	int top_1_title_id;			//第一名称号id
	int top_10_title_id;			//前十名称号id
	int refresh_intervar;
	int buff_open_server_days;		// 守方buff开启天数
	int battle_reward;			// 单场奖励

	int raiding_battle;		// 扫荡战力比例
	int open_level;			//扫荡开启等级
};

struct ColosseumCfg
{
	ColosseumCfg() : exp(0), bind_coin(0) {}

	int exp;
	int bind_coin;
};

struct ColosseumAwardCfg
{
	ColosseumAwardCfg() : min_rank(0), max_rank(0), award_coin(0), colosseum_credits(0){}

	int min_rank;
	int max_rank;	
	int award_coin;
	int colosseum_credits;
	std::vector< ItemConfigData> award_list;
};

struct ColosseumRobotLevelCfg
{
	ColosseumRobotLevelCfg() : rank_min(0), rank_max(0), level_min(0), level_max(0), show_level(0), attr_adjust_group(0), buff_group(0) {}

	int rank_min; // [rank_min, rank_max] 前闭后闭区间
	int rank_max; // [rank_min, rank_max] 前闭后闭区间
	int level_min;
	int level_max;
	int show_level;
	int attr_adjust_group;
	int buff_group;
};

struct ColosseumRobotDaysLevelCfg
{
	ColosseumRobotDaysLevelCfg() : open_days(0), robot_level_cfg_num(0) {}

	int open_days;
	int robot_level_cfg_num;
	ColosseumRobotLevelCfg robot_level_cfg_list[MAX_COLOSSEUM_ROBOT_LEVEL_CFG_NUM];
};

struct ColosseumBuffCfg
{
	ColosseumBuffCfg()
	{
		rate_count = 0;
	}

	struct ItemCfg
	{
		ItemCfg()
		{
			rate = 0;
			buff_id = 0;
		}

		int rate;
		int buff_id;
	};

	int rate_count;
	std::vector<ItemCfg> buff_list;
};

struct ColosseumRefreshCfg
{
	struct ItemCfg
	{
		ItemCfg()
		{
			section_left = 0;
			section_right = 0;
		}

		double  section_left;
		double  section_right;
	};

	const ItemCfg * GetItemCfg(int index) const
	{
		if (index < 0 || index  >= (int)refresh_num.size())
		{
			return NULL;
		}

		static ItemCfg item_cfg;
		item_cfg.section_left = refresh_num[index];

		index + 1 >= (int)refresh_num.size() ? item_cfg.section_right = 1.0 : item_cfg.section_right = refresh_num[index + 1];

		return &item_cfg;
	}


	ColosseumRefreshCfg()
	{
		rank = 0;
	}

	int rank;
	std::vector<double> refresh_num;
};

struct ColosseumRobotAttrAdjustCfg
{
	ColosseumRobotAttrAdjustCfg() : group_id(0), role_attack_adjust(0), role_mental_adjust(0), role_recover_adjust(0),
		role_maxhp_adjust(0), pet_attack_adjust(0), pet_mental_adjust(0), pet_recover_adjust(0), 
		pet_maxhp_adjust(0),partner_attack_adjust(0), partner_mental_adjust(0), partner_recover_adjust(0),
		partner_maxhp_adjust(0) {}

	int group_id;
	int role_attack_adjust;
	int role_mental_adjust;
	int role_recover_adjust;
	int role_maxhp_adjust;
	int pet_attack_adjust;
	int pet_mental_adjust;
	int pet_recover_adjust;
	int pet_maxhp_adjust;
	int partner_attack_adjust;
	int partner_mental_adjust;
	int partner_recover_adjust;
	int partner_maxhp_adjust;
};

struct ColosseumAchievementCfg
{
	ColosseumAchievementCfg() : sn(0), ach_group(0), achieve_type(0), ach_param1(0), progress_way(0), price_type(0), price_num(0) {}
	int sn;
	int	ach_group;
	int achieve_type;
	int	ach_param1;
	int	progress_way;
	int price_type;
	int price_num;
	std::vector<ItemConfigData> item_list;
};

struct ColosseumHistoryRankRewardCfg
{
	ColosseumHistoryRankRewardCfg() : rank_head(0), rank_tail(0), reward_score_num(0) {}

	int rank_head;
	int rank_tail;
	int reward_score_num;
};

struct ColossumBattleRewardCfg
{
	ColossumBattleRewardCfg():battle_price(0), integral(0), bind_coin(0), rate(0)
	{}

	int battle_price;	// 奖励组
	int integral;
	int bind_coin;
	int rate;
	std::vector<ItemConfigData> reward_item_list;
};

class ColosseumConfig : public ILogicConfig
{
public:
	static const int MAX_DAYS_NUM = 20;
	static const int MAX_ATTR_ADJUST_GROUP_NUM = 320;

public:
	ColosseumConfig();
	virtual ~ColosseumConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	void GetRankAwardListSeq(std::set<int> & out_list);

	const ColosseumOtherCfg & GetOtherCfg() const { return m_other_cfg; }
	const ColosseumCfg * GetColosseumCfg(int level) const;
	const ColosseumAwardCfg * GetAwardCfg(int award_seq) const;
	const ColosseumAwardCfg * GetRandRewardCfg(int rank) const;
	int RandRobotLevelByRank(int open_server_days, int rank) const;
	const ColosseumRefreshCfg * GetRefreshCfg(int rank);

	void GetRandDefenderBuffId(int max_num, ARG_OUT int* out_num, ARG_OUT int* out_buff_list, int server_open_day);
	int GetTitleIdByRank(int rank);

	int GetRobotShowLevelByRankAndOpenServerDays(int rank, int open_server_days);
	const ColosseumRobotLevelCfg* GetRobotLevelCfgByRankAndOpenServerDays(int rank, int open_server_days);
	const ColosseumRobotAttrAdjustCfg* GetRobotAttrAdjustCfg(int group_id);
	const ColosseumAchievementCfg* GetAchievementCfg(int sn);

	const ColossumBattleRewardCfg* RandBattleRewardCfg();

	int CalcHistoryRankReward(int old_rank, int change_rank_num);
	int GetMaxHistoryRank();
private:
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitColosseumCfg(TiXmlElement * RootElement);
	int InitRankAwardCfg(TiXmlElement * RootElement);
	int InitRobotLevelCfg(TiXmlElement * RootElement);
	int InitBuffCfg(TiXmlElement * RootElement);
	int InitRefreshCfg(TiXmlElement * RootElement);
	int InitAttrAdjustGroupCfg(TiXmlElement * RootElement);
	int InitArenaAchievementCfg(TiXmlElement * RootElement);
	int InitHistoryRankRewardCfg(TiXmlElement * RootElement);
	int InitBattleRewardCfg(TiXmlElement * RootElement);
	int GetRandDefenderBuffId(const ColosseumBuffCfg& cfg);

	ColosseumOtherCfg m_other_cfg;
	std::map<int, ColosseumCfg> m_colosseum_cfg;
	std::map<int, ColosseumAwardCfg> m_rank_award_cfg;
	std::vector<ColosseumRefreshCfg> m_refresh_cfg;
	
	int m_robot_level_days_cfg_num;
	ColosseumRobotDaysLevelCfg m_robot_days_level_cfg_list[MAX_DAYS_NUM];

	ColosseumBuffCfg m_buff_cfg;

	int m_robot_attr_adjust_cfg_num;
	ColosseumRobotAttrAdjustCfg m_robot_attr_adjust_list[MAX_ATTR_ADJUST_GROUP_NUM];

	std::map<int, ColosseumAchievementCfg> m_achievement_map;

	std::vector<ColosseumHistoryRankRewardCfg> m_history_rank_reward_cfg_vec;

	int m_total_rate;
	std::vector<ColossumBattleRewardCfg> m_battle_reward_cfg;
};

#endif