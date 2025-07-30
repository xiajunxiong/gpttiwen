#ifndef __KNAPSACK_CONFIG_HPP__
#define __KNAPSACK_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"

struct KnapsackGridCfg
{
	static const int MAX_EXTEND_TIMES = 40;

	KnapsackGridCfg() : column_id(-1), start_grid_num(0), extend_grid_num(0), max_grid_num(0), max_extend_times(0)
	{
		memset(extend_cost_gold_list, 0, sizeof(extend_cost_gold_list));
	}

	bool CanExtend() const
	{
		return max_extend_times > 0 && extend_grid_num > 0;
	}

	int column_id;
	int start_grid_num;
	int extend_grid_num;
	int max_grid_num;

	int max_extend_times;
	int extend_cost_gold_list[MAX_EXTEND_TIMES];
};

struct ItemConvertByJobKey	
{			
	bool operator<(const ItemConvertByJobKey &  other) const 
	{
		if (item_id < other.item_id)
			return true;
		if (item_id > other.item_id)
			return false;

		if (job < other.job)
			return true;
		if (job > other.job)
			return false;

		return false;
	}

	ItemConvertByJobKey(int item_id, int job)
	{
		this->item_id = item_id;
		this->job = job;
	}

	ItemConvertByJobKey()
	{
		item_id = 0;
		job = 0;
	}

	int item_id;
	int job;
};

enum ITEM_RETURN_MAIL_TYPE
{
	ITEM_RETURN_MAIL_TYPE_BEGIN = 0,
	ITEM_RETURN_MAIL_TYPE_CRYSTAL = 1,			//秘宝阁封印石头
	ITEM_RETURN_MAIL_TYPE_DREAM_NOTES = 2,		//梦渊卷轴
	ITEM_RETURN_MAIL_TYPE_PET_STR_GUA = 3,		//宠物强化保底
	ITEM_RETURN_MAIL_TYPE_HEART_SKILL = 4,		//心法残卷
	ITEM_RETURN_MAIL_TYPE_CRYSTAL_NORMAL = 5,	//普通秘宝阁钥匙
	ITEM_RETURN_MAIL_TYPE_BREAK_ITEM = 6,		//灵力丹
	ITEM_RETURN_MAIL_TYPE_CRYSTAL_JING_YING = 7,//上古遗迹(水晶副本-精英副本)
	ITEM_RETURN_MAIL_TYPE_KNAPSACK_BACK = 8,	//材料道具统一回收
	ITEM_RETURN_MAIL_TYPE_FIVE_SQUARE = 9,		//五方擂台回收
	ITEM_RETURN_MAIL_TYPE_VOCULTIMATE = 10,		//淬炼回收
	ITEM_RETURN_MAIL_TYPE_EQUIP_JEWELRY_UP = 11,//装备强化回收
	ITEM_RETURN_MAIL_TYPE_PARTNER_SOUL = 12,	//伙伴天赋精魂回收
	ITEM_RETURN_MAIL_TYPE_SMART_MOUNTS = 13,	//灵骑技能道具回收
	ITEM_RETURN_MAIL_TYPE_CHANGE_CARD = 14,		//变身卡道具回收

	ITEM_RETURN_MAIL_TYPE_MAX
};

struct ItemReturnConfig
{
	ItemReturnConfig() : item_id(0), price_type(0), price(0), mail_type(0), mail_sort(0) {}
	int item_id;

	int price_type;
	int price;
	int mail_type;
	int mail_sort;
	std::vector<ItemConfigData> return_item;
};

class KnapsackConfig : public ILogicConfig
{
public:
	KnapsackConfig();
	virtual ~KnapsackConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const KnapsackGridCfg* GetKnapsackGridCfg(int column);
	int GetItemIdConvertByJob(int item_id, int job);
	int GetSubTypePrior(int item_sub_type);
	const ItemReturnConfig * GetItemReturnConfig(int item_id);
private:
	int InitBagCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitExtendCfg(TiXmlElement *RootElement);
	int InitItemConvertCfg(TiXmlElement *RootElement);
	int InitItemReturnCfg(TiXmlElement *RootElement);

	KnapsackGridCfg m_grid_cfg_list[ItemNamespace::ITEM_COLUMN_TYPE_MAX];

	std::map<ItemConvertByJobKey, int>  m_item_convert_map;			//根据职业物品转换map

	int m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_MAX];

	std::vector<ItemReturnConfig> m_item_vec;
	ItemReturnConfig* item_return[MAX_UINT16];
};

#endif