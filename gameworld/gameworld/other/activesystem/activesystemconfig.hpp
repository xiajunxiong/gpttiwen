#ifndef __ACTIVE_SYSTEM_CONFIG_HPP__
#define __ACTIVE_SYSTEM_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/activesystemdef.hpp"


struct ActiveSystemCommonCfg
{
	ActiveSystemCommonCfg() :reward_coin(0), reward_energy(0), reward_exp(0), level_min(0), level_max(0),reward_num(0), open_day(0) 
	{}

	int reward_coin;			//奖励铜币
	int reward_energy;			//奖励体力
	int reward_exp;				//奖励经验
	int level_min;
	int level_max;
	int reward_num;				//奖励数
	int open_day;
	ItemConfigData reward_list[REWARD_LIST_ITEM_NUM];	//奖励列表
};

struct ActiveSystemCfg
{
	ActiveSystemCfg() :type(0), need_count(0), integral_active(0), is_every_time(0)
	{}

	int type;
	int need_count;				//次数
	int integral_active;        //完成奖励的总活跃度
	int is_every_time;			//单次完成发放还是全部完成发放
	ActiveSystemCommonCfg reward_cfg;
};

struct ActiveSystemListCfg
{
	ActiveSystemListCfg() {}

	std::vector<ActiveSystemCfg> m_reward_vec;
};

struct ActiveSystemSpecialCfg
{
	ActiveSystemSpecialCfg() : special_type(0), need_count(0)
	{}

	int special_type;
	int need_count;
	ActiveSystemCommonCfg reward_cfg;
};

struct ActiveSystemSpecialListCfg
{
	ActiveSystemSpecialListCfg() {}

	std::vector<ActiveSystemSpecialCfg> m_reward_vec;
};

struct ActiveSystemRewardCfg
{
	ActiveSystemRewardCfg():seq(0), integral_active(0), coin(0), gold(0), power(0), exp(0), count(0)
	{}

	int seq;
	int integral_active;             //所需活跃度
	int coin;
	int gold;
	int power;
	int exp;
	int count;
	ItemConfigData reward[ACTIVE_SYSTEM_REWARD_MAX_NUM];
};

struct ActiveSystemLevelGroupCfg
{
	ActiveSystemLevelGroupCfg():level_min(0), level_max(0)
	{}

	int level_min;
	int level_max;

	ActiveSystemRewardCfg node_cfg[ACTIVE_SYSTEM_LEVEL_GROUP_MAX_NUM];
};

struct ActiveSystemRelatedCfg
{
	ActiveSystemRelatedCfg() :seq(0), count(0)
	{}

	int seq;
	int count;
	ItemConfigData reward[ACTIVE_SYSTEM_REWARD_MAX_NUM];
};

struct ActiveSystemRelatedActCfg
{
	ActiveSystemRelatedActCfg()
	{}

	ActiveSystemRelatedCfg node_cfg[ACTIVE_SYSTEM_LEVEL_GROUP_MAX_NUM];
};

// 周常 任务
struct WeekActiveDegreeTaskCfg
{
	WeekActiveDegreeTaskCfg()
	{
		task_type = ACTIVE_DEGREE_TYPE_BEGIN;
		task_param = 0;
		week_active_degree = 0;
	}
	int task_type;			//任务类型
	int task_param;			//任务参数

	int week_active_degree;	//周 活跃奖励
};

// 周常 - 活跃奖励
struct WeekActiveDegreeRewardCfg
{
	WeekActiveDegreeRewardCfg()
	{
		reward_index = 0;
		need_degree = 0;
	}
	int reward_index;
	int need_degree;

	std::vector<ItemConfigData> reward_item_vec;
};

// 周常 - 活跃奖励等级分段
struct WeekActiveDegreeRewardLevelCfg
{
	WeekActiveDegreeRewardLevelCfg():level_max(0)
	{}

	int level_max;
	WeekActiveDegreeRewardCfg week_degree_reward_cfg[MAX_ACTIVE_DEGREE_WEEK_REWARD_NUM];
};

//转盘次数奖励
struct TurntableTaskCfg
{
	TurntableTaskCfg() : seq(0), turntable_num(0) {}

