#ifndef __RAND_ACTIVITY_SKY_DROP_LUXURY_GIFT_CONFIG_HPP__
#define __RAND_ACTIVITY_SKY_DROP_LUXURY_GIFT_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itembase.h"

struct SkyDropGiftCfg
{
	SkyDropGiftCfg() :scene_id(0), next_time(0), max_box_num(0), treasure_chest_id(0), mount(0)
	{}

	int scene_id;
	int next_time;
	int max_box_num;
	int treasure_chest_id;
	int mount;
};

struct SkyDropGiftSectionCfg
{
	SkyDropGiftSectionCfg() :section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;

	std::vector<SkyDropGiftCfg> section_cfg_vec;
};

const static int MAX_RANDACTIVITY_MAX_SECTION = 20;

class RandActivityManager;

class RandActivitySkyDropLuxuryGiftConfig : public RandActivityConfig
{
public:
	RandActivitySkyDropLuxuryGiftConfig();
	virtual ~RandActivitySkyDropLuxuryGiftConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const std::vector<SkyDropGiftCfg>* GetSkyDropGiftCfg(RandActivityManager* ramgr) const;

private:
	int InitCfg(TiXmlElement * RootElement);

	int m_sky_drop_count;
	SkyDropGiftSectionCfg m_sky_drop_gift_sections[MAX_RANDACTIVITY_MAX_SECTION];		// Ìì½µºÀÀñ-³¡¾°IDÎª¼ü
};



#endif	//__RAND_ACTIVITY_SKY_DROP_LUXURY_GIFT_CONFIG_HPP__