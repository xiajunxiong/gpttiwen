#ifndef __RA_COLORFUL_DOUBLE_CONFIG_HPP__
#define __RA_COLORFUL_DOUBLE_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itembase.h"
#include "servercommon/roleactivity/racolorfuldoubleparam.hpp"

class RandActivityManager;

namespace RAColorfulDouble
{

	struct OtherCfg
	{
		OtherCfg()
		{
			act_type = 0;

			act_cost_type = 0;
			act_cost_num = 0;
			cfg_ver = 0;

			box_act_reward_type = 0;
			box_act_reward_num = 0;
		}
		//宝箱激活配置
		int act_type;		//激活类型

		int act_cost_type;	//激活消耗类型
		int act_cost_num;	//激活消耗数量
		int cfg_ver;		//直购配置版本

		int box_act_reward_type;//激活奖励类型
		int box_act_reward_num;	//激活奖励数量

		//激活类型
		enum ActType
		{
			ACT_TYPE_BOX = 1,	//激活单个宝箱
			ACT_TYPE_TOTAL,		//全部激活

			ACT_TYPE_MAX,
		};
		//激活消耗类型
		enum ActCostType
		{
			ACT_COST_TYPE_GOLD = 0,			//元宝
			ACT_COST_TYPE_CMD_BUY = 1,		//直购
			ACT_COST_TYPE_SILVER_COIN = 2,	//银两(灵玉)

			ACT_COST_TYPE_MAX,
		};

		//奖励类型
		enum ActRewardType
		{
			ACT_REWARD_TYPE_NONE = 0,	//无激活奖励
			ACT_REWARD_TYPE_GOLD,		//元宝
			ACT_REWARD_TYPE_SILVER_COIN,//银两(灵玉)
			ACT_REWARD_TYPE_COIN_BIND,	//铜币

			ACT_REWARD_TYPE_MAX,
		};
	};

	//免费奖励
	struct FreeRewardCfg
	{
		FreeRewardCfg()
		{
			section_day_begin = 0;
		}

		int section_day_begin;	//分段开始的开服天数 (当天为1)
		//免费奖励
		std::vector<ItemConfigData> free_reward_vec;
	};
	//每日免费奖励阶段map<分段开始的开服天数, 奖励>
	typedef std::map<int, FreeRewardCfg> FreeSectionMap;

	//宝箱奖励
	struct BosRewardCfg
	{
		BosRewardCfg()
		{
			box_seq = 0;
			section_day_begin = 0;

			need_act_open_day = 0;
			need_finish_task_num = 0;
		}
		int box_seq;	//宝箱seq

		int section_day_begin;	//分段开始的开服天数 (当天为1)

		//宝箱奖励
		std::vector<ItemConfigData> box_reward_vec;

		//限制
		int need_act_open_day;		//活动开启天数(第一天为1)
		int need_finish_task_num;	//活动期间完成任务数
	};
	//宝箱阶段信息<活动开启的开服时间, 宝箱奖励>
	typedef std::map<int, BosRewardCfg> BoxSectionDayMap;
	//宝箱信息map<宝箱seq, 宝箱阶段信息>
	typedef std::map<int, BoxSectionDayMap> BoxSeqMap;

	//任务信息
	struct TaskCfg
	{
		TaskCfg()
		{
			act_open_day = 0;
			task_index = 0;

			task_type = 0;
			task_param = 0;
		}

		int act_open_day;	//活动开启天数(第一天为1)

		int task_index;		//储存位置

		int task_type;		//任务类型
		int task_param;		//任务参数
	};
	typedef std::vector<TaskCfg> TaskVec;

	//任务配置-类型map<任务类型, 任务配置vec>
	typedef std::map<int, TaskVec> TaskTypeMap;

	//任务信息-活动天数map<活动天数, 任务配置-类型map>
	typedef std::map<int, TaskTypeMap> TaskDayMap;

};

class RAColorfulDoubleConfig : public RandActivityConfig
{
public:
	RAColorfulDoubleConfig();
	virtual ~RAColorfulDoubleConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAColorfulDouble::OtherCfg& GetOtherCfg() const { return m_other_cfg; };

	const RAColorfulDouble::FreeRewardCfg* GetFreeRewardCfg(RandActivityManager* ramgr) const;
	const RAColorfulDouble::BosRewardCfg* GetBoxRewardCfg(RandActivityManager * ramgr, 
		unsigned int act_begin_time, int box_seq) const;

	const RAColorfulDouble::TaskVec* GetTaskVecByType(RandActivityManager* ramgr, int act_open_day, int task_type) const;


private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitFreeRewardCfg(TiXmlElement *RootElement);
	int InitBoxCfg(TiXmlElement *RootElement);
	int InitTaskCfg(TiXmlElement *RootElement);

	RAColorfulDouble::OtherCfg m_other_cfg;

	//每日免费奖励阶段map<分段开始的开服天数, 奖励>
	RAColorfulDouble::FreeSectionMap m_free_section_day_map;

	//宝箱信息map<宝箱seq, 宝箱阶段信息>
	RAColorfulDouble::BoxSeqMap m_box_seq_map;

	//任务信息-活动天数map<活动天数, 任务配置-类型map>
	RAColorfulDouble::TaskDayMap m_task_day_map;
};


#endif