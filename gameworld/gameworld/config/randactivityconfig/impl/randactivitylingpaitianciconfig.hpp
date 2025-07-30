#ifndef __RAND_ACTIVITY_LING_PAI_TIAN_CI_CONFIG_HPP__
#define __RAND_ACTIVITY_LING_PAI_TIAN_CI_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"
#include "item/itembase.h"

#include <vector>
#include <set>
#include <map>

class RandActivityManager;

struct RALingPaiTianCiCfg
{
	RALingPaiTianCiCfg()
	{
		seq = 0;
		need_chong_zhi_gold = 0;
		is_borcast = 0;
		borcast_item_id = 0;
	}

	int seq;
	unsigned int need_chong_zhi_gold;
	int is_borcast;
	int borcast_item_id;
	std::vector<ItemConfigData> rewards;
};

struct RALingPaiTianCiSectionCfg
{
	RALingPaiTianCiSectionCfg() :section_start(0), section_end(0)
	{
	}

	int section_start;
	int section_end;
	std::vector<RALingPaiTianCiCfg> item;
};


class RandActivityManager;
class RandActivityLingPaiTianCiConfig : public RandActivityConfig
{
public:
	RandActivityLingPaiTianCiConfig();
	virtual ~RandActivityLingPaiTianCiConfig();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const RALingPaiTianCiCfg * GetRALingPaiTianCiCfgCfg(RandActivityManager * ramgr, int seq) const;
private:
	int InitCfg(TiXmlElement * RootElement);


	std::vector<RALingPaiTianCiSectionCfg> m_section_cfg;
};

#endif	