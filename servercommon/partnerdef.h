#ifndef __PARTNER_DEF_H__
#define __PARTNER_DEF_H__

#include "servercommon/struct/battle/battle_def.hpp"

#pragma pack(push, 4)

static const int PARTNER_NUM_MAX = 40;			// ���ͬʱӵ�еĻ������
static const int PARTNER_SYNFIGHT_NUM_MAX = 5;	// ���ͬʱ��ս�Ļ������
static const int PARTNER_FETTER_COUNT_MAX = 2;	// ÿ���������ӵ�е������ - �����޸ģ���Ҫ��չ�Ļ�����
static const int PARTNER_COMMON_SKILL_MAX = 3;	// ÿ������ս���ܵ�������� - �����޸ģ���Ҫ��չ�Ļ�����

static const int PARTNER_ERROR_SEQ = -1;		// ��������α�����
static const int PARTNER_ID_START = 1;
static const int PARTNER_HOLE_PER_LEVEL = 10;	// ����ÿ�׶εȼ����

static const int PARTNER_SKIN_ONCE = 12;				// ��һ����������12��Ƥ��		4�ı���
UNSTD_STATIC_CHECK(PARTNER_SKIN_ONCE <= 12);
static const int PARTNER_SKIN_MAX = PARTNER_SKIN_ONCE * PARTNER_NUM_MAX;		// ���Ƥ���ܺ����ֵ
static const int MOVE_BYTE_BASE = 0x00000001;			// λ�ƻ���

UNSTD_STATIC_CHECK(PARTNER_NUM_MAX <= 64)				//!< ����б�������չʱ��Ҫ���� 64���������ݿⷽ��������

static const int MYSTERY_STORE_MAX_SEQ = 12;			// �̵������Ʒ��

static const int PARTNER_QUALITY_SEGMENT_MAX_NUM = 5;	// �����߶���

static const int MAX_PARTNER_ARMS_LEVEL = 20;			// ���ר��������ߵȼ�

// ���--Ԫ����Լ ����Ǽ� 1--n
static const int PARTNER_SPIRIT_PACT_MAX_STAR = 5;
// ���--Ԫ����Լ �ֿ������ = ������� * ����Ǽ�
static const int PARTNER_SPIRIT_PACT_MAX_GRID = PARTNER_NUM_MAX * PARTNER_SPIRIT_PACT_MAX_STAR;	

// ���--Ԫ����Լ �����Լ����
static const int PARTNER_SPIRIT_PACT_MAX = 3;
// ���--Ԫ����Լ �����Լ����
static const int PARTNER_SPIRIT_BE_PACT_MAX = 2;

// ���ˮ��Ԫ��
#define CLEAR_PARTNER_CRYSTAL(attr_list)								\
do																		\
{																		\
	attr_list[BATTLE_ATTR_ELEMENT_WATER] = PARTNER_CRYSTAL_NULL;		\
	attr_list[BATTLE_ATTR_ELEMENT_FIRE] = PARTNER_CRYSTAL_NULL;			\
	attr_list[BATTLE_ATTR_ELEMENT_WIND] = PARTNER_CRYSTAL_NULL;			\
	attr_list[BATTLE_ATTR_ELEMENT_EARTH] = PARTNER_CRYSTAL_NULL;		\
} while (0);

// ���ˮ��Ԫ��
#define PARTNER_CRYSTAL_PUT(cyrstal_id,attr_list)							\
do																			\
{																			\
switch (cyrstal_id)															\
{																			\
case PARTNER_CRYSTAL_ID_WATER:												\
	attr_list[BATTLE_ATTR_ELEMENT_WATER] = PARTNER_CRYSTAL_PURE;			\
	break;																	\
																			\
case PARTNER_CRYSTAL_ID_FIRE:												\
	attr_list[BATTLE_ATTR_ELEMENT_FIRE] = PARTNER_CRYSTAL_PURE;				\
	break;																	\
																			\
case PARTNER_CRYSTAL_ID_WIND:												\
	attr_list[BATTLE_ATTR_ELEMENT_WIND] = PARTNER_CRYSTAL_PURE;				\
	break;																	\
																			\
case PARTNER_CRYSTAL_ID_EARTH:												\
	attr_list[BATTLE_ATTR_ELEMENT_EARTH] = PARTNER_CRYSTAL_PURE;			\
	break;																	\
																			\
case PARTNER_CRYSTAL_ID_WATER_AND_FIRE:										\
	attr_list[BATTLE_ATTR_ELEMENT_WATER] = PARTNER_CRYSTAL_MIX;				\
	attr_list[BATTLE_ATTR_ELEMENT_FIRE] = PARTNER_CRYSTAL_MIX;				\
	break;																	\
																			\
case PARTNER_CRYSTAL_ID_EARTH_AND_WATER:									\
	attr_list[BATTLE_ATTR_ELEMENT_EARTH] = PARTNER_CRYSTAL_MIX;				\
	attr_list[BATTLE_ATTR_ELEMENT_WATER] = PARTNER_CRYSTAL_MIX;				\
	break;																	\
																			\
case PARTNER_CRYSTAL_ID_WIND_AND_EARTH:										\
	attr_list[BATTLE_ATTR_ELEMENT_WIND] = PARTNER_CRYSTAL_MIX;				\
	attr_list[BATTLE_ATTR_ELEMENT_EARTH] = PARTNER_CRYSTAL_MIX;				\
	break;																	\
																			\
case PARTNER_CRYSTAL_ID_FIRE_AND_WIND:										\
	attr_list[BATTLE_ATTR_ELEMENT_FIRE] = PARTNER_CRYSTAL_MIX;				\
	attr_list[BATTLE_ATTR_ELEMENT_WIND] = PARTNER_CRYSTAL_MIX;				\
	break;																	\
}																			\
} while (0)



