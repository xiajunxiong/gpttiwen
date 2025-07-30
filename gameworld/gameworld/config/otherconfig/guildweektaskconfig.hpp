#ifndef __GUILD_WEEK_TASK_CONFIG_HPP__
#define __GUILD_WEEK_TASK_CONFIG_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"

#include <map>

struct GWTOtherInfo
{
	GWTOtherInfo() : reward_day(0), reward_time(0), daily_refresh_time(0), day_fin_fund_award(0) {}

	int reward_day;				// �ܽ���������
	int reward_time;			// �ܽ�������ʱ��
	int daily_refresh_time;		// ÿ������ˢ��ʱ��
	int day_fin_fund_award;		// ÿ����ɼ�������ļ����ʽ���
};

struct GWTTaskInfor
{
	GWTTaskInfor() : task_condition(0), param1(0), need_prog(0), contribution_per_times(0) {}

	int task_condition;				// ��������
	int param1;
	int need_prog;
	int min_level;					// �峤��͵ȼ�
	int contribution_per_times;		// ÿ���һ�λ�õ������׶�
};

struct GWTWeekAward
{
	GWTWeekAward() : guild_fund(0), coin_award(0), exp_award(0), guild_contribution(0) {}

	int guild_fund;					// �Լ��壺�ʽ���

	int coin_award;					// ����ң�ͭ�ҽ���
	int exp_award;					// ����ң����齱��
	int guild_contribution;			// ����ң����׽���
	ItemConfigData item_award;		// ����ң����߽���
};

struct GWTDayRankReawardCfg //�չ������а���
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


struct GWTWeekRankReawardCfg //�ܹ������а���
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