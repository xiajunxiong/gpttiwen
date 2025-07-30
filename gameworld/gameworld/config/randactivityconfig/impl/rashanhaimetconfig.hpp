#ifndef __RA_SHAN_HAI_MET_CONFIG_HPP__
#define __RA_SHAN_HAI_MET_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itembase.h"
#include "servercommon/roleactivity/rashanhaimetparam.hpp"

class RandActivityManager;

namespace RAShanHaiMet
{
	struct OtherCfg
	{
		OtherCfg()
		{
			high_reward_buy_money = 0;
			cfg_ver = 0;
		}
		int high_reward_buy_money;
		int cfg_ver;
	};
	//每日奖励配置
	struct DailyCfg
	{
		DailyCfg()
		{
			section_day_begin = 0;

			act_day_index = 0;
			task_group_id = 0;
		}
		int section_day_begin;	//分段开始的开服天数 (当天为1)

		int act_day_index;		//活动天数index(当天为0)
		int task_group_id;		//任务组id
	
		std::vector<ItemConfigData> normal_reward_vec;		//通常奖励
		std::vector<ItemConfigData> unfinish_reward_vec;	//未完成奖励
		std::vector<ItemConfigData> extra_reward_vec;		//额外奖励
	};
	//阶段配置map<活动天数index, 每日配置>
	typedef std::map<int, DailyCfg> SectionCfgDayMap;
	//活动配置阶段map<开服天数begin, 阶段配置>
	typedef std::map<int, SectionCfgDayMap> SectionMap;
	//奖励类型
	enum DailyRewardType
	{
		DAILY_REWARD_TYPE_NONE = 0,	//无
		DAILY_REWARD_TYPE_NORMAL,	//通常奖励
		DAILY_REWARD_TYPE_UNFINISH,	//未完成奖励
		DAILY_REWARD_TYPE_EXTRA,	//额外奖励			(屏蔽)
		DAILY_REWARD_TYPE_ALL,		//通常奖励+额外奖励	(屏蔽)

		DAILY_REWARD_TYPE_MAX,
	};

	//任务配置
	struct TaskCfg
	{
		TaskCfg()
		{
			task_group_id = 0;
			task_seq = 0;

			task_type = 0;
			task_param_1 = 0;
			task_param_2 = 0;
		}

		int task_group_id;		//任务组id
		int task_seq;			//任务序号

		int task_type;			//任务类型

		int task_param_1;		//任务参数
		int task_param_2;		//任务参数
	};
	//任务组信息<任务类型, 任务信息>
	typedef std::map<int, TaskCfg> TaskGroupCfg;

	//任务组id map<任务组id, 任务组信息>
	typedef std::map<int, TaskGroupCfg> TaskGroupIDMap;


};

class RAShanHaiMetConfig : public RandActivityConfig
{
public:
	RAShanHaiMetConfig();
	virtual ~RAShanHaiMetConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAShanHaiMet::OtherCfg & GetOtherCfg() const { return m_other_cfg; };
	//任务
	const RAShanHaiMet::DailyCfg * GetDailyCfg(RandActivityManager* ramgr, int act_day_index) const;

	virtual int GetConfigVer() const;

	//任务完成
	//任务组信息
	const RAShanHaiMet::TaskGroupCfg * GetTaskGroupCfg(int task_group_id) const;

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitTaskGroupCfg(TiXmlElement *RootElement);
	int InitDailyCfg(TiXmlElement *RootElement);


	RAShanHaiMet::OtherCfg m_other_cfg;
	RAShanHaiMet::SectionMap m_section_map;
	RAShanHaiMet::TaskGroupIDMap m_task_group_id_cfg;
};

#endif		//__RA_SHAN_HAI_JU_BAO_CONFIG_HPP__