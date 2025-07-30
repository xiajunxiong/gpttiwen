#ifndef __RESEARCH_TASK_CONFIG_HPP__
#define __RESEARCH_TASK_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"

#include <algorithm>

#include <map>
#include <set>
#include <vector>

static const int RESEARCH_TASK_TIMES_PER_ROUND = 10;			// 写死，十次一轮
static const int RESEARCH_TASK_AWARD_MAX_COUNT = 5;
static const int RESEARCH_TASK_AWARD_MAX_SEQID = 32;

struct ResearchTaskOtherCfg		// 其它配置
{
	ResearchTaskOtherCfg() : daily_refresh_time(0), one_key_cost_per_times(0), one_key_auto_max_times(0), 
		max_daily_task_times(0), team_need_member_min(0), gather_power_award(0) {}

	int daily_refresh_time;			// 每日刷新时间 - 默认每日零点刷新
	int one_key_cost_per_times;		// 一键完成每次消耗
	int one_key_auto_max_times;		// 一键完成自动次数
	int max_daily_task_times;		// 每日进行任务的最大次数
	int team_need_member_min;		// 最小组队人数
	int gather_power_award;			// 单次任务奖励活力值
};

struct ResearchTaskCfg		// 任务配置
{
	ResearchTaskCfg() : task_id(0), lv_limit_min(0), lv_limit_max(0), 
		task_npc_seq(0), scene_group(0), demon_group(0) {}

	int task_id;			// 任务ID
	int lv_limit_min;		// 最小等级限制
	int lv_limit_max;		// 最大等级限制
	int task_npc_seq;		// 任务相关NPC（接收与提交是同一个NPC，完成一轮才提交）

	int scene_group;		// 随机场景组
	int demon_group;		// 随机怪物组
};

struct ResearchRewardCfg	// 单个任务奖励配置
{
	ResearchRewardCfg() : experience(0), coin_bind(0) {}

	int experience;
	int coin_bind;
};

struct RandSceneCon		// 场景随机组
{
	RandSceneCon() : scene_id(0), rand_rate(0), pos_group_id(0) {}

	int scene_id;		// 场景ID
	int rand_rate;		// 随机概率
	int pos_group_id;	// 对应坐标组
};

struct RandDemonCon		// 怪物随机组 
{
	RandDemonCon() : img_id(0), battle_group_id(0), rand_rate(0) {}

	int img_id;				// 用于场景中显示的怪物ID
	int battle_group_id;	// 用于战斗的怪物组ID
	int rand_rate;			// 随机概率
};

struct RandPosCon
{
	RandPosCon() : pos_group_id(0), pos_x(0), pos_y(0), rand_rate(0) {}

	int pos_group_id;
	int pos_x;
	int pos_y;
	int rand_rate;
};

struct RandAwardCon		// 轮次奖励组
{
	RandAwardCon() : give_rate(0) {}

	int give_rate;					// 给予概率
	ItemConfigData award_item;		// 奖励道具
};

struct RTRandGroupCon
{
	RTRandGroupCon() : prob(0) {}

	int prob;
	ItemConfigData item;
};

struct AwardBackCon
{
	AwardBackCon() : experience(0), coin_bind(0), family_contribution(0), item_group(0), find_type(0){}

	int experience;
	int coin_bind;
	int family_contribution;
	int item_group;
	int find_type;
};

struct RTRoundRewardCon
{
	RTRoundRewardCon() : reward_group(0), rate(0) {}
	int reward_group;
	int rate;
};

struct RTRoundRewardCfg
{
	RTRoundRewardCfg() : rate(0) {}

	int rate;
	ItemConfigData reward;
};

struct RTRoundRewardGroupCfg
{
	RTRoundRewardGroupCfg() : weight(0) {}

	int weight;
	std::vector<RTRoundRewardCfg> m_reward_vec;
};

class ResearchTaskConfig : public ILogicConfig
{
public:
	ResearchTaskConfig();
	virtual ~ResearchTaskConfig();

	virtual bool Init(const std::string & configname, std::string * err);

	const ResearchTaskOtherCfg & GetOtherConfig() { return m_zzt_other_cfg; }

	const ResearchTaskCfg * GetTaskByLevel(int level);
	const ResearchTaskCfg * GetTaskByTaskID(int task_id);
	const ResearchRewardCfg * GetTaskReward(int role_level);
	const AwardBackCon * GetRewardBack(int role_level, int back_type);
	const ItemConfigData * RandBackItem(int group_id);

	bool GetRandSceneAndDemonByTaskID(int task_id, int * scene_id, int * demon_id, int * image_id, ARG_OUT Posi* pos);

	void GetAwardTimesListByFinishTimes(int finish_times, std::set<int> & out_list);
	int GetAwardSeqByAwardNeedTimes(int need_times);	// 从 0 开始
	bool RandAwardList(int level, int need_times, int * out_list_len, ItemConfigData * out_award_list);
	const AwardBackCon* GetFindType(int _role_level);

	const std::vector<RTRoundRewardCon> * RandRewardListRoundCfg(int need_times, int role_level);
	const RTRoundRewardCfg * RandRoundRewardCfg(int group_id);
private:
	int InitZTaskOtherCfg(TiXmlElement * RootElement);
	int InitTasksBasicCfg(TiXmlElement * RootElement);
	int InitSceneGroupCfg(TiXmlElement * RootElement);
	int InitDemonGroupCfg(TiXmlElement * RootElement);
	int InitPosGroupCfg(TiXmlElement * RootElement);
	int InitAwardGroupCfg(TiXmlElement * RootElement);
	int InitTaskRewardCfg(TiXmlElement * RootElement);
	int InitBackGroupsCfg(TiXmlElement * RootElement);
	int InitRewardBackCfg(TiXmlElement * RootElement);
	int InitOneRundRewardCfg(TiXmlElement * RootElement);
	int InitRundRewardGroupCfg(TiXmlElement * RootElement);

	bool IsTimerCfg(int check_time);

	ResearchTaskOtherCfg m_zzt_other_cfg;
	std::map<int, ResearchTaskCfg> m_research_task_cfg;				// 键 - 任务ID

	std::map<int, std::vector<RandSceneCon> > m_rand_scene_map;
	std::map<int, std::vector<RandDemonCon> > m_rand_demon_map;
	std::map<int, std::vector<RandPosCon> > m_rand_pos_map;
	std::map<int, std::map<ParamSection, std::vector<RandAwardCon> > > m_rand_award_map;
	std::map<ParamSection, ResearchRewardCfg> m_task_reward_map;

	std::map<int, std::map<ParamSection, std::vector<RTRoundRewardCon> > > m_round_reward_map;
	std::map<int, RTRoundRewardGroupCfg> m_round_reward_group_map;

	// 奖励找回
	std::map<int, std::vector<RTRandGroupCon> > m_back_item_group_map;			// 找回道具组：键 - 组ID
	std::map<ParamSection, std::map<int, AwardBackCon> > m_reward_back_map;		// 奖励找回表：键 - 等级区间、找回类型
	std::map<int, AwardBackCon> m_seq_to_rrcfg;
};

#endif