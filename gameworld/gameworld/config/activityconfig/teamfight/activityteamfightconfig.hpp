#ifndef __ACTIVITY_TEAM_FIGHT_CONFIG__
#define __ACTIVITY_TEAM_FIGHT_CONFIG__

#include "servercommon/configcommon.h"
#include "servercommon/maildef.hpp"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/commondata.hpp"
#include <vector>
#include <map>

static const int ACT_TEAM_FIGHT_RANK_MAX_NUM = 200;
static const int ACT_TEAM_FIGHT_REWARD_GROUP_NUM = 20;
static const int ACT_TEAM_FIGHT_MATCH_RULE_NUM = 5;

struct ActTeamFightOtherCfg
{
	ActTeamFightOtherCfg() :scene_id(0), level(0), pos(0, 0), start_battle_countdown(0), limit_max_round(0)
	{}

	int scene_id;
	int level;
	Posi pos;
	int start_battle_countdown;
	int limit_max_round;
};

struct ActTeamFightRankCfg
{
	ActTeamFightRankCfg() : min_rank(0), max_rank(0), exp(0), bind_coin(0), title_id(0) {}

	int min_rank;
	int max_rank;
	int exp;
	int bind_coin;
	std::vector<ItemConfigData> reward_list;
	int title_id;
};

enum ACT_TEAM_FIGHT_BOX_TYPE
{
	ACT_TEAM_FIGHT_BOX_TYPE_FIRST_WIN = 0,		// 首胜宝箱
	ACT_TEAM_FIGHT_BOX_TYPE_THIRD_BATTLE = 1,	// 三战宝箱
};

struct ActTeamFightBoxCfg
{
	ActTeamFightBoxCfg() : exp(0), coin(0), min_level(0), max_level(0)
	{
		memset(mail_txt, 0, sizeof(mail_txt));
	}

	ItemConfigData item;
	int exp;
	int coin;
	int min_level;
	int max_level;
	char mail_txt[2048];
};

struct ActTeamFightRewardGroupCfg
{
	ActTeamFightRewardGroupCfg() : group_id(0), min_level(0), max_level(0) {}

	int group_id;
	int min_level;
	int max_level;
	ItemConfigData item;
};

struct ActTeamFightMatchRule
{
	ActTeamFightMatchRule() : match_sec_min(0), match_sec_max(0), average_level_diff(0) {}

	int match_sec_min;			//等待秒数下限
	int match_sec_max;			//等待秒数上限
	int average_level_diff;		//平均等级的差值
};

struct TeamFightJoinAwardCfg
{
	TeamFightJoinAwardCfg() : min_level(0), max_level(0), exp(0), coin(0)
	{}

	int min_level;
	int max_level;
	int exp;
	int coin;
	std::vector<ItemConfigData> reward_list;
};

struct TeamFightFindRewardGroupCfg
{
	TeamFightFindRewardGroupCfg() :rate(0)
	{}

	int rate;
	ItemConfigData reward;
};

struct TeamFightFindCfg
{
	TeamFightFindCfg() :min_level(0), max_level(0), exp(0), coin(0), family_gongxian(0), group_id(0)
	{}

	int min_level;
	int max_level;
	int exp;
	int coin;
	int family_gongxian;
	int group_id;
};

struct TeamFightFindGroupCfg
{
	TeamFightFindGroupCfg()
	{}

	std::vector<TeamFightFindCfg> node_list;
};

class Role;
class ActivityTeamFightConfig : public ILogicConfig
{
public:
	typedef std::vector<ActTeamFightRewardGroupCfg> RewardGroupVec;

public:
	ActivityTeamFightConfig();
	virtual ~ActivityTeamFightConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const ActTeamFightOtherCfg* GetOtherCfg() const { return &m_other_cfg; }
	const ActTeamFightRankCfg* GetRankCfg(int rank, int role_level)const;
	int GetMaxRank() const { return m_max_rank; }

	bool SendBoxReward(Role * role, int type, int role_level)const;
	const ActTeamFightBoxCfg * GetBoxReward(int type, int role_level) const;
	bool MailReissueBoxReward(int box_type, int role_level, int uid) const;
	
	const ActTeamFightRewardGroupCfg* GetRewardGroupCfg(int group_id, int role_level) const;
	int GetTeamMateLevelDiffByWaitSeconds(int wait_sec);
	int GetOpponentLevelDiffByWaitSeconds(int wait_sec);

	const TeamFightJoinAwardCfg * GetTeamFightJoinAwardCfg(int role_level);

	const TeamFightFindCfg * GetFindCfg(int find_type, int role_level)const;
	const TeamFightFindRewardGroupCfg * GetFindRewardGroupCfg(int group_id)const;
private:
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitRankCfg(TiXmlElement * RootElement);
	int InitBoxCfg(TiXmlElement * RootElement);
	int InitRewardGroup(TiXmlElement * RootElement);
	int InitTeamMateMatchRule(TiXmlElement * RootElement);
	int InitEnemyMatchRule(TiXmlElement * RootElement);
	int InitTeamFightJoinAwardCfg(TiXmlElement * RootElement);
	int InitTeamFightFindAwardGroupCfg(TiXmlElement * RootElement);
	int InitTeamFightFindAwardCfg(TiXmlElement * RootElement);

	ActTeamFightOtherCfg m_other_cfg;
	int m_max_rank;
	std::map<int, std::vector<ActTeamFightRankCfg> > m_rank_cfg;		// 排行奖励 key:min_level * 1000 + max_level
	std::map<int, std::vector<ActTeamFightBoxCfg> > m_box_cfg;

	RewardGroupVec m_reward_group_vec_list[ACT_TEAM_FIGHT_REWARD_GROUP_NUM];

	int m_match_teammate_rule_num;
	ActTeamFightMatchRule m_match_teammate_rule_cfg_list[ACT_TEAM_FIGHT_MATCH_RULE_NUM];

	int m_match_enemy_rule_num;
	ActTeamFightMatchRule m_match_enemy_rule_cfg_list[ACT_TEAM_FIGHT_MATCH_RULE_NUM];

	std::vector<TeamFightJoinAwardCfg> m_join_award_vec;								// 参与奖励
	std::map<int, std::vector<TeamFightFindRewardGroupCfg> > m_find_reward_group_map;	// 找回道具组:key - group_id
	TeamFightFindGroupCfg m_find_cfg[RB_TYPE_MAX];										// 奖励找回

};

#endif // !__ACTIVITY_TEAM_FIGHT_CONFIG__