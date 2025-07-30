#ifndef __ACTIVITY_QING_YUAN_SHENG_HUI_CONFIG__
#define __ACTIVITY_QING_YUAN_SHENG_HUI_CONFIG__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/activitydef.hpp"
#include "servercommon/commonsavedef.h"

#include <set>
#include <map>
#include <vector>


struct QingYuanShengHuiOtherCfg
{
	int scene_id;

	int gather_interval;
	int gather_reward_group_id;

	int scene_reward_interval;
	int scene_reward_group_id;
	
	int yu_yue_reward_group_id;

	int max_question_num;
	int question_interval;
	int qi_yuan_reward_group_id;

	std::map<int, std::vector<ItemConfigData> > m_battle_reward;
};

struct QingYuanShengHuiPhaseCfg
{
	int phase;
	int phase_length_of_time;
};

struct QingYuanShengHuiGroupRewardItemCfg
{
	int rate;
	int buff_id;
	std::vector<ItemConfigData> rewards;
};

struct QingYuanShengHuiGroupRewardCfg
{
	QingYuanShengHuiGroupRewardCfg()
	{
		group_id = 0;
		rate_count = 0;
	}

	int group_id;

	int rate_count;
	std::vector<QingYuanShengHuiGroupRewardItemCfg> group_list;
};

struct QingYuanShengHuiQuestionCfg
{
	int question_id;
	std::vector<ItemConfigData> same_answer_rewards;
	std::vector<ItemConfigData> diff_answer_rewards;
};

class ActivityQingYuanShengHuiConfig : public ILogicConfig
{
public:
	ActivityQingYuanShengHuiConfig();
	virtual ~ActivityQingYuanShengHuiConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const QingYuanShengHuiGroupRewardItemCfg * GetGatherReward();
	const QingYuanShengHuiOtherCfg & GetQingYuanShengHuiOtherCfg();
	const QingYuanShengHuiPhaseCfg * GetQingYuanShengHuiPhaseCfg(int phase);
	const QingYuanShengHuiQuestionCfg * GetQingYuanShengHuiQuestionCfg(int question_id);
	int GetRandQuestionId(std::set<int> exclude_list);
	const QingYuanShengHuiGroupRewardItemCfg * GetRewardByGroupId(int reward_group_id);
private:
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitGroupRewardCfg(TiXmlElement *RootElement);
	int InitPhaseCfg(TiXmlElement * RootElement);
	int InitQuestionCfg(TiXmlElement * RootElement);
	//-----------------------

	QingYuanShengHuiOtherCfg m_other_cfg;
	std::map<int, QingYuanShengHuiQuestionCfg> m_question_cfg;
	std::map<int, QingYuanShengHuiGroupRewardCfg> m_group_reward_cfg;
	std::map<int, QingYuanShengHuiPhaseCfg> m_phase_cfg;
};

#endif 