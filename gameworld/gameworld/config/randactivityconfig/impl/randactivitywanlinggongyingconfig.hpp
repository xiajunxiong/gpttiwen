#ifndef __RAND_ACTIVITY_WAN_LING_GONG_YING_CONFIG_HPP__
#define __RAND_ACTIVITY_WAN_LING_GONG_YING_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itembase.h"
#include "servercommon/roleactivity/rawanlinggongyingparam.hpp"

struct RAWanLingGongYingTaskCfg
{
	RAWanLingGongYingTaskCfg():index(0), type(0), param1(0), param2(0), reward_count(0), unfinish_count(0) {}

	short index;				// 任务index
	short type;					// 任务类型
	int param1;
	int param2;

	short reward_count;
	ItemConfigData reward_item[5];			// 完成奖励道济

	short unfinish_count;
	ItemConfigData unfinish_item[5];		// 中途激活活动 领取的未完成奖励
};

struct RAWanLingGongYingOtherCfg
{
	RAWanLingGongYingOtherCfg() :buy_gold(0) {}

	int buy_gold;
};

class RandActivityManager;

class RandActivityWanLingGongYingConfig : public RandActivityConfig
{
public:
	RandActivityWanLingGongYingConfig();
	virtual ~RandActivityWanLingGongYingConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAWanLingGongYingOtherCfg & GetOtherCfg() const { return m_other_cfg; }
	const RAWanLingGongYingTaskCfg * GeTaskCfg(RandActivityManager * ramgr, int index) const;
	const std::vector<int> * GetTaskIndexCfg(RandActivityManager * ramgr, int activity_type) const;
	int GetActivityType(int task_type) const;

private:
	int InitActivityCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitTaskCfg(TiXmlElement *RootElement);

	RAWanLingGongYingOtherCfg m_other_cfg;

	std::map<int, std::vector<RAWanLingGongYingTaskCfg> > m_task_cfg;
	std::map<int, std::map<int, std::vector<int> > > m_task_index_cfg;

	std::vector<int> m_activity_cfg;
};

#endif	//__RAND_ACTIVITY_WAN_LING_GONG_YING_CONFIG_HPP__