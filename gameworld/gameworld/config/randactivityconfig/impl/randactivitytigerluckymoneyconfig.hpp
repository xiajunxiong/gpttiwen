#ifndef __RAND_ACTIVITY_TIGER_LUCKY_MONEY_CONFIG_HPP__
#define __RAND_ACTIVITY_TIGER_LUCKY_MONEY_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itempool.h"
#include "servercommon/activitydef.hpp"

// 基础配置
struct RATLMBasisCfg
{
	RATLMBasisCfg() :seq(0),task_type(0),task_parameter(0)
	{}

	int seq;
	int task_type;
	int task_parameter;
	std::vector<ItemConfigData> reward_item;
};

struct RATLMBasisLevelCfg
{
	RATLMBasisLevelCfg():level(0)
	{}

	int level;
	std::vector<RATLMBasisCfg> basis_cfg;
};

struct RATLMBasisSectionCfg
{
	RATLMBasisSectionCfg() :section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<RATLMBasisLevelCfg> basis_level_cfg;
};

// 奖励配置
struct RATLMRewardCfg
{
	RATLMRewardCfg():reward_type(0), item_id(0),start_num(0),add_num(0)
	{}

	int reward_type;	// 奖励道具类型，RATLM::LUCKY_MONEY_TYPE,修改时同时修改枚举
	int item_id;		// 奖励道具id，0-货币
	int start_num;		// 初始数量
	int add_num;		// 完成任务增加数量
};

struct RATLMRewardSectionCfg
{
	RATLMRewardSectionCfg() :section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	RATLMRewardCfg reward_cfg;
};


class RandActivityManager;
class RandActivityTigerLuckyMoneyConfig : public RandActivityConfig
{
public:
	RandActivityTigerLuckyMoneyConfig();
	virtual ~RandActivityTigerLuckyMoneyConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const std::vector<RATLMBasisCfg> * GetBasisCfg(RandActivityManager* ramgr, int role_level)const;
	const RATLMRewardCfg * GetRewardCfg(RandActivityManager* ramgr)const;
	const int GetStartLevel()const;

private:
	int InitBasisCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);

	std::vector<RATLMBasisSectionCfg> m_basis_section_cfg;
	std::vector<RATLMRewardSectionCfg> m_reward_section_cfg;
};

#endif