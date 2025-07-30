#ifndef __ZHOU_QI_SHOP_CONFIG_HPP__
#define __ZHOU_QI_SHOP_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include <vector>
#include <map>


struct ZhouQiShopItemCfg
{
	int seq;
	int limit_buy_num;
	int consume_item_id;
	int consume_item_num;
	std::vector<ItemConfigData> rewards;
};

struct ZhouQiShopCfg
{
	int zhou_num;
	std::map<int, ZhouQiShopItemCfg> item_list;
};


class ZhouQiShopConfig : public ILogicConfig
{
public:
	ZhouQiShopConfig();
	virtual ~ZhouQiShopConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const ZhouQiShopItemCfg * GetZhouQiShopItemCfg(int zhou_qi_num, int item_seq);
private:
	int InitStoreCfg(TiXmlElement *RootElement, std::string& errormsg);

	std::map<int, ZhouQiShopCfg> m_shop_cfg;
};

#endif