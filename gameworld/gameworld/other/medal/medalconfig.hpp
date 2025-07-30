#ifndef __MEDAL_CONFIG_HPP__
#define __MEDAL_CONFIG_HPP__

#include <vector>
#include <map>
#include <set>
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "servercommon/medaldef.h"
#include "item/medalitem/medalitem.hpp"

struct MedalItemAttrsCfg
{
	MedalItemAttrsCfg();

	struct AttrItem
	{
		AttrItem();

		int attr_type;
		int attr_min;
		int attr_max;
	};

	int rate;
	std::vector<AttrItem> attrs;
};

struct MedalItemAttrGroupCfg
{
	MedalItemAttrGroupCfg();

	int rate_count;
	std::vector<MedalItemAttrsCfg> attrs;
};

struct MedalSpGroupCfg
{
	MedalSpGroupCfg();

	struct ItemCfg
	{
		ItemCfg()
		{
			rate = 0;
			sp_id = 0;
		}

		int rate;
		int sp_id;
	};

	static const int RATE_COUNT=10000;
	std::vector<ItemCfg> item_cfgs;
};


struct MedalDecomposeCfg
{
	MedalDecomposeCfg()
	{
		medal_grade = 0;
	}

	int medal_grade;	
	ItemConfigData get_item;
};

struct MedalExchangeCfg
{
	MedalExchangeCfg()
	{
		level_min = 0;
		level_max = 0;
		item_id = 0;
		one_num = 0;
		ten_num = 0;
		reward_group = 0;
	}

	int level_min;
	int level_max;
	int item_id;
	int one_num;
	int ten_num;
	int reward_group;
};

struct MedalExchangeRewardCfg
{
	MedalExchangeRewardCfg()
	{
		rate = 0;
		is_broadcast = 0;
	}

	ItemConfigData item;
	int rate;
	bool is_broadcast;			//false:不发布 true:发布
};

struct MedalExchangeRewardListCfg
{
	MedalExchangeRewardListCfg() : weight_count(0) {}
	int weight_count;
	std::vector<MedalExchangeRewardCfg> reward_list;
};

struct MedalUpgradeCfg
{
	MedalUpgradeCfg()
	{
		grade = 0;
		color = 0;
		rate = 0;
		hearsay = 0;

		for (int i = 0; i < ARRAY_LENGTH(add_rate); ++i)
		{
			add_rate[i] = 0;
		}

		for (int i = 0; i < ARRAY_LENGTH(attrs); ++i)
		{
			attrs[i] = 0;
		}
	}

	int grade;
	int color;
	int rate;
	int hearsay;
	int add_rate[MAX_MEDAL_MAX_GRADE];
	Attribute attrs[BATTLE_ATTR_MAX];
};

struct MedalOtherCfg
{
	MedalOtherCfg()
	{
		rand_sp_id_grade = 0;
		rand_sp_id_group = 0;
		medal_exchange_item = 0;
	}

	int rand_sp_id_grade;		//进阶到多少级随机特效id
	int rand_sp_id_group;		//从那里随机
	int medal_exchange_item;	//天赐令牌转换所需道具
};

struct MedalMarketCfg
{
	MedalMarketCfg() : sold_price_coin(0) {}

	int sold_price_coin;
	std::vector<int> trade_min_gold_price;
	std::vector<int> trade_max_gold_price;
	std::vector<int> trade_recommend_price;
};

// -----------------------------------------------------------------------------------
class MedalConfig : public ILogicConfig
{
public:
	MedalConfig();
	virtual ~MedalConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const MedalItemAttrsCfg * GetRandMedalItemAttrsCfg(int attr_group_id);
	const MedalSpGroupCfg::ItemCfg * GetRandMedalItemSpItemCfg(int sp_group_id);
	const MedalDecomposeCfg * GetMedalDecomposeCfg(int grade);
	const MedalExchangeCfg * GetMedalExchangeCfg(int seq);
	const MedalExchangeRewardCfg * GetRandRewardInfoByRandGroup(int group_id);
	const MedalUpgradeCfg * GetMedalUpgradeCfg(int grade);
	int GetMedalMinGradeByColor(int color);
	int GetUpgradeRate(int main_level, int consume_level);
	const MedalOtherCfg &  GetMedalOtherCfg();
	const MedalMarketCfg* GetMedalMarketCfg(int grade);
	bool RandMedalParam(NetValueItemParam * param_data, int attr_group, int sp_group, int init_rank);
	bool IsSpeMedalItem(int sp_id);
	const int GetMaxGrade()const { return (int)m_upgrade_cfg.size() - 1; }
private:
	int InitAttrGroupCfg(TiXmlElement * RootElement);
	int InitSpGroupCfg(TiXmlElement * RootElement);
	int InitDecomposeCfg(TiXmlElement * RootElement);
	int InitExchangeCfg(TiXmlElement * RootElement);
	int InitExchangeRewardGroupCfg(TiXmlElement * RootElement);
	int InitUpgradeCfg(TiXmlElement * RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitTradeCfg(TiXmlElement * RootElement);
	int InitSpecialExchangeCfg(TiXmlElement * RootElement);

	std::map<int, MedalItemAttrGroupCfg> m_medal_attr_cfg;
	std::map<int, MedalSpGroupCfg> m_medal_sp_cfg;
	std::map<int, MedalDecomposeCfg> m_medal_decompose_cfg;
	std::map<int, MedalExchangeCfg> m_medal_exchange_cfg;
	std::map<int, MedalExchangeRewardListCfg> m_medal_exchange_reward_list_cfg;
	std::vector<MedalUpgradeCfg> m_upgrade_cfg;
	MedalOtherCfg m_other_cfg;

	MedalMarketCfg m_medal_grade_trade_cfg[MAX_MEDAL_MAX_GRADE];
	std::set<int> m_special_medal_exchange_set;			//天赐令牌置换列表
};

#endif