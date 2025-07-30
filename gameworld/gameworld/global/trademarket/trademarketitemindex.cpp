#include "trademarketitemindex.hpp"
#include "gamelog.h"
#include "global/trademarket/trademarketconfig.hpp"
#include "item/itempool.h"
#include "item/itembase.h"
#include "equipment/equipment.h"
#include "equipment/jewelry.h"
#include "other/pet/petconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/medal/medalconfig.hpp"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"

TradeMarketItemIndex::TradeMarketItemIndex()
{
	memset(m_barrier, 0, sizeof(m_barrier));

	memset(m_sale_item_index_map_list, 0, sizeof(m_sale_item_index_map_list));
	memset(m_announce_item_index_map_list, 0, sizeof(m_announce_item_index_map_list));
	memset(m_sale_type_index_map_list, 0, sizeof(m_sale_type_index_map_list));
	memset(m_announce_type_index_map_list, 0, sizeof(m_announce_type_index_map_list));
	memset(m_sale_big_type_index_map_list, 0, sizeof(m_sale_big_type_index_map_list));
	memset(m_announce_big_type_index_map_list, 0, sizeof(m_announce_big_type_index_map_list));
	memset(m_sale_type_level_index_map_list, 0, sizeof(m_sale_type_level_index_map_list));
	memset(m_announce_type_level_index_map_list, 0, sizeof(m_announce_type_level_index_map_list));

	memset(m_pet_price_up_index_map, 0, sizeof(m_pet_price_up_index_map));
	memset(m_pet_price_down_index_map, 0, sizeof(m_pet_price_down_index_map));
	memset(m_pet_str_level_up_index_map, 0, sizeof(m_pet_str_level_up_index_map));
	memset(m_pet_str_level_down_index_map, 0, sizeof(m_pet_str_level_down_index_map));

	memset(m_pet_quality_price_up_index_map, 0, sizeof(m_pet_quality_price_up_index_map));
	memset(m_pet_quality_price_down_index_map, 0, sizeof(m_pet_quality_price_down_index_map));
	memset(m_pet_quality_str_level_up_index_map, 0, sizeof(m_pet_quality_str_level_up_index_map));
	memset(m_pet_quality_str_level_down_index_map, 0, sizeof(m_pet_quality_str_level_down_index_map));

	memset(m_sale_medal_grade_index_map_list, 0, sizeof(m_sale_medal_grade_index_map_list));
	memset(m_announce_medal_grade_index_map_list, 0, sizeof(m_announce_medal_grade_index_map_list));

	memset(m_sale_medal_special_effect_index_map_list, 0, sizeof(m_sale_medal_special_effect_index_map_list));
	memset(m_sale_medal_spe_color_index_map_list, 0, sizeof(m_sale_medal_spe_color_index_map_list));
	memset(m_announce_medal_spe_color_index_map_list, 0, sizeof(m_announce_medal_spe_color_index_map_list));

	memset(m_sale_medal_prof_color_index_map_list, 0, sizeof(m_sale_medal_prof_color_index_map_list));
	memset(m_announce_medal_prof_color_index_map_list, 0, sizeof(m_announce_medal_prof_color_index_map_list));
}

TradeMarketItemIndex::~TradeMarketItemIndex()
{
	for (int i = 0; i < MAX_UINT16 + 1; ++i)
	{
		for (int k = 0; k < TRADE_ITEM_ORDER_MAX; ++k)
		{
			if (NULL != m_sale_item_index_map_list[i][k])
			{
				delete m_sale_item_index_map_list[i][k];
				m_sale_item_index_map_list[i][k] = NULL;
			}

			if (NULL != m_announce_item_index_map_list[i][k])
			{
				delete m_announce_item_index_map_list[i][k];
				m_announce_item_index_map_list[i][k] = NULL;
			}
		}	
	}

	for (int i = 0; i < TRADE_ITEM_SEARCH_TYPE_MAX; ++i)
	{
		for (int k = 0; k < TRADE_ITEM_ORDER_MAX; ++k)
		{
			if (NULL != m_sale_type_index_map_list[i][k])
			{
				delete m_sale_type_index_map_list[i][k];
				m_sale_type_index_map_list[i][k] = NULL;
			}

			if (NULL != m_announce_type_index_map_list[i][k])
			{
				delete m_announce_type_index_map_list[i][k];
				m_announce_type_index_map_list[i][k] = NULL;
			}
		}	
	}

	for (int i = 0; i < TRADE_ITEM_SEARCH_BIG_TYPE_MAX; ++i)
	{
		for (int k = 0; k < TRADE_ITEM_ORDER_MAX; ++k)
		{
			if (NULL != m_sale_big_type_index_map_list[i][k])
			{
				delete m_sale_big_type_index_map_list[i][k];
				m_sale_big_type_index_map_list[i][k] = NULL;
			}

			if (NULL != m_announce_big_type_index_map_list[i][k])
			{
				delete m_announce_big_type_index_map_list[i][k];
				m_announce_big_type_index_map_list[i][k] = NULL;
			}
		}
	}

	UNSTD_STATIC_CHECK(TRADE_ITEM_LEVEL_MAX >= ItemNamespace::EQUIPMENT_MAX_LEVEL);
	for (int i = 0; i < TRADE_ITEM_LEVEL_SEARCH_TYPE_NUM; ++i)
	{
		for (int j = 0; j < TRADE_ITEM_LEVEL_MAX + 1; ++j)
		{
			for (int k = 0; k < TRADE_ITEM_ORDER_MAX; ++k)
			{
				if (NULL != m_sale_type_level_index_map_list[i][j][k])
				{
					delete m_sale_type_level_index_map_list[i][j][k];
					m_sale_type_level_index_map_list[i][j][k] = NULL;
				}

				if (NULL != m_announce_type_level_index_map_list[i][j][k])
				{
					delete m_announce_type_level_index_map_list[i][j][k];
					m_announce_type_level_index_map_list[i][j][k] = NULL;
				}
			}
		}	
	}

	for (int i = 0; i < TRADE_PET_ITEM_ID_END - TRADE_PET_ITEM_ID_BEGIN; ++i)
	{
		if (m_pet_price_up_index_map[i] != NULL)
		{
			delete m_pet_price_up_index_map[i];
			m_pet_price_up_index_map[i] = NULL;
		}

		if (m_pet_price_down_index_map[i] != NULL)
		{
			delete m_pet_price_down_index_map[i];
			m_pet_price_down_index_map[i] = NULL;
		}

		if (m_pet_str_level_up_index_map[i] != NULL)
		{
			delete m_pet_str_level_up_index_map[i];
			m_pet_str_level_up_index_map[i] = NULL;
		}

		if (m_pet_str_level_down_index_map[i] != NULL)
		{
			delete m_pet_str_level_down_index_map[i];
			m_pet_str_level_down_index_map[i] = NULL;
		}
	}

	for (int i = PET_QUALITY_INVALID + 1; i < PET_QUALITY_MAX; ++i)
	{
		if (m_pet_quality_price_up_index_map[i] != NULL)
		{
			delete m_pet_quality_price_up_index_map[i];
			m_pet_quality_price_up_index_map[i] = NULL;
		}

		if (m_pet_quality_price_down_index_map[i] != NULL)
		{
			delete m_pet_quality_price_down_index_map[i];
			m_pet_quality_price_down_index_map[i] = NULL;
		}

		if (m_pet_quality_str_level_up_index_map[i] != NULL)
		{
			delete m_pet_quality_str_level_up_index_map[i];
			m_pet_quality_str_level_up_index_map[i] = NULL;
		}

		if (m_pet_quality_str_level_down_index_map[i] != NULL)
		{
			delete m_pet_quality_str_level_down_index_map[i];
			m_pet_quality_str_level_down_index_map[i] = NULL;
		}
	}

	for (int i = ItemBase::I_COLOR_INVALID + 1; i < ItemBase::I_COLOR_MAX; ++i)
	{
		for (int k = 0; k < TRADE_ITEM_ORDER_MAX; ++k)
		{
			if (m_sale_medal_grade_index_map_list[i][k] != NULL)
			{
				delete m_sale_medal_grade_index_map_list[i][k];
				m_sale_medal_grade_index_map_list[i][k] = NULL;
			}

			if (m_announce_medal_grade_index_map_list[i][k] != NULL)
			{
				delete m_announce_medal_grade_index_map_list[i][k];
				m_announce_medal_grade_index_map_list[i][k] = NULL;
			}
		}
	}	

	for (int i = 0; i < BATTLE_SPECIAL_EFFECT_TYPE_NUM; ++i)
	{
		if (NULL != m_sale_medal_special_effect_index_map_list[i])
		{
			delete m_sale_medal_special_effect_index_map_list[i];
			m_sale_medal_special_effect_index_map_list[i] = NULL;
		}
	}

	for (int color = 0; color < ItemBase::I_COLOR_MAX; ++color)
	{
		for (int type = 0; type < BATTLE_SPECIAL_EFFECT_TYPE_NUM; ++type)
		{
			for (int k = 0; k < TRADE_ITEM_ORDER_MAX; ++k)
			{
				if (NULL != m_sale_medal_spe_color_index_map_list[color][type][k])
				{
					delete m_sale_medal_spe_color_index_map_list[color][type][k];
					m_sale_medal_spe_color_index_map_list[color][type][k] = NULL;
				}

				if (NULL != m_announce_medal_spe_color_index_map_list[color][type][k])
				{
					delete m_announce_medal_spe_color_index_map_list[color][type][k];
					m_announce_medal_spe_color_index_map_list[color][type][k] = NULL;
				}
			}	
		}
	}

	for (int prof = 0; prof < PROFESSION_TYPE_NUM; ++prof)
	{
		for (int color = 0; color < ItemBase::I_COLOR_MAX; ++color)
		{
			for (int k = 0; k < TRADE_ITEM_ORDER_MAX; ++k)
			{
				if (NULL != m_sale_medal_prof_color_index_map_list[prof][color][k])
				{
					delete m_sale_medal_prof_color_index_map_list[prof][color][k];
					m_sale_medal_prof_color_index_map_list[prof][color][k] = NULL;
				}

				if (NULL != m_announce_medal_prof_color_index_map_list[prof][color][k])
				{
					delete m_announce_medal_prof_color_index_map_list[prof][color][k];
					m_announce_medal_prof_color_index_map_list[prof][color][k] = NULL;
				}
			}
		}
	}
}

void TradeMarketItemIndex::Update(unsigned long interval, time_t now_second)
{
#ifdef _DEBUG
	// 只在内网检测
	for (int i = 0; i < ARRAY_LENGTH(m_barrier); ++i)
	{
		if (m_barrier[i] != 0)
		{
			assert(0);
		}
	}
#endif
}

bool TradeMarketItemIndex::OnSearchByItem(bool is_search_announce, short order_type, ItemID item_id, int req_page, int req_item_per_page,
	SaleItemBase** out_list, int* out_count, int* out_cur_page, int* out_total_page)
{
	if (order_type < 0 || order_type >= TRADE_ITEM_ORDER_MAX) return false;
	if (NULL == out_list || NULL == out_count || NULL == out_cur_page || NULL == out_total_page) return false;
	
	const ItemBase* item_base = ITEMPOOL->GetItem(item_id);
	if (NULL == item_base) return false;

	ItemID index_item_id = item_id;
	if (I_TYPE_PET == item_base->GetItemType())
	{
		const PetCfg* pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(index_item_id);
		if (NULL != pet_cfg && pet_cfg->pet_mutant_times)
		{
			index_item_id = pet_cfg->prototype_pet_id; // 转变为普通宠物的pet_id
		}
	}

	ItemIndexMap* index_map = this->GetItemIndexMap(index_item_id, is_search_announce, order_type);
	if (NULL == index_map) return false;

	int total_item_count = static_cast<int>(index_map->size());
	int item_offset = req_page * req_item_per_page;
	if (item_offset >= total_item_count)
	{
		return false;
	}

	ItemIndexMap::iterator begin_it = index_map->begin();
	std::advance(begin_it, item_offset);

	*out_count = 0;
	for (ItemIndexMap::iterator it = begin_it; it != index_map->end() && (*out_count) < req_item_per_page; ++it)
	{
		if (it->second->GetSaleItemParam().unique_key != it->first.unique_key)
		{
#ifdef _DEBUG
			assert(0);
#else
			continue;
#endif
		}

		out_list[(*out_count)++] = it->second;
	}
	*out_cur_page = req_page;
	*out_total_page = total_item_count / req_item_per_page;
	if (total_item_count % req_item_per_page != 0)
	{
		*out_total_page += 1;
	}

	return true;
}

bool TradeMarketItemIndex::OnSearchByType(bool is_search_announce, short order_type, short item_search_type, int req_page, int req_item_per_page, SaleItemBase** out_list, int* out_count, int* out_cur_page, int* out_total_page)
{
	if (order_type < 0 || order_type >= TRADE_ITEM_ORDER_MAX) return false;
	if (NULL == out_list || NULL == out_count || NULL == out_cur_page || NULL == out_total_page) return false;

	TypeIndexMap* index_map = this->GetTypeIndexMap(item_search_type, is_search_announce, order_type);
	if (NULL == index_map) return false;

	int total_item_count = static_cast<int>(index_map->size());
	int item_offset = req_page * req_item_per_page;
	if (item_offset >= total_item_count)
	{
		return false;
	}

	TypeIndexMap::iterator begin_it = index_map->begin();
	std::advance(begin_it, item_offset);

	*out_count = 0;
	for (TypeIndexMap::iterator it = begin_it; it != index_map->end() && (*out_count) < req_item_per_page; ++it)
	{
		if (it->second->GetSaleItemParam().unique_key != it->first.unique_key)
		{
#ifdef _DEBUG
			assert(0);
#else
			continue;
#endif
		}

		out_list[(*out_count)++] = it->second;
	}
	*out_cur_page = req_page;
	*out_total_page = total_item_count / req_item_per_page;
	if (total_item_count % req_item_per_page != 0)
	{
		*out_total_page += 1;
	}

	return true;
}

