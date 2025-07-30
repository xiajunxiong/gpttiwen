#ifndef __RAND_ACTIVITY_QIYUAN_KONGMINGDENG_CONFIG_HPP__
#define __RAND_ACTIVITY_QIYUAN_KONGMINGDENG_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "config/randactivityconfig/randactivityconfig.hpp"


struct RAQiYuanKongMingDengTaskCfg
{
	RAQiYuanKongMingDengTaskCfg() :task_type(0), param(0) {}

	int task_type;
	int param;
};

struct RAQiYuanKongMingDengRewardCfg
{
	RAQiYuanKongMingDengRewardCfg() {}

	std::vector<ItemConfigData> reward;
};

struct RAQiYuanKongMingDengOtherCfg
{
	RAQiYuanKongMingDengOtherCfg() : time_start(0), time_end(0), reward_increase(0), reward_consume(0) {}

	unsigned int time_start;				// 开始时间
	unsigned int time_end;					// 结束时间

	int reward_increase;					// 任务奖励火种次数
	int reward_consume;						// 点灯消耗火种次数
};


class RandActivityManager;
class RandActivityQiYuanKongMingDengConfig : public RandActivityConfig
{
public:
	RandActivityQiYuanKongMingDengConfig();
	virtual ~RandActivityQiYuanKongMingDengConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const std::vector<RAQiYuanKongMingDengTaskCfg> * GetTaskListCfg(RandActivityManager * ramgr) const;
	const RAQiYuanKongMingDengRewardCfg * GetRewardCfg(RandActivityManager * ramgr) const;
	const RAQiYuanKongMingDengOtherCfg * GetOtherCfg() const { return &m_other_cfg; }

private:
	int InitTaskCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	std::map<short, std::vector<RAQiYuanKongMingDengTaskCfg> > m_task_cfg;
	std::map<short, RAQiYuanKongMingDengRewardCfg> m_reward_cfg;
	RAQiYuanKongMingDengOtherCfg m_other_cfg;
};


#endif	// __RAND_ACTIVITY_QIYUAN_KONGMINGDENG_CONFIG_HPP__