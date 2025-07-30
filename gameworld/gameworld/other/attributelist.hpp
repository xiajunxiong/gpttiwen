#ifndef __ATTRIBUTE_LIST_HPP__
#define __ATTRIBUTE_LIST_HPP__

#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/attributedef.hpp"

static const int MAX_REASON_TYPE_LIST_NUM = 20;			//下发属性来源列表数组最大值

enum ROLE_REASON_LIST_TYPE
{
	ROLE_REASON_LIST_TYPE_MIN = 0,
	ROLE_REASON_LIST_TYPE_BASE  = ROLE_REASON_LIST_TYPE_MIN,			// 基础
	ROLE_REASON_LIST_TYPE_POINT = 1,			// 潜能
	ROLE_REASON_LIST_TYPE_EQUIP = 2,			// 装备
	ROLE_REASON_LIST_TYPE_MEDAL = 3,			// 令牌(勋章)
	ROLE_REASON_LIST_TYPE_TRAIN = 4,			// 修炼
	ROLE_REASON_LIST_TYPE_ADV   = 5,			// 进阶
	ROLE_REASON_LIST_TYPE_COLLE = 6,			// 藏品(图鉴+收录)
	ROLE_REASON_LIST_TYPE_OTHER = 7,			// 其它
	ROLE_REASON_LIST_TYPE_SMS   = 8,			// 灵骑系统
	ROLE_REASON_LIST_TYPE_FABAO = 9,			// 法宝
	ROLE_REASON_LIST_TYPE_SOUL = 10,			// 魂器
	ROLE_REASON_LIST_TYPE_GOD_PRINT = 11,		// 神印
	ROLE_REASON_LIST_TYPE_PELLET = 12,			// 属性丹
	ROLE_REASON_LIST_TYPE_ACCESSORIES = 13,		// 配饰
	ROLE_REASON_LIST_TYPE_XIU_WEI = 14,			// 修为
	ROLE_REASON_LIST_TYPE_FLY_UP = 15,			// 飞升
	ROLE_REASON_LIST_TYPE_ZAO_HUA = 16,			// 造化装备
	ROLE_REASON_LIST_TYPE_HOLY_EQUIP = 17,		// 圣器
	ROLE_REASON_LIST_TYPE_STAR_CHART = 18,		// 星图系统
	ROLE_REASON_LIST_TYPE_HUANSHOU = 19,		// 幻兽系统

	ROLE_REASON_LIST_TYPE_MAX,
};
UNSTD_STATIC_CHECK(ROLE_REASON_LIST_TYPE_MAX <= MAX_REASON_TYPE_LIST_NUM);

enum PET_REASON_LIST_TYPE
{
	PET_REASON_LIST_TYPE_BASE   = 0,				// 基础
	PET_REASON_LIST_TYPE_GROWTH = 1,				// 资质
	PET_REASON_LIST_TYPE_POINT  = 2,				// 潜能
	PET_REASON_LIST_TYPE_STREN  = 3,				// 强化
	PET_REASON_LIST_TYPE_RESON  = 4,				// 共鸣
	PET_REASON_LIST_TYPE_SKILL  = 5,				// 技能(包括护符技能)
	PET_REASON_LIST_TYPE_TRAIN  = 6,				// 修炼
	PET_REASON_LIST_TYPE_OTHER  = 7,				// 其它
	PET_REASON_LIST_TYPE_SMS	= 8,				// 灵骑系统
	PET_REASON_LIST_TYPE_FABAO	= 9,				// 法宝
	PET_REASON_LIST_TYPE_SOUL = 10,					// 魂器
	PET_REASON_LIST_TYPE_GOD_PRINT = 11,			// 神印
	PET_REASON_LIST_TYPE_PELLET = 12,				// 属性丹
	PET_REASON_LIST_TYPE_BENEDICTION = 13,			// 宠物祝福
	PET_REASON_LIST_TYPE_XIU_WEI = 14,				// 修为
	PET_REASON_LIST_TYPE_TALENT = 15,				// 天赋
	PET_REASON_LIST_TYPE_HOLY_EQUIP = 16,			// 圣器
	PET_REASON_LIST_TYPE_STAR_CHART = 17,			// 星图系统
	PET_REASON_LIST_TYPE_GAI_MING = 18,			// 改命

