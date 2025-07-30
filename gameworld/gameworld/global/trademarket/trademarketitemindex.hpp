#ifndef __TRADE_MARKET_ITEM_INDEX_HPP__
#define __TRADE_MARKET_ITEM_INDEX_HPP__

#include <map>
#include <vector>
#include "usersaleitem.hpp"
#include "servercommon/medaldef.h"
#include "item/itembase.h"
#include "servercommon/struct/battle/battle_special_effect_def.hpp"
#include "other/pet/petconfig.hpp"

enum TRADE_ITEM_ORDER
{
	TRADE_ITEM_ORDER_PRICE_UP = 0,		// 价格由低到高
	TRADE_ITEM_ORDER_PRICE_DOWN,		// 价格由高到低
	TRADE_ITEM_ORDER_MAX,
};

enum TRADE_PET_INDEX_ORDER
{
	TRADE_PET_STR_LEVEL_ORDER_UP = 0,	//!< 0 强化等级由低到高 （相同时价格低到高）
	TRADE_PET_STR_LEVEL_ORDER_DOWN,		//!< 1 强化等级由高到低 （相同时价格低到高）
	TRADE_PET_PRICE_UP,					//!< 2 价格由低到高 （相同时强化等级低到高）
	TRADE_PET_PRICE_DOWN,				//!< 3 价格由高到低 （相同时强化等级低到高）
	TRADE_PET_INDEX_ORDER_MAX,
};

struct SaleItemIndex // 这里都是相同item_id的物品
{
	bool operator< (const SaleItemIndex& index) const
	{
		if (price != index.price)
		{
			if (TRADE_ITEM_ORDER_PRICE_UP == order_type)
			{
				return price < index.price;
			}
			else
			{
				return price > index.price;
			}
		}
		else if (put_on_shelve_time != index.put_on_shelve_time)
		{
			return put_on_shelve_time < index.put_on_shelve_time;
		}
		else
		{
			return unique_key < index.unique_key;
		}
	}

	int uid;
	unsigned int put_on_shelve_time;
	int price;
	ItemID item_id;
	short order_type;
	long long unique_key;
};

struct SaleTypeIndex // 这里都是同一种类的物品
{
	bool operator< (const SaleTypeIndex& index) const
	{
		if (price != index.price)
		{
			if (TRADE_ITEM_ORDER_PRICE_UP == order_type)
			{
				return price < index.price;
			}
			else
			{
				return price > index.price;
			}
		}
		else if (item_id != index.item_id)
		{
			return item_id < index.item_id;
		}
		else if (put_on_shelve_time != index.put_on_shelve_time)
		{
			return put_on_shelve_time < index.put_on_shelve_time;
		}
		else
		{
			return unique_key < index.unique_key;
		}
	}

	int uid;
	unsigned int put_on_shelve_time;
	int price;
	ItemID item_id;
	short order_type;
	long long unique_key;
};

struct SalePetIndex // 这里都是相同item_id的宠物
{
	int uid;
	unsigned int put_on_shelve_time;
	int price;
	ItemID item_id;
	short pet_str_level; // 强化等级
	long long unique_key;
};

struct SalePetQualityIndex
{
	int uid;
	unsigned int put_on_shelve_time;
	int price;
	short quality;
	short pet_str_level; // 强化等级
	long long unique_key;
};

struct PetOrderPriceUp
{
	bool operator()(const SalePetIndex& index1, const SalePetIndex& index2) const
	{
		if (index1.price != index2.price)
			return index1.price < index2.price;
		else if (index1.pet_str_level != index2.pet_str_level)
			return index1.pet_str_level > index2.pet_str_level;
		else if (index1.put_on_shelve_time != index2.put_on_shelve_time)
			return index1.put_on_shelve_time < index2.put_on_shelve_time;
		else
			return index1.unique_key < index2.unique_key;
	}
};

struct PetOrderPriceDown
{
	bool operator()(const SalePetIndex& index1, const SalePetIndex& index2) const
	{
		if (index1.price != index2.price)
			return index1.price > index2.price;
		else if (index1.pet_str_level != index2.pet_str_level)
			return index1.pet_str_level > index2.pet_str_level;
		else if (index1.put_on_shelve_time != index2.put_on_shelve_time)
			return index1.put_on_shelve_time < index2.put_on_shelve_time;
		else
			return index1.unique_key < index2.unique_key;
	}
};

