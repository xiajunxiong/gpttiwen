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


class Medal				//ѫ��
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

	//��ȡ������Ч����
	int GetSpeScore();
	//�һ���������
	void ExcahngFbMaterial(int seq);
	//�ֽ�ѫ��
	void DecomposeMedal(int column, int index);
	//ʹ�ø���ʯ
	bool UseAddSpItem(int sp_id, int column, int index, bool is_wear_grid);
	//�齱ѫ��
	void OnlottreyMedal(int seq,int times);
	//ѫ�½���
	int OnMedalUpGrade(int column, int index, int consume_column, int consume_index, bool is_wear_grid);
	//�����������
	void OnSpeMedalUpGrade(bool is_wear_grid, int column, int index, int consume_column_spe, int consume_index_spe, int consume_column, int consume_index);
	//��������û�
	void OnSpeMedalExchange(int column, int index, int sp_id, int second_sp_id, bool is_wear);
	//ѫ��һ������
	void OnOneKeyMedalUpGrade(int count, int column, BitMap<MEDAL_ONE_KEY_GRADE_MAXNUM> index_flag);
	
	int GetMedalNum();//��ȡ��������
	int GetLowMedalColorOnWear();//��ȡ������Ʒ�����
	int GetCurWearNumByColor(int color); //��ȡ��ʱ������Ʒ�ʵ�ѫ������
	int GetCurWearNumByGrade(int grade); //��ȡ��ʱ�������ڵ��ڸý�����ѫ������

	//����
	void SynHuaShenMedal(std::set<long long> * item_set = NULL);		//ͬ�����𴩴���ѫ�µ�����
	void SynHuaShenMedalGrid(bool is_check = false);					//ͬ�����𴩴���ѫ�µ�����װ����

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