// ���� =  ���� / 24 + ħ�� / 20 + ���� / 4 + ���� / 4 + ���� / 4 + �ظ� / 2.4 + ���� / 2.4
enum PARTNER_ELEMENT_TYPE		// ���Ԫ�� - ֻ�� ˮ������ ���� 
{
	PARTNER_ELE_WATER = 0,		// ˮԪ��
	PARTNER_ELE_FLAME = 1,		// ��Ԫ��
	PARTNER_ELE_STORM = 2,		// ��Ԫ��
	PARTNER_ELE_EARTH = 3,		// ��Ԫ��

	PARTNER_ELE_MAX
};

enum PARTNER_CRYSTAL_TYPE		// ����Ԫ��ˮ��
{
	PARTNER_CRYSTAL_WATER				= 0,			// ˮ��ˮ��
	PARTNER_CRYSTAL_FIRE				= 1,			// ���ˮ��
	PARTNER_CRYSTAL_WIND				= 2,			// ���ˮ��
	PARTNER_CRYSTAL_EARTH				= 3,			// �ص�ˮ��

	PARTNER_CRYSTAL_WATER_AND_FIRE		= 4,			// ˮ���ˮ��
	PARTNER_CRYSTAL_EARTH_AND_WATER		= 5,			// ��ˮ��ˮ��
	PARTNER_CRYSTAL_WIND_AND_EARTH		= 6,			// ��ص�ˮ��
	PARTNER_CRYSTAL_FIRE_AND_WIND		= 7,			// ����ˮ��

	PARTNER_CRYSTAL_TYPE_MAX
};

enum PARTNER_CRYSTAL_ITEMID		// ���Ԫ��ˮ����id
{
	PARTNER_CRYSTAL_ID_START			= 40000,

	PARTNER_CRYSTAL_ID_WATER			= PARTNER_CRYSTAL_ID_START,					// ˮ��ˮ��_id
	PARTNER_CRYSTAL_ID_FIRE				= 40001,									// ���ˮ��_id
	PARTNER_CRYSTAL_ID_WIND				= 40002,									// ���ˮ��_id
	PARTNER_CRYSTAL_ID_EARTH			= 40003,									// �ص�ˮ��_id

	PARTNER_CRYSTAL_ID_WATER_AND_FIRE	= 40004,									// ˮ���ˮ��_id
	PARTNER_CRYSTAL_ID_EARTH_AND_WATER	= 40005,									// ��ˮ��ˮ��_id
	PARTNER_CRYSTAL_ID_WIND_AND_EARTH	= 40006,									// ��ص�ˮ��_id
	PARTNER_CRYSTAL_ID_FIRE_AND_WIND	= 40007,									// ����ˮ��_id

	PARTNER_CRYSTAL_ID_END
};

enum PARTNER_CRYSTAL_VALUE	// ���Ԫ��ˮ����ֵ  �� = 10  �� = 5  �����Ԫ�� = 0
{
	PARTNER_CRYSTAL_NULL		= 0,
	PARTNER_CRYSTAL_MIX			= 5,
	PARTNER_CRYSTAL_PURE		= 10
};

enum PARTNER_SKILL_TYPE
{
	PARTNER_SKILL_COMM_1 = 0,		// ��ͨ���� 1
	PARTNER_SKILL_COMM_2 = 1,		// ��ͨ���� 2
	PARTNER_SKILL_COMM_3 = 2,		// ��ͨ���� 3
	PARTNER_SKILL_TALENT = 3,		// �츳����

	PARTNER_SKILL_MAX
};

enum TALENT_SKILL_EFFECT_TYPE
{
	TALENT_SKILL_TYPE_INVALID = 0,				// ��Ч���츳��������

	TALENT_PLAYER_ATTR_VALUE_INCR = 1,			// ���ӹ̶�����
	TALENT_PLAYER_WITHOUT_ANY_EFF = 2,			// û���κ�Ч��
	TALENT_PLAYER_PASSIVE_SKILL = 3,            // ��������

	TALENT_SKILL_TYPE_MAX
};

enum TALENT_SKILL_TARGET_TYPE
{
	TALENT_TARGET_PLAYER = 0,		// �������ΪĿ��
	TALENT_TARGET_MYSELF = 1,		// ��������ΪĿ��

	TALENT_TARGET_MAX
};

enum PARTNER_ATTR		//!< �����������
{
	PARTNER_ATTR_MHP = 0,	// ����
	PARTNER_ATTR_MMP = 1,	// ħ��
	PARTNER_ATTR_ATK = 2,	// ����
	PARTNER_ATTR_DEF = 3,	// ����
	PARTNER_ATTR_AGI = 4,	// ����
	PARTNER_ATTR_REC = 5,	// �ظ�
	PARTNER_ATTR_SPI = 6,	// ����

	PARTNER_ATTR_MAX
};

static const float MARK_SCALE_VALUE_LIST[PARTNER_ATTR_MAX] = { 24.0f, 20.0f, 4.0f, 4.0f, 4.0f, 2.4f, 2.4f };

//��� ����/��Լ ��������->ս������ ��Ӧ
static const int  PARTNER_ATTR_BATTLE_HAS[PARTNER_ATTR_MAX] = {
	BATTLE_ATTR_MAX_HP,		// 6 ���Ѫ��
	BATTLE_ATTR_MAX_MP,		// 8 ���ħ��
	BATTLE_ATTR_ATTACK,		// 0 ���� 
	BATTLE_ATTR_DEFENSE,	// 1 ���� 
	BATTLE_ATTR_AGILITY,	// 2 ���� 
	BATTLE_ATTR_RECOVERY,	// 3 �ظ�
	BATTLE_ATTR_MENTAL,		// 4 ����
};
UNSTD_STATIC_CHECK(PARTNER_ATTR_MAX == ARRAY_ITEM_COUNT(PARTNER_ATTR_BATTLE_HAS));

