#ifndef _FLY_UP_SYSTEM_CONFIG_HPP_
#define _FLY_UP_SYSTEM_CONFIG_HPP_

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "servercommon/flyupsystemdef.h"
#include <map>
#include <vector>

struct FlyUpSystemOtherCfg
{
	FlyUpSystemOtherCfg() :level_limit1(0), level_limit2(0), use_limit_up(0),
		feishengtasklist_id(0), feishengtasklist_id2(0), check_feishengtask(0), feisheng_title1(0), feisheng_title2(0),
		max_exp1(0), max_exp2(0), add_exp(0), score_limit1(0), score_limit2(0), equipmentdh_task(0) {}
	int level_limit1;
	int	level_limit2;
	int	use_limit_up;
	int	feishengtasklist_id;
	int	feishengtasklist_id2;
	int check_feishengtask;
	int	feisheng_title1;
	int	feisheng_title2;
	int max_exp1;
	int max_exp2;
	int add_exp;
	int score_limit1;
	int	score_limit2;
	int equipmentdh_task;
};

struct FlyUpShiLianCfg
{
	FlyUpShiLianCfg() :seq(0), type_jobs(0), sub_type(0), stuff_id(0), stuff_num(0), attr_min(0), attr_max(0), attr_type(0), attr_val(0) {}

	int seq;//飞升阶段
	int	type_jobs;//玩家职业
	int	sub_type;//属性类别

	int	stuff_id;
	int	stuff_num;
	
	int	attr_min;//升级提升最小值
	int	attr_max;//升级提示最大值

	int	attr_type;//属性类型
	int	attr_val;//每点经验增加属性值
};

struct FlyUpAttrCfg
{
	FlyUpAttrCfg() :shilian_type(0), hp_buff(0)
	{
		memset(attr_value, 0, sizeof(attr_value));
	}
	enum FLY_UP_ATTR_TYPE
	{
		FLY_UP_ATTR_TYPE_0 = 0,//生命
		FLY_UP_ATTR_TYPE_1 = 1,//法力
		FLY_UP_ATTR_TYPE_2 = 2,//攻击
		FLY_UP_ATTR_TYPE_3 = 3,//防御
		FLY_UP_ATTR_TYPE_4 = 4,//速度
		FLY_UP_ATTR_TYPE_5 = 5,//精神
		FLY_UP_ATTR_TYPE_6 = 6,//回复
		FLY_UP_ATTR_TYPE_MAX,
	};

	int shilian_type;
	int attr_value[FLY_UP_ATTR_TYPE_MAX];
	int hp_buff;
};

struct FlyUpSkillCfg
{
	FlyUpSkillCfg() :type_jobs(0), ascension_skills(0) {}
	int type_jobs;
	int	ascension_skills;
};

struct FlyUpSkillUpLevelCfg
{
	FlyUpSkillUpLevelCfg() : skill_level(0), role_level(0), consume(0), item_id(0), need_exp(0) {}
	int skill_level;
	int	role_level;
	int	consume;
	int	item_id;
	int need_exp;
};

struct FlyUpTaskCfg
{
	FlyUpTaskCfg() :stag(0), is_shilian(0), task_id(0), rate(0) {}
	int stag;
	int is_shilian;
	int	task_id;
	int	rate;
};

struct FlyUpTaskListCfg
{
	FlyUpTaskListCfg() :weigth(0) {}
	int weigth;
	std::vector<FlyUpTaskCfg> task_vec;
};

struct FlyUpTaskRewardCfg
{
	FlyUpTaskRewardCfg() : feisheng_stage(0), is_shilian(0), task_num(0), reward_group(0) {}
	int feisheng_stage;
	int	is_shilian;
	int	task_num;
	int	reward_group;
};

struct FlyUpTaskRewardGroupCfg
{
	FlyUpTaskRewardGroupCfg() {}
	int reward_id;
	int rate;
	ItemConfigData reward;
};

struct FlyUpTaskRewardGroupListCfg
{
	FlyUpTaskRewardGroupListCfg() : reward_id(0), weight(0) {}
	int reward_id;
	int weight;
	std::vector<FlyUpTaskRewardGroupCfg> m_reward_vec;
};

struct FlyUpFinishRewardCfg
{
	FlyUpFinishRewardCfg() : feisheng_stage(0), is_shilian(0) {}
	int feisheng_stage;
	int	is_shilian;
	std::vector<ItemConfigData> reward_vec;
};

class FlyUpSystemConfig : public ILogicConfig
{
public:
	FlyUpSystemConfig();
	virtual ~FlyUpSystemConfig();

	virtual bool Init(const std::string & configname, std::string * err);
	const FlyUpSystemOtherCfg & GetOtherCfg() { return m_other_cfg; }
	const FlyUpShiLianCfg * GetShiLianCfg(int stag, int prof, int element);
	const FlyUpAttrCfg * GetAttrCfg(int stag);
	const FlyUpSkillCfg * GetSkillCfg(int prof);
	const FlyUpSkillUpLevelCfg * GetSkillUplevelCfg(int skill_level);
	const FlyUpTaskCfg * RandTask(int stag, int is_shilian, int pre_task_id = 0);
	const FlyUpTaskRewardCfg * GetTaskRewardCfg(int stag, int is_shilian, int task_idx);
	const FlyUpTaskRewardGroupCfg * RandTaskRewardCfg(int reward_id);
	const FlyUpFinishRewardCfg * GetTaskFinishRewardCfg(int stag, int is_shilian);
private:
	
	int InitOtherCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitShiLianAttrCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitFeiShengAttrCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitSkillCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitSkillUpCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitFlyUpTaskCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitFlyUpTaskRewardCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitFlyUpTaskRewardGroupCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitFlyUpTaskLastRewardCfg(TiXmlElement *RootElement, std::string& errormsg);

	FlyUpSystemOtherCfg m_other_cfg;

	FlyUpShiLianCfg * shilian_list[FlyUpSystemParam::FLY_UP_STAG_MAX][PROFESSION_TYPE_NUM][FlyUpSystemParam::FLY_UP_ORGAN_MAX + 1];
	std::vector<FlyUpShiLianCfg> m_shilian_vec;

	FlyUpAttrCfg fly_up_attr_cfg[FlyUpSystemParam::FLY_UP_STAG_MAX];

	FlyUpSkillCfg skill_list[PROFESSION_TYPE_NUM];
	std::vector<FlyUpSkillUpLevelCfg> skill_uplevel_vec;

	FlyUpTaskListCfg fly_task_list[FlyUpSystemParam::FLY_UP_STAG_MAX][FlyUpSystemParam::FLY_UP_SHILIAN_MAX];

	FlyUpTaskRewardCfg fly_task_reward_list[FlyUpSystemParam::FLY_UP_STAG_MAX][FlyUpSystemParam::FLY_UP_SHILIAN_MAX][MAX_FLY_UP_TASK_IDX];

	std::map<int, FlyUpTaskRewardGroupListCfg> m_task_reward_map;

	FlyUpFinishRewardCfg finish_reward_list[FlyUpSystemParam::FLY_UP_STAG_MAX][FlyUpSystemParam::FLY_UP_SHILIAN_MAX];
};



#endif // _FLY_UP_SYSTEM_CONFIG_HPP_
