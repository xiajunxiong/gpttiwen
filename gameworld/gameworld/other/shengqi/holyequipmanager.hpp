#ifndef __HOLY_EUQIP_HPP__
#define __HOLY_EUQIP_HPP__

#include "servercommon/holyequipdef.h"
#include "protocol/msgholyequip.h"
#include "other/attributelist.hpp"

enum HOLY_EQUIP_SEND_REASON
{
	HOLY_EQUIP_SEND_REASON_NORMAL = 0,			//!< 0 �����·������߻�ͻ������󣩣���Ʈ��
	HOLY_EQUIP_SEND_REASON_IN = 1,				//!< 1 �ⲿ���Ʈ��
	HOLY_EQUIP_SEND_REASON_HOLY_LVUP = 2,		//!< 2 ʥ������
	HOLY_EQUIP_SEND_REASON_MOVE = 3,			//!< 3 �������ݸı�	��ʯ/ʥ��
	HOLY_EQUIP_SEND_REASON_STATUS = 4,			//!< 4 ʥ��״̬�ı�
	HOLY_EQUIP_SEND_REASON_FUSHI_FUSING = 5,	//!< 5 ��ʯ�ϳ�
	HOLY_EQUIP_SEND_REASON_FUSHI_REACST = 6,	//!< 6 ��ʯ����
	HOLY_EQUIP_SEND_REASON_FUSHI_INLAY = 7,		//!< 7 ��ʯ��Ƕ
	HOLY_EQUIP_SEND_REASON_XUN_BAO = 8,			//!< 8 Ѱ����ã���Ʈ��(Ѱ���Ѿ����ɵ���)
	HOLY_EQUIP_SEND_REASON_BLESS_PET = 9,		//!< 9 ʥ�ӳ���
	HOLY_EQUIP_SEND_REASON_SKILL_STUDY = 10,	//!< 10 ������ѧϰ
	HOLY_EQUIP_SEND_REASON_SALE_FU_SHI = 11,	//!< 11 �г����۷�ʯ
};

static const int MAX_HOLY_EQUIP_FIGHT_STANDBY_NUM = 5;		// ��߳�ս�ʹ���ʥ������
UNSTD_STATIC_CHECK(MAX_HOLY_EQUIP_FIGHT_STANDBY_NUM == HOLY_EQUIP_STATUS_MAX);	// �����Ա�,�ı�Ļ���ɫ������Ϣ���holy_equip_status_indexҲ��Ҫ�������

class Role;
class RoleModuleManager;

