#ifndef __GUILD_WEEK_TASK_CONFIG_HPP__
#define __GUILD_WEEK_TASK_CONFIG_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"

#include <map>

struct GWTOtherInfo
{
	GWTOtherInfo() : reward_day(0), reward_time(0), daily_refresh_time(0), day_fin_fund_award(0) {}

	int reward_day;				// 周奖励发放日
	int reward_time;			// 周奖励发放时间
	int daily_refresh_time;		// 每日任务刷新时间
	int day_fin_fund_award;		// 每日完成家族任务的家族资金奖励
};

struct GWTTaskInfor
{
	GWTTaskInfor() : task_condition(0), param1(0), need_prog(0), contribution_per_times(0) {}

	int task_condition;				// 任务条件
	int param1;
	int need_prog;
	int min_level;					// 族长最低等级
	int contribution_per_times;		// 每完成一次获得的任务贡献度
};

struct GWTWeekAward
{
	GWTWeekAward() : guild_fund(0), coin_award(0), exp_award(0), guild_contribution(0) {}

	int guild_fund;					// 对家族：资金奖励

	int coin_award;					// 对玩家：铜币奖励
	int exp_award;					// 对玩家：经验奖励
	int guild_contribution;			// 对玩家：贡献奖励
	ItemConfigData item_award;		// 对玩家：道具奖励
};

struct GWTDayRankReawardCfg //日贡献排行榜奖励
{
	GWTDayRankReawardCfg()
	{
		min_rank = 0;
		max_rank = 0;
		guild_gong_xian = 0;
	}

	int min_rank;
	int max_rank;
	int guild_gong_xian;
};


struct GWTWeekRankReawardCfg //周贡献排行榜奖励
{
	GWTWeekRankReawardCfg()
	{
		min_rank = 0;
		max_rank = 0;
	}

	int min_rank;
	int max_rank;
	std::vector<ItemConfigData> reward;
};

class GuildWeekTaskConfig : public ILogicConfig
{
public:
	GuildWeekTaskConfig();
	virtual ~GuildWeekTaskConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	int RandGuildWeekTask(int patriarch_level);

	const GWTOtherInfo & GetGWTOtherCfg() { return m_gwt_other_cfg; }
	const GWTTaskInfor * GetGWTTaskInforCfg(int task_id);
	const GWTWeekAward * GetGWTWeekAwardCfg(int finish_times);
	const ItemConfigData * GetContributeAward(int contribution_value);
	const GWTDayRankReawardCfg * GetGWTDayRankReawardCfg(int rank);
	const GWTWeekRankReawardCfg * GetGWTWeekRankReawardCfg(int rank);
private:
	int InitGWTOtherInfoCfg(TiXmlElement * RootElement);
	int InitGWTTaskInforCfg(TiXmlElement * RootElement);
	int InitGWTWeekAwardCfg(TiXmlElement * RootElement);
	int InitGWTContAwardCfg(TiXmlElement * RootElement);
	int InitGWTDayContRankRewardCfg(TiXmlElement * RootElement);
	int InitGWTWeekContRankRewardCfg(TiXmlElement * RootElement);

	bool IsTimerCfg(int check_time);

	GWTOtherInfo m_gwt_other_cfg;
	std::map<int, GWTTaskInfor> m_gwt_task_infor_map;
	std::map<int, GWTWeekAward> m_gwt_week_award_map;
	std::map<int, ItemConfigData> m_gwt_contribute_award_map;

	std::vector<GWTDayRankReawardCfg> m_gwt_day_rank_reward_cfg;
	std::vector<GWTWeekRankReawardCfg> m_gwt_week_rank_reward_cfg;
};

#endif