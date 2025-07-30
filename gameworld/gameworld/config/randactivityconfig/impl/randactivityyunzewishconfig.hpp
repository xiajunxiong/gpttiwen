#ifndef __RAND_ACTIVITY_YUN_ZE_WISH_CONFIG_HPP__
#define __RAND_ACTIVITY_YUN_ZE_WISH_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itempool.h"
#include "servercommon/activitydef.hpp"

struct RAYZWishRewardCfg
{
	RAYZWishRewardCfg():seq(0),rate(0)
	{}

	int seq;
	int rate;
	ItemConfigData reward;
};

struct RAYZWishRewardSectionCfg
{
	RAYZWishRewardSectionCfg() :section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<RAYZWishRewardCfg> reward_list;
};

struct RAYZWishOtherCfg
{
	RAYZWishOtherCfg():time(0),reward_choice(0),reward_get(0)
	{}

	int time;				// 刷新时长
	int reward_choice;		// 可选奖励数量
	int reward_get;			// 可获得奖励数量
};

class RandActivityManager;
class RandActivityYunZeWishConfig : public RandActivityConfig
{
public:
	RandActivityYunZeWishConfig();
	virtual ~RandActivityYunZeWishConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAYZWishRewardSectionCfg * GetRewardCfg(RandActivityManager* ramgr)const;
	const RAYZWishOtherCfg & GetOtherCfg()const { return m_other_cfg; }

private:
	int InitRewardSectionCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);


	std::vector<RAYZWishRewardSectionCfg> m_reward_section_cfg;
	RAYZWishOtherCfg m_other_cfg;
};





#endif