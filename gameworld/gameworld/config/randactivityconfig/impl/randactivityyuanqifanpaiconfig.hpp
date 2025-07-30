#ifndef __RAND_ACTIVITY_YUAN_QI_FAN_PAI_CONFIG_HPP__
#define __RAND_ACTIVITY_YUAN_QI_FAN_PAI_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/activitydef.hpp"

struct YQFPBaseCfg
{
	YQFPBaseCfg():rate(0),is_hearsay(0)
	{}

	int rate;
	int is_hearsay;
	ItemConfigData reward;
};

struct YQFPBaseSectionCfg
{
	YQFPBaseSectionCfg() :section_start(0), section_end(0), total_rate(0)
	{}

	int section_start;
	int section_end;
	int total_rate;
	std::vector<YQFPBaseCfg> reward_cfg;
};

struct YQFPTimesRewardCfg
{
	YQFPTimesRewardCfg():task_limit(0)
	{}

	int task_limit;		// 所需任务次数
	ItemConfigData reward;
};

struct YQFPTimesSectionCfg
{
	YQFPTimesSectionCfg() :section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<YQFPTimesRewardCfg> reward_cfg;
};

struct YQFPGatherCfg
{
	YQFPGatherCfg()
	{}

	std::vector<ItemConfigData> consunme_item;	// 兑换消耗道具
	ItemConfigData reward;
};

struct YQFPOtherCfg
{
	YQFPOtherCfg()
	{}

	ItemConfigData consunme_item;		// 翻牌消耗道具
	ItemConfigData day_fetch_reward;
};

class RandActivityManager;
class RandActivityYuanQiFanPaiConfig : public RandActivityConfig
{
public:
	RandActivityYuanQiFanPaiConfig();
	virtual ~RandActivityYuanQiFanPaiConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const YQFPBaseCfg* GetRandRewardCfg(RandActivityManager* ramgr)const;
	const YQFPTimesRewardCfg* GetTimesRewardCfg(RandActivityManager* ramgr, int seq)const;
	const YQFPGatherCfg* GetGatherCfg(int seq)const;
	const YQFPOtherCfg& GetOtherCfg()const { return m_other_cfg; }

private:
	int InitBaseCfg(TiXmlElement *RootElement);
	int InitTimesCfg(TiXmlElement *RootElement);
	int InitGtherCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	std::vector<YQFPBaseSectionCfg> m_base_section_cfg;
	std::vector<YQFPTimesSectionCfg> m_times_section_cfg;
	std::vector<YQFPGatherCfg> m_gather_cfg;
	YQFPOtherCfg m_other_cfg;
};




#endif