struct PetOrderStrLevelUp
{
	bool operator()(const SalePetIndex& index1, const SalePetIndex& index2) const
	{
		if (index1.pet_str_level != index2.pet_str_level)
			return index1.pet_str_level < index2.pet_str_level;
		else if (index1.price != index2.price)
			return index1.price < index2.price;
		else if (index1.put_on_shelve_time != index2.put_on_shelve_time)
			return index1.put_on_shelve_time < index2.put_on_shelve_time;
		else
			return index1.unique_key < index2.unique_key;
	}
};

struct PetOrderStrLevelDown
{
	bool operator()(const SalePetIndex& index1, const SalePetIndex& index2) const
	{
		if (index1.pet_str_level != index2.pet_str_level)
			return index1.pet_str_level > index2.pet_str_level;
		else if (index1.price != index2.price)
			return index1.price < index2.price;
		else if (index1.put_on_shelve_time != index2.put_on_shelve_time)
			return index1.put_on_shelve_time < index2.put_on_shelve_time;
		else
			return index1.unique_key < index2.unique_key;
	}
};

struct PetQualityOrderPriceUp
{
	bool operator()(const SalePetQualityIndex& index1, const SalePetQualityIndex& index2) const
	{
		if (index1.price != index2.price)
			return index1.price < index2.price;
		else if (index1.pet_str_level != index2.pet_str_level)
			return index1.pet_str_level > index2.pet_str_level;
		else if (index1.put_on_shelve_time != index2.put_on_shelve_time)
			return index1.put_on_shelve_time < index2.put_on_shelve_time;
		else
			return index1.unique_key < index2.unique_key;
	}
};

struct PetQualityOrderPriceDown
{
	bool operator()(const SalePetQualityIndex& index1, const SalePetQualityIndex& index2) const
	{
		if (index1.price != index2.price)
			return index1.price > index2.price;
		else if (index1.pet_str_level != index2.pet_str_level)
			return index1.pet_str_level > index2.pet_str_level;
		else if (index1.put_on_shelve_time != index2.put_on_shelve_time)
			return index1.put_on_shelve_time < index2.put_on_shelve_time;
		else
			return index1.unique_key < index2.unique_key;
	}
};

struct PetQualityOrderStrLevelUp
{
	bool operator()(const SalePetQualityIndex& index1, const SalePetQualityIndex& index2) const
	{
		if (index1.pet_str_level != index2.pet_str_level)
			return index1.pet_str_level < index2.pet_str_level;
		else if (index1.price != index2.price)
			return index1.price < index2.price;
		else if (index1.put_on_shelve_time != index2.put_on_shelve_time)
			return index1.put_on_shelve_time < index2.put_on_shelve_time;
		else
			return index1.unique_key < index2.unique_key;
	}
};

struct PetQualityOrderStrLevelDown
{
	bool operator()(const SalePetQualityIndex& index1, const SalePetQualityIndex& index2) const
	{
		if (index1.pet_str_level != index2.pet_str_level)
			return index1.pet_str_level > index2.pet_str_level;
		else if (index1.price != index2.price)
			return index1.price < index2.price;
		else if (index1.put_on_shelve_time != index2.put_on_shelve_time)
			return index1.put_on_shelve_time < index2.put_on_shelve_time;
		else
			return index1.unique_key < index2.unique_key;
	}
};

struct SaleMedalIndex
{
	int uid;
	unsigned int put_on_shelve_time;
	int price;
	ItemID item_id;
	short grade; // 阶级
	short color; // 品质
	short order_type;
	long long unique_key;
};

struct MedalOrderGradeDown
{
	bool operator()(const SaleMedalIndex& index1, const SaleMedalIndex& index2) const
	{
		if (index1.price != index2.price)
		{
			if (TRADE_ITEM_ORDER_PRICE_UP == index1.order_type)
			{
				return index1.price < index2.price;
			}
			else
			{
				return index1.price > index2.price;
			}
		}
		else if (index1.grade != index2.grade)
			return index1.grade > index2.grade;
		else if (index1.item_id != index2.item_id)
			return index1.item_id < index2.item_id;
		else if (index1.put_on_shelve_time != index2.put_on_shelve_time)
			return index1.put_on_shelve_time < index2.put_on_shelve_time;
		else
			return index1.unique_key < index2.unique_key;
	}
};

