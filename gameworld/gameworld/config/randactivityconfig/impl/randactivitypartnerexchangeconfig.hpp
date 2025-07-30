#ifndef __RAND_ACTIVITY_PARTNER_EXCHANGE_CONFIG_HPP__
#define __RAND_ACTIVITY_PARTNER_EXCHANGE_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/rapartnerexchangeparam.hpp"
#include "item/itembase.h"

struct RAPartnerExchangeCfg			//伙伴兑换
{
	enum BUY_TEPY
	{
		BUY_TEPY_NO = 0,	// 无
		BUY_TEPY_ALL = 1,	// 总限兑
		BUY_TEPY_TODAY = 2,	// 每日限兑
	};
	RAPartnerExchangeCfg() :section_start(0), section_end(0), seq1(0), number(0), condition_id(0), item_id(0), condition_num(0), hearsay(0),
		buy_tepy(BUY_TEPY_NO), buy_seccond_day(0) {}

	int section_start;
	int section_end;
	int seq1;					//活动类型
	ItemConfigData item1;		//与该活动关联的道具
	ItemConfigData item2;		//与该活动关联的道具
	ItemConfigData item3;		//与该活动关联的道具

	int number;
	ItemConfigData reward_item;		//奖励

	int condition_id;				//需要已激活伙伴id
	int item_id;					//需要碎片id
	int condition_num;				//需要碎片数量
	int hearsay;

	int buy_tepy;
	int buy_seccond_day;
};

class RandActivityManager;

class RandActivityPartnerExchangeConfig : public RandActivityConfig
{
public:
	RandActivityPartnerExchangeConfig();
	virtual ~RandActivityPartnerExchangeConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAPartnerExchangeCfg* GetPartnerExchangeBySeq(RandActivityManager* ramgr, int seq)const;
	int GetActOpenDay(RandActivityManager* ramgr, int activity_type) const;							//当前时间	    到	活动开启时间天数
	int GetActRealOpenDay(RandActivityManager* ramgr, int activity_type) const;						//活动开启时间	到	开服时间天数

private:
	int InitPartnerExchangeCfg(TiXmlElement *RootElement);

	std::map<int, std::map<int, RAPartnerExchangeCfg> > m_partner_exchange_map;			// 伙伴兑换
};

#endif	//__RAND_ACTIVITY_PARTNER_EXCHANGE_CONFIG_HPP__