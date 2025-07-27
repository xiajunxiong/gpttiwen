#ifndef __UTA_CONFIG_HPP__
#define __UTA_CONFIG_HPP__

#include <vector>
#include "servercommon/struct/itemlistparam.h"

struct UTAOtherCfg
{
	UTAOtherCfg() : scene_id(0), enter_scene_count_down_s(0), max_rounds(0), win_score(0),
		lose_score(0), draw_score(0), give_up_score(0), need_active(0), level_limit(0),
		icon_keep_time_s(0){}

	int scene_id;
	int enter_scene_count_down_s;
	int max_rounds;
	int win_score;
	int lose_score;
	int draw_score;
	int give_up_score;
	std::vector<ItemConfigData> qualification_join_rewards;

	int need_active;
	std::vector<ItemConfigData> active_rewards;

	int level_limit;
	int icon_keep_time_s;
};

struct UTAWinBoxRewardCfg
{
	UTAWinBoxRewardCfg() : seq(0), qualification_win_times(0) {}

	int seq;
	int qualification_win_times;
	std::vector<ItemConfigData> reward_list;
};

struct UTARankCfg
{
	UTARankCfg() : type(0), min_rank(0), max_rank(0), title_id(0), title_item_id(0){}

	enum TYPE
	{
		QUALIFICATION = 0,
		MAIN = 1
	};

	int type;		// 0 资格赛  1 正赛
	int min_rank;
	int max_rank;	// [min_rank, max_rank)
	int title_id;
	ItemID title_item_id;
	std::vector<ItemConfigData> reward_list;
	std::vector<ItemDataWrapper> server_reward_list;
};

struct UTAAdvanceRewardCfg
{
	UTAAdvanceRewardCfg() : type(0), stage(0) {}

	enum TYPE
	{
		WIN_GROUP = 0,		//胜者组
		LOSE_GROUP = 1		//败者组
	};

	int type;
	int stage;
	std::string stage_name;
	std::vector<ItemConfigData> reward_list;
};

class UTAConfig
{
public:
	typedef std::vector<UTAWinBoxRewardCfg> WinBoxRewardCfgList;
	typedef std::vector<UTARankCfg> RankRewardCfgList;
	typedef std::vector<UTAAdvanceRewardCfg> AdvanceRewardCfgList;
	typedef std::vector<int> InitialScoreVec;
	friend class UTAConfigChecker;

public:
	static UTAConfig& Instance();

	bool Init(const std::string &configname, std::string *err);
	static bool Reload(const std::string &configname, std::string *err);

	const UTAWinBoxRewardCfg* GetWinBoxRewardCfg(int box_seq);
	std::vector<const UTAWinBoxRewardCfg*> GetAllWinBoxRewardCfg();
	const UTAOtherCfg& GetOtherCfg() const { return m_other_cfg; }
	const UTARankCfg* GetRankCfg(int type, int rank_index);
	const UTAAdvanceRewardCfg* GetAdvanceCfg(int type, int round_idx);
	int GetInitialScore(int world_team_arena_rank) const;
	int GetGiveGuessCoinNeedActive() const { return m_other_cfg.need_active; }

private:
	UTAConfig();
	~UTAConfig();

	//禁止拷贝复制（请不要公有化以下这两个函数， 除非做好深拷贝， 因为在析构时会delete掉某些成员变量）
	UTAConfig(const UTAConfig&);
	UTAConfig& operator=(const UTAConfig &);

	int InitOtherCfg(TiXmlElement* RootElement);
	int InitWinBoxRewardCfg(TiXmlElement* RootElement);
	int InitRankRewardCfg(TiXmlElement* RootElement);
	int InitAdvanceRewardCfg(TiXmlElement* RootElement);
	int InitInitialScoreCfg(TiXmlElement* RootElement);
private:
	UTAOtherCfg m_other_cfg;
	WinBoxRewardCfgList m_win_box_reward_cfg_list;
	RankRewardCfgList m_rank_reward_cfg_list;
	AdvanceRewardCfgList m_advance_reward_cfg_list;
	InitialScoreVec m_initial_score_vec;
};

#endif