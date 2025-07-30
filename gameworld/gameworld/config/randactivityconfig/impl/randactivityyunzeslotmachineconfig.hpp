#ifndef __RAND_ACTIVITY_YUN_ZE_SLOT_MACHINE_CONFIG_HPP__
#define __RAND_ACTIVITY_YUN_ZE_SLOT_MACHINE_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "item/itempool.h"
#include "config/randactivityconfig/randactivityconfig.hpp"

static const int RA_YZ_SLOT_MACHINE_MAX_REWARD_POOL_NUM = 9;		// 奖池奖励类型数量

enum YUN_ZE_SLOT_MACHINE_TYPE
{
	YUN_ZE_SLOT_MACHINE_TYPE_BEGIN = 0,
	YUN_ZE_SLOT_MACHINE_TYPE_PERFECT = 1,	// 最高奖，匹配三种花色
	YUN_ZE_SLOT_MACHINE_TYPE_WONDERFUL = 2,	// 中等奖，匹配二种花色
	YUN_ZE_SLOT_MACHINE_TYPE_GOOD = 3,		// 普通奖，匹配一种花色
	YUN_ZE_SLOT_MACHINE_TYPE_COMFORT = 4,	// 安慰奖，匹配零种花色

	YUN_ZE_SLOT_MACHINE_TYPE_MAX
};

enum YUN_ZE_SLOT_MACHINE_MONEY_TYPE
{
	YUN_ZE_SLOT_MACHINE_MONEY_TYPE_GOLD = 0,	// 元宝
	YUN_ZE_SLOT_MACHINE_MONEY_TYPE_RMB = 1,		// 直购
	YUN_ZE_SLOT_MACHINE_MONEY_TYPE_JADE = 2,	// 灵玉
};

enum YUN_ZE_SLOT_MACHINE_BUY_TYPE
{
	YUN_ZE_SLOT_MACHINE_BUY_TYPE_TOTAL = 1,	// 总限购
	YUN_ZE_SLOT_MACHINE_BUY_TYPE_TODAY = 2,	// 每日限购
};

struct RAYZSlotMachineBasicsCfg
{
	RAYZSlotMachineBasicsCfg():item_pool(0), rate(0)
	{}

	int item_pool;
	int rate;
};

struct RAYZSlotMachineRewardCfg
{
	RAYZSlotMachineRewardCfg():item_group(0), item_pool(0)
	{}

	int item_group;
	int item_pool;
	ItemConfigData item;
};

struct RAYZSlotMachineTaskRewardCfg
{
	RAYZSlotMachineTaskRewardCfg() : section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<RAYZSlotMachineRewardCfg> reward_list;
};

struct RAYZSlotMachineOtherCfg
{
	RAYZSlotMachineOtherCfg():price_type(0),price(0),item_consume(0),limit_type(0),total_buy_times(0),today_buy_times(0)
	{}

	int price_type;			// 货币类型
	int price;				// 购买金额
	int cfg_ver;			// 直购配置验证
	int item_consume;		// 抽奖消耗
	int limit_type;			// 限购类型
	int total_buy_times;	// 总限购次数
	int today_buy_times;	// 日限购次数
	ItemConfigData reward_item;		// 抽奖道具
};

class RandActivityManager;
class RandActivityYunZeSlotMachineConfig : public RandActivityConfig
{
public:
	RandActivityYunZeSlotMachineConfig();
	virtual ~RandActivityYunZeSlotMachineConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	int GetRandRewardByRateCfg()const;		// 返回奖池号
	const RAYZSlotMachineTaskRewardCfg* GetTaskRewardCfg(RandActivityManager* ramgr)const;
	const RAYZSlotMachineOtherCfg & GetOtherCfg()const { return m_other_cfg; }
	virtual int GetConfigVer()const { return m_other_cfg.cfg_ver; }

private:
	int InitBasicsCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);


	int m_total_rate;				// 总权重
	std::vector<RAYZSlotMachineBasicsCfg> m_basics_cfg;
	std::vector<RAYZSlotMachineTaskRewardCfg> m_task_reward_cfg;
	RAYZSlotMachineOtherCfg m_other_cfg;
};

#endif