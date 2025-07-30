#ifndef __RAND_ACTIVITY_SHAN_HAI_MEET_CONFIG_HPP__
#define __RAND_ACTIVITY_SHAN_HAI_MEET_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/rashanhaimeetparam.hpp"
#include "item/itembase.h"

class RandActivityManager;

struct RAShanHaiMeetOtherCfg
{
	RAShanHaiMeetOtherCfg() :buy_money(0), cfg_ver(0) {}

	int buy_money;
	int cfg_ver;
	int poker_cost_id;
	int poker_cost_num;
};

struct RAShanHaiMeetCfg
{
	RAShanHaiMeetCfg()
	{
		seq = 0;
		task_id = 0;
		task_type = 0;
		param_1 = 0;
		param_2 = 0;
	}

	int seq;
	int task_id;
	int task_type;
	int param_1;
	int param_2;
	std::vector<ItemConfigData> finish_item;
	std::vector<ItemConfigData> unfinish_item;
};

struct RAShanHaiMeetSectionCfg
{
	RAShanHaiMeetSectionCfg() :section_start(0), section_end(0) {}

	int section_start;
	int section_end;
	std::vector<RAShanHaiMeetCfg> item;
};

struct RAShanHaiMeetPokerCfg
{
	RAShanHaiMeetPokerCfg()
	{
		seq = 0;
		section_start = 0;
		section_end = 0;
		reward_group = 0;
	}

	int seq;
	int section_start;
	int section_end;
	int reward_group;
};

struct RAShanHaiMeetRewardCfg
{
	RAShanHaiMeetRewardCfg() : seq(0), rate(0) {}

	int seq;
	int rate;
	ItemConfigData item;
};

struct RAShanHaiMeetRewardListCfg
{
	RAShanHaiMeetRewardListCfg() : all_rate(0) {}

	int all_rate;
	std::vector<RAShanHaiMeetRewardCfg> item;
};

class RandActivityShanHaiMeetConfig : public RandActivityConfig
{
public:
	RandActivityShanHaiMeetConfig();
	virtual ~RandActivityShanHaiMeetConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAShanHaiMeetOtherCfg & GetOtherCfg()const { return m_other_cfg; }
	const RAShanHaiMeetCfg* GetBaseCfg(RandActivityManager * ramgr, int seq)const;
	const int GetBaseCfgSize(RandActivityManager * ramgr)const;
	const RAShanHaiMeetPokerCfg* GetPokerCfg(RandActivityManager * ramgr)const;
	const RAShanHaiMeetRewardCfg* GetRewardCfg(int reward_group)const;
	virtual int GetConfigVer()const { return m_other_cfg.cfg_ver; }
private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitBaseCfg(TiXmlElement *RootElement);
	int InitPokerCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);;

	RAShanHaiMeetOtherCfg m_other_cfg;
	std::vector<RAShanHaiMeetSectionCfg> m_section_cfg;
	std::vector<RAShanHaiMeetPokerCfg> m_poker_cfg;
	std::map<int, RAShanHaiMeetRewardListCfg> m_reward_cfg;
};

#endif	//__RAND_ACTIVITY_SHAN_HAI_MEET_CONFIG_HPP__