class TradeMarketItemIndex
{
public:
	typedef std::map<SaleItemIndex, SaleItemBase*> ItemIndexMap;
	typedef std::map<SaleTypeIndex, SaleItemBase*> TypeIndexMap;
	typedef std::map<SalePetIndex, SaleItemBase*, PetOrderPriceUp> PetPriceUpIndexMap;
	typedef std::map<SalePetIndex, SaleItemBase*, PetOrderPriceDown> PetPriceDownIndexMap;
	typedef std::map<SalePetIndex, SaleItemBase*, PetOrderStrLevelUp> PetStrLevelUpIndexMap;
	typedef std::map<SalePetIndex, SaleItemBase*, PetOrderStrLevelDown> PetStrLevelDownIndexMap;
	typedef std::map<SalePetQualityIndex, SaleItemBase*, PetQualityOrderPriceUp> PetQualityPriceUpIndexMap;
	typedef std::map<SalePetQualityIndex, SaleItemBase*, PetQualityOrderPriceDown> PetQualityPriceDownIndexMap;
	typedef std::map<SalePetQualityIndex, SaleItemBase*, PetQualityOrderStrLevelUp> PetQualityStrLevelUpIndexMap;
	typedef std::map<SalePetQualityIndex, SaleItemBase*, PetQualityOrderStrLevelDown> PetQualityStrLevelDownIndexMap;
	typedef std::map<SaleMedalIndex, SaleItemBase*, MedalOrderGradeDown> MedalGradeDownIndexMap;

public:
	TradeMarketItemIndex();
	~TradeMarketItemIndex();

	void Update(unsigned long interval, time_t now_second);

	bool OnSearchByItem(bool is_search_announce, short order_type, ItemID item_id, int req_page, int req_item_per_page, SaleItemBase** out_list,
		int* out_count, int* out_cur_page, int* out_total_page);
	bool OnSearchByType(bool is_search_announce, short order_type, short item_search_type, int req_page, int req_item_per_page, SaleItemBase** out_list,
		int* out_count, int* out_cur_page, int* out_total_page);
	bool OnSearchByBigType(bool is_search_announce, short order_type, short item_search_big_type, int req_page, int req_item_per_page, SaleItemBase** out_list,
		int* out_count, int* out_cur_page, int* out_total_page);
	bool OnSearchByTypeWithLevel(bool is_search_announce, short order_type, short level, short item_search_type, int req_page, int req_item_per_page, SaleItemBase** out_list,
		int* out_count, int* out_cur_page, int* out_total_page);
	bool OnSearchForAutoBuy(ItemID item_id, int need_num, int expect_price, std::vector<SaleItemBase*>* out_list);
	bool OnSearchByAdvertise(bool is_search_announce, ItemID item_id, int price, unsigned int put_on_shelve_time, int seller_uid, long long unique_key, int req_item_per_page, SaleItemBase** out_list,
		int* out_count, int* out_cur_page, int* out_total_page, short* out_cursor_index);
	bool OnSearchForSellingList(ItemID item_id, int max_num, SaleItemBase** out_list, int* out_num);
	bool OnSearchForMedalSellingList(short medal_effect_type, int max_num, SaleItemBase** out_list, int* out_num);
	bool OnSearchPetForQuickBuy(short order_type, const SalePetIndex& pet_index, int req_item_per_page, SaleItemBase** out_list,
		int* out_count);
	bool OnSearchPetQualityForQuickBuy(short order_type, const SalePetQualityIndex& pet_index, int req_item_per_page, SaleItemBase** out_list,
		int* out_count);
	bool OnSearchByMedalColor(bool is_search_announce, short order_type, short medal_color, int req_page, int req_item_per_page, SaleItemBase** out_list,
		int* out_count, int* out_cur_page, int* out_total_page);
	bool OnSearchByMedalEffectTypeAndColor(bool is_search_announce, short order_type, short sp_type, short medal_color, int req_page, int req_item_per_page, SaleItemBase** out_list,
		int* out_count, int* out_cur_page, int* out_total_page);
	bool OnSearchByMedalColorAndProf(bool is_search_announce, short order_type, short medal_color, int medal_prof, int req_page, int req_item_per_page, SaleItemBase** out_list,
		int* out_count, int* out_cur_page, int* out_total_page);

	bool OnAddItemOnSale(SaleItemBase* usi);
	bool OnRemoveItemOnSale(SaleItemBase* usi);
	bool OnAddItemOnAnnounce(SaleItemBase* usi);
	bool OnRemoveItemOnAnnounce(SaleItemBase* usi);
	bool IsItemInIndex(SaleItemBase* usi);

