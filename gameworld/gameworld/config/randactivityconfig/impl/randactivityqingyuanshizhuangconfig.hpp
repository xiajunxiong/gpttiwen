#ifndef __RAND_ACTIVITY_QING_YUAN_SHI_ZHUANG_CONFIG_HPP__
#define __RAND_ACTIVITY_QING_YUAN_SHI_ZHUANG_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itembase.h"

struct RAQingYuanShiZhuangGiftCfg
{
	RAQingYuanShiZhuangGiftCfg() : seq(0), price_type(0), price(0) {}

	short seq;
	short price_type;					// 货币类型
	int price;							// 购买价格

	ItemConfigData reward_item;
};

struct RAQingYuanShiZhuangOtherCfg
{
	RAQingYuanShiZhuangOtherCfg() : buy_times(0) {}

	short buy_times;
};

class RandActivityManager;

class RandActivityQingYuanShiZhuangConfig : public RandActivityConfig
{
public:
	RandActivityQingYuanShiZhuangConfig();
	virtual ~RandActivityQingYuanShiZhuangConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAQingYuanShiZhuangGiftCfg * GetGiftCfg(RandActivityManager * ramgr, int seq) const;
	const RAQingYuanShiZhuangOtherCfg & GetOtherCfg() const { return m_other_cfg; }

	int GetGiftSize(RandActivityManager * ramgr) const;
private:
	int InitGiftCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	std::map<int, std::vector<RAQingYuanShiZhuangGiftCfg> > m_cfg_map;
	RAQingYuanShiZhuangOtherCfg m_other_cfg;
};

#endif	//__RAND_ACTIVITY_QING_YUAN_SHI_ZHUANG_CONFIG_HPP__
