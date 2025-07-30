#ifndef __ITEMBASE_H__
#define __ITEMBASE_H__

#include <string>
#include <map>

#include "servercommon/basedef.h"
#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#include "servercommon/attributedef.hpp"
#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/petdef.h"
#include "servercommon/medaldef.h"
#include "itempool.h"
#include "servercommon/partnerdef.h"
#include "servercommon/roleattrpelletdef.hpp"

const static int OLD_EQUIP_STONE_MOUNT_MAX = 2;			// װ��(����)��ʯ��Ƕ�������
UNSTD_STATIC_CHECK(2 == OLD_EQUIP_STONE_MOUNT_MAX);		// Ӱ�����ݿ⣬�����޸�

const static int NEW_EQUIP_STONE_MOUNT_MAX = 4;			// װ��(����)��ʯ��Ƕ�������
UNSTD_STATIC_CHECK(4 == NEW_EQUIP_STONE_MOUNT_MAX);		// Ӱ�����ݿ⣬�����޸�
static const int MAX_NEW_STONE_MODIFY_VER_NUM = 1;

const static int EQUIP_JEWELRY_ATTR_NUM = 6;		// װ������ǿ�������������
UNSTD_STATIC_CHECK(6 == EQUIP_JEWELRY_ATTR_NUM);		// Ӱ�����ݿ⣬�����޸�

const static int EQUIP_ZHU_LING_SLOT_NUM = 5;			// װ��(����) �����λ���� , Ӱ�����ݿ⣬�����޸�

class Role;
struct ItemDataWrapper;
struct ItemGridData;

const static int MAX_ACCESSORIES_BASE_VOC_NUM = 2;//�������δ�����

enum ACCESSORIES_SUIT_TYPE
{
	ACCESSORIES_SUIT_TYPE_MIN = 0,
	ACCESSORIES_SUIT_TYPE_1,				// ����
	ACCESSORIES_SUIT_TYPE_2,				// ����
	ACCESSORIES_SUIT_TYPE_3,				// ��ħ
	ACCESSORIES_SUIT_TYPE_4,				// ��˫
	ACCESSORIES_SUIT_TYPE_5,				// ����
	ACCESSORIES_SUIT_TYPE_6,				// ����
	ACCESSORIES_SUIT_TYPE_7,				// ����
	ACCESSORIES_SUIT_TYPE_8,				// �׶�
	ACCESSORIES_SUIT_TYPE_9,				// ŭ��
	ACCESSORIES_SUIT_TYPE_10,				// ����
	ACCESSORIES_SUIT_TYPE_11,				// ��ŭ
	ACCESSORIES_SUIT_TYPE_12,				// ���
	ACCESSORIES_SUIT_TYPE_13,				// �ն�
	ACCESSORIES_SUIT_TYPE_14,				// ����
	ACCESSORIES_SUIT_TYPE_15,				// ʥҽ
	ACCESSORIES_SUIT_TYPE_16,				// ��ʦ

	ACCESSORIES_SUIT_TYPE_MAX,
};

enum ITEM_PARAM_TYPE
{
	ITEM_PARAM_TYPE_INVAILD,
	ITEM_PARAM_TYPE_OPTIOAN_GIFT,			//��ѡ���
	ITEM_PARAM_TYPE_OPTIOAN_GIFT_2,		//��ѡ���2| ��ѡ�������
};

enum ITEM_SCORE_LEVEL
{
	ITEM_SCORE_LEVEL_S = 3,
	ITEM_SCORE_LEVEL_A = 2,
	ITEM_SCORE_LEVEL_B = 1,
	ITEM_SCORE_LEVEL_DEF = 0,
};

enum ITEM_BIG_TYPE
{
	I_TYPE_INVALID = -1,							// -1 ��Ч

	I_TYPE_EXPENSE = 0,								// 0 ������ ��ֱ��ʹ��
	I_TYPE_OTHER,									// 1 ����ʹ���� type value ��ò�Ҫֱ��ʹ��
	I_TYPE_GIFT,									// 2 �����
	I_TYPE_MEDAL_DISCARD,							// 3 ѫ��(û����)
	I_TYPE_SPECIAL_PET,								// 4 �������뱳��ʱ���Զ���ɼ�����ﱳ������ȷ���������ԣ�
	I_TYPE_CONVERT_ITEM,							// 5 ���뱳�������ְҵת����Ʒid
	I_TYPE_ZHU_LING,								// 6 ����
	I_TYPE_PARTNER_PACT,							// 7 ���--Ԫ����Լ	
	I_TYPE_HUAN_SHOU_SKILL,							// 8 ���޼�����

	I_TYPE_NET_VALUE_ITEM_BEGIN = 100,				// ���о�ֵ����Ʒ����
	I_TYPE_EQUIPMENT = I_TYPE_NET_VALUE_ITEM_BEGIN,	// 100 װ��
	I_TYPE_PET,										// 101 ����
	I_TYPE_ELEMENT_CRYSTAL,							// 102 Ԫ��ˮ��
	I_TYPE_JEWELRY,									// 103 ����
	I_TYPE_EXP_CRYSTAL,								// 104 ��ˮ��
	I_TYPE_TREASURE_MAP,							// 105 �ر�ͼ
	I_TYPE_PARTNER_NATAL_CARD,						// 106 �������
	I_TYPE_PET_EQUIPMENT,							// 107 ����װ��
	I_TYPE_MEDAL,									// 108 ѫ��
	I_TYPE_PET_SOUL_EQUIP,							// 109 �������
	I_TYPE_GOD_PRINT,								// 110 ��ӡ
	I_TYPE_ACCESSORIES,								// 111 ����
	I_TYPE_DIAN_HUA_SHI,							// 112 �㻯ʯ
	I_TYPE_PET_BADGE,								// 113 �������
	I_TYPE_PEAK_TOKEN,								// 114 ����������
	I_TYPE_ZAO_HUA_EQUIP,							// 115 �컯װ��
	I_TYPE_HOLY_EQUIP,								// 116 ʥ��
	I_TYPE_RUNE_STONE,								// 117 ��ʯ
	I_TYPE_YUAN_HUN_ZHU,							// 118 Ԫ����
	I_TYPE_FU_YU,									// 119 ����

	I_TYPE_NET_VALUE_ITEM_END						// ��ֵ��Ʒ���ͽ���
};
static const int NET_VALLUE_ITEM_COUNT = I_TYPE_NET_VALUE_ITEM_END - I_TYPE_NET_VALUE_ITEM_BEGIN;

enum BAGS_BELONG	// ���ⱳ������
{
	B_BELONG_KNAPSACK = 0,						// ���ﱳ��
	B_BELONG_PET_BAG = 1,						// ���ﱳ��
	B_BELONG_PET_GOD_PRINT = 2,					// ��ӡ����
	B_BELONG_PARTNER_SPIRIT_PACT = 3,			// ���--Ԫ����Լ
	B_BELONG_ZAO_HUA_BAG = 4,					// �컯����
	B_BELONG_HOLY_EQUIP_BAG = 5,				// ʥ������
	B_BELONG_RUNE_STONE_BAG = 6,				// ��ʯ����
	B_BELONG_HUAN_SHOU_BAG = 7,					// ���ޱ���(��ֵ���߲��ܷ�)

	B_BELONG_TYPE_MAX
};

union NetValueItemParam;
class ItemBase
{
public:
	static const UInt16 INVALID_ITEM_ID = 0;
	static const int MAX_EQUIP_LEVEL = 200;			// ���װ���ȼ�

	enum ITEM_COLOR
	{
		I_COLOR_INVALID = 0,

		I_COLOR_WHITE = 1,								// ��
		I_COLOR_GREEN = 2,								// ��
		I_COLOR_BLUE = 3,								// ��
		I_COLOR_PURPLE = 4,								// ��
		I_COLOR_ORANGE = 5,								// ��
		I_COLOR_RED = 6,								// ��
		I_COLOR_GOD_COLOR_BEGAIN = 7,
		I_COLOR_STAR = I_COLOR_GOD_COLOR_BEGAIN,		//��ҫ
		I_COLOR_GOD = 8,								//��װ
		I_COLOR_GOD_COLOR_END = I_COLOR_GOD,
		I_COLOR_GOLD = 9,								// ��
		I_COLOR_NIELLO = 10,							// �ڽ�

		I_COLOR_MAX,
	};

	enum NOT_DISCARD_REASON							// ���ɶ�������
	{
		NDR_DEFAULT,
		NDR_INLAY,									// ����Ƕ
	};

	ItemBase(short item_type);
	virtual ~ItemBase();

	inline short GetItemType() const { return m_item_type; }
	inline short GetItemSubType() const { return m_item_sub_type; }
	inline UInt16 GetItemId() const { return m_item_id; }
	inline const char * GetItemName() const { return m_item_name; }
	inline short GetColor() const { return m_color; }
	inline short GetLimitLevel() const { return m_limit_level; }
	inline short GetLimitProf() const { return m_limit_prof; }
	inline short GetPileLimit() const { return m_pile_limit; }
	inline short GetItemColumn() const { return m_item_column; }
	inline short GetBagsBelong() const { return m_bags_belong; }

	virtual bool HasInvalidTime() const;
	inline UInt32 GetTimeLength() const { return m_time_length; }
	virtual UInt32 CalInvalidTime() const;

	bool IsRecord() const { return m_is_record; }
	bool IsBroadcast() const { return m_is_broadcast; }
	bool IsBind() const { return m_is_bind; }