	void GetStorageInfo(ItemID item_id, ARG_OUT int* storage_num, ARG_OUT int* low_price, ARG_OUT int* high_price);
	int GetMerchandiseNum(int search_type, bool is_announce);
private:
	SaleItemIndex MakeSaleItemIndex(unsigned short item_id, int price, unsigned int put_on_shelve_time, int uid, short item_order_type, long long unique_key);
	SaleTypeIndex MakeSaleTypeIndex(unsigned short item_id, int price, unsigned int put_on_shelve_time, int uid, short item_order_type, long long unique_key);
	SalePetIndex MakeSalePetIndex(ItemID item_id, int price, unsigned int put_on_shelve_time, int uid, short str_level, long long unique_key);
	SalePetQualityIndex MakeSalePetQualityIndex(short pet_quality, int price, unsigned int put_on_shelve_time, int uid, short str_level, long long unique_key);
	SaleMedalIndex MakeSaleMedalIndex(short medal_grade, short medal_color, unsigned short item_id, int price, unsigned int put_on_shelve_time, int uid, short item_order_type, long long unique_key);

	ItemIndexMap* GetItemIndexMap(ItemID item_id, bool is_announcing, short item_order_type);
	TypeIndexMap* GetTypeIndexMap(short item_search_type, bool is_announcing, short item_order_type);
	TypeIndexMap* GetBigTypeIndexMap(short item_search_big_type, bool is_announcing, short item_order_type);
	TypeIndexMap* GetLevelTypeIndexMap(short level, short item_search_type, bool is_announcing, short item_order_type);
	
	PetPriceUpIndexMap* GetPetPriceUpIndexMap(ItemID item_id);
	PetPriceDownIndexMap* GetPetPriceDownIndexMap(ItemID item_id);
	PetStrLevelUpIndexMap* GetPetStrLevelUpIndexMap(ItemID item_id);
	PetStrLevelDownIndexMap* GetPetStrLevelDownIndexMap(ItemID item_id);

	PetQualityPriceUpIndexMap* GetPetQualityPriceUpIndexMap(short quality);
	PetQualityPriceDownIndexMap* GetPetQualityPriceDownIndexMap(short quality);
	PetQualityStrLevelUpIndexMap* GetPetQualityStrLevelUpIndexMap(short quality);
	PetQualityStrLevelDownIndexMap* GetPetQualityStrLevelDownIndexMap(short quality);

	MedalGradeDownIndexMap* GetMedalGradeDownIndexMap(short color, bool is_announcing, short item_order_type);
	MedalGradeDownIndexMap* GetMedalSpecialEffectTypeIndexMap(int sp_type);
	MedalGradeDownIndexMap* GetMedalSpeTypeColorIndexMap(int sp_type, int color, bool is_announcing, short item_order_type);
	MedalGradeDownIndexMap* GetMedalProfColorIndexMap(int prof, int color, bool is_announcing, short item_order_type);

	bool OnAddItemToIndex(SaleItemBase* usi, bool is_announce);
	bool OnRemoveItemFromIndex(SaleItemBase* usi, bool is_announce);
	
	bool OnSearchPetForQuickBuyByPriceUp(const SalePetIndex& sale_pet_index, int req_item_per_page, SaleItemBase** out_list, int* out_count);
	bool OnSearchPetForQuickBuyByPriceDown(const SalePetIndex& sale_pet_index, int req_item_per_page, SaleItemBase** out_list, int* out_count);
	bool OnSearchPetForQuickBuyByStrLevelUp(const SalePetIndex& sale_pet_index, int req_item_per_page, SaleItemBase** out_list, int* out_count);
	bool OnSearchPetForQuickBuyByStrLevelDown(const SalePetIndex& sale_pet_index, int req_item_per_page, SaleItemBase** out_list, int* out_count);
	bool OnSearchPetQualityForQuickBuyByPriceUp(const SalePetQualityIndex& sale_pet_index, int req_item_per_page, SaleItemBase** out_list, int* out_count);
	bool OnSearchPetQualityForQuickBuyByPriceDown(const SalePetQualityIndex& sale_pet_index, int req_item_per_page, SaleItemBase** out_list, int* out_count);
	bool OnSearchPetQualityForQuickBuyByStrLevelUp(const SalePetQualityIndex& sale_pet_index, int req_item_per_page, SaleItemBase** out_list, int* out_count);
	bool OnSearchPetQualityForQuickBuyByStrLevelDown(const SalePetQualityIndex& sale_pet_index, int req_item_per_page, SaleItemBase** out_list, int* out_count);

