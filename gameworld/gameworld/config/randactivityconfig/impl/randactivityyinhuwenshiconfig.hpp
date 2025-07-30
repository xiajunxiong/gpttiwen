#ifndef __RAND_ACTIVITY_YINHU_WENSHI_CONFIG_HPP__
#define __RAND_ACTIVITY_YINHU_WENSHI_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "config/randactivityconfig/randactivityconfig.hpp"


struct RAYinHuWenShiGroupCfg
{
	RAYinHuWenShiGroupCfg() :type(0), is_hearsay(false) {}

	short type;
	std::vector<ItemConfigData> reward;
	bool is_hearsay;
};

struct RAYinHuWenShiGroupMap
{
	RAYinHuWenShiGroupMap() : max_rate(0) {}

	int max_rate;
	std::map<int, RAYinHuWenShiGroupCfg> map_cfg;
};

struct RAYinHuWenShiBuyCfg
{
	RAYinHuWenShiBuyCfg() : price(0) {}

	ItemConfigData reward_item;
	int price;
};


class RandActivityManager;
class RandActivityYinHuWenShiConfig : public RandActivityConfig
{
public:
	RandActivityYinHuWenShiConfig();
	virtual ~RandActivityYinHuWenShiConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAYinHuWenShiGroupCfg * GetGiftCfg(RandActivityManager * ramgr, int seq) const;
	const RAYinHuWenShiBuyCfg * GetBuyCfg(RandActivityManager * ramgr) const;

	int GetGiftMax(RandActivityManager * ramgr) const;
private:
	int InitGiftCfg(TiXmlElement *RootElement);
	int InitGroupCfg(TiXmlElement *RootElement);
	int InitBuyCfg(TiXmlElement *RootElement);


	std::map<short, std::vector<short> > m_gift_cfg;
	std::map<short, RAYinHuWenShiGroupMap> m_group_cfg;
	std::map<short, RAYinHuWenShiBuyCfg> m_buy_cfg;
};


#endif	// __RAND_ACTIVITY_YINHU_WENSHI_CONFIG_HPP__