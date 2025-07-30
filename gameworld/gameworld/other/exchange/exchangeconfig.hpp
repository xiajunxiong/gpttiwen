#ifndef __EXCHANGE_CONFIG_H__
#define __EXCHANGE_CONFIG_H__

#include "servercommon/configcommon.h"
#include "servercommon/basedef.h"
#include "servercommon/exchangedef.h"
#include <map>
#include <vector>

struct ExchangeItemCfg
{
	ExchangeItemCfg() :seq(0), score_type(0), score_price(0), item_id(0), is_bind(0), limit_type(0)
		, limit_times(0), open_time(0), sale_time(0), money_type(0), price(0)
	{}

	int seq;
	int score_type;
	int score_price;
	int item_id;
	int is_bind;
	int limit_type;
	int limit_times;
	int open_time;
	int sale_time;
	int money_type;
	int price;
};

class ExchangeConfig : public ILogicConfig
{
public:
	ExchangeConfig();
	virtual ~ExchangeConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const ExchangeItemCfg* GetItemCfg(int seq);

private:
	int InitItemCfg(TiXmlElement *RootElement);

	std::map<int, ExchangeItemCfg> m_exchange_item_map;

};

#endif
