#ifndef __RAND_ACTIVITY_LUCKY_TURN_TABLE_CONFIG_HPP__
#define __RAND_ACTIVITY_LUCKY_TURN_TABLE_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/raluckyturntableparam.hpp"
#include "item/itembase.h"

struct LuckyTurntableParamCfg
{
	LuckyTurntableParamCfg() : param(0), frequency(0)
	{}

	//int type;				//条件类型 RA_LUCKY_TURNTABLE_TYPE
	int param;				//在此表示为完成条件类型次数
	int frequency;			//增加转盘次数
};

struct LuckyTurntableRewardCfg
{
	LuckyTurntableRewardCfg() :seq(0), rate(0), hearsay(0)
	{}

	int seq;							//对应配置表的seq
	ItemConfigData reward_item;			//奖励道具
	int rate;							//权重/占比
	int hearsay;						//是否传闻 0 :否
};

struct LuckyTurntableRewardSectionCfg
{
	LuckyTurntableRewardSectionCfg() :section_start(0), section_end(0), rate_count(0)
	{}

	int section_start;						//开始时间
	int section_end;						//结束时间
	int rate_count;							//总权重
	LuckyTurntableRewardCfg node[MAX_RA_LUCKY_TURNTABLE_REWARD_NUM];
};

class RandActivityManager;

class RandActivityLuckyTurnTableConfig : public RandActivityConfig
{
public:
	RandActivityLuckyTurnTableConfig();
	virtual ~RandActivityLuckyTurnTableConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const LuckyTurntableParamCfg* GetParamCfg(int type)const;
	const LuckyTurntableRewardCfg* GetRandRewardCfg(RandActivityManager* ramgr)const;
	const LuckyTurntableRewardCfg* GetRewardCfgBySeq(int seq)const;

private:
	int InitTurntableParamCfg(TiXmlElement *RootElement);
	int InitTurntableRewardCfg(TiXmlElement *RootElement);

	LuckyTurntableParamCfg m_param_cfg[RA_LUCKY_TURNTABLE_TYPE_MAX];	// 转盘条件
	std::vector<LuckyTurntableRewardSectionCfg> m_reward_section_cfg;	// 转盘奖励
	std::vector<LuckyTurntableRewardCfg> m_reward_cfg;					// 转盘奖励
};

#endif	//__RAND_ACTIVITY_LUCKY_TURN_TABLE_CONFIG_HPP__