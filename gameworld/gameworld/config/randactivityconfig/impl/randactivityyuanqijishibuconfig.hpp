#ifndef __RAND_ACTIVITY_YUAN_QI_JISHIBU_CONFIG_HPP__
#define __RAND_ACTIVITY_YUAN_QI_JISHIBU_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "config/randactivityconfig/randactivityconfig.hpp"


struct RAYuanQiJiShiBuTaskCfg
{
	RAYuanQiJiShiBuTaskCfg() :task_type(0), param(0), combat(0){}

	int task_type;
	int param;
	int combat;
};

struct RAYuanQiJiShiBuRewardCfg
{
	RAYuanQiJiShiBuRewardCfg() :combat(0) {}

	int combat;
	ItemConfigData reward;
	std::vector<ItemConfigData> reward_2;
};

struct RAYuanQiJiShiBuOtherCfg
{
	RAYuanQiJiShiBuOtherCfg() : price(0) {}

	int price;
};


class RandActivityManager;
class RandActivityYuanQiJiShiBuConfig : public RandActivityConfig
{
public:
	RandActivityYuanQiJiShiBuConfig();
	virtual ~RandActivityYuanQiJiShiBuConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const std::vector<RAYuanQiJiShiBuTaskCfg> * GetTaskListCfg(RandActivityManager * ramgr) const;
	const RAYuanQiJiShiBuRewardCfg * GetRewardCfg(RandActivityManager * ramgr, int seq) const;
	const std::vector<RAYuanQiJiShiBuRewardCfg> * GetRewardMap(RandActivityManager * ramgr) const;
	const RAYuanQiJiShiBuOtherCfg * GetOtherCfg() const { return &m_other_cfg; }

private:
	int InitTaskCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	std::map<short, std::vector<RAYuanQiJiShiBuTaskCfg> > m_task_cfg;
	std::map<short, std::vector<RAYuanQiJiShiBuRewardCfg> > m_reward_cfg;
	RAYuanQiJiShiBuOtherCfg m_other_cfg;
};


#endif	// __RAND_ACTIVITY_YUAN_QI_JISHIBU_CONFIG_HPP__