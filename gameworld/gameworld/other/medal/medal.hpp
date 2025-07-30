#ifndef __MEDAL_HPP__
#define __MEDAL_HPP__

#include "servercommon/medaldef.h"
#include "other/attributelist.hpp"
#include "item/itemgriddata.h"
#include <set>

class RoleModuleManager;
class Role;

struct MedalSpPriority
{
	MedalSpPriority()
	{
		sp_id = 0;
		sp_type = 0;
		sp_priority = 0;
		prof_type = 0;
	}

	bool operator<(const MedalSpPriority & other) const
	{
		if (sp_type < other.sp_type)
			return true;
		if (sp_type > other.sp_type)
			return false;

		if (sp_priority > other.sp_priority)
			return true;
		if (sp_priority < other.sp_priority)
			return false;

		if (sp_id < other.sp_id)
			return true;
		if (sp_id > other.sp_id)
			return false;

		return false;
	}

	int sp_id;
	int sp_type;
	int sp_priority;
	int prof_type;
};


class Medal				//勋章
{
public:
	Medal();
	~Medal();

	void Init(RoleModuleManager * mgr, const MedalCommonParam & param, const std::vector<const ItemListParam::ItemListData * >& medal_item_list);
	void GetInitParam(MedalCommonParam * param);
	void GetMedalList(int * count, QueryItemListParam::ItemListData * save_list);
	void GetChangeMedalItemList(int * count, ItemListParam::ItemListData * save_list); 
	void ClearDirtyMark();

	const AttributeList & ReCalcAttr(AttributeList &base_add, bool is_recalc);

	void Update(unsigned long interval, time_t now_second);

	void SendInfo();
	void PutOn(int wear_index, int column, int index);
	void TakeOff(int wear_index);

	bool GetMedalGridData(short put_on_index, ItemDataWrapper & carrier_data);

	//获取令牌特效评分
	int GetSpeScore();
	//兑换副本材料
	void ExcahngFbMaterial(int seq);
	//分解勋章
	void DecomposeMedal(int column, int index);
	//使用附灵石
	bool UseAddSpItem(int sp_id, int column, int index, bool is_wear_grid);
	//抽奖勋章
	void OnlottreyMedal(int seq,int times);
	//勋章进阶
	int OnMedalUpGrade(int column, int index, int consume_column, int consume_index, bool is_wear_grid);
	//天赐令牌升华
	void OnSpeMedalUpGrade(bool is_wear_grid, int column, int index, int consume_column_spe, int consume_index_spe, int consume_column, int consume_index);
	//天赐令牌置换
	void OnSpeMedalExchange(int column, int index, int sp_id, int second_sp_id, bool is_wear);
	//勋章一键进阶
	void OnOneKeyMedalUpGrade(int count, int column, BitMap<MEDAL_ONE_KEY_GRADE_MAXNUM> index_flag);
	
	int GetMedalNum();//获取穿戴数量
	int GetLowMedalColorOnWear();//获取穿戴中品质最低
	int GetCurWearNumByColor(int color); //获取此时穿戴该品质的勋章数量
	int GetCurWearNumByGrade(int grade); //获取此时穿戴大于等于该阶数的勋章数量

	//化身
	void SynHuaShenMedal(std::set<long long> * item_set = NULL);		//同步本尊穿戴的勋章到化身
	void SynHuaShenMedalGrid(bool is_check = false);					//同步本尊穿戴的勋章到化身装备栏

	//----
	void GetBattleDate(RoleBattleData * role_data);
	std::set<int> GetEffectSp(std::set<MedalSpPriority> * out_sp_id_set = NULL);

	void* operator new(size_t c);
	void operator delete(void* m);
private:
	RoleModuleManager * m_role_module_mgr;

	MedalCommonParam m_param;

	AttributeList m_attrs_add;

	bool m_medal_old_state[MAX_MEDAL_WEAR_GRID];
	ItemGridData m_wear_medal_grid[MAX_MEDAL_WEAR_GRID];
};

#endif