#ifndef __RAND_ACTIVITY_SPORTS_CONFIG_HPP__
#define __RAND_ACTIVITY_SPORTS_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/rasportsparam.hpp"
#include "item/itembase.h"

struct RASportsHistoryCfg			//新服竞技历史排名奖励
{
	RASportsHistoryCfg() :seq(0), historical_ranking(0) {}

	int seq;
	int historical_ranking;
	std::vector<ItemConfigData> itemlist;
};

struct RASportsSettlementCfg			//新服竞技排名结算奖励
{
	RASportsSettlementCfg() :seq(0), ranking(0) {}

	int seq;
	int ranking;
	std::vector<ItemConfigData> itemlist;
};

class RandActivityManager;

class RandActivitySportsConfig : public RandActivityConfig
{
public:
	RandActivitySportsConfig();
	virtual ~RandActivitySportsConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RASportsHistoryCfg * GetSportsHistoryCfg(int seq) const;
	const std::map<int, RASportsHistoryCfg>* GetSportsHistoryList() const;
	const RASportsSettlementCfg * GetSportsSettlementCfg(int role_rank) const;

private:
	int InitSportsHistoryCfg(TiXmlElement * RootElement);
	int InitSportsSettlementCfg(TiXmlElement * RootElement);

	std::map<int, RASportsHistoryCfg> m_sports_history_map;								// 竞技场历史奖励
	std::map<int, RASportsSettlementCfg> m_sports_settlement_map;						// 竞技场排行结算
};

#endif	//__RAND_ACTIVITY_SPORTS_CONFIG_HPP__