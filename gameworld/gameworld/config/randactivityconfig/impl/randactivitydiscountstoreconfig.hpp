#ifndef __RAND_ACTIVITY_DISCOUNT_STORE_CONFIG_HPP__
#define __RAND_ACTIVITY_DISCOUNT_STORE_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/radiscountstoreparam.hpp"
#include "item/itembase.h"

struct RADiscountStoreCfg
{
	RADiscountStoreCfg() :seq(0), need_gold(0), buy_times(0), total_exchange(0)
	{}

	int seq;
	ItemConfigData item;
	int need_gold;
	int buy_times;
	int total_exchange;
};

struct RADiscountStoreSectionCfg
{
	RADiscountStoreSectionCfg() :section_start(0), section_end(0), count(0)
	{}

	int section_start;
	int section_end;

	int count;
	RADiscountStoreCfg node_cfg[RA_DISCOUNT_STORE_MAX_NUM];
};

class RandActivityManager;

class RandActivityDiscountStoreConfig : public RandActivityConfig
{
public:
	RandActivityDiscountStoreConfig();
	virtual ~RandActivityDiscountStoreConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RADiscountStoreCfg * GetDiscountStoreItemCfg(RandActivityManager* ramgr, int seq) const;
	const RADiscountStoreSectionCfg * GetDiscountStoreSectionCfg(RandActivityManager* ramgr) const;
private:
	int InitCfg(TiXmlElement * RootElement);

	std::vector<RADiscountStoreSectionCfg> m_discount_store_sections;			// уш©шил╣Й
};

#endif	//__RAND_ACTIVITY_DISCOUNT_STORE_CONFIG_HPP__