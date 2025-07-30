#ifndef __CONCENTRIC_KNOT_CONFIG_HPP__
#define __CONCENTRIC_KNOT_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include <vector>
#include <map>

struct ConcentricKnotOtherCfg
{
	ConcentricKnotOtherCfg() : play_time(0) {}

	std::vector<ItemConfigData> list;
	int play_time;
	std::vector<ItemConfigData> reward;
};


class ConcentricKnotConfig : public ILogicConfig
{
public:
	ConcentricKnotConfig();
	virtual ~ConcentricKnotConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const ConcentricKnotOtherCfg & GetOtherCfg() { return m_other_cfg; }
	int GetActiveRateCfg(int active);
	bool IsEndActiveRate(int active);

private:
	int InitOtherCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitActiveCfg(TiXmlElement *RootElement, std::string& errormsg);

	ConcentricKnotOtherCfg m_other_cfg;
	std::map<int, int>	m_active_cfg;
};

#endif