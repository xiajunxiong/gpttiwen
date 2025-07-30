#ifndef __GOTO_SCHOOL_CONFIG_HPP__
#define __GOTO_SCHOOL_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "gamedef.h"

#include <set>

struct GotoSchoolOtherConfig
{
	int can_get_reward_times;
	int scene_id;
	std::map<int, std::vector<ItemConfigData> > rewards;
};

class GotoSchoolConfig : public ILogicConfig
{
public:
	GotoSchoolConfig();
	virtual ~GotoSchoolConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const GotoSchoolOtherConfig & GetOtherCfg();
private:
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitRewardCfg(TiXmlElement * RootElement);

	GotoSchoolOtherConfig m_other_cfg;
};


#endif