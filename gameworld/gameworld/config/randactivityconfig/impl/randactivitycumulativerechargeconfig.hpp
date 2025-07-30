#ifndef __RAND_ACTIVITY_CUMULATIVE_RECHARGE_CONFIG_HPP__
#define __RAND_ACTIVITY_CUMULATIVE_RECHARGE_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/raexpelmonsterparam.hpp"
#include "item/itembase.h"
#include <vector>

class RandActivityManager;

struct CumulativeRechargeOtherCfg
{
	CumulativeRechargeOtherCfg() : cfg_ver(0)
	{}

	int cfg_ver;
	std::vector<ItemConfigData> free_reward;
};

struct CumulativeRechargeCfg
{
	CumulativeRechargeCfg():type(0), section_start(0), section_end(0), acc_price(0), is_broadcast(0), buy_money(0)
	{}

	int type;
	int section_start;
	int section_end;
	int acc_price;
	int is_broadcast;
	int buy_money;			//RMB,使用时需要转化为元宝

	std::vector<ItemConfigData> m_reward_vec;
};

class RandActivityCumulativeRechargeConfig : public RandActivityConfig
{
public:
	RandActivityCumulativeRechargeConfig();
	virtual ~RandActivityCumulativeRechargeConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const CumulativeRechargeCfg * GetRewardCfg(int open_day) const;
	virtual int GetConfigVer() const;
	const CumulativeRechargeOtherCfg & GetOtherCfg()const { return m_other_cfg; }
private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitCumulativeRechargeCfg(TiXmlElement *RootElement);

	CumulativeRechargeOtherCfg m_other_cfg;
	std::map<int, CumulativeRechargeCfg> m_cfg_map;
};

#endif	//__RAND_ACTIVITY_GIVE_SECRET_KEY_CONFIG_HPP__