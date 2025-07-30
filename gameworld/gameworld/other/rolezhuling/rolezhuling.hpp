#ifndef __ROLE_ZHU_LING_HPP__
#define __ROLE_ZHU_LING_HPP__

#include "servercommon/medaldef.h"
#include "other/attributelist.hpp"
#include "item/itemgriddata.h"
#include "protocol/msgzhuling.h"
#include <set>

class RoleModuleManager;
class Role;

enum ZHU_LING_ITEM_SRC
{
	ZHU_LING_ITEM_SRC_KNAPSACK,			//背包里
	ZHU_LING_ITEM_SRC_EQUIP,				//装备里
	ZHU_LING_ITEM_SRC_LING_SHI,			//灵饰里
};

struct ZhuLinParamRet 
{
	ZhuLinParamRet()
	{
		is_ret_succ = false;
		eq_item_id = 0;
		item_id = 0;
		is_zhu_ling = false;
		param = NULL;		
	}

	bool is_ret_succ;
	ItemID eq_item_id;   //没什么用，用来记录log
	ItemID item_id;
	bool is_zhu_ling;
	ZhuLinParam * param;
};


class RoleZhuLing
{
public:
	RoleZhuLing();
	~RoleZhuLing();

	void Init(RoleModuleManager * mgr);

	int UpLevelZhuLing(int item_src, int index, int column, int slot_index, int need_buy_base_num, int count, unsigned short * item_list);
	int PutOn(int item_src, int index, int column, int slot_index, int other_item_src, int  other_index, int other_column, int other_slot_index);
	int TakeOff(int item_src, int index, int column, int slot_index);
	int Decompose(int count, Protocol::CSZhuLingDecomposeReq::Item * item_list);

	void* operator new(size_t c);
	void operator delete(void* m);

	const ItemGridData * GetItemGridData(int item_src, int index, int column) const;
	int GetZhuLinItemId(ItemGridData * grid_data, int slot_index);
	ZhuLinParamRet GetZhuLinParam(const ItemGridData * grid_data, int slot_index, bool is_need_equipment = false);
	int SetItemDirty(int item_src, int index, int column);
private:
	bool IsTargetAndSourceSame(int item_src, int index, int column, int slot_index, int other_item_src, int  other_index, int other_column, int other_slot_index);

	RoleModuleManager * m_role_module_mgr;
};

#endif