	PET_REASON_LIST_TYPE_MAX,
};
UNSTD_STATIC_CHECK(PET_REASON_LIST_TYPE_MAX <= MAX_REASON_TYPE_LIST_NUM);

enum PARTNER_REASON_LIST_TYPE
{
	PARTNER_REASON_LIST_TYPE_LEVEL  = 0,			// 等级(等级配给属性)
	PARTNER_REASON_LIST_TYPE_EQUIP  = 1,			// 装备(命格)
	PARTNER_REASON_LIST_TYPE_ADV    = 2,			// 晋升(进阶)
	PARTNER_REASON_LIST_TYPE_WAKE   = 3,			// 觉醒(升品)
	PARTNER_REASON_LIST_TYPE_TALENT = 4,			// 天赋
	PARTNER_REASON_LIST_TYPE_TRAIN  = 5,			// 修炼
	PARTNER_REASON_LIST_TYPE_OTHER  = 6,			// 其它
	PARTNER_REASON_LIST_TYPE_PELLET = 7,			// 属性丹
	PARTNER_REASON_LIST_TYPE_PACT	= 8,			// 元神契约
	PARTNER_REASON_LIST_TYPE_XIU_WEI = 9,			// 修为
	PARTNER_REASON_LIST_TYPE_INTENSIFY = 10,		// 强化
	PARTNER_REASON_LIST_TYPE_STAR_CHART = 11,		// 星图

	PARTNER_REASON_LIST_TYPE_MAX,
};
UNSTD_STATIC_CHECK(PARTNER_REASON_LIST_TYPE_MAX <= MAX_REASON_TYPE_LIST_NUM);

class AttributeList
{
public:
	enum RECALC_REASON
	{
		RECALC_REASON_INVALID = 0,
		RECALC_REASON_SELF = 1,						// 等级配给属性
		RECALC_REASON_ATTRIBUTE_PLAN = 2,			// 人物加点属性
		RECALC_REASON_COLLECTION = 3,				// 藏品
		RECALC_REASON_TITLE = 4,					// 称号
		RECALC_REASON_TRAIN = 5,					// 修炼
		RECALC_REASON_PARTNER = 6,					// 伙伴
		RECALC_REASON_SURFACE = 7,					// 幻化
		RECALC_REASON_BABY = 8,						// 子女 
		RECALC_REASON_EQUIPMENT = 9,				// 装备 / 灵饰
		RECALC_REASON_FABAO = 10,					// 法宝
		RECALC_REASON_MEDAL = 11,					// 勋章
		RECALC_REASON_BUFF = 12,					// buff
		RECALC_REASON_FACE_SCORE = 13,				// 颜值
		RECALC_REASON_PROF_ADVAN = 14,				// 进阶
		RECALC_REASON_ROLE_SKILL = 15,				// 角色技能
		RECALC_REASON_SMART_MOUNTS_SYSTEM = 16,		// 灵骑系统
		RECALC_REASON_GUILD_INBORN = 17,			// 家族灵脉
		RECALC_REASON_HEART_SKILL = 18,				// 心法
		RECALC_REASON_ATTR_PELLET = 19,				// 属性丹
		RECALC_REASON_ACCESSORIES = 20,				// 配饰
		RECALC_REASON_LIFE_SKILL_PROPS = 21,		// 生活食物
		RECALC_REASON_FLY_UP_SYSTEM = 22,			// 飞升
		RECALC_REASON_XIU_WEI = 23,					// 修为
		RECALC_REASON_GUILD_QIFU = 24,				// 家族祈福
		RECALC_REASON_ZAO_HUA = 25,					// 造化装备
		RECALC_REASON_HOLY_EQUIP = 26,				// 圣器
		RECALC_REASON_STAR_CHART = 27,				// 星图系统
		RECALC_REASON_FASHION_COLLECTION = 28,		// 外观升级
		RECALC_REASON_HUAN_SHOU = 29,				// 幻兽
	};

public:
	AttributeList()
	{
		this->Reset();
	}

	~AttributeList()
	{
		
	}

	void Reset()
	{
		memset(m_attrs, 0, sizeof(m_attrs));
	}

