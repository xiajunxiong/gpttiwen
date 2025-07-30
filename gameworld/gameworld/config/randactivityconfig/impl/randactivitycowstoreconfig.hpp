#ifndef __RAND_ACTIVITY_COW_STORE_CONFIG_HPP__
#define __RAND_ACTIVITY_COW_STORE_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itemextern.hpp"
#include "item/itembase.h"
#include <map>
#include <vector>
#include "servercommon/roleactivity/racowparam.hpp"


struct RACowStoreCfg
{
	RACowStoreCfg() : type(0), phase(0), section_start(0), section_end(0), start_time(0), end_time(0), seq(0), limit_type(0), buy_times(0), today_times(0), price_type(0), price(0) {}

	enum PRICE_TYPE
	{
		PRICE_TYPE_GOLD = 0,//元宝
		PRICE_TYPE_COIN = 2,//铜币
		PRICE_TYPE_MONEY = 3,//直购
	};
	int type;
	int	phase;
	int	section_start;
	int section_end;
	int start_time;
	int end_time;
	int	seq;
	
	enum LIMIT_TYPE
	{
		LIMIT_TYPE_ACTIVITY = 1,//总限购
		LIMIT_TYPE_TODAY = 2,	//每日限购
	};

	int	limit_type;
	int	buy_times;
	int	today_times;
	int	price_type;
	int	price;
	ItemConfigData reward_item;
};

class RandActivityManager;

class RandActivityCowStoreConfig : public RandActivityConfig
{
public:
	RandActivityCowStoreConfig();
	virtual ~RandActivityCowStoreConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);
	const RACowStoreCfg * GetCowStoreCfgByIndex(int id) const;
	const RACowStoreCfg * GetCowStoreCfgByActivityDay(int server_open_day, int open_day, int id) const;
private:
	int InitCowStoreCfg(TiXmlElement *RootElement);

	long long GetKeyBySection(int section_start, int section_end) const;
	void GetSectionByKey(long long key, int* section_start, int* section_end) const;

	std::map<long long, std::map<long long,std::vector<RACowStoreCfg> > > m_section_map;
	std::map<int, RACowStoreCfg> m_index_map;
};

#endif	//__RAND_ACTIVITY_GIVE_PET_DRAW_CONFIG_HPP__