#include "item/itemextern.hpp"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/errornum.h"
#include "item/knapsack.h"
#include "item/itempool.h"
#include "item/itembase.h"
#include "item/itemgriddata.h"
#include "obj/character/role.h"
#include "protocol/msgitem.h"
#include "engineadapter.h"
#include <set>
#include "other/rolemodulemanager.hpp"
#include "other/shop/shopconfig.h"
#include "config/logicconfigmanager.hpp"
#include "other/shop/shop.h"

bool ItemExtern::CheckIndexListNoDuplicate(short index_list_num, short *index_list)
{
	if (index_list_num <= 0) return true;
	if (NULL == index_list) return false;

	std::set<short> index_set;

	for (int i = 0; i < index_list_num; i++)
	{
		if (index_set.find(index_list[i]) != index_set.end()) return false;

		index_set.insert(index_list[i]);
	}

	return true;
}

bool ItemExtern::CheckItemByIndexList(Role *role, ItemID item_id, short check_num, short index_list_num, short* column_list, short *index_list, ItemConsumeInfo *item_consume_info, short *consume_num)
{
	if (NULL == role || ItemBase::INVALID_ITEM_ID == item_id || check_num < 0 || index_list_num <= 0 ||
		index_list_num > ItemNamespace::KNAPSACK_MAX_GRID_NUM || NULL == index_list || NULL == item_consume_info) return false;

	if (NULL != item_consume_info)
	{
		item_consume_info->consume_bind_item_num = 0;
		item_consume_info->consume_nobind_item_num = 0;
	}

	if (NULL != consume_num) *consume_num = 0;

	if (0 == check_num) return true;

	int count = 0;
	Knapsack *knapsack = role->GetRoleModuleManager()->GetKnapsack();

	for (int i = 0; i < index_list_num; ++i)
	{
		const ItemGridData* item_data = knapsack->GetItemGridData(column_list[i], index_list[i]);
		if (NULL == item_data) return false;

		if (item_id == item_data->item_id)
		{
			int valid_num = item_data->num - item_consume_info->consume_num_list[i];		// consume_num_list已记录了消耗信息
			if (valid_num > 0)
			{
				if (count + valid_num > check_num) valid_num = check_num - count;

				count += valid_num;
				item_consume_info->consume_num_list[i] += valid_num;

				if (item_data->is_bind)
				{
					item_consume_info->consume_bind_item_num += valid_num;
				}
				else
				{
					item_consume_info->consume_nobind_item_num += valid_num;
				}

				if (NULL != consume_num) *consume_num = count;

				if (count >= check_num) return true;
			}
		}
	}

	return false;
}

bool ItemExtern::PutInItemConfigList(int list_num, ItemConfigData *config_list, int *index_count, ItemID item_id, int item_num, bool is_bind)
{
	if (list_num <= 0 || NULL == config_list || NULL == index_count || *index_count > list_num || ItemBase::INVALID_ITEM_ID == item_id || item_num <= 0) return false;

	const ItemBase *itembase = ITEMPOOL->GetItem(item_id);
	if (NULL == itembase) return false;

	if (!itembase->CanMerge())
	{
		if (1 != item_num) return false;
	}
	else
	{
		for (int k = 0; k < *index_count; ++k)
		{
			if (config_list[k].item_id == item_id && ((0 != config_list[k].is_bind) == is_bind))
			{
				config_list[k].num += item_num;
				if (config_list[k].num > itembase->GetPileLimit())
				{
					item_num = config_list[k].num - itembase->GetPileLimit();
					config_list[k].num = itembase->GetPileLimit();
				}
				else
				{
					item_num = 0;
					break;
				}
			}
		}
	}

	if (item_num > 0)
	{
		if (*index_count >= list_num) return false;

		config_list[*index_count].item_id = item_id;
		config_list[*index_count].num = item_num;
		config_list[*index_count].is_bind = is_bind;

		++(*index_count);
	}

	return true;
}

