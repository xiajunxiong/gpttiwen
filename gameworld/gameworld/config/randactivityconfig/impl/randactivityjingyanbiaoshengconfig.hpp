#ifndef __RAND_ACTIVITY_JING_YAN_BIAO_SHENG_CONFIG_HPP__
#define __RAND_ACTIVITY_JING_YAN_BIAO_SHENG_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"
#include "item/itembase.h"

#include <vector>
#include <set>
#include <map>

class RandActivityManager;

struct RAJingYanBiaoShengItemCfg
{
	RAJingYanBiaoShengItemCfg()
	{
		role_level = 0;
		comsume_item_id = 0;
		comsume_num = 0;
		need_gold = 0;
		get_exp = 0;
	}

	int role_level;
	int comsume_item_id;
	int comsume_num;
	int need_gold;
	int get_exp;
};

struct RAJingYanBiaoShengOtherCfg
{
	RAJingYanBiaoShengOtherCfg()
	{
		limit_type = 0;
		limit_buy_times = 0;
		limit_day_buy_times = 0;
	}

	int limit_type;
	int limit_buy_times;
	int limit_day_buy_times;
};


class RandActivityManager;
class RandActivityJingYanBiaoShengConfig : public RandActivityConfig
{
public:
	RandActivityJingYanBiaoShengConfig();
	virtual ~RandActivityJingYanBiaoShengConfig();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const RAJingYanBiaoShengItemCfg * GetRAJingYanBiaoShengCfg(int role_level) const;
	const RAJingYanBiaoShengOtherCfg & GetOtherCg() const;
private:
	int InitBuyCfg(TiXmlElement * RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);

	std::map<int, RAJingYanBiaoShengItemCfg> m_item_cfg;
	RAJingYanBiaoShengOtherCfg m_other_cfg;
};

#endif	//__RAND_ACTIVITY_LUCKY_BAG_CONFIG_HPP__