	virtual bool Init(const char* path, std::string *err) = 0;
	virtual int Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type = ITEM_PARAM_TYPE_INVAILD, void * param = NULL) const = 0;

	virtual int GetSellPrice(NetValueItemParam* param) const { return m_sell_price; }
	virtual bool CanDiscard(NetValueItemParam* param, int *reason) const;
	virtual bool CanSell(NetValueItemParam* param) const { return this->GetSellPrice(param) > 0; }

	bool IsNeedReCalSmeltEntryNum() const;	//�ж��Ƿ���Ҫ����������������

	static NetValueItemParam * CreateParam();
	static NetValueItemParam * CreateParamFromStruct(const NetValueItemParam& param_data);
	static NetValueItemParam * CreateParamFromParamData(const NetValueItemParamData param_data);
	bool HasRandomNetValue(NetValueItemParam* param_data) const;
	void SetRandomNetValueFlag(NetValueItemParam* param_data) const;

	UNSTD_STATIC_CHECK(20 == NET_VALLUE_ITEM_COUNT); // ����ֻ�����ѣ�������ֵ��Ʒ�ǵ�Ҫ������д�������
	virtual void RandomNetValueOnPutItem(Role* role, NetValueItemParam* param_data, int put_reason) const {} // ��ֵ��Ʒ����д�����ֵ�ķ������Ƿ���Ҫ��������ⲿ�������жϣ�

	static bool CanTrade(const ItemGridData *item_data);
	bool CanTrade() const;
	bool CanMerge() const;
	inline int GetTradeMoneyType() const { return m_item_trade_money_type; }
	inline bool IsTradeNeedAnnouncement() const { return m_trade_need_announcement; }
	std::vector<int> GetTradeMinPrice(const ItemDataWrapper* wrapper) const;
	std::vector<int> GetTradeMaxPrice(const ItemDataWrapper* wrapper) const;
	std::vector<int> GetTradeRecommendPrice(const ItemDataWrapper* wrapper) const;
	inline short GetTradeSearchType() const { return m_trade_search_type; }
	inline int GetTradeNumPerGroup() const { return m_trade_num_per_group; }
	inline bool IsLimitDrop() const { return m_is_limit_drop; }
	inline bool HasDropCoolDown() const { return m_drop_cooldown_s > 0; }
	inline int GetDropCoolDown_S() const { return m_drop_cooldown_s; }
	inline bool IsNetValueItem() const { return m_item_type >= I_TYPE_NET_VALUE_ITEM_BEGIN && m_item_type < I_TYPE_NET_VALUE_ITEM_END; }

	static bool CheckTradePrice(const std::vector<int>& min_price_vec, const std::vector<int>& max_price_vec, const std::vector<int>& recommend_price_vec);
protected:
	const short m_item_type;
	ItemID m_item_id;								// ��Ʒid
	short m_color;									// ��ɫ
	bool m_is_bind;									// ��ʼ�Ƿ��
	bool m_can_discard;								// �Ƿ�ɶ���
	bool m_is_broadcast;							// �Ƿ�㲥
	int m_sell_price;								// ���ۼ۸�
	bool m_is_record;								// �Ƿ��¼����־�У����䡢ʹ��
	UInt32 m_time_length;							// ��Чʱ���������Ʒ���㣩
	UInt32 m_invalid_time;							// ʧЧʱ�䣨�ض�ʱ��㣩
	short m_limit_prof;								// ְҵ����
	short m_limit_level;							// �ȼ�����
	short m_pile_limit;								// �ѵ�����
	ItemName m_item_name;
	short m_item_column;							// ��Ʒ�����ĸ���
	short m_bags_belong;							// ������������
	short m_item_sub_type;							//!< ��Ʒ������ \see ITEM_SUB_TYPE

	short m_item_trade_money_type;					//!< �����в��������׻������� \see ITEM_TRADE_MONEY_TYPE
	bool m_trade_need_announcement;					//!< �����в������Ƿ���Ҫ��ʾ
	std::vector<int> m_item_trade_min_price;		//!< �����в������׼�
	std::vector<int> m_item_trade_max_price;		//!< �����в�������߼�
	std::vector<int> m_item_trade_recommend_price;	//!< �����в������Ƽ��۸�
	short m_trade_search_type;						//!< �����в�������������
	int m_trade_num_per_group;						//!< �����в�����ÿ����Ʒ������( >1 ����Ҫ�������ۣ�
	bool m_is_limit_drop;							//!< �Ƿ����Ƶ���[0:�� 1:��] (��Ϊ�ǣ����޷��ӵ����л�ȡ����Ʒ�����Ǳ�ĳЩ���������¿��ţ�
	int m_drop_cooldown_s;							//!< ������ȴ���룩
};

#pragma pack(push, 4)

struct ZhuLinParam
{
	void Reset()
	{
		zhu_ling_item_id = 0;
		reserved_sh = 0;
	}

	ItemID zhu_ling_item_id;
	unsigned short reserved_sh;
};

static const int PET_BADGE_ITEM_ATTR_NUM = 2;

struct PetBadgeItemAttr
{
	unsigned short attr_type;
	unsigned short attr_value;
};

struct PetBadgeItemParam
{
	void Reset()
	{
		has_random_net_value = 0;
		reserved_ch = 0;
		pet_badgei_item_id = 0;
		item_uuid = 0;
		skill_id = 0;
		ArrayFillZero(attrs);
		ArrayFillZero(reserved_ch2);
	}

	unsigned char  has_random_net_value;
	unsigned char reserved_ch;
	unsigned short pet_badgei_item_id;
	long long item_uuid;
	int skill_id;
	AttrInfo attrs[PET_BADGE_ITEM_ATTR_NUM];
	unsigned char reserved_ch2[16];
};

struct EquipmentParam // ��ΪNetValueItemParam��һ����Ա���ýṹ�岻��Ҫ�����reset, ��ԱĬ��ֵΪ0�������ܵ�����ʹ�ã�������NetValueItemParam����
{
	static const int ATTR_NUM_MAX = 6;
	static const int EXA_ATTR_NUM_MAX = 6;
	UNSTD_STATIC_CHECK(BATTLE_WEAPON_ATTR_NUM == ATTR_NUM_MAX);
	UNSTD_STATIC_CHECK(EQUIP_JEWELRY_ATTR_NUM == ATTR_NUM_MAX);
	int IsSameExtraInc(short attr_type)
	{
		for (int i = 0; i < EXA_ATTR_NUM_MAX; i++)
		{
			if (attr_extra_inc[i].attr_type == attr_type && attr_extra_inc[i].attr_value > 0)return i;
		}
		return -1;
	}

	int HasVacancyStone()
	{
		for (int i = 0; i < NEW_EQUIP_STONE_MOUNT_MAX; i++)
		{
			if (new_stone[i] == 0)return i;
		}
		return NEW_EQUIP_STONE_MOUNT_MAX;
	}

	bool HasStone()
	{
		for (int i = 0; i < NEW_EQUIP_STONE_MOUNT_MAX; i++)
		{
			if (new_stone[i] != 0)return true;
		}
		return false;
	}

	const char* GetAttrInfoLog(AttrInfo* attr_list, int attr_num, short ultimate_attr_list[ATTR_NUM_MAX])
	{
		static const int BUFFER_LEN = 512;
		static char buffer[BUFFER_LEN];

		memset(buffer, 0, sizeof(buffer));
		int cur_len = 0;
		for (int i = 0; i < attr_num; ++i)
		{
			int len = SNPRINTF(buffer + cur_len, BUFFER_LEN - cur_len, "[type:%d,value:%d,ultimate_value:%d],",
				(int)attr_list[i].attr_type, (int)attr_list[i].attr_value, (int)ultimate_attr_list[i]);
			if (len <= 0)
			{
				break;
			}
			cur_len += len;
		}

		return buffer;
	}

	const char* GetAttrVocInfoLog(AttrVocInfo* attr_list, int attr_num)
	{
		static const int BUFFER_LEN = 512;
		static char buffer[BUFFER_LEN];

		memset(buffer, 0, sizeof(buffer));
		int cur_len = 0;
		for (int i = 0; i < attr_num; ++i)
		{
			int len = SNPRINTF(buffer + cur_len, BUFFER_LEN - cur_len, "[type:%d,value:%d,unlock:%d],", (int)attr_list[i].attr_type, (int)attr_list[i].attr_value, (int)attr_list[i].un_lock);
			if (len <= 0)
			{
				break;
			}
			cur_len += len;
		}

		return buffer;
	}

	bool HasZhuLing() const
	{
		for (int i = 0; i < ARRAY_LENGTH(zhu_ling_info); ++i)
		{
			if (zhu_ling_info[i].zhu_ling_item_id)
			{
				return true;
			}
		}

		return false;
	}
	bool CheckModifyVersion();

	char has_random_net_value;					//!< �Ƿ��������ֵ
	char is_treasure;							//!< ��Ʒ����
	short attr_num;								//!< ��������
	AttrInfo attr_list[ATTR_NUM_MAX];			//!< ������Ϣ���飨���鳤��Ϊ6��

	ItemID old_stone[OLD_EQUIP_STONE_MOUNT_MAX];	//!< ��Ƕ�ı�ʯItemID  --> ����

	GameName creator_name;						//!< ����������  GameName����64

	int sp_id;									//!< ��Чid   0Ϊû����Ч

	int inc_num;									//!< ���Զ�����������
	AttrVocInfo attr_extra_inc[EXA_ATTR_NUM_MAX];	//!< ���Զ��������б�

	AttrVocInfo refresh_attr_extra_inc[EXA_ATTR_NUM_MAX];	//!< ���Զ��������б�-ϴ������
	UNSTD_STATIC_CHECK(6 == EXA_ATTR_NUM_MAX);

	//ǿ������
	short ultimate_attr_list[ATTR_NUM_MAX];			//!< ǿ������������Ϣ���飨���鳤��Ϊ6��

	short suit_effect_id;							//!< ��װЧ��
	short is_strong_create;							//!< �Ƿ�ǿ������

	int strengthen_level;							//!< ǿ���ȼ�

	ZhuLinParam zhu_ling_info[EQUIP_ZHU_LING_SLOT_NUM];  //5���������λ

	char modify_ver;									//!< ��ʯǨ�Ʊ��
	char suit_effect_change_flag;
	short new_suit_effect_id;
	ItemID new_stone[NEW_EQUIP_STONE_MOUNT_MAX];		//!< ��Ƕ�ı�ʯItemID
	short reserve_sh_list[6];							//!< Ԥ��һЩ,��Ĺ�������ʹ�����β����ʼ
	//װ���㻯Ч��--����
	int effect_id;
	int effect_level;
	//װ���㻯Ч��--δ����
	int refresh_effect_id;
	int refresh_effect_level;

	int star_level;
};

const static int MAX_PET_EQUIPMENT_SKILL_NUM = 2;//����װ��Я�����������ֵ

struct PetEquipMentParam			// ����װ���Ĳ���
{
	void Reset()
	{
		has_random_net_value = 0;
		reserve_ch = 0;
		reserve_sh = 0;
		pet_equipment_id = 0;
		equipment_skill_num = 0;
		memset(equipment_skill_list, 0, sizeof(equipment_skill_list));
	}
	char has_random_net_value;
	char reserve_ch;
	short reserve_sh;
	//����װ��
	unsigned short pet_equipment_id;
	unsigned short equipment_skill_num;
	short equipment_skill_list[MAX_PET_EQUIPMENT_SKILL_NUM];
	short equipment_skill_refresh_list[MAX_PET_EQUIPMENT_SKILL_NUM];//����ϴ�����ܱ���
};
UNSTD_STATIC_CHECK(MAX_PET_EQUIPMENT_SKILL_NUM == 2);

const static int MAX_PET_SOUL_EQUIPMENT_ATTR_NUM = 5;//���������������

