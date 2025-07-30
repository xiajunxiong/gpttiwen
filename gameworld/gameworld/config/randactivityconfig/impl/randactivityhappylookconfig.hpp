#ifndef __RAND_ACTIVITY_HAPPY_LOOK_CONFIG_HPP__
#define __RAND_ACTIVITY_HAPPY_LOOK_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itembase.h"

struct RAHappyLookOtherCfg
{
	RAHappyLookOtherCfg() :frequency(0), reward_count(0) {}

	short frequency;						// 可挑战次数

	short reward_count;
	ItemConfigData reward_item[8];			// 通关失败奖励道具
};

struct RAHappyLookRewardCfg
{
	RAHappyLookRewardCfg() :time(0), reward_count(0) {}

	short time;								// 挑战时间

	short reward_count;
	ItemConfigData reward_item[8];			// 通关奖励道具
};

class RandActivityManager;

class RandActivityHappyLookConfig : public RandActivityConfig
{
public:
	RandActivityHappyLookConfig();
	virtual ~RandActivityHappyLookConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAHappyLookOtherCfg & GetOtherCfg() const { return m_other_cfg; }
	const RAHappyLookRewardCfg * GetRewardCfg(RandActivityManager * ramgr, int index) const;

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);

	RAHappyLookOtherCfg m_other_cfg;
	std::map<int, std::vector<RAHappyLookRewardCfg> > m_reward_cfg;
};

#endif	//__RAND_ACTIVITY_HAPPY_LOOK_CONFIG_HPP__