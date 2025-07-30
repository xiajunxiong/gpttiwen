#ifndef __RAND_ACTIVITY_CAI_SHEN_JU_BAO_CONFIG_HPP__
#define __RAND_ACTIVITY_CAI_SHEN_JU_BAO_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"
#include "item/itembase.h"

class RandActivityManager;

struct RACaiShenJuBaoItemCfg
{
	RACaiShenJuBaoItemCfg()
	{
		seq = 0;
		add_gold = 0;
		extra_add_gold = 0;
		need_chong_zhi_gold = 0;
		limit_type = 0;
		limit_buy_times_day = 0;
		limit_buy_times = 0;
	}

	int seq;
	int add_gold;
	int extra_add_gold;
	int need_chong_zhi_gold;
	int limit_type;
	int limit_buy_times_day;
	int limit_buy_times;
};

struct RACaiShenJuBaoSectionCfg
{
	RACaiShenJuBaoSectionCfg() :section_start(0), section_end(0)
	{
	}

	int section_start;
	int section_end;
	std::vector<RACaiShenJuBaoItemCfg> item;
};

class RandActivityManager;
class RandActivityCaiShenJuBaoConfig : public RandActivityConfig
{
public:
	RandActivityCaiShenJuBaoConfig();
	virtual ~RandActivityCaiShenJuBaoConfig();
	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RACaiShenJuBaoItemCfg * GetRACaiShenJuBaoItemCfg(RandActivityManager * ramgr,int seq) const;
	int GetRACaiShenJuBaoItemNum(RandActivityManager * ramgr) const;
private:
	int InitBuyCfg(TiXmlElement *RootElement);

	std::vector<RACaiShenJuBaoSectionCfg> m_section_cfg;
};

#endif	//__RAND_ACTIVITY_LUCKY_BAG_CONFIG_HPP__