#ifndef __GIFT_RAND_ITEM_CONFIG_HPP__
#define __GIFT_RAND_ITEM_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/servercommon.h"
#include "other/attributelist.hpp"
#include "servercommon/attributedef.hpp"
#include "servercommon/struct/itemlistparam.h"
#include <map>
#include <vector>

enum GIFT_RAND_ITEM_TYPE
{
	ITEM_TYPE_BEGIN = 0,
	ITEM_TYPE_KNAPSACK = ITEM_TYPE_BEGIN,
	ITEM_TYPE_PET = 1,
	ITEM_TYPE_END,
};

struct BoxItemCfg
{
	BoxItemCfg() {}

	int rate;
	ItemConfigData item_data;
	int item_type;
	int is_broadcast;
};
struct BoxCfg
{
	BoxCfg()
	{
		total_rate = 0;
	}

	int total_rate;
	std::vector<BoxItemCfg> item_list;
};



class GiftRandItemconfig : public ILogicConfig
{
public:
	//ºì°ü½±ÀøÅäÖÃ
	struct RedBagCfg
	{
		RedBagCfg()
		{
			box_type = 0;
			total_power = 0;
		}

		struct ItemInfo
		{
			ItemInfo()
			{
				money_type = 0;
				num = 0;
			}
			int money_type;
			int num;
		};

		int box_type;

		int total_power;
		//½±³Ømap<È¨ÖØ_begin, ½±Àø>
		std::map<int, ItemInfo> item_map;
	};
	//ºì°ü½±Àø×é<ºì°üÀàÐÍ, ºì°ü½±Àø>
	typedef std::map<int, RedBagCfg> RedBagCfgMap;
public:
	GiftRandItemconfig();
	virtual ~GiftRandItemconfig();

	virtual bool Init(const std::string &configname, std::string *err);
	bool RandItem(int box_type, BoxItemCfg* item_cfg);
	const BoxItemCfg* GetBoxItemCfg(int box_type, int index);

	const RedBagCfg::ItemInfo* GetRandRedBagReward(int box_type) const;


private:

	int InitBoxCfg(TiXmlElement *RootElement);
	int InitRedBagCfg(TiXmlElement *RootElement);

	std::map<int, BoxCfg> m_box_map;

	RedBagCfgMap m_red_box_type_map;
};

#endif