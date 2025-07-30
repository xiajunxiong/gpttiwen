#ifndef __RAND_ACTIVITY_GOD_TOKEN_CONFIG_HPP__
#define __RAND_ACTIVITY_GOD_TOKEN_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/ragodtokenparam.hpp"
#include "item/itembase.h"

static const int GOD_TOKEN_TIME_SEQ_MAXNUM = 5;
static const int GOD_TOKEN_LEVEL_SEGMENT = 5;

struct GodTokenLevelLimit
{
	GodTokenLevelLimit(int _level_min = 0, int _level_max = 0) : level_min(_level_min), level_max(_level_max) {}

	int level_min;
	int level_max;

	bool operator < (const GodTokenLevelLimit & tmp) const
	{
		return this->level_min < tmp.level_min;
	}
};

struct RAGodTokenOtherCfg
{
	RAGodTokenOtherCfg() :buy_exp(0), buy_token(0), max_leve(0), reclaim_item(0) {}

	int buy_exp;
	int buy_token;
	int max_leve;
	int reclaim_item;
};

struct RAGodTokenTaskCfg
{
	RAGodTokenTaskCfg() : seq(0), type(0), param(0), get_exp(0) {}

	int seq;
	int type;
	unsigned int param;
	int get_exp;
};
struct RAGodTokenTaskListCfg
{
	RAGodTokenTaskListCfg() : level_min(0), level_max(0) {}

	int level_min;
	int level_max;
	std::vector<RAGodTokenTaskCfg> task_list;
};

struct RAGodTokenLevelCfg
{
	RAGodTokenLevelCfg() :seq(0), need_exp(0), open_level(0), senior_gold(0){}

	int seq;
	int need_exp;
	int open_level;
	ItemConfigData common_item;
	std::vector<ItemConfigData> senior_item;
	int senior_gold;
};

struct RAGodTokenLevelSectionCfg
{
	RAGodTokenLevelSectionCfg() : open_level(0), end_level(0)
	{}

	int open_level;
	int end_level;
	std::vector<RAGodTokenLevelCfg> level_vec;
};

struct RAGodTokenExchangeCfg
{
	RAGodTokenExchangeCfg() :seq(0), daily_exchange(0), all_exchange(0) {}

	int seq;
	int daily_exchange;
	int all_exchange;
	int open_level;
	int	end_level;
	int	open_token;
	int	end_token;
	ItemConfigData item;
	ItemConfigData consume_item;
};

struct RAGodTokenSurplusCfg
{
	RAGodTokenSurplusCfg() :open_exp(0) {}

	int open_exp;
	std::vector<ItemConfigData> common_item;
	std::vector<ItemConfigData> senior_item;
};
class RandActivityManager;

class RandActivityGodTokenConfig : public RandActivityConfig
{
public:
	RandActivityGodTokenConfig();
	virtual ~RandActivityGodTokenConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAGodTokenOtherCfg & GetOtherCfg()const  { return m_other_cfg; }
	const RAGodTokenTaskCfg * GetTaskCfg(int time_seq, int level, int type) const;
	const RAGodTokenLevelCfg * GetLevelCfg(int time_seq, int seq, int level)const;
	const RAGodTokenExchangeCfg * GetExchangeCfg(int time_seq, int seq)const;
	const RAGodTokenTaskCfg * GetWeekTaskCfg(int time_seq, int level, int type)const;

	const unsigned int GetTimeCfg(int seq)const;
	const int GetTimeSeqCfg(unsigned int timestamp) const;

	const int GetMinOpenLevel(int time_seq)const;
	const int GetExchangeCfgLen(int time_seq)const;

	void RandDailyTask(int time_seq, int level, char* type_list) const;
	void RandWeeklyTask(int time_seq, int level, char* type_list) const;

	const RAGodTokenSurplusCfg * GetSurplusCfg()const { return &m_surplus_cfg; }
private:
	int InitRAGodTokenOtherCfg(TiXmlElement *RootElement);
	int InitRAGodTokenTaskCfg(TiXmlElement *RootElement);
	int InitRAGodTokenLevelCfg(TiXmlElement *RootElement);
	int InitRAGodTokenExchangeCfg(TiXmlElement *RootElement);
	int InitRAGodTokenTimeCfg(TiXmlElement *RootElement);
	int InitRAGodTokenWeekTaskCfg(TiXmlElement *RootElement);
	int InitRAGodTokenSurplusCfg(TiXmlElement *RootElement);

	RAGodTokenOtherCfg m_other_cfg;
	std::map<int, std::vector<RAGodTokenLevelSectionCfg> > m_level_cfg;
	std::map<int, std::vector<RAGodTokenExchangeCfg> > m_exchange_cfg;
	unsigned int m_time_cfg[GOD_TOKEN_TIME_SEQ_MAXNUM];

	RAGodTokenTaskListCfg m_daily_task_cfg[GOD_TOKEN_TIME_SEQ_MAXNUM][GOD_TOKEN_LEVEL_SEGMENT];
	RAGodTokenTaskListCfg m_weekly_task_cfg[GOD_TOKEN_TIME_SEQ_MAXNUM][GOD_TOKEN_LEVEL_SEGMENT];

	RAGodTokenSurplusCfg m_surplus_cfg;
};

#endif	//__RAND_ACTIVITY_GOD_TOKEN_CONFIG_HPP__