enum PARTNER_RESI		// ���ֿ�����
{
	PARTNER_RESI_TOXIC = 0,		// �綾�ֿ�
	PARTNER_RESI_SLEEP = 1,		// ���ߵֿ�
	PARTNER_RESI_STONE = 2,		// ʯ���ֿ�
	PARTNER_RESI_CHAOS = 3,		// ���ҵֿ�
	PARTNER_RESI_DRUNK = 4,		// ��Ƶֿ�

	PARTNER_RESI_MAX
};

enum PARTNER_AMEND		// �����������
{
	PARTNER_AMEN_CRI = 0,		// ��ɱ����
	PARTNER_AMEN_HIT = 1,		// ��������
	PARTNER_AMEN_EVA = 2,		// ��������
	PARTNER_AMEN_FBK = 3,		// ��������
	PARTNER_AMEN_MAK = 4,		// ħ������
	PARTNER_AMEN_MRS = 5,		// ħ������
	PARTNER_AMEN_BISHA_DMG = 6,		// ��ɱ�˺�
	PARTNER_AMEN_TEN = 7,		// ��������	

	PARTNER_AMEN_MAX
};

enum PARTNER_PROFESSION
{
	PARTNER_PROF_NEWCOMER = 0,			// ����
	PARTNER_PROF_PUGILIST = 1,			// ��ʿ
	PARTNER_PROF_SWORDMAN = 2,			// ��ʿ
	PARTNER_PROF_ROMAHAWK = 3,			// ս����ʿ
	PARTNER_PROF_HORSEMAN = 4,			// ��ʿ
	PARTNER_PROF_MARKSMAN = 5,			// ������
	PARTNER_PROF_MAGICIAN = 6,			// ħ��ʦ
	PARTNER_PROF_MINISTER = 7,			// ����ʿ
	PARTNER_PROF_SORCERER = 8,			// ��ʦ

	PARTNER_PROF_MAX
};

enum FETTER_QUALITY		// �Ʒ��ö��
{
	FETTER_NONE = 0,	// �Ƿ�Ʒ��

	FETTER_WHITES = 1,	// ��
	FETTER_GRASSY = 2,	// ��
	FETTER_BLUE	  = 3,	// ��
	FETTER_PURPLE = 4,	// ��
	FETTER_ORANGE = 5,	// ��
	PETTER_RED    = 6,  // ��

	FETTER_MAX
};

enum PartnerQualityT		// ���Ʒ��
{
	PQT_MIN = 0,

	PARTNERQUALITYT_WHITES		= 1,	// ��
	PARTNERQUALITYT_GRASSY		= 2,	// ��
	PARTNERQUALITYT_BLUE		= 3,	// ��
	PARTNERQUALITYT_PURPLE		= 4,	// ��
	PARTNERQUALITYT_ORANGE		= 5,	// ��
	PARTNERQUALITYT_RED			= 6,	// ��
	PARTNERQUALITYT_GOLD		= 7,	// ��

	PQT_MAX
};

enum QUALITY_UPGRADE_TYPE  //��Ʒ����
{
	QUALITY_TYPE_MIN					= 1,

	QUALITY_TYPE_BLUE_TO_PURPLE			= 1,			//!< ��  --> ��
	QUALITY_TYPE_PURPLE_TO_ORANGE		= 2,			//!< ��  --> ��
	QUALITY_TYPE_ORANGE_TO_RED			= 3,			//!< ��  --> ��
	QUALITY_TYPE_RED_TO_GOLD			= 4,			//!< ��  --> ��
	QUALITY_TYPE_GOLD					= 5,			//!< ��ɫ

	QUALITY_TYPE_MAX,
};

enum OTHER_PARAM_TYPE
{
	LOCK_STATE_LOCKED = 0,		// ����״̬ - ����
	LOCK_STATE_UNLOCK = 1,		// ����״̬ - ����

	BATTLE_STATE_AWAIT = 0,		// ս��״̬ - ����
	BATTLE_STATE_FIGHT = 1,		// ս��״̬ - ��ս

	UNLOCK_TYPE_TASK = 0,		// ����ԭ�� - ����
	UNLOCK_TYPE_ITEM = 1,		// ����ԭ�� - ����
	UNLOCK_TYPE_ITEM_NOREX = 2,	// ����ԭ�� - ��������

	MAX_TYPE_ONLY_CHP = 0,		// ֻˢ����ǰ HP
	MAX_TYPE_ONLY_CMP = 1,		// ֻˢ����ǰ MP
	MAX_TYPE_BOTH_TWO = 2,		// ��ǰ HP �� MP ��ˢ��

	POP_UP_WINDOWS_NONE = 1,	// �������� - С���� - ���赯��
	POP_UP_WINDOWS_TRUE = 2,	// �������� - ����� - ��Ҫ����

	NOT_ACTIVE = 0,				// δ����
	HAS_ACTIVE = 1,				// ����

	SUB_ATTR = 0,				// ��������
	ADD_ATTR = 1,				// ��������
};

enum PARTNER_NATAL_HOROSCOPE_TYPE
{
	PE_TYPE_SKILY = 0,		// ��֮����
	PE_TYPE_EARTH = 1,		// ��֮����
	PE_TYPE_HUMAN = 2,		// ��֮����

	PE_TYPE_MAX
};

