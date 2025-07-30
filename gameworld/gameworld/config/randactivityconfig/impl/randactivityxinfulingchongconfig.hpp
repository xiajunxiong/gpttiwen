#ifndef __RAND_ACTIVITY_XIN_FU_LING_CHONG_CONFIG_HPP__
#define __RAND_ACTIVITY_XIN_FU_LING_CHONG_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itembase.h"
#include "servercommon/servercommon.h"

struct RAXinFuLingChongRewardCfg
{
	RAXinFuLingChongRewardCfg() : price(0), price_type(0), is_cmd_buy(0), cfg_ver(0)
	{}

	int price;
	int price_type;
	int is_cmd_buy;	
	int cfg_ver;
	std::vector<ItemConfigData> reward_list;
};

class RandActivityManager;
class RandActivityXinFuLingChongConfig : public RandActivityConfig
{
public:
	RandActivityXinFuLingChongConfig();
	virtual ~RandActivityXinFuLingChongConfig();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const RAXinFuLingChongRewardCfg & GetRewardCfg() const { return m_reward_cfg; }

	virtual int GetConfigVer() const;
private:
	int InitRewardCfg(TiXmlElement * RootElement, std::string & errormsg);

	RAXinFuLingChongRewardCfg m_reward_cfg;			//Ω±¿¯≈‰÷√
};

#endif	//__RAND_ACTIVITY_LUCKY_BAG_CONFIG_HPP__