bool TradeMarketItemIndex::OnSearchByBigType(bool is_search_announce, short order_type, short item_search_big_type, int req_page, int req_item_per_page, SaleItemBase** out_list, int* out_count, int* out_cur_page, int* out_total_page)
{
	if (order_type < 0 || order_type >= TRADE_ITEM_ORDER_MAX) return false;
	if (NULL == out_list || NULL == out_count || NULL == out_cur_page || NULL == out_total_page) return false;

	TypeIndexMap* index_map = this->GetBigTypeIndexMap(item_search_big_type, is_search_announce, order_type);
	if (NULL == index_map) return false;

	int total_item_count = static_cast<int>(index_map->size());
	int item_offset = req_page * req_item_per_page;
	if (item_offset >= total_item_count)
	{
		return false;
	}

	TypeIndexMap::iterator begin_it = index_map->begin();
	std::advance(begin_it, item_offset);

	*out_count = 0;
	for (TypeIndexMap::iterator it = begin_it; it != index_map->end() && (*out_count) < req_item_per_page; ++it)
	{
		if (it->second->GetSaleItemParam().unique_key != it->first.unique_key)
		{
#ifdef _DEBUG
			assert(0);
#else
			continue;
#endif
		}

		out_list[(*out_count)++] = it->second;
	}
	*out_cur_page = req_page;
	*out_total_page = total_item_count / req_item_per_page;
	if (total_item_count % req_item_per_page != 0)
	{
		*out_total_page += 1;
	}

	return true;
}

bool TradeMarketItemIndex::OnSearchByTypeWithLevel(bool is_search_announce, short order_type, short level, short item_search_type, int req_page, int req_item_per_page, SaleItemBase** out_list, int* out_count, int* out_cur_page, int* out_total_page)
{
	if (order_type < 0 || order_type >= TRADE_ITEM_ORDER_MAX) return false;
	if (NULL == out_list || NULL == out_count || NULL == out_cur_page || NULL == out_total_page) return false;
	if (item_search_type < 0 || item_search_type >= TRADE_ITEM_LEVEL_SEARCH_TYPE_NUM) return false;
	if (level < 0 || level > TRADE_ITEM_LEVEL_MAX) return false;

	TypeIndexMap* index_map = this->GetLevelTypeIndexMap(level, item_search_type, is_search_announce, order_type);
	if (NULL == index_map) return false;

	int total_item_count = static_cast<int>(index_map->size());
	int item_offset = req_page * req_item_per_page;
	if (item_offset >= total_item_count)
	{
		return false;
	}

	TypeIndexMap::iterator begin_it = index_map->begin();
	std::advance(begin_it, item_offset);

	*out_count = 0;
	for (TypeIndexMap::iterator it = begin_it; it != index_map->end() && (*out_count) < req_item_per_page; ++it)
	{
		if (it->second->GetSaleItemParam().unique_key != it->first.unique_key)
		{
#ifdef _DEBUG
			assert(0);
#else
			continue;
#endif
		}

		out_list[(*out_count)++] = it->second;
	}
	*out_cur_page = req_page;
	*out_total_page = total_item_count / req_item_per_page;
	if (total_item_count % req_item_per_page != 0)
	{
		*out_total_page += 1;
	}

	return true;
}

bool TradeMarketItemIndex::OnSearchForAutoBuy(ItemID item_id, int need_num, int expect_price, std::vector<SaleItemBase*>* out_list)
{
	if (NULL == out_list) return false;

	const ItemBase* item_base = ITEMPOOL->GetItem(item_id);
	if (NULL == item_base) return false;

	ItemID index_item_id = item_id;
	if (I_TYPE_PET == item_base->GetItemType())
	{
		const PetCfg* pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(index_item_id);
		if (NULL != pet_cfg && pet_cfg->pet_mutant_times)
		{
			index_item_id = pet_cfg->prototype_pet_id; // 转变为普通宠物的pet_id
		}
	}

	ItemIndexMap* index_map = this->GetItemIndexMap(index_item_id, false, TRADE_ITEM_ORDER_PRICE_UP);
	if (NULL == index_map) return false;

	int total_num = 0;
	for (ItemIndexMap::iterator it = index_map->begin(); it != index_map->end(); ++it)
	{
		if (it->second->GetSaleItemParam().unique_key != it->first.unique_key)
		{
#ifdef _DEBUG
			assert(0);
#else
			continue;
#endif
		}

		const SaleItemParam& param = it->second->GetSaleItemParamConst();

		total_num += param.item_wrapper.num;
		out_list->push_back(it->second);

		if (total_num >= need_num) // 已有足够数量则返回
		{
			break;
		}

		int price = param.gold_price > 0 ? param.gold_price : param.immortal_coin_price;
		if (price > expect_price) // 价格已超过预期，返回
		{
			break;
		}
	}

	return true;
}

bool TradeMarketItemIndex::OnSearchByAdvertise(bool is_search_announce, ItemID item_id, int price, unsigned int put_on_shelve_time, int seller_uid, long long unique_key, int req_item_per_page, SaleItemBase** out_list, int* out_count, int* out_cur_page, int* out_total_page, short* out_cursor_index)
{
	if (NULL == out_list || NULL == out_count || NULL == out_cur_page || NULL == out_total_page || NULL == out_cursor_index) return false;
	if (req_item_per_page <= 0) return false;

	const ItemBase* item_base = ITEMPOOL->GetItem(item_id);
	if (NULL == item_base) return false;

	ItemID index_item_id = item_id;
	if (I_TYPE_PET == item_base->GetItemType())
	{
		const PetCfg* pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(index_item_id);
		if (NULL != pet_cfg && pet_cfg->pet_mutant_times)
		{
			index_item_id = pet_cfg->prototype_pet_id; // 转变为普通宠物的pet_id
		}
	}
	
	ItemIndexMap* index_map = this->GetItemIndexMap(index_item_id, is_search_announce, TRADE_ITEM_ORDER_PRICE_UP);
	if (NULL == index_map) return false;

	// 找到目标物品
	SaleItemIndex advertise_item_index = this->MakeSaleItemIndex(index_item_id, price, put_on_shelve_time, seller_uid, TRADE_ITEM_ORDER_PRICE_UP, unique_key);
	ItemIndexMap::iterator advertise_it = index_map->find(advertise_item_index);
	if (advertise_it == index_map->end()) return false;

	int total_item_count = index_map->size();
	*out_count = 0;
	ItemIndexMap::iterator it = index_map->begin();
	ItemIndexMap::iterator page_first_item_it = it;
	for (int i = 0; it != index_map->end() && (*out_count) < req_item_per_page; ++it, ++i)
	{
		if (i % req_item_per_page == 0)
		{
			page_first_item_it = it;
		}

		if (it == advertise_it)
		{
			// 找到了广告上的商品,则把该页的数据填充好
			ItemIndexMap::iterator tmp_it = page_first_item_it;
			for (; tmp_it != advertise_it; ++tmp_it)
			{
				if (tmp_it->second->GetSaleItemParam().unique_key != tmp_it->first.unique_key)
				{
#ifdef _DEBUG
					assert(0);
#else
					continue;
#endif
				}

				if ((*out_count) < req_item_per_page)
				{
					out_list[(*out_count)++] = tmp_it->second;
				}	
			}

			if ((*out_count) < req_item_per_page)
			{
				*out_cursor_index = (*out_count); // cursor指向广告上的商品
				out_list[(*out_count)++] = advertise_it->second;
			}

			*out_cur_page = i / req_item_per_page;
			continue;
		}

		//若已找到广告上的商品，且当前页还有位置，则继续加商品到当前页
		if ((*out_count) > 0)
		{
			if (it->second->GetSaleItemParam().unique_key != it->first.unique_key)
			{
#ifdef _DEBUG
				assert(0);
#else
				continue;
#endif
			}

			out_list[(*out_count)++] = it->second;
		}
	}
	*out_total_page = total_item_count / req_item_per_page;
	if (total_item_count % req_item_per_page != 0)
	{
		*out_total_page += 1;
	}

	return true;
}

bool TradeMarketItemIndex::OnSearchForSellingList(ItemID item_id, int max_num, SaleItemBase** out_list, int* out_num)
{
	if (NULL == out_list || NULL == out_num)
	{
		return false;
	}

	const ItemBase* item_base = ITEMPOOL->GetItem(item_id);
	if (NULL == item_base) return false;

	ItemID index_item_id = item_id;
	if (I_TYPE_PET == item_base->GetItemType())
	{
		const PetCfg* pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(index_item_id);
		if (NULL != pet_cfg && pet_cfg->pet_mutant_times)
		{
			index_item_id = pet_cfg->prototype_pet_id; // 转变为普通宠物的pet_id
		}
	}

	ItemIndexMap* index_map = this->GetItemIndexMap(index_item_id, false, TRADE_ITEM_ORDER_PRICE_UP);
	if (NULL == index_map) return false;

	for (ItemIndexMap::iterator it = index_map->begin(); it != index_map->end() && (*out_num) < max_num ; ++it)
	{
		if (it->second->GetSaleItemParam().unique_key != it->first.unique_key)
		{
#ifdef _DEBUG
			assert(0);
#else
			continue;
#endif
		}

		out_list[(*out_num)++] = it->second;
	}

	return true;
}

bool TradeMarketItemIndex::OnSearchForMedalSellingList(short medal_effect_type, int max_num, SaleItemBase** out_list, int* out_num)
{
	if (NULL == out_list || NULL == out_num)
	{
		return false;
	}

	MedalGradeDownIndexMap* index_map = this->GetMedalSpecialEffectTypeIndexMap(medal_effect_type);
	if (NULL == index_map) return false;

	for (MedalGradeDownIndexMap::iterator it = index_map->begin(); it != index_map->end() && (*out_num) < max_num; ++it)
	{
		if (it->second->GetSaleItemParam().unique_key != it->first.unique_key)
		{
#ifdef _DEBUG
			assert(0);
#else
			continue;
#endif
		}

		out_list[(*out_num)++] = it->second;
	}

	return true;
}

bool TradeMarketItemIndex::OnSearchPetForQuickBuy(short order_type, const SalePetIndex& pet_index, int req_item_per_page, SaleItemBase** out_list, int* out_count)
{
	if (order_type < 0 || order_type >= TRADE_PET_INDEX_ORDER_MAX) return false;
	if (NULL == out_list || NULL == out_count) return false;

	switch (order_type)
	{
	case TRADE_PET_PRICE_UP:
		{
			return this->OnSearchPetForQuickBuyByPriceUp(pet_index, req_item_per_page, out_list, out_count);
		}
		break;

	case TRADE_PET_PRICE_DOWN:
		{
			return this->OnSearchPetForQuickBuyByPriceDown(pet_index, req_item_per_page, out_list, out_count);
		}
		break;

	case TRADE_PET_STR_LEVEL_ORDER_UP:
		{
			return this->OnSearchPetForQuickBuyByStrLevelUp(pet_index, req_item_per_page, out_list, out_count);
		}
		break;

	case TRADE_PET_STR_LEVEL_ORDER_DOWN:
		{
			return this->OnSearchPetForQuickBuyByStrLevelDown(pet_index, req_item_per_page, out_list, out_count);
		}
		break;

	}

	return false;
}

bool TradeMarketItemIndex::OnSearchPetQualityForQuickBuy(short order_type, const SalePetQualityIndex& pet_index, int req_item_per_page, SaleItemBase** out_list, int* out_count)
{
	if (order_type < 0 || order_type >= TRADE_PET_INDEX_ORDER_MAX) return false;
	if (NULL == out_list || NULL == out_count) return false;

	switch (order_type)
	{
	case TRADE_PET_PRICE_UP:
		{
			return this->OnSearchPetQualityForQuickBuyByPriceUp(pet_index, req_item_per_page, out_list, out_count);
		}
		break;

	case TRADE_PET_PRICE_DOWN:
		{
			return this->OnSearchPetQualityForQuickBuyByPriceDown(pet_index, req_item_per_page, out_list, out_count);
		}
		break;

	case TRADE_PET_STR_LEVEL_ORDER_UP:
		{
			return this->OnSearchPetQualityForQuickBuyByStrLevelUp(pet_index, req_item_per_page, out_list, out_count);
		}
		break;

	case TRADE_PET_STR_LEVEL_ORDER_DOWN:
		{
			return this->OnSearchPetQualityForQuickBuyByStrLevelDown(pet_index, req_item_per_page, out_list, out_count);
		}
		break;

	}

	return false;
}