enum PARTNER_EQ_SUIT_EFFECT_TYPE
{
	PE_EFFECT_INVALID = 0,			// ��Ч���ԣ�
	PE_EFFECT_LIFE_STEAL = 1,		// ������ȡ��	��ȡ��(%)
	PE_EFFECT_PSY_DMG_AD = 2,		// ���˸��ӣ�	����ֵ		���ӱ�(%)
	PE_EFFECT_MGC_DMG_AD = 3,		// ���˸��ӣ�	����ֵ		���ӱ�(%)

	PE_EFFECT_MAX
};

struct PartnerEquipment
{
	PartnerEquipment() { this->Reset(); }

	void Reset()
	{
		partner_equip_id = 0;
		unlock_level = MAX_ROLE_LEVEL;
		attr_type = 0;
		attr_value = 0;
	}

	void ResetByPutOff()
	{
		partner_equip_id = 0;
		attr_type = 0;
		attr_value = 0;
	}

	ItemID partner_equip_id;
	UInt16 unlock_level;
	int attr_type;
	int attr_value;
};

struct PartnerCon			// ����������� ���� Э���ʽ������ʽ
{
	PartnerCon() { this->Reset(); }

	void Reset()
	{
		partner_id = 0;
		is_unlocked = 0;
		fight_state = 0;

		upgrade_lv = 0;
		attr_stage = 0;

		memset(skill_list, 0, sizeof(skill_list));
		memset(curr_fetter, 0, sizeof(curr_fetter));
		memset(wash_fetter, 0, sizeof(wash_fetter));

		for (int type = 0; type < PE_TYPE_MAX; ++type)
		{
			equip_list[type].Reset();
		}
	}

	// �б� - 4
	unsigned short partner_id;					// ��� ID - [0, 65535]
	char is_unlocked;							// ����״̬��0 - ������1 - ����
	char fight_state;							// ս��״̬��0 - ������1 - ��ս
	// ���� - 2
	unsigned char upgrade_lv;					// ���׵ȼ� - [0, 255](�ҾͲ��Ų߻��ܸ�����䵽 255 ��)
	char attr_stage;							// �����Ѿ�ͻ�ƵĽ׶Σ�[0, maxBreakStage]�����׳ɹ��󣬴����ݻᱻ����Ϊ 0
	
	short skill_list[PARTNER_COMMON_SKILL_MAX];	// �������ý��� 0Ϊ���� 1Ϊ����

	// ��Ե - 4
	unsigned char curr_fetter[PARTNER_FETTER_COUNT_MAX];	// ��ǰ��Ե
	unsigned char wash_fetter[PARTNER_FETTER_COUNT_MAX];	// ϴ����Ե

	// װ�� - 12 * 3 = 36
	PartnerEquipment equip_list[PE_TYPE_MAX];	// ��������
};

struct PartnerSql	// ���������ֻ��� ���� ����ʽ
{
	PartnerSql() { this->Reset(); }

	void Reset()
	{
		cur_hp = 0;
		cur_mp = 0;
		max_hp = 0;
		max_mp = 0;
	}

	// ��ǰ����ֵ��ħ��ֵ - 8
	Attribute cur_hp;							// ��ǰ����ֵ
	Attribute cur_mp;							// ��ǰħ��ֵ
	Attribute max_hp;							// �������ֵ
	Attribute max_mp;							// ���ħ��ֵ
};

struct PartnerQuality			// �����Ʒ����
{
	PartnerQuality()
	{
		this->Reset();
	}
	void Reset()
	{
		partner_id = 0;
		partner_quality_type = 0;
		old_quality = 0;
		new_quality = 0;
		is_up = 0;
	}
	int partner_id;						// ���ID
	int partner_quality_type;			// ��Ʒ����
	int old_quality;					// ԭ����Ʒ��
	int new_quality;					// ��������Ʒ��
	int is_up;							// �Ƿ������  ����
};

struct PartnerCrytal		// װ��Ԫ��ˮ��
{
	PartnerCrytal()
	{
		this->Reset();
	}
	void Reset()
	{
		current_crytal_id = 0;
		use_time = 0;
		memset(att_list, 0, sizeof(att_list));
	}
	ItemID current_crytal_id;					// Ԫ��ˮ��id
	short use_time;								// ʣ��ʹ�ô���
	int att_list[PARTNER_ELE_MAX];				// Ԫ������ֵ ����Ϊ[ˮ���𡢷硢��]
};

struct QualitiedAtt				// ������Ʒ���Ʒ��
{
	QualitiedAtt()
	{
		this->Reset();
	}

	void Reset()
	{
		partner_id = 0;
		memset(att, 0, sizeof(att));
	}

	int partner_id;
	int att[PARTNER_ATTR_MAX];
};

struct PartnerUseElementCrytalFightInfo		// ˮ���·�
{
	PartnerUseElementCrytalFightInfo()
	{
		this->Reset();
	}
	void Reset()
	{
		partner_id = 0;
		crytal_id = 0;
		use_time = 0;
	}

	int partner_id;								// ���id
	int crytal_id;								// װ����ˮ��id
	int use_time;								// ʣ��ʹ�ô���
};

struct PartnerSkinInfo
{
	enum SkinUseStatus
	{
		UNACTIVE	 = 0,
		ACTIVE		 = 1,
		PUT_ON		 = 2,
		PUT_OFF		 = 3,
	};
	PartnerSkinInfo()
	{
		this->Reset();
	}

	void Reset()
	{
		partner_id = 0;
		size = 0;
		memset(skin_id_list, 0, sizeof(skin_id_list));
		memset(skin_use, UNACTIVE, sizeof(skin_use));
	}