struct PetSoulEquipMentParam			// ��������Ĳ���
{
	void Reset()
	{
		has_random_net_value = 0;
		is_lock = 0;
		sp_id = 0;

		refine = 0;
		level = 0;
		steps = 0;

		pet_equipment_id = 0;
		attr_num = 0;
		memset(add_attr_list, 0, sizeof(add_attr_list));
	}

	const char* GetAttrInfoLog(AttrInfo* attr_list, int attr_num)
	{
		static const int BUFFER_LEN = 512;
		static char buffer[BUFFER_LEN];

		memset(buffer, 0, sizeof(buffer));
		int cur_len = 0;
		for (int i = 0; i < attr_num; ++i)
		{
			int len = SNPRINTF(buffer + cur_len, BUFFER_LEN - cur_len, "[type:%d,value:%d", (int)attr_list[i].attr_type, (int)attr_list[i].attr_value);
			if (len <= 0)
			{
				break;
			}
			cur_len += len;
		}

		return buffer;
	}
	char has_random_net_value;
	char is_lock;
	short sp_id;							//��Ч
	//�������

	int exp;
	int m_max_exp;

	int refine;				// ����
	short level;			// �ȼ�
	short steps;			// ͻ��

	unsigned short pet_equipment_id;
	unsigned short attr_num;
	AttrInfo add_attr_list[MAX_PET_SOUL_EQUIPMENT_ATTR_NUM];
};

struct PetGodPrintEquipMentParam			// ������ӡ�Ĳ���
{
	void Reset()
	{
		has_random_net_value = 0;
		reserve_ch = 0;
		reserve_sh = 0;

		god_print_data.Reset();
	}

	const char* GetAttrInfoLog(AttrInfo* attr_list, int attr_num)
	{
		static const int BUFFER_LEN = 512;
		static char buffer[BUFFER_LEN];

		memset(buffer, 0, sizeof(buffer));
		int cur_len = 0;
		for (int i = 0; i < attr_num; ++i)
		{
			int len = SNPRINTF(buffer + cur_len, BUFFER_LEN - cur_len, "[type:%d,value:%d", (int)attr_list[i].attr_type, (int)attr_list[i].attr_value);
			if (len <= 0)
			{
				break;
			}
			cur_len += len;
		}

		return buffer;
	}

	char has_random_net_value;
	char reserve_ch;
	short reserve_sh;

	GodPrintItem god_print_data;
};

enum PET_SOURCE_TYPE
{
	PET_SOURCE_TYPE_DEFAULT,
	PET_SOURCE_TYPE_TRANSACTION,
};

const static int MAX_PET_MODIFY_VER = 2;

const static int MAX_PET_SOUL_EQUIP_LIST = 2;//����װ����
struct PetSoulEquip
{
	void Reset()
	{
		item_unique_id = 0;
		item_id = 0;
		is_bind = 0;
		is_lock = 0;

		exp = 0;
		m_max_exp = 0;
		m_level = 0;
		m_gread = 0;
		m_refine = 0;

		attr_num = 0;
		for (int i = 0; i < MAX_PET_SOUL_EQUIPMENT_ATTR_NUM; i++)
		{
			add_attr_list[i].attr_type = 0;
			add_attr_list[i].attr_value = 0;
		}

		sp_id = 0;
	}
	bool Invalid() const
	{
		return item_unique_id == 0;
	}
	const char* GetAttrInfoLog(const AttrInfo* attr_list, int attr_num) const
	{
		static const int BUFFER_LEN = 512;
		static char buffer[BUFFER_LEN];

		memset(buffer, 0, sizeof(buffer));
		int cur_len = 0;
		for (int i = 0; i < attr_num; ++i)
		{
			int len = SNPRINTF(buffer + cur_len, BUFFER_LEN - cur_len, "[type:%d,value:%d", (int)attr_list[i].attr_type, (int)attr_list[i].attr_value);
			if (len <= 0)
			{
				break;
			}
			cur_len += len;
		}

		return buffer;
	}
	long long item_unique_id;
	ItemID item_id;
	char is_bind;
	char is_lock;

	int exp;
	int m_max_exp;
	short m_level;//�ȼ�
	short m_gread;//�Ƚ�
	int m_refine;//����

	short sp_id;
	unsigned short attr_num;
	AttrInfo add_attr_list[MAX_PET_SOUL_EQUIPMENT_ATTR_NUM];
};

UNSTD_STATIC_CHECK(sizeof(PetSoulEquip) % 4 == 0);//�ṹ����

enum GOD_PRINT_INDEX_TYPE
{
	GOD_PRINT_INDEX_TYPE_0 = 0,
	GOD_PRINT_INDEX_TYPE_1 = 1,
	GOD_PRINT_INDEX_TYPE_2 = 2,
	GOD_PRINT_INDEX_TYPE_3 = 3,
	GOD_PRINT_INDEX_TYPE_4 = 4,
	GOD_PRINT_INDEX_TYPE_MAX,
};
UNSTD_STATIC_CHECK(GOD_PRINT_INDEX_TYPE_MAX == 5);//�޸���Ҫ�����Ƿ�Ҫת�������ֶ�

enum PET_REPLACE_SKILL_INDEX_TYPE
{
	PET_REPLACE_SKILL_INDEX_TYPE_DEFAULT = 0,
	PET_REPLACE_SKILL_INDEX_TYPE_LIAN_YAO = 1,
};


struct PetParam			// ����Ĳ���,�Ķ�ͬʱ�޸���̨վ�ġ�
{
	void Reset()
	{
		pet_id = 0;
		memset(name, 0, sizeof(name));
		str_level = 0;
		level = 0;
		exp = 0;
		add_exp = 0;
		hp = 0;
		mp = 0;

		reserve_int = 0;

		pet_growth_version = 0;
		auto_add_points_flag = 0;
		reserve_sh = 0;

		reserve_int1 = 0;
		memset(use_attr_pellet_list, 0, sizeof(use_attr_pellet_list));

		memset(growth, 0, sizeof(growth));
		memset(new_growth, 0, sizeof(new_growth));
		comprehend_gift_num = 0;
		comprehend_passive_value = 0;
		lock = 0;
		refined_flag = 0;
		memset(attr, 0, sizeof(attr));
		max_hp = 0;
		max_mp = 0;
		pet_auto_move_type = 0;
		pet_auto_skill_id = 0;
		pet_auto_skill_level = 0;
		guide_pet_flag = 0;
		pet_equipment_is_bind = 0;
		equipment_item_unique_id = 0;
		pet_equipment_id = 0;
		equipment_skill_num = 0;
		memset(equipment_skill_list, 0, sizeof(equipment_skill_list));
		memset(equipment_skill_refresh_list, 0, sizeof(equipment_skill_refresh_list));
		skin_id = 0;

		modify_ver = 0;
		is_up_level_break = 0;
		source = 0;
		has_refresh_points = 0;
		for (int i = 0; i < PET_PASSIVE_MAX_COUNT; i++)
		{
			passive_list[i].Reset();
			new_passive_list[i].Reset();
		}

		replace_skill_id = 0;
		replace_index = 0;
		replace_type = PET_REPLACE_SKILL_INDEX_TYPE_DEFAULT;
		fei_sheng_times = 0;
		fei_sheng_task_jie_duan = 0;

		m_soul_equip_1.Reset();

		memset(god_print_list, 0, sizeof(god_print_list));
		fei_sheng_select_skill_id = 0;
		fei_sheng_select_skill_level = 0;

		pet_bage_item.Reset();

		m_soul_equip_2.Reset();
		lian_yao_pet_id = 0;
		lian_yao_pet_str_lv = 0;
		lian_yao_reserved_sh = 0;
		ArrayCallReset(lian_yao_passive_list);
		ArrayCallReset(talent_list);
		pet_gai_ming_level = 0;
		pet_gai_ming_exp = 0;
		ArrayFillZero(pet_gai_ming_attr_level);
	}
	void ClearGift()
	{
		for (int i = 0; i < ARRAY_LENGTH(passive_list); ++i)
		{
			passive_list[i].passive_skill_id = 0;
			passive_list[i].passive_source = 0;
			passive_list[i].skill_level = 0;
		}
	}

	void ClearNewGift()
	{
		for (int i = 0; i < ARRAY_LENGTH(new_passive_list); ++i)
		{
			new_passive_list[i].passive_skill_id = 0;
			new_passive_list[i].passive_source = 0;
			new_passive_list[i].skill_level = 0;
		}
	}

	void ClearNewGrowth()
	{
		for (int i = 0; i < ARRAY_LENGTH(new_growth); ++i)
		{
			new_growth[i] = 0;
		}
	}

	const char* GetPassiveInfoLog(const PetPassiveItemInfo* passive_list, int passive_num) const
	{
		static const int BUFFER_LEN = 512;
		static char buffer[BUFFER_LEN];

		memset(buffer, 0, sizeof(buffer));
		int cur_len = 0;
		for (int i = 0; i < passive_num; ++i)
		{
			int len = SNPRINTF(buffer + cur_len, BUFFER_LEN - cur_len, "[skill_id:%d,source:%d],", (int)passive_list[i].passive_skill_id, (int)passive_list[i].passive_source);
			if (len <= 0)
			{
				break;
			}
			cur_len += len;
		}

		return buffer;
	}

	bool GetPetEquip(PetEquipMentParam * pet_equipment)
	{
		if (NULL == pet_equipment)return false;
		pet_equipment->has_random_net_value = 1;
		pet_equipment->pet_equipment_id = pet_equipment_id;
		pet_equipment->equipment_skill_num = equipment_skill_num;
		for (int i = 0; i < MAX_PET_EQUIPMENT_SKILL_NUM && i < equipment_skill_num; i++)
		{
			pet_equipment->equipment_skill_list[i] = equipment_skill_list[i];
			pet_equipment->equipment_skill_refresh_list[i] = equipment_skill_refresh_list[i];
		}
		return true;
	}
	bool SetPetEquip(PetEquipMentParam * pet_equipment)
	{
		if (NULL == pet_equipment)return false;

		pet_equipment_id = pet_equipment->pet_equipment_id;
		equipment_skill_num = pet_equipment->equipment_skill_num;
		for (int i = 0; i < MAX_PET_EQUIPMENT_SKILL_NUM && i < equipment_skill_num; i++)
		{
			equipment_skill_list[i] = pet_equipment->equipment_skill_list[i];
			equipment_skill_refresh_list[i] = pet_equipment->equipment_skill_refresh_list[i];
		}

		return true;
	}