bool TradeMarketItemIndex::OnSearchByMedalColor(bool is_search_announce, short order_type, short medal_color, int req_page, int req_item_per_page, SaleItemBase** out_list, int* out_count, int* out_cur_page, int* out_total_page)
{
	if (order_type < 0 || order_type >= TRADE_ITEM_ORDER_MAX) return false;
	if (NULL == out_list || NULL == out_count || NULL == out_cur_page || NULL == out_total_page) return false;

	MedalGradeDownIndexMap* index_map = this->GetMedalGradeDownIndexMap(medal_color, is_search_announce, order_type);
	if (NULL == index_map) return false;

	int total_item_count = static_cast<int>(index_map->size());
	int item_offset = req_page * req_item_per_page;
	if (item_offset >= total_item_count)
	{
		return false;
	}

	MedalGradeDownIndexMap::iterator begin_it = index_map->begin();
	std::advance(begin_it, item_offset);

	*out_count = 0;
	for (MedalGradeDownIndexMap::iterator it = begin_it; it != index_map->end() && (*out_count) < req_item_per_page; ++it)
	{
		if (it->second->GetSaleItemParam().unique_key != it->first.unique_key)
		{
#ifdef _DEBUG
			assert(0);
#else
			continue;
#endif
		}

		out_list[(*out_count)++] = it->second;
	}
	*out_cur_page = req_page;
	*out_total_page = total_item_count / req_item_per_page;
	if (total_item_count % req_item_per_page != 0)
	{
		*out_total_page += 1;
	}

	return true;
}

bool TradeMarketItemIndex::OnSearchByMedalEffectTypeAndColor(bool is_search_announce, short order_type, short sp_type, short medal_color, int req_page, int req_item_per_page, SaleItemBase** out_list, int* out_count, int* out_cur_page, int* out_total_page)
{
	if (order_type < 0 || order_type >= TRADE_ITEM_ORDER_MAX) return false;
	if (NULL == out_list || NULL == out_count || NULL == out_cur_page || NULL == out_total_page) return false;

	MedalGradeDownIndexMap* index_map = this->GetMedalSpeTypeColorIndexMap(sp_type, medal_color, is_search_announce, order_type);
	if (NULL == index_map) return false;

	int total_item_count = static_cast<int>(index_map->size());
	int item_offset = req_page * req_item_per_page;
	if (item_offset >= total_item_count)
	{
		return false;
	}

	MedalGradeDownIndexMap::iterator begin_it = index_map->begin();
	std::advance(begin_it, item_offset);

	*out_count = 0;
	for (MedalGradeDownIndexMap::iterator it = begin_it; it != index_map->end() && (*out_count) < req_item_per_page; ++it)
	{
		if (it->second->GetSaleItemParam().unique_key != it->first.unique_key)
		{
#ifdef _DEBUG
			assert(0);
#else
			continue;
#endif
		}

		out_list[(*out_count)++] = it->second;
	}
	*out_cur_page = req_page;
	*out_total_page = total_item_count / req_item_per_page;
	if (total_item_count % req_item_per_page != 0)
	{
		*out_total_page += 1;
	}

	return true;
}

bool TradeMarketItemIndex::OnSearchByMedalColorAndProf(bool is_search_announce, short order_type, short medal_color, int medal_prof, int req_page, int req_item_per_page, SaleItemBase** out_list, int* out_count, int* out_cur_page, int* out_total_page)
{
	if (order_type < 0 || order_type >= TRADE_ITEM_ORDER_MAX) return false;
	if (NULL == out_list || NULL == out_count || NULL == out_cur_page || NULL == out_total_page) return false;

	MedalGradeDownIndexMap* index_map = this->GetMedalProfColorIndexMap(medal_prof, medal_color, is_search_announce, order_type);
	if (NULL == index_map) return false;

	int total_item_count = static_cast<int>(index_map->size());
	int item_offset = req_page * req_item_per_page;
	if (item_offset >= total_item_count)
	{
		return false;
	}

	MedalGradeDownIndexMap::iterator begin_it = index_map->begin();
	std::advance(begin_it, item_offset);

	*out_count = 0;
	for (MedalGradeDownIndexMap::iterator it = begin_it; it != index_map->end() && (*out_count) < req_item_per_page; ++it)
	{
		if (it->second->GetSaleItemParam().unique_key != it->first.unique_key)
		{
#ifdef _DEBUG
			assert(0);
#else
			continue;
#endif
		}

		out_list[(*out_count)++] = it->second;
	}
	*out_cur_page = req_page;
	*out_total_page = total_item_count / req_item_per_page;
	if (total_item_count % req_item_per_page != 0)
	{
		*out_total_page += 1;
	}

	return true;
}

bool TradeMarketItemIndex::OnAddItemOnSale(SaleItemBase* usi)
{
	return this->OnAddItemToIndex(usi, false);
}

bool TradeMarketItemIndex::OnRemoveItemOnSale(SaleItemBase* usi)
{
	return this->OnRemoveItemFromIndex(usi, false);
}

bool TradeMarketItemIndex::OnAddItemOnAnnounce(SaleItemBase* usi)
{
	return this->OnAddItemToIndex(usi, true);
}

bool TradeMarketItemIndex::OnRemoveItemOnAnnounce(SaleItemBase* usi)
{
	return this->OnRemoveItemFromIndex(usi, true);
}

bool TradeMarketItemIndex::IsItemInIndex(SaleItemBase* usi)
{
	if (NULL == usi) return false;

	bool is_equipment_or_jewelry = false;
	bool is_pet = false;
	short level = 0;
	ItemID index_item_id = usi->GetSaleItemParamConst().item_wrapper.item_id;
	const ItemBase* item = ITEMPOOL->GetItem(usi->GetSaleItemParamConst().item_wrapper.item_id);
	if (NULL == item) return false;
	if (I_TYPE_EQUIPMENT == item->GetItemType())
	{
		is_equipment_or_jewelry = true;
		level = ((Equipment*)item)->GetEquipLevel();
	}
	else if (I_TYPE_JEWELRY == item->GetItemType())
	{
		is_equipment_or_jewelry = true;
		level = ((Jewelry*)item)->GetJewelryLevel();
	}
	else if (I_TYPE_PET == item->GetItemType() && item->GetItemId() >= TRADE_PET_ITEM_ID_BEGIN && item->GetItemId() < TRADE_PET_ITEM_ID_END)
	{
		is_pet = true;

		static NetValueItemParam item_param;
		item_param.SetData(usi->GetSaleItemParamConst().item_wrapper.param_data);

		level = item_param.pet_param.str_level;

		const PetCfg* pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(index_item_id);
		if (NULL != pet_cfg && pet_cfg->pet_mutant_times)
		{
			index_item_id = pet_cfg->prototype_pet_id; // 转变为普通宠物的pet_id
		}
	}

	const SaleItemParam& param = usi->GetSaleItemParamConst();
	int price = (param.gold_price > 0 ? param.gold_price : param.immortal_coin_price);

	short item_search_type = item->GetTradeSearchType();
	short item_search_big_type = TRADEMARKET_CFG.GetSearchBigType(item_search_type);

	/************************************** 生成物品index **************************************************/
	SaleItemIndex item_index = this->MakeSaleItemIndex(index_item_id, price,
		param.put_on_shelve_time, usi->GetSellerUid(), TRADE_ITEM_ORDER_PRICE_UP, param.unique_key);

	/************************************** 生成类型index ***************************************************/
	SaleTypeIndex type_index = this->MakeSaleTypeIndex(index_item_id, price,
		param.put_on_shelve_time, usi->GetSellerUid(), TRADE_ITEM_ORDER_PRICE_UP, param.unique_key);

	/************************************** 获取管理器并查找 ******************************************************/
	ItemIndexMap* sale_item_map = this->GetItemIndexMap(index_item_id, false, TRADE_ITEM_ORDER_PRICE_UP);
	if (NULL != sale_item_map && sale_item_map->find(item_index) != sale_item_map->end()) return true;
	
	ItemIndexMap* announce_item_map = this->GetItemIndexMap(index_item_id, true, TRADE_ITEM_ORDER_PRICE_UP);
	if (NULL != announce_item_map && announce_item_map->find(item_index) != announce_item_map->end()) return true;


	TypeIndexMap* type_map = this->GetTypeIndexMap(item_search_type, false, TRADE_ITEM_ORDER_PRICE_UP);
	if (NULL != type_map && type_map->find(type_index) != type_map->end()) return true;

	TypeIndexMap* type_announce_map = this->GetTypeIndexMap(item_search_type, true, TRADE_ITEM_ORDER_PRICE_UP);
	if (NULL != type_announce_map && type_announce_map->find(type_index) != type_announce_map->end()) return true;


	TypeIndexMap* big_type_map = this->GetBigTypeIndexMap(item_search_big_type, false, TRADE_ITEM_ORDER_PRICE_UP);
	if (NULL != big_type_map && big_type_map->find(type_index) != big_type_map->end()) return true;
	
	TypeIndexMap* big_type_announce_map = this->GetBigTypeIndexMap(item_search_big_type, true, TRADE_ITEM_ORDER_PRICE_UP);
	if (NULL != big_type_announce_map && big_type_announce_map->find(type_index) != big_type_announce_map->end()) return true;

	return false;
}

void TradeMarketItemIndex::GetStorageInfo(ItemID item_id, ARG_OUT int* storage_num, ARG_OUT int* low_price, ARG_OUT int* high_price)
{
	if (NULL == storage_num || NULL == low_price || NULL == high_price) return;

	const ItemBase* item_base = ITEMPOOL->GetItem(item_id);
	if (NULL == item_base) return;

	ItemID index_item_id = item_id;
	if (I_TYPE_PET == item_base->GetItemType())
	{
		const PetCfg* pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(index_item_id);
		if (NULL != pet_cfg && pet_cfg->pet_mutant_times)
		{
			index_item_id = pet_cfg->prototype_pet_id; // 转变为普通宠物的pet_id
		}
	}

	ItemIndexMap* index_map = this->GetItemIndexMap(index_item_id, false, TRADE_ITEM_ORDER_PRICE_UP);
	if (NULL == index_map) return;

	ItemIndexMap::iterator it = index_map->begin();
	for (; it != index_map->end(); ++it)
	{
		if (SYSTEM_TRADE_UID == it->second->GetSellerUid()) continue; // 系统的不计入库存

		*storage_num += 1;

		int price = it->second->GetSaleItemParamConst().gold_price;
		if (*low_price > price)
		{
			*low_price = price;
		}

		if (*high_price < price)
		{
			*high_price = price;
		}
	}
}

int TradeMarketItemIndex::GetMerchandiseNum(int search_type, bool is_announce)
{
	if (search_type >= TRADE_MEDAL_SEARCH_TYPE_MIN && search_type <= TRADE_MEDAL_SEARCH_TYPE_MAX)
	{
		short color = search_type - static_cast<short>(TRADE_MEDAL_SEARCH_TYPE_MIN) + ItemBase::I_COLOR_GREEN;
		MedalGradeDownIndexMap* index_map = this->GetMedalGradeDownIndexMap(color, is_announce, TRADE_ITEM_ORDER_PRICE_UP);
		
		if (NULL == index_map) return 0;

		return static_cast<int>(index_map->size());
	}
	else
	{
		TypeIndexMap* index_map = this->GetTypeIndexMap(search_type, is_announce, TRADE_ITEM_ORDER_PRICE_UP);
		if (NULL == index_map) return 0;

		return static_cast<int>(index_map->size());
	}

	
}

