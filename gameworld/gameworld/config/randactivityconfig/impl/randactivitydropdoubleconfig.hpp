#ifndef __RAND_ACTIVITY_DROP_DOUBLE_CONFIG_HPP__
#define __RAND_ACTIVITY_DROP_DOUBLE_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/radropdoubleparam.hpp"
#include "item/itembase.h"

struct DropDoubleCfg
{
	DropDoubleCfg() :seq(0), fb_id(0), drop_promote(0) {}

	int seq;
	int fb_id;
	int drop_promote;
};

struct DropDoubleSectionCfg
{
	DropDoubleSectionCfg() :section_start(0), section_end(0) {}

	int section_start;
	int section_end;
	std::vector<DropDoubleCfg> node;
};

class RandActivityManager;

class RandActivityDropDoubleConfig : public RandActivityConfig
{
public:
	RandActivityDropDoubleConfig();
	virtual ~RandActivityDropDoubleConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);
	const DropDoubleCfg * GetDropPeomoteCfg(RandActivityManager* ramgr, int fb_id)const;

private:
	int InitDropDoubleSectionCfg(TiXmlElement *RootElement);

	std::vector<DropDoubleSectionCfg> m_section_cfg;
};

#endif	//__RAND_ACTIVITY_DROP_DOUBLE_CONFIG_HPP__