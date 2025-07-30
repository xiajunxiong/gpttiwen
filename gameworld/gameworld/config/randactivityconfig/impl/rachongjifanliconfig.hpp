#ifndef __RA_CHONG_JI_FAN_LI_HPP__
#define __RA_CHONG_JI_FAN_LI_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"
#include "item/itembase.h"

#include <vector>
#include <set>
#include <map>


struct RAChongJiFanLiRewardCfg
{
	RAChongJiFanLiRewardCfg() : day(0), need_chong_num(0)
	{}

	int day;
	int need_chong_num;
	ItemConfigData grand_prize;			//不一定每个都有大奖
	std::vector<ItemConfigData> common_reward_list;
};

struct RAChongJiFanLiSectionRewardCfg
{
	RAChongJiFanLiSectionRewardCfg(): section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<RAChongJiFanLiRewardCfg> reward_list;
};

class RandActivityManager;
class RandActivityChongJiFanLiConfig : public RandActivityConfig
{
public:
	RandActivityChongJiFanLiConfig();
	virtual ~RandActivityChongJiFanLiConfig();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const RAChongJiFanLiRewardCfg * GetRewardCfg(RandActivityManager* ramgr, int seq)const;
	const RAChongJiFanLiRewardCfg * GetRewardCfgByDay(RandActivityManager* ramgr, int day)const;
	const RAChongJiFanLiSectionRewardCfg * GetRewardSectionCfg(RandActivityManager* ramgr)const;

private:
	int InitRewardCfg(TiXmlElement * RootElement, std::string error_msg);

	std::vector<RAChongJiFanLiSectionRewardCfg> m_section_reward_cfg;
};

#endif	