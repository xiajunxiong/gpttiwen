#ifndef __RAND_ACTIVITY_SIGN_IN_CONTINUITY_CONFIG_HPP__
#define __RAND_ACTIVITY_SIGN_IN_CONTINUITY_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/rasignincontinuityparam.hpp"
#include "item/itembase.h"

struct RASignInContinuityCfg
{
	RASignInContinuityCfg() :seq(0), day(0) {}

	int seq;
	int day;
	std::vector<ItemConfigData> item;
};

struct RASignInContinuitySectionCfg
{
	RASignInContinuitySectionCfg() :section_start(0), section_end(0) {}

	int section_start;
	int section_end;

	std::vector<RASignInContinuityCfg> node;
};

class RandActivityManager;

class RandActivitySignInContinuityConfig : public RandActivityConfig
{
public:
	RandActivitySignInContinuityConfig();
	virtual ~RandActivitySignInContinuityConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RASignInContinuityCfg * GetSignInContinuityCfg(RandActivityManager* ramgr, int seq)const;

private:
	int InitSignInContinuityOtherCfg(TiXmlElement *RootElement);

	std::vector<RASignInContinuitySectionCfg> m_section_cfg;
};

#endif	//__RAND_ACTIVITY_SIGN_IN_CONTINUITY_CONFIG_HPP__