bool TradeMarketItemIndex::OnAddItemToIndex(SaleItemBase* usi, bool is_announce)
{
	if (NULL == usi) return false;

	bool is_equipment_or_jewelry = false;
	bool is_pet = false;
	short pet_quality = PET_QUALITY_INVALID;
	short level = 0;
	bool is_medal = false;
	short medal_color = 0;
	short medal_grade = 0;
	int medal_special_effect_type = 0;
	int medal_prof = 0;

	ItemID index_item_id = usi->GetSaleItemParamConst().item_wrapper.item_id;

	const ItemBase* item = ITEMPOOL->GetItem(usi->GetSaleItemParamConst().item_wrapper.item_id);
	if (NULL == item) return false;
	if (I_TYPE_EQUIPMENT == item->GetItemType())
	{
		is_equipment_or_jewelry = true;
		level = ((Equipment*)item)->GetEquipLevel();
	}
	else if (I_TYPE_JEWELRY == item->GetItemType())
	{
		is_equipment_or_jewelry = true;
		level = ((Jewelry*)item)->GetJewelryLevel();
	}
	else if (I_TYPE_PET == item->GetItemType() && item->GetItemId() >= TRADE_PET_ITEM_ID_BEGIN && item->GetItemId() < TRADE_PET_ITEM_ID_END)
	{
		is_pet = true;

		static NetValueItemParam item_param;
		item_param.SetData(usi->GetSaleItemParamConst().item_wrapper.param_data);

		level = item_param.pet_param.str_level;
		
		const PetCfg* pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(index_item_id);
		if (NULL != pet_cfg && pet_cfg->pet_mutant_times)
		{
			index_item_id = pet_cfg->prototype_pet_id; // 转变为普通宠物的pet_id

			gamelog::g_log_trade_market.printf(LL_INFO, "OnAddItemToIndex role[%d,%s] MutantPet item_id change from [%d] to [%d]",
				usi->GetSellerUid(), usi->GetSellerName(), 
				usi->GetSaleItemParamConst().item_wrapper.item_id, index_item_id);
		}

		if (NULL != pet_cfg)
		{
			pet_quality = pet_cfg->quality;
		}
	}
	else if (I_TYPE_MEDAL == item->GetItemType())
	{
		is_medal = true;

		static NetValueItemParam item_param;
		item_param.SetData(usi->GetSaleItemParamConst().item_wrapper.param_data);

		medal_grade = item_param.medal_param.grade;
		const MedalUpgradeCfg* cfg = LOGIC_CONFIG->GetMedalConfig()->GetMedalUpgradeCfg(medal_grade);
		if (NULL != cfg)
		{
			medal_color = cfg->color;
		}

		const SpecialEffectCfg* sp_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(item_param.medal_param.sp_id);
		if (NULL != sp_cfg)
		{
			medal_special_effect_type = sp_cfg->sp_type;
			medal_prof = sp_cfg->prof_type;
		}
	}

	const SaleItemParam& param = usi->GetSaleItemParamConst();
	int price = (param.gold_price > 0 ? param.gold_price : param.immortal_coin_price);

	short item_search_type = item->GetTradeSearchType();
	short item_search_big_type = TRADEMARKET_CFG.GetSearchBigType(item_search_type);
	if (-1 == item_search_big_type) return false; //big_type必须有效
	
	/************************************** 生成物品index **************************************************/
	SaleItemIndex index = this->MakeSaleItemIndex(index_item_id, price,
		param.put_on_shelve_time, usi->GetSellerUid(), TRADE_ITEM_ORDER_PRICE_UP, param.unique_key);
	SaleItemIndex reverse_index = this->MakeSaleItemIndex(index_item_id, price,
		param.put_on_shelve_time, usi->GetSellerUid(), TRADE_ITEM_ORDER_PRICE_DOWN, param.unique_key);

	/************************************** 生成类型index ***************************************************/
	SaleTypeIndex type_index = this->MakeSaleTypeIndex(index_item_id, price,
		param.put_on_shelve_time, usi->GetSellerUid(), TRADE_ITEM_ORDER_PRICE_UP, param.unique_key);
	SaleTypeIndex reverse_type_index = this->MakeSaleTypeIndex(index_item_id, price,
		param.put_on_shelve_time, usi->GetSellerUid(), TRADE_ITEM_ORDER_PRICE_DOWN, param.unique_key);

	/************************************** 生成宠物index ***************************************************/
	static SalePetIndex pet_index;
	static SalePetQualityIndex pet_quality_index;
	if (!is_announce && is_pet)
	{
		pet_index = this->MakeSalePetIndex(index_item_id, price, param.put_on_shelve_time, usi->GetSellerUid(), level, param.unique_key);
		pet_quality_index = this->MakeSalePetQualityIndex(pet_quality, price, param.put_on_shelve_time, usi->GetSellerUid(), level, param.unique_key);
	}

	/************************************** 生成令牌index ***************************************************/
	SaleMedalIndex medal_index = this->MakeSaleMedalIndex(medal_grade, medal_color, 
		index_item_id, price, param.put_on_shelve_time, usi->GetSellerUid(), 
		TRADE_ITEM_ORDER_PRICE_UP, param.unique_key);

	SaleMedalIndex reverse_medal_index = this->MakeSaleMedalIndex(medal_grade, medal_color,
		index_item_id, price, param.put_on_shelve_time, usi->GetSellerUid(),
		TRADE_ITEM_ORDER_PRICE_DOWN, param.unique_key);

	/************************************** 获取管理器 ******************************************************/
	ItemIndexMap* item_map = this->GetItemIndexMap(index_item_id, is_announce, TRADE_ITEM_ORDER_PRICE_UP);
	if (NULL == item_map) return false;
	ItemIndexMap* reverse_item_map = this->GetItemIndexMap(index_item_id, is_announce, TRADE_ITEM_ORDER_PRICE_DOWN);
	if (NULL == reverse_item_map) return false;
	TypeIndexMap* type_map = this->GetTypeIndexMap(item_search_type, is_announce, TRADE_ITEM_ORDER_PRICE_UP);
	if (NULL == type_map) return false;
	TypeIndexMap* reverse_type_map = this->GetTypeIndexMap(item_search_type, is_announce, TRADE_ITEM_ORDER_PRICE_DOWN);
	if (NULL == reverse_type_map) return false;
	
	TypeIndexMap* big_type_map = this->GetBigTypeIndexMap(item_search_big_type, is_announce, TRADE_ITEM_ORDER_PRICE_UP);
	if (NULL == big_type_map) return false;
	TypeIndexMap* reverse_big_type_map = this->GetBigTypeIndexMap(item_search_big_type, is_announce, TRADE_ITEM_ORDER_PRICE_DOWN);
	if (NULL == reverse_big_type_map) return false;

	TypeIndexMap* level_type_map = NULL;
	TypeIndexMap* reverse_level_type_map = NULL;
	if (is_equipment_or_jewelry)
	{
		level_type_map = this->GetLevelTypeIndexMap(level, item_search_type, is_announce, TRADE_ITEM_ORDER_PRICE_UP);
		if (NULL == level_type_map) return false;
		reverse_level_type_map = this->GetLevelTypeIndexMap(level, item_search_type, is_announce, TRADE_ITEM_ORDER_PRICE_DOWN);
		if (NULL == reverse_level_type_map) return false;
	}

	PetPriceUpIndexMap* pet_price_up_index_map = NULL;
	PetPriceDownIndexMap* pet_price_down_index_map = NULL;
	PetStrLevelUpIndexMap* pet_str_level_up_index_map = NULL;
	PetStrLevelDownIndexMap* pet_str_level_down_index_map = NULL;
	PetQualityPriceUpIndexMap* pet_quality_price_up_index_map = NULL;
	PetQualityPriceDownIndexMap* pet_quality_price_down_index_map = NULL;
	PetQualityStrLevelUpIndexMap* pet_quality_str_level_up_index_map = NULL;
	PetQualityStrLevelDownIndexMap* pet_quality_str_level_down_index_map = NULL;
	if (!is_announce && is_pet)
	{
		pet_price_up_index_map = this->GetPetPriceUpIndexMap(index_item_id);
		if (NULL == pet_price_up_index_map) return false;
		pet_price_down_index_map = this->GetPetPriceDownIndexMap(index_item_id);
		if (NULL == pet_price_down_index_map) return false;
		pet_str_level_up_index_map = this->GetPetStrLevelUpIndexMap(index_item_id);
		if (NULL == pet_str_level_up_index_map) return false;
		pet_str_level_down_index_map = this->GetPetStrLevelDownIndexMap(index_item_id);
		if (NULL == pet_str_level_down_index_map) return false;

		pet_quality_price_up_index_map = this->GetPetQualityPriceUpIndexMap(pet_quality);
		if (NULL == pet_quality_price_up_index_map) return false;
		pet_quality_price_down_index_map = this->GetPetQualityPriceDownIndexMap(pet_quality);
		if (NULL == pet_quality_price_down_index_map) return false;
		pet_quality_str_level_up_index_map = this->GetPetQualityStrLevelUpIndexMap(pet_quality);
		if (NULL == pet_quality_str_level_up_index_map) return false;
		pet_quality_str_level_down_index_map = this->GetPetQualityStrLevelDownIndexMap(pet_quality);
		if (NULL == pet_quality_str_level_down_index_map) return false;
	}

	MedalGradeDownIndexMap* medal_price_up_index_map = NULL;
	MedalGradeDownIndexMap* medal_price_down_index_map = NULL;
	MedalGradeDownIndexMap* medal_special_effect_index_map = NULL;
	MedalGradeDownIndexMap* medal_spe_type_color_index_map = NULL;
	MedalGradeDownIndexMap* reverse_medal_spe_type_color_index_map = NULL;
	MedalGradeDownIndexMap* medal_prof_color_index_map = NULL;
	MedalGradeDownIndexMap* reverse_medal_prof_color_index_map = NULL;
	if (is_medal)
	{
		medal_price_up_index_map = this->GetMedalGradeDownIndexMap(medal_color, is_announce, TRADE_ITEM_ORDER_PRICE_UP);
		if (NULL == medal_price_up_index_map) return false;
		
		medal_price_down_index_map = this->GetMedalGradeDownIndexMap(medal_color, is_announce, TRADE_ITEM_ORDER_PRICE_DOWN);
		if (NULL == medal_price_down_index_map) return false;

		medal_special_effect_index_map = this->GetMedalSpecialEffectTypeIndexMap(medal_special_effect_type);
		if (NULL == medal_special_effect_index_map) return false;

		medal_spe_type_color_index_map = this->GetMedalSpeTypeColorIndexMap(medal_special_effect_type, medal_color, is_announce, TRADE_ITEM_ORDER_PRICE_UP);
		if (NULL == medal_spe_type_color_index_map) return false;

		reverse_medal_spe_type_color_index_map = this->GetMedalSpeTypeColorIndexMap(medal_special_effect_type, medal_color, is_announce, TRADE_ITEM_ORDER_PRICE_DOWN);
		if (NULL == reverse_medal_spe_type_color_index_map) return false;

		medal_prof_color_index_map = this->GetMedalProfColorIndexMap(medal_prof, medal_color, is_announce, TRADE_ITEM_ORDER_PRICE_UP);
		if (NULL == medal_prof_color_index_map) return false;

		reverse_medal_prof_color_index_map = this->GetMedalProfColorIndexMap(medal_prof, medal_color, is_announce, TRADE_ITEM_ORDER_PRICE_DOWN);
		if (NULL == reverse_medal_prof_color_index_map) return false;
	}
	 
	/************************************** 检查是否已有重复 *************************************************/
	if (item_map->find(index) != item_map->end() || reverse_item_map->find(reverse_index) != reverse_item_map->end())
	{
		gamelog::g_log_trade_market.printf(LL_ERROR, "OnAddItemToIndex_1 duplicated! role[%d,%s] index[%u,%d,%u,%d,%lld] is_announce[%s]",
			usi->GetSellerUid(), usi->GetSellerName(), index.item_id, index.price,
			index.put_on_shelve_time, index.uid, index.unique_key, is_announce ? "True" : "False");
		return false;
	}
	if (type_map->find(type_index) != type_map->end() || reverse_type_map->find(reverse_type_index) != reverse_type_map->end())
	{
		gamelog::g_log_trade_market.printf(LL_ERROR, "OnAddItemToIndex_2 duplicated! role[%d,%s] index[%u,%d,%u,%d,%lld] is_announce[%s]",
			usi->GetSellerUid(), usi->GetSellerName(), index.item_id, index.price,
			index.put_on_shelve_time, index.uid, index.unique_key, is_announce ? "True" : "False");
		return false;
	}

	if (big_type_map->find(type_index) != big_type_map->end() || reverse_big_type_map->find(reverse_type_index) != reverse_big_type_map->end())
	{
		gamelog::g_log_trade_market.printf(LL_ERROR, "OnAddItemToIndex_3 duplicated! role[%d,%s] index[%u,%d,%u,%d,%lld] is_announce[%s]",
			usi->GetSellerUid(), usi->GetSellerName(), index.item_id, index.price,
			index.put_on_shelve_time, index.uid, index.unique_key, is_announce ? "True" : "False");
		return false;
	}

	if (is_equipment_or_jewelry)
	{
		if (level_type_map->find(type_index) != level_type_map->end() || reverse_level_type_map->find(reverse_type_index) != reverse_level_type_map->end())
		{
			gamelog::g_log_trade_market.printf(LL_ERROR, "OnAddItemToIndex_4 duplicated! role[%d,%s] index[%u,%d,%u,%d,%lld] is_announce[%s]",
				usi->GetSellerUid(), usi->GetSellerName(), index.item_id, index.price,
				index.put_on_shelve_time, index.uid, index.unique_key, is_announce ? "True" : "False");
			return false;
		}
	}

	if (!is_announce && is_pet)
	{
		if (pet_price_up_index_map->find(pet_index) != pet_price_up_index_map->end() ||
			pet_price_down_index_map->find(pet_index) != pet_price_down_index_map->end() ||
			pet_str_level_up_index_map->find(pet_index) != pet_str_level_up_index_map->end() ||
			pet_str_level_down_index_map->find(pet_index) != pet_str_level_down_index_map->end())
		{
			gamelog::g_log_trade_market.printf(LL_ERROR, "OnAddItemToIndex_5 duplicated! role[%d,%s] index[%u,%d,%u,%d,%lld] str_level[%d]",
				usi->GetSellerUid(), usi->GetSellerName(), index.item_id, index.price, index.put_on_shelve_time, index.uid, index.unique_key, level);
			return false;
		}

		if (pet_quality_price_up_index_map->find(pet_quality_index) != pet_quality_price_up_index_map->end() ||
			pet_quality_price_down_index_map->find(pet_quality_index) != pet_quality_price_down_index_map->end() ||
			pet_quality_str_level_up_index_map->find(pet_quality_index) != pet_quality_str_level_up_index_map->end() ||
			pet_quality_str_level_down_index_map->find(pet_quality_index) != pet_quality_str_level_down_index_map->end())
		{
			gamelog::g_log_trade_market.printf(LL_ERROR, "OnAddItemToIndex_6 duplicated! role[%d,%s] index[%d,%d,%u,%d,%lld] str_level[%d]",
				usi->GetSellerUid(), usi->GetSellerName(), pet_quality_index.quality, pet_quality_index.price, pet_quality_index.put_on_shelve_time, pet_quality_index.uid, pet_quality_index.unique_key, level);
			return false;
		}
	}

	if (is_medal)
	{
		if (medal_price_up_index_map->find(medal_index) != medal_price_up_index_map->end()
			|| medal_price_down_index_map->find(reverse_medal_index) != medal_price_down_index_map->end())
		{
			gamelog::g_log_trade_market.printf(LL_ERROR, "OnAddItemToIndex_Medal_Index duplicated! role[%d,%s] index[%u,%d,%u,%d,%lld] is_announce[%s]",
				usi->GetSellerUid(), usi->GetSellerName(), index.item_id, index.price,
				index.put_on_shelve_time, index.uid, index.unique_key, is_announce ? "True" : "False");
			return false;
		}

		if (medal_special_effect_index_map->find(medal_index) != medal_special_effect_index_map->end())
		{
			gamelog::g_log_trade_market.printf(LL_ERROR, "OnAddItemToIndex_Medal_SPE_TYPE_Index duplicated! role[%d,%s] index[%u,%d,%u,%d,%lld] is_announce[%s]",
				usi->GetSellerUid(), usi->GetSellerName(), index.item_id, index.price,
				index.put_on_shelve_time, index.uid, index.unique_key, is_announce ? "True" : "False");
			return false;
		}

		if (medal_spe_type_color_index_map->find(medal_index) != medal_spe_type_color_index_map->end()
			|| reverse_medal_spe_type_color_index_map->find(reverse_medal_index) != reverse_medal_spe_type_color_index_map->end())
		{
			gamelog::g_log_trade_market.printf(LL_ERROR, "OnAddItemToIndex_Medal_SPE_COLOR_Index duplicated! role[%d,%s] index[%u,%d,%u,%d,%lld] is_announce[%s]",
				usi->GetSellerUid(), usi->GetSellerName(), index.item_id, index.price,
				index.put_on_shelve_time, index.uid, index.unique_key, is_announce ? "True" : "False");
			return false;
		}

		if (medal_prof_color_index_map->find(medal_index) != medal_prof_color_index_map->end()
			|| reverse_medal_prof_color_index_map->find(reverse_medal_index) != reverse_medal_prof_color_index_map->end())
		{
			gamelog::g_log_trade_market.printf(LL_ERROR, "OnAddItemToIndex_Medal_PROF_COLOR_Index duplicated! role[%d,%s] index[%u,%d,%u,%d,%lld] is_announce[%s]",
				usi->GetSellerUid(), usi->GetSellerName(), index.item_id, index.price,
				index.put_on_shelve_time, index.uid, index.unique_key, is_announce ? "True" : "False");
			return false;
		}
	}

	/************************************** 统一insert到map中 ************************************************/
	item_map->insert(ItemIndexMap::value_type(index, usi));
	reverse_item_map->insert(ItemIndexMap::value_type(reverse_index, usi));
	type_map->insert(TypeIndexMap::value_type(type_index, usi));
	reverse_type_map->insert(TypeIndexMap::value_type(reverse_type_index, usi));
	big_type_map->insert(TypeIndexMap::value_type(type_index, usi));
	reverse_big_type_map->insert(TypeIndexMap::value_type(reverse_type_index, usi));
	
	if (is_equipment_or_jewelry)
	{
		level_type_map->insert(TypeIndexMap::value_type(type_index, usi));
		reverse_level_type_map->insert(TypeIndexMap::value_type(reverse_type_index, usi));
	}

	if (!is_announce && is_pet)
	{
		pet_price_up_index_map->insert(PetPriceUpIndexMap::value_type(pet_index, usi));
		pet_price_down_index_map->insert(PetPriceDownIndexMap::value_type(pet_index, usi));
		pet_str_level_up_index_map->insert(PetStrLevelUpIndexMap::value_type(pet_index, usi));
		pet_str_level_down_index_map->insert(PetStrLevelDownIndexMap::value_type(pet_index, usi));
	
		pet_quality_price_up_index_map->insert(PetQualityPriceUpIndexMap::value_type(pet_quality_index, usi));
		pet_quality_price_down_index_map->insert(PetQualityPriceDownIndexMap::value_type(pet_quality_index, usi));
		pet_quality_str_level_up_index_map->insert(PetQualityStrLevelUpIndexMap::value_type(pet_quality_index, usi));
		pet_quality_str_level_down_index_map->insert(PetQualityStrLevelDownIndexMap::value_type(pet_quality_index, usi));
	}

	if (is_medal)
	{
		medal_price_up_index_map->insert(MedalGradeDownIndexMap::value_type(medal_index, usi));
		medal_price_down_index_map->insert(MedalGradeDownIndexMap::value_type(reverse_medal_index, usi));
		medal_special_effect_index_map->insert(MedalGradeDownIndexMap::value_type(medal_index, usi));
		medal_spe_type_color_index_map->insert(MedalGradeDownIndexMap::value_type(medal_index, usi));
		reverse_medal_spe_type_color_index_map->insert(MedalGradeDownIndexMap::value_type(reverse_medal_index, usi));
		medal_prof_color_index_map->insert(MedalGradeDownIndexMap::value_type(medal_index, usi));
		reverse_medal_prof_color_index_map->insert(MedalGradeDownIndexMap::value_type(reverse_medal_index, usi));
	}

	gamelog::g_log_trade_market.printf(LL_INFO, "OnAddItemToIndex SUCC role[%d,%s] index[%u,%d,%u,%d,%lld] is_announce[%s]",
		usi->GetSellerUid(), usi->GetSellerName(), index.item_id, index.price,
		index.put_on_shelve_time, index.uid, index.unique_key, is_announce ? "True" : "False");

	return true;
}