	int m_barrier[100];	// 用于检测m_sale_item_index_map_list[0][1]=0x3的问题 (怀疑是数组越界被篡改了数据）

	ItemIndexMap* m_sale_item_index_map_list[MAX_UINT16 + 1][TRADE_ITEM_ORDER_MAX];
	ItemIndexMap* m_announce_item_index_map_list[MAX_UINT16 + 1][TRADE_ITEM_ORDER_MAX];

	TypeIndexMap* m_sale_type_index_map_list[TRADE_ITEM_SEARCH_TYPE_MAX][TRADE_ITEM_ORDER_MAX];
	TypeIndexMap* m_announce_type_index_map_list[TRADE_ITEM_SEARCH_TYPE_MAX][TRADE_ITEM_ORDER_MAX];

	TypeIndexMap* m_sale_big_type_index_map_list[TRADE_ITEM_SEARCH_BIG_TYPE_MAX][TRADE_ITEM_ORDER_MAX];
	TypeIndexMap* m_announce_big_type_index_map_list[TRADE_ITEM_SEARCH_BIG_TYPE_MAX][TRADE_ITEM_ORDER_MAX];

	TypeIndexMap* m_sale_type_level_index_map_list[TRADE_ITEM_LEVEL_SEARCH_TYPE_NUM][TRADE_ITEM_LEVEL_MAX + 1][TRADE_ITEM_ORDER_MAX];
	TypeIndexMap* m_announce_type_level_index_map_list[TRADE_ITEM_LEVEL_SEARCH_TYPE_NUM][TRADE_ITEM_LEVEL_MAX + 1][TRADE_ITEM_ORDER_MAX];

	PetPriceUpIndexMap* m_pet_price_up_index_map[TRADE_PET_ITEM_ID_END - TRADE_PET_ITEM_ID_BEGIN];
	PetPriceDownIndexMap* m_pet_price_down_index_map[TRADE_PET_ITEM_ID_END - TRADE_PET_ITEM_ID_BEGIN];
	PetStrLevelUpIndexMap* m_pet_str_level_up_index_map[TRADE_PET_ITEM_ID_END - TRADE_PET_ITEM_ID_BEGIN];
	PetStrLevelDownIndexMap* m_pet_str_level_down_index_map[TRADE_PET_ITEM_ID_END - TRADE_PET_ITEM_ID_BEGIN];

	PetQualityPriceUpIndexMap* m_pet_quality_price_up_index_map[PET_QUALITY_MAX];
	PetQualityPriceDownIndexMap* m_pet_quality_price_down_index_map[PET_QUALITY_MAX];
	PetQualityStrLevelUpIndexMap* m_pet_quality_str_level_up_index_map[PET_QUALITY_MAX];
	PetQualityStrLevelDownIndexMap* m_pet_quality_str_level_down_index_map[PET_QUALITY_MAX];

	MedalGradeDownIndexMap* m_sale_medal_grade_index_map_list[ItemBase::I_COLOR_MAX][TRADE_ITEM_ORDER_MAX];
	MedalGradeDownIndexMap* m_announce_medal_grade_index_map_list[ItemBase::I_COLOR_MAX][TRADE_ITEM_ORDER_MAX];

	MedalGradeDownIndexMap* m_sale_medal_special_effect_index_map_list[BATTLE_SPECIAL_EFFECT_TYPE_NUM];
	
	MedalGradeDownIndexMap* m_sale_medal_spe_color_index_map_list[ItemBase::I_COLOR_MAX][BATTLE_SPECIAL_EFFECT_TYPE_NUM][TRADE_ITEM_ORDER_MAX];
	MedalGradeDownIndexMap* m_announce_medal_spe_color_index_map_list[ItemBase::I_COLOR_MAX][BATTLE_SPECIAL_EFFECT_TYPE_NUM][TRADE_ITEM_ORDER_MAX];

	MedalGradeDownIndexMap* m_sale_medal_prof_color_index_map_list[PROFESSION_TYPE_NUM][ItemBase::I_COLOR_MAX][TRADE_ITEM_ORDER_MAX];
	MedalGradeDownIndexMap* m_announce_medal_prof_color_index_map_list[PROFESSION_TYPE_NUM][ItemBase::I_COLOR_MAX][TRADE_ITEM_ORDER_MAX];
	UNSTD_STATIC_CHECK(9 == PROFESSION_TYPE_NUM);
};

#endif