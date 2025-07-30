#ifndef __RAND_ACTIVITY_YUAN_QI_HELP_CONFIG_HPP__
#define __RAND_ACTIVITY_YUAN_QI_HELP_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/rayuanqihelpparam.hpp"

struct YuanQiHelpTaskCfg
{
	YuanQiHelpTaskCfg() : task_id(0), item_id(0) {}
	int task_id;
	int	item_id;
};

struct YuanQiHelpRewardCfg
{
	YuanQiHelpRewardCfg() {}

	std::vector<ItemConfigData> m_reward_vec;
};

struct YuanQiHelpRewardListCfg
{
	YuanQiHelpRewardListCfg() {}

	std::map<int, YuanQiHelpRewardCfg> m_reward_group;
};

struct YuanQiHelpOtherCfg
{
	YuanQiHelpOtherCfg() : frequency(0) {}

	int frequency;
};

class RandActivityManager;
class RandActivityYuanQiHelpConfig : public RandActivityConfig
{
public:
	RandActivityYuanQiHelpConfig();
	virtual ~RandActivityYuanQiHelpConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const YuanQiHelpOtherCfg & GetOtherCfg() const { return m_other_cfg; }
	const YuanQiHelpRewardCfg * GetRewardCfg(int open_day, int seq) const;
	const YuanQiHelpTaskCfg * RandTaskCfg(int pre_task_id[MAX_YUAN_QI_HELP_MAX_TASK_LIST]) const;
	const YuanQiHelpTaskCfg * GetTaskCfg(int task_id) const;
private:
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitRewardCfg(TiXmlElement * RootElement);
	int InitTaskCfg(TiXmlElement * RootElement);

	YuanQiHelpOtherCfg m_other_cfg;
	std::vector<YuanQiHelpTaskCfg> m_task_vec;
	std::map<int, YuanQiHelpRewardListCfg> m_reward_map;
};

#endif