	bool GetPetSoulEquip(PetSoulEquipMentParam * pet_equipment,int pet_soul_equipment_index)
	{
		if (NULL == pet_equipment)return false;
		PetSoulEquip * soul_equip = this->GetPetSoulEquipInPetParam(pet_soul_equipment_index);
		if (NULL == soul_equip)return false;
		

		pet_equipment->has_random_net_value = 1;

		pet_equipment->is_lock = soul_equip->is_lock;
		pet_equipment->level = soul_equip->m_level;
		pet_equipment->exp = soul_equip->exp;
		pet_equipment->m_max_exp = soul_equip->m_max_exp;
		pet_equipment->refine = soul_equip->m_refine;
		pet_equipment->steps = soul_equip->m_gread;
		pet_equipment->sp_id = soul_equip->sp_id;

		pet_equipment->attr_num = soul_equip->attr_num;

		for (int i = 0; i < MAX_PET_SOUL_EQUIPMENT_ATTR_NUM; i++)
		{
			pet_equipment->add_attr_list[i].attr_type = soul_equip->add_attr_list[i].attr_type;
			pet_equipment->add_attr_list[i].attr_value = soul_equip->add_attr_list[i].attr_value;
		}
		return true;
	}
	bool SetPetSoulEquip(PetSoulEquipMentParam * pet_equipment, int pet_soul_equipment_index)
	{
		if (NULL == pet_equipment)return false;
		PetSoulEquip * soul_equip_ptr = this->GetPetSoulEquipInPetParam(pet_soul_equipment_index);
		if (NULL == soul_equip_ptr)return false;

		PetSoulEquip & soul_equip = *soul_equip_ptr;

		soul_equip.is_lock = pet_equipment->is_lock;
		soul_equip.m_level = pet_equipment->level;
		soul_equip.exp = pet_equipment->exp;
		soul_equip.m_max_exp = pet_equipment->m_max_exp;
		soul_equip.m_refine = pet_equipment->refine;
		soul_equip.m_gread = pet_equipment->steps;
		soul_equip.sp_id = pet_equipment->sp_id;

		soul_equip.attr_num = pet_equipment->attr_num;

		for (int i = 0; i < MAX_PET_SOUL_EQUIPMENT_ATTR_NUM; i++)
		{
			soul_equip.add_attr_list[i].attr_type = pet_equipment->add_attr_list[i].attr_type;
			soul_equip.add_attr_list[i].attr_value = pet_equipment->add_attr_list[i].attr_value;
		}

		return true;
	}

	PetSoulEquip * GetPetSoulEquipInPetParam(int pet_soul_equipment_index, bool check_lian_yao = true)
	{
		if (pet_soul_equipment_index == 0)
		{
			return &m_soul_equip_1;
		}

		if (pet_soul_equipment_index == 1)
		{		//ֻ��������Ż����Ӹ���
			if (!check_lian_yao || lian_yao_pet_id > 0)
			{
				return &m_soul_equip_2;
			}
		}

		return NULL;
	}

	const PetSoulEquip * GetPetSoulEquipInPetParamConst(int pet_soul_equipment_index, bool check_lian_yao = true) const
	{
		if (pet_soul_equipment_index == 0)
		{
			return &m_soul_equip_1;
		}

		if (pet_soul_equipment_index == 1)
		{
			//ֻ��������Ż����Ӹ���
			if (!check_lian_yao || lian_yao_pet_id > 0)
			{
				return &m_soul_equip_2;
			}
		}

		return NULL;
	}

	void ClearPetEquipment()
	{
		pet_equipment_is_bind = 0;
		equipment_item_unique_id = 0;
		pet_equipment_id = 0;
		equipment_skill_num = 0;
		memset(equipment_skill_list, 0, sizeof(equipment_skill_list));
		memset(equipment_skill_refresh_list, 0, sizeof(equipment_skill_refresh_list));
	}
	void ClearPetSoulEquipment(int pet_soul_equipment_index)
	{
		PetSoulEquip * soul_equip_ptr = this->GetPetSoulEquipInPetParam(pet_soul_equipment_index);
		if (soul_equip_ptr)
		{
			soul_equip_ptr->Reset();
		}		
	}

	bool HasGodPrint()
	{
		for (int i = 0; i < GOD_PRINT_INDEX_TYPE_MAX; i++)
		{
			if (god_print_list[i] > 0)
			{
				return true;
			}
		}
		return false;
	}
	
	bool IsHasPetTalent() const;

	bool CheckGrowthVersion(long long pet_item_unique_id, const char* caller_func);
	bool CheckModifyVersion();

	int GetPetPassiveSkillAndLianYaoPassiveSkillNum();
	std::string ToString(long long pet_uuid) const;
	std::string GetPetBaseInfoString(long long pet_uuid) const;

	int				pet_id;
	GameName		name;
	// ǿ���ȼ�
	unsigned short	str_level;
	// �ȼ�&����
	unsigned short	level;
	long long		exp;
	long long       add_exp;		// ս�������������ӵľ���

	// ��ǰ��Ѫ��������
	int				hp;
	int				mp;

	int	reserve_int;

	char pet_growth_version;
	unsigned char auto_add_points_flag;		//�Զ��ӵ��־��bit
	short reserve_sh;

	int reserve_int1;
	short use_attr_pellet_list[MAX_ATTR_PELLET_TYPE_NUM];

	// ����
	unsigned short	growth[ADD_POINT_TYPE_MAX];
	// ϴ����������
	unsigned short	new_growth[ADD_POINT_TYPE_MAX];

	int comprehend_gift_num;		//û���ˣ����ڲ����������������ˣ������ܵĸ�����ÿ������ֻ������2������
	int comprehend_passive_value;		//�����ܵ�ֵ

	//0ΪĬ��û������  1 Ϊ������
	unsigned char		lock;
	unsigned char		refined_flag;		//ϴ����ʲô��־
	// �ѷ������Ե�
	unsigned short	attr[ADD_POINT_TYPE_MAX];

	// ����Ѫ��������
	int				max_hp;
	int				max_mp;

	short pet_auto_move_type;
	short pet_auto_skill_id;
	short pet_auto_skill_level;
	char guide_pet_flag;
	char pet_equipment_is_bind;			//�˴��Ǹ��������װ����¼��������ﴩ����Ӱ��󶨣��Ա���Ϊ׼��

	//����װ��
	long long equipment_item_unique_id;
	unsigned short pet_equipment_id;
	unsigned short equipment_skill_num;
	short equipment_skill_list[MAX_PET_EQUIPMENT_SKILL_NUM];
	short equipment_skill_refresh_list[MAX_PET_EQUIPMENT_SKILL_NUM];//����ϴ�����ܱ���
	int skin_id;

	char modify_ver;
	char is_up_level_break;			//��ǰ�ȼ��Ƿ�ͻ����
	unsigned char source;			//��Դ PET_SOURCE_TYPE, �Զ��ӵ㹦���õ���ͨ������ĳ�����Զ��ӵ�
	unsigned char has_refresh_points;		//ÿһֻ���ﶼ��һ�����ϴ��Ļ��ᣬϴ��֮��Ͳ��Զ��ӵ�

	// �츳�б�
	PetPassiveItemInfo	passive_list[PET_PASSIVE_MAX_COUNT];
	// ϴ�������츳�б�
	PetPassiveItemInfo	new_passive_list[PET_PASSIVE_MAX_COUNT];

	int replace_skill_id;
	unsigned char replace_index;
	unsigned char replace_type;		//0����ͨindex��1������index

	unsigned char fei_sheng_times;			//��������
	unsigned char fei_sheng_task_jie_duan;			//��������׶� 

	PetSoulEquip m_soul_equip_1;			
	int god_print_list[GOD_PRINT_INDEX_TYPE_MAX];//��ӡ�����±�+1
	int fei_sheng_select_skill_id;
	int fei_sheng_select_skill_level;
	PetBadgeItemParam pet_bage_item;

	PetSoulEquip m_soul_equip_2;//��������2 ,��չ��//ֻ��������Ż����Ӹ���
	int lian_yao_pet_id;		//�����ϲ������ĳ���id
	unsigned short lian_yao_pet_str_lv;		//�����ϲ������ĳ���ǿ���ȼ�
	unsigned short lian_yao_reserved_sh;		
	PetPassiveItemInfo	lian_yao_passive_list[PET_LIAN_YAO_PASSIVE_MAX_COUNT]; //8,�������ӵļ���
	PetTalentInfo talent_list[MAX_PET_TALENT_GROUP_NUM];	//�����츳�б�
	unsigned short pet_gai_ming_level;		//�����ȼ�
	unsigned short pet_gai_ming_exp;		//������ǰ�ȼ�����
	unsigned char pet_gai_ming_attr_level[PET_GAI_MING_ATTR_MAX_NUM];	//16���������Եȼ�
};
UNSTD_STATIC_CHECK(5 == ADD_POINT_TYPE_MAX);
UNSTD_STATIC_CHECK(sizeof(PetBadgeItemParam) == 40);

struct ElementCrystalParam		// Ԫ��ˮ��
{
	short used_times;
	short has_set_net_value;
};

struct JewelryParam			//!< ����
{
	static const int ATTR_NUM_MAX = 6;
	static const int EXA_ATTR_NUM_MAX = 6;
	UNSTD_STATIC_CHECK(BATTLE_WEAPON_ATTR_NUM == ATTR_NUM_MAX);
	UNSTD_STATIC_CHECK(EQUIP_JEWELRY_ATTR_NUM == ATTR_NUM_MAX);
	int IsSameExtraInc(short attr_type)
	{
		for (int i = 0; i < EXA_ATTR_NUM_MAX; i++)
		{
			if (attr_extra_inc[i].attr_type == attr_type && attr_extra_inc[i].attr_value > 0)return i;
		}
		return -1;
	}

	int HasVacancyStone()
	{
		for (int i = 0; i < NEW_EQUIP_STONE_MOUNT_MAX; i++)
		{
			if (new_stone[i] == 0)return i;
		}
		return NEW_EQUIP_STONE_MOUNT_MAX;
	}

	bool HasStone()
	{
		for (int i = 0; i < NEW_EQUIP_STONE_MOUNT_MAX; i++)
		{
			if (new_stone[i] != 0)return true;
		}
		return false;
	}

	bool HasZhuLing() const
	{
		for (int i = 0; i < ARRAY_LENGTH(zhu_ling_info); ++i)
		{
			if (zhu_ling_info[i].zhu_ling_item_id)
			{
				return true;
			}
		}

		return false;
	}

