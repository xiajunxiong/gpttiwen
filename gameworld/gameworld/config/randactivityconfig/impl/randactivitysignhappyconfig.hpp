#ifndef __RAND_ACTIVITY_SIGN_HAPPY_CONFIG_HPP__
#define __RAND_ACTIVITY_SIGN_HAPPY_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itembase.h"


struct RASignHappyRewardCfg
{
	RASignHappyRewardCfg() :reward_count(0), reward_count_1(0) {}

	short reward_count;
	ItemConfigData reward_item[2];			// ½±ÀøµÀ¾ß

	short reward_count_1;
	ItemConfigData reward_item_1[4];			// ¶îÍâ½±Àø
};

class RandActivityManager;

class RandActivitySignHappyConfig : public RandActivityConfig
{
public:
	RandActivitySignHappyConfig();
	virtual ~RandActivitySignHappyConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RASignHappyRewardCfg * GetRewardCfg(RandActivityManager * ramgr, int index) const;

private:
	int InitRewardCfg(TiXmlElement *RootElement);

	std::map<int, std::vector<RASignHappyRewardCfg> > m_reward_cfg;
};

#endif	//__RAND_ACTIVITY_HAPPY_LOOK_CONFIG_HPP__