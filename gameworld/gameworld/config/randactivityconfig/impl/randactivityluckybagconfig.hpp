#ifndef __RAND_ACTIVITY_LUCKY_BAG_CONFIG_HPP__
#define __RAND_ACTIVITY_LUCKY_BAG_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/raluckybagparam.hpp"
#include "item/itembase.h"

struct RandActivityLuckyBagBuyCfg
{
	RandActivityLuckyBagBuyCfg()
	{
		need_chong_zhi_gold = 0;
	}

	int need_chong_zhi_gold;
	std::vector<ItemConfigData > reward;
};

struct RandActivityLuckyBagBatchBuyCfg
{
	RandActivityLuckyBagBatchBuyCfg()
	{
		need_chong_zhi_gold = 0;
		add_day = 0;
	}

	int need_chong_zhi_gold;
	int add_day;
	std::vector<ItemConfigData > batch_buy_reward;
};

struct RandActivityLuckyBagOtherCfg
{
	RandActivityLuckyBagOtherCfg()
	{

	}

	std::vector<ItemConfigData > day_reward;  //每日免费领取奖励
};

class RandActivityManager;

class RandActivityLuckyBagConfig : public RandActivityConfig
{
public:
	RandActivityLuckyBagConfig();
	virtual ~RandActivityLuckyBagConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RandActivityLuckyBagBuyCfg * GetRandActivityLuckyBagBuyCfg(int seq)const;
	const RandActivityLuckyBagOtherCfg & GetRandActivityLuckyBagOtherCfg()const;

	const RandActivityLuckyBagBatchBuyCfg * GetRandActivityLuckyBagBatchBuyCfg(int seq)const;
private:
	int InitBuyCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitBatchBuyCfg(TiXmlElement * RootElement);

	RandActivityLuckyBagOtherCfg m_lucky_bag_other_cfg;
	std::vector<RandActivityLuckyBagBuyCfg> m_lucky_bag_buy_cfg;
	std::vector<RandActivityLuckyBagBatchBuyCfg> m_lucky_bag_batch_buy_cfg;
};

#endif	//__RAND_ACTIVITY_LUCKY_BAG_CONFIG_HPP__