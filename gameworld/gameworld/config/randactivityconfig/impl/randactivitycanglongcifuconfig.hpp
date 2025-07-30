#ifndef __RAND_ACTIVITY_CANG_LONG_CI_FU_CONFIG_HPP__
#define __RAND_ACTIVITY_CANG_LONG_CI_FU_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/struct/itemlistparam.h"
#include "item/itempool.h"


struct CLCFBasicSectionCfg
{
	CLCFBasicSectionCfg() :section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<std::pair<ItemConfigData, int> > consume_item_list;		// pair<消耗道具, 奖励组>
};

struct CLCFRewardGroupCfg
{
	CLCFRewardGroupCfg() :reward_group(0), total_rate(0)
	{}

	int reward_group;
	int total_rate;
	std::vector<std::pair<int, std::vector<ItemConfigData> > > reward_list;		// pair<rate, 奖励>
};

struct CLCFOtherCfg
{
	CLCFOtherCfg()
	{}

	ItemConfigData last_reward;		// 全部点亮奖励
};


class RandActivityManager;
class RandActivityCangLongCiFuConfig : public RandActivityConfig
{
public:
	RandActivityCangLongCiFuConfig();
	virtual ~RandActivityCangLongCiFuConfig();

	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const std::pair<ItemConfigData, int>* GetBasicBySeqCfg(RandActivityManager* ramgr, int seq, int& fragments_num)const;
	const std::vector<ItemConfigData>* GetRewardCfg(int reward_group)const;
	const CLCFOtherCfg& GetOtherCfg()const { return m_other_cfg; }

private:
	int InitBasicCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	std::vector<CLCFBasicSectionCfg> m_base_section_cfg;
	std::vector<CLCFRewardGroupCfg> m_reward_group_cfg;
	CLCFOtherCfg m_other_cfg;
};

#endif