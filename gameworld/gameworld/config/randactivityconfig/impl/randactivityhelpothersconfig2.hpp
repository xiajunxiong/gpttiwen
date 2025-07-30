#ifndef __RAND_ACTIVITY_HELP_OTHERS_CONFIG_2_HPP__
#define __RAND_ACTIVITY_HELP_OTHERS_CONFIG_2_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/rahelpothersparam.hpp"


enum HELP_OTHERS_TASK_TYPE_2
{
	HELP_OTHERS_TASK_TYPE_2_BEGIN = 0,					
	HELP_OTHERS_TASK_TYPE_2_TALK = 1,							// 交谈
	HELP_OTHERS_TASK_TYPE_2_DELIVERY = 2,						// 交货
	HELP_OTHERS_TASK_TYPE_2_BATTLE = 3,						// 打怪
	HELP_OTHERS_TASK_TYPE_2_DELIVERY_PET = 4,					// 寻宠
	HELP_OTHERS_TASK_TYPE_2_INSTEAD_OF_SEND = 5,				// 代送

	HELP_OTHERS_TASK_TYPE_2_MAX
};


struct RAHelpOthersOtherCfg2
{
	RAHelpOthersOtherCfg2() : ring_up(0), space_times(0)
	{}

	int ring_up;			//每日任务上限个数
	int space_times;		//寻宠任务最低间隔环数
	int level_open;			//开启等级
};

struct RAHelpOthersTaskInfoCfg2
{
	RAHelpOthersTaskInfoCfg2() :task_id(0), task_type(0), param1(0), param2(0), accept_npc_seq(0), commit_npc_seq(0), rate(0)
	{}

	int task_id;
	int task_type;
	int param1;
	int param2;
	int accept_npc_seq;
	int commit_npc_seq;
	int rate;
};

struct RAHelpOthersRewardCfg2
{
	RAHelpOthersRewardCfg2() : rate(0)
	{}

	int rate;
	std::vector<ItemConfigData> reward_list;
};

struct RAHelpOthersRewardGroupCfg2
{
	RAHelpOthersRewardGroupCfg2(): rate_count(0)
	{}

	int rate_count;
	std::vector<RAHelpOthersRewardCfg2> node_list;
};

struct RAHelpOthersTaskRewardCfg2
{
	RAHelpOthersTaskRewardCfg2() : ring_down(0), ring_up(0), group_id(0), exp(0), coin(0)
	{}
	static const int EXP_BASE_NUM = 10000;

	int ring_down;
	int ring_up;
	int group_id;
	int exp;			//当前等级万分底系数 
	int coin;
};

struct RAHelpOtherTaskRewardSectionCfg2
{
	RAHelpOtherTaskRewardSectionCfg2(): section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<RAHelpOthersTaskRewardCfg2> node_list;
};

struct RAHelpOtherTaskRewardSectionSectionCfg2
{
	RAHelpOtherTaskRewardSectionSectionCfg2(int _start_time, int _end_time) : start_time(_start_time), end_time(_end_time)
	{}
		
	int start_time;
	int end_time;

	RAOpenDayList open_day_list;
	std::vector<RAHelpOtherTaskRewardSectionCfg2> m_reward_section_cfg;		// 任务奖励配置
};

class RandActivityManager;
class RandActivityHelpOthersConfig2 : public RandActivityConfig
{
public:
	RandActivityHelpOthersConfig2();
	virtual ~RandActivityHelpOthersConfig2();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);
	
	int GetOpenDay(RandActivityManager * ramgr) const;
	const RAHelpOthersOtherCfg2 & GetOtherCfg()const { return m_other_cfg; }
	const RAHelpOthersTaskInfoCfg2 * GetTaskInfoCfg(int task_id)const;
	const RAHelpOthersTaskInfoCfg2 * GetRankTaskCfg(int cur_task_id, bool is_can_rand_pet_task, bool is_first_rank_day = false)const;
	const RAHelpOthersTaskRewardCfg2* GetTaskRewardCfg(RandActivityManager* ramgr, int pass_count)const;
	const RAHelpOthersRewardCfg2 * GetRewardCfg(int group_id)const;

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitTaskCfg(TiXmlElement *RootElement);
	int InitRewardGroupCfg(TiXmlElement  *RootElement);
	int InitTaskRewardCfg(TiXmlElement *RootElement);

	RAHelpOthersOtherCfg2 m_other_cfg;
	int m_task_rate_count;													// 任务总权重
	std::map<int, RAHelpOthersTaskInfoCfg2> m_task_map;						// 任务配置	
	std::map<int, RAHelpOthersRewardGroupCfg2> m_reward_group_cfg;			// 奖励组配置  key:group_id
	std::vector<RAHelpOtherTaskRewardSectionSectionCfg2> m_reward_section_section_cfg;
};

#endif	//__RAND_ACTIVITY_HELP_OTHERS_CONFIG_HPP__
