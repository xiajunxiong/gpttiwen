#ifndef __LOVERS_MARKET_CONFIG_HPP__
#define __LOVERS_MARKET_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include <vector>
#include "servercommon/cycleactivity/loversmarketparam.hpp"

struct LoversMarketOtherCfg
{
	LoversMarketOtherCfg() : play_coin(0) {}

	ItemID play_coin;
};

struct LoversMarketStoreCfg
{
	LoversMarketStoreCfg() : item_id(0), is_bind(true), num(0), price(0), limit_convert_count(0) {}

	ItemID item_id;
	bool is_bind;
	int num;
	int price;
	short limit_convert_count;				// 0: 不限购 大于0 为周期限购
};


class LoversMarketConfig : public ILogicConfig
{
public:
	LoversMarketConfig();
	virtual ~LoversMarketConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const LoversMarketOtherCfg & GetOtherCfg() { return m_other_cfg; }
	const LoversMarketStoreCfg * GetStoreCfg(int seq);

private:
	int InitOtherCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitStoreCfg(TiXmlElement *RootElement, std::string& errormsg);

	LoversMarketOtherCfg m_other_cfg;
	std::vector<LoversMarketStoreCfg> m_store_cfg;
};

#endif