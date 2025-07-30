#ifndef __RAND_ACTIVITY_BO_ZHONG_YOU_LI_CONFIG_HPP__
#define __RAND_ACTIVITY_BO_ZHONG_YOU_LI_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/activitydef.hpp"

struct BZYLBaseRewardSectionCfg
{
	BZYLBaseRewardSectionCfg():section_start(0),section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<ItemConfigData> reward_list;
};

struct BZYLRandRewardCfg
{
	BZYLRandRewardCfg():rate(0)
	{}

	int rate;
	ItemConfigData reward;
};

struct BZYLRandRewardSectionCfg
{
	BZYLRandRewardSectionCfg() :section_start(0), section_end(0), total_rate(0)
	{}

	int section_start;
	int section_end;
	int total_rate;
	std::vector<BZYLRandRewardCfg> reward_group;
};

struct BZYLOtherCfg
{
	BZYLOtherCfg():sowing_limit(0), watering_limit(0), mature_time(0)
	{}

	int sowing_limit;		// 每日可播种次数
	int watering_limit;		// 浇水时间间隔
	int mature_time;		// 成熟需要的时间
};


class RandActivityManager;
class RandActivityBoZhongYouLiConfig : public RandActivityConfig
{
public:
	RandActivityBoZhongYouLiConfig();
	virtual ~RandActivityBoZhongYouLiConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const std::vector<ItemConfigData>* GetBaseRewardCfg(RandActivityManager* ramgr)const;
	const ItemConfigData * GetRandRewardCfg(RandActivityManager* ramgr)const;
	const BZYLOtherCfg& GetOtherCfg()const { return m_other_cfg; }

private:
	int InitBaseCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	std::vector<BZYLBaseRewardSectionCfg> m_base_section_cfg;
	std::vector<BZYLRandRewardSectionCfg> m_reward_section_cfg;
	BZYLOtherCfg m_other_cfg;
};




#endif