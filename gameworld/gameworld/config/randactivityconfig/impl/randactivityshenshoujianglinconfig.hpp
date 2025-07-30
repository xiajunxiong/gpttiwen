#ifndef __RAND_ACTIVITY_SHEN_SHOU_JIANG_LIN_CONFIG_HPP__
#define __RAND_ACTIVITY_SHEN_SHOU_JIANG_LIN_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"
#include "item/itembase.h"

#include <vector>
#include <set>
#include <map>

struct RAShenShouJiangLinItemCfg
{
	RAShenShouJiangLinItemCfg()
	{
		seq = 0;
		rate = 0;
		gold_num = 0;
		pool_get_reward_percent = 0;
		broadcast = 0;
		bao_di_id = 0;
		bao_di_times = 0;
	}

	int seq;
	int rate;
	int gold_num;
	int pool_get_reward_percent;
	ItemConfigData reward;
	int broadcast;    //0是不播,1百分奖励文字，2：道具奖励文字
	int bao_di_id;
	int bao_di_times;
};

struct RAShenShouJiangLinSectionCfg
{
	RAShenShouJiangLinSectionCfg() :section_start(0), section_end(0)
	{
		rate_count = 0;
	}

	int section_start;
	int section_end;
	int rate_count;
	std::vector<RAShenShouJiangLinItemCfg> item;
};

struct RAShenShouJiangLinOtherCfg
{
	RAShenShouJiangLinOtherCfg()
	{
		pool_item_id = 0;
		pool_item_init_num = 0;
		pool_gold_init_num = 0;
		pool_add_item_draw_items = 0;
		pool_add_item_num = 0;
		pool_add_gold_num = 0;
		draw_comsume_item_id = 0;
		draw_comsume_num = 0;
		cfg_ver = 0;
	}

	int pool_item_id;
	int pool_item_init_num;
	int pool_gold_init_num;
	int pool_add_item_draw_items;
	int pool_add_item_num;
	int pool_add_gold_num;
	int draw_comsume_item_id;
	int draw_comsume_num;
	int cfg_ver;
};

struct RAShenShouJiangLinOtherSectionCfg
{
	RAShenShouJiangLinOtherSectionCfg(int _section_start, int _section_end) : section_start(_section_start), section_end(_section_end) 
	{}	
	
	int section_start;
	int section_end;
	RAShenShouJiangLinOtherCfg other_cfg;
};

struct RAShenShouJiangLinBuyItemCfg
{
	RAShenShouJiangLinBuyItemCfg()
	{
		type = 0;
		seq = 0;
		need_chong_zhi = 0;
		limit_type = 0;
		limit_buy_times_day = 0;
		limit_buy_times = 0;
	}

	int type;
	int seq;
	int need_chong_zhi;
	int limit_type;
	int limit_buy_times_day;
	int limit_buy_times;

	std::vector<ItemConfigData> rewards;
};

struct RAShenShouJiangLinBuySectionCfg
{
	RAShenShouJiangLinBuySectionCfg() :section_start(0), section_end(0)
	{
	}

	int section_start;
	int section_end;
	std::vector<RAShenShouJiangLinBuyItemCfg> item;
};


class RandActivityManager;
class RandActivityShenShouJiangLinConfig : public RandActivityConfig
{
public:
	RandActivityShenShouJiangLinConfig();
	virtual ~RandActivityShenShouJiangLinConfig();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);
	virtual int GetConfigVer(RandActivityManager * ramgr) const;

	const RAShenShouJiangLinOtherCfg & GetOtherCfg(RandActivityManager * ramgr) const;
	const RAShenShouJiangLinItemCfg * GetRandReward(RandActivityManager * ramgr) const;
	const RAShenShouJiangLinBuyItemCfg * GetRAShenShouJiangLinBuyItemCfg(RandActivityManager * ramgr, int seq) const;
	const RAShenShouJiangLinSectionCfg * GetRAShenShouJiangLinSectionCfg(RandActivityManager * ramgr) const;
	std::vector<const RAShenShouJiangLinItemCfg *> GetRAShenShouJiangLinBaoDiCfgList(RandActivityManager * ramgr,std::set<int> & bao_di_id_list) const;
private:
	int InitDrawCfg(TiXmlElement * RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitBuyCfg(TiXmlElement * RootElement);

	std::vector<RAShenShouJiangLinSectionCfg> m_section_cfg;
	std::vector<RAShenShouJiangLinOtherSectionCfg> m_other_section_cfg;
	std::vector<RAShenShouJiangLinBuySectionCfg> m_buy_section_cfg;
};

#endif	//__RAND_ACTIVITY_LUCKY_BAG_CONFIG_HPP__