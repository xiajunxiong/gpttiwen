#ifndef __RAND_ACTIVITY_SHEN_CI_HUA_FU_CONFIG_HPP__
#define __RAND_ACTIVITY_SHEN_CI_HUA_FU_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/randactivityshencihuafuparam.hpp"
#include "item/itembase.h"

#include <vector>
#include <set>
#include <map>

enum RAND_ACTIVITY_SHEN_CI_HUA_FU_REWARD_TYPE
{
	RAND_ACTIVITY_SHEN_CI_HUA_FU_REWARD_TYPE_SELECT,
	RAND_ACTIVITY_SHEN_CI_HUA_FU_REWARD_TYPE_RAND,
};

struct  RandActivityShenCiHuaFuCfg
{
	int type;
	int reward_group_id;
};

struct  RandActivityShenCiHuaFuSectionCfg
{
	RandActivityShenCiHuaFuSectionCfg() :section_start(0), section_end(0)
	{

	}

	int section_start;
	int section_end;

	std::map<int, RandActivityShenCiHuaFuCfg> item_list;
};

struct  RandActivityShenCiHuaFuRewardGroupItemCfg
{
	int rate;
	int reward_group_id;
	int reward_seq;
	std::vector<ItemConfigData> rewards;
	int is_broadcast;
};


struct  RandActivityShenCiHuaFuRewardGroupCfg
{
	int reward_group_id;
	std::map<int, RandActivityShenCiHuaFuRewardGroupItemCfg> item_list;
};

struct  RandActivityShenCiHuaFuBuyItemCfg
{
	RandActivityShenCiHuaFuBuyItemCfg()
	{
		seq = 0;
		limit_type = 0;
		limit_buy_times_day = 0;
		limit_buy_times = 0;

		is_need_chong_zhi = 0;
		need_chong_zhi_gold = 0;
		money_type = 0;
		money_value = 0;		
	}

	int seq;
	int limit_type;
	int limit_buy_times_day;
	int limit_buy_times;

	int is_need_chong_zhi;
	int need_chong_zhi_gold;
	int money_type;
	int money_value;

	std::vector<ItemConfigData> rewards;
};

struct  RandActivityShenCiHuaFuBuyItemSectionCfg
{
	RandActivityShenCiHuaFuBuyItemSectionCfg() :section_start(0), section_end(0)
	{

	}

	int section_start;
	int section_end;

	std::map<int, RandActivityShenCiHuaFuBuyItemCfg> item_list;
};

struct  RandActivityShenCiHuaFuExchangeShopItemCfg
{
	int seq;
	int limit_type;
	int limit_times_day;
	int limit_times;
	std::vector<ItemConfigData> rewards;

	int need_item_id;
	int need_item_num;
};

struct  RandActivityShenCiHuaFuExchangeShopItemSectionCfg
{
	RandActivityShenCiHuaFuExchangeShopItemSectionCfg() :section_start(0), section_end(0)
	{

	}

	int section_start;
	int section_end;

	std::map<int, RandActivityShenCiHuaFuExchangeShopItemCfg> item_list;
};

struct  RandActivityShenCiHuaFuOtherCfg
{
	int cfg_ver;	
	int draw_need_item_id;
};

class RandActivityManager;
class RandActivityShenCiHuaFuConfig : public RandActivityConfig
{
public:
	RandActivityShenCiHuaFuConfig();
	virtual ~RandActivityShenCiHuaFuConfig();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const RandActivityShenCiHuaFuOtherCfg & GetOtherCfg() const;
	const RandActivityShenCiHuaFuCfg * GetRandActivityShenCiHuaFuCfg(RandActivityManager * ramgr, int reward_type) const;	
	const RandActivityShenCiHuaFuRewardGroupItemCfg * GetRandActivityShenCiHuaFuRewardGroupItemCfg(RandActivityManager * ramgr, int reward_type, int reward_seq) const;	
	const RandActivityShenCiHuaFuRewardGroupCfg * GetRandActivityShenCiHuaFuRewardGroupCfg(RandActivityManager * ramgr, int reward_type) const;	
	const RandActivityShenCiHuaFuBuyItemCfg * GetRandActivityShenCiHuaFuBuyItemCfg(RandActivityManager * ramgr, int buy_seq) const;
	const RandActivityShenCiHuaFuExchangeShopItemCfg * GetRandActivityShenCiHuaFuExchangeShopItemCfg(RandActivityManager * ramgr, int buy_seq) const;
	
	virtual int GetConfigVer() const;
private: 
	int InitOthercfg(TiXmlElement * RootElement);
	int InitRandActivityShenCiHuaFuCfg(TiXmlElement * RootElement);
	int InitRandActivityShenCiHuaFuRewardGroupItemCfg(TiXmlElement * RootElement);
	int InitRandActivityShenCiHuaFuBuyItemCfg(TiXmlElement * RootElement);
	int InitRandActivityShenCiHuaFuExchangeShopItemCfg(TiXmlElement * RootElement);

	std::vector<RandActivityShenCiHuaFuSectionCfg> m_section_cfg;
	std::vector<RandActivityShenCiHuaFuBuyItemSectionCfg> m_buy_section_cfg;
	std::map<int, RandActivityShenCiHuaFuRewardGroupCfg> m_reward_group_cfg;
	std::vector<RandActivityShenCiHuaFuExchangeShopItemSectionCfg> m_exchange_shop_item_section_cfg;
	
	RandActivityShenCiHuaFuOtherCfg m_other_cfg;
};

#endif	