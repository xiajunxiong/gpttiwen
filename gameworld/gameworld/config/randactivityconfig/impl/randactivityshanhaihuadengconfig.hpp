#ifndef __RAND_ACTIVITY_SHANHAI_HUADENG_CONFIG_HPP__
#define __RAND_ACTIVITY_SHANHAI_HUADENG_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "config/randactivityconfig/randactivityconfig.hpp"

enum SHANHAI_HUADENG_TASK_TYPE
{
	SHANHAI_HUADENG_TASK_TYPE_BEGIN = 0,
	SHANHAI_HUADENG_TASK_TYPE_DAY = 1,				//每日任务
	SHANHAI_HUADENG_TASK_TYPE_ACTIVITY = 2,			//活动任务

	SHANHAI_HUADENG_TASK_TYPE_MAX
};

enum SHANHAI_HUADENG_TASK_MODE
{
	SHANHAI_HUADENG_TASK_MODE_BEGIN = 0, 
	SHANHAI_HUADENG_TASK_MODE_SINGLE_ENTER_SCORE = 1,			// 单次积分任务
	SHANHAI_HUADENG_TASK_MODE_ADD_UP_SCORE = 2,					// 累计积分
	SHANHAI_HUADENG_TASK_MODE_ENTER_NUM = 3,					// 参与次数

	SHANHAI_HUADENG_TASK_MODE_MAX
};

struct RAShanHaiHuaDengOtherCfg
{
	RAShanHaiHuaDengOtherCfg() : refresh_time(0), refresh_num_down(0), refresh_num_up(0)
	{}

	int refresh_time;
	int refresh_num_down;
	int refresh_num_up;
	int game_time;				//游戏持续时间
};

struct RAShanHaiHuaDengBaseCfg
{
	RAShanHaiHuaDengBaseCfg() : task_type(0), task_param(0), param1(0), param2(0)
	{}

	int task_type;					//SHANHAI_HUADENG_TASK_TYPE_MAX
	int task_param;					//SHANHAI_HUADENG_TASK_MODE_MAX
	int param1;
	int param2;
	ItemConfigData reward_item;
};

struct RAShanHaiHuaDengSectionBaseCfg
{
	RAShanHaiHuaDengSectionBaseCfg() : section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::map<int, RAShanHaiHuaDengBaseCfg> m_task_cfg;			//key:task_id
};

struct RAShanHaiHuaDengLightCfg
{
	RAShanHaiHuaDengLightCfg() : score(0), rate(0)
	{}

	int score;
	int rate;
};

class RandActivityManager;
class RandActivityShanHaiHuaDengConfig : public RandActivityConfig
{
public:
	RandActivityShanHaiHuaDengConfig();
	virtual ~RandActivityShanHaiHuaDengConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);
	
	const RAShanHaiHuaDengOtherCfg & GetOtherCfg()const { return m_other_cfg; }
	const RAShanHaiHuaDengBaseCfg * GetBaseCfg(RandActivityManager* ramgr, int task_id)const;
	int GetLightScoreNum(int type)const;
	void GetRefreshLightList(std::vector<int> & light_list)const;
	void GetTaskList(RandActivityManager * ramgr, std::map<int, RAShanHaiHuaDengBaseCfg> & task_list)const;

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitBaseCfg(TiXmlElement *RootElement);
	int InitLightCfg(TiXmlElement *RootElement);

	RAShanHaiHuaDengOtherCfg m_other_cfg;
	std::vector<RAShanHaiHuaDengSectionBaseCfg> m_section_base_cfg;

	int rate_count;
	std::map<int, RAShanHaiHuaDengLightCfg> m_light_cfg;		//花灯配置 key:type
};	


#endif	// __RAND_ACTIVITY_SHANHAI_HUADENG_CONFIG_HPP__