#ifndef __ZAO_HUA_EQUIP_MANAGER_HPP__
#define __ZAO_HUA_EQUIP_MANAGER_HPP__

#include "other/attributelist.hpp"
#include "servercommon/struct/zaohuaparam.hpp"
#include "other/rolemodulemanager.hpp"
#include "obj/character/role.h"
#include "item/zaohua/zaohuaitem.hpp"
#include "protocol/msgequipment.h"
#include "servercommon/zaohuaequipdef.hpp"
#include "zaohuaequipconfig.hpp"
#include "item/knapsack.h"
#include <set>

struct ZaoHuaSortStruct
{
	void Set(long long _prior_id, bool _is_bind, int _src_index)
	{
		prior_id = _prior_id;
		is_bind = _is_bind;
		src_index = _src_index;
	}

	static long long GetSupplyPriorId(short _item_level, short _item_type, short _item_sub_type, short _item_color, short _equip_type, ItemID _item_id, short _param)
	{
		static const long long BASE_PRIOR = 10000000000000000LL; //9223372036854775807最大值
																 // sub_type > color > level > param > type > item_id
		static const long long SUB_TYPE_PRIOR = BASE_PRIOR / 1000;
		static const long long COLOR_PRIOR = SUB_TYPE_PRIOR / 100;
		static const long long EQUIP_LEVEL_PRIOR = COLOR_PRIOR / 1000;
		static const long long PARAM_PRIOR = EQUIP_LEVEL_PRIOR / 100;
		static const long long EQUIP_TYPE_PRIOR = PARAM_PRIOR / 10;


		UNSTD_STATIC_CHECK(ItemBase::I_COLOR_MAX <= 11);
		UNSTD_STATIC_CHECK(ZaoHuaItem::Z_TYPE_MAX < 10);
		UNSTD_STATIC_CHECK(ItemNamespace::ITEM_SUB_TYPE_MAX < 100);
		UNSTD_STATIC_CHECK(PARAM_PRIOR > MAX_UINT16);


		return _item_level * EQUIP_LEVEL_PRIOR + EQUIP_TYPE_PRIOR * _equip_type + _item_color * COLOR_PRIOR + _param * PARAM_PRIOR + _item_id;
	}

	bool is_bind;
	int src_index;

	long long prior_id;
};
int ZaoHuaSortCompare(const void *arg1, const void *arg2);

struct ReCalcSuit
{
	ReCalcSuit() : color(0), level(0) {}
	void Reset()
	{
		color = 0;
		level = 0;
	}
	int color;
	int level;
};

struct ReCalcSuitList
{
	ReCalcSuitList() : suit_id(0), num(0) {}
	void Reset()
	{
		suit_id = 0;
		num = 0;
		for (int i = 0; i < ZaoHuaItem::Z_TYPE_MAX; i++)
		{
			suit_list[i].Reset();
		}
	}
	void AddSuit(int new_suit_id, ReCalcSuit * new_suit)
	{
		if (suit_id == 0)suit_id = new_suit_id;
		//检查新加入的品质
		if (num > 0)
		{
			ReCalcSuit change_node; change_node.Reset();
			ReCalcSuit push_node; push_node.Reset();
			push_node = *new_suit;
			for (int i = num; i >= 0 && i <= ZaoHuaItem::Z_TYPE_MAX; i--)
			{
				//与前一个比较，比他小则插入新位置，大则替代他，并且把他移动到新位置
				if (i == 0)
				{
					suit_list[i].color = push_node.color;
					suit_list[i].level = push_node.level;
					break;//到这里的，说明前一位已经被替代了，直接放入
				}
				if (suit_list[i - 1].color < new_suit->color || (suit_list[i - 1].color == new_suit->color && suit_list[i - 1].level < new_suit->level))
				{
					change_node.color = suit_list[i - 1].color;
					change_node.level = suit_list[i - 1].level;

					suit_list[i - 1].color = push_node.color;
					suit_list[i - 1].level = push_node.level;

					suit_list[i].color = change_node.color;
					suit_list[i].level = change_node.level;
				}
				else
				{
					suit_list[i].color = push_node.color;
					suit_list[i].level = push_node.level;
					break;//放置之后，不需后续遍历了
				}
			}
		}
		else
		{
			suit_list[num].color = new_suit->color;
			suit_list[num].level = new_suit->level;
		}
		num++;
	}
	int suit_id;
	int num;
	ReCalcSuit suit_list[ZaoHuaItem::Z_TYPE_MAX];
};

