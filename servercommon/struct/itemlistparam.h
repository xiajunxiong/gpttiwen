
#ifndef __ITEMLIST_PARAM_H__
#define __ITEMLIST_PARAM_H__

#include "structcommon.h"
#include "servercommon/basedef.h"
#include "servercommon/serverdef.h"
#include "common/tlvprotocol.h"
#include "servercommon/servercommon.h"
#include "servercommon/configcommon.h"

class ItemParam;

namespace ItemNamespace
{
	enum ITEM_COLUMN_TYPE
	{
		ITEM_COLUMN_TYPE_MIN = 0,
		ITEM_COLUMN_TYPE_ITEM = 0,							//!< ��Ʒ�� 0
		ITEM_COLUMN_TYPE_SUPPLY = 1,						//!< ������ 1
		ITEM_COLUMN_TYPE_FRAGMENT = 2,						//!< ��Ƭ�� 2
		ITEM_COLUMN_TYPE_TASK_MATERIAL = 3,					//!< ����������� 3
		ITEM_COLUMN_TYPE_ILLUSTRATION = 4,					//!< ͼ���� 4
		ITEM_COLUMN_TYPE_PET_SOUL_EQIOP = 5,				//!< ������1 5 -- �� ��������
		ITEM_COLUMN_TYPE_ACCESSORIES = 6,					//!< ������2 6 -- > ���α���
		ITEM_COLUMN_TYPE_NORMAL_COLUMN_END = 7,
		ITEM_COLUMN_TYPE_STORAGE = 7,						//!< �ֿ� 7
		ITEM_COLUMN_TYPE_TEMPORARY = 8,						//!< ��ʱ���� 8
		ITEM_COLUMN_TYPE_MAX = 9,
	};

	enum ITEM_SUB_TYPE								//!< ��Ҫ����������Ʒ
	{
		ITEM_SUB_TYPE_TASK_ITEM = 0,				//!< 0 �������
		ITEM_SUB_TYPE_WEAPON = 1,					//!< 1 ����
		ITEM_SUB_TYPE_ARMOR = 2,					//!< 2 ����
		ITEM_SUB_TYPE_NORMAL_ITEM = 3,				//!< 3 ��ͨ��Ʒ
		ITEM_SUB_TYPE_STONE = 4,					//!< 4 ��ʯ
		ITEM_SUB_TYPE_WOOD = 5,						//!< 5 ľ��
		ITEM_SUB_TYPE_HERB = 6,						//!< 6 ����
		ITEM_SUB_TYPE_FABRIC = 7,					//!< 7 ����
		ITEM_SUB_TYPE_MEDICINE = 8,					//!< 8 ҩƷ
		ITEM_SUB_TYPE_GEM = 9,						//!< 9 ��ʯ
		ITEM_SUB_TYPE_KEYS = 10,					//!< 10 Կ��
		ITEM_SUB_TYPE_MAGIC_STONE = 11,				//!< 11 ħʯ
		ITEM_SUB_TYPE_ELEMENT_CRYSTAL = 12,			//!< 12 Ԫ��ˮ��
		ITEM_SUB_TYPE_SKILL_GRASS = 13,				//!< 13 ���ܲ�
		ITEM_SUB_TYPE_FB_CRYSTAL = 14,				//!< 14 ����ˮ��
		ITEM_SUB_TYPE_ELEMENT_FRAGMENT = 15,		//!< 15 Ԫ��ˮ����Ƭ
		ITEM_SUB_TYPE_GIFT = 16,					//!< 16 ���
		ITEM_SUB_TYPE_ATTR_PILL = 17,				//!< 17 ����ҩ
		ITEM_SUB_TYPE_COLLECTION = 18,				//!< 18 ͼ����
		ITEM_SUB_TYPE_PET_SKILL = 19,				//!< 19 ���＼����
		ITEM_SUB_TYPE_JEWELRY = 20,					//!< 20 ����
		ITEM_SUB_TYPE_SHIZHUANG = 21,				//!< 21 �û�ʱװ
		ITEM_SUB_TYPE_EXP_CRYSTAL = 22,				//!< 22 ��ˮ��
		ITEM_SUB_TYPE_MEDAL = 23,					//!< 23 ѫ��
		ITEM_SUB_TYPE_PARTNER_EQUIPMENT = 24,		//!< 24 ���װ��
		ITEM_SUB_TYPE_PET_EQUIPMENT = 25,			//!< 25 ����װ��
		ITEM_SUB_TYPE_ADD_MEDAL_SP = 26,			//!< 26 ����ʯ
		ITEM_SUB_TYPE_APPEARANCE = 27,				//!< 27 ��ֵ��Ʒ��ͷ�Ρ����ݿ�β�꣩
		ITEM_SUB_TYPE_SKIN = 28,					//!< 28 Ƥ�w�����ⷰ飩
		ITEM_SUB_TYPE_SHANHAI_KEY = 29,				//!< 29 ɽ������KEY
		ITEM_SUB_TYPE_PARTNER_FRAGMENT = 30,		//!< 30 �����Ƭ
		ITEM_SUB_TYPE_PET_FRAGMENT = 31,			//!< 31 ������Ƭ
		ITEM_SUB_TYPE_NO_CHECK_EQUIP = 32,			//!< 32 δ����װ��
		ITEM_SUB_TYPE_PARTNER_NOREXITEM = 33,		//!< 33 ��鼤�
		ITEM_SUB_TYPE_FABAO = 34,					//!< 34 ��������
		ITEM_SUB_TYPE_PET_SOUL = 35,				//!< 35 ����
		ITEM_SUB_TYPE_ZHU_LING = 36,				//!< 36 ����
		ITEM_SUB_TYPE_GOD_PRINT = 37,				//!< 37 ��ӡ
		ITEM_SUB_TYPE_ATTR_PELLET = 38,				//!< 38 ���Ե�
		ITEM_SUB_TYPE_SPIRIT_PACT = 39,				//!< 39 Ԫ����Լ
		ITEM_SUB_TYPE_PEISHI = 40,					//!< 40 ����
		ITEM_SUB_TYPE_DIANHUASHI = 41,				//!< 41 �㻯ʯ
		ITEM_SUB_TYPE_FISH = 42,					//!< 42 ��
		ITEM_SUB_TYPE_PEAK_TOKEN = 43,				//!< 43 ����������
		ITEM_SUB_TYPE_PET_BADGE = 44,				//!< 44 �������
		ITEM_SUB_TYPE_ZAO_HUA_EQUIP = 45,			//!< 45 �컯װ��
		ITEM_SUB_TYPE_YUAN_HUN_ZHU = 46,			//!< 46 Ԫ����
		ITEM_SUB_TYPE_HOLY_EQUIP = 47,				//!< 47 ʥ��
		ITEM_SUB_TYPE_RUNE_STONE = 48,				//!< 48 ʥ����ʯ
		ITEM_SUB_TYPE_HALLOWS_SKILL = 49,			//!< 49 ʥ��������
		ITEM_SUB_TYPE_FU_YU = 50,					//!< 50 ����
		ITEM_SUB_TYPE_SMART_MOUNTS_SKILL = 51,		//!< 51 ���＼����
		ITEM_SUB_TYPE_HUAN_SHOU_SKILL = 52,			//!< 52 ���޼�����

