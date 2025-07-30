#ifndef __RAND_ACTIVITY_DISCOUNT_CARNIVAL_CONFIG_HPP__
#define __RAND_ACTIVITY_DISCOUNT_CARNIVAL_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/radiscountcarnivalparam.hpp"
#include "item/itemextern.hpp"
#include "item/itembase.h"


struct RADiscountCarnivalCfg
{
	RADiscountCarnivalCfg() :seq(0), buy_limit_all(0), price_type(0), price(0), buy_type(0), buy_limit_day(0) {}

	int seq;
	int buy_limit_all;
	int price_type;
	int price;
	int buy_type;
	int buy_limit_day;
	ItemConfigData item;
};

struct RADiscountCarnivalSectionCfg
{
	RADiscountCarnivalSectionCfg() :section_start(0), section_end(0) {}

	int section_start;
	int section_end;
	std::vector<RADiscountCarnivalCfg> node;
};

struct RADiscountCarnivalCouponCfg						//”≈ª›»Ø≈‰÷√
{
	RADiscountCarnivalCouponCfg() : item_id(0), discount(0) {}

	int item_id;
	int discount;
};

class RandActivityManager;

class RandActivityDiscountCarnivalConfig : public RandActivityConfig
{
public:
	RandActivityDiscountCarnivalConfig();
	virtual ~RandActivityDiscountCarnivalConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RADiscountCarnivalCfg * GetDiscountCarnivalCfg(RandActivityManager* ramgr, int seq)const;
	const int GetCouponCfg(int item_id)const;

private:
	int InitDiscountCarnivalSectionCfg(TiXmlElement *RootElement);
	int InitDiscountCarnivalCouponCfg(TiXmlElement *RootElement);

	std::vector<RADiscountCarnivalCouponCfg> m_coupon_cfg;
	std::vector<RADiscountCarnivalSectionCfg> m_discount_carnival_section_cfg;

};

#endif	//__RAND_ACTIVITY_DISCOUNT_CARNIVAL_CONFIG_HPP__