#ifndef __RAND_ACTIVITY_SHEN_SHOU_EN_ZE_CONFIG_HPP__
#define __RAND_ACTIVITY_SHEN_SHOU_EN_ZE_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"
#include "item/itembase.h"

#include <vector>
#include <set>
#include <map>

struct  RandActivityShenShouEnZeCfg
{
	int seq;
	int rate;
	int is_broadcast;
	std::vector<ItemConfigData> rewards;
};

struct  RandActivityShenShouEnZeSectionCfg
{
	RandActivityShenShouEnZeSectionCfg() :section_start(0), section_end(0)
	{
		rate_count = 0;
	}

	int section_start;
	int section_end;

	int rate_count;
	std::vector<RandActivityShenShouEnZeCfg> item_list;
};

struct  RandActivityShenShouEnZeSelectRewardCfg
{
	int seq;
	int is_broadcast;
	int need_draw_times;
	std::vector<ItemConfigData> rewards;
};


struct  RandActivityShenShouEnZeSelectRewardSectionCfg
{
	RandActivityShenShouEnZeSelectRewardSectionCfg() :section_start(0), section_end(0)
	{
	}

	int section_start;
	int section_end;
	std::map<int, RandActivityShenShouEnZeSelectRewardCfg> item_list;
};


struct  RandActivityShenShouEnZeBuyItemCfg
{
	RandActivityShenShouEnZeBuyItemCfg()
	{
		seq = 0;
		limit_type = 0;
		limit_buy_times_day = 0;
		limit_buy_times = 0;
		money_type = 0;
		money_value = 0;
		is_need_chong_zhi = 0;
		need_chong_zhi_gold = 0;
	}

	int seq;
	int limit_type;
	int limit_buy_times_day;
	int limit_buy_times;
	int money_type;
	int money_value;
	int is_need_chong_zhi;
	int need_chong_zhi_gold;

	std::vector<ItemConfigData> rewards;
};

struct  RandActivityShenShouEnZeBuySectionCfg
{
	RandActivityShenShouEnZeBuySectionCfg() :section_start(0), section_end(0)
	{

	}

	int section_start;
	int section_end;

	std::map<int, RandActivityShenShouEnZeBuyItemCfg> item_list;
};

struct  RandActivityShenShouEnZeOtherCfg
{
	int cfg_ver;
	int draw_need_item_id;
	int draw_need_item_num;
};

class RandActivityManager;
class RandActivityShenShouEnZeConfig : public RandActivityConfig
{
public:
	RandActivityShenShouEnZeConfig();
	virtual ~RandActivityShenShouEnZeConfig();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const RandActivityShenShouEnZeOtherCfg & GetOtherCfg() const;
	const RandActivityShenShouEnZeSectionCfg * GetRandActivityShenShouEnZeSectionCfg(RandActivityManager * ramgr) const;
	const RandActivityShenShouEnZeSelectRewardCfg * GetRandActivityShenShouEnZeSelectRewardCfg(RandActivityManager * ramgr,int reward_seq) const;
	const RandActivityShenShouEnZeBuyItemCfg * GetRandActivityShenShouEnZeBuyItemCfg(RandActivityManager * ramgr, int reward_seq) const;


	virtual int GetConfigVer() const;
private:
	int InitOtherfg(TiXmlElement * RootElement);
	int InitRandActivityShenShouEnZeCfg(TiXmlElement * RootElement);
	int InitRandActivityShenShouEnZeSelectRewardCfg(TiXmlElement * RootElement);
	int InitRandActivityShenShouEnZeBuyItemCfg(TiXmlElement * RootElement);

	std::vector<RandActivityShenShouEnZeSectionCfg> m_section_cfg;
	std::vector<RandActivityShenShouEnZeSelectRewardSectionCfg> m_times_reward_section_cfg;
	std::vector<RandActivityShenShouEnZeBuySectionCfg> m_buy_section_cfg;
	RandActivityShenShouEnZeOtherCfg m_other_cfg;
};

#endif	