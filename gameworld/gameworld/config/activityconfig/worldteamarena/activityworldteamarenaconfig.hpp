#ifndef __ACTIVITY_WORLD_TEAM_ARENA_CONFIG_HPP__
#define __ACTIVITY_WORLD_TEAM_ARENA_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/activitydef.hpp"

struct ActivityWorldTeamArenaOtherCfg
{
	ActivityWorldTeamArenaOtherCfg() : need_level(0), scene_id(0), max_fail_num(0), max_rounds(0), win_score(0), lose_score(0), draw_score(0), refresh_time(0),
		countdown(0), registration_type(0), registration_price(0), battle_protect(0)
	{}

	int need_level;				//角色等级/世界等级
	int scene_id;				//活动场景ID
	int max_fail_num;			//最大战斗失败次数
	int max_rounds;				//最大战斗回合数
	int win_score;				//胜利获得积分
	int lose_score;				//失败获得积分
	int draw_score;				//平局获得积分
	int refresh_time;			//排名刷新时间(秒)
	int countdown;				//进场倒计时(秒)
	int registration_type;		//报名费货币类型
	int registration_price;		//报名价格
	int battle_protect;			//战斗保护时间
};

struct ActivityWorldTeamArenaParRewardCfg
{
	ActivityWorldTeamArenaParRewardCfg() : exp(0), coin(0)
	{}

	int exp;
	int coin;
	std::vector<ItemConfigData> reward_list;
};

struct ActivityWorldTeamArenaRankRewardCfg
{
	ActivityWorldTeamArenaRankRewardCfg() : coin(0), title_id(0)
	{}

	int coin;
	int title_id;
	std::vector<ItemConfigData> reward_list;
};

struct ActivityWorldTeamArenaLevelRankRewardCfg
{
	ActivityWorldTeamArenaLevelRankRewardCfg() : min_role_level(0), max_role_level(0)
	{}

	int min_role_level;
	int max_role_level;
	std::map<ParamSection, ActivityWorldTeamArenaRankRewardCfg> rank_reward;
};

class ActivityWorldTeamArenaConfig : public ILogicConfig
{
public:
	ActivityWorldTeamArenaConfig() {}
	virtual ~ActivityWorldTeamArenaConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const ActivityWorldTeamArenaOtherCfg & GetOtherCfg() { return m_other_cfg; }
	const ActivityWorldTeamArenaParRewardCfg * GetParRewardCfg(int level);
	const ActivityWorldTeamArenaRankRewardCfg * GetRankRewardCfg(int rank, int role_level);
	Posi GetRandBornPosi();
	const std::map<int, int> & GetRankTitleListCfg() { return m_rank_reward_title_list_cfg; }

private:
	int InitOtherCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitParticicpationCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitRankAwardCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitBornPosiCfg(TiXmlElement * RootElement, std::string& errormsg);

	ActivityWorldTeamArenaOtherCfg m_other_cfg;
	std::map<ParamSection, ActivityWorldTeamArenaParRewardCfg> m_particicpation_map;	// 参与奖励：    键 - 等级区间
	std::vector<ActivityWorldTeamArenaLevelRankRewardCfg> m_rank_award_cfg;				// 活动排名奖励：	键 - 排名区间
	std::vector<Posi> m_born_list;														// 出生点位

	std::map<int, int> m_rank_reward_title_list_cfg;									// 存储排名奖励的称号
};

#endif