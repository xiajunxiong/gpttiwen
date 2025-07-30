#ifndef __ACTIVITY_HUAN_JIE_ZHAN_CHANG_CONFIG__
#define __ACTIVITY_HUAN_JIE_ZHAN_CHANG_CONFIG__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/activitydef.hpp"
#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/attributedef.hpp"

#include <set>
#include <map>
#include <vector>

struct ActivityHuanJieZhanChangOtherCfg
{
	ActivityHuanJieZhanChangOtherCfg()
	{
		max_battle_rounds = 0;
		base_score = 0;
		win_add_score = 0;
		lose_add_score = 0;
		win_streak_add_score = 0;
		win_streak_times = 0;
		win_add_huan_jie_coin = 0;
		lose_add_huan_jie_coin = 0;
		draw_add_huan_jie_coin = 0;
		ding_ji_sai_times = 0;
		ding_ji_win_add_score = 0;
		ding_ji_lose_add_score = 0;
		today_huan_jie_coin_limit = 0;
		rank_protect = 0;
		level_limit = 0;
	}

	int max_battle_rounds;
	int base_score;
	int win_add_score;
	int lose_add_score;
	int win_streak_add_score;
	int win_streak_times;
	int win_add_huan_jie_coin;
	int lose_add_huan_jie_coin;
	int draw_add_huan_jie_coin;
	int ding_ji_sai_times;
	int ding_ji_win_add_score;
	int ding_ji_lose_add_score;
	int today_huan_jie_coin_limit;
	int rank_protect;
	int rank_wave_section;
	int fluctuation_value_1;
	int fluctuation_value_2;
	int level_limit;
};

struct ActivityHuanJieZhanChangRankCfg
{
	ActivityHuanJieZhanChangRankCfg()
	{		
		need_rank_score = 0;
		season_reduce_score = 0;
		rank_reward_huan_jie_coin = 0;
		rank = 0;
	}

	int need_rank_score;
	int season_reduce_score;
	int rank_reward_huan_jie_coin;
	int rank;			//段位
	std::vector<ItemConfigData> rank_reward;
	std::vector<ItemConfigData> settlement_rank_reward;
};

struct ActivityHuanJieZhanChangRankingListCfg
{
	ActivityHuanJieZhanChangRankingListCfg()
	{
		min_ranking_pos = 0;
		max_ranking_pos = 0;
		need_rank_score = 0;
		title_id = 0;
	}

	int min_ranking_pos;				//排行榜最小名次
	int max_ranking_pos;
	int need_rank_score;
	int title_id;
	std::vector<ItemConfigData> ranking_reward;
};

enum HUAN_JIE_HUA_SHEN_ATTR_LIMIT_TYPE
{
	HUAN_JIE_HUA_SHEN_ATTR_LIMIT_TYPE_EQUIPMENT = 0,		//装备
	HUAN_JIE_HUA_SHEN_ATTR_LIMIT_TYPE_OTHER = 1,			//其它系统
	HUAN_JIE_HUA_SHEN_ATTR_LIMIT_TYPE_PET = 2,				//宠物

	HUAN_JIE_HUA_SHEN_ATTR_LIMIT_TYPE_MAX
};

enum HUAN_JIE_BIG_RANK_TYPE
{
	HUAN_JIE_BIG_RANK_TYPE_INVALID = 0,
	HUAN_JIE_BIG_RANK_TYPE_BRONZE = 1,
	HUAN_JIE_BIG_RANK_TYPE_SILVER = 2,
	HUAN_JIE_BIG_RANK_TYPE_GOLD = 3,
	HUAN_JIE_BIG_RANK_TYPE_PLATINUM = 4,
	HUAN_JIE_BIG_RANK_TYPE_DIAMOND = 5,
	HUAN_JIE_BIG_RANK_TYPE_KING = 6,
	HUAN_JIE_BIG_RANK_TYPE_MAX
};

static const int HZJC_ATTR_LIMIT_MAX_NUM = 10000;
static const int HZJC_ATTR_JOB_LIMIT_MAX_NUM = 100;
static const int HJZC_EQUIP_ATTR_LIMIT_MAX_NUM = 23;
UNSTD_STATIC_CHECK(HJZC_EQUIP_ATTR_LIMIT_MAX_NUM <= BATTLE_ATTR_MAX);
struct ActivityHuanJieZhanChangEquipAttrLimitCfg
{
	ActivityHuanJieZhanChangEquipAttrLimitCfg()
	{
		memset(attr_limit_list, 0, sizeof(attr_limit_list));
	}
	
	struct AttrConCfg
	{
		AttrConCfg() : attr_value(0), attr_type(0) {}

		int attr_value;
		short attr_type;
	};

	AttrConCfg attr_limit_list[HJZC_EQUIP_ATTR_LIMIT_MAX_NUM];
};

enum HUAN_JIE_HUA_SHEN_ROBOT_ATTR_LIMIT_TYPE
{
	HUAN_JIE_HUA_SHEN_ROBOT_ATTR_LIMIT_TYPE_ROLE = 1,		//角色
	HUAN_JIE_HUA_SHEN_ROBOT_ATTR_LIMIT_TYPE_PET = 2,				//宠物
};

struct ActivityHuanJieZhanChangRobotAttrLimitCfgKey
{
	ActivityHuanJieZhanChangRobotAttrLimitCfgKey()
	{
		type = 0;
		p1 = 0;
	}

