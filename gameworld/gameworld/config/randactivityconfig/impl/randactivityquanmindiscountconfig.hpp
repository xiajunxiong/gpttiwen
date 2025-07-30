#ifndef __RAND_ACTIVITY_QUAN_MIN_DISCOUNT_CONFIG_HPP__
#define __RAND_ACTIVITY_QUAN_MIN_DISCOUNT_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/raquanmindiscountparam.hpp"
#include "item/itembase.h"

struct RAQuanMinDiscountCfg
{
	RAQuanMinDiscountCfg() : 
		seq(0), money_type(0), discount_id(0), price(0), hour(0), support_number(0),
		buy_type(0), buy_limit_day(0),buy_limit_all(0){}

	int seq;
	int money_type;
	int discount_id;
	int price;
	short hour;				//每过多少小时 自动增加support_number购买人数
	short support_number;
	int buy_type;
	int buy_limit_day;
	int buy_limit_all;
	ItemConfigData item;
};

struct RAQuanMinDiscountSectionCfg
{
	RAQuanMinDiscountSectionCfg() : section_start(0), section_end(0) {}

	int section_start;
	int section_end;
	std::vector<RAQuanMinDiscountCfg> node;
};

struct RAQuanMinDiscountDiscountCfg
{
	RAQuanMinDiscountDiscountCfg() : discount_proportion(0), discount_number(0) {}

	double discount_proportion;
	int discount_number;
};

class RandActivityManager;

class RandActivityQuanMinDiscountConfig : public RandActivityConfig
{
public:
	RandActivityQuanMinDiscountConfig();
	virtual ~RandActivityQuanMinDiscountConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAQuanMinDiscountCfg * GetQuanMinDiscountCfg(RandActivityManager* ramgr, int seq)const;
	double GetDiscountCfg(int discount_id, int discount_num) const;
	double GetMaxDiscountCfg(int discount_id) const;						//获取某折扣ID最大折扣（最低折扣）
	int GetMaxSeq(RandActivityManager* ramgr) const;
	int GetMinOrMaxBuyNeedNum(int discount_id, bool is_max) const;				//获取某折扣ID最大折扣所需人数或者最低折扣所需人数

private:
	int InitQuanMinDiscountSectionCfg(TiXmlElement *RootElement);
	int InitQuanMinDiscountDiscountCfg(TiXmlElement *RootElement);

	std::vector<RAQuanMinDiscountSectionCfg> m_section_cfg;
	std::map<int, std::vector<RAQuanMinDiscountDiscountCfg> > m_discount_cfg;
};

#endif	//__RAND_ACTIVITY_QUAN_MIN_DISCOUNT_CONFIG_HPP__