	short skin_id_list[PARTNER_SKIN_ONCE];		// ӵ�е�Ƥ��id  [�Ѽ����]
	char skin_use[PARTNER_SKIN_ONCE];			// 0 - δ����  1 - ����  2 - ����  3- δ����
	int partner_id;
	int size;									// ӵ��Ƥ���ĸ���
};

struct StoreNatalCard
{
	StoreNatalCard() { this->Reset(); }
	void Reset()
	{
		buy_count = 0;
		group_id = 0;
		seq = -1;
		attr_type = -1;
		attr_value = 0;
	}
	char buy_count;
	char group_id;
	short seq;
	int attr_type;
	int attr_value;
};


/****************************** ��������Ϣ���ݽṹ**************************************/
struct PartnerBaseInfo
{
	enum ReverNum
	{
		LONG_LONG_REVER_MAX = 6,			// Ԥ�������ֳ���
	};

	enum InitBitUse							// ��ʼ��ʹ��λ һ���ֽ�  �綼ʹ�ù��ˣ�������������
	{
		INIT_BIT_MIN	= 0,

		INIT_BIT_0		= INIT_BIT_MIN,					// ��ʹ�� [��ʼ���� ����  ����  Ԫ��ˮ��]
		INIT_BIT_1		=	1,
		INIT_BIT_2		=	2,
		INIT_BIT_3		=	3,
		INIT_BIT_4		=	4,
		INIT_BIT_5		=	5,
		INIT_BIT_6		=	6,
		INIT_BIT_7		=	7,

		INIT_BIT_MAX,
	};

	PartnerBaseInfo() { this->Reset(); }

	void Reset()
	{
		partner_list.Reset();
		partner_data.Reset();
		partner_crytal.Reset();

		partner_quality_type = 0;
		old_quality = 0;
		new_quality = 0;
		is_init = 0;
		skin_active_status = 0;
		cur_skin_id = 0;
		capability = 0;

		battle_pos = 0;				
		quality_segment = 0;
		is_set_partner_quality_segment_flag = 0;
		biography_open_flag = 0;
		level = 1;
		biography_fetch_flag = 0;
		reserve_ch = 0;
		reserve_sh = 0;
		reserve_int = 0;

		for (int i = 0;i < LONG_LONG_REVER_MAX;++i)
		{
			reserved_ll[i] = 0;
		}
	}

	bool IsInvalid() const { return (partner_list.partner_id <= 0) || (partner_list.partner_id >= PARTNER_NUM_MAX); }
	unsigned short GetPartnerId()const {return partner_list.partner_id;}

	bool IsInitBitUse(InitBitUse bit)
	{
		if (bit < INIT_BIT_MIN || bit >= INIT_BIT_MAX) return false;
		if ((is_init & (MOVE_BYTE_BASE << bit)) != (MOVE_BYTE_BASE << bit)) return false;

		return true;
	}
	bool InitBitSet(InitBitUse bit)
	{
		if (bit < INIT_BIT_MIN || bit >= INIT_BIT_MAX) return false;
		
		is_init |= (MOVE_BYTE_BASE << bit);

		return true;
	}

	void InitBitClear(InitBitUse bit)
	{
		if (bit < INIT_BIT_MIN || bit >= INIT_BIT_MAX) return;

		is_init &= ~(MOVE_BYTE_BASE << bit);

	}

	void InitBitClearAll()
	{
		is_init &= (~is_init);
	}

	PartnerCon partner_list;
	PartnerSql partner_data;

	char partner_quality_type;			// ��Ʒ����  
	char old_quality;					// ԭ����Ʒ��
	char new_quality;					// ��������Ʒ��
	unsigned char is_init;				// �û���Ƿ��ʼ��(������) ��ֹ��������ʱ���³�ʼ��

	PartnerCrytal partner_crytal;

	short skin_active_status;						// Ƥ������״̬  skin_status |= (1 << Ƥ��seq(����skin_seq)) ������  
	short cur_skin_id;								// ��ǰ����Ƥ��id

	int capability;									// ս����

	char battle_pos;								// ս�� 0:����  1:ǰ��
	char quality_segment;							// ����
	char is_set_partner_quality_segment_flag;		// �������޸Ķ�λ�Ƿ����ñ�ʶ  4:������
	char biography_open_flag;						// �����������ʶ(����) 0:û�򿪹� 1:�򿪹�
	int level;
	char biography_fetch_flag;						// �����������ʶ(����) 0:δ��ȡ 1:����ȡ
	char reserve_ch;
	short reserve_sh;
	int reserve_int;
	long long reserved_ll[LONG_LONG_REVER_MAX];		// ����
};

struct PartnerParam
{
	/*************** **********************/
	PartnerParam() { this->Reset(); }

	void Reset()
	{
		for (int i = 0; i < PARTNER_NUM_MAX;++i)
		{
			partner_base_info[i].Reset();
		}
		memset(synfight_list, 0, sizeof(synfight_list));

		store_next_auto_refresh_time = 0;
		for (int i = 0; i < MYSTERY_STORE_MAX_SEQ; ++i)
		{
			store_natal_card[i].Reset();
		}
		new_partner_unlock_flag = 0;
		is_recover_item_flag = 0;
		is_refresh_store_flag = 0;
		tranf_id = 1;

		store_refresh_num = 0;
		store_ver = 0;
		next_free_refresh_time = 0u;

		partner_skill_tree_back_flag = 0;
		reserve_ch = 0;
		reserve_sh = 0;
	}

	PartnerBaseInfo partner_base_info[PARTNER_NUM_MAX]; // partnet_id ��Ϊ����  [0] ���ǿ�
	int synfight_list[PARTNER_SYNFIGHT_NUM_MAX];

	unsigned int	store_next_auto_refresh_time;
	StoreNatalCard  store_natal_card[MYSTERY_STORE_MAX_SEQ];		//�����̵�ˢ������

