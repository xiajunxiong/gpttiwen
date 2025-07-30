#ifndef __RAND_ACTIVITY_YUN_ZE_ER_SAN_SHI_CONFIG_HPP__
#define __RAND_ACTIVITY_YUN_ZE_ER_SAN_SHI_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/rayunzeersanshiparam.hpp"

struct RAYZErSanShiBasicsCfg
{
	RAYZErSanShiBasicsCfg():task_id(0), reward_group_id(0)
	{}

	int task_id;
	int reward_group_id;
};

struct RAYZErSanShiRewardCfg
{
	RAYZErSanShiRewardCfg():reward_group(0)
	{}

	int reward_group;
	std::vector<ItemConfigData> reward_list;
};

struct RAYZErSanShiRewardSectionCfg
{
	RAYZErSanShiRewardSectionCfg() :section_start(0), section_end(0)
	{}
	
	int section_start;
	int section_end;
	std::vector<RAYZErSanShiRewardCfg> reward;
};

struct RAYZErSanShiTurnTableRewardCfg
{
	RAYZErSanShiTurnTableRewardCfg(): seq(-1),rate(0),is_hearsay(0)
	{}

	int seq;
	int rate;
	int is_hearsay;			// 是否传闻，1-传闻，0-不传闻
	ItemConfigData turntable_reward;
};

struct RAYZErSanShiTurnTableRewardSectionCfg
{
	RAYZErSanShiTurnTableRewardSectionCfg() :section_start(0), section_end(0), m_turntable_total_rate(0)
	{}

	int section_start;
	int section_end;
	int m_turntable_total_rate;				// 转盘总权重
	std::vector<RAYZErSanShiTurnTableRewardCfg> turntable_reward;
};

struct RAYZErSanShiOtherCfg
{
	RAYZErSanShiOtherCfg(): frequency(0),frequency_total(0),exp(0)
	{}
	static const int EXP_BASE_NUM = 10000;

	int frequency;				// 每日次数
	int frequency_total;		// 活动持续天数
	int exp;					// 完成全部任务经验
};

class RandActivityManager;
class RandActivityYunZeErSanShiConfig : public RandActivityConfig
{
public:
	RandActivityYunZeErSanShiConfig();
	virtual ~RandActivityYunZeErSanShiConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAYZErSanShiTurnTableRewardCfg * GetRandTurntableRewardCfg(RandActivityManager* ramgr) const;	// 根据权重随机获取转盘奖励
	const RAYZErSanShiRewardCfg * GetRewardCfg(RandActivityManager* ramgr, int task_id) const;			// 根据题目id返回奖励组
	const RAYZErSanShiOtherCfg & GetOtherCfg()const { return m_other_cfg; }
	int GetRandTaskCfg(int frequency)const;

private:
	int InitBasicsCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitTurntableRewardCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	std::vector<RAYZErSanShiBasicsCfg> m_basics_cfg;
	std::vector<RAYZErSanShiRewardSectionCfg> m_reward_cfg;
	std::vector<RAYZErSanShiTurnTableRewardSectionCfg> m_turntable_reward_cfg;
	RAYZErSanShiOtherCfg m_other_cfg;
};

#endif