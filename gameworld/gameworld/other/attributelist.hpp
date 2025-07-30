#ifndef __ATTRIBUTE_LIST_HPP__
#define __ATTRIBUTE_LIST_HPP__

#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/attributedef.hpp"

static const int MAX_REASON_TYPE_LIST_NUM = 20;			//�·�������Դ�б��������ֵ

enum ROLE_REASON_LIST_TYPE
{
	ROLE_REASON_LIST_TYPE_MIN = 0,
	ROLE_REASON_LIST_TYPE_BASE  = ROLE_REASON_LIST_TYPE_MIN,			// ����
	ROLE_REASON_LIST_TYPE_POINT = 1,			// Ǳ��
	ROLE_REASON_LIST_TYPE_EQUIP = 2,			// װ��
	ROLE_REASON_LIST_TYPE_MEDAL = 3,			// ����(ѫ��)
	ROLE_REASON_LIST_TYPE_TRAIN = 4,			// ����
	ROLE_REASON_LIST_TYPE_ADV   = 5,			// ����
	ROLE_REASON_LIST_TYPE_COLLE = 6,			// ��Ʒ(ͼ��+��¼)
	ROLE_REASON_LIST_TYPE_OTHER = 7,			// ����
	ROLE_REASON_LIST_TYPE_SMS   = 8,			// ����ϵͳ
	ROLE_REASON_LIST_TYPE_FABAO = 9,			// ����
	ROLE_REASON_LIST_TYPE_SOUL = 10,			// ����
	ROLE_REASON_LIST_TYPE_GOD_PRINT = 11,		// ��ӡ
	ROLE_REASON_LIST_TYPE_PELLET = 12,			// ���Ե�
	ROLE_REASON_LIST_TYPE_ACCESSORIES = 13,		// ����
	ROLE_REASON_LIST_TYPE_XIU_WEI = 14,			// ��Ϊ
	ROLE_REASON_LIST_TYPE_FLY_UP = 15,			// ����
	ROLE_REASON_LIST_TYPE_ZAO_HUA = 16,			// �컯װ��
	ROLE_REASON_LIST_TYPE_HOLY_EQUIP = 17,		// ʥ��
	ROLE_REASON_LIST_TYPE_STAR_CHART = 18,		// ��ͼϵͳ
	ROLE_REASON_LIST_TYPE_HUANSHOU = 19,		// ����ϵͳ

	ROLE_REASON_LIST_TYPE_MAX,
};
UNSTD_STATIC_CHECK(ROLE_REASON_LIST_TYPE_MAX <= MAX_REASON_TYPE_LIST_NUM);

enum PET_REASON_LIST_TYPE
{
	PET_REASON_LIST_TYPE_BASE   = 0,				// ����
	PET_REASON_LIST_TYPE_GROWTH = 1,				// ����
	PET_REASON_LIST_TYPE_POINT  = 2,				// Ǳ��
	PET_REASON_LIST_TYPE_STREN  = 3,				// ǿ��
	PET_REASON_LIST_TYPE_RESON  = 4,				// ����
	PET_REASON_LIST_TYPE_SKILL  = 5,				// ����(������������)
	PET_REASON_LIST_TYPE_TRAIN  = 6,				// ����
	PET_REASON_LIST_TYPE_OTHER  = 7,				// ����
	PET_REASON_LIST_TYPE_SMS	= 8,				// ����ϵͳ
	PET_REASON_LIST_TYPE_FABAO	= 9,				// ����
	PET_REASON_LIST_TYPE_SOUL = 10,					// ����
	PET_REASON_LIST_TYPE_GOD_PRINT = 11,			// ��ӡ
	PET_REASON_LIST_TYPE_PELLET = 12,				// ���Ե�
	PET_REASON_LIST_TYPE_BENEDICTION = 13,			// ����ף��
	PET_REASON_LIST_TYPE_XIU_WEI = 14,				// ��Ϊ
	PET_REASON_LIST_TYPE_TALENT = 15,				// �츳
	PET_REASON_LIST_TYPE_HOLY_EQUIP = 16,			// ʥ��
	PET_REASON_LIST_TYPE_STAR_CHART = 17,			// ��ͼϵͳ
	PET_REASON_LIST_TYPE_GAI_MING = 18,			// ����

