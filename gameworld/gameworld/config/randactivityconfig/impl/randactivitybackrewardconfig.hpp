#ifndef __RAND_ACTIVITY_BACK_REWARD_CONFIG_HPP__
#define __RAND_ACTIVITY_BACK_REWARD_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/raanimalraceparam.hpp"
#include "item/itembase.h"
#include "servercommon/roleactivity/rabackrewardparam.hpp"

struct RABackRewardDayCfg
{
	RABackRewardDayCfg() : count(0) {}

	int count;
	ItemConfigData reward_item[8];
};

struct RABackRewardTaskCfg
{
	RABackRewardTaskCfg() : activity_days(0), task_type(0), task_id(0), param1(0), param2(0) {}

	short activity_days;
	short task_type;
	short task_id;
	short param1;
	short param2;

	ItemConfigData reward_item;
};

struct RABackRewardOtherCfg
{
	RABackRewardOtherCfg() : open_level(0), back_days(0) {}

	short open_level;
	short back_days;
};

struct RABackRewardTaskMap
{
	std::map<int, std::vector<RABackRewardTaskCfg> > task_type_list;
	std::vector<RABackRewardTaskCfg> task_cfg;
};

class RandActivityManager;

class RandActivityBackRewardConfig : public RandActivityConfig
{
public:
	RandActivityBackRewardConfig();
	virtual ~RandActivityBackRewardConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RABackRewardDayCfg * GetDayCfg(RandActivityManager * ramgr, int day_index) const;
	const RABackRewardTaskCfg * GetTaskCfg(RandActivityManager * ramgr, int task_index) const;
	const std::vector<RABackRewardTaskCfg> * GetTaskTypeListCfg(RandActivityManager * ramgr, int task_type) const;
	const RABackRewardOtherCfg * GetOtherCfg() const { return &m_other_cfg; }

private:
	int InitDayCfg(TiXmlElement *RootElement);
	int InitTaskCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	std::map<int, std::vector<RABackRewardDayCfg> > m_day_cfg;				//<最小开始时间, <活动天数> >
	std::map<int, RABackRewardTaskMap> m_task_cfg;			//<最小开始时间, <任务ID> >
	RABackRewardOtherCfg m_other_cfg;
};

#endif	//__RAND_ACTIVITY_BACK_REWARD_CONFIG_HPP__