	const char* GetAttrInfoLog(AttrInfo* attr_list, int attr_num, short ultimate_attr_list[ATTR_NUM_MAX])
	{
		static const int BUFFER_LEN = 512;
		static char buffer[BUFFER_LEN];

		memset(buffer, 0, sizeof(buffer));
		int cur_len = 0;
		for (int i = 0; i < attr_num; ++i)
		{
			int len = SNPRINTF(buffer + cur_len, BUFFER_LEN - cur_len, "[type:%d,value:%d,ultimate_value:%d],",
				(int)attr_list[i].attr_type, (int)attr_list[i].attr_value, (int)ultimate_attr_list[i]);
			if (len <= 0)
			{
				break;
			}
			cur_len += len;
		}

		return buffer;
	}

	const char* GetAttrVocInfoLog(AttrVocInfo* attr_list, int attr_num)
	{
		static const int BUFFER_LEN = 512;
		static char buffer[BUFFER_LEN];

		memset(buffer, 0, sizeof(buffer));
		int cur_len = 0;
		for (int i = 0; i < attr_num; ++i)
		{
			int len = SNPRINTF(buffer + cur_len, BUFFER_LEN - cur_len, "[type:%d,value:%d,unlock:%d],", (int)attr_list[i].attr_type, (int)attr_list[i].attr_value, (int)attr_list[i].un_lock);
			if (len <= 0)
			{
				break;
			}
			cur_len += len;
		}

		return buffer;
	}

	bool CheckModifyVersion();

	char has_random_net_value;					//!< �Ƿ��������ֵ
	char is_treasure;							//!< �Ƿ���Ʒ
	short attr_num;								//!< ��������
	AttrInfo attr_list[ATTR_NUM_MAX];			//!< ������Ϣ���飨���鳤��Ϊ6��

	GameName creator_name;						//!< ����������  GameName����32

	int inc_num;									//!< ���Զ�����������
	AttrVocInfo attr_extra_inc[EXA_ATTR_NUM_MAX];	//!< ���Զ��������б�

	AttrVocInfo refresh_attr_extra_inc[EXA_ATTR_NUM_MAX];	//!< ���Զ��������б�-ϴ������
	ItemID old_stone[OLD_EQUIP_STONE_MOUNT_MAX];		//!< ��Ƕ�ı�ʯItemID
	UNSTD_STATIC_CHECK(6 == EXA_ATTR_NUM_MAX);

	//ǿ������
	short ultimate_attr_list[ATTR_NUM_MAX];			//!< ǿ������������Ϣ���飨���鳤��Ϊ6��

	short suit_effect_id;							//!< ��װЧ��
	short is_strong_create;							//!< �Ƿ�ǿ������

	int strengthen_level;							//!< ǿ���ȼ�

	ZhuLinParam zhu_ling_info[EQUIP_ZHU_LING_SLOT_NUM];  //5���������λ

	char modify_ver;									//!< ��ʯǨ�Ʊ��
	char suit_effect_change_flag;
	short new_suit_effect_id;
	ItemID new_stone[NEW_EQUIP_STONE_MOUNT_MAX];		//!< ��Ƕ�ı�ʯItemID
	short reserve_sh_list[6];							//!< Ԥ��һЩ,��Ĺ�������ʹ�����β����ʼ

	int star_level;
};

UNSTD_STATIC_CHECK(sizeof(JewelryParam::old_stone) == sizeof(EquipmentParam::old_stone));//װ�������εı�ʯ�ױ���һ��
UNSTD_STATIC_CHECK(sizeof(JewelryParam::new_stone) == sizeof(EquipmentParam::new_stone));//װ�������εı�ʯ�ױ���һ��

struct ExpCrystalParam
{
	short remain_times;			//!< ʣ�����
	short is_turn_on;			//!< �Ƿ���
};

struct TreasureMapItemParam
{
	short is_set_rand;
	char is_active_map;			//�����ǳ���ͼ	0:δ���� 1:�Ѽ���
	char reserve_ch;
	int scene_id;
	int pos_x;
	int pos_y;
	int monster_id;
	long long client_need_uid;			//�ͻ���Ҫ�����ӵ�id�������û��
	int is_rand_map;				// �Ƿ��Ѿ���ͼ(Ϊ�˾�ͼҲ����ʹ��)
	int param_type;					// �������� TREASIRE_MAP_PARAM_TYPE
	int param1;
	int param2;
	int param3;
};

struct PartnerNatalCardParam		//!< �������
{
	int has_net_value;
	int attr_type;
	int attr_value;
};

UNSTD_STATIC_CHECK(sizeof(PartnerNatalCardVoc) == 8);//�����PartnerNatalCardParam �����sizeof��attr_type + attr_value�� ֵ��Сһ�£�����ṹ�޸���Ҫ�������߾�ֵ����дһ��

struct NewPartnerNatalCardParam		//!< �������
{
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
	int has_net_value;
	PartnerNatalCardVoc m_voc_list[MAX_PARTNER_NATAL_CARD_VOC_NUM];
};


struct MedalItemParam		//!< ѫ����Ʒ��ֵ
{
	bool CheckModifyVersion();
	void Reset()
	{
		has_net_value = 0;
		attr_num = 0;
		sp_id = 0;
		memset(attrs, 0, sizeof(attrs));
		medal_move_ver = 0;
		grade = 0;
		special_medal_gread = 0;
		second_sp_id = 0;
	}

	short has_net_value;
	short attr_num;									//!< û����
	int sp_id;							//��Чid
	MedalItemAttrInfo attrs[MAX_MEDAL_MAX_ATTR_NUM];		//!< û����

	short medal_move_ver;
	short grade;

	short special_medal_gread;		// �����������
	short second_sp_id;				// ����Ч
};

const static int MAX_ACCESSORIES_VOC_NUM = 7;//���θ�������
struct AccessoriesParam		//!< ����
{
	const char* GetAttrVocInfoLog(AttrInfo* attr_list, int attr_num)
	{
		static const int BUFFER_LEN = 512;
		static char buffer[BUFFER_LEN];

		memset(buffer, 0, sizeof(buffer));
		int cur_len = 0;
		for (int i = 0; i < attr_num; ++i)
		{
			int len = SNPRINTF(buffer + cur_len, BUFFER_LEN - cur_len, "[type:%d,value:%d],", (int)voc_list[i].attr_type, (int)voc_list[i].attr_value);
			if (len <= 0)
			{
				break;
			}
			cur_len += len;
		}

		return buffer;
	}
	int has_net_value;

	AttrInfo voc_list[MAX_ACCESSORIES_VOC_NUM];

	int strengthen_level;
};

struct DianHuaShiParam
{
	const static int MAX_DIAN_HUA_SHI_LEVEL = 40;// �㻯ʯ�����Ч�ȼ�
	int has_net_value;
	int effect_id;
	int effect_level;
};

//����������
struct PeakTokenParam
{
	int has_net_value;
	int used_times;		//��ʹ�ô���
};

const static int YUAN_HUN_ZHU_MAX_ATTR_NUM = 3;  //Ԫ����3������

struct YuanHunZhuParam
{
	std::string ToString() const;

	void Reset()
	{
		has_net_value = 0;
		yuan_hun_zhu_item_id = 0;
		ArrayFillZero(attr_seq);
		ArrayFillZero(attrs);
	}

	int has_net_value;
	ItemID yuan_hun_zhu_item_id;
	unsigned short attr_seq[YUAN_HUN_ZHU_MAX_ATTR_NUM];		//������������ seq����
	AttrInfo attrs[YUAN_HUN_ZHU_MAX_ATTR_NUM];
};

const static int ZAO_HUA_EQUIP_YUAN_HUN_ZHU_MAX_NUM = 4;

struct ZaoHuaEquipParam
{
	int has_net_value;	

	int ling_li_value;
	int hun_li_value;
	YuanHunZhuParam yuan_hun_zhu[ZAO_HUA_EQUIP_YUAN_HUN_ZHU_MAX_NUM];
};

// ��ʯ
static const int MAX_RUNE_STONE_VOC_NUM = 3;
struct RuneStoneParam
{
	void Reset()
	{
		has_net_value = 0;
		item_id = 0; 
		attr_num = 0;
		memset(voc_list, 0, sizeof(voc_list));
		role_attr.attr_type = 0;
		role_attr.attr_value = 0;
	}

	const char* GetAttrInfoLog(AttrInfo* attr_list, int attr_num)
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

	std::string ToString() const;

	int has_net_value;		//!< �Ƿ��������ֵ

	ItemID item_id;			//!< ��ʯid
	short attr_num;			//!< ��������
	AttrInfo voc_list[MAX_RUNE_STONE_VOC_NUM];	// ��ʯ�����Ŀ����Ӹ�ʥ���Ļ�������
	AttrInfo role_attr;		//!< ��ʯ�����Ŀ����Ӹ���ɫ������
};

// ʥ��
static const int MAX_HOLY_EQUIP_BASE_VOC_NUM = 7;	// ʥװ��������
static const int MAX_HOLY_EQUIP_EXTRA_VOC_NUM = 3;	// �����������
static const int MAX_RUNE_STONE_NUM = 5;			// �����Ƕ��ʯ����
static const int MAX_ACTIVE_SKILLS_NUM = 5;			// ���������������
static const int MAX_TALENT_SKILLS_NUM = 2;			// �츳�����������

struct BattleHallowInfo;
struct HolyEquipParam
{
	enum HOLY_EQUIP_STATUS
	{
		HOLY_EQUIP_STATUS_REST = 0,			//!< ��Ϣ
		HOLY_EQUIP_STATUS_STANDBY = 1,		//!< ����
		HOLY_EQUIP_STATUS_FIGHTING = 2,		//!< ��ս
	};

	enum SKILL_HOLE_STATUS		// �����������ʯ�׹���
	{
		SKILL_HOLE_STATUS_LOCK = -1,	 // δ����
		SKILL_HOLE_STATUS_UNLOCK = 0,	 // �ѽ���
		SKILL_HOLE_STATUS_HAVE_VALUE = 1,// ��ʯ��λ�з�ʯ
	};

	void Reset()
	{
		has_net_value = 0;
		level = 0;
		exp = 0;
		score = 0;
		active_max = 0;
		holy_status = 0;
		valid_extra_attr_num = 0;
		memset(m_extra_attr_list, 0, sizeof(m_extra_attr_list));
		memset(reserve_int, 0, sizeof(reserve_int));
		pet_unique_id = 0LL;
		pet_id = 0;
		memset(total_attr_list, 0, sizeof(total_attr_list));
		memset(base_attr_list, 0, sizeof(base_attr_list));
		memset(unsaved_skill_id, 0, sizeof(unsaved_skill_id));
		memset(active_skills, SKILL_HOLE_STATUS_LOCK, sizeof(active_skills));
		memset(talent_skills, 0, sizeof(talent_skills));
		memset(rune_stone_hole, SKILL_HOLE_STATUS_LOCK, sizeof(rune_stone_hole));
		reserve_ch = 0;
		reserve_sh = 0;

		for (int i = 0; i < ARRAY_LENGTH(rune_stone_list); i++)
		{
			rune_stone_list[i].Reset();
		}
	}