bool TradeMarketItemIndex::OnRemoveItemFromIndex(SaleItemBase* usi, bool is_announce)
{
	if (NULL == usi) return false;

	bool is_equipment_or_jewelry = false;
	bool is_pet = false;
	short level = 0;
	short pet_quality = PET_QUALITY_INVALID;
	bool is_medal = false;
	short medal_color = 0;
	short medal_grade = 0;
	int medal_special_effect_type = 0;
	int medal_prof = 0;

	ItemID index_item_id = usi->GetSaleItemParamConst().item_wrapper.item_id;
	const ItemBase* item = ITEMPOOL->GetItem(usi->GetSaleItemParamConst().item_wrapper.item_id);
	if (NULL == item) return false;
	if (I_TYPE_EQUIPMENT == item->GetItemType())
	{
		is_equipment_or_jewelry = true;
		level = ((Equipment*)item)->GetEquipLevel();
	}
	else if (I_TYPE_JEWELRY == item->GetItemType())
	{
		is_equipment_or_jewelry = true;
		level = ((Jewelry*)item)->GetJewelryLevel();
	}
	else if (I_TYPE_PET == item->GetItemType() && item->GetItemId() >= TRADE_PET_ITEM_ID_BEGIN && item->GetItemId() < TRADE_PET_ITEM_ID_END)
	{
		is_pet = true;

		static NetValueItemParam item_param;
		item_param.SetData(usi->GetSaleItemParamConst().item_wrapper.param_data);

		level = item_param.pet_param.str_level;

		const PetCfg* pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(index_item_id);
		if (NULL != pet_cfg && pet_cfg->pet_mutant_times)
		{
			index_item_id = pet_cfg->prototype_pet_id; // 转变为普通宠物的pet_id

			gamelog::g_log_trade_market.printf(LL_INFO, "OnRemoveItemFromIndex role[%d,%s] MutantPet item_id change from [%d] to [%d]",
				usi->GetSellerUid(), usi->GetSellerName(),
				usi->GetSaleItemParamConst().item_wrapper.item_id, index_item_id);
		}

		if (NULL != pet_cfg)
		{
			pet_quality = pet_cfg->quality;
		}
	}
	else if (I_TYPE_MEDAL == item->GetItemType())
	{
		is_medal = true;

		static NetValueItemParam item_param;
		item_param.SetData(usi->GetSaleItemParamConst().item_wrapper.param_data);

		medal_grade = item_param.medal_param.grade;
		const MedalUpgradeCfg* cfg = LOGIC_CONFIG->GetMedalConfig()->GetMedalUpgradeCfg(medal_grade);
		if (NULL != cfg)
		{
			medal_color = cfg->color;
		}

		const SpecialEffectCfg* sp_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(item_param.medal_param.sp_id);
		if (NULL != sp_cfg)
		{
			medal_special_effect_type = sp_cfg->sp_type;
			medal_prof = sp_cfg->prof_type;
		}
	}

	const SaleItemParam& param = usi->GetSaleItemParamConst();
	int price = (param.gold_price > 0 ? param.gold_price : param.immortal_coin_price);

	short item_search_type = item->GetTradeSearchType();
	short item_search_big_type = TRADEMARKET_CFG.GetSearchBigType(item_search_type);
	if (-1 == item_search_big_type) return false; //big_type必须有效

	/************************************** 生成物品index **************************************************/
	SaleItemIndex index = this->MakeSaleItemIndex(index_item_id, price,
		param.put_on_shelve_time, usi->GetSellerUid(), TRADE_ITEM_ORDER_PRICE_UP, param.unique_key);
	SaleItemIndex reverse_index = this->MakeSaleItemIndex(index_item_id, price,
		param.put_on_shelve_time, usi->GetSellerUid(), TRADE_ITEM_ORDER_PRICE_DOWN, param.unique_key);

	/************************************** 生成类型index ***************************************************/
	SaleTypeIndex type_index = this->MakeSaleTypeIndex(index_item_id, price,
		param.put_on_shelve_time, usi->GetSellerUid(), TRADE_ITEM_ORDER_PRICE_UP, param.unique_key);
	SaleTypeIndex reverse_type_index = this->MakeSaleTypeIndex(index_item_id, price,
		param.put_on_shelve_time, usi->GetSellerUid(), TRADE_ITEM_ORDER_PRICE_DOWN, param.unique_key);

	/************************************** 生成宠物index ***************************************************/
	static SalePetIndex pet_index;
	static SalePetQualityIndex pet_quality_index;
	if (!is_announce && is_pet)
	{
		pet_index = this->MakeSalePetIndex(index_item_id, price, param.put_on_shelve_time, usi->GetSellerUid(), level, param.unique_key);
		pet_quality_index = this->MakeSalePetQualityIndex(pet_quality, price, param.put_on_shelve_time, usi->GetSellerUid(), level, param.unique_key);
	}

	/************************************** 生成令牌index ***************************************************/
	SaleMedalIndex medal_index = this->MakeSaleMedalIndex(medal_grade, medal_color,
		index_item_id, price, param.put_on_shelve_time, usi->GetSellerUid(),
		TRADE_ITEM_ORDER_PRICE_UP, param.unique_key);

	SaleMedalIndex reverse_medal_index = this->MakeSaleMedalIndex(medal_grade, medal_color,
		index_item_id, price, param.put_on_shelve_time, usi->GetSellerUid(),
		TRADE_ITEM_ORDER_PRICE_DOWN, param.unique_key);

	/************************************** 获取管理器 ******************************************************/
	ItemIndexMap* item_map = this->GetItemIndexMap(index_item_id, is_announce, TRADE_ITEM_ORDER_PRICE_UP);
	if (NULL == item_map) return false;
	ItemIndexMap* reverse_item_map = this->GetItemIndexMap(index_item_id, is_announce, TRADE_ITEM_ORDER_PRICE_DOWN);
	if (NULL == reverse_item_map) return false;
	TypeIndexMap* type_map = this->GetTypeIndexMap(item_search_type, is_announce, TRADE_ITEM_ORDER_PRICE_UP);
	if (NULL == type_map) return false;
	TypeIndexMap* reverse_type_map = this->GetTypeIndexMap(item_search_type, is_announce, TRADE_ITEM_ORDER_PRICE_DOWN);
	if (NULL == reverse_type_map) return false;
	TypeIndexMap* big_type_map = this->GetBigTypeIndexMap(item_search_big_type, is_announce, TRADE_ITEM_ORDER_PRICE_UP);
	if (NULL == big_type_map) return false;
	TypeIndexMap* reverse_big_type_map = this->GetBigTypeIndexMap(item_search_big_type, is_announce, TRADE_ITEM_ORDER_PRICE_DOWN);
	if (NULL == reverse_big_type_map) return false;

	TypeIndexMap* level_type_map = NULL;
	TypeIndexMap* reverse_level_type_map = NULL;
	if (is_equipment_or_jewelry)
	{
		level_type_map = this->GetLevelTypeIndexMap(level, item_search_type, is_announce, TRADE_ITEM_ORDER_PRICE_UP);
		if (NULL == level_type_map) return false;
		reverse_level_type_map = this->GetLevelTypeIndexMap(level, item_search_type, is_announce, TRADE_ITEM_ORDER_PRICE_DOWN);
		if (NULL == reverse_level_type_map) return false;
	}

	PetPriceUpIndexMap* pet_price_up_index_map = NULL;
	PetPriceDownIndexMap* pet_price_down_index_map = NULL;
	PetStrLevelUpIndexMap* pet_str_level_up_index_map = NULL;
	PetStrLevelDownIndexMap* pet_str_level_down_index_map = NULL;
	PetQualityPriceUpIndexMap* pet_quality_price_up_index_map = NULL;
	PetQualityPriceDownIndexMap* pet_quality_price_down_index_map = NULL;
	PetQualityStrLevelUpIndexMap* pet_quality_str_level_up_index_map = NULL;
	PetQualityStrLevelDownIndexMap* pet_quality_str_level_down_index_map = NULL;
	if (!is_announce && is_pet)
	{
		pet_price_up_index_map = this->GetPetPriceUpIndexMap(index_item_id);
		if (NULL == pet_price_up_index_map) return false;
		pet_price_down_index_map = this->GetPetPriceDownIndexMap(index_item_id);
		if (NULL == pet_price_down_index_map) return false;
		pet_str_level_up_index_map = this->GetPetStrLevelUpIndexMap(index_item_id);
		if (NULL == pet_str_level_up_index_map) return false;
		pet_str_level_down_index_map = this->GetPetStrLevelDownIndexMap(index_item_id);
		if (NULL == pet_str_level_down_index_map) return false;

		pet_quality_price_up_index_map = this->GetPetQualityPriceUpIndexMap(pet_quality);
		if (NULL == pet_quality_price_up_index_map) return false;
		pet_quality_price_down_index_map = this->GetPetQualityPriceDownIndexMap(pet_quality);
		if (NULL == pet_quality_price_down_index_map) return false;
		pet_quality_str_level_up_index_map = this->GetPetQualityStrLevelUpIndexMap(pet_quality);
		if (NULL == pet_quality_str_level_up_index_map) return false;
		pet_quality_str_level_down_index_map = this->GetPetQualityStrLevelDownIndexMap(pet_quality);
		if (NULL == pet_quality_str_level_down_index_map) return false;
	}
	
	MedalGradeDownIndexMap* medal_price_up_index_map = NULL;
	MedalGradeDownIndexMap* medal_price_down_index_map = NULL;
	MedalGradeDownIndexMap* medal_special_effect_index_map = NULL;
	MedalGradeDownIndexMap* medal_spe_type_color_index_map = NULL;
	MedalGradeDownIndexMap* reverse_medal_spe_type_color_index_map = NULL;
	MedalGradeDownIndexMap* medal_prof_color_index_map = NULL;
	MedalGradeDownIndexMap* reverse_medal_prof_color_index_map = NULL;
	if (is_medal)
	{
		medal_price_up_index_map = this->GetMedalGradeDownIndexMap(medal_color, is_announce, TRADE_ITEM_ORDER_PRICE_UP);
		if (NULL == medal_price_up_index_map) return false;

		medal_price_down_index_map = this->GetMedalGradeDownIndexMap(medal_color, is_announce, TRADE_ITEM_ORDER_PRICE_DOWN);
		if (NULL == medal_price_down_index_map) return false;

		medal_special_effect_index_map = this->GetMedalSpecialEffectTypeIndexMap(medal_special_effect_type);
		if (NULL == medal_special_effect_index_map) return false;

		medal_spe_type_color_index_map = this->GetMedalSpeTypeColorIndexMap(medal_special_effect_type, medal_color, is_announce, TRADE_ITEM_ORDER_PRICE_UP);
		if (NULL == medal_spe_type_color_index_map) return false;

		reverse_medal_spe_type_color_index_map = this->GetMedalSpeTypeColorIndexMap(medal_special_effect_type, medal_color, is_announce, TRADE_ITEM_ORDER_PRICE_DOWN);
		if (NULL == reverse_medal_spe_type_color_index_map) return false;

		medal_prof_color_index_map = this->GetMedalProfColorIndexMap(medal_prof, medal_color, is_announce, TRADE_ITEM_ORDER_PRICE_UP);
		if (NULL == medal_prof_color_index_map) return false;

		reverse_medal_prof_color_index_map = this->GetMedalProfColorIndexMap(medal_prof, medal_color, is_announce, TRADE_ITEM_ORDER_PRICE_DOWN);
		if (NULL == reverse_medal_prof_color_index_map) return false;
	}

	/************************************** 查找并删除 *******************************************************/
	ItemIndexMap::iterator it = item_map->find(index);
	if (it == item_map->end())
	{
		gamelog::g_log_trade_market.printf(LL_ERROR, "OnRemoveItemFromIndex_1 not found! role[%d,%s] index[%u,%d,%u,%d, %lld] is_announce[%s]",
			usi->GetSellerUid(), usi->GetSellerName(), index.item_id, index.price,
			index.put_on_shelve_time, index.uid, index.unique_key, is_announce ? "True" : "False");
	}
	else
	{
		item_map->erase(it);
	}

	it = reverse_item_map->find(reverse_index);
	if (it == reverse_item_map->end())
	{
		gamelog::g_log_trade_market.printf(LL_ERROR, "OnRemoveItemFromReverseIndex_2 not found! role[%d,%s] index[%u,%d,%u,%d,%lld] is_announce[%s]",
			usi->GetSellerUid(), usi->GetSellerName(), index.item_id, index.price,
			index.put_on_shelve_time, index.uid, index.unique_key, is_announce ? "True" : "False");
	}
	else
	{
		reverse_item_map->erase(it);
	}

	TypeIndexMap::iterator type_it = type_map->find(type_index);
	if (type_it == type_map->end())
	{
		gamelog::g_log_trade_market.printf(LL_ERROR, "OnRemoveTypeFromIndex_3 not found! role[%d,%s] index[%u,%d,%u,%d,%lld] is_announce[%s]",
			usi->GetSellerUid(), usi->GetSellerName(), index.item_id, index.price,
			index.put_on_shelve_time, index.uid, index.unique_key, is_announce ? "True" : "False");
	}
	else
	{
		type_map->erase(type_it);
	}

	type_it = reverse_type_map->find(reverse_type_index);
	if (type_it == reverse_type_map->end())
	{
		gamelog::g_log_trade_market.printf(LL_ERROR, "OnRemoveTypeFromReverseIndex_4 not found! role[%d,%s] index[%u,%d,%u,%d,%lld] is_announce[%s]",
			usi->GetSellerUid(), usi->GetSellerName(), index.item_id, index.price,
			index.put_on_shelve_time, index.uid, index.unique_key, is_announce ? "True" : "False");
	}
	else
	{
		reverse_type_map->erase(type_it);
	}

	TypeIndexMap::iterator big_type_it = big_type_map->find(type_index);
	if (big_type_it == big_type_map->end())
	{
		gamelog::g_log_trade_market.printf(LL_ERROR, "OnRemoveBigTypeFromIndex_5 not found! role[%d,%s] index[%u,%d,%u,%d,%lld] is_announce[%s]",
			usi->GetSellerUid(), usi->GetSellerName(), index.item_id, index.price,
			index.put_on_shelve_time, index.uid, index.unique_key, is_announce ? "True" : "False");
	}
	else
	{
		big_type_map->erase(big_type_it);
	}

	big_type_it = reverse_big_type_map->find(reverse_type_index);
	if (big_type_it == reverse_big_type_map->end())
	{
		gamelog::g_log_trade_market.printf(LL_ERROR, "OnRemoveBigTypeFromReverseIndex_6 not found! role[%d,%s] index[%u,%d,%u,%d,%lld] is_announce[%s]",
			usi->GetSellerUid(), usi->GetSellerName(), index.item_id, index.price,
			index.put_on_shelve_time, index.uid, index.unique_key, is_announce ? "True" : "False");
	}
	else
	{
		reverse_big_type_map->erase(big_type_it);
	}

	if (is_equipment_or_jewelry)
	{
		TypeIndexMap::iterator level_it = level_type_map->find(type_index);
		if (level_it == level_type_map->end())
		{
			gamelog::g_log_trade_market.printf(LL_ERROR, "OnRemoveLevelTypeFromIndex_7 not found! role[%d,%s] index[%u,%d,%u,%d,%lld] is_announce[%s]",
				usi->GetSellerUid(), usi->GetSellerName(), index.item_id, index.price,
				index.put_on_shelve_time, index.uid, index.unique_key, is_announce ? "True" : "False");
		}
		else
		{
			level_type_map->erase(level_it);
		}

		level_it = reverse_level_type_map->find(reverse_type_index);
		if (level_it == reverse_level_type_map->end())
		{
			gamelog::g_log_trade_market.printf(LL_ERROR, "OnRemoveLevelTypeFromReverseIndex_8 not found! role[%d,%s] index[%u,%d,%u,%d,%lld] is_announce[%s]",
				usi->GetSellerUid(), usi->GetSellerName(), index.item_id, index.price,
				index.put_on_shelve_time, index.uid, index.unique_key, is_announce ? "True" : "False");
		}
		else
		{
			reverse_level_type_map->erase(level_it);
		}
	}

	if (!is_announce && is_pet)
	{
		PetPriceUpIndexMap::iterator it1 = pet_price_up_index_map->find(pet_index);
		if (it1 == pet_price_up_index_map->end())
		{
			gamelog::g_log_trade_market.printf(LL_ERROR, "OnRemoveLevelTypeFromReverseIndex_9 not found! role[%d,%s] index[%u,%d,%u,%d,%lld] str_level[%d]",
				usi->GetSellerUid(), usi->GetSellerName(), index.item_id, index.price,
				index.put_on_shelve_time, index.uid, index.unique_key, level);
		}
		else
		{
			pet_price_up_index_map->erase(pet_index);
		}
		
		PetPriceDownIndexMap::iterator it2 = pet_price_down_index_map->find(pet_index);
		if (it2 == pet_price_down_index_map->end())
		{
			gamelog::g_log_trade_market.printf(LL_ERROR, "OnRemoveLevelTypeFromReverseIndex_10 not found! role[%d,%s] index[%u,%d,%u,%d,%lld] str_level[%d]",
				usi->GetSellerUid(), usi->GetSellerName(), index.item_id, index.price,
				index.put_on_shelve_time, index.uid, index.unique_key, level);
		}
		else
		{
			pet_price_down_index_map->erase(pet_index);
		}

		PetStrLevelUpIndexMap::iterator it3 = pet_str_level_up_index_map->find(pet_index);
		if (it3 == pet_str_level_up_index_map->end())
		{
			gamelog::g_log_trade_market.printf(LL_ERROR, "OnRemoveLevelTypeFromReverseIndex_11 not found! role[%d,%s] index[%u,%d,%u,%d,%lld] str_level[%d]",
				usi->GetSellerUid(), usi->GetSellerName(), index.item_id, index.price,
				index.put_on_shelve_time, index.uid, index.unique_key, level);
		}
		else
		{
			pet_str_level_up_index_map->erase(pet_index);
		}

		PetStrLevelDownIndexMap::iterator it4 = pet_str_level_down_index_map->find(pet_index);
		if (it4 == pet_str_level_down_index_map->end())
		{
			gamelog::g_log_trade_market.printf(LL_ERROR, "OnRemoveLevelTypeFromReverseIndex_12 not found! role[%d,%s] index[%u,%d,%u,%d,%lld] str_level[%d]",
				usi->GetSellerUid(), usi->GetSellerName(), index.item_id, index.price,
				index.put_on_shelve_time, index.uid, index.unique_key, level);
		}
		else
		{
			pet_str_level_down_index_map->erase(pet_index);
		}
	}

	if (!is_announce && is_pet)
	{
		PetQualityPriceUpIndexMap::iterator it1 = pet_quality_price_up_index_map->find(pet_quality_index);
		if (it1 == pet_quality_price_up_index_map->end())
		{
			gamelog::g_log_trade_market.printf(LL_ERROR, "OnRemoveLevelTypeFromReverseIndex_13 not found! role[%d,%s] index[%d,%d,%u,%d,%lld] str_level[%d]",
				usi->GetSellerUid(), usi->GetSellerName(), pet_quality_index.quality, pet_quality_index.price,
				pet_quality_index.put_on_shelve_time, pet_quality_index.uid, pet_quality_index.unique_key, level);
		}
		else
		{
			pet_quality_price_up_index_map->erase(pet_quality_index);
		}

		PetQualityPriceDownIndexMap::iterator it2 = pet_quality_price_down_index_map->find(pet_quality_index);
		if (it2 == pet_quality_price_down_index_map->end())
		{
			gamelog::g_log_trade_market.printf(LL_ERROR, "OnRemoveLevelTypeFromReverseIndex_14 not found! role[%d,%s] index[%d,%d,%u,%d,%lld] str_level[%d]",
				usi->GetSellerUid(), usi->GetSellerName(), pet_quality_index.quality, pet_quality_index.price,
				pet_quality_index.put_on_shelve_time, pet_quality_index.uid, pet_quality_index.unique_key, level);
		}
		else
		{
			pet_quality_price_down_index_map->erase(pet_quality_index);
		}

		PetQualityStrLevelUpIndexMap::iterator it3 = pet_quality_str_level_up_index_map->find(pet_quality_index);
		if (it3 == pet_quality_str_level_up_index_map->end())
		{
			gamelog::g_log_trade_market.printf(LL_ERROR, "OnRemoveLevelTypeFromReverseIndex_15 not found! role[%d,%s] index[%d,%d,%u,%d,%lld] str_level[%d]",
				usi->GetSellerUid(), usi->GetSellerName(), pet_quality_index.quality, pet_quality_index.price,
				pet_quality_index.put_on_shelve_time, pet_quality_index.uid, pet_quality_index.unique_key, level);
		}
		else
		{
			pet_quality_str_level_up_index_map->erase(pet_quality_index);
		}

		PetQualityStrLevelDownIndexMap::iterator it4 = pet_quality_str_level_down_index_map->find(pet_quality_index);
		if (it4 == pet_quality_str_level_down_index_map->end())
		{
			gamelog::g_log_trade_market.printf(LL_ERROR, "OnRemoveLevelTypeFromReverseIndex_16 not found! role[%d,%s] index[%d,%d,%u,%d,%lld] str_level[%d]",
				usi->GetSellerUid(), usi->GetSellerName(), pet_quality_index.quality, pet_quality_index.price,
				pet_quality_index.put_on_shelve_time, pet_quality_index.uid, pet_quality_index.unique_key, level);
		}
		else
		{
			pet_quality_str_level_down_index_map->erase(pet_quality_index);
		}
	}

	if (is_medal)
	{
		MedalGradeDownIndexMap::iterator it = medal_price_up_index_map->find(medal_index);
		if (it == medal_price_up_index_map->end())
		{
			gamelog::g_log_trade_market.printf(LL_ERROR, "OnRemoveMedalIndex_1 not found! role[%d,%s] index[%u,%d,%u,%d,%lld]",
				usi->GetSellerUid(), usi->GetSellerName(), index.item_id, index.price,
				index.put_on_shelve_time, index.uid, index.unique_key);
		}
		else
		{
			medal_price_up_index_map->erase(it);
		}

		it = medal_price_down_index_map->find(reverse_medal_index);
		if (it == medal_price_down_index_map->end())
		{
			gamelog::g_log_trade_market.printf(LL_ERROR, "OnRemoveMedalIndex_2 not found! role[%d,%s] index[%u,%d,%u,%d,%lld]",
				usi->GetSellerUid(), usi->GetSellerName(), index.item_id, index.price,
				index.put_on_shelve_time, index.uid, index.unique_key);
		}
		else
		{
			medal_price_down_index_map->erase(it);
		}		

		it = medal_special_effect_index_map->find(medal_index);
		if (it == medal_special_effect_index_map->end())
		{
			gamelog::g_log_trade_market.printf(LL_ERROR, "OnRemoveMedalIndex_3 not found! role[%d,%s] index[%u,%d,%u,%d,%lld]",
				usi->GetSellerUid(), usi->GetSellerName(), index.item_id, index.price,
				index.put_on_shelve_time, index.uid, index.unique_key);
	}
		else
		{
			medal_special_effect_index_map->erase(it);
		}

		it = medal_spe_type_color_index_map->find(medal_index);
		if (it == medal_spe_type_color_index_map->end())
		{
			gamelog::g_log_trade_market.printf(LL_ERROR, "OnRemoveMedalIndex_4 not found! role[%d,%s] index[%u,%d,%u,%d,%lld]",
				usi->GetSellerUid(), usi->GetSellerName(), index.item_id, index.price,
				index.put_on_shelve_time, index.uid, index.unique_key);
		}
		else
		{
			medal_spe_type_color_index_map->erase(it);
		}

		it = reverse_medal_spe_type_color_index_map->find(reverse_medal_index);
		if (it == reverse_medal_spe_type_color_index_map->end())
		{
			gamelog::g_log_trade_market.printf(LL_ERROR, "OnRemoveMedalIndex_5 not found! role[%d,%s] index[%u,%d,%u,%d,%lld]",
				usi->GetSellerUid(), usi->GetSellerName(), index.item_id, index.price,
				index.put_on_shelve_time, index.uid, index.unique_key);
		}
		else
		{
			reverse_medal_spe_type_color_index_map->erase(it);
		}

		it = medal_prof_color_index_map->find(medal_index);
		if (it == medal_prof_color_index_map->end())
		{
			gamelog::g_log_trade_market.printf(LL_ERROR, "OnRemoveMedalIndex_6 not found! role[%d,%s] index[%u,%d,%u,%d,%lld]",
				usi->GetSellerUid(), usi->GetSellerName(), index.item_id, index.price,
				index.put_on_shelve_time, index.uid, index.unique_key);
		}
		else
		{
			medal_prof_color_index_map->erase(it);
		}

		it = reverse_medal_prof_color_index_map->find(reverse_medal_index);
		if (it == reverse_medal_prof_color_index_map->end())
		{
			gamelog::g_log_trade_market.printf(LL_ERROR, "OnRemoveMedalIndex_7 not found! role[%d,%s] index[%u,%d,%u,%d,%lld]",
				usi->GetSellerUid(), usi->GetSellerName(), index.item_id, index.price,
				index.put_on_shelve_time, index.uid, index.unique_key);
		}
		else
		{
			reverse_medal_prof_color_index_map->erase(it);
		}
	}

	gamelog::g_log_trade_market.printf(LL_INFO, "OnRemoveItemFromIndex SUCC, role[%d,%s] index[%u,%d,%u,%d, %lld] is_announce[%s]",
		usi->GetSellerUid(), usi->GetSellerName(), index.item_id, index.price,
		index.put_on_shelve_time, index.uid, index.unique_key, is_announce ? "True" : "False");
	
	return true;
}

