#ifndef __RAND_ACTIVITY_DUAN_YANG_FU_LI_CONFIG_HPP__
#define __RAND_ACTIVITY_DUAN_YANG_FU_LI_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"
#include "item/itembase.h"

#include <vector>
#include <set>
#include <map>

struct RandActivityDuanYangFiLiOtherCfg
{
	int all_buy_need_chong_zhi_gold;
	int cfg_ver;
};

struct  RandActivityDuanYangFiLiCfg
{
	int seq;
	int day;
	int can_fetch_times;
	int need_chong_zhi_gold;

	std::vector<ItemConfigData> rewards;
};

struct  RandActivityDuanYangFuLiSectionCfg
{
	RandActivityDuanYangFuLiSectionCfg() :section_start(0), section_end(0)
	{
	
	}

	int section_start;
	int section_end;

	std::map<int, RandActivityDuanYangFiLiCfg> item_list;
};

class RandActivityManager;
class RandActivityDuanYangFuLiConfig : public RandActivityConfig
{
public:
	RandActivityDuanYangFuLiConfig();
	virtual ~RandActivityDuanYangFuLiConfig();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const RandActivityDuanYangFiLiOtherCfg & GetOtherCfg() const;
	const RandActivityDuanYangFiLiCfg * GetRandActivityDuanYangFiLiCfg(RandActivityManager * ramgr,  int reward_seq) const;
	const RandActivityDuanYangFuLiSectionCfg * GetRandActivityDuanYangFiLiSectionCfg(RandActivityManager * ramgr, time_t time = 0) const;
	
	virtual int GetConfigVer() const;
private: 
	int InitOtherfg(TiXmlElement * RootElement);
	int InitRandActivityDuanYangFiLiCfg(TiXmlElement * RootElement);

	std::vector<RandActivityDuanYangFuLiSectionCfg> m_section_cfg;
	RandActivityDuanYangFiLiOtherCfg m_other_cfg;
};

#endif	