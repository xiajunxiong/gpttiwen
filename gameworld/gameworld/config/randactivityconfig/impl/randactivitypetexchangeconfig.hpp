#ifndef __RAND_ACTIVITY_PET_EXCHANGE_CONFIG_HPP__
#define __RAND_ACTIVITY_PET_EXCHANGE_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/rapetexchangeparam.hpp"
#include "item/itembase.h"

struct RAPetExchangeCfg			//宠物兑换
{
	RAPetExchangeCfg() :section_start(0), section_end(0), seq1(0),
		limit_type(0), daily_purchase(0), today_times(0), open_level_limit(0), hearsay(0)
	{}

	int section_start;
	int section_end;
	int seq1;					//活动类型
	std::vector<ItemConfigData> consume_item_list;

	int limit_type;				//\see ACTVITY_BUY_LIMIT_TYPE_MAX
	int daily_purchase;			//活动限兑
	int today_times;			//每日限兑
	int open_level_limit;		//开启兑换等级限制
	ItemConfigData reward_item;		//奖励

	int hearsay;
};

struct RAPetExchangeShopCfg
{
	RAPetExchangeShopCfg() : seq(0), limit_type(0), buy_times(0), price_type(0), price(0), is_cmd_buy(false)
	{}

	int seq;
	int limit_type;
	int buy_times;
	int price_type;
	int price;
	bool is_cmd_buy;
	ItemConfigData reward_item;
};

struct RAPetExchangeShopSectionCfg
{
	RAPetExchangeShopSectionCfg() : section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<RAPetExchangeShopCfg> shop_list;
};

struct RAPetExchangeOtherCfg
{
	RAPetExchangeOtherCfg() : cfg_ver(0)
	{}

	int cfg_ver;
};

class RandActivityManager;

class RandActivityPetExchangeConfig : public RandActivityConfig
{
public:
	RandActivityPetExchangeConfig();
	virtual ~RandActivityPetExchangeConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAPetExchangeCfg* GetPetExchangeBySeq(RandActivityManager* ramgr, int seq)const;
	const RAPetExchangeShopCfg* GetShopBySeq(RandActivityManager* ramgr, int seq)const;
	const RAPetExchangeShopSectionCfg* GetShopSectionCfg(RandActivityManager* ramgr)const;
	//当前时间	    到	活动开启时间天数
	int GetActOpenDay(RandActivityManager* ramgr, int activity_type)const; 
	 //活动开启时间	到	开服时间天数
	int GetActRealOpenDay(RandActivityManager* ramgr, int activity_type)const;

	virtual int GetConfigVer() const;
private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitPetExchangeCfg(TiXmlElement *RootElement);
	int InitPetExchangeShopCfg(TiXmlElement *RootElement);

	RAPetExchangeOtherCfg m_other_cfg;
	std::map<int, std::map<int, RAPetExchangeCfg> > m_pet_exchange_map;			// 宠物兑换
	std::vector<RAPetExchangeShopSectionCfg> m_shop_section_cfg;
};

#endif	//__RAND_ACTIVITY_PET_EXCHANGE_CONFIG_HPP__