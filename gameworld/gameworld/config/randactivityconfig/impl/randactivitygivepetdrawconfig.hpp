#ifndef __RAND_ACTIVITY_GIVE_PET_DRAW_CONFIG_HPP__
#define __RAND_ACTIVITY_GIVE_PET_DRAW_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itemextern.hpp"
#include "item/itembase.h"


struct RAGivePetDrawOtherCfg
{
	RAGivePetDrawOtherCfg() {}

	ItemConfigData item;
};

class RandActivityManager;

class RandActivityGivePetDrawConfig : public RandActivityConfig
{
public:
	RandActivityGivePetDrawConfig();
	virtual ~RandActivityGivePetDrawConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAGivePetDrawOtherCfg * GetGivePetDrawOtherCfg()const { return &m_other_cfg; }

private:
	int InitGivePetDrawOtherCfg(TiXmlElement *RootElement);

	RAGivePetDrawOtherCfg m_other_cfg;

};

#endif	//__RAND_ACTIVITY_GIVE_PET_DRAW_CONFIG_HPP__