class HolyEquipManager
{
	enum LOG_REASON
	{
		LOG_REASON_STATE_CHANGE = 0,	// ʥ����ս/����/��Ϣ״̬�ı�
		LOG_REASON_LEVEL_UP = 1,
		LOG_REASON_PUT_ON_FU_SHI = 2,
		LOG_REASON_TAKE_OFF_FU_SHI = 3,
	};

public:
	HolyEquipManager();
	~HolyEquipManager();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * mgr, const HolyEquipCommonParam & param, int shengqi_param_list_num, const ItemListParam::ItemListData * shengqi_data_list[],
																		   int fushi_param_list_num, const ItemListParam::ItemListData * fushi_data_list[]);
	void GetInitParam(HolyEquipCommonParam * param);
	// ����ʥ����������
	void GetChangeHolyEquipBagList(int *count, ItemListParam::ItemListData *save_list);
	// �����ʯ��������
	void GetChangeFuShiBagList(int *count, ItemListParam::ItemListData *save_list);

	void GetHolyEquipList(int *count, QueryItemListParam::ItemListData *save_list);

	void ClearHolyDirtyMark();
	void ClearFuShiDirtyMark();
	
	// תΪItemDataWrapper, ������
	bool GetHolyEquipGridData(short index, ItemDataWrapper & carrier_data);

	// �Ƿ���ʥ�ӳ���
	bool HasBlessPet(int index);

	// ����ʥ������
	void ReCalHolyBaseAttr(int index, bool is_send = false);
	void ReCalAllHolyBaseAttr();

	// �������ɫ���ӵ�����
	const AttributeList & ReCalcAttr(AttributeList &base_add, bool is_recalc);

	// תְ
	void ChangeProfession(int old_prof, int new_prof);

	// ������������ӵ�����
	void ReCalcPetAttr(unsigned long long pet_unique_id, AttributeList &base_add);
	void RefreshPetAttr();

	// �ͻ�������
	void ClientReq(Protocol::CSHolyEquipOpReq * msg);


	void OnRoleLogin();

	// ��ȡ��սʥ��, out_index���ص��ǽ�ɫͨ����Ϣholy_equip_stat_index���±�
	const ItemGridData* GetFightHolyEquip(int * out_index = NULL);

	// ��ȡ������Ϊ����/��ս״̬ʥ��λ�õ��±�,-1��ʾû��
	int GetEmptyStandbytStatus();
	// ����ʥ��״̬,����ʥ����ս/����/��Ϣ״̬, return < 0 ���ݴ���
	int FindStatusByIndex(int index);

	// ��ȡʥ������
	const ItemGridData* GetHolyEquipByIndex(int index);

	// ʥ������
	void HolyItemLevelUp(Protocol::CSHolyEquipLevelUpReq * req);
	// ʥ�����Ӿ���
	bool HolyAddExp(ItemGridData& item_data, int add_exp);
	// ����ʥ������
	void CalculateScore(int index);

	// ����ʥ��״̬ ��ս/����/��Ϣ
	void SetHolyEquipStatus(int index, int status);
	int SetFightingStatus(int index);
	int SetStandbyStatus(int index);
	int SetRestStatus(int index);
	void StatusChange(int index, int status_type = -1);
	// �����������ܿ�λ
	void UnlockSkillHole(int index, int hole);
	// �����µ��������� 
	void StudySkill(int index, int consume_item_id);
	// �����������������
	void SaveSkill(int index, int is_save);

	void GmStudySkill(int skill_id, int hole);
	void GmSetGift(int gift_id, int hole);

	// ������ʯ��
	void UnlockFuShiHole(int index, int fushi_hole);
	// ��Ƕ��ʯ
	void PutOnFuShi(int sheng_qi_index, int sheng_qi_hole, int fu_shi_index);
	// ȡ�·�ʯ
	void TakeOffFuShi(int sheng_qi_index, int sheng_qi_hole);
	// ȡ��ȫ����ʯ
	void TakeOffAllFuShi(int index);
	bool IsCanTakeOffAll(int index);

	// �ϳɷ�ʯ fushi_level-��ʯĿ��ȼ� [2-5]
	void FusionFuShi(int fushi_level, int fushi_index1, int fushi_index2, int fushi_index3, int fushi_index4, int fushi_index5);
	// ������ʯ
	void RecastFuShi(int fushi_index, int consume_index);
	// ����ʥ���ϵķ�ʯ
	void RecastFuShiOnHolyEquip(int index, int hole, int consume_fushi_index);

	// ʥ��һֻ����, pet_index = -1��ʾȡ�������ʥ��
	void BlessPet(int holy_index, int pet_index);
	void DeleteBlessPet(long long pet_unique_id);

	// ʥ������
	void HolyFollow(int index = -1);

	// ʥ��������һ�
	void HallowsSkillConvert(int skill_1, int skill_2);

