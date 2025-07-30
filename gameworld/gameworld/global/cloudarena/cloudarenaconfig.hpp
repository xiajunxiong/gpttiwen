#ifndef __CLOUD_ARENA_CONFIG_HPP__
#define __CLOUD_ARENA_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/cloudarenadef.hpp"
#include "servercommon/struct/itemlistparam.h"

static const int CLOUD_ARENA_ROBOT_ATTR_ADJUST_PERCENT = 10000;
static const int CLOUD_ARENA_ROBOT_ATTR_ADJUST_GROUP_NUM = 4;

struct CloudArenaFloatFactorCfg
{
	CloudArenaFloatFactorCfg() : refresh_value(0), fail_value(0), win_value(0) {}

	int refresh_value;
	int fail_value;
	int win_value;
};

struct CloudArenaMatchScoreFactorCfg
{
	CloudArenaMatchScoreFactorCfg()
	{
		memset(position_factor_list, 0, sizeof(position_factor_list));
	}

	int position_factor_list[CLOUD_ARENA_OPPONENT_NUM];
};

struct CloudArenaOtherCfg
{
	CloudArenaOtherCfg() : season_init_fight_times(0), max_store_fight_times(0), each_fight_consume_times(0), 
		initial_score(0), min_score(0), max_round(0), level_limit(0), refresh_opponent_cd_s(0), manual_refresh_times_limit(0),
		match_factor_range(0), match_wave_threshold(0), boundry_rank_wave_value(0), 
		season_reduce_score(0), allow_surrender_round(0) {}

	int season_init_fight_times;	// 赛季初始挑战次数
	int max_store_fight_times;		// 挑战次数保存上限
	int each_fight_consume_times;	// 每次挑战消耗次数
	int initial_score;				// 赛季初始积分
	int min_score;					// 保底积分
	int max_round;					// 战斗最大回合数
	int level_limit;				// 等级限制
	int refresh_opponent_cd_s;		// 手动刷新对手CD(s)
	int manual_refresh_times_limit;	// 手动刷新次数限制
	int match_factor_range;			// 匹配变量
	int match_wave_threshold;		// 匹配浮动阈值
	int boundry_rank_wave_value;	// 边界浮动系数
	int season_reduce_score;		// 赛季减分
	int max_season_initial_score;	// 最高赛季初始分
	int allow_surrender_round;		// 允许投降的回合数
};

struct CloudArenaBuffCfg
{
	CloudArenaBuffCfg() : buff_id(0), rate(0) {}

	int buff_id;
	int rate;
};

struct CloudArenaBuffGroupCfg
{
	static const int MAX_BUFF_NUM = 10;

	CloudArenaBuffGroupCfg() : group_id(0), total_rate(0), buff_num(0) {}

	int group_id;
	int total_rate;
	int buff_num;
	CloudArenaBuffCfg buff_list[MAX_BUFF_NUM];
};

struct CloudArenaRobotCfg
{
	CloudArenaRobotCfg() : id(0), score(0), pet_helper_num(0), 
		world_level_diff(0), level_start(0)
	{
		memset(attr_adjust_group_list, 0, sizeof(attr_adjust_group_list));
	}

	int id;
	int score;
	int attr_adjust_group_list[CLOUD_ARENA_ROBOT_ATTR_ADJUST_GROUP_NUM];
	int pet_helper_num;
	int world_level_diff;
	int level_start;
};

struct CloudArenaRobotAttrAdjustCfg
{
	CloudArenaRobotAttrAdjustCfg() : group_id(0), role_attack_adjust(0), role_mental_adjust(0), role_recover_adjust(0),
		role_maxhp_adjust(0), pet_attack_adjust(0), pet_mental_adjust(0), pet_recover_adjust(0),
		pet_maxhp_adjust(0), partner_attack_adjust(0), partner_mental_adjust(0), partner_recover_adjust(0),
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

struct CloudArenaRankCfg
{
	CloudArenaRankCfg() : rank_id(0), rank_score(0), rank_dan_type(0), rank_reward_num(0), rank_reward_arena_money(0),
		day_reward_arena_money(0), day_reward_bind_coin(0), score_level(0), season_min_score(0){}

	static const int REWARD_NUM = 5;

	int rank_id;
	int rank_score;
	int rank_dan_type;
	int rank_reward_num;
	ItemConfigData rank_reward_list[REWARD_NUM];
	int rank_reward_arena_money;
	int day_reward_arena_money;
	int day_reward_bind_coin;
	int score_level;
	int season_min_score;
};

struct CloudArenaScoreCfg
{
	CloudArenaScoreCfg() : win_add_score(0), lose_add_score(0), draw_add_score(0), 
		win_add_money(0), lose_add_money(0), draw_add_money(0) {}

	int win_add_score;
	int lose_add_score;
	int draw_add_score;

	int win_add_money;
	int lose_add_money;
	int draw_add_money;
};

struct CloudArenaSeasonRewardCfg
{
	static const int REWARD_NUM = 5;

	CloudArenaSeasonRewardCfg() : min_rank(0), max_rank(0), title_id(0), reward_num(0) {}

	int min_rank;
	int max_rank;
	int title_id;
	int reward_num;
	ItemConfigData reward_list[REWARD_NUM];
};

struct CloudArenaAchievementCfg
{
	static const int REWARD_NUM = 5;

