#ifndef __RAND_ACTIVITY_COLORFUL_LANTERN_CONFIG_HPP__
#define __RAND_ACTIVITY_COLORFUL_LANTERN_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "config/randactivityconfig/randactivityconfig.hpp"

enum COLORFUL_LANTERN_TASK_TYPE
{
	COLORFUL_LANTERN_TASK_TYPE_BEGIN = 0,
	COLORFUL_LANTERN_TASK_TYPE_DAY = 1,				//每日任务
	COLORFUL_LANTERN_TASK_TYPE_ACTIVITY = 2,		//活动任务

	COLORFUL_LANTERN_TASK_TYPE_MAX
};

enum COLORFUL_LANTERN_TASK_MODE
{
	COLORFUL_LANTERN_TASK_MODE_BEGIN = 0, 
	COLORFUL_LANTERN_TASK_MODE_SINGLE_ENTER_SCORE = 1,			// 单次积分任务
	COLORFUL_LANTERN_TASK_MODE_ADD_UP_SCORE = 2,				// 累计积分
	COLORFUL_LANTERN_TASK_MODE_ENTER_NUM = 3,					// 参与次数

	COLORFUL_LANTERN_TASK_MODE_MAX
};

struct RAColorfulLanternOtherCfg
{
	RAColorfulLanternOtherCfg() : refresh_time(0), refresh_num_down(0), refresh_num_up(0)
	{}

	int refresh_time;
	int refresh_num_down;
	int refresh_num_up;
	int game_time;				//游戏持续时间
};

struct RAColorfulLanternBaseCfg
{
	RAColorfulLanternBaseCfg() : task_type(0), task_param(0), param1(0), param2(0)
	{}

	int task_type;					//COLORFUL_LANTERN_TASK_TYPE_MAX
	int task_param;					//COLORFUL_LANTERN_TASK_MODE_MAX
	int param1;
	int param2;
	ItemConfigData reward_item;
};

struct RAColorfulLanternSectionBaseCfg
{
	RAColorfulLanternSectionBaseCfg() : section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::map<int, RAColorfulLanternBaseCfg> m_task_cfg;			//key:task_id
};

struct RAColorfulLanternLightCfg
{
	RAColorfulLanternLightCfg() : score(0), rate(0)
	{}

	int score;
	int rate;
};

class RandActivityManager;
class RandActivityColorfulLanternConfig : public RandActivityConfig
{
public:
	RandActivityColorfulLanternConfig();
	virtual ~RandActivityColorfulLanternConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);
	
	const RAColorfulLanternOtherCfg & GetOtherCfg()const { return m_other_cfg; }
	const RAColorfulLanternBaseCfg * GetBaseCfg(RandActivityManager* ramgr, int task_id)const;
	int GetLightScoreNum(int type)const;
	void GetRefreshLightList(std::vector<int> & light_list)const;
	void GetTaskList(RandActivityManager * ramgr, std::map<int, RAColorfulLanternBaseCfg> & task_list)const;

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitBaseCfg(TiXmlElement *RootElement);
	int InitLightCfg(TiXmlElement *RootElement);

	RAColorfulLanternOtherCfg m_other_cfg;
	std::vector<RAColorfulLanternSectionBaseCfg> m_section_base_cfg;

	int rate_count;
	std::map<int, RAColorfulLanternLightCfg> m_light_cfg;		//花灯配置 key:type
};	


#endif	// __RAND_ACTIVITY_COLORFUL_LANTERN_CONFIG_HPP__