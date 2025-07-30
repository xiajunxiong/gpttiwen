#ifndef __EQUIPMENT_H__
#define __EQUIPMENT_H__

#include "gamedef.h"
#include "item/itembase.h"
#include "item/itemgriddata.h"
#include "servercommon/struct/itemlistparam.h"

// ------------------------------------------------------------------------------------------------------------

class Equipment : public ItemBase
{
public:
	enum EQUIPMENT_TYPE												// װ������
	{
		E_TYPE_MIN = 100,

		E_TYPE_HELMET = E_TYPE_MIN,									// ͷ��
		E_TYPE_ARMOUR,												// ����
		E_TYPE_WEAPON,												// ����
		E_TYPE_SHIELD,												// ����
		E_TYPE_SHOE,												// Ь��

		E_TYPE_MAX,
	};

	const static int EQUIPMENT_TYPE_COUNT = E_TYPE_MAX - E_TYPE_MIN;// װ����������

	enum EQUIPMENT_CLASS
	{
		E_CLASS_MIN = 3,

		E_CLASS_LIGHT_ARMOR = E_CLASS_MIN,		// ���
		E_CLASS_HEAVY_ARMOR = 4,				// �ؼ�
		E_CLASS_CLOTH_ARMOR = 5,				// ����
		E_CLASS_FISTS_GLOVE = 6,				// ȭ��
		E_CLASS_LARGE_SWORD = 7,				// ��
		E_CLASS_BATTLE_AXES = 8,				// ս��
		E_CLASS_SHARP_SPEAR = 9,				// ��ǹ
		E_CLASS_WAR_LONGBOW = 10,				// ս��
		E_CLASS_MAGIC_STAFF = 11,				// ħ����
		E_CLASS_CLERIC_WAND = 12,				// ������(��յ)
		E_CLASS_WIZARD_WAND = 13,				// ��ʦ��(����)
		E_CLASS_WARD_SHIELD = 14,				// �ػ���
		E_CLASS_HEAVY_ARMET = 15,				// �ؼ� ͷ��
		E_CLASS_LIGHTLY_CAP = 16,				// ��� ñ��
		E_CLASS_CLOTH_HOODS = 17,				// ���� ͷ��
		E_CLASS_HEAVY_BOOTS = 18,				// �ؼ� սѥ
		E_CLASS_LIGHT_SHOES = 19,				// ��� Ь��
		E_CLASS_CLOTH_SHOES = 20,				// ���� ����

		E_CLASS_MAX
	};

	enum EQUIPMENT_INDEX											// װ����λ
	{		
		E_INDEX_MIN = 0,
		E_INDEX_HELMET = E_INDEX_MIN,								// ͷ�� 0
		E_INDEX_ARMOUR,												// ���� 1
		E_INDEX_WEAPON,												// ���� 2
		E_INDEX_SHIELD,												// ���� 3
		E_INDEX_SHOE,												// Ь�� 4
		
		E_INDEX_MAX,
	};
	UNSTD_STATIC_CHECK(E_INDEX_MAX == EQUIPMENT_MAX_INDEX);

	enum EQUIP_ATTR_IDX
	{
		EQUIP_ATTR_IDX_MIN = 0,
		EQUIP_ATTR_IDX_MAXHP = EQUIP_ATTR_IDX_MIN,		// 0 �������ֵ
		EQUIP_ATTR_IDX_MAXMP,							// 1 ���ħ��ֵ
		EQUIP_ATTR_IDX_ATTACK,							// 2 ����
		EQUIP_ATTR_IDX_DEFENSE,							// 3 ����
		EQUIP_ATTR_IDX_AGILITY,							// 4 ����
		EQUIP_ATTR_IDX_MENTAL,							// 5 ����
		EQUIP_ATTR_IDX_RECOVERY,						// 6 �ظ�
		EQUIP_ATTR_IDX_CRITICAL,						// 7 ��ɱ��
		EQUIP_ATTR_IDX_CRITICAL_INC_VALUE,				// 8 �����˺��ӳ�
		EQUIP_ATTR_IDX_CRITICAL_DEC_VALUE,				// 9 �����˺��ֿ�
		EQUIP_ATTR_IDX_COUNTER_ATTACK,					// 10 ������
		EQUIP_ATTR_IDX_HIT,								// 11 ������
		EQUIP_ATTR_IDX_DODGE,							// 12 ������
		EQUIP_ATTR_IDX_ANTI_POISON,						// 13 �綾�ֿ�
		EQUIP_ATTR_IDX_ANTI_PETRIFY,					// 14 ʯ���ֿ�
		EQUIP_ATTR_IDX_ANTI_SLEEP,						// 15 ˯�ߵֿ�
		EQUIP_ATTR_IDX_ANTI_DRUNK,						// 16 ��Ƶֿ�
		EQUIP_ATTR_IDX_ANTI_CHAOS,						// 17 ���ҵֿ�
		EQUIP_ATTR_IDX_MAGIC_ATTACK,					// 18 ħ��
		EQUIP_ATTR_IDX_MAGIC_DEFENSE,					// 19 ħ��

