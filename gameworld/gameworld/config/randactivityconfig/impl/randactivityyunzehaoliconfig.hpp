#ifndef __RAND_ACTIVITY_YUN_ZE_HAO_LI_CONFIG_HPP__
#define __RAND_ACTIVITY_YUN_ZE_HAO_LI_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itempool.h"
#include "servercommon/activitydef.hpp"

enum RAYZ_HAO_LI_BUY_TYPE
{
	RAYZ_HAO_LI_BUY_TYPE_GOLD = 0,			// 元宝购买
	RAYZ_HAO_LI_BUY_TYPE_JADE = 16,			// 灵玉购买
};

enum RAYZ_HAO_LI_LIMIT_BUY_TYPE
{
	RAYZ_HAO_LI_LIMIT_BUY_TYPE_TODAY = 1,	// 每日限购
	RAYZ_HAO_LI_LIMIT_BUY_TYPE_TOTAL = 2,	// 活动总限购
};

struct RAYZHaoLiRewardCfg
{
	RAYZHaoLiRewardCfg():seq(0), rate(0), is_rate_added(0)
	{}

	int seq;
	int rate;
	int is_rate_added;				// 额外权重，1-开启额外权重，0-不开启
	ItemConfigData reward;
};

struct RAYZHaoLiRewardSectionCfg
{
	RAYZHaoLiRewardSectionCfg() :section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<RAYZHaoLiRewardCfg> reward;
};

struct RAYZHaoLiOtherCfg
{
	RAYZHaoLiOtherCfg():prop_added_id(0), rate_added(0), price_type(0),price(0), buy_limit(0), quantity(0)
	{}


	int prop_added_id;		// 收费道具id
	int rate_added;			// 额外增加的概率
	int price_type;			// 购买类型 0-元宝，16-灵玉
	int price;				// 花费金额
	int buy_limit;			// 限购类型，0-本次活动总限购，1-每日限购
	int quantity;			// 限购数量
	std::vector<int> time;	// 送道具时间段
	ItemConfigData limit_item;
};

class RandActivityManager;
class RandActivityYunZeHaoLiConfig : public RandActivityConfig
{
public:
	RandActivityYunZeHaoLiConfig();
	virtual ~RandActivityYunZeHaoLiConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAYZHaoLiRewardCfg * GetRandRewardCfg(RandActivityManager* ramgr, bool is_advanced_item)const;
	const RAYZHaoLiOtherCfg & GetOtherCfg()const { return m_other_cfg; }

private:
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	std::vector<RAYZHaoLiRewardSectionCfg> m_reward_section_cfg;
	RAYZHaoLiOtherCfg m_other_cfg;
};



#endif