	void ResetAddPointTypeValue()
	{
		this->m_attrs[BATTLE_ADD_POINT_TYPE_VITALITY] = 0;
		this->m_attrs[BATTLE_ADD_POINT_TYPE_STRENGTH] = 0;
		this->m_attrs[BATTLE_ADD_POINT_TYPE_POWER] = 0;
		this->m_attrs[BATTLE_ADD_POINT_TYPE_AGILITY] = 0;
		this->m_attrs[BATTLE_ADD_POINT_TYPE_MAGIC] = 0;

		UNSTD_STATIC_CHECK(BATTLE_ADD_POINT_TYPE_END - BATTLE_ADD_POINT_TYPE_BEGIN == 4);
	}

	void GetAddPointTypeValue(int (&add_point_list)[ADD_POINT_TYPE_MAX])const
	{
		add_point_list[ADD_POINT_TYPE_VITALITY] = this->m_attrs[BATTLE_ADD_POINT_TYPE_VITALITY];
		add_point_list[ADD_POINT_TYPE_STRENGTH] = this->m_attrs[BATTLE_ADD_POINT_TYPE_STRENGTH];
		add_point_list[ADD_POINT_TYPE_POWER] = this->m_attrs[BATTLE_ADD_POINT_TYPE_POWER];
		add_point_list[ADD_POINT_TYPE_AGILITY] = this->m_attrs[BATTLE_ADD_POINT_TYPE_AGILITY];
		add_point_list[ADD_POINT_TYPE_MAGIC] = this->m_attrs[BATTLE_ADD_POINT_TYPE_MAGIC];

		UNSTD_STATIC_CHECK(BATTLE_ADD_POINT_TYPE_END - BATTLE_ADD_POINT_TYPE_BEGIN == 4);
		UNSTD_STATIC_CHECK(ADD_POINT_TYPE_MAX == 5);
	}


	Attribute GetAttr(int index) const
	{
		if (index < 0 || index >= BATTLE_ATTR_MAX) return 0;
		return m_attrs[index];
	}
	void SetAttr(int index, Attribute value)
	{
		if (index < 0 || index >= BATTLE_ATTR_MAX) return;
		m_attrs[index] = value;
	}
	void Add(const Attribute attrs[BATTLE_ATTR_MAX])
	{
		for (int i = 0; i < BATTLE_ATTR_MAX; i++)
		{
			m_attrs[i] += attrs[i];
		}
	}
	void Deduct(const Attribute attrs[BATTLE_ATTR_MAX])
	{
		for (int i = 0; i < BATTLE_ATTR_MAX; i++)
		{
			m_attrs[i] -= attrs[i];
		}
	}
	void Set(Attribute attrs[BATTLE_ATTR_MAX])
	{
		for (int i = 0; i < BATTLE_ATTR_MAX; i++)
		{
			m_attrs[i] = attrs[i];
		}
	}

	void AddAttrCfg(const std::vector<AttrItemCfg> & attr_cfg)
	{
		for (int i = 0; i < (int)attr_cfg.size(); ++i)
		{
			const AttrItemCfg & curr = attr_cfg[i];
			this->AddAttrCfg(curr);
		}
	}

	void AddAttrCfg(const AttrItemCfg & attr_item_cfg)
	{
		if (attr_item_cfg.is_pet_attr)
		{
			return;
		}

		if (attr_item_cfg.attr_type < 0 || attr_item_cfg.attr_type >= ARRAY_LENGTH(m_attrs))
		{
			return;
		}

		m_attrs[attr_item_cfg.attr_type] += attr_item_cfg.attr_value;
	}

	template<class T, size_t size>
	inline void  AddAttrInfo(T(&arg)[size])
	{
		for (size_t i = 0; i < size; ++i)
		{
			const T & curr = arg[i];

			if (curr.attr_type < 0 || curr.attr_type >= ARRAY_LENGTH(m_attrs))
			{
				continue;
			}

			m_attrs[curr.attr_type] += curr.attr_value;
		}
	}

	Attribute m_attrs[BATTLE_ATTR_MAX];
};

void GetSpecialEffectAttr(std::vector<int> sp_list, AttributeList & attr, const AttributeList & base_attr_list, int role_level, int level_by_type = 0, AttributeList * base_point_list = NULL);
void GetSpecialEffectAttr(int sp_id, AttributeList & attr, const AttributeList & base_attr_list, int role_level, int level_by_type = 0, AttributeList * base_point_list = NULL);

struct SkillPro
{
	SkillPro(): skill_id(0), skill_level(0) {}
	SkillPro(UInt16 _skill_id, short _skill_level) : skill_id(_skill_id), skill_level(_skill_level) {}

	UInt16 skill_id;
	short skill_level;
};

#endif