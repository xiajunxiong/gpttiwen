#ifndef __RAND_ACTIVITY_WANLING_ZHULI_CONFIG_HPP__
#define __RAND_ACTIVITY_WANLING_ZHULI_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "config/randactivityconfig/randactivityconfig.hpp"


struct RAWanLingZhuLiRewardCfg
{
	RAWanLingZhuLiRewardCfg() {}

	std::vector<ItemConfigData> reward;
};


struct RAWanLingZhuLiOtherCfg
{
	RAWanLingZhuLiOtherCfg() : price(0) {}

	int price;

	ItemConfigData reward_item;
};


class RandActivityManager;
class RandActivityWanLingZhuLiConfig : public RandActivityConfig
{
public:
	RandActivityWanLingZhuLiConfig();
	virtual ~RandActivityWanLingZhuLiConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAWanLingZhuLiRewardCfg * GetRewardCfg(RandActivityManager * ramgr, int day) const;
	const RAWanLingZhuLiOtherCfg * GetOtherCfg() const { return &m_other_cfg; }

	int GetRewardNum(RandActivityManager * ramgr) const;

private:
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	std::map<short, std::vector<RAWanLingZhuLiRewardCfg> > m_reward_cfg;
	RAWanLingZhuLiOtherCfg m_other_cfg;
};


#endif	// __RAND_ACTIVITY_XUNMENG_HUDIE_CONFIG_HPP__