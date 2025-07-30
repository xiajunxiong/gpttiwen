#ifndef __RAND_ACTIVITY_SUPER_OFF_SHOP_CONFIG_HPP__
#define __RAND_ACTIVITY_SUPER_OFF_SHOP_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/racoolsummerparam.hpp"

struct RASuperOffShopOtherCfg
{
	RASuperOffShopOtherCfg() :  cfg_ver(0), phase(0)
	{}

	int cfg_ver;
	int phase;
};

struct RASuperOffShopDiscCfg
{
	RASuperOffShopDiscCfg() : type(0), limit_type(0), buy_times(0), today_times(0), price_type(0), price(0), rate(0)
	{}

	int type;
	int limit_type;
	int buy_times;
	int today_times;
	int price_type;
	int price;
	ItemConfigData reward_item;
	int rate;
};

struct RASuperOffShopDiscGroupCfg
{
	RASuperOffShopDiscGroupCfg() : rate_count(0)
	{}

	int rate_count;
	std::vector<RASuperOffShopDiscCfg> item_list;
};

struct RASuperOffShopPhaseRangeCfg
{
	RASuperOffShopPhaseRangeCfg() : discount_group_id(0), random_down_num(0), random_up_num(0)
	{}

	int discount_group_id;
	int random_down_num;
	int random_up_num;
};

struct RASuperOffShopShopCfg
{
	RASuperOffShopShopCfg() :/* type(0), */index(0), seq(0), limit_type(0), buy_times(0), today_times(0), price_type(0), price(0)
	{}

	//int type;
	int index;
	int seq;
	int limit_type;
	int buy_times;
	int today_times;
	int price_type;
	int price;
	ItemConfigData reward_item;
};

struct RASuperOffShopSectionShopCfg
{
	RASuperOffShopSectionShopCfg() : start_time(0), end_time(0), phase(0)
	{}

	int start_time;
	int end_time;
	int phase;
	std::vector<RASuperOffShopShopCfg> shop_list;
};

struct RASuperOffShopSeverSectionShopCfg
{
	RASuperOffShopSeverSectionShopCfg() : section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<RASuperOffShopSectionShopCfg> section_list;
};

class RandActivityManager;
class RandActivitySuperOffShopConfig : public RandActivityConfig
{
public:
	enum PRICE_TYPE
	{
		PRICE_TYPE_MIN,
		PRICE_TYPE_GOLD = 0,//元宝
		PRICE_TYPE_COIN = 2,//铜币
		PRICE_TYPE_MONEY = 3,//直购
	};

public:
	RandActivitySuperOffShopConfig();
	virtual ~RandActivitySuperOffShopConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);
	const RASuperOffShopDiscCfg * GetDiscItemCfgByType(int type)const;
	bool GetDiscItemRandListCfg(RandActivityManager * ramgr, short * rand_disc_list, short list_num)const;
	const RASuperOffShopShopCfg * GetShopCfg(RandActivityManager * ramgr, int seq)const;
	int GetPhase(RandActivityManager * ramgr)const;

	virtual int GetConfigVer() const;

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitDiscGroupCfg(TiXmlElement *RootElement);
	int InitPhaseRangeCfg(TiXmlElement *RootElement);
	int InitShopCfg(TiXmlElement *RootElement);

	RASuperOffShopOtherCfg m_other_cfg;
	std::map<int, RASuperOffShopDiscCfg> m_disc_item_map;					//折扣道具 key:type
	std::map<int, RASuperOffShopDiscGroupCfg> m_disc_group_cfg;				//折扣道具组 key:disc_group_id
	std::map<int, RASuperOffShopPhaseRangeCfg> m_phase_disc_cfg;			//阶段折扣信息 key:phase
	std::vector<RASuperOffShopSeverSectionShopCfg> m_shop_section_cfg;		//正常商品配置
};

#endif