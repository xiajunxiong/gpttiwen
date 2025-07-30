#ifndef __RAND_ACTIVITY_INTEGRAL_DRAW_CONFIG_HPP__
#define __RAND_ACTIVITY_INTEGRAL_DRAW_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itembase.h"
#include "servercommon/roleactivity/raintegraldrawparam.hpp"

struct RAIntegralDrawRewardCfg
{
	RAIntegralDrawRewardCfg() : seq(0), color_num(0), is_borcast(false) {}

	short seq;
	short color_num;
	bool is_borcast;
	std::vector<ItemConfigData> reward_item;
};

struct RAIntegralDrawOtherCfg
{
	RAIntegralDrawOtherCfg() : day_times(0){}

	short day_times;
};

class RandActivityManager;

class RandActivityIntegralDrawConfig : public RandActivityConfig
{
public:
	RandActivityIntegralDrawConfig();
	virtual ~RandActivityIntegralDrawConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAIntegralDrawRewardCfg * GetRewardCfg(RandActivityManager * ramgr, int seq) const;
	const RAIntegralDrawOtherCfg & GetOtherCfg() const { return m_other_cfg; }

private:
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	std::map<int, std::vector<RAIntegralDrawRewardCfg> > m_reward_cfg;
	RAIntegralDrawOtherCfg m_other_cfg;
};

#endif	//__RAND_ACTIVITY_INTEGRAL_DRAW_CONFIG_HPP__