	CloudArenaAchievementCfg() : is_hide(false), progress_need(0), reward_arena_coin(0), reward_num(0) {}

	bool is_hide;
	int progress_need;
	int reward_arena_coin;
	int reward_num;
	ItemConfigData reward_list[REWARD_NUM];
};

struct CloudArenaWorldLevelAttrAdjustGroupHash
{
	CloudArenaWorldLevelAttrAdjustGroupHash() : level_min(0), level_max(0), group_index(0) {}

	int level_min;
	int level_max;
	int group_index;
};

class CloudArenaConfig : public ILogicConfig
{
public:
	static const int MAX_REFRESH_COST_CFG_NUM = 302;
	static const int MAX_BUFF_GROUP_NUM = 5;
	static const int MAX_ROBOT_CFG_NUM = 302;
	static const int MAX_ATTR_ADJUST_GROUP_NUM = 302;
	static const int MAX_RANK_CFG_NUM = 30;
	static const int MAX_SCORE_CFG_NUM = 5;
	static const int MAX_SEASON_REWARD_CFG_NUM = 12;

	typedef std::vector<CloudArenaAchievementCfg> AchievementCfgVec;

public:
	CloudArenaConfig();
	virtual ~CloudArenaConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const CloudArenaOtherCfg& GetOtherCfg() const;
	int GetPositionFactor(int opponent_idx);
	void CalculateMatchFactor(int opponent_idx, bool last_fight_win, bool last_fight_fail, int manual_refresh_times, ARG_OUT int* low_match_factor, ARG_OUT int* high_match_factor);
	
	bool RandomDefenseBuff(ARG_OUT int* buff_id);
	const CloudArenaBuffGroupCfg* GetBuffGroupCfg(int group_id);
	const CloudArenaRobotCfg* GetRobotCfg(int id);
	const CloudArenaRobotAttrAdjustCfg* GetRobotAttrAdjustCfg(int group_id);
	inline int GetRobotNum() const { return m_robot_cfg_num; }
	const CloudArenaRankCfg* GetRankCfg(int score);
	const CloudArenaRankCfg* GetRankCfgByRankId(int rank_id);
	const CloudArenaScoreCfg* GetScoreCfg(int target_seq);
	void GetScoreAndArenaCoinReward(int target_seq, bool is_win, bool is_draw, ARG_OUT int* modify_score, ARG_OUT int* modify_arena_coin);
	int GetDanTypeByScore(int score);
	int GetDanIdByScore(int score);
	int GetSeasonReduceToScore(int last_season_score);
	bool IsHighestDanId(int dan_id);
	const CloudArenaSeasonRewardCfg* GetSeasonRewardCfg(int rank);
	const CloudArenaAchievementCfg* GetAchievementCfg(int achieve_type, int achieve_level);

	int GetRobotAttrAdjustGroupIndexByWorldLevel(int world_level);
private:
	int InitOtherCfg(TiXmlElement* RootElement);
	int InitFloatFactorCfg(TiXmlElement* RootElement);
	int InitMatchScoreFactorCfg(TiXmlElement* RootElement);
	int InitBuffGroupCfg(TiXmlElement* RootElement);
	int InitRobotCfg(TiXmlElement* RootElement);
	int InitRobotAttrAdjustCfg(TiXmlElement* RootElement);
	int InitRankingCfg(TiXmlElement* RootElement);
	int InitScoreCfg(TiXmlElement* RootElement);
	int InitSeasonRewardCfg(TiXmlElement* RootElement);
	int InitAchievementCfg(TiXmlElement* RootElement);
	int InitWorldLevelAttrAdjustGroupHash(TiXmlElement* RootElement);

	CloudArenaBuffGroupCfg* MutableBuffGroupCfg(int group_id);
	
	CloudArenaOtherCfg m_other_cfg;
	CloudArenaFloatFactorCfg m_float_factor_cfg;
	CloudArenaMatchScoreFactorCfg m_match_score_factor_cfg;

	int m_refresh_cost_cfg_num;
	int m_refresh_cost_cfg_list[MAX_REFRESH_COST_CFG_NUM];

	int m_buff_group_num;
	CloudArenaBuffGroupCfg m_buff_group_list[MAX_BUFF_GROUP_NUM];

	int m_robot_cfg_num;
	CloudArenaRobotCfg m_robot_cfg_list[MAX_ROBOT_CFG_NUM];

	int m_robot_attr_adjust_cfg_num;
	CloudArenaRobotAttrAdjustCfg m_robot_attr_adjust_list[MAX_ATTR_ADJUST_GROUP_NUM];

	int m_rank_cfg_num;
	CloudArenaRankCfg m_rank_cfg_list[MAX_RANK_CFG_NUM];

	int m_score_cfg_num;
	CloudArenaScoreCfg m_score_cfg_list[MAX_SCORE_CFG_NUM];

	int m_season_reward_cfg_num;
	CloudArenaSeasonRewardCfg m_season_reward_cfg_list[MAX_SEASON_REWARD_CFG_NUM];

	AchievementCfgVec m_achieve_cfg_list[CLOUD_ARENA_ACHIEVEMENT_MAX];

	std::vector<CloudArenaWorldLevelAttrAdjustGroupHash> m_world_level_attr_adjust_group_hash;
};


#endif