	short seq;
	short param;
	short turntable_num;
};

struct TurntableRewardCfg
{
	TurntableRewardCfg() : seq(0) {}

	short seq;

	ItemConfigData item;
};

struct ActiveSystemOtherCfg
{
	ActiveSystemOtherCfg():one_click_level(0), one_click_open_time(0)
	{}

	int one_click_level;		// 一键完成开启等级
	int one_click_open_time;	//一键完成开服时间开启
};


class RandActivityManager;
class ActiveSystemConfig : public ILogicConfig
{
public:
	ActiveSystemConfig();
	virtual ~ActiveSystemConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const ActiveSystemCfg* GetActiveSystemCfg(int type, int level);
	const ActiveSystemSpecialCfg* GetSpecialActiveCfg(int type, int level);
	const ActiveSystemRewardCfg* GetActiveSystemRewardCfg(int level, int index);
	void GetActiveSystemRelatedCfgNum(RandActivityManager* ramgr, int seq, int count, const ItemConfigData * reward_num, std::vector<ItemConfigData> * reward_list);

	const WeekActiveDegreeTaskCfg* GetWeekActiveDegreeTaskCfg(int task_type) const;
	const WeekActiveDegreeRewardCfg* GetWeekActiveDegreeRewardCfg(int role_level, int reward_seq) const;

	const TurntableTaskCfg* GetTurntableActivityCfg(int activity_type);
	const std::vector<TurntableTaskCfg> & GetTurntableWeekCfg() { return m_turntable_week_task_cfg; }
	const TurntableRewardCfg* GetTurntableRewardCfg(int role_leve);

	int GetOneClickConsumeGold(int activie_type)const;
	const ActiveSystemOtherCfg& GetOtherCfg()const { return m_other_cfg; }

private:
	int InitActiveSystemCfg(TiXmlElement *RootElement);
	int InitActiveLevelGroupCfg(TiXmlElement *RootElement);
	int InitActiveRelatedCfg(TiXmlElement *RootElement);

	int InitActiveCommonCfg(TiXmlElement *root_element, ActiveSystemCommonCfg & node_cfg);

	int InitWeekActiveDegreeTaskCfg(TiXmlElement *RootElement);
	int InitWeekActiveDegreeRewardfg(TiXmlElement *RootElement);

	int InitTurntableTaskCfg(TiXmlElement *RootElement);
	int InitTurntableLevelCfg(TiXmlElement *RootElement);
	int InitTurntableRewardCfg(TiXmlElement *RootElement);

	int InitOneClickListCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	ActiveSystemListCfg m_active_system_cfg[ACTIVE_SYSTEM_TYPE_MAX];				//以type为下标
	ActiveSystemSpecialListCfg m_special_active_cfg[ACTIVITY_SYSTEM_SPECIAL_TYPE_MAX];
	std::vector<ActiveSystemLevelGroupCfg> m_active_level_group_cfg;
	
	std::map<int, ActiveSystemRelatedActCfg> m_active_related_cfg;			//有时候运营活动需要给特定道具 以act_type为key

	//周常 -任务信息[任务类型]
	WeekActiveDegreeTaskCfg m_week_degree_task_cfg[ACTIVE_DEGREE_TYPE_MAX];

	//周常 -活跃度阶段奖励[奖励seq - 1]
// 	int m_week_degree_reward_cfg_num;
	std::vector<WeekActiveDegreeRewardLevelCfg> m_week_degree_reward_level_cfg;

	std::map<int, TurntableTaskCfg> m_turntable_day_task_cfg;								// 每日更新的参与活动任务
	std::vector<TurntableTaskCfg> m_turntable_week_task_cfg;								// 周活跃
	std::map<short, short> m_turntable_level_cfg;											// <最小等级, 奖励组>
	std::map<short, std::map<int, TurntableRewardCfg> > m_turntable_reward_cfg;				//<奖励组, <rate, cfg> >

	std::map<int, int> m_one_click_list_cfg;	// 一键完成任务需要消耗元宝数, key-see: Protocol::CSRoleJoinActivityRewardReq::ONE_KEY_FINISH_TYPE, value-消耗元宝
	ActiveSystemOtherCfg m_other_cfg;
};



#endif