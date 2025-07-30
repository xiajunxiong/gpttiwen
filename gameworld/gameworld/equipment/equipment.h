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
	enum EQUIPMENT_TYPE												// 装备类型
	{
		E_TYPE_MIN = 100,

		E_TYPE_HELMET = E_TYPE_MIN,									// 头盔
		E_TYPE_ARMOUR,												// 盔甲
		E_TYPE_WEAPON,												// 武器
		E_TYPE_SHIELD,												// 盾牌
		E_TYPE_SHOE,												// 鞋子

		E_TYPE_MAX,
	};

	const static int EQUIPMENT_TYPE_COUNT = E_TYPE_MAX - E_TYPE_MIN;// 装备类型数量

	enum EQUIPMENT_CLASS
	{
		E_CLASS_MIN = 3,

		E_CLASS_LIGHT_ARMOR = E_CLASS_MIN,		// 轻甲
		E_CLASS_HEAVY_ARMOR = 4,				// 重甲
		E_CLASS_CLOTH_ARMOR = 5,				// 布甲
		E_CLASS_FISTS_GLOVE = 6,				// 拳套
		E_CLASS_LARGE_SWORD = 7,				// 大剑
		E_CLASS_BATTLE_AXES = 8,				// 战斧
		E_CLASS_SHARP_SPEAR = 9,				// 长枪
		E_CLASS_WAR_LONGBOW = 10,				// 战弓
		E_CLASS_MAGIC_STAFF = 11,				// 魔法杖
		E_CLASS_CLERIC_WAND = 12,				// 传教杖(灯盏)
		E_CLASS_WIZARD_WAND = 13,				// 巫师杖(羽扇)
		E_CLASS_WARD_SHIELD = 14,				// 守护盾
		E_CLASS_HEAVY_ARMET = 15,				// 重甲 头盔
		E_CLASS_LIGHTLY_CAP = 16,				// 轻甲 帽子
		E_CLASS_CLOTH_HOODS = 17,				// 布甲 头巾
		E_CLASS_HEAVY_BOOTS = 18,				// 重甲 战靴
		E_CLASS_LIGHT_SHOES = 19,				// 轻甲 鞋子
		E_CLASS_CLOTH_SHOES = 20,				// 布甲 布履

		E_CLASS_MAX
	};

	enum EQUIPMENT_INDEX											// 装备部位
	{		
		E_INDEX_MIN = 0,
		E_INDEX_HELMET = E_INDEX_MIN,								// 头盔 0
		E_INDEX_ARMOUR,												// 盔甲 1
		E_INDEX_WEAPON,												// 武器 2
		E_INDEX_SHIELD,												// 盾牌 3
		E_INDEX_SHOE,												// 鞋子 4
		
		E_INDEX_MAX,
	};
	UNSTD_STATIC_CHECK(E_INDEX_MAX == EQUIPMENT_MAX_INDEX);

	enum EQUIP_ATTR_IDX
	{
		EQUIP_ATTR_IDX_MIN = 0,
		EQUIP_ATTR_IDX_MAXHP = EQUIP_ATTR_IDX_MIN,		// 0 最大生命值
		EQUIP_ATTR_IDX_MAXMP,							// 1 最大魔法值
		EQUIP_ATTR_IDX_ATTACK,							// 2 攻击
		EQUIP_ATTR_IDX_DEFENSE,							// 3 防御
		EQUIP_ATTR_IDX_AGILITY,							// 4 敏捷
		EQUIP_ATTR_IDX_MENTAL,							// 5 精神
		EQUIP_ATTR_IDX_RECOVERY,						// 6 回复
		EQUIP_ATTR_IDX_CRITICAL,						// 7 必杀率
		EQUIP_ATTR_IDX_CRITICAL_INC_VALUE,				// 8 暴击伤害加成
		EQUIP_ATTR_IDX_CRITICAL_DEC_VALUE,				// 9 暴击伤害抵抗
		EQUIP_ATTR_IDX_COUNTER_ATTACK,					// 10 反击率
		EQUIP_ATTR_IDX_HIT,								// 11 命中率
		EQUIP_ATTR_IDX_DODGE,							// 12 闪避率
		EQUIP_ATTR_IDX_ANTI_POISON,						// 13 剧毒抵抗
		EQUIP_ATTR_IDX_ANTI_PETRIFY,					// 14 石化抵抗
		EQUIP_ATTR_IDX_ANTI_SLEEP,						// 15 睡眠抵抗
		EQUIP_ATTR_IDX_ANTI_DRUNK,						// 16 醉酒抵抗
		EQUIP_ATTR_IDX_ANTI_CHAOS,						// 17 混乱抵抗
		EQUIP_ATTR_IDX_MAGIC_ATTACK,					// 18 魔攻
		EQUIP_ATTR_IDX_MAGIC_DEFENSE,					// 19 魔抗

		EQUIP_ATTR_IDX_MAX,
	};

	enum EQUIP_COLOR_IDX
	{
		EQUIP_COLOR_IDX_MIN = 0,
		EQUIP_COLOR_IDX_WHITE,			//白	1
		EQUIP_COLOR_IDX_GREEN,			//绿	2
		EQUIP_COLOR_IDX_BLUE,			//蓝	3
		EQUIP_COLOR_IDX_PURPLE,			//紫	4
		EQUIP_COLOR_IDX_ORANGE,			//橙	5
		EQUIP_COLOR_IDX_RED,			//红	6
		EQUIP_COLOR_IDX_STAR,			//星耀	7
		EQUIP_COLOR_IDX_GOD,			//神装	8

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
	
	// 属性替代值为 0 时，表示此项属性不进行替代 - 只会随机属性，不会做别的事情
	void RandomAttrByInsteadMin(NetValueItemParam * out_param, const int min_instead_list[EQUIP_ATTR_IDX_MAX]) const;
	void RandomAttrByInsteadMax(NetValueItemParam * out_param, const int max_instead_list[EQUIP_ATTR_IDX_MAX]) const;
	void RandomEquipmentAttr(NetValueItemParam * out_param) const;
	void MaxValEquipmentAttr(NetValueItemParam * out_param) const;
	void RandomSpecialEffect(NetValueItemParam * out_param) const;
	int CheckEquipmentIsTreasure(const EquipmentParam & p) const;		// 检查这样属性的装备是不是珍品
	bool CheckEquipmentAttrValid(int type, Attribute value) const;
	bool CheckEquipmentAttrMax(NetValueItemParam * param) const;		//检查是否为满属性装备

	//精工检查
	bool CanStrongUltimate(const EquipmentParam & p);
	bool GetNormalUltimateMaxValue(int ultimate_type, int *min_value, int * max_value);//获取精工上限
	bool GetUltimateMaxValue(int ultimate_type,int * max_value, int *min_value = NULL);//获取强化精工最大值
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
	const short	m_equipment_type;									// 装备类型EQUIPMENT_TYPE 武器 衣服等
	short m_equipment_class;										// 装备分类EQUIPMENT_CLASS 法杖 弓箭 布甲等
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


	short m_effect_random_group;									// 随机特效组
	short m_equip_level;
	short m_limit_prof_grade;										// 职业进阶次数限制
	int m_suit_random_group;

	int star_group;
};

#endif