int ItemExtern::GetGoldPriceByNum(ItemID item_id, short total_num, int total_gold_price, short num)
{
	if (total_num <= 0 || total_gold_price <= 0 || num <= 0) return 0;

	int unit_gold_price = 0;
	if (unit_gold_price <= 0) unit_gold_price = total_gold_price;

	int ret_gold_price = num * unit_gold_price;
	if (ret_gold_price > total_gold_price) ret_gold_price = total_gold_price;

	return ret_gold_price;
}

bool ItemExtern::GetConsumeListByOrder(Role *role, bool nobind_first, int stuff_num, const ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM], ItemConsumeList *result_list, bool notify_lack)
{
	/************************************************************************/
	/* 规则如下：
	如果非绑优先(nobind_first)
	先扣非绑再扣绑定
	如果绑定优先
	则优先扣绑定再扣非绑*/
	/************************************************************************/

	if (NULL == role || stuff_num <= 0 || stuff_num > ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM || 
		NULL == stuff_list || NULL == result_list) return false;

	int tmp_consume_num = stuff_num;
	ItemConsumeConfig tmp_consume_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
	for (int i = 0; i < tmp_consume_num && i < ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM; ++i)
	{
		if (stuff_list[i].num <= 0) return false;

		tmp_consume_list[i] = stuff_list[i];
	}

	static ItemList item_list_array[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
	for (int i = 0; i < ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM; ++i)
	{
		item_list_array[i].Reset();
	}

	result_list->Reset();

	Knapsack *knapsack = role->GetRoleModuleManager()->GetKnapsack();

	{
		// 获取每种物品的绑定列表和非绑定列表
		for (int i = 0;i < tmp_consume_num && i < ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM; ++i)
		{
			if (!knapsack->GetListOfItem(tmp_consume_list[i].item_id, ItemNamespace::KNAPSACK_MAX_GRID_NUM,
				item_list_array[i].bind_column_list, item_list_array[i].bind_index_list, item_list_array[i].bind_num_list, &(item_list_array[i].bind_count),
				item_list_array[i].nobind_column_list, item_list_array[i].nobind_index_list, item_list_array[i].nobind_num_list, &(item_list_array[i].nobind_count)))
			{
				return false;
			}
		}
	}

	{
		// 检查每种物品总量够不够
		for (int i = 0; i < tmp_consume_num && i < ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM; ++i)
		{
			int count = item_list_array[i].GetTotalCount();
			if (count < tmp_consume_list[i].num)
			{
				if (!tmp_consume_list[i].buyable)						// 不能用元宝的就直接失败
				{
					if (notify_lack)
					{
						Protocol::SCLackItem li;
						li.item_id = tmp_consume_list[i].item_id;
						li.item_count = tmp_consume_list[i].num - count;
						EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&li, sizeof(li));
					}

					return false;			// 不能用元宝的就直接失败
				}

				// 换算成元宝					
				int auto_buy_num = tmp_consume_list[i].num - count;
				tmp_consume_list[i].num = count;	
				int price = 0;
				if (!role->GetRoleModuleManager()->GetShop()->GetFastBuyGoldUnitPrice(tmp_consume_list[i].item_id, &price))
				{
					if (notify_lack)
					{
						Protocol::SCLackItem li;
						li.item_id = tmp_consume_list[i].item_id;
						li.item_count = auto_buy_num;
						EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&li, sizeof(li));
					}

					return false;
				}

				result_list->need_gold_num += (price * auto_buy_num);

				if (result_list->buy_count < (int)(sizeof(result_list->buy_list) / sizeof(result_list->buy_list[0])))
				{
					result_list->buy_list[result_list->buy_count].item_id = tmp_consume_list[i].item_id;
					result_list->buy_list[result_list->buy_count].item_count = auto_buy_num;
					result_list->buy_list[result_list->buy_count].consume_gold = (price * auto_buy_num);
					++ result_list->buy_count;
				}
			}
		}
	}

	for (int i = 0; i < tmp_consume_num && i < ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM; ++i)
	{
		if (result_list->count >= ItemNamespace::KNAPSACK_MAX_GRID_NUM) break;

		if (!SelectItemByOrderHelper(&item_list_array[i], tmp_consume_list[i].num, result_list, nobind_first)) return false;				
	}

	return true;
}

