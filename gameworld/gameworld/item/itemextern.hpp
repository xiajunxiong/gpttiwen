#ifndef __ITEM_EXTERN_H__
#define __ITEM_EXTERN_H__

#include "servercommon/basedef.h"
#include "servercommon/serverdef.h"
#include "servercommon/struct/itemlistparam.h"

#include <map>

class Role;
struct ItemGridData;

class ItemExtern
{
public:
	struct ItemConsumeConfig
	{
		ItemConsumeConfig() : item_id(0), buyable(false), num(0) {}
		ItemConsumeConfig(ItemID _item_id, bool _buyable, int _num) : item_id(_item_id), buyable(_buyable), num(_num) {}

		ItemID item_id;
		bool buyable;
		int num;
	};

	struct ItemConsumeInfo
	{
		ItemConsumeInfo() { this->Reset(); }

		void Reset()
		{
			consume_bind_item_num = 0;
			consume_nobind_item_num = 0;
			memset(consume_num_list, 0, sizeof(consume_num_list));
		}

		int consume_bind_item_num;
		int consume_nobind_item_num;
		short consume_num_list[ItemNamespace::KNAPSACK_MAX_GRID_NUM];
	};

	struct BuyItemInfo
	{
		BuyItemInfo() { this->Reset(); }

		void Reset() { item_id = 0; item_count = 0; consume_gold = 0; }

		ItemID item_id;
		short item_count;
		int consume_gold;
	};

	struct ItemConsumeList
	{
		ItemConsumeList() { this->Reset(); }

		void Reset()
		{
			bind_item_num = 0;
			nobind_item_num = 0;
			need_gold_num = 0;
			cost_bindcoin_first = true;
			count = 0;
			memset(column_list, 0, sizeof(column_list));
			memset(index_list, 0, sizeof(index_list));
			memset(num_list, 0, sizeof(num_list));
			buy_count = 0;
			for (int i = 0; i < (int)(sizeof(buy_list) / sizeof(buy_list[0])); ++i)
			{
				buy_list[i].Reset();
			}
		}

		int bind_item_num;
		int nobind_item_num;
		int need_gold_num;
		bool cost_bindcoin_first;
		short count;
		short column_list[ItemNamespace::KNAPSACK_MAX_GRID_NUM];
		short index_list[ItemNamespace::KNAPSACK_MAX_GRID_NUM];
		short num_list[ItemNamespace::KNAPSACK_MAX_GRID_NUM];
		short buy_count;
		BuyItemInfo buy_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
	};

	struct ItemList
	{
		ItemList() { this->Reset(); }

		void Reset()
		{
			bind_count = 0;
			memset(bind_column_list, 0, sizeof(bind_column_list));
			memset(bind_index_list, 0, sizeof(bind_index_list));
			memset(bind_num_list, 0, sizeof(bind_num_list));

			nobind_count = 0;
			memset(nobind_column_list, 0, sizeof(nobind_column_list));
			memset(nobind_index_list, 0, sizeof(nobind_index_list));
			memset(nobind_num_list, 0, sizeof(nobind_num_list));
		}

		int GetTotalCount()
		{
			int count = 0;
			for (int i = 0; i < bind_count; ++i) count += bind_num_list[i];
			for (int i = 0; i < nobind_count; ++i) count += nobind_num_list[i];
			return count;
		}

		int GetNobindCount()
		{
			int count = 0;
			for (int i = 0; i < nobind_count; ++i) count += nobind_num_list[i];
			return count;
		}

		int GetBindCount()
		{
			int count = 0;
			for (int i = 0; i < bind_count; ++i) count += bind_num_list[i];
			return count;
		}

		short bind_count;
		short bind_column_list[ItemNamespace::KNAPSACK_MAX_GRID_NUM];
		short bind_index_list[ItemNamespace::KNAPSACK_MAX_GRID_NUM];
		short bind_num_list[ItemNamespace::KNAPSACK_MAX_GRID_NUM];

		short nobind_count;
		short nobind_column_list[ItemNamespace::KNAPSACK_MAX_GRID_NUM];
		short nobind_index_list[ItemNamespace::KNAPSACK_MAX_GRID_NUM];
		short nobind_num_list[ItemNamespace::KNAPSACK_MAX_GRID_NUM];
	};

	static bool CheckIndexListNoDuplicate(short index_list_num, short *index_list);

	static bool CheckItemByIndexList(Role *role, ItemID item_id, short check_num, short index_list_num, short *column_list, short *index_list, ItemConsumeInfo *item_check_info, short *consume_num = NULL);

	static bool PutInItemConfigList(int list_num, ItemConfigData *config_list, int *index_count, ItemID item_id, int item_num, bool is_bind);

	static int GetGoldPriceByNum(ItemID item_id, short total_num, int total_gold_price, short num);

	static bool GetConsumeListByOrder(Role *role, bool nobind_first, int stuff_num,const ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM], ItemConsumeList *result_list, bool notify_lack = true);

	static bool GetItemDataWrapperFromConfig(const ItemConfigData *item_cfg, ItemDataWrapper *item_wrapper);

private:
	static bool SelectItemByOrderHelper(ItemList *item_list, int need_count, ItemConsumeList *result_list, bool nobind_first);
};

#endif