	PET_REASON_LIST_TYPE_MAX,
};
UNSTD_STATIC_CHECK(PET_REASON_LIST_TYPE_MAX <= MAX_REASON_TYPE_LIST_NUM);

enum PARTNER_REASON_LIST_TYPE
{
	PARTNER_REASON_LIST_TYPE_LEVEL  = 0,			// �ȼ�(�ȼ��������)
	PARTNER_REASON_LIST_TYPE_EQUIP  = 1,			// װ��(����)
	PARTNER_REASON_LIST_TYPE_ADV    = 2,			// ����(����)
	PARTNER_REASON_LIST_TYPE_WAKE   = 3,			// ����(��Ʒ)
	PARTNER_REASON_LIST_TYPE_TALENT = 4,			// �츳
	PARTNER_REASON_LIST_TYPE_TRAIN  = 5,			// ����
	PARTNER_REASON_LIST_TYPE_OTHER  = 6,			// ����
	PARTNER_REASON_LIST_TYPE_PELLET = 7,			// ���Ե�
	PARTNER_REASON_LIST_TYPE_PACT	= 8,			// Ԫ����Լ
	PARTNER_REASON_LIST_TYPE_XIU_WEI = 9,			// ��Ϊ
	PARTNER_REASON_LIST_TYPE_INTENSIFY = 10,		// ǿ��
	PARTNER_REASON_LIST_TYPE_STAR_CHART = 11,		// ��ͼ

	PARTNER_REASON_LIST_TYPE_MAX,
};
UNSTD_STATIC_CHECK(PARTNER_REASON_LIST_TYPE_MAX <= MAX_REASON_TYPE_LIST_NUM);

class AttributeList
{
public:
	enum RECALC_REASON
	{
		RECALC_REASON_INVALID = 0,
		RECALC_REASON_SELF = 1,						// �ȼ��������
		RECALC_REASON_ATTRIBUTE_PLAN = 2,			// ����ӵ�����
		RECALC_REASON_COLLECTION = 3,				// ��Ʒ
		RECALC_REASON_TITLE = 4,					// �ƺ�
		RECALC_REASON_TRAIN = 5,					// ����
		RECALC_REASON_PARTNER = 6,					// ���
		RECALC_REASON_SURFACE = 7,					// �û�
		RECALC_REASON_BABY = 8,						// ��Ů 
		RECALC_REASON_EQUIPMENT = 9,				// װ�� / ����
		RECALC_REASON_FABAO = 10,					// ����
		RECALC_REASON_MEDAL = 11,					// ѫ��
		RECALC_REASON_BUFF = 12,					// buff
		RECALC_REASON_FACE_SCORE = 13,				// ��ֵ
		RECALC_REASON_PROF_ADVAN = 14,				// ����
		RECALC_REASON_ROLE_SKILL = 15,				// ��ɫ����
		RECALC_REASON_SMART_MOUNTS_SYSTEM = 16,		// ����ϵͳ
		RECALC_REASON_GUILD_INBORN = 17,			// ��������
		RECALC_REASON_HEART_SKILL = 18,				// �ķ�
		RECALC_REASON_ATTR_PELLET = 19,				// ���Ե�
		RECALC_REASON_ACCESSORIES = 20,				// ����
		RECALC_REASON_LIFE_SKILL_PROPS = 21,		// ����ʳ��
		RECALC_REASON_FLY_UP_SYSTEM = 22,			// ����
		RECALC_REASON_XIU_WEI = 23,					// ��Ϊ
		RECALC_REASON_GUILD_QIFU = 24,				// ������
		RECALC_REASON_ZAO_HUA = 25,					// �컯װ��
		RECALC_REASON_HOLY_EQUIP = 26,				// ʥ��
		RECALC_REASON_STAR_CHART = 27,				// ��ͼϵͳ
		RECALC_REASON_FASHION_COLLECTION = 28,		// �������
		RECALC_REASON_HUAN_SHOU = 29,				// ����
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