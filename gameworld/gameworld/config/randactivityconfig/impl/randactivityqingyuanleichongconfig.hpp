#ifndef __RAND_ACTIVITY_QING_YUAN_LEI_CHONG_CONFIG_HPP__
#define __RAND_ACTIVITY_QING_YUAN_LEI_CHONG_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itembase.h"

struct RAQingYuanLeiChongCfg
{
	RAQingYuanLeiChongCfg() : seq(0), acc_price(0) {}

	short seq;
	int acc_price;					// 累计充值金额

	std::vector<ItemConfigData> reward_item;
};

struct RAQingYuanLeiChongOtherCfg
{
	RAQingYuanLeiChongOtherCfg() : continue_times(0), reset_times(0) {}

	short continue_times;			// 持续时间 单位：小时
	short reset_times;				// 重置时间 单位：小时
};

class RandActivityManager;
class RandActivityQingYuanLeiChongConfig : public RandActivityConfig
{
public:
	RandActivityQingYuanLeiChongConfig();
	virtual ~RandActivityQingYuanLeiChongConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAQingYuanLeiChongCfg * GetCfg(RandActivityManager * ramgr, int seq, unsigned int join_times) const;
	const RAQingYuanLeiChongOtherCfg & GetOtherCfg() const { return m_other_cfg; }

	int GetGiftSize(RandActivityManager * ramgr, unsigned int join_times) const;
private:
	int InitCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	std::map<int, std::vector<RAQingYuanLeiChongCfg> > m_cfg_map;
	RAQingYuanLeiChongOtherCfg m_other_cfg;
};

#endif	//__RAND_ACTIVITY_QING_YUAN_LEI_CHONG_CONFIG_HPP__