		ITEM_SUB_TYPE_MAX,
	};

	static const int ITEM_COLUMN_GRID_NUM_MAX = 300;		// ÿ�������������ɷŸ�����
	static const int ITEM_COLUMN_GRID_UPBOUND = 1000;		// ÿ�������������޸�����
	UNSTD_STATIC_CHECK(ITEM_COLUMN_GRID_NUM_MAX <= ITEM_COLUMN_GRID_UPBOUND);

	static const int KNAPSACK_MAX_GRID_NUM = ITEM_COLUMN_GRID_NUM_MAX * ITEM_COLUMN_TYPE_MAX;
	static const int KNAPSACK_MAX_GRID_NUM_RESERVED = ITEM_COLUMN_GRID_UPBOUND * ITEM_COLUMN_TYPE_MAX;

	static const int ITEM_COLUMN_GRID_NUM_UPBOUND_LIST[ITEM_COLUMN_TYPE_MAX] = {
		300, 100, 100, 300, 300, 0, 0, 300, 60
	};

	static const int ITEM_COLUMN_GRID_NUM_LOWBOUND_LIST[ITEM_COLUMN_TYPE_MAX] = {
		60, 100, 100, 60, 60, 0, 0, 60, 60
	};

	static const int ITEM_INDEX_BEGIN = 0;
	static const int ITEM_INDEX_END = 10000;

	static const int MAX_EQUIPMENT_GRID_NUM = 5;											// ���װ����������
	static const int EQUIPMENT_INDEX_BEGIN = 10000;											// װ����ʼ����
	static const int EQUIPMENT_INDEX_END = EQUIPMENT_INDEX_BEGIN + MAX_EQUIPMENT_GRID_NUM;	// װ��������

	static const int MAX_JEWELRY_GRID_NUM = 3;												// ������θ�������
	static const int JEWELRY_INDEX_BEGIN = 11000;											// ������ʼ����
	static const int JEWELRY_INDEX_END = JEWELRY_INDEX_BEGIN + MAX_JEWELRY_GRID_NUM;		// ����������

