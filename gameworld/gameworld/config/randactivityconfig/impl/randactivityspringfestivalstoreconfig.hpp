#ifndef __RAND_ACTIVITY_SPRING_FESTIVAL_STORE_CONFIG_HPP__
#define __RAND_ACTIVITY_SPRING_FESTIVAL_STORE_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itemextern.hpp"
#include "item/itembase.h"
#include <map>
#include <vector>
#include "servercommon/roleactivity/raspringfestivalstoreparam.hpp"


struct RASpringFestivalStoreCfg
{
	RASpringFestivalStoreCfg() : type(0), phase(0), section_start(0), section_end(0), start_time(0), end_time(0), seq(0), limit_type(0), buy_times(0), today_times(0), price_type(0), price(0) {}

	enum PRICE_TYPE
	{
		PRICE_TYPE_MIN,
		PRICE_TYPE_GOLD = 0,//元宝
		PRICE_TYPE_COIN = 2,//铜币
		PRICE_TYPE_MONEY = 3,//直购
	};

	int type;
	int phase;
	int section_start;
	int section_end;
	int start_time;
	int	end_time;
	int seq;

	ItemConfigData item_reward;

	enum LIMIT_TYPE
	{
		LIMIT_TYPE_ACTIVITY = 1,//总限购
		LIMIT_TYPE_TODAY = 2,	//每日限购
	};

	int limit_type;
	int buy_times;
	int today_times;
	int price_type;
	int price;
};

class RandActivityManager;

class RandActivitySpringFestivalStoreConfig : public RandActivityConfig
{
public:
	RandActivitySpringFestivalStoreConfig();
	virtual ~RandActivitySpringFestivalStoreConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RASpringFestivalStoreCfg * GetSpringFestivalStoreCfgByIndex(int id) const;
	const RASpringFestivalStoreCfg * GetSpringFestivalStoreCfgByActivityDay(int server_open_day, int open_day, int id) const;
private:
	int InitSpringFestivalStoreCfg(TiXmlElement *RootElement);

	union KeyChangeUn
	{
		int section[2];
		long long key;
	};

	long long GetKeyBySection(int section_start, int section_end) const;
	void GetSectionByKey(long long key, int* section_start, int* section_end) const;

	std::map<long long, std::map<long long, std::vector<RASpringFestivalStoreCfg> > > m_section_map;
	std::map<int, RASpringFestivalStoreCfg> m_index_map;
};

#endif	//__RAND_ACTIVITY_GIVE_PET_DRAW_CONFIG_HPP__