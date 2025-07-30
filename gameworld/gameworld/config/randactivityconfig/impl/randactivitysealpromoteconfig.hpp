#ifndef __RAND_ACTIVITY_SEAL_PROMOTE_CONFIG_HPP__
#define __RAND_ACTIVITY_SEAL_PROMOTE_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/rasealpromoteparam.hpp"
#include "item/itembase.h"

static const int THE_RATE_MAX_PERCENT = 100;

struct RASealPromoteCfg
{
	RASealPromoteCfg() :seq(0), pet_id(0), rate_common(0), rate_silver(0), rate_gold(0) {}

	int seq;
	int pet_id;
	int rate_common;
	int rate_silver;
	int rate_gold;
};

struct RASealPromoteSectionCfg
{
	RASealPromoteSectionCfg() :section_start(0), section_end(0) {}

	int section_start;
	int section_end;
	std::vector<RASealPromoteCfg> node;
};

class RandActivityManager;

class RandActivitySealPromoteConfig : public RandActivityConfig
{
public:
	RandActivitySealPromoteConfig();
	virtual ~RandActivitySealPromoteConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RASealPromoteCfg * GetSealPromoteCfg(RandActivityManager* ramgr, int pet_id)const;
private:
	int InitSealPromoteSectionCfg(TiXmlElement *RootElement);

	std::vector<RASealPromoteSectionCfg> m_seal_promote_section_cfg;
};

#endif	//__RAND_ACTIVITY_SEAL_PROMOTE_CONFIG_HPP__