public:
	/*ʥ����������*/
	int GetHolyItemEmptyBag();//�������еĸ���
	bool HasHolyItemBagNum(int grid_num);
	bool AddNewHolyItem(const ItemDataWrapper * new_item, int outside_reason, int *out_idx = NULL, int reason_type = HOLY_EQUIP_SEND_REASON_NORMAL);//����һ��ʥ�����Ѵ��ڵģ�
	bool AddNewHolyItem(const ItemGridData * new_item, int outside_reason, int *out_idx = NULL);//����һ��ʥ�����Ѵ��ڵģ�
	const ItemGridData * AddHolyItem(ItemID new_id, int outside_reason, int *out_idx = NULL, int reason_type = HOLY_EQUIP_SEND_REASON_IN);
	ItemGridData * GetHolyItemGridDataKnapsack(int index);

	/*��ʯ��������*/
	int GetFuShiEmptyBag();//�������еĸ���
	bool HasFuShiBagNum(int grid_num);
	bool AddNewFuShiItem(const ItemDataWrapper * new_item, int outside_reason, int *out_idx = NULL);//����һ����ʯ���Ѵ��ڵģ�, outside_reason: PUT_REASON
	bool AddNewFuShiItem(const ItemGridData * new_item, int outside_reason, int *out_idx = NULL);//����һ����ʯ���Ѵ��ڵģ�
	const ItemGridData * AddFuShiItem(ItemID new_id, int outside_reason, int *out_idx = NULL);
	ItemGridData * GetFuShiGridDataKnapsack(int index);

	// ��ձ��� ʥ������&��ʯ����
	void ClearAll();
	void ClearHolyGrid(int index);
	void ClearFuShiGrid(int index);

	bool ConsumeHolyGrid(int consume_idx, int reason_type, const char * reason, bool is_notice = true);		// ����ʥ��
	bool ConsumeFuShiGrid(int consume_idx, int reason_type, const char * reason, bool is_notice = true);	// ���ķ�ʯ

	void SetHolyBagDirty(short index, bool is_recalculate, short reason = HOLY_EQUIP_SEND_REASON_NORMAL, bool is_notice = true);
	void SetFuShiBagDirty(short index, short reason = HOLY_EQUIP_SEND_REASON_NORMAL, bool is_notice = true);

	void GmClearBag(int op_type);

	void CheckHolyEquipCollection();
	
	void GetBattleHallows(ARG_OUT short* hallow_num, ARG_OUT BattleHallowInfo* hallows, ARG_OUT short* fight_hallow_idx, int max_hallow_num);

private:
	// ALL IN!
	void SendAllInfo();
	void SendFirstGainFlagInfo();

	// ʥ��/���� ��������
	void SendSingleHolyBagInfo(int index, short reason_type);
	void SendSingleFuShiBagInfo(int index, short reason_type);

	// ʥ��/��ʯ ����ȫ������
	void SendHolyBagListInfo();
	void SendFuShiBagListInfo();

	void SendHolyFollowInfo();

	ItemGridData* MutableFightHolyEquip(int * out_index = NULL, int* out_index_in_bag = NULL);
private:
	RoleModuleManager * m_role_module_mgr;
	AttributeList m_role_attrs_add;			// ��ʯ������ɫ����
	AttributeList m_attrs_add[MAX_HOLY_EQUIP_FIGHT_STANDBY_NUM];	// ʥ��������������

	ItemGridData* GetHolyBagNode(int bag_idx);
	ItemGridData* GetFuShiBagNode(int bag_idx);

	// ʥ������
	int				m_holy_grid_num;
	bool			m_holy_bag_is_change;
	bool			m_holy_old_state[ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM];
	bool			m_holy_dirty_mark[ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM];
	ItemGridData*	m_holy_grid_list[ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM];

	// ��ʯ����
	int				m_fushi_grid_num;
	bool			m_fushi_bag_is_change;
	bool			m_fushi_old_state[ItemNamespace::MAX_RUNE_STONE_BAG_NUM];
	bool			m_fushi_dirty_mark[ItemNamespace::MAX_RUNE_STONE_BAG_NUM];
	ItemGridData*	m_fushi_grid_list[ItemNamespace::MAX_RUNE_STONE_BAG_NUM];

	HolyEquipCommonParam m_param;
};

#endif
