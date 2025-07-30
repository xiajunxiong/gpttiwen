#ifndef __RAND_ACTIVITY_XIAN_SHI_BAO_DAI_CONFIG_HPP__
#define __RAND_ACTIVITY_XIAN_SHI_BAO_DAI_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"
#include "item/itembase.h"

#include <vector>
#include <set>
#include <map>

class RandActivityManager;

struct RAXianShiBaoDaiItemCfg
{
	RAXianShiBaoDaiItemCfg()
	{
		seq = 0;
		need_chong_zhi_gold = 0;
		limit_type = 0;
		limit_buy_times_day = 0;
		limit_buy_times = 0;
		add_gold = 0;
	}

	int seq;
	int need_chong_zhi_gold;
	int limit_type;
	int limit_buy_times_day;
	int limit_buy_times;

	int add_gold;
	std::vector<ItemConfigData> rewards;
};

struct RAXianShiBaoDaiSectionCfg
{
	RAXianShiBaoDaiSectionCfg() :section_start(0), section_end(0)
	{
	}

	int section_start;
	int section_end;
	std::vector<RAXianShiBaoDaiItemCfg> item;
};

struct RAXianShiBaoDaiOtherCfg
{
	RAXianShiBaoDaiOtherCfg()
	{}

	std::vector<ItemConfigData> free_reward;
};

class RandActivityManager;
class RandActivityXianShiBaoDaiConfig : public RandActivityConfig
{
public:
	RandActivityXianShiBaoDaiConfig();
	virtual ~RandActivityXianShiBaoDaiConfig();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const RAXianShiBaoDaiItemCfg * GetRAXianShiBaoDaiItemCfg(RandActivityManager * ramgr, int seq) const;
	int GetRAXianShiBaoDaiItemNum(RandActivityManager * ramgr) const;
	const RAXianShiBaoDaiOtherCfg & GetOtherCfg()const { return m_other_cfg; }
private:
	int InitBuyCfg(TiXmlElement * RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);

	std::vector<RAXianShiBaoDaiSectionCfg> m_section_cfg;
	RAXianShiBaoDaiOtherCfg m_other_cfg;
};

#endif	//__RAND_ACTIVITY_LUCKY_BAG_CONFIG_HPP__