struct SuitEffectReCalc3//装备套装计算--排序数3
{
	void Reset()
	{
		suit_type_num = 0;
		for (int i = 0; i < ZaoHuaItem::Z_TYPE_MAX; i++)
		{
			suit_type_list[i].Reset();
		}
	}
	void AddSuit(int suit_id, ReCalcSuit * new_suit)
	{
		bool is_new = true;
		int back_idx = -1;
		for (int suit_type_idx = 0; suit_type_idx < suit_type_num && suit_type_idx < ZaoHuaItem::Z_TYPE_MAX; suit_type_idx++)
		{
			ReCalcSuitList &node = suit_type_list[suit_type_idx];
			if (node.suit_id == suit_id)
			{
				is_new = false;
				back_idx = suit_type_idx;
			}
		}
		if (is_new)suit_type_list[suit_type_num++].AddSuit(suit_id, new_suit);
		else suit_type_list[back_idx].AddSuit(suit_id, new_suit);
	}
	int suit_type_num;
	ReCalcSuitList suit_type_list[ZaoHuaItem::Z_TYPE_MAX];
};

enum  YUAN_HUN_ZHU_ITEM_SRC
{
	YUAN_HUN_ZHU_ITEM_SRC_KNAPSACK,			//背包里
	YUAN_HUN_ZHU_ITEM_SRC_WEAR_ZAO_HUA_EQUIP,				//装备里
	YUAN_HUN_ZHU_ITEM_SRC_ZAO_HUA_EQUIP_KNAPSACK,				//造化背包
};

enum ZAO_HUA_SEND_REASON
{
	ZAO_HUA_SEND_REASON_NORMAL = 0,				//!< 0 正常下发（上线或客户端请求）			
	ZAO_HUA_SEND_REASON_IN = 1,					//!< 1 外部获得飘字
	ZAO_HUA_SEND_REASON_OUT = 2,				//!< 2 内部消耗
	ZAO_HUA_SEND_REASON_MOVE = 3,				//!< 3 内部移动
	ZAO_HUA_SEND_REASON_CLEARUP = 4,			//!< 4 整理
};

struct YuanHunZhuParamRet
{
	YuanHunZhuParamRet()
	{
		is_ret_succ = false;
		eq_item_id = 0;
		item_id = 0;
		is_yuan_hun_zhu = false;
		param = NULL;
	}

	bool is_ret_succ;
	ItemID eq_item_id;   //没什么用，用来记录log
	ItemID item_id;
	bool is_yuan_hun_zhu;
	YuanHunZhuParam * param;
};

class Role;
class ZaoHuaEquipManager;

