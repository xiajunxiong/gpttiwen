#ifndef __RAND_ACTIVITY_TRADE_MARKET_RETURN_GOLD_CONFIG_HPP__
#define __RAND_ACTIVITY_TRADE_MARKET_RETURN_GOLD_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/ratrademarketreturngoldparam.hpp"
#include "item/itembase.h"

struct RATradeMarketReturnGoldSectionCfg
{
	RATradeMarketReturnGoldSectionCfg() :section_start(0), section_end(0), return_gold_percent(0)
	{}

	int section_start;
	int section_end;

	int return_gold_percent;
};

class RandActivityManager;
class RandActivityTradeMarketReturnGoldConfig : public RandActivityConfig
{
public:
	static const int RETURN_GOLD_CFG_SECTION_COUNT = 10;

public:
	RandActivityTradeMarketReturnGoldConfig();
	virtual ~RandActivityTradeMarketReturnGoldConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	double GetReturnGoldNum(RandActivityManager* ramgr, int consume_gold) const;
private:
	int InitReturnGoldCfg(TiXmlElement * RootElement);

	int m_section_count;
	RATradeMarketReturnGoldSectionCfg m_return_gold_section_cfg_list[RETURN_GOLD_CFG_SECTION_COUNT];
};

#endif	//__RAND_ACTIVITY_TRADE_MARKET_RETURN_GOLD_CONFIG_HPP__