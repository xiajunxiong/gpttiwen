#ifndef __RAND_ACTIVITY_YUAN_QI_SPEED_CONFIG_HPP__
#define __RAND_ACTIVITY_YUAN_QI_SPEED_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/rayuanqispeedparam.hpp"

struct YuanQiSpeedOtherCfg
{
	YuanQiSpeedOtherCfg() : frequency(0), time(0) {}
	int frequency;
	int	time;
};

struct YuanQiSpeedReward
{
	YuanQiSpeedReward() {}

	std::vector<ItemConfigData> m_pass_reward;
	std::vector<ItemConfigData> m_fail_reward;
	std::vector<ItemConfigData> m_fisrt_reward;
};

struct YuanQiSpeedRewardCfg
{
	YuanQiSpeedRewardCfg(){}

	std::map<int, YuanQiSpeedReward> m_reward_vec;
};

class RandActivityManager;
class RandActivityYuanQiSpeedConfig : public RandActivityConfig
{
public:
	RandActivityYuanQiSpeedConfig();
	virtual ~RandActivityYuanQiSpeedConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const YuanQiSpeedOtherCfg & GetOtherCfg()const { return m_other_cfg; }
	const YuanQiSpeedReward * GetRewardCfg(int open_day, int seq) const;
private:
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitRewardCfg(TiXmlElement * RootElement);

	YuanQiSpeedOtherCfg m_other_cfg;
	std::map<int, YuanQiSpeedRewardCfg> m_reward_cfg;
};

#endif