bool TradeMarketItemIndex::OnSearchPetForQuickBuyByPriceUp(const SalePetIndex& sale_pet_index, int req_item_per_page, SaleItemBase** out_list, int* out_count)
{
	PetPriceUpIndexMap* index_map = this->GetPetPriceUpIndexMap(sale_pet_index.item_id);
	if (NULL == index_map) return false;

	PetPriceUpIndexMap::iterator begin_it;
	if (0 == sale_pet_index.pet_str_level && 0 == sale_pet_index.price && 0 == sale_pet_index.put_on_shelve_time && 0 == sale_pet_index.uid)
	{
		begin_it = index_map->begin();
	}
	else
	{
		begin_it = index_map->upper_bound(sale_pet_index);
	}

	*out_count = 0;
	for (PetPriceUpIndexMap::iterator it = begin_it; it != index_map->end() && (*out_count) < req_item_per_page; ++it)
	{
		out_list[(*out_count)++] = it->second;
	}
	
	return true;
}

bool TradeMarketItemIndex::OnSearchPetForQuickBuyByPriceDown(const SalePetIndex& sale_pet_index, int req_item_per_page, SaleItemBase** out_list, int* out_count)
{
	PetPriceDownIndexMap* index_map = this->GetPetPriceDownIndexMap(sale_pet_index.item_id);
	if (NULL == index_map) return false;

	PetPriceDownIndexMap::iterator begin_it;
	if (0 == sale_pet_index.pet_str_level && 0 == sale_pet_index.price && 0 == sale_pet_index.put_on_shelve_time && 0 == sale_pet_index.uid)
	{
		begin_it = index_map->begin();
	}
	else
	{
		begin_it = index_map->upper_bound(sale_pet_index);
	}

	*out_count = 0;
	for (PetPriceDownIndexMap::iterator it = begin_it; it != index_map->end() && (*out_count) < req_item_per_page; ++it)
	{
		out_list[(*out_count)++] = it->second;
	}

	return true;
}

