#ifndef __RAND_ACTIVITY_COOL_BREEZE_CONFIG_HPP__
#define __RAND_ACTIVITY_COOL_BREEZE_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/racoolsummerparam.hpp"

struct RACoolBreezeOtherCfg
{
	RACoolBreezeOtherCfg()	: refresh_num(0), fixe_refresh_time(0), fixe_refresh_hour(0), fixe_refresh_minute(0), random_refresh_time(0), random_refresh_hour(0), 
		random_refresh_minute(0), gold(0)
	{}

	int refresh_num;
	int fixe_refresh_time;
	int fixe_refresh_hour;
	int fixe_refresh_minute;
	int random_refresh_time;
	int random_refresh_hour;
	int random_refresh_minute;
	int gold;
};

struct RACoolBreezeTaskCfg
{
	RACoolBreezeTaskCfg() : type(0), task_group(0), task_type(0), param1(0), param2(0), rate(0)
	{}

	int type;
	int task_group;
	int task_type;
	int param1;
	int param2;
	int rate;
	std::vector<ItemConfigData> reward_list;
};

struct RACoolBreezeTaskListCfg
{
	RACoolBreezeTaskListCfg() : rate_count(0)
	{}

	int rate_count;
	std::vector<RACoolBreezeTaskCfg> task_cfg_list;
};

class RandActivityManager;
class RandActivityCoolBreezeConfig : public RandActivityConfig
{
public:
	RandActivityCoolBreezeConfig();
	virtual ~RandActivityCoolBreezeConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);
	const RACoolBreezeOtherCfg & GetOtherCfg()const { return m_other_cfg; }
	const RACoolBreezeTaskCfg* GetTaskCfgByType(int type)const;
	void RandTask(int task_group_id, RACoolBreezeTaskInfo& get_task, std::vector<int>* has_type_list = NULL)const;

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitTaskListCfg(TiXmlElement *RootElement);

	RACoolBreezeOtherCfg m_other_cfg;
	std::map<int, RACoolBreezeTaskListCfg> m_task_list_map;			//任务列表	key:task_group
	std::map<int, RACoolBreezeTaskCfg> m_task_map;					//任务列表	key:type
	
};

#endif