	static const int PET_INDEX_BEGIN = 12000;												// ���ﱳ����ʼ����index
	static const int MAX_PET_GRID_NUM = 100;												// �����ﱳ����������	
	static const int PET_INDEX_END = PET_INDEX_BEGIN + MAX_PET_GRID_NUM;					// ���ﱳ��������index

	static const int PET_STORAGE_INDEX_BEGIN = 13000;												// ����ֿ���ʼ����index
	static const int MAX_PET_STORAGE_GRID_NUM = 100;												// ������ֿ��������
	static const int PET_STORAG_INDEX_END = PET_STORAGE_INDEX_BEGIN + MAX_PET_STORAGE_GRID_NUM;		// ����������index

	static const int MAX_HUASHEN_EQUIPEMNET_GRID_NUM = 5;																// ����װ�������������
	static const int HUASHEN_EQUIPEMNET_INDEX_BEGIN = 14000;															// ����װ����ʼ����
	static const int HUASHEN_EQUIPEMNET_INDEX_END = HUASHEN_EQUIPEMNET_INDEX_BEGIN + MAX_HUASHEN_EQUIPEMNET_GRID_NUM;	// ����װ��������

	static const int MAX_HUASHEN_JEWELRY_GRID_NUM = 3;															// �������θ����������
	static const int HUASHEN_JEWELRY_INDEX_BEGIN = 15000;														// ����������ʼ����
	static const int HUASHEN_JEWELRY_INDEX_END = HUASHEN_JEWELRY_INDEX_BEGIN + MAX_HUASHEN_JEWELRY_GRID_NUM;	// ��������������

	static const int HUASHEN_ITEM_GRID_NUM_MAX = ITEM_COLUMN_GRID_NUM_MAX + 100;					//���������ɷŸ���
	static const int HUASHEN_ITEM_INDEX_BEGIN = 16000;												//��������ʼ����
	static const int HUASHEN_ITEM_INDEX_END = HUASHEN_ITEM_INDEX_BEGIN + HUASHEN_ITEM_GRID_NUM_MAX;	//������������

	static const int MAX_HUASHEN_PET_GRID_NUM = 120;												// ������ﱳ�������������	
	static const int PET_HUASHEN_INDEX_BEGIN = 17000;												// ������ﱳ����ʼ����index
	static const int PET_HUASHEN_INDEX_END = PET_HUASHEN_INDEX_BEGIN + MAX_HUASHEN_PET_GRID_NUM;	// ������ﱳ��������index

	static const int MAX_MEADL_INDEX_NUM = 10;												// ѫ��װ�������������	
	static const int MEDAL_INDEX_BEGIN = 18000;												// ѫ��װ����ʼ����index
	static const int MEDAL_INDEX_END = MEDAL_INDEX_BEGIN + MAX_MEADL_INDEX_NUM;				// ѫ��װ��������index

	static const int MAX_HUASHEN_MEADL_INDEX_NUM = 10;														// ����ѫ��װ�������������	
	static const int HUASHEN_MEDAL_INDEX_BEGIN = 19000;														// ����ѫ��װ����ʼ����index
	static const int HUASHEN_MEDAL_INDEX_END = HUASHEN_MEDAL_INDEX_BEGIN + MAX_HUASHEN_MEADL_INDEX_NUM;		// ����ѫ��װ��������index

	static const int MAX_ACCESSORIES_INDEX_NUM = 5;															// ����װ�������������	
	static const int ACCESSORIES_INDEX_BEGIN = 20000;														// ����װ����ʼ����index
	static const int ACCESSORIES_INDEX_END = ACCESSORIES_INDEX_BEGIN + MAX_ACCESSORIES_INDEX_NUM;		// ����װ��������index

	static const int MAX_ZAO_HUA_EQUIPMENT_GRID_NUM = 8;															// ����컯װ����������
	static const int ZAO_HUA_EQUIPMENT_INDEX_BEGIN = 20010;															// �컯װ����ʼ����
	static const int ZAO_HUA_EQUIPMENT_INDEX_END = ZAO_HUA_EQUIPMENT_INDEX_BEGIN + MAX_ZAO_HUA_EQUIPMENT_GRID_NUM;	// �컯װ��������

	static const int MAX_ZAO_HUA_BAG_NUM = 500;														// �컯������������
	static const int ZAO_HUA_BAG_INDEX_BEGIN = 20020;												// �컯������ʼ����
	static const int ZAO_HUA_BAG_INDEX_END = ZAO_HUA_BAG_INDEX_BEGIN + MAX_ZAO_HUA_BAG_NUM;			// �컯����������

	static const int MAX_HOLY_EQUIP_BAG_NUM = 200;														// ʥ��������������
	static const int HOLY_EQUIP_BAG_INDEX_BEGIN = 21000;												// ʥ��������ʼ����
	static const int HOLY_EQUIP_BAG_INDEX_END = HOLY_EQUIP_BAG_INDEX_BEGIN + MAX_HOLY_EQUIP_BAG_NUM;	// ʥ������������

	static const int MAX_RUNE_STONE_BAG_NUM = 500;														// ��ʯ������������
	static const int RUNE_STONE_BAG_INDEX_BEGIN = 22000;												// ��ʯ������ʼ����
	static const int RUNE_STONE_BAG_INDEX_END = RUNE_STONE_BAG_INDEX_BEGIN + MAX_RUNE_STONE_BAG_NUM;	// ��ʯ����������

	static const int MAX_FU_YU_GRID_NUM = 150;													// ����װ����������
	static const int FU_YU_GRID_INDEX_BEGIN = 23000;											// ����װ����ʼ����
	static const int FU_YU_GRID_INDEX_END = FU_YU_GRID_INDEX_BEGIN + MAX_FU_YU_GRID_NUM;		// ����װ��������

	static const int MAX_HUAN_SHOU_GRID_NUM = 200;														// ���ޱ�����������
	static const int HUAN_SHOU_GRID_INDEX_BEGIN = 24000;												// ���ޱ�����ʼ����
	static const int HUAN_SHOU_GRID_INDEX_END = HUAN_SHOU_GRID_INDEX_BEGIN + MAX_HUAN_SHOU_GRID_NUM;	// ���ޱ���������

	static const int MAX_GRID_NUM = KNAPSACK_MAX_GRID_NUM + MAX_EQUIPMENT_GRID_NUM + MAX_JEWELRY_GRID_NUM + MAX_PET_GRID_NUM + MAX_PET_STORAGE_GRID_NUM
		+ MAX_HUASHEN_EQUIPEMNET_GRID_NUM + MAX_HUASHEN_JEWELRY_GRID_NUM + HUASHEN_ITEM_GRID_NUM_MAX + MAX_HUASHEN_PET_GRID_NUM
		+ MAX_MEADL_INDEX_NUM + MAX_HUASHEN_MEADL_INDEX_NUM + MAX_ACCESSORIES_INDEX_NUM + MAX_ZAO_HUA_EQUIPMENT_GRID_NUM + MAX_ZAO_HUA_BAG_NUM
		+ MAX_HOLY_EQUIP_BAG_NUM + MAX_RUNE_STONE_BAG_NUM + MAX_FU_YU_GRID_NUM + MAX_HUAN_SHOU_GRID_NUM; // ������Ʒ�ܸ�����

	static const int PET_GRID_MAX_COUNT = MAX_PET_GRID_NUM + MAX_PET_STORAGE_GRID_NUM;						//�����ܹ�����������(����)
	static const int EQUIPMENT_GRID_MAX_COUNT = MAX_EQUIPMENT_GRID_NUM + MAX_HUASHEN_EQUIPEMNET_GRID_NUM;	//װ���ܹ�����������(����+����)
	static const int JEWELRY_GRID_MAX_COUNT = MAX_JEWELRY_GRID_NUM + MAX_HUASHEN_JEWELRY_GRID_NUM;			//�����ܹ�����������(����+����)
	static const int ACCESSORIES_GRID_MAX_COUNT = MAX_ACCESSORIES_INDEX_NUM;			//�����ܹ�����������(����)

	static const int MAX_BATCH_CONSUME_ITEM_NUM = 8;										// �������ģ����������Ʒ����

	static const int MAX_STORAGE_PAGE_NUM = 5;												// �ֿ�ҳ��
	static const int STORAGE_GRID_NUM_EACH_PAGE = 60;										// �ֿ�ÿҳ������

	UNSTD_STATIC_CHECK(0 + KNAPSACK_MAX_GRID_NUM < EQUIPMENT_INDEX_BEGIN);
	UNSTD_STATIC_CHECK(EQUIPMENT_INDEX_BEGIN + MAX_EQUIPMENT_GRID_NUM < PET_INDEX_BEGIN);
	UNSTD_STATIC_CHECK(PET_INDEX_BEGIN + MAX_PET_GRID_NUM < PET_STORAGE_INDEX_BEGIN);
	UNSTD_STATIC_CHECK(PET_STORAGE_INDEX_BEGIN + MAX_PET_STORAGE_GRID_NUM < HUASHEN_EQUIPEMNET_INDEX_BEGIN);
	UNSTD_STATIC_CHECK(HUASHEN_EQUIPEMNET_INDEX_END < HUASHEN_JEWELRY_INDEX_BEGIN);
	UNSTD_STATIC_CHECK(HUASHEN_JEWELRY_INDEX_END < HUASHEN_ITEM_INDEX_BEGIN);
	UNSTD_STATIC_CHECK(HUASHEN_ITEM_INDEX_END < PET_HUASHEN_INDEX_BEGIN);
	UNSTD_STATIC_CHECK(PET_HUASHEN_INDEX_END < MEDAL_INDEX_BEGIN);
	UNSTD_STATIC_CHECK(MEDAL_INDEX_END < HUASHEN_MEDAL_INDEX_BEGIN);
	UNSTD_STATIC_CHECK(HUASHEN_MEDAL_INDEX_BEGIN < ACCESSORIES_INDEX_BEGIN);
	UNSTD_STATIC_CHECK(ACCESSORIES_INDEX_BEGIN < ZAO_HUA_EQUIPMENT_INDEX_BEGIN);
	UNSTD_STATIC_CHECK(ZAO_HUA_EQUIPMENT_INDEX_BEGIN < ZAO_HUA_BAG_INDEX_BEGIN);
	UNSTD_STATIC_CHECK(ZAO_HUA_BAG_INDEX_END < HOLY_EQUIP_BAG_INDEX_BEGIN);
	UNSTD_STATIC_CHECK(HOLY_EQUIP_BAG_INDEX_END < RUNE_STONE_BAG_INDEX_BEGIN);
	UNSTD_STATIC_CHECK(RUNE_STONE_BAG_INDEX_END < FU_YU_GRID_INDEX_BEGIN);
	UNSTD_STATIC_CHECK(FU_YU_GRID_INDEX_END < HUAN_SHOU_GRID_INDEX_BEGIN);

	enum ITEM_TRADE_MONEY_TYPE
	{
		ITEM_TRADE_MONEY_TYPE_INVALID = 0,
		ITEM_TRADE_MONEY_TYPE_GOLD = 1,				//!< 1 Ԫ��
		ITEM_TRADE_MONEY_TYPE_IMMORTAL_COIN = 2,	//!< 2 �ɱ�
		ITEM_TRADE_MONEY_TYPE_MAX = 3,
	};

	static const int EQUIPMENT_MAX_LEVEL = 13;
	static const int EXCEED_PILE_LIMIT_PUT_NUM = 20;

	static const int VIRTUAL_ITEM_EXP = 65535;

#ifdef _DEBUG
#define TRACK_ITEM_FLAG
#endif
}

struct SuitEffectReCalc//װ����װ����
{
	SuitEffectReCalc() : sp_id(0), num(0) {}

	void Reset()
	{
		sp_id = 0;
		num = 0;
		color_top.clear();
	}

	int sp_id;
	int num;
	std::vector<int> color_top;//��Ӧ����num�����������Ʒ��
};

#pragma pack(push, 4)

const static int MAX_SUIT_TYPE = 5;//�����װ����

static const int REWARD_LIST_ITEM_NUM = 8;

struct ItemConfigData
{
	ItemConfigData() : item_id(0), is_bind(false), num(0) {}
	ItemConfigData(ItemID _item_id, bool _is_bind, int _num) : item_id(_item_id), is_bind(_is_bind), num(_num) {}

	ItemID item_id;
	bool is_bind;
	int num;

	bool ReadConfig(TiXmlElement *element);
	bool ReadConfigNoCheck(TiXmlElement *element);
	bool CheckConfig(std::string& errmsg) const;
};

typedef  char ItemDataWrapperOld[540];

struct ItemDataWrapper
{
	ItemDataWrapper() : item_id(0), is_market_buy_flag(0), reserve_ch(0), num(0), is_bind(0), has_param(0), invalid_time(0), gold_price(0), item_unique_id(0LL)
	{
		memset(param_data, 0, sizeof(param_data));
	}

	ItemDataWrapper(ItemID _item_id, short _num, short _is_bind) : item_id(_item_id), is_market_buy_flag(0), reserve_ch(0), num(_num), is_bind(_is_bind), has_param(0), invalid_time(0), gold_price(0), item_unique_id(0LL)
	{
		memset(param_data, 0, sizeof(param_data));
	}