	const char* GetBaseAttrLog(AttrInfo* attr_list, int attr_num)
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

	char* GetAttrInfoLog(AttrInfo* attr_list, int attr_count)
	{
		static const int BUFFER_LEN = 512;
		static char buffer[BUFFER_LEN];

		memset(buffer, 0, sizeof(buffer));
		int cur_len = 0;
		for (int i = 0; i < attr_count; ++i)
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

	void GetBattleHallowAttr(BattleHallowInfo& info) const;
	std::string AttrToString(const AttrInfo* attr_list, int attr_count) const;
	std::string ToString() const;

	int has_net_value;		//!< �Ƿ��������ֵ

	unsigned int level;			//!< ʥ���ȼ�
	unsigned int exp;			//!< ����, ��Ϊ����ʱ100%����
	unsigned int score;			//!< ʥ������
	unsigned int active_max;	//!< ʥ����ǰ�ȼ���Ծ������

	short holy_status;			//!< ʥ��״̬

	short valid_extra_attr_num;				//!< m_extra_attr_list��Ч����
	AttrInfo m_extra_attr_list[MAX_HOLY_EQUIP_EXTRA_VOC_NUM];	//!< ��ʥ���������µļӵ����������
	int reserve_int[2];
	long long pet_unique_id;	//!< �󶨵ĳ���Ψһid
	unsigned int pet_id;		//!< ʥ�ӳ���id

	AttrInfo total_attr_list[MAX_HOLY_EQUIP_BASE_VOC_NUM];		//!< ʥ�����Ի���

	AttrInfo base_attr_list[MAX_HOLY_EQUIP_BASE_VOC_NUM];		//!< ʥ����������

	unsigned short unsaved_skill_id[2];			//!< ѧϰ�˵���δ�����Ƿ��滻�ļ�����Ϣ, [0]-����id, [1]-������������(����м���id����źϷ�)
	int active_skills[MAX_ACTIVE_SKILLS_NUM];	//!< �������� ��ʼ��Ϊ-1,����֮����0(������Զ������0) \see SKILL_HOLE_STATUS
	int talent_skills[MAX_TALENT_SKILLS_NUM];				//!< �츳����

	char rune_stone_hole[MAX_RUNE_STONE_NUM];	//!< ��ʯ��λ����״̬,\see SKILL_HOLE_STATUS
	char reserve_ch;
	short reserve_sh;
	RuneStoneParam rune_stone_list[MAX_RUNE_STONE_NUM];		//!< ��ʯ
};
UNSTD_STATIC_CHECK(sizeof(RuneStoneParam) == 24)
UNSTD_STATIC_CHECK(sizeof(HolyEquipParam) == 272)

// ����
static const int MAX_FU_YU_ATTR_NUM = 3;
struct FuYuParam
{
	void Reset()
	{
		has_net_value = 0;
		expired_timestamp = 0;
		sp_id = 0;
		attr_num = 0;
		extra_sp_id = 0;
		memset(base_attr_list, 0, sizeof(base_attr_list));
	}

	std::string ToString() const;

	int has_net_value;		//!< �Ƿ��������ֵ

	uint32_t expired_timestamp;	// ��Ч��������ʱ���

	int sp_id;		// ��Чid

	int attr_num;
	AttrInfo base_attr_list[MAX_FU_YU_ATTR_NUM];	//���񸽼ӻ�������

	int extra_sp_id;	// ������Чid,(����)
};


#pragma pack(pop)

union NetValueItemParam
{
	NetValueItemParam() // union�ɴ��й��캯���������Ա�������Զ���Ĺ��캯��
	{
		this->Reset();
	}

	void GetData(NetValueItemParamData out_data) const
	{
		if (NULL == out_data) return;

		memcpy(out_data, data, sizeof(NetValueItemParamData));
	}

	void SetData(const NetValueItemParamData in_data)
	{
		if (NULL == in_data) return;

		memcpy(data, in_data, sizeof(NetValueItemParamData));
	}

	void Reset()
	{
		memset(data, 0, sizeof(data));
	}

