#ifndef __RAND_ACTIVITY_ELEMENT_CARDS_CONFIG_HPP__
#define __RAND_ACTIVITY_ELEMENT_CARDS_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/raelementcardsparam.hpp"
#include "item/itembase.h"

struct RAElementCardsOtherCfg
{
	RAElementCardsOtherCfg() : login_add_count(0) {}

	int login_add_count;
};

struct RAElementCardsCfg
{
	RAElementCardsCfg() :seq(0), weight(0) {}

	int seq;
	int weight;
	std::vector<ItemConfigData> item;
};

struct RAElementCardsSectionCfg
{
	RAElementCardsSectionCfg() :section_start(0), section_end(0), all_weight(0) {}

	int section_start;
	int section_end;
	int all_weight;

	std::vector<RAElementCardsCfg> node;
};

class RandActivityManager;

class RandActivityElementCardsConfig : public RandActivityConfig
{
public:
	RandActivityElementCardsConfig();
	virtual ~RandActivityElementCardsConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAElementCardsOtherCfg & GetElementCardsOtherCfg()const { return m_other_cfg; }
	const RAElementCardsCfg * GetElementCardsCfg(RandActivityManager* ramgr)const;

private:
	int InitElementCardsOtherCfg(TiXmlElement *RootElement);
	int InitElementCardsSectionCfg(TiXmlElement *RootElement);

	RAElementCardsOtherCfg m_other_cfg;
	std::vector<RAElementCardsSectionCfg> m_section_cfg;
};

#endif	//__RAND_ACTIVITY_ELEMENT_CARDS_CONFIG_HPP__