bool TradeMarketItemIndex::OnSearchPetForQuickBuyByStrLevelUp(const SalePetIndex& sale_pet_index, int req_item_per_page, SaleItemBase** out_list, int* out_count)
{
	PetStrLevelUpIndexMap* index_map = this->GetPetStrLevelUpIndexMap(sale_pet_index.item_id);
	if (NULL == index_map) return false;

	PetStrLevelUpIndexMap::iterator begin_it;
	if (0 == sale_pet_index.pet_str_level && 0 == sale_pet_index.price && 0 == sale_pet_index.put_on_shelve_time && 0 == sale_pet_index.uid)
	{
		begin_it = index_map->begin();
	}
	else
	{
		begin_it = index_map->upper_bound(sale_pet_index);
	}

	*out_count = 0;
	for (PetStrLevelUpIndexMap::iterator it = begin_it; it != index_map->end() && (*out_count) < req_item_per_page; ++it)
	{
		out_list[(*out_count)++] = it->second;
	}

	return true;
}

bool TradeMarketItemIndex::OnSearchPetForQuickBuyByStrLevelDown(const SalePetIndex& sale_pet_index, int req_item_per_page, SaleItemBase** out_list, int* out_count)
{
	PetStrLevelDownIndexMap* index_map = this->GetPetStrLevelDownIndexMap(sale_pet_index.item_id);
	if (NULL == index_map) return false;

	PetStrLevelDownIndexMap::iterator begin_it;
	if (0 == sale_pet_index.pet_str_level && 0 == sale_pet_index.price && 0 == sale_pet_index.put_on_shelve_time && 0 == sale_pet_index.uid)
	{
		begin_it = index_map->begin();
	}
	else
	{
		begin_it = index_map->upper_bound(sale_pet_index);
	}

	*out_count = 0;
	for (PetStrLevelDownIndexMap::iterator it = begin_it; it != index_map->end() && (*out_count) < req_item_per_page; ++it)
	{
		out_list[(*out_count)++] = it->second;
	}

	return true;
}

bool TradeMarketItemIndex::OnSearchPetQualityForQuickBuyByPriceUp(const SalePetQualityIndex& sale_pet_index, int req_item_per_page, SaleItemBase** out_list, int* out_count)
{
	PetQualityPriceUpIndexMap* index_map = this->GetPetQualityPriceUpIndexMap(sale_pet_index.quality);
	if (NULL == index_map) return false;

	PetQualityPriceUpIndexMap::iterator begin_it;
	if (0 == sale_pet_index.pet_str_level && 0 == sale_pet_index.price && 0 == sale_pet_index.put_on_shelve_time && 0 == sale_pet_index.uid)
	{
		begin_it = index_map->begin();
	}
	else
	{
		begin_it = index_map->upper_bound(sale_pet_index);
	}

	*out_count = 0;
	for (PetQualityPriceUpIndexMap::iterator it = begin_it; it != index_map->end() && (*out_count) < req_item_per_page; ++it)
	{
		out_list[(*out_count)++] = it->second;
	}

	return true;
}

bool TradeMarketItemIndex::OnSearchPetQualityForQuickBuyByPriceDown(const SalePetQualityIndex& sale_pet_index, int req_item_per_page, SaleItemBase** out_list, int* out_count)
{
	PetQualityPriceDownIndexMap* index_map = this->GetPetQualityPriceDownIndexMap(sale_pet_index.quality);
	if (NULL == index_map) return false;

	PetQualityPriceDownIndexMap::iterator begin_it;
	if (0 == sale_pet_index.pet_str_level && 0 == sale_pet_index.price && 0 == sale_pet_index.put_on_shelve_time && 0 == sale_pet_index.uid)
	{
		begin_it = index_map->begin();
	}
	else
	{
		begin_it = index_map->upper_bound(sale_pet_index);
	}

	*out_count = 0;
	for (PetQualityPriceDownIndexMap::iterator it = begin_it; it != index_map->end() && (*out_count) < req_item_per_page; ++it)
	{
		out_list[(*out_count)++] = it->second;
	}

	return true;
}

bool TradeMarketItemIndex::OnSearchPetQualityForQuickBuyByStrLevelUp(const SalePetQualityIndex& sale_pet_index, int req_item_per_page, SaleItemBase** out_list, int* out_count)
{
	PetQualityStrLevelUpIndexMap* index_map = this->GetPetQualityStrLevelUpIndexMap(sale_pet_index.quality);
	if (NULL == index_map) return false;

	PetQualityStrLevelUpIndexMap::iterator begin_it;
	if (0 == sale_pet_index.pet_str_level && 0 == sale_pet_index.price && 0 == sale_pet_index.put_on_shelve_time && 0 == sale_pet_index.uid)
	{
		begin_it = index_map->begin();
	}
	else
	{
		begin_it = index_map->upper_bound(sale_pet_index);
	}

	*out_count = 0;
	for (PetQualityStrLevelUpIndexMap::iterator it = begin_it; it != index_map->end() && (*out_count) < req_item_per_page; ++it)
	{
		out_list[(*out_count)++] = it->second;
	}

	return true;
}

bool TradeMarketItemIndex::OnSearchPetQualityForQuickBuyByStrLevelDown(const SalePetQualityIndex& sale_pet_index, int req_item_per_page, SaleItemBase** out_list, int* out_count)
{
	PetQualityStrLevelDownIndexMap* index_map = this->GetPetQualityStrLevelDownIndexMap(sale_pet_index.quality);
	if (NULL == index_map) return false;

	PetQualityStrLevelDownIndexMap::iterator begin_it;
	if (0 == sale_pet_index.pet_str_level && 0 == sale_pet_index.price && 0 == sale_pet_index.put_on_shelve_time && 0 == sale_pet_index.uid)
	{
		begin_it = index_map->begin();
	}
	else
	{
		begin_it = index_map->upper_bound(sale_pet_index);
	}

	*out_count = 0;
	for (PetQualityStrLevelDownIndexMap::iterator it = begin_it; it != index_map->end() && (*out_count) < req_item_per_page; ++it)
	{
		out_list[(*out_count)++] = it->second;
	}

	return true;
}

SaleItemIndex TradeMarketItemIndex::MakeSaleItemIndex(unsigned short item_id, int price, unsigned int put_on_shelve_time, int uid, short item_order_type, long long unique_key)
{
	static SaleItemIndex index;
	index.item_id = item_id;
	index.price = price;
	index.put_on_shelve_time = put_on_shelve_time;
	index.uid = uid;
	index.order_type = item_order_type;
	index.unique_key = unique_key;

	return index;
}

SaleTypeIndex TradeMarketItemIndex::MakeSaleTypeIndex(unsigned short item_id, int price, unsigned int put_on_shelve_time, int uid, short item_order_type, long long unique_key)
{
	static SaleTypeIndex index;
	index.item_id = item_id;
	index.price = price;
	index.put_on_shelve_time = put_on_shelve_time;
	index.uid = uid;
	index.order_type = item_order_type;
	index.unique_key = unique_key;

	return index;
}

SalePetIndex TradeMarketItemIndex::MakeSalePetIndex(ItemID item_id, int price, unsigned int put_on_shelve_time, int uid, short str_level, long long unique_key)
{
	static SalePetIndex index;
	index.item_id = item_id;
	index.price = price;
	index.put_on_shelve_time = put_on_shelve_time;
	index.uid = uid;
	index.pet_str_level = str_level;
	index.unique_key = unique_key;

	return index;
}

SalePetQualityIndex TradeMarketItemIndex::MakeSalePetQualityIndex(short pet_quality, int price, unsigned int put_on_shelve_time, int uid, short str_level, long long unique_key)
{
	static SalePetQualityIndex index;
	index.quality = pet_quality;
	index.price = price;
	index.put_on_shelve_time = put_on_shelve_time;
	index.uid = uid;
	index.pet_str_level = str_level;
	index.unique_key = unique_key;

	return index;
}

SaleMedalIndex s_medal_index;
SaleMedalIndex TradeMarketItemIndex::MakeSaleMedalIndex(short medal_grade, short medal_color, unsigned short item_id, int price, unsigned int put_on_shelve_time, int uid, short item_order_type, long long unique_key)
{
	s_medal_index.grade = medal_grade;
	s_medal_index.color = medal_color;
	s_medal_index.item_id = item_id;
	s_medal_index.price = price;
	s_medal_index.put_on_shelve_time = put_on_shelve_time;
	s_medal_index.uid = uid;
	s_medal_index.unique_key = unique_key;
	s_medal_index.order_type = item_order_type;

	return s_medal_index;
}

TradeMarketItemIndex::ItemIndexMap* TradeMarketItemIndex::GetItemIndexMap(ItemID item_id, bool is_announcing, short item_order_type)
{
	if (item_id < 0 || item_id > MAX_UINT16) return NULL;
	if (item_order_type < 0 || item_order_type >= TRADE_ITEM_ORDER_MAX) return NULL;

	if (is_announcing)
	{
		if (NULL == m_announce_item_index_map_list[item_id][item_order_type])
		{
			m_announce_item_index_map_list[item_id][item_order_type] = new ItemIndexMap();
		}

		return m_announce_item_index_map_list[item_id][item_order_type];
	}
	else
	{
		if (NULL == m_sale_item_index_map_list[item_id][item_order_type])
		{
			m_sale_item_index_map_list[item_id][item_order_type] = new ItemIndexMap();
		}

		return m_sale_item_index_map_list[item_id][item_order_type];
	}
}

TradeMarketItemIndex::TypeIndexMap* TradeMarketItemIndex::GetTypeIndexMap(short item_search_type, bool is_announcing, short item_order_type)
{
	if (item_search_type < 0 || item_search_type >= TRADE_ITEM_SEARCH_TYPE_MAX) return NULL;
	if (item_order_type < 0 || item_order_type >= TRADE_ITEM_ORDER_MAX) return NULL;

	if (is_announcing)
	{
		if (NULL == m_announce_type_index_map_list[item_search_type][item_order_type])
		{
			m_announce_type_index_map_list[item_search_type][item_order_type] = new TypeIndexMap();
		}

		return m_announce_type_index_map_list[item_search_type][item_order_type];
	}
	else
	{
		if (NULL == m_sale_type_index_map_list[item_search_type][item_order_type])
		{
			m_sale_type_index_map_list[item_search_type][item_order_type] = new TypeIndexMap();
		}

		return m_sale_type_index_map_list[item_search_type][item_order_type];
	}
}

TradeMarketItemIndex::TypeIndexMap* TradeMarketItemIndex::GetBigTypeIndexMap(short item_search_big_type, bool is_announcing, short item_order_type)
{
	if (item_search_big_type < 0 || item_search_big_type >= TRADE_ITEM_SEARCH_BIG_TYPE_MAX) return NULL;
	if (item_order_type < 0 || item_order_type >= TRADE_ITEM_ORDER_MAX) return NULL;

	if (is_announcing)
	{
		if (NULL == m_announce_big_type_index_map_list[item_search_big_type][item_order_type])
		{
			m_announce_big_type_index_map_list[item_search_big_type][item_order_type] = new TypeIndexMap();
		}

		return m_announce_big_type_index_map_list[item_search_big_type][item_order_type];
	}
	else
	{
		if (NULL == m_sale_big_type_index_map_list[item_search_big_type][item_order_type])
		{
			m_sale_big_type_index_map_list[item_search_big_type][item_order_type] = new TypeIndexMap();
		}

		return m_sale_big_type_index_map_list[item_search_big_type][item_order_type];
	}
}

TradeMarketItemIndex::TypeIndexMap* TradeMarketItemIndex::GetLevelTypeIndexMap(short level, short item_search_type, bool is_announcing, short item_order_type)
{
	if (level < 0 || level > TRADE_ITEM_LEVEL_MAX || item_search_type < 0 || item_search_type >= TRADE_ITEM_SEARCH_TYPE_MAX) return NULL;
	item_search_type = TRADEMARKET_CFG.GetItemSearchType(item_search_type);
	if (item_search_type < 0 || item_search_type >= TRADE_ITEM_LEVEL_SEARCH_TYPE_NUM) return NULL;
	if (item_order_type < 0 || item_order_type >= TRADE_ITEM_ORDER_MAX) return NULL;

	if (is_announcing)
	{
		if (NULL == m_announce_type_level_index_map_list[item_search_type][level][item_order_type])
		{
			m_announce_type_level_index_map_list[item_search_type][level][item_order_type] = new TypeIndexMap();
		}

		return m_announce_type_level_index_map_list[item_search_type][level][item_order_type];
	}
	else
	{
		if (NULL == m_sale_type_level_index_map_list[item_search_type][level][item_order_type])
		{
			m_sale_type_level_index_map_list[item_search_type][level][item_order_type] = new TypeIndexMap();
		}

		return m_sale_type_level_index_map_list[item_search_type][level][item_order_type];
	}
}

TradeMarketItemIndex::PetPriceUpIndexMap* TradeMarketItemIndex::GetPetPriceUpIndexMap(ItemID item_id)
{
	if (item_id < TRADE_PET_ITEM_ID_BEGIN || item_id >= TRADE_PET_ITEM_ID_END) return NULL;

	if (NULL == m_pet_price_up_index_map[item_id - TRADE_PET_ITEM_ID_BEGIN])
	{
		m_pet_price_up_index_map[item_id - TRADE_PET_ITEM_ID_BEGIN] = new PetPriceUpIndexMap();
	}

	return m_pet_price_up_index_map[item_id - TRADE_PET_ITEM_ID_BEGIN];
}

TradeMarketItemIndex::PetPriceDownIndexMap* TradeMarketItemIndex::GetPetPriceDownIndexMap(ItemID item_id)
{
	if (item_id < TRADE_PET_ITEM_ID_BEGIN || item_id >= TRADE_PET_ITEM_ID_END) return NULL;

	if (NULL == m_pet_price_down_index_map[item_id - TRADE_PET_ITEM_ID_BEGIN])
	{
		m_pet_price_down_index_map[item_id - TRADE_PET_ITEM_ID_BEGIN] = new PetPriceDownIndexMap();
	}

	return m_pet_price_down_index_map[item_id - TRADE_PET_ITEM_ID_BEGIN];
}

TradeMarketItemIndex::PetStrLevelUpIndexMap* TradeMarketItemIndex::GetPetStrLevelUpIndexMap(ItemID item_id)
{
	if (item_id < TRADE_PET_ITEM_ID_BEGIN || item_id >= TRADE_PET_ITEM_ID_END) return NULL;

	if (NULL == m_pet_str_level_up_index_map[item_id - TRADE_PET_ITEM_ID_BEGIN])
	{
		m_pet_str_level_up_index_map[item_id - TRADE_PET_ITEM_ID_BEGIN] = new PetStrLevelUpIndexMap();
	}

	return m_pet_str_level_up_index_map[item_id - TRADE_PET_ITEM_ID_BEGIN];
}

TradeMarketItemIndex::PetStrLevelDownIndexMap* TradeMarketItemIndex::GetPetStrLevelDownIndexMap(ItemID item_id)
{
	if (item_id < TRADE_PET_ITEM_ID_BEGIN || item_id >= TRADE_PET_ITEM_ID_END) return NULL;

	if (NULL == m_pet_str_level_down_index_map[item_id - TRADE_PET_ITEM_ID_BEGIN])
	{
		m_pet_str_level_down_index_map[item_id - TRADE_PET_ITEM_ID_BEGIN] = new PetStrLevelDownIndexMap();
	}

	return m_pet_str_level_down_index_map[item_id - TRADE_PET_ITEM_ID_BEGIN];
}

TradeMarketItemIndex::PetQualityPriceUpIndexMap* TradeMarketItemIndex::GetPetQualityPriceUpIndexMap(short quality)
{
	if (quality <= PET_QUALITY_INVALID || quality >= PET_QUALITY_MAX) return NULL;

	if (NULL == m_pet_quality_price_up_index_map[quality])
	{
		m_pet_quality_price_up_index_map[quality] = new PetQualityPriceUpIndexMap();
	}

	return m_pet_quality_price_up_index_map[quality];
}

TradeMarketItemIndex::PetQualityPriceDownIndexMap* TradeMarketItemIndex::GetPetQualityPriceDownIndexMap(short quality)
{
	if (quality <= PET_QUALITY_INVALID || quality >= PET_QUALITY_MAX) return NULL;

	if (NULL == m_pet_quality_price_down_index_map[quality])
	{
		m_pet_quality_price_down_index_map[quality] = new PetQualityPriceDownIndexMap();
	}

	return m_pet_quality_price_down_index_map[quality];
}

TradeMarketItemIndex::PetQualityStrLevelUpIndexMap* TradeMarketItemIndex::GetPetQualityStrLevelUpIndexMap(short quality)
{
	if (quality <= PET_QUALITY_INVALID || quality >= PET_QUALITY_MAX) return NULL;

	if (NULL == m_pet_quality_str_level_up_index_map[quality])
	{
		m_pet_quality_str_level_up_index_map[quality] = new PetQualityStrLevelUpIndexMap();
	}

	return m_pet_quality_str_level_up_index_map[quality];
}

TradeMarketItemIndex::PetQualityStrLevelDownIndexMap* TradeMarketItemIndex::GetPetQualityStrLevelDownIndexMap(short quality)
{
	if (quality <= PET_QUALITY_INVALID || quality >= PET_QUALITY_MAX) return NULL;

	if (NULL == m_pet_quality_str_level_down_index_map[quality])
	{
		m_pet_quality_str_level_down_index_map[quality] = new PetQualityStrLevelDownIndexMap();
	}

	return m_pet_quality_str_level_down_index_map[quality];
}

TradeMarketItemIndex::MedalGradeDownIndexMap* TradeMarketItemIndex::GetMedalGradeDownIndexMap(short color, bool is_announcing, short item_order_type)
{
	if (color < 0 || color >= ItemBase::I_COLOR_MAX) return NULL;
	if (item_order_type < 0 || item_order_type >= TRADE_ITEM_ORDER_MAX) return NULL;

	if (is_announcing)
	{
		if (NULL == m_announce_medal_grade_index_map_list[color][item_order_type])
		{
			m_announce_medal_grade_index_map_list[color][item_order_type] = new MedalGradeDownIndexMap();
		}

		return m_announce_medal_grade_index_map_list[color][item_order_type];
	}
	else
	{
		if (NULL == m_sale_medal_grade_index_map_list[color][item_order_type])
		{
			m_sale_medal_grade_index_map_list[color][item_order_type] = new MedalGradeDownIndexMap();
		}

		return m_sale_medal_grade_index_map_list[color][item_order_type];
	}
}

TradeMarketItemIndex::MedalGradeDownIndexMap* TradeMarketItemIndex::GetMedalSpecialEffectTypeIndexMap(int sp_type)
{
	if (sp_type < 0 || sp_type >= BATTLE_SPECIAL_EFFECT_TYPE_NUM) return NULL;

	if (NULL == m_sale_medal_special_effect_index_map_list[sp_type])
	{
		m_sale_medal_special_effect_index_map_list[sp_type] = new MedalGradeDownIndexMap();
	}

	return m_sale_medal_special_effect_index_map_list[sp_type];
}

TradeMarketItemIndex::MedalGradeDownIndexMap* TradeMarketItemIndex::GetMedalSpeTypeColorIndexMap(int sp_type, int color, bool is_announcing, short item_order_type)
{
	if (sp_type < 0 || sp_type >= BATTLE_SPECIAL_EFFECT_TYPE_NUM) return NULL;
	if (color < 0 || color >= ItemBase::I_COLOR_MAX) return NULL;
	if (item_order_type < 0 || item_order_type >= TRADE_ITEM_ORDER_MAX) return NULL;

	if (is_announcing)
	{
		if (NULL == m_announce_medal_spe_color_index_map_list[color][sp_type][item_order_type])
		{
			m_announce_medal_spe_color_index_map_list[color][sp_type][item_order_type] = new MedalGradeDownIndexMap();
		}

		return m_announce_medal_spe_color_index_map_list[color][sp_type][item_order_type];
	}
	else
	{
		if (NULL == m_sale_medal_spe_color_index_map_list[color][sp_type][item_order_type])
		{
			m_sale_medal_spe_color_index_map_list[color][sp_type][item_order_type] = new MedalGradeDownIndexMap();
		}

		return m_sale_medal_spe_color_index_map_list[color][sp_type][item_order_type];
	}
}

TradeMarketItemIndex::MedalGradeDownIndexMap* TradeMarketItemIndex::GetMedalProfColorIndexMap(int prof, int color, bool is_announcing, short item_order_type)
{
	if (prof < 0 || prof >= PROFESSION_TYPE_NUM) return NULL;
	if (color < 0 || color >= ItemBase::I_COLOR_MAX) return NULL;
	if (item_order_type < 0 || item_order_type >= TRADE_ITEM_ORDER_MAX) return NULL;

	if (is_announcing)
	{
		if (NULL == m_announce_medal_prof_color_index_map_list[prof][color][item_order_type])
		{
			m_announce_medal_prof_color_index_map_list[prof][color][item_order_type] = new MedalGradeDownIndexMap();
		}

		return m_announce_medal_prof_color_index_map_list[prof][color][item_order_type];
	}
	else
	{
		if (NULL == m_sale_medal_prof_color_index_map_list[prof][color][item_order_type])
		{
			m_sale_medal_prof_color_index_map_list[prof][color][item_order_type] = new MedalGradeDownIndexMap();
		}

		return m_sale_medal_prof_color_index_map_list[prof][color][item_order_type];
	}
}