	const char* GetParamLog(int item_type)
	{
		static char defalut_log_str[LOG_BASE_LENGTH];
		memset(defalut_log_str, 0, sizeof(defalut_log_str));

		UNSTD_STATIC_CHECK(20 == NET_VALLUE_ITEM_COUNT);
		switch (item_type)
		{
		case I_TYPE_FU_YU:
			{
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{%s}", fu_yu_param.ToString().c_str());
			}
			break;
		case I_TYPE_YUAN_HUN_ZHU:
			{
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{%s}", yuan_hun_zhu_param.ToString().c_str());
			}
			break;
		case I_TYPE_RUNE_STONE:
			{
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{%s}", rune_stone_param.ToString().c_str());
			}
			break;
		case I_TYPE_HOLY_EQUIP:
			{
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{%s}", holy_equip_param.ToString().c_str());
				
			}
			break;
		case I_TYPE_ZAO_HUA_EQUIP:
			{
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{ZaoHuaEquipParam (has_net_value:%d) , ling_li_value[%d] hun_li_value[%d] yuanhunzhu[%s]}", 
					zao_hua_param.has_net_value, zao_hua_param.ling_li_value, zao_hua_param.hun_li_value, zao_hua_param.yuan_hun_zhu->ToString().c_str());
			}
			break;
		case I_TYPE_PEAK_TOKEN:
			{
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{PeakTokenParam {has_net_value:%d, used_times[%d]}}",
					peak_token_param.has_net_value,
					peak_token_param.used_times);
			}
			break;
		case I_TYPE_PET_BADGE:
			{
				if (ARRAY_LENGTH(pet_badge_item_param.attrs) >= 2)
				{
					SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{PetBadgeItemParam {has_net_value:%d, skill_id[%d] arrt_type_1:[%d]  arrt_value_1:[%d] arrt_type_2:[%d]  arrt_value_2:[%d] }}",
						pet_badge_item_param.has_random_net_value,
						pet_badge_item_param.skill_id,
						pet_badge_item_param.attrs[0].attr_type, pet_badge_item_param.attrs[0].attr_value,
						pet_badge_item_param.attrs[1].attr_type, pet_badge_item_param.attrs[1].attr_value
					);
				}
			}
			break;
		case I_TYPE_DIAN_HUA_SHI:
			{
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{DianHuaShiParam {has_net_value:%d, sp_id[%d] sp_level[%d]}}",
					dian_hua_shi_param.has_net_value,
					dian_hua_shi_param.effect_id, dian_hua_shi_param.effect_level);
			}
			break;
		case I_TYPE_ACCESSORIES:
			{
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{AccessoriesParam {has_net_value:%d, voc_list[%s], strengthen_level:[%d]}}",
					accessoriess_param.has_net_value,
					accessoriess_param.GetAttrVocInfoLog(accessoriess_param.voc_list, MAX_ACCESSORIES_VOC_NUM),
					accessoriess_param.strengthen_level);
			}
			break;
		case I_TYPE_GOD_PRINT:
			{
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{PetGodPrintEquipMentParam:{suit_id:%d, lock:[%d],color[%d], level[%d] exp[%d] m_max_exp[%d]"
					"m_posi:[%d], m_wear_pet_idx:[%d] item_id:[%d] main_attr:[%s] add_attr_list[%s]}}",
					god_print_param.god_print_data.suit_id, (int)god_print_param.god_print_data.lock, (int)god_print_param.god_print_data.color,
					god_print_param.god_print_data.level, god_print_param.god_print_data.exp, god_print_param.god_print_data.m_max_exp,
					god_print_param.god_print_data.m_posi, god_print_param.god_print_data.m_wear_pet_idx,
					god_print_param.god_print_data.item_id, god_print_param.GetAttrInfoLog(&god_print_param.god_print_data.main_attr, 1),
					god_print_param.GetAttrInfoLog(god_print_param.god_print_data.add_attr_list, god_print_param.god_print_data.attr_num));
			}
			break;
		case I_TYPE_TREASURE_MAP:
			{
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{TreasureMapItemParam:{scene_id:%d, pos_x:%d, pos_y:%d}}",
					treasure_map_param.scene_id, treasure_map_param.pos_x, treasure_map_param.pos_y);
			}
			break;

		case I_TYPE_PARTNER_NATAL_CARD:
			{
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{PartnerNatalCardParam:{%s}}",
					partner_natal_card_param.GetAttrInfoLog(partner_natal_card_param.m_voc_list, MAX_PARTNER_NATAL_CARD_VOC_NUM));
			}
			break;

		case I_TYPE_ELEMENT_CRYSTAL:
			{
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{ElementCrystalParam:{used_times:%d}}",
					element_crystal_param.used_times);
			}
			break;

		case I_TYPE_EQUIPMENT:
			{
				UNSTD_STATIC_CHECK(OLD_EQUIP_STONE_MOUNT_MAX == 2);
				UNSTD_STATIC_CHECK(NEW_EQUIP_STONE_MOUNT_MAX == 4);
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{EquipmentParam:{is_treasure:%d, creator:%s, sp_id:%d, "
					"new_stone:[%d,%d,%d,%d], attr:[%s], extra_attr:[%s] suit_effect_id:[%d] is_strong_create:[%d] strengthen_level[%d]}}",
					equip_param.is_treasure, equip_param.creator_name, equip_param.sp_id,
					equip_param.new_stone[0], equip_param.new_stone[1], equip_param.new_stone[2], equip_param.new_stone[3],
					equip_param.GetAttrInfoLog(equip_param.attr_list, equip_param.attr_num, equip_param.ultimate_attr_list),
					equip_param.GetAttrVocInfoLog(equip_param.attr_extra_inc, equip_param.inc_num),
					(int)equip_param.new_suit_effect_id, (int)equip_param.is_strong_create, equip_param.strengthen_level);
			}
			break;

		case I_TYPE_JEWELRY:
			{
				UNSTD_STATIC_CHECK(OLD_EQUIP_STONE_MOUNT_MAX == 2);
				UNSTD_STATIC_CHECK(NEW_EQUIP_STONE_MOUNT_MAX == 4);
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{JewelryParam:{is_treasure:%d, creator:%s,  attr:[%s],"
					"extra_attr:[%s] new_stone:[%d,%d,%d,%d] suit_effect_id:[%d] is_strong_create:[%d] strengthen_level[%d]}}",
					jewelry_param.is_treasure, jewelry_param.creator_name,
					jewelry_param.GetAttrInfoLog(jewelry_param.attr_list, jewelry_param.attr_num, jewelry_param.ultimate_attr_list),
					jewelry_param.GetAttrVocInfoLog(jewelry_param.attr_extra_inc, jewelry_param.inc_num),
					jewelry_param.new_stone[0], jewelry_param.new_stone[1], jewelry_param.new_stone[2], jewelry_param.new_stone[3],
					(int)jewelry_param.new_suit_effect_id, (int)jewelry_param.is_strong_create, jewelry_param.strengthen_level);
			}
			break;

		case I_TYPE_PET:
			{
				UNSTD_STATIC_CHECK(ADD_POINT_TYPE_MAX == 5);
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{PetParam:{pet_id:%d, name:%s, str_level:%d, level:%d, exp:%lld, add_exp:%lld, "
					"maxhp:%d, maxmp:%d, pet_growth_version[%d], passive_list:{%s}, growth_list:[%d,%d,%d,%d,%d], comprehend_num:%d, comprehend_value:%d, "				
					"attr:[%d,%d,%d,%d,%d] "
					"m_soul_equip_1[itemid:%d is_bind:%d is_lock:%d unique_id:%lld exp:%d max_exp:%d level:%d gread:%d refine:%d sp_id:%d attr[%s], "
					"m_soul_equip_2[itemid:%d is_bind:%d is_lock:%d unique_id:%lld exp:%d max_exp:%d level:%d gread:%d refine:%d sp_id:%d attr[%s]"
					"lian_yao_pet_id:%d lian_yao_pet_str_lv:%d feisheng_times:%d feisheng_skill_id:%d feisheng_skill_lv:%d"
					"}}", pet_param.pet_id, pet_param.name, (int)pet_param.str_level, (int)pet_param.level,
					pet_param.exp, pet_param.add_exp, pet_param.max_hp, pet_param.max_mp, (int)pet_param.pet_growth_version,
					pet_param.GetPassiveInfoLog(pet_param.passive_list, PET_PASSIVE_MAX_COUNT),
					pet_param.growth[0], pet_param.growth[1], pet_param.growth[2], pet_param.growth[3], pet_param.growth[4],
					pet_param.comprehend_gift_num, pet_param.comprehend_passive_value,
					pet_param.attr[0], pet_param.attr[1], pet_param.attr[2], pet_param.attr[3], pet_param.attr[4],
					pet_param.m_soul_equip_1.item_id, pet_param.m_soul_equip_1.is_bind, pet_param.m_soul_equip_1.is_lock, pet_param.m_soul_equip_1.item_unique_id,
					pet_param.m_soul_equip_1.exp, pet_param.m_soul_equip_1.m_max_exp, pet_param.m_soul_equip_1.m_level, pet_param.m_soul_equip_1.m_gread,
					pet_param.m_soul_equip_1.m_refine, pet_param.m_soul_equip_1.sp_id, pet_param.m_soul_equip_1.GetAttrInfoLog(pet_param.m_soul_equip_1.add_attr_list, pet_param.m_soul_equip_1.attr_num),					
					pet_param.m_soul_equip_2.item_id, pet_param.m_soul_equip_2.is_bind, pet_param.m_soul_equip_2.is_lock, pet_param.m_soul_equip_2.item_unique_id,
					pet_param.m_soul_equip_2.exp, pet_param.m_soul_equip_2.m_max_exp, pet_param.m_soul_equip_2.m_level, pet_param.m_soul_equip_2.m_gread,
					pet_param.m_soul_equip_2.m_refine, pet_param.m_soul_equip_2.sp_id, pet_param.m_soul_equip_2.GetAttrInfoLog(pet_param.m_soul_equip_2.add_attr_list, pet_param.m_soul_equip_2.attr_num),
					pet_param.lian_yao_pet_id, pet_param.lian_yao_pet_str_lv, 
					pet_param.fei_sheng_times, pet_param.fei_sheng_select_skill_id, pet_param.fei_sheng_select_skill_level 
				);
			}
			break;

		case I_TYPE_EXP_CRYSTAL:
			{
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{ExpCrystalParam:{remain_times:%d, is_turn_on:%d}}",
					exp_crystal_param.remain_times, exp_crystal_param.is_turn_on);
			}
			break;

		case I_TYPE_PET_EQUIPMENT:
			{
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{PetEquipMentParam:{has_random_net_value:%d,reserve_sh:%d reserve_ch:%d item_id:%d, skill_num:%d, skill_1:%d, skill_2:%d re_skill_1:%d, re_skill_2:%d}}",
					(int)pet_equipment_param.has_random_net_value, pet_equipment_param.reserve_sh, (int)pet_equipment_param.reserve_ch, pet_equipment_param.pet_equipment_id,
					pet_equipment_param.equipment_skill_num, pet_equipment_param.equipment_skill_list[0], pet_equipment_param.equipment_skill_list[1],
					pet_equipment_param.equipment_skill_refresh_list[0], pet_equipment_param.equipment_skill_refresh_list[1]);
			}
			break;
		case I_TYPE_MEDAL:
			{
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{MedalItemParam:{sp_id:%d, grade:%d second_sp_id:%d second_grade:%d}}",
					medal_param.sp_id, medal_param.grade, medal_param.second_sp_id, medal_param.special_medal_gread);
			}
			break;

		case I_TYPE_PET_SOUL_EQUIP:
			{
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{PetSoulEquipParam:{sp_id:%d, refine:%d, level:%d, steps:%d, exp[%d], m_max_exp[%d], attr_info[%s]}}", pet_soul_equipment_param.sp_id, pet_soul_equipment_param.refine,
					pet_soul_equipment_param.level, pet_soul_equipment_param.steps, pet_soul_equipment_param.exp, pet_soul_equipment_param.m_max_exp,
					pet_soul_equipment_param.GetAttrInfoLog(pet_soul_equipment_param.add_attr_list, pet_soul_equipment_param.attr_num));
			}
			break;

		default:
			{
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{NotNetValueItem}");
			}
			break;
		}

		return defalut_log_str;
	}

	static const char* GetParamLog(ItemID item_id, const NetValueItemParamData param_data)
	{
		const ItemBase* item = ITEMPOOL->GetItem(item_id);
		if (NULL == item)
		{
			return "NotValidItem";
		}

		if (NULL == param_data)
		{
			return "{NULL}";
		}

		static NetValueItemParam param;
		param.Reset();
		param.SetData(param_data);

		return param.GetParamLog(item->GetItemType());
	}

	static int GetDataLengthByItemType(int item_type)
	{
		UNSTD_STATIC_CHECK(20 == NET_VALLUE_ITEM_COUNT);
		int length = 0;
		switch (item_type)
		{
		case I_TYPE_FU_YU:
			{
				length = sizeof(FuYuParam);
			}
			break;
		case I_TYPE_YUAN_HUN_ZHU:
			{
				length = sizeof(YuanHunZhuParam);
			}
			break;
		case I_TYPE_RUNE_STONE:
			{
				length = sizeof(RuneStoneParam);
			}
			break;
		case I_TYPE_HOLY_EQUIP:
			{
				length = sizeof(HolyEquipParam);
			}
			break;
		case I_TYPE_ZAO_HUA_EQUIP:
			{
				length = sizeof(ZaoHuaEquipParam);
			}
			break;
		case I_TYPE_PEAK_TOKEN:
			{
				length = sizeof(PeakTokenParam);
			}
			break;
		case I_TYPE_PET_BADGE:
			{
				length = sizeof(PetBadgeItemParam);
			}
			break;
		case I_TYPE_DIAN_HUA_SHI:
			{
				length = sizeof(DianHuaShiParam);
			}
			break;
		case I_TYPE_ACCESSORIES:
			{
				length = sizeof(AccessoriesParam);
			}
			break;
		case I_TYPE_GOD_PRINT:
			{
				length = sizeof(PetGodPrintEquipMentParam);
			}
			break;
		case I_TYPE_EQUIPMENT:
			{
				length = sizeof(EquipmentParam);
			}
			break;

		case I_TYPE_PET:
			{
				length = sizeof(PetParam);
			}
			break;

		case I_TYPE_ELEMENT_CRYSTAL:
			{
				length = sizeof(ElementCrystalParam);
			}
			break;

		case I_TYPE_JEWELRY:
			{
				length = sizeof(JewelryParam);
			}
			break;

		case I_TYPE_EXP_CRYSTAL:
			{
				length = sizeof(ExpCrystalParam);
			}
			break;

		case I_TYPE_TREASURE_MAP:
			{
				length = sizeof(TreasureMapItemParam);
			}
			break;

		case I_TYPE_PARTNER_NATAL_CARD:
			{
				length = sizeof(NewPartnerNatalCardParam);
			}
			break;

		case I_TYPE_PET_EQUIPMENT:
			{
				length = sizeof(PetEquipMentParam);
			}
			break;

		case I_TYPE_MEDAL:
			{
				length = sizeof(MedalItemParam);
			}
			break;
		case I_TYPE_PET_SOUL_EQUIP:
			{
				length = sizeof(PetSoulEquipMentParam);
			}
			break;

		default:
			{
				length = 0;
			}
			break;
		}

		return length;
	}

	int GetDataByItemType(int item_type, NetValueItemParamData & out_data) const
	{
		UNSTD_STATIC_CHECK(20 == NET_VALLUE_ITEM_COUNT);
		int length = 0;
		switch (item_type)
		{
		case I_TYPE_FU_YU:
			{
				memcpy(out_data, &fu_yu_param, sizeof(fu_yu_param));
				length = sizeof(fu_yu_param);
			}
			break;
		case I_TYPE_YUAN_HUN_ZHU:
			{
				memcpy(out_data, &yuan_hun_zhu_param, sizeof(yuan_hun_zhu_param));
				length = sizeof(yuan_hun_zhu_param);
			}
			break;
		case I_TYPE_RUNE_STONE:
			{
				memcpy(out_data, &rune_stone_param, sizeof(rune_stone_param));
				length = sizeof(rune_stone_param);
			}
			break;
		case I_TYPE_HOLY_EQUIP:
			{
				memcpy(out_data, &holy_equip_param, sizeof(holy_equip_param));
				length = sizeof(holy_equip_param);
			}
			break;
		case I_TYPE_ZAO_HUA_EQUIP:
			{
				memcpy(out_data, &zao_hua_param, sizeof(zao_hua_param));
				length = sizeof(zao_hua_param);
			}
			break;
		case I_TYPE_PEAK_TOKEN:
			{
				memcpy(out_data, &peak_token_param, sizeof(peak_token_param));
				length = sizeof(peak_token_param);
			}
			break;
		case I_TYPE_PET_BADGE:
			{
				memcpy(out_data, &pet_badge_item_param, sizeof(pet_badge_item_param));
				length = sizeof(pet_badge_item_param);
			}
			break;
		case I_TYPE_DIAN_HUA_SHI:
			{
				memcpy(out_data, &dian_hua_shi_param, sizeof(dian_hua_shi_param));
				length = sizeof(dian_hua_shi_param);
			}
			break;
		case I_TYPE_ACCESSORIES:
			{
				memcpy(out_data, &accessoriess_param, sizeof(accessoriess_param));
				length = sizeof(accessoriess_param);
			}
			break;
		case I_TYPE_GOD_PRINT:
			{
				memcpy(out_data, &god_print_param, sizeof(PetGodPrintEquipMentParam));
				length = sizeof(PetGodPrintEquipMentParam);
			}
			break;
		case I_TYPE_PET_SOUL_EQUIP:
			{
				memcpy(out_data, &pet_soul_equipment_param, sizeof(PetSoulEquipMentParam));
				length = sizeof(PetSoulEquipMentParam);
			}
			break;
		case I_TYPE_EQUIPMENT:
			{
				memcpy(out_data, &equip_param, sizeof(EquipmentParam));
				length = sizeof(EquipmentParam);
			}
			break;
		case I_TYPE_PET:
			{
				memcpy(out_data, &pet_param, sizeof(PetParam));
				length = sizeof(PetParam);
			}
			break;

		case I_TYPE_ELEMENT_CRYSTAL:
			{
				memcpy(out_data, &element_crystal_param, sizeof(ElementCrystalParam));
				length = sizeof(ElementCrystalParam);
			}
			break;

		case I_TYPE_JEWELRY:
			{
				memcpy(out_data, &jewelry_param, sizeof(JewelryParam));
				length = sizeof(JewelryParam);
			}
			break;

		case I_TYPE_EXP_CRYSTAL:
			{
				memcpy(out_data, &exp_crystal_param, sizeof(ExpCrystalParam));
				length = sizeof(ExpCrystalParam);
			}
			break;

		case I_TYPE_TREASURE_MAP:
			{
				memcpy(out_data, &treasure_map_param, sizeof(TreasureMapItemParam));
				length = sizeof(TreasureMapItemParam);
			}
			break;
		case I_TYPE_PARTNER_NATAL_CARD:
			{
				memcpy(out_data, &partner_natal_card_param, sizeof(NewPartnerNatalCardParam));
				length = sizeof(NewPartnerNatalCardParam);
			}
			break;
		case I_TYPE_PET_EQUIPMENT:
			{
				memcpy(out_data, &pet_equipment_param, sizeof(PetEquipMentParam));
				length = sizeof(PetEquipMentParam);
			}
			break;
		case I_TYPE_MEDAL:
			{
				memcpy(out_data, &medal_param, sizeof(medal_param));
				length = sizeof(medal_param);
			}
			break;
		default:
			{
				length = 0;
			}
			break;
		}

		return length;
	}

	void* operator new(size_t c);
	void operator delete(void* m);

	NetValueItemParamData data;
	EquipmentParam equip_param;
	PetParam pet_param;
	ElementCrystalParam element_crystal_param;
	JewelryParam jewelry_param;
	ExpCrystalParam exp_crystal_param;
	TreasureMapItemParam treasure_map_param;
	NewPartnerNatalCardParam partner_natal_card_param;
	PetEquipMentParam pet_equipment_param;
	MedalItemParam medal_param;
	PetSoulEquipMentParam pet_soul_equipment_param;
	PetGodPrintEquipMentParam god_print_param;
	AccessoriesParam accessoriess_param;
	DianHuaShiParam dian_hua_shi_param;
	PetBadgeItemParam pet_badge_item_param;
	PeakTokenParam peak_token_param;
	ZaoHuaEquipParam zao_hua_param;
	HolyEquipParam holy_equip_param;
	RuneStoneParam rune_stone_param;
	YuanHunZhuParam yuan_hun_zhu_param;
	FuYuParam fu_yu_param;
};

