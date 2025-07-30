#ifndef __RAND_ACTIVITY_TIME_LIMIT_STORE_CONFIG_HPP__
#define __RAND_ACTIVITY_TIME_LIMIT_STORE_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/ratimelimitstoreparam.hpp"
#include "item/itembase.h"

struct TimeLimitStoreCfg
{
	TimeLimitStoreCfg() :seq(0), limit_times(0), money_type(0), price(0)
	{}

	int seq;
	ItemConfigData item;
	int limit_times;
	int money_type;
	int price;
};

struct TimeLimitStoreSectionCfg
{
	TimeLimitStoreSectionCfg() :section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;

	TimeLimitStoreCfg node_cfg[RA_TIME_LIMIT_STORE_MAX_NUM];
};

class RandActivityManager;

class RandActivityTimeLimitStoreConfig : public RandActivityConfig
{
public:
	RandActivityTimeLimitStoreConfig();
	virtual ~RandActivityTimeLimitStoreConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const TimeLimitStoreCfg* GetTimeLimitStoreItemCfg(RandActivityManager* ramgr, int seq)const;

private:
	int InitCfg(TiXmlElement * RootElement);

	std::vector<TimeLimitStoreSectionCfg> m_time_limit_store_sections;
};



#endif	//__RAND_ACTIVITY_TIME_LIMIT_STORE_CONFIG_HPP__