	ItemDataWrapper(ItemID _item_id, short _num, short _is_bind, short _has_param, NetValueItemParamData _param_data, UInt32 _invalid_time, int _gold_price, long long _item_unique_id, char _is_market_buy_flag)
		: item_id(_item_id), is_market_buy_flag(_is_market_buy_flag), reserve_ch(0), num(_num), is_bind(_is_bind), has_param(_has_param), invalid_time(_invalid_time), gold_price(_gold_price), item_unique_id(_item_unique_id)
	{
		if (NULL != _param_data)
		{
			memcpy(param_data, _param_data, sizeof(NetValueItemParamData));
			has_param = 1;
		}
		else
		{
			memset(param_data, 0, sizeof(NetValueItemParamData));
			has_param = 0;
		}
	}

	static void RevealConfigItemId(int config_item_id, ItemID *item_id, short *is_bind)
	{
		if (NULL == item_id || NULL == is_bind) return;

		*is_bind = short((config_item_id / 100000) != 0);
		*item_id = config_item_id % 100000;
	}

	bool IsEqual(const ItemDataWrapper &item_wrapper, bool ignore_num = false) const
	{
		if (item_id != item_wrapper.item_id || invalid_time != item_wrapper.invalid_time ||
			is_bind != item_wrapper.is_bind || has_param != item_wrapper.has_param ||
			is_market_buy_flag != item_wrapper.is_market_buy_flag) return false;
		if (!ignore_num && item_wrapper.num != num) return false;

		if (!has_param) return true;

		static const int data_size = sizeof(NetValueItemParamData);
		for (int i = 0; i < data_size; ++i)
		{
			if (param_data[i] != item_wrapper.param_data[i])
			{
				return false;
			}
		}

		return true;
	}

	bool CanMerge(const ItemDataWrapper & other) const
	{
		if (this->has_param || other.has_param || this->num <= 0 || other.num <= 0 || this->item_id == 0)
		{
			return false;
		}

		return this->IsEqual(other, true);
	}

	bool Merge(ItemDataWrapper & other)
	{
		if (!this->CanMerge(other))
		{
			return false;
		}

		this->num += other.num;
		other.item_id = 0;
		other.num = 0;

		return true;
	}

	bool Invalid() const { return 0 == item_id || num <= 0; }

	bool Serialize(TLVSerializer &outstream) const
	{
		bool ret = outstream.Push(item_id) && outstream.Push(num) && outstream.Push(is_bind)
			&& outstream.Push(has_param) && outstream.Push(invalid_time) && outstream.Push(gold_price)
			&& outstream.Push(item_unique_id) && outstream.Push(is_market_buy_flag);

		if (!ret) return false;

		if (0 != has_param)
		{
			TLVSerializer itemparam_data;
			itemparam_data.Reset((void*)param_data, sizeof(param_data));
			itemparam_data.MoveCurPos(sizeof(param_data));

			ret = outstream.Push(itemparam_data);

			if (!ret) return false;
		}

		return true;
	}

	bool Unserialize(TLVUnserializer &instream)
	{
		bool ret = instream.Pop(&item_id) && instream.Pop(&num) && instream.Pop(&is_bind)
			&& instream.Pop(&has_param) && instream.Pop(&invalid_time) && instream.Pop(&gold_price)
			&& instream.Pop(&item_unique_id) && instream.Pop(&is_market_buy_flag);

		if (!ret) return false;

		if (0 != has_param)
		{
			static TLVUnserializer itemparam_data;
			if (!instream.Pop(&itemparam_data)) return false;

			if (itemparam_data.Size() <= sizeof(param_data))
			{
				memcpy(param_data, itemparam_data.Ptr(), itemparam_data.Size());
			}
		}
		else
		{
			memset(param_data, 0, sizeof(param_data));
		}

		return true;
	}

	void Reset()
	{
		item_id = 0; is_market_buy_flag = 0; reserve_ch = 0; num = 0; is_bind = 0; has_param = 0; invalid_time = 0; gold_price = 0; item_unique_id = 0;
		memset(param_data, 0, sizeof(param_data));
	}

	ItemID item_id;						//!<  ��ƷID 
	char is_market_buy_flag;			//!<  �Ƿ���г������ʶ 0:�� 1:���г�����
	char reserve_ch;
	int num;							//!<  ����
	short is_bind;						//!<  �Ƿ��
	short has_param;					//!<  �Ƿ��о�ֵ
	UInt32 invalid_time;				//!<  ����ʱ��
	int gold_price;						//!<  �ۼ�
	NetValueItemParamData param_data;	//!<  char[1024]  ��Ʒ��ֵ��������ν����Ҫ������Ʒ�����͵Ĳ�ͬ����ͬ
	long long item_unique_id;			//!<  ��ƷΨһID ��ϵͳ����
};

UNSTD_STATIC_CHECK(sizeof(NetValueItemParamData) == 1024); //!< \note �����ȴ���1024����Ҫ�޸����ݱ��е�itemparam���ֶγ���

static const int MAX_RECORD_ITEM_COUNT = 150;
static const int MAX_RECORD_DUPLICATE_ITEM_COUNT = 10;
struct ItemUseRecordParam
{
	ItemUseRecordParam()
	{
		this->Reset();
	}

	struct Record
	{
		Record() : item_id(0), reserved_sh(0), used_times(0) {}

		ItemID item_id;
		unsigned short reserved_sh;
		int used_times;//����
	};

	struct DuplicateItemRecord
	{
		DuplicateItemRecord() : used_times(0), reward_time_s(0) {}

		int used_times;//����
		unsigned int reward_time_s;//ʱ��
	};

	void Reset()
	{
		record_count = 0;
		memset(record_list, 0, sizeof(record_list));
	}

	// ÿ�����
	int record_count;
	Record record_list[MAX_RECORD_ITEM_COUNT];

	DuplicateItemRecord duplicate_record_list[MAX_RECORD_DUPLICATE_ITEM_COUNT];
};

struct GemUpgradedata
{
	struct ItemPair
	{
		ItemPair(ItemID _item_id = 0, short _item_num = 0) : item_id(_item_id), item_num(_item_num) {}

		ItemID item_id;
		short item_num;
	};

	GemUpgradedata() : target_gem(0), fin_gem_id(0), fin_need_numb(0), money_type(0), consume_coins(0), list_num(0) {};

	bool IsInvalid() const { return 0 == target_gem; }
	void SetInvalid() { target_gem = 0; }

	ItemID target_gem;		//!< Ŀ�걦ʯ��
	ItemID fin_gem_id;		//!< ������Ҫ�ı�ʯ������һ��������Ϊ 0����������£���ID��Ӧ�ĵ���Ӧ��Ϊ 1 ����ʯ��
	int fin_need_numb;		//!< ������Ҫ�ı�ʯ������
	int money_type;			//!< �������ͣ�
	int consume_coins;		//!< ��Ҫ�Ļ��ң�

	int list_num;
	ItemPair own_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];	// �����б�
};

struct GemUpgradeInfo
{
	GemUpgradeInfo() : equip_posi(0), equip_grid(0), stone_slot(0), compose_times(0) {}

	void SetParam(short _stone_type, short _e_posi, short _e_grid, short _g_slot)
	{
		stone_type = _stone_type;
		equip_posi = _e_posi;
		equip_grid = _e_grid;
		stone_slot = _g_slot;
	}

	short stone_type;
	short equip_posi;
	short equip_grid;
	short stone_slot;
	short compose_times;
	short reserve_sh;

	GemUpgradedata ug_data;
};

class KnapsackOtherParam
{
public:
	KnapsackOtherParam() { this->Reset(); }

	void Reset()
	{
		init_flag = 0;
		is_get_novice_gift = 0;
		reserve_sh = 0;
		reserve_sh2 = 0;
		memset(curr_valid_grid_num_list, 0, sizeof(curr_valid_grid_num_list));
		reserve_ll = 0LL;
		reserve_int = 0;
		clear_temp_column_time = 0u;
		memset(extend_times_list, 0, sizeof(extend_times_list));
		reserve_sh3 = 0;
	}

	char init_flag;
	char is_get_novice_gift;
	short reserve_sh;
	short reserve_sh2;
	short curr_valid_grid_num_list[ItemNamespace::ITEM_COLUMN_TYPE_MAX];
	unsigned int clear_temp_column_time;
	int reserve_int;
	long long reserve_ll;
	short extend_times_list[ItemNamespace::ITEM_COLUMN_TYPE_MAX];
	short reserve_sh3;
};
typedef char KnapsackOtherParamHex[sizeof(KnapsackOtherParam) * 2 + 1];
UNSTD_STATIC_CHECK(ItemNamespace::ITEM_COLUMN_TYPE_MAX == 9);
UNSTD_STATIC_CHECK(sizeof(KnapsackOtherParamHex) < 128);

struct EquipVocLock
{
	static const int EXA_ATTR_NUM_MAX = 6;
	EquipVocLock() : reserve_sh(0)
	{
		this->Reset();
	}

	void Reset()
	{
		memset(is_lock, 0, sizeof(is_lock));
		reserve_sh = 0;
	}

	char is_lock[EXA_ATTR_NUM_MAX];
	short reserve_sh;
};

static const int EQUIPMENT_MAX_INDEX = 5;
static const int JEWELRY_MAX_INDEX = 3;

enum EQUIP_JEWELRY_TYPE
{
	EQUIP_JEWELRY_TYPE_HELMET = 0,		// ͷ�� 0
	EQUIP_JEWELRY_TYPE_ARMOUR = 1,		// ���� 1
	EQUIP_JEWELRY_TYPE_WEAPON = 2,		// ���� 2
	EQUIP_JEWELRY_TYPE_SHIELD = 3,		// ������ 3
	EQUIP_JEWELRY_TYPE_SHOE = 4,		// Ь�� 4
	EQUIP_JEWELRY_TYPE_GEM_RING = 5,	// ��ָ 5
	EQUIP_JEWELRY_TYPE_BRACELET = 6,	// ���� 6
	EQUIP_JEWELRY_TYPE_PENDANTS = 7,	// ׹�� 7
	EQUIP_JEWELRY_TYPE_MAX,
};

UNSTD_STATIC_CHECK(EQUIP_JEWELRY_TYPE_MAX == EQUIPMENT_MAX_INDEX + JEWELRY_MAX_INDEX);

class EquipmentOtherParam
{
public:
	EquipmentOtherParam() { this->Reset(); }

	void Reset()
	{
		element_crystal_id = 0;
		element_crystal_used_times = 0;
		element_crystal_unique_id = 0;
		huashen_element_crystal_id = 0;
		huashen_element_crystal_used_times = 0;
		huashen_element_crystal_unique_id = 0;
		huashen_element_crystal_is_bind = 0;
		element_crystal_is_market_buy_flag = 0;
		new_suit_flag = 0;
		memset(equipment_jewelry_upgreade, 0, sizeof(equipment_jewelry_upgreade));

		memset(reserve_ll, 0, sizeof(reserve_ll));
		equip_jewelry_cap = 0;

		new_voc_extra_flag = 0;
		reserve_ch = 0;
		reserve_sh = 0;

		resonace_open_level = 0;
		memset(equipment_jewelry_strengthen, 0, sizeof(equipment_jewelry_strengthen));
	}
	//����
	ItemID element_crystal_id;
	short element_crystal_used_times;

	EquipVocLock m_equipment_lock_list[EQUIPMENT_MAX_INDEX];
	EquipVocLock m_jewelry_lock_list[JEWELRY_MAX_INDEX];

	long long element_crystal_unique_id;

	//����
	ItemID huashen_element_crystal_id;
	short huashen_element_crystal_used_times;
	long long huashen_element_crystal_unique_id;
	char huashen_element_crystal_is_bind;		//��¼�����ʱ������Ԫ��ˮ���İ����
	char element_crystal_is_market_buy_flag;	//�Ƿ���г������ʶ 0:�� 1 : ���г�����
	short new_suit_flag;							//��װ�����ת����װ

	int equipment_jewelry_upgreade[EQUIP_JEWELRY_TYPE_MAX];

	long long reserve_ll[2];

	int equip_jewelry_cap;						//װ��+��������

	char new_voc_extra_flag;
	char reserve_ch;
	short reserve_sh;

	int resonace_open_level;					// �����ȼ�����
	int equipment_jewelry_strengthen[EQUIP_JEWELRY_TYPE_MAX];//ͻ�Ƶȼ�
};

#pragma pack(pop)

typedef char EquipmentOtherParamHex[sizeof(EquipmentOtherParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(EquipmentOtherParamHex) < 512);

typedef char NetValueItemParamDataHex[sizeof(NetValueItemParamData) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(NetValueItemParamDataHex) < 8192);

typedef char ItemUseRecordParamHex[sizeof(ItemUseRecordParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ItemUseRecordParamHex) < 4096);

class ItemListParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	struct ItemListData
	{
		char change_state_item;
		short index;

		ItemDataWrapper item_wrapper;
	};

	int count;
	ItemListData item_list[ItemNamespace::MAX_GRID_NUM];
};

class QueryItemListParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	struct ItemListData
	{
		char change_state_item;
		short index;

		ItemDataWrapper item_wrapper;
	};

	int count;
	ItemListData item_list[200];
};

int ReadItemConfigData(TiXmlElement * data_element, std::string str, ItemConfigData ARG_OUT & item_data, bool need_exist = true);
int ReadItemConfigData2(TiXmlElement * data_element, std::string str, std::vector<ItemConfigData> ARG_OUT & item_data_vec, bool need_exist = true);
int ReadItemConfigDataList(TiXmlElement * data_element, std::string str, std::vector<ItemConfigData> ARG_OUT & item_data_vec, bool need_exist = true);
int ReadCfgListValue(TiXmlElement * data_element, std::string str, std::vector<int> ARG_OUT & vec, bool is_fliter_zero = false);



#endif
