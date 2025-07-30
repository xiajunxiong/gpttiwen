#ifndef __MONTH_CARD_CONFIG_HPP__
#define __MONTH_CARD_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/activitydef.hpp"

enum MONTH_CARD_TYPE
{
	MONTH_CARD_TYPE_WEEK = 0,			//周卡
	MONTH_CARD_TYPE_MONTH = 1,			//月卡

	MONTH_CARD_TYPE_MAX
};
UNSTD_STATIC_CHECK(MONTH_CARD_TYPE_MAX <= BIT_COUNT_OF_BYTE);

enum MONTH_CARD_LIMIT_TYPE
{
	MONTH_CARD_LIMIT_TYPE_INVAILD = 0,
	MONTH_CARD_LIMIT_TYPE_WEEK = 1,		//每周限购
	MONTH_CARD_LIMIT_TYPE_DAY = 2,		//每日限购

	MONTH_CARD_LIMIT_TYPE_MAX
};

enum MONTH_CARD_PRICE_TYPE
{
	MONTH_CARD_PRICE_TYPE_GOLD = 0,				//元宝
	MONTH_CARD_PRICE_TYPE_SILVER_COIN = 1,		//灵玉
	MONTH_CARD_PRICE_TYPE_COIN = 2,				//铜币
	MONTH_CARD_PRICE_TYPE_MONEY = 3,			//直购

	MONTH_CARD_PRICE_TYPE_MAX
};

enum MONTH_CARD_CMD_TYPE
{
	MONTH_CARD_CMD_TYPE_BUY_ITEM = 0,			// 商城	
	MONTH_CARD_CMD_TYPE_CARD = 1,				// 周卡/月卡

	MONTH_CARD_CMD_TYPE_MAX
};

struct MonthCardOtherCfg
{
	MonthCardOtherCfg() : cfg_ver(0)
	{}

	int cfg_ver;
};

struct MonthCardCfg
{
	MonthCardCfg() : card_day(0), buy_reward_gold(0), buy_reward_immortal(0), buy_money(0), level_down(0), level_up(0)
	{}

	int card_day;
	int buy_reward_gold;
	int buy_reward_immortal;
	int buy_money;
	int level_down;
	int	level_up;
	ItemConfigData buy_reward;
	std::vector<ItemConfigData> day_reward_list;
};

struct MonthCardListCfg
{
	MonthCardListCfg() {}

	std::vector<MonthCardCfg> list_vec;
};

struct MonthShopCfg
{
	MonthShopCfg() : type(0), limit_type(0), week_buy_times(0), today_buy_times(0), price_type(0), price(0)
	{}
	int type;
	int limit_type;
	int week_buy_times;
	int today_buy_times;
	int price_type;
	int price;
	ItemConfigData item;
};

struct MonthShopListCfg
{
	MonthShopListCfg() : type(0), level_down(0), level_up(0), m_shop_count(0) {}
	int type;
	int level_down;
	int level_up;
	int m_shop_count;
	MonthShopCfg shop_list[MAX_MONTH_CARD_SHOP_ITEM_NUM];		//商城配置
};

class MonthCardConfig : public ILogicConfig
{
public:
	MonthCardConfig();
	virtual ~MonthCardConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const MonthCardOtherCfg & GetOtherCfg() { return m_other_cfg; }
	const MonthCardCfg* GetCardCfg(int level, int card_type);
	const MonthShopCfg* GetShopCfg(int level, int seq);
	void OnRefreshBuyTimes(int level, int * buy_times_list, int max_count, int limit_type);

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitMonthCardCfg(TiXmlElement * RootElement);
	int InitMonthShopCfg(TiXmlElement * RootElement);

	MonthCardOtherCfg m_other_cfg;
	MonthCardListCfg m_month_card_list[MONTH_CARD_TYPE_MAX];		//月卡配置
	std::map<int, MonthShopListCfg> m_shop_list;					//商城配置
};


#endif