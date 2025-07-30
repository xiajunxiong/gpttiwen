#ifndef __RAND_ACTIVITY_YIN_HU_BAI_SUI_CONFIG_HPP__
#define __RAND_ACTIVITY_YIN_HU_BAI_SUI_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "config/randactivityconfig/randactivityconfig.hpp"


struct RAYinHuBaiSuiGiftCfg
{
	RAYinHuBaiSuiGiftCfg() : activity_days(0) {}

	short activity_days;
	std::vector<ItemConfigData> item;
};

struct RAYinHuBaiSuiBuyCfg
{
	RAYinHuBaiSuiBuyCfg() : price(0) {}

	ItemConfigData reward_item;
	int price;
};


class RandActivityManager;
class RandActivityYinHuBaiSuiConfig : public RandActivityConfig
{
public:
	RandActivityYinHuBaiSuiConfig();
	virtual ~RandActivityYinHuBaiSuiConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAYinHuBaiSuiGiftCfg * GetGiftCfg(RandActivityManager * ramgr, int seq) const;
	const RAYinHuBaiSuiBuyCfg * GetBuyCfg(RandActivityManager * ramgr) const;

	int GetGiftMax(RandActivityManager * ramgr) const;

private:
	int InitGiftCfg(TiXmlElement *RootElement);
	int InitBuyCfg(TiXmlElement *RootElement);
	

	std::map<short, std::vector<RAYinHuBaiSuiGiftCfg> > m_gift_cfg;
	std::map<short, RAYinHuBaiSuiBuyCfg> m_buy_cfg;
};


#endif	// __RAND_ACTIVITY_YIN_HU_BAI_SUI_CONFIG_HPP__