		EQUIP_ATTR_IDX_MAX,
	};

	enum EQUIP_COLOR_IDX
	{
		EQUIP_COLOR_IDX_MIN = 0,
		EQUIP_COLOR_IDX_WHITE,			//��	1
		EQUIP_COLOR_IDX_GREEN,			//��	2
		EQUIP_COLOR_IDX_BLUE,			//��	3
		EQUIP_COLOR_IDX_PURPLE,			//��	4
		EQUIP_COLOR_IDX_ORANGE,			//��	5
		EQUIP_COLOR_IDX_RED,			//��	6
		EQUIP_COLOR_IDX_STAR,			//��ҫ	7
		EQUIP_COLOR_IDX_GOD,			//��װ	8

		EQUIP_COLOR_IDX_MAX,
	};

	UNSTD_STATIC_CHECK(E_INDEX_MAX == ItemNamespace::MAX_EQUIPMENT_GRID_NUM);
	
	Equipment(short equip_type);
	virtual ~Equipment() {}
	bool IsUnLimitLevel()const { return m_is_unlimit_level; }
	short GetEquipType() const { return m_equipment_type; }
	short GetEquipIndex()const { return  m_equipment_type - E_TYPE_MIN; }
	short GetEquipClass() const { return m_equipment_class; }
	short GetEquipLevel() const { return m_equip_level; }
	short GetWeaponType() const { return m_weapon_type; }

	virtual bool Init(const char* path, std::string *err);
	virtual int Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type = ITEM_PARAM_TYPE_INVAILD, void * param = NULL) const;

	virtual int GetSellPrice(NetValueItemParam *param) const;
	virtual bool CanDiscard(NetValueItemParam *param, int *reason) const;

	static bool IsMatchIndex(short equip_type, short equip_index);
	static bool IsValidType(short equip_type);

	virtual void RandomNetValueOnPutItem(Role* role, NetValueItemParam* param_data, int put_reason) const;

	bool GetEquipAttrSegmentByIdx(int equip_attr_idx, AttrSegment * out_segment) const;
	
	// �������ֵΪ 0 ʱ����ʾ�������Բ�������� - ֻ��������ԣ��������������
	void RandomAttrByInsteadMin(NetValueItemParam * out_param, const int min_instead_list[EQUIP_ATTR_IDX_MAX]) const;
	void RandomAttrByInsteadMax(NetValueItemParam * out_param, const int max_instead_list[EQUIP_ATTR_IDX_MAX]) const;
	void RandomEquipmentAttr(NetValueItemParam * out_param) const;
	void MaxValEquipmentAttr(NetValueItemParam * out_param) const;
	void RandomSpecialEffect(NetValueItemParam * out_param) const;
	int CheckEquipmentIsTreasure(const EquipmentParam & p) const;		// ����������Ե�װ���ǲ�����Ʒ
	bool CheckEquipmentAttrValid(int type, Attribute value) const;
	bool CheckEquipmentAttrMax(NetValueItemParam * param) const;		//����Ƿ�Ϊ������װ��

	//�������
	bool CanStrongUltimate(const EquipmentParam & p);
	bool GetNormalUltimateMaxValue(int ultimate_type, int *min_value, int * max_value);//��ȡ��������
	bool GetUltimateMaxValue(int ultimate_type,int * max_value, int *min_value = NULL);//��ȡǿ���������ֵ
	bool GetNormalMinAndMaxUltimateValue(int * min_value, int *max_value);
	bool GetStrongMinAndMaxUltimateValue(int * min_value, int *max_value);

	static short GetBattleWeaponType(short equipment_class);
	static int GetEquipmentAttrType(int battle_attr_type);

	bool IsSingleHandWeapon() const;
	bool IsWeapon() const;
	bool IsShield() const;
	short GetEntryNum()const { return m_entry_num; }
	short GetEntryGroup()const { return m_entry_group; }
	int GetEquipColor()const { return m_color; }
	int GetSpecialEffectRandomGroup() const { return m_effect_random_group; }
	int GetSpecialEquip()const { return m_special_equip; }
	int GetSuitRandomGroup() const { return m_suit_random_group; }
protected:
	const short	m_equipment_type;									// װ������EQUIPMENT_TYPE ���� �·���
	short m_equipment_class;										// װ������EQUIPMENT_CLASS ���� ���� ���׵�
	short m_entry_num;
	short m_entry_group;
	short m_weapon_type;
	bool m_is_unlimit_level;
	AttrSegment m_attr_list[EQUIP_ATTR_IDX_MAX];					

	int m_attr_type[EQUIP_JEWELRY_ATTR_NUM];
	int m_min_value[EQUIP_JEWELRY_ATTR_NUM];
	int m_max_value[EQUIP_JEWELRY_ATTR_NUM];

	int m_special_equip;

	int m_min_jg_common;
	int m_max_jg_common;
	int	m_min_jg_added;
	int m_max_jg_added;


	short m_effect_random_group;									// �����Ч��
	short m_equip_level;
	short m_limit_prof_grade;										// ְҵ���״�������
	int m_suit_random_group;

	int star_group;
};

#endif