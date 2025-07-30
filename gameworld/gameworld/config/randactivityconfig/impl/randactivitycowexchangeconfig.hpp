#ifndef __RAND_ACTIVITY_COW_EXCHANGE_CONFIG_HPP__
#define __RAND_ACTIVITY_COW_EXCHANGE_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itemextern.hpp"
#include "item/itembase.h"
#include <map>
#include <vector>
#include "servercommon/roleactivity/racowparam.hpp"


struct RACowExchangeCfg
{
	RACowExchangeCfg() : type(0), section_start(0), section_end(0), seq(0), unlock_integral(0), limit_type(0), buy_times(0), today_times(0), price_type(0), price(0) {}

	enum PRICE_TYPE
	{
		PRICE_TYPE_GOLD = 0,		//元宝
		PRICE_TYPE_JIFEN,			//积分
		PRICE_TYPE_COIN,			//铜币
		PRICE_TYPE_MONEY,			//直购
	};
	int type;
	int section_start;
	int section_end;
	int	seq;
	int unlock_integral;

	enum LIMIT_TYPE
	{
		LIMIT_TYPE_ACTIVITY = 1,//总限购
		LIMIT_TYPE_TODAY = 2,	//每日限购
	};

	int	limit_type;
	int	buy_times;
	int	today_times;
	int price_type;
	int	price;
	ItemConfigData reward_item;
};

class RandActivityManager;

class RandActivityCowExchangeConfig : public RandActivityConfig
{
public:
	RandActivityCowExchangeConfig();
	virtual ~RandActivityCowExchangeConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);
	const RACowExchangeCfg * GetCowExchangeCfgByIndex(int id) const;
	const RACowExchangeCfg * GetCowExchangeCfgByActivityDay(int open_day, int id) const;
private:
	int InitCowExchangeCfg(TiXmlElement *RootElement);

	long long GetKeyBySection(int section_start, int section_end) const;
	void GetSectionByKey(long long key, int* section_start, int* section_end) const;

	std::map<long long,std::vector<RACowExchangeCfg> > m_section_map;
	std::map<int, RACowExchangeCfg> m_index_map;
};

#endif	//__RAND_ACTIVITY_COW_EXCHANGE_CONFIG_HPP__