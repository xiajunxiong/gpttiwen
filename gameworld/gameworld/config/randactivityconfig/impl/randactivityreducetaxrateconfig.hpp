#ifndef __RAND_ACTIVITY_REDUCE_TAX_RATE_CONFIG_HPP__
#define __RAND_ACTIVITY_REDUCE_TAX_RATE_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/rareducetaxrateparam.hpp"
#include "item/itembase.h"

struct RAReduceTaxRateSectionCfg
{
	RAReduceTaxRateSectionCfg() :section_start(0), section_end(0) {}

	int section_start;
	int section_end;

	unsigned int tax_rate;
};

class RandActivityManager;

class RandActivityReduceTaxRateConfig : public RandActivityConfig
{
public:
	RandActivityReduceTaxRateConfig();
	virtual ~RandActivityReduceTaxRateConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const int GetRateFromReduceTaxRateSectionCfg(RandActivityManager* ramgr) const;

private:
	int InitRAReduceTexRateSectionCfg(TiXmlElement *RootElement);

	std::vector<RAReduceTaxRateSectionCfg> m_reduce_tax_rate_section_cfg;
};

#endif	//__RAND_ACTIVITY_REDUCE_TAX_TATE_CONFIG_HPP__