	char new_partner_unlock_flag;
	char is_recover_item_flag;			// ������Һ 3:�ѻ���
	char is_refresh_store_flag;			// 4����ˢ�� �������̵꣩
	char tranf_id;

	short store_refresh_num;			// �����̵�ˢ�´���(ÿ��ˢ��)
	short store_ver;					// �汾�� �汾������ˢ��
	unsigned int next_free_refresh_time;// �´����ˢ��ʱ���

	char partner_skill_tree_back_flag;
	char reserve_ch;
	short reserve_sh;
};

typedef char PartnerParamHex[sizeof(PartnerParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(PartnerParamHex) < 14000);

/******************************�»�������Ϣ���ݽṹ***********************************/
struct PartnerNatalCardVoc
{
	int attr_type;
	int attr_value;
};

const static int MAX_PARTNER_NATAL_CARD_VOC_NUM = 4;//���������
struct NewStoreNatalCard
{
	NewStoreNatalCard() { this->Reset(); }
	void Reset()
	{
		buy_count = 0;
		group_id = 0;
		seq = -1;
		for (int i = 0; i < MAX_PARTNER_NATAL_CARD_VOC_NUM; i++)
		{
			m_voc_list[i].attr_type = 0;
			m_voc_list[i].attr_value = 0;
		}
	}
	char buy_count;
	char group_id;
	short seq;
	
	PartnerNatalCardVoc m_voc_list[MAX_PARTNER_NATAL_CARD_VOC_NUM];
};

struct NewPartnerEquipment
{
	NewPartnerEquipment() { this->Reset(); }

	void Reset()
	{
		partner_equip_id = 0;
		unlock_level = MAX_ROLE_LEVEL;
		for (int i = 0; i < MAX_PARTNER_NATAL_CARD_VOC_NUM; i++)
		{
			m_voc_list[i].attr_type = 0;
			m_voc_list[i].attr_value = 0;
		}
	}

	void ResetByPutOff()
	{
		partner_equip_id = 0;
		for (int i = 0; i < MAX_PARTNER_NATAL_CARD_VOC_NUM; i++)
		{
			m_voc_list[i].attr_type = 0;
			m_voc_list[i].attr_value = 0;
		}
	}
	const char* GetAttrInfoLog(PartnerNatalCardVoc* attr_list, int attr_num)
	{
		static const int BUFFER_LEN = 512;
		static char buffer[BUFFER_LEN];

		memset(buffer, 0, sizeof(buffer));
		int cur_len = 0;
		for (int i = 0; i < attr_num; ++i)
		{
			int len = SNPRINTF(buffer + cur_len, BUFFER_LEN - cur_len, "[type:%d,value:%d]",
				(int)attr_list[i].attr_type, (int)attr_list[i].attr_value);
			if (len <= 0)
			{
				break;
			}
			cur_len += len;
		}

		return buffer;
	}
	ItemID partner_equip_id;
	UInt16 unlock_level;
	
	PartnerNatalCardVoc m_voc_list[MAX_PARTNER_NATAL_CARD_VOC_NUM];
};

struct NewPartnerCon			// ����������� ���� Э���ʽ������ʽ
{
	NewPartnerCon() { this->Reset(); }

	void Reset()
	{
		partner_id = 0;
		is_unlocked = 0;
		fight_state = 0;

		upgrade_lv = 0;
		attr_stage = 0;

		memset(skill_list, 0, sizeof(skill_list));
		memset(curr_fetter, 0, sizeof(curr_fetter));
		memset(wash_fetter, 0, sizeof(wash_fetter));

		for (int type = 0; type < PE_TYPE_MAX; ++type)
		{
			equip_list[type].Reset();
		}
	}

	// �б� - 4
	unsigned short partner_id;					// ��� ID - [0, 65535]
	char is_unlocked;							// ����״̬��0 - ������1 - ����
	char fight_state;							// ս��״̬��0 - ������1 - ��ս
												// ���� - 2
	unsigned char upgrade_lv;					// ���׵ȼ� - [0, 255](�ҾͲ��Ų߻��ܸ�����䵽 255 ��)
	char attr_stage;							// �����Ѿ�ͻ�ƵĽ׶Σ�[0, maxBreakStage]�����׳ɹ��󣬴����ݻᱻ����Ϊ 0

	short skill_list[PARTNER_COMMON_SKILL_MAX];	// �������ý��� 0Ϊ���� 1Ϊ����

												// ��Ե - 4
	unsigned char curr_fetter[PARTNER_FETTER_COUNT_MAX];	// ��ǰ��Ե
	unsigned char wash_fetter[PARTNER_FETTER_COUNT_MAX];	// ϴ����Ե

															// װ�� - 12 * 3 = 36
	NewPartnerEquipment equip_list[PE_TYPE_MAX];	// ��������
};

struct NewPartnerBaseInfo
{
	enum ReverNum
	{
		LONG_LONG_REVER_MAX = 5,			// Ԥ�������ֳ���
	};

	enum InitBitUse							// ��ʼ��ʹ��λ һ���ֽ�  �綼ʹ�ù��ˣ�������������
	{
		INIT_BIT_MIN = 0,

		INIT_BIT_0 = INIT_BIT_MIN,					// ��ʹ�� [��ʼ���� ����  ����  Ԫ��ˮ��]
		INIT_BIT_1 = 1,
		INIT_BIT_2 = 2,
		INIT_BIT_3 = 3,
		INIT_BIT_4 = 4,
		INIT_BIT_5 = 5,
		INIT_BIT_6 = 6,
		INIT_BIT_7 = 7,

		INIT_BIT_MAX,
	};

	NewPartnerBaseInfo() { this->Reset(); }

	void Reset()
	{
		partner_list.Reset();
		partner_data.Reset();
		partner_crytal.Reset();

		partner_quality_type = 0;
		old_quality = 0;
		new_quality = 0;
		is_init = 0;
		skin_active_status = 0;
		cur_skin_id = 0;
		capability = 0;

		battle_pos = 0;
		quality_segment = 0;
		is_set_partner_quality_segment_flag = 0;
		biography_open_flag = 0;
		level = 1;
		biography_fetch_flag = 0;
		intensify_level = 0;
		soar_count = 0;

		memset(put_on_pact_id_list, 0, sizeof(put_on_pact_id_list));
		reserve_sh_1 = 0;

		reserve_sh = 0;
		soar_arms_level = 0;

		for (int i = 0; i < LONG_LONG_REVER_MAX; ++i)
		{
			reserved_ll[i] = 0;
		}
	}

	bool IsInvalid() const { return (partner_list.partner_id <= 0) || (partner_list.partner_id >= PARTNER_NUM_MAX); }
	unsigned short GetPartnerId()const { return partner_list.partner_id; }

	bool IsInitBitUse(InitBitUse bit)
	{
		if (bit < INIT_BIT_MIN || bit >= INIT_BIT_MAX) return false;
		if ((is_init & (MOVE_BYTE_BASE << bit)) != (MOVE_BYTE_BASE << bit)) return false;

		return true;
	}
	bool InitBitSet(InitBitUse bit)
	{
		if (bit < INIT_BIT_MIN || bit >= INIT_BIT_MAX) return false;

		is_init |= (MOVE_BYTE_BASE << bit);

		return true;
	}

	void InitBitClear(InitBitUse bit)
	{
		if (bit < INIT_BIT_MIN || bit >= INIT_BIT_MAX) return;

		is_init &= ~(MOVE_BYTE_BASE << bit);

	}

	void InitBitClearAll()
	{
		is_init &= (~is_init);
	}

	NewPartnerCon partner_list;
	PartnerSql partner_data;

	char partner_quality_type;			// ��Ʒ����  
	char old_quality;					// ԭ����Ʒ��
	char new_quality;					// ��������Ʒ��
	unsigned char is_init;				// �û���Ƿ��ʼ��(������) ��ֹ��������ʱ���³�ʼ��

	PartnerCrytal partner_crytal;

	short skin_active_status;						// Ƥ������״̬  skin_status |= (1 << Ƥ��seq(����skin_seq)) ������  
	short cur_skin_id;								// ��ǰ����Ƥ��id

	int capability;									// ս����

	char battle_pos;								// ս�� 0:����  1:ǰ��
	char quality_segment;							// ����
	char is_set_partner_quality_segment_flag;		// �������޸Ķ�λ�Ƿ����ñ�ʶ  4:������
	char biography_open_flag;						// �����������ʶ(����) 0:û�򿪹� 1:�򿪹�
	int level;
	char biography_fetch_flag;						// �����������ʶ(����) 0:δ��ȡ 1:����ȡ
	char intensify_level;							// ǿ���ȼ�	
	short soar_count;								// ����������

	//Ԫ����Լ��Ʒid[װ��index]
	UNSTD_STATIC_CHECK(3 == PARTNER_SPIRIT_PACT_MAX);
	ItemID put_on_pact_id_list[PARTNER_SPIRIT_PACT_MAX];
	short reserve_sh_1;

	short reserve_sh;
	short soar_arms_level;							// ��������������


	long long reserved_ll[LONG_LONG_REVER_MAX];		// ����
};

struct NewPartnerParam
{
	/*************** **********************/
	NewPartnerParam() { this->Reset(); }

	void Reset()
	{
		for (int i = 0; i < PARTNER_NUM_MAX; ++i)
		{
			partner_base_info[i].Reset();
		}
		memset(synfight_list, 0, sizeof(synfight_list));

		store_next_auto_refresh_time = 0;
		for (int i = 0; i < MYSTERY_STORE_MAX_SEQ; ++i)
		{
			store_natal_card[i].Reset();
		}
		new_partner_unlock_flag = 0;
		is_recover_item_flag = 0;
		is_refresh_store_flag = 0;
		reserve_ch = 0;

		store_refresh_num = 0;
		store_ver = 0;
		next_free_refresh_time = 0u;
	}

	NewPartnerBaseInfo partner_base_info[PARTNER_NUM_MAX]; // partnet_id ��Ϊ����  [0] ���ǿ�
	int synfight_list[PARTNER_SYNFIGHT_NUM_MAX];

	unsigned int	store_next_auto_refresh_time;
	NewStoreNatalCard  store_natal_card[MYSTERY_STORE_MAX_SEQ];		//�����̵�ˢ������

	char new_partner_unlock_flag;
	char is_recover_item_flag;			// ������Һ 3:�ѻ���
	char is_refresh_store_flag;			// 4����ˢ�� �������̵꣩
	char reserve_ch;

	short store_refresh_num;			// �����̵�ˢ�´���(ÿ��ˢ��)
	short store_ver;					// �汾�� �汾������ˢ��
	unsigned int next_free_refresh_time;// �´����ˢ��ʱ���
};
typedef char NewPartnerParamHex[sizeof(NewPartnerParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(NewPartnerParamHex) < 20000);

const static int MAX_PARTNER_GIFT_TREE_SKILL_NUM = 15;//�츳���������
const static int MAX_PARTNER_GIFT_TREE_LEVEL = 10;//�츳�����ȼ�

struct PartnerGiftTree
{
	PartnerGiftTree() { this->Reset(); }
	void Reset() 
	{
		memset(gift_tree, 0, sizeof(gift_tree));
	}

	int gift_tree[MAX_PARTNER_GIFT_TREE_SKILL_NUM];
};

struct PartnerGiftTreeParam
{
	PartnerGiftTreeParam() { this->Reset(); }
	void Reset() 
	{
		for (int i = 0; i < PARTNER_NUM_MAX; i++)
		{
			partner_gift_tree_list[i].Reset();
		}
	}

	PartnerGiftTree partner_gift_tree_list[PARTNER_NUM_MAX];
};
typedef char PartnerGiftTreeParamHex[sizeof(PartnerGiftTreeParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(PartnerGiftTreeParamHex) < 5000);

//���--Ԫ����Լ�ֿ�
struct PartnerSpiritPactParam
{
	PartnerSpiritPactParam() { this->Reset(); }
	void Reset()
	{
		reserve_ll = 0;
		memset(grid_list, 0, sizeof(grid_list));
	}

	struct PactGridInfo
	{
		PactGridInfo()
		{
			this->Reset();
		}
		void Reset()
		{
			item_num = 0;
			reserve_int = 0;
		}
		int item_num;
		int reserve_int;
	};

	long long reserve_ll;
	//�ڻ�������������Ʒid��Ĵ���λ�õĹ�ϵ
	PactGridInfo grid_list[PARTNER_SPIRIT_PACT_MAX_GRID];
};
typedef char PartnerSpiritPactParamHex[sizeof(PartnerSpiritPactParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(PartnerSpiritPactParamHex) < 4096);

struct PartnerPro			// ����������� ���� Э���ʽ
{
	PartnerPro() { this->Reset(); }

	void Reset()
	{
		base_info.Reset();
		capability = 0;
		quality_segment = 0;
		quality = 0;
		intensify_level = 0;
		task_finish = 0;
		//attr_count = 0;
		crytal_id = 0;
		use_time = 0;
		skin_id = 0;
		level = 0;
		//memset(attr_pair, 0, sizeof(attr_pair));
		memset(attr_pair, 0, sizeof(attr_pair));
		biography_open_flag = 0;
		biography_fetch_flag = 0;
		soar_count = 0;

		memset(put_on_pact_id_list, 0, sizeof(put_on_pact_id_list));
		reserve_sh_1 = 0;

		memset(attach_base_attr_list, 0, sizeof(attach_base_attr_list));
		soar_arms_level = 0;
		rank = -1;
	}

	NewPartnerCon base_info;

	int capability;				// ս����

	char quality_segment;		// ����
	char task_finish;			// �Ƿ����������������
	char quality;				// Ʒ��
	char intensify_level;		// ǿ���ȼ�
	int attr_pair[BATTLE_ATTR_MAX];
	int crytal_id;				// װ��Ԫ��ˮ��id
	int use_time;				// Ԫ��ˮ��ʣ��ʹ�ô���
	int skin_id;				// ������Ƥ��id, 0��ʾδ���� �оͷ�id
	int level;
	char biography_open_flag;	// �����������ʶ(����) 0:û�򿪹� 1:�򿪹�s
	char biography_fetch_flag;	// �����������ʶ(����) 0:δ��ȡ 1:����ȡ
	short soar_count;			// ����������

	//Ԫ����Լ��Ʒid[װ��index]
	ItemID put_on_pact_id_list[PARTNER_SPIRIT_PACT_MAX];
	short reserve_sh_1;

	//�û�����ڸ�����ԼĿ�����ӵ�����
	Attribute attach_base_attr_list[PARTNER_ATTR_MAX];

	short soar_arms_level;		// ��������������
	short rank;					// ��鱾����������0��ʼ��-1Ϊδ�ϰ�
};

struct PartnerQueryPro			//����ѯЭ��
{
	PartnerQueryPro() { this->Reset(); }

	//�˴�index�İ�ȫ���ϲ��߼���֤[ȡ��seq�Ĳ��ҷ�ʽ����Ϊpartner_id]
	void SetData(const NewPartnerParam & param, int partner_id_)
	{
		partner_id = partner_id_;
		upgrade_lv = param.partner_base_info[partner_id].partner_list.upgrade_lv;
		quality_lv = param.partner_base_info[partner_id].new_quality;
		capability = param.partner_base_info[partner_id].capability;
		skin_id = param.partner_base_info[partner_id].cur_skin_id;
		quality_segment = param.partner_base_info[partner_id].quality_segment;
		intensify_level = param.partner_base_info[partner_id].intensify_level;
		level = param.partner_base_info[partner_id].level;
		soar_count = param.partner_base_info[partner_id].soar_count;
		soar_arms_level = param.partner_base_info[partner_id].soar_arms_level;
	}

	void Reset()
	{
		partner_id = 0;
		upgrade_lv = 0;
		quality_lv = 0;
		capability = 0;
		skin_id = 0;
		quality_segment = 0;
		intensify_level = 0;
		level = 0;
		soar_count = 0;
		soar_arms_level = 0;
		rank = -1;
	}

	unsigned short partner_id;					// ��� ID - [0, 65535]
	unsigned char upgrade_lv;					// ���׵ȼ� - [0, 255](�ҾͲ��Ų߻��ܸ�����䵽 255 ��)
	unsigned char quality_lv;					// ��������
	int capability;								// �������
	short skin_id;								// ������Ƥ��id�� 0Ϊδ����
	char quality_segment;						// ����
	char intensify_level;						// ǿ���ȼ�
	short level;								// ���ȼ�
	short soar_count;							// ����������
	short soar_arms_level;						// ��������������
	short rank;									// ��鱾����������0��ʼ��-1Ϊδ�ϰ�
};

#pragma pack(pop)
#endif