#pragma pack(push, 4)
struct PetPro
{
	PetPro() { this->Reset(); }

	void Reset()
	{
		pet_unique_id = 0;
		index = -1;
		pet_state = -1;
		is_bind = 0;
		capability = 0;
		pet_param.Reset();
	}

	unsigned long long pet_unique_id;
	short index;				// ���
	char pet_state;				// ����״̬  -1��״̬  0 - ����  1 - ��ս  (2 - 4) ��������(1-3)
	char is_bind;				// �Ƿ��
	int capability;				// ����

	PetParam pet_param;
};

struct EquipmentPro
{
	short index;						//!<  ���
	ItemID item_id;						//!<  ��ƷID 
	short num;							//!<  ����
	short is_bind;						//!<  �Ƿ��
	long long item_unique_id;			//!<  ����ΨһID

	EquipmentParam param;				//!<  װ����ֵ����
};

struct AccessoriesPro
{
	short index;						//!<  ���
	ItemID item_id;						//!<  ��ƷID 
	short num;							//!<  ����
	short is_bind;						//!<  �Ƿ��
	long long item_unique_id;			//!<  ����ΨһID

	AccessoriesParam param;
};

struct ZaoHuaPro
{
	short index;						//!<  ���
	ItemID item_id;						//!<  ��ƷID 
	short num;							//!<  ����
	short is_bind;						//!<  �Ƿ��
	long long item_unique_id;			//!<  ����ΨһID

	ZaoHuaEquipParam param;
};

struct HolyEquipPro
{
	HolyEquipPro() { this->Reset(); }

	void Reset()
	{
		item_id = 0;
		reserve_sh = 0;
		param.Reset();
	}

	ItemID item_id;						//!<  ��ƷID 
	short reserve_sh;

	HolyEquipParam param;
};

struct JewelryPro		//!< ���� ��Ϣ
{
	JewelryPro() { this->Reset(); }

	void Reset()
	{
		jewelry_type = 0;
		jewelry_item = 0;
		item_unique_id = 0;

		j_param.has_random_net_value = 0;
		j_param.is_treasure = 0;
		j_param.attr_num = 0;
		is_bind = 0;

		memset(j_param.attr_list, 0, sizeof(j_param.attr_list));
		memset(j_param.creator_name, 0, sizeof(GameName));
		j_param.inc_num = 0;
		memset(j_param.attr_extra_inc, 0, sizeof(j_param.attr_extra_inc));
	}

	short jewelry_type;			//!< ���� ����
	ItemID jewelry_item;		//!< ����ID
	long long item_unique_id;	//!<  ����ΨһID

	JewelryParam j_param;	//!< ��ֵ����
	int is_bind;				// �Ƿ��
};

struct MedalPro
{
	MedalPro() { this->Reset(); }

	void Reset()
	{
		item_id = 0;
		index = -1;
		num = 0;
		is_bind = 0;
		medal_unique_id = 0;
		param.Reset();
	}
	ItemID item_id;
	short index;
	short num;
	short is_bind;
	long long medal_unique_id;

	MedalItemParam param;
};


struct GodPrintListPro
{
	GodPrintListPro() { this->Reset(); }
	void Reset()
	{
		for (int i = 0; i < MAX_PET_GOD_PRONT_ONE_SCHEME_NUM; i++)
		{
			god_print_list[i].Reset();
		}
	}
	PetGodPrintEquipMentParam god_print_list[MAX_PET_GOD_PRONT_ONE_SCHEME_NUM];	
};

struct InfoType
{
	ItemID			item_id;			//!< ��Ʒid
	short			column;				//!< ��Ʒ���ĸ��������� [0,9)
	short			index;				//!< ��Ʒ�ڱ����е����
	char			is_market_buy_flag;	//!< �Ƿ���г������ʶ 0:�� 1:���г�����
	char			reserve_ch;
	int				num;				//!< ��Ʒ����
	char			is_bind;			//!< �Ƿ��  1Ϊ�� 0Ϊ�ǰ�
	char			has_param;			//!< �Ƿ��о�ֵ���� ��Ϊ1����Ϊ0   \note ��ֵ������װ���������Լ�����ֵ��ÿ����Ʒ����Ҫ���ɲ�һ����
	short			param_length;		//!< ��ֵ���������ݳ��� ����has_paramΪ1ʱ�ɶ���
	UInt32		    invalid_time;		//!< ����ʱ��
	long long       item_unique_id;		//!< ����ΨһID
	NetValueItemParamData param_data;	//!< ��ֵ���������ݣ�����param_length��ֵ����ȡ ����has_paramΪ1ʱ�ɶ���
};

struct InfoType2
{
	ItemID			item_id;			//!< ��Ʒid
	short			index;				//!< ��Ʒ�ڱ����е����
	char			is_market_buy_flag;	//!< �Ƿ���г������ʶ 0:�� 1:���г�����
	char			reserve_ch;
	short			reserve_sh;
	int				num;				//!< ��Ʒ����
	char			is_bind;			//!< �Ƿ��  1Ϊ�� 0Ϊ�ǰ�
	char			has_param;			//!< �Ƿ��о�ֵ���� ��Ϊ1����Ϊ0   \note ��ֵ������װ���������Լ�����ֵ��ÿ����Ʒ����Ҫ���ɲ�һ����
	short			param_length;		//!< ��ֵ���������ݳ��� ����has_paramΪ1ʱ�ɶ���
	UInt32		    invalid_time;		//!< ����ʱ��
	long long       item_unique_id;		//!< ����ΨһID
	NetValueItemParamData param_data;	//!< ��ֵ���������ݣ�����param_length��ֵ����ȡ ����has_paramΪ1ʱ�ɶ���
};

#pragma pack(pop)
// PS: ע�⣬NetValueItemParam�е�data�ĳ��ȱ��������ģ����򽫻���ֲ���֪����������
UNSTD_STATIC_CHECK(sizeof(NetValueItemParamData) % 4 == 0);

UNSTD_STATIC_CHECK(20 == NET_VALLUE_ITEM_COUNT);
UNSTD_STATIC_CHECK(sizeof(RuneStoneParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(HolyEquipParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(ZaoHuaEquipParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(EquipmentParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(PetParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(ElementCrystalParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(JewelryParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(ExpCrystalParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(TreasureMapItemParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(PartnerNatalCardParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(PetEquipMentParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(MedalItemParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(NewPartnerNatalCardParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(PetSoulEquipMentParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(ZhuLinParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(PetGodPrintEquipMentParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(PetBadgeItemParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(PeakTokenParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(YuanHunZhuParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(FuYuParam) <= sizeof(NetValueItemParamData));

typedef char GodPrintListProHex[sizeof(GodPrintListPro) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(GodPrintListProHex) < 1024);

#endif