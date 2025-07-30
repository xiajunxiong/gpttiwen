#ifndef __RAND_ACTIVITY_LINKAGE_GIFT_CONFIG_HPP__
#define __RAND_ACTIVITY_LINKAGE_GIFT_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/ralinkagegiftparam.hpp"
#include "item/itembase.h"

struct LinkageGiftRewardCfg
{
	LinkageGiftRewardCfg() : grade_start(0), grade_end(0)
	{}

	int grade_start;
	int grade_end;
	ItemConfigData item;
};

struct LinkageGiftOtherCfg
{
	LinkageGiftOtherCfg() : receive_time(0) {}

	int receive_time;			// 创角天数需达到的天数
};

class RandActivityManager;

class RandActivityLinkageGiftConfig : public RandActivityConfig
{
public:
	RandActivityLinkageGiftConfig();
	virtual ~RandActivityLinkageGiftConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	int GetLinkageGiftRewardCfgSeq(int level)const;
	const LinkageGiftRewardCfg * GetLinkageGiftRewardCfgBySeq(int seq)const;
	const LinkageGiftOtherCfg & GetOhterCfg() const { return m_other_cfg; }

private:
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	LinkageGiftOtherCfg m_other_cfg;
	std::vector<LinkageGiftRewardCfg> m_reward_cfg;
};

#endif	//__RAND_ACTIVITY_LINKAGE_GIFT_CONFIG_HPP__