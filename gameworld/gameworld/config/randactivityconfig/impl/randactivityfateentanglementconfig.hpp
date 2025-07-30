#ifndef __RAND_ACTIVITY_FATE_ENTENGLEMENT_CONFIG_HPP__ 
#define __RAND_ACTIVITY_FATE_ENTENGLEMENT_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"



enum FATE_ENTANGLEMENT_TYPE
{
	FATE_ENTANGLEMENT_TYPE_BEGIN = 0,
	FATE_ENTANGLEMENT_TYPE_TALK = 1,				// 交谈
	FATE_ENTANGLEMENT_TYPE_DELIVERY = 2,			// 交货
	FATE_ENTANGLEMENT_TYPE_BATTLE = 3,				// 打怪
	FATE_ENTANGLEMENT_TYPE_DELIVERY_PET = 4,		// 寻宠
	FATE_ENTANGLEMENT_TYPE_INSTEAD_OF_SEND = 5,		// 代送

	FATE_ENTANGLEMENT_TYPE_MAX
};

struct RAFateOtherCfg
{
	RAFateOtherCfg() : ring_up(0), space_times(0), level_open(0)
	{}

	int ring_up;			//每日任务上限个数
	int space_times;		//寻宠任务最低间隔环数
	int level_open;			//开启等级
};

struct RAFateTaskInfoCfg
{
	RAFateTaskInfoCfg() :task_id(0), task_type(0), param1(0), param2(0), accept_npc_seq(0), commit_npc_seq(0), rate(0)
	{}

	int task_id;
	int task_type;
	int param1;
	int param2;
	int accept_npc_seq;
	int commit_npc_seq;
	int rate;
};

struct RAFateRewardCfg
{
	RAFateRewardCfg() : rate(0)
	{}

	int rate;
	std::vector<ItemConfigData> reward_list;
};

struct RAFateRewardGroupCfg
{
	RAFateRewardGroupCfg() : rate_count(0)
	{}

	int rate_count;
	std::vector<RAFateRewardCfg> node_list;
};

struct RAFateTaskRewardCfg
{
	RAFateTaskRewardCfg() : ring_down(0), ring_up(0), group_id(0), exp(0), coin(0)
	{}
	static const int EXP_BASE_NUM = 10000;

	int ring_down;
	int ring_up;
	int group_id;
	int exp;			//当前等级万分底系数 
	int coin;
};

struct RAFateTaskRewardSectionCfg
{
	RAFateTaskRewardSectionCfg() : section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<RAFateTaskRewardCfg> node_list;
};

struct RAFateTaskRewardSectionSectionCfg
{
	RAFateTaskRewardSectionSectionCfg(int _start_time, int _end_time) : start_time(_start_time), end_time(_end_time)
	{}

	int start_time;
	int end_time;

	RAOpenDayList open_day_list;
	std::vector<RAFateTaskRewardSectionCfg> m_reward_section_cfg;		// 任务奖励配置
};


class RandActivityManager;
class RandActivityFateEntanglementConfig : public RandActivityConfig
{
public:
	RandActivityFateEntanglementConfig();
	virtual ~RandActivityFateEntanglementConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	int GetOpenDay(RandActivityManager * ramgr) const;
	const RAFateOtherCfg & GetOtherCfg()const { return m_other_cfg; }
	const RAFateTaskInfoCfg * GetTaskInfoCfg(int task_id)const;
	const RAFateTaskInfoCfg * GetRankTaskCfg(int cur_task_id, bool is_can_rand_pet_task, bool is_first_rank_day = false)const;
	const RAFateTaskRewardCfg * GetTaskRewardCfg(RandActivityManager* ramgr, int pass_count)const;
	const RAFateRewardCfg * GetRewardCfg(int group_id)const;
	
private:
	int InitTaskRewardCfg(TiXmlElement *RootElement);
	int InitRewardGroupCfg(TiXmlElement  *RootElement);
	int InitTaskCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);
		
	RAFateOtherCfg m_other_cfg;
	int m_task_rate_count;													// 任务总权重
	std::map<int, RAFateTaskInfoCfg> m_task_map;							// 任务配置	
	std::map<int, RAFateRewardGroupCfg> m_reward_group_cfg;					// 奖励组配置  key:group_id
	std::vector<RAFateTaskRewardSectionSectionCfg> m_reward_section_section_cfg;			// 任务奖励配置
};

#endif //  __RAND_ACTIVITY_FATE_ENTENGLEMENT_CONFIG_HPP__
