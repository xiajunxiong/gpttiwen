#ifndef __ACTIVITY_CHIEF_PEAK_BATTLE_CONFIG__
#define __ACTIVITY_CHIEF_PEAK_BATTLE_CONFIG__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/activitydef.hpp"

#include <set>
#include <map>
#include <vector>

struct ChiefPeakBattleRankRewardCfg
{
	ChiefPeakBattleRankRewardCfg()
	{
		round = 0;
		title_id = 0;
		coin = 0;
	}

	int round;
	int title_id;
	int coin ;
	std::vector<ItemConfigData> reward_list;
};

struct ChiefPeakBattleBetCfg
{
	ChiefPeakBattleBetCfg()
	{
		bet_seq = 0;
		bet_coin = 0;
		win_coin = 0;
	}

	int bet_seq;
	int bet_coin;
	int win_coin;
	
};

struct ChiefPeakBattleReadySceneCfg
{
	ChiefPeakBattleReadySceneCfg()
	{
		scene_id = 0;
	}

	int scene_id;
};

struct ChiefPeakBattleOtherCfg
{
	ChiefPeakBattleOtherCfg()
	{
		max_round = 0;
		first_ready_time = 60;
		ready_time = 30;
	}

	int max_round;
	int first_ready_time;
	int ready_time;
};

struct ActivityChiefPeakBattleBattleRewardCfg  //√ø≥°’Ω∂∑Ω±¿¯≈‰÷√
{
	ActivityChiefPeakBattleBattleRewardCfg()
	{
		min_role_level = 0;
		max_role_level = 0;

		winner_exp = 0;
		winner_coin = 0;
		loser_exp = 0;
		loser_coin = 0;
	}

	int min_role_level;
	int max_role_level;

	int winner_exp;
	int winner_coin;
	int loser_exp;
	int loser_coin;
};


class ActivityChiefPeakBattleConfig : public ILogicConfig
{
public:
	ActivityChiefPeakBattleConfig();
	virtual ~ActivityChiefPeakBattleConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const ChiefPeakBattleRankRewardCfg * GetChiefPeakBattleRewardCfg(int round);
	const ChiefPeakBattleBetCfg * GetChiefPeakBattleBetCfg(int bet_seq);
	int GetReadySceneId(int round);
	const ChiefPeakBattleOtherCfg & GetOhterCfg();
	const ActivityChiefPeakBattleBattleRewardCfg * GetPerBattleRewardCfg(int role_level);
	const std::set<int> * GetChiefPeakBattleRankTitleList() { return &m_chief_peak_battle_rank_title_set; }
private:
	int InitSceneCfg(TiXmlElement *RootElement);
	int InitBetCfg(TiXmlElement *RootElement);
	int InitRandRewardCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitBattleRewardCfg(TiXmlElement * RootElement);

	std::vector<ChiefPeakBattleReadySceneCfg> m_ready_scene_cfg;
	std::vector<ChiefPeakBattleBetCfg> m_bet_cfg;
	std::vector<ChiefPeakBattleRankRewardCfg> m_rank_cfg;
	std::set<int> m_chief_peak_battle_rank_title_set;
	ChiefPeakBattleOtherCfg m_other_cfg;
	std::vector<ActivityChiefPeakBattleBattleRewardCfg> m_battle_reward_cfg; //√ø≥°’Ω∂∑Ω±¿¯≈‰÷√
};

#endif // !__ACTIVITY_MAZE_RACER_CONFIG__