	bool operator<(const ActivityHuanJieZhanChangRobotAttrLimitCfgKey & other) const
	{
		if (type < other.type)
			return true;
		if (type > other.type)
			return false;

		if (p1 < other.p1)
			return true;
		if (p1 > other.p1)
			return false;

		return false;
	}

	int type;
	int p1;
};

struct ActivityHuanJieZhanChangRobotAttrLimitCfg
{
	ActivityHuanJieZhanChangRobotAttrLimitCfg()
	{
		type = 0;
		p1 = 0;
	}

	int type;
	int p1;

	std::vector<AttrItemCfg> max_attr_cfg;
};

struct HuanJieZhanChangRobotCfg
{
	HuanJieZhanChangRobotCfg() : id(0), min_score(0), max_score(0), level(0),  attr_adjust_group(0) {}

	int id;
	int min_score;
	int max_score;
	int level;
	int attr_adjust_group;
};

struct HuanJieZhanChangRobotAttrAdjustCfg
{
	HuanJieZhanChangRobotAttrAdjustCfg() : group_id(0), role_attack_adjust(0), role_mental_adjust(0), role_recover_adjust(0),
		role_maxhp_adjust(0), pet_attack_adjust(0), pet_mental_adjust(0), pet_recover_adjust(0),pet_maxhp_adjust(0)
	{
	}

	int group_id;
	int role_attack_adjust;
	int role_mental_adjust;
	int role_recover_adjust;
	int role_maxhp_adjust;
	int pet_attack_adjust;
	int pet_mental_adjust;
	int pet_recover_adjust;
	int pet_maxhp_adjust;
};

struct HuanJieRankScoreSection
{
	HuanJieRankScoreSection() : rank_min_score(0), rank_max_score(0) {}

	int rank_min_score;
	int rank_max_score;
};

struct HuanJieTodayWinRewardCfg
{
	int seq;
	int need_win_times;

	//奖励
	int exp;
	int coin;
	int huan_jie_coin;
	std::vector<ItemConfigData> rewards;
};


class  ActivityHuanJieZhanChangConfig : public ILogicConfig
{
public:
	ActivityHuanJieZhanChangConfig();
	virtual ~ActivityHuanJieZhanChangConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const ActivityHuanJieZhanChangOtherCfg & GetOtherCfg();
	const std::vector<ActivityHuanJieZhanChangRankCfg> & GetAllRankCfg();
	const  ActivityHuanJieZhanChangRankCfg * GetActivityHuanJieZhanChangRankCfg(int score);
	const  ActivityHuanJieZhanChangRankingListCfg * GetActivityHuanJieZhanChangRankingListCfg(int rank_pos);
	int GetActivityHuanJieZhanChangRankingTitle(int rank_pos);
	bool IsRankProtect(int score);
	int GetExtraScore(int score, int opponent_score);
	const ActivityHuanJieZhanChangEquipAttrLimitCfg * GetEquipAttrLimitCfg(int limit_type, int app_prof_type, int main_attr_type);
	const ActivityHuanJieZhanChangRobotAttrLimitCfg * GetActivityHuanJieZhanChangRobotAttrLimitCfg(int limit_type, int p1);

	const HuanJieZhanChangRobotCfg * GetHuanJieRobotCfg(int rank_score);
	const HuanJieZhanChangRobotAttrAdjustCfg * GetHuanJieRobotAttrAdjustCfg(int group_id);
	int GetBigRankType(int rank_score);
	const HuanJieTodayWinRewardCfg * GetHuanJieTodayWinRewardCfg(int seq);
	std::vector< HuanJieTodayWinRewardCfg > GetHuanJieAllTodayWinRewardCfg();
private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitRankCfg(TiXmlElement *RootElement);
	int InitRankingListCfg(TiXmlElement * RootElement);
	int InitEquipAttrLimitListCfg(TiXmlElement * RootElement);
	int InitRobotAttrLimitListCfg(TiXmlElement * RootElement);
	int InitRobotAttrAdjustCfg(TiXmlElement * RootElement);
	int InitRobotCfg(TiXmlElement * RootElement);
	int InitTodayRewardCfg(TiXmlElement * RootElement);

	int GetAttrLimitKey(int limit_type, int job_type, int main_attr_type);

	ActivityHuanJieZhanChangOtherCfg m_other_cfg;
	std::vector<ActivityHuanJieZhanChangRankCfg> m_rank_cfg;
	std::vector<ActivityHuanJieZhanChangRankingListCfg> m_ranking_list_cfg;
	std::map<int, ActivityHuanJieZhanChangEquipAttrLimitCfg> m_attr_limit_cfg;			//属性限制 key:GetAttrLimitKey
	std::map<ActivityHuanJieZhanChangRobotAttrLimitCfgKey, ActivityHuanJieZhanChangRobotAttrLimitCfg> m_robot_attr_limit_cfg;			//属性限制 key:GetAttrLimitKey

	std::vector<HuanJieZhanChangRobotCfg> m_huan_jie_robot_cfg;
	std::map<int, HuanJieZhanChangRobotAttrAdjustCfg>  m_huan_jie_robot_adjust_cfg;

	HuanJieRankScoreSection m_rank_least_score_list[HUAN_JIE_BIG_RANK_TYPE_MAX];

	std::map<int, HuanJieTodayWinRewardCfg>  m_huan_jie_today_win_reward_cfg;
};

#endif 