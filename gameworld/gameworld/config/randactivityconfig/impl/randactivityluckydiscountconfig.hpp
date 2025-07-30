#ifndef __RAND_ACTIVITY_LUCKY_DISCOUNT_CONFIG_HPP__
#define __RAND_ACTIVITY_LUCKY_DISCOUNT_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/raluckydiscountparam.hpp"
#include "item/itembase.h"

struct RALuckyDiscountDiscountCfg
{
	RALuckyDiscountDiscountCfg() :seq(0), proportion(0), weight(0) {}

	int seq;
	int proportion;
	int weight;
};

struct RALuckyDiscountRewardCfg
{
	RALuckyDiscountRewardCfg() :seq(0), money_type(0), price(0), limit_count(0), limit_type(0){}

	enum LIMIT_TYPE
	{
		LIMIT_TYPE_TOTAL_MIN = 0,

		LIMIT_TYPE_TOTAL,		//1 = 总限购
		LIMIT_TYPE_DAY,			//2 = 每日限购

		LIMIT_TYPE_TOTAL_MAX,
	};

	int seq;
	int money_type;
	int price;
	int limit_count;
	int limit_type;
	ItemConfigData item;
};

struct RALuckyDiscountRewardSectionCfg
{
	RALuckyDiscountRewardSectionCfg(int _section_start, int _section_end) : section_start(_section_start), section_end(_section_end)
	{}

	int section_start;
	int section_end;

	std::vector<RALuckyDiscountRewardCfg> reward_cfg;
};

class RandActivityManager;

class RandActivityLuckyDiscountConfig : public RandActivityConfig
{
public:
	RandActivityLuckyDiscountConfig();
	virtual ~RandActivityLuckyDiscountConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const int GetDiscountCfg()const;
	const RALuckyDiscountRewardCfg * GetRewardCfg(int seq, RandActivityManager * ramgr)const;

private:
	int InitRALuckyDiscountDiscoutnCfg(TiXmlElement *RootElement);
	int InitRALuckyDiscountRewardCfg(TiXmlElement *RootElement);

	std::vector<RALuckyDiscountDiscountCfg> m_discount_cfg;
	std::vector<RALuckyDiscountRewardSectionCfg> m_reward_section_cfg;

	int m_all_weight;
};

#endif	//__RAND_ACTIVITY_LUCKY_DISCOUNT_CONFIG_HPP__