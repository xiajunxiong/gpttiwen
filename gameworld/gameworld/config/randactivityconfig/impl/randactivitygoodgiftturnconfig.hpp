#ifndef __RAND_ACTIVITY_GOOD_GIFT_TURN_CONFIG_HPP__
#define __RAND_ACTIVITY_GOOD_GIFT_TURN_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/ragoodgiftturnparam.hpp"

struct RAGoodGiftTurnOtherCfg
{
	RAGoodGiftTurnOtherCfg(): reward_frequency(0)
	{}

	int reward_frequency;			// 每日礼包领取次数
	int reward_frequency_total;		// 礼包总领取次数
};

struct RAGoodGiftTurnCfg
{
	RAGoodGiftTurnCfg() : seq1(0), rate(0)
	{}
	
	int seq1;
	int rate;
	std::vector<ItemConfigData> item_list;
};

struct RAGoodGiftTurnSectionCfg
{
	RAGoodGiftTurnSectionCfg():section_start(0), section_end(0), rate_count(0)
	{}

	int section_start;
	int section_end;
	int rate_count;
	RAGoodGiftTurnCfg node[MAX_GOOD_GIFT_TURN_NUM];
};

class RandActivityGoodGiftTurnConfig : public RandActivityConfig
{
public:
	RandActivityGoodGiftTurnConfig();
	virtual ~RandActivityGoodGiftTurnConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAGoodGiftTurnOtherCfg & GetOtherCfg()const  { return m_other_cfg; }
	const RAGoodGiftTurnCfg * GetRandReward(RandActivityManager* ramgr, short fetch_flag)const;

private:

	int InitOtherCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);

	RAGoodGiftTurnOtherCfg m_other_cfg;
	std::vector<RAGoodGiftTurnSectionCfg> m_reward_section_cfg;			
	//std::map<int, RAGoodGiftTurnCfg> m_reward_map;					//key:seq
};

#endif