class ZaoHuaEquipManager
{
public:
	ZaoHuaEquipManager();
	~ZaoHuaEquipManager();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * role_module_manager,const ZaoHuaRefineParam & refine_param,const ZaoHuaLongZhuParam & long_zhu_param, int equip_list_num, const ItemListParam::ItemListData * equip_list[]);
	void GetInitParam(ZaoHuaRefineParam * refine_param, ZaoHuaLongZhuParam * long_zhu_param);
	void OnLogin();

	void ChangeProfession(int old_prof, int new_prof);

	bool GetZaoHuaEquipGridData(short equip_index, ItemGridData *equip_data);//装备栏
	bool GetZaoHuaEquipGridData(short equip_index, ItemDataWrapper *equip_data);//装备栏

	const AttributeList & ReCalcAttr(AttributeList & base_add, bool is_recalc);

	void GetZaoHuaEquipWearList(int *count, ItemListParam::ItemListData *save_list);
	void GetZaoHuaEquipBagList(int *count, ItemListParam::ItemListData *save_list);

	void GetChangeZaoHuaEquipWearList(int *count, ItemListParam::ItemListData *save_list);
	void GetChangeZaoHuaEquipBagList(int *count, ItemListParam::ItemListData *save_list);

	void OperatorMsg(Protocol::CSZaoHuaEquipmentOperaReq * msg);
	void ComposeItem(int compose_seq, bool is_put_on,int index);
	void DisCompose(int index);
	void DisComposeList(Protocol::CSZaoHuaEquipmentDiscomposeReq * msg);

	int GetVigorous() { return m_role_module_mgr->GetCommonData().zao_hua_wear_app_id; }
	void ActiveSuitApp(int app_id);
	void WearSuitApp(int app_id);
	void TaskOffSuitApp();

	void PutOnEquip(int index);
	bool TakeOffEquip(int index);

	/*背包操作*/
	int GetEmptyBag();//背包空闲格子数
	bool HasBagNum(int grid_num);
	bool AddNewZaoHuaItem(const ItemDataWrapper * new_item, int *out_idx = NULL);//放入一个造化装备（已存在的）
	bool AddNewZaoHuaItem(const ItemGridData * new_item, int *out_idx = NULL);//放入一个造化装备（已存在的）
	const ItemGridData * AddZaoHuaItem(ItemID new_id,bool is_from_compose = false, int *out_idx = NULL);
	ItemGridData * GetItemGridData(int index);
	ItemGridData * GetItemGridDataKnapsack(int index);	

	void ClearAll();
	bool ConsumeItem(int consume_idx, const char * reason);
	bool InOrder();

	void SetZaoHuaEquipGridDirty(short equip_index, bool to_notice_client = true);
	void SetZaoHuaBagDirty(short equip_index,short reason_type = ZAO_HUA_SEND_REASON_NORMAL, short put_reason = PUT_REASON_NO_NOTICE, bool to_notice_client = true);
	
	void ClearDirtyMark();
	void ClearWearDirtyMark();

	void GetUserSpecialEffectList(int max_other_sp_num, short* out_other_sp_num, int* out_other_sp_list);

	void ClearWearGrid(int index);
	void ClearGrid(int index);

	void SendSingleBagInfo(int index, short reason_type, short put_reason, bool is_notice = true);
	void SendWearChange(int index);
	void SendComposeResult(bool is_succ, const ItemDataWrapper & get_item);
	void SendBagList(int reason = ZAO_HUA_SEND_REASON_NORMAL);
	void SendWearList();
	void SendWearApp(); 
	void SendCommonInfo();
	ItemGridData * GetItemGridData(int item_src, int index, int column);
	ZaoHuaEquipParam * GetZaoHuaEquipParam(int item_src, int index, int column = 0, int * ARG_OUT item_id_ptr = NULL, long long * ARG_OUT item__uuid = NULL);
	ZaoHuaEquipParam * GetZaoHuaEquipParamByEquipSlotIndex(int equip_slot_index, int ARG_OUT * item_id_ptr = NULL);
	int SetItemDirty(int item_src, int index, int column);
	const ZaoHuaItem * GetZaoHuaItem(int item_id);

	//精炼
	int Refine(int equip_slot_index);
	int GetRefineResonanceLevel();
	ZaoHuaRefineItemParam * GetZaoHuaRefineItemParam(int equip_slot_index);
	void SendRefineInfo();

	//血祭，魂力升级
	void HunLiUplv(int item_src, int index, int column);

	//元魂珠操作
	int YuanHunZhuUpLevel(int item_src, int index, int column, int slot_index);
	int YuanHunZhuDecompose(int index, int column);
	int YuanHunZhuCompose(int item_id);
	int YuanHunZhuPutOn(int item_src, int index, int column, int slot_index, int other_item_src, int  other_index, int other_column, int other_slot_index);
	int YuanHunZhuTakeOff(int item_src, int index, int column, int slot_index);
	YuanHunZhuParamRet YuanHunZhuGetParam(const ItemGridData * grid_data, int slot_index, bool is_need_equipment = false);

	//珑铸
	int LongZhuDraw(int equip_index, int slot_index, int draw_times);
	int LongZhuPutOn(int equip_index, int slot_index, int ci_tiao_index);
	int LongZhuClear(int equip_index, int slot_index, int ci_tiao_index, bool is_clear_all = false);
	int LongZhuUnlockSlot(int equip_index, int slot_index);
	ZaoHuaLongZhuSlotParam * GetZaoHuaLongZhuSlotParam(int equip_index, int slot_index);
	ZaoHuaLongZhuPartParam * GetZaoHuaLongZhuPartParam(int equip_index);
	void LongZhuSendInfo();
	void LongZhuSendSlotInfo(int equip_index, int slot_index);

	//入魔
	int RuMo(int item_src, int index);

	//战斗特效
	void CheckAddItem(const ItemBase * item_base);
	void GetEquipSpecialEffect(int max_other_sp_num, short * out_other_sp_num, int * out_other_sp_list);
	void GetPotentialAdd(int potential_add[ZaoHuaSuitCfg::ZAO_HUA_POTENTIAL_TYPE_MAX]);
	int GetWearZaoHuaLevelCount() { return m_wear_level; }
private:
	RoleModuleManager * m_role_module_mgr;
	AttributeList	m_attrs_add;
	int m_wear_level;
	int m_capability;
	unsigned int m_next_order_tamp;
	int m_potential_add[ZaoHuaSuitCfg::ZAO_HUA_POTENTIAL_TYPE_MAX];
	const ItemGridData* GetBagNode(int bag_idx) const;
	int				m_wear_grid_num;
	bool			m_wear_is_change;
	bool			m_wear_old_state[ZaoHuaItem::Z_TYPE_MAX];
	bool			m_wear_dirty_mark[ZaoHuaItem::Z_TYPE_MAX];
	ItemGridData*	m_wear_grid_list[ZaoHuaItem::Z_TYPE_MAX];

	int				m_grid_num;
	bool			m_bag_is_change;
	bool			m_old_state[ItemNamespace::MAX_ZAO_HUA_BAG_NUM];
	bool			m_dirty_mark[ItemNamespace::MAX_ZAO_HUA_BAG_NUM];
	ItemGridData*	m_grid_list[ItemNamespace::MAX_ZAO_HUA_BAG_NUM];


	ZaoHuaRefineParam m_refine_param;
	ZaoHuaLongZhuParam m_long_zhu_param;

	std::set<int> m_sp_id_list;  //缓存ReCalcAttr 得到spid,目前时元魂珠
};


#endif // !__FABAO_HPP__
