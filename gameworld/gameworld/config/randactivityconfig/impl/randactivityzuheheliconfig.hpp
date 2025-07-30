#ifndef __RAND_ACTIVITY_ZU_HE_HE_LI_HPP__
#define __RAND_ACTIVITY_ZU_HE_HE_LI_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"
#include "item/itembase.h"

#include <vector>
#include <set>
#include <map>

class RandActivityManager;


struct RAZuHeHeLiItemCfg
{
	RAZuHeHeLiItemCfg()
	{
		type = 0;
		seq = 0;
		need_chong_zhi_gold = 0;
		limit_type = 0;
		limit_buy_times_day = 0;
		limit_buy_times = 0;
		phase = 0;
		min_nuy_day = 0;
		max_buy_day = 0;
		is_chong_zhi = 0;
	}

	int type;
	int seq;
	int need_chong_zhi_gold;
	int limit_type;
	int limit_buy_times_day;
	int limit_buy_times;
	int phase;
	int min_nuy_day;
	int max_buy_day;
	int is_chong_zhi;

	ItemConfigData reward;
};

struct RAZuHeHeLiDiscountCfg
{
	RAZuHeHeLiDiscountCfg()
	{
		phase = 0;
	}

	int phase;
	std::map<int, int> num_to_discount;
};

struct RAZuHeHeLiOtherCfg
{
	RAZuHeHeLiOtherCfg()
	{
		cfg_ver = 0;
	}

	int cfg_ver;
};


class RandActivityManager;
class RandActivityZuHeHeLiConfig : public RandActivityConfig
{
public:
	RandActivityZuHeHeLiConfig();
	virtual ~RandActivityZuHeHeLiConfig();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const RAZuHeHeLiItemCfg * GetRAZuHeHeLiItemCfgBySeq(RandActivityManager * ramgr, int seq)const;
	const RAZuHeHeLiItemCfg * GetRAZuHeHeLiItemCfg(RandActivityManager * ramgr, int type) const;
	double GetDiscount(int phase,int buy_num) const;
	virtual int GetConfigVer() const;
private:
	int InitDiscountCfg(TiXmlElement * RootElement);
	int InitBuyCfg(TiXmlElement * RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);


	std::vector<RAZuHeHeLiItemCfg> m_item_cfg;
	std::map<int, RAZuHeHeLiDiscountCfg> m_discount_cfg;
	RAZuHeHeLiOtherCfg m_other_cfg;
};

#endif	