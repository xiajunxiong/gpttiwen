#ifndef __WORKSHOP_DEF_H__
#define __WORKSHOP_DEF_H__

#include "servercommon/struct/battle/battle_def.hpp"
#include "gameworld/gameworld/item/itembase.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/bitmapfunction.hpp"

#pragma pack(push, 4)

static const int SPECIAL_RECIPE_TOTAL_NUM = 80 * BIT_COUNT_OF_LONG_64;
static const int PET_SKILL_BOOK_NEED_ITEM_TYPE_NUM = 10;
const static int INTJEWELRY_SUCC_MAX_LIST = 5 + 1;//����Ʒ���������ɫ 5
const static int INTJEWELRY_SUCC_MAX_GROUP = 30 + 1;//����Ʒ�����������
const static int MAX_GOD_EQUIP_MAX_LEVEL = 86;//��װ�����׵ȼ�

enum WORKSHOP_OTHER_PARAM
{
	WORKSHOP_CRAFT_NONE = 0,	// �Ƿ�ǿ������ - ��
	WORKSHOP_CRAFT_TRUE = 1,	// �Ƿ�ǿ������ - ��

	WORKSHOP_RETURN_NONE = 0,	// �ϳ�ʧ��ʱ�������Ƿ񷵻� - ��
	WORKSHOP_RETURN_TRUE = 1,	// �ϳ�ʧ��ʱ�������Ƿ񷵻� - ��

	WORKSHOP_EQUIP_POSI_EQUIP = 0,	// װ������λ�� - װ����
	WORKSHOP_EQUIP_POSI_INBAG = 1,	// װ������λ�� - �ڱ���

	WORKSHOP_STONE_TYPE_EQUIP = 0,		// װ��
	WORKSHOP_STONE_TYPE_JEWELRY = 1,	// ����
};

enum WORKSHOP_JEWELRY_REFOEGE_LIST
{
	JEWELRY_REFORGE_MATE_1 = 0,
	JEWELRY_REFORGE_MATE_2 = 1,
	JEWELRY_REFORGE_RESULT = 2,

	JEWELTY_REFORGE_LIST_MAX
};

enum WORKSHOP_EXCHANGE_TYPE
{
	WORKSHOP_EXCHANGE_PET_SKILL_BOOK = 0,		// ���＼����
	WORKSHOP_EXCHANGE_PET_SEALS_CARD = 1,		// �����ӡ��
	WORKSHOP_DISCOMPOSE_FABAO = 2,				// ������Ƭ�ֽ�
	WORKSHOP_SYNTHESIS_KEY= 3,					// ��Կ�ס���Կ�׺ϳ�
	WORKSHOP_SYNTHESIS_SUPER_MARROW  = 4,		// ����ϴ��¶�ϳ�

	WORKSHOP_EXCHANGE_TYPE_MAX
};

// 0-4,װ����5-7������
enum ULTIMATE_EQUIP_TYPE_E
{
	ULTIMATE_EQUIP_TYPE_E_HELMET = 0,		// ͷ��
	ULTIMATE_EQUIP_TYPE_E_ARMOUR,			// ����
	ULTIMATE_EQUIP_TYPE_E_WEAPON,			// ����
	ULTIMATE_EQUIP_TYPE_E_SHIELD,			// ����
	ULTIMATE_EQUIP_TYPE_E_SHOE,				// Ь��
	ULTIMATE_EQUIP_TYPE_J_RING,				// ��ָ
	ULTIMATE_EQUIP_TYPE_J_BRACELET,			// ����
	ULTIMATE_EQUIP_TYPE_J_PENDANTS,			// ׹��
	ULTIMATE_EQUIP_TYPE_MAX,
};

struct GridPair
{
	GridPair() : grid_idx(0), item_num(0) {}

	UInt16 grid_idx;
	UInt16 item_num;
};

struct JewelryProReserve		//!< ���� ��Ϣ �ڴ˶�����Ϊ�˷�ֹ֮��ͨ�����νṹ���������±������
{
	JewelryProReserve() { this->Reset(); }

	void Reset()
	{
		jewelry_type = 0;
		jewelry_item = 0;

		memset(reserve_ch, 0, sizeof(reserve_ch));
		/*j_param.has_random_net_value = 0;
		j_param.is_treasure = 0;
		j_param.attr_num = 0;

		memset(j_param.attr_list, 0, sizeof(j_param.attr_list));
		memset(j_param.creator_name, 0, sizeof(GameName));
		j_param.inc_num = 0;
		memset(j_param.attr_extra_inc, 0, sizeof(j_param.attr_extra_inc));*/
	}

	short jewelry_type;			//!< ���� ����
	ItemID jewelry_item;		//!< ����ID

	char reserve_ch[192];
	//JewelryParam j_param;	//!< ��ֵ����
};

struct WorkShopParam
{
	WorkShopParam() { this->Reset(); }

	void Reset()
	{
		for (int i = 0; i < JEWELTY_REFORGE_LIST_MAX; ++i)
		{
			jewelry_reforge_list[i].Reset();
		}
		sp_recipe_active_flag.Reset();
		memset(jewelry_create_group, 0, sizeof(jewelry_create_group));
		memset(reserve_longlong, 0, sizeof(reserve_longlong));
		god_equip_flag = 0;
		sp_recipe_created_flag.Reset();
		version_god_equip = 0;
		god_equip_change_flag = 0;
		check_god_equip_base_voc_flag = 0;
		reserve_sh_2 = 0;
	}

	JewelryProReserve jewelry_reforge_list[JEWELTY_REFORGE_LIST_MAX];		//���ֶ������� 
	BitMap<SPECIAL_RECIPE_TOTAL_NUM> sp_recipe_active_flag;		// �����䷽�����б�
	int jewelry_create_group[INTJEWELRY_SUCC_MAX_GROUP];
	int reserve_sh[JEWELTY_REFORGE_LIST_MAX];
	long long reserve_longlong[JEWELTY_REFORGE_LIST_MAX];
	int god_equip_flag;//��װ������
	BitMap<SPECIAL_RECIPE_TOTAL_NUM> sp_recipe_created_flag;	// �����䷽�������б�
	int version_god_equip;//��װ�汾
	char god_equip_change_flag;
	char check_god_equip_base_voc_flag;
	char reserve_sh_2;
};

UNSTD_STATIC_CHECK(INTJEWELRY_SUCC_MAX_LIST == 6);//�����Ժ����޸���ת������
typedef char WorkShopParamHex[sizeof(WorkShopParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(WorkShopParamHex) < 4096);

#pragma pack(pop)
#endif