bool ItemExtern::GetItemDataWrapperFromConfig(const ItemConfigData *item_cfg, ItemDataWrapper *item_wrapper)
{
	if (NULL == item_cfg || NULL == item_wrapper) return false;

	const ItemBase *itembase = ITEMPOOL->GetItem(item_cfg->item_id);
	if (NULL == itembase) return false;

	item_wrapper->item_id = item_cfg->item_id;
	item_wrapper->num = item_cfg->num;
	item_wrapper->is_bind = item_cfg->is_bind ? 1 : 0;
	item_wrapper->invalid_time = itembase->CalInvalidTime();

	return true;
}

bool ItemExtern::SelectItemByOrderHelper(ItemList *item_list, int need_count, ItemConsumeList *result_list, bool nobind_first)
{
	if (NULL == item_list || NULL == result_list || need_count < 0) return false;

	if (need_count == 0) return true;

	int have_nobind_count = item_list->GetNobindCount();
	int have_bind_count = item_list->GetBindCount();

	if (have_bind_count + have_nobind_count < need_count) return false;

	int nobind_need_count = 0;
	int bind_need_count = 0;

	if (nobind_first)
	{
		// 非绑定优先
		if (have_nobind_count >= need_count)
		{
			nobind_need_count = need_count;
			bind_need_count = 0;
		}
		else
		{
			nobind_need_count = have_nobind_count;
			bind_need_count = need_count - have_nobind_count;
		}
	}
	else
	{
		// 绑定优先
		if (have_bind_count >= need_count)
		{
			bind_need_count = need_count;
			nobind_need_count = 0;
		}
		else
		{
			bind_need_count = have_bind_count;
			nobind_need_count = need_count - have_bind_count;
		}
	}

	{
		// 查找非绑
		int now_nobind_count = 0;
		for (int i = 0; i < item_list->nobind_count && nobind_need_count > 0; ++i)	// 遍历这个物品的非绑列表
		{
			if (result_list->count >= ItemNamespace::KNAPSACK_MAX_GRID_NUM) break;

			if (now_nobind_count + item_list->nobind_num_list[i] < nobind_need_count)
			{
				result_list->column_list[result_list->count] = item_list->nobind_column_list[i];
				result_list->index_list[result_list->count] = item_list->nobind_index_list[i];
				result_list->num_list[result_list->count] = item_list->nobind_num_list[i];
				++ result_list->count;

				now_nobind_count += item_list->nobind_num_list[i];
			}
			else
			{
				result_list->column_list[result_list->count] = item_list->nobind_column_list[i];
				result_list->index_list[result_list->count] = item_list->nobind_index_list[i];
				result_list->num_list[result_list->count] = nobind_need_count - now_nobind_count;
				++ result_list->count;

				now_nobind_count = nobind_need_count;
			}

			if (now_nobind_count >= nobind_need_count) break;
		}

		result_list->nobind_item_num += now_nobind_count;
	}

	{
		// 查找绑定
		int now_bind_count = 0;
		for (int i = 0; i < item_list->bind_count && bind_need_count > 0; ++i)	// 遍历这个物品的绑定列表
		{
			if (result_list->count >= ItemNamespace::KNAPSACK_MAX_GRID_NUM) break;

			if (now_bind_count + item_list->bind_num_list[i] < bind_need_count)
			{
				result_list->column_list[result_list->count] = item_list->bind_column_list[i];
				result_list->index_list[result_list->count] = item_list->bind_index_list[i];
				result_list->num_list[result_list->count] = item_list->bind_num_list[i];
				++ result_list->count;

				now_bind_count += item_list->bind_num_list[i];
			}
			else
			{
				result_list->column_list[result_list->count] = item_list->bind_column_list[i];
				result_list->index_list[result_list->count] = item_list->bind_index_list[i];
				result_list->num_list[result_list->count] = bind_need_count - now_bind_count;
				++ result_list->count;

				now_bind_count = bind_need_count;
			}

			if (now_bind_count >= bind_need_count) break;
		}

		result_list->bind_item_num += now_bind_count;
	}

	return true;
}
