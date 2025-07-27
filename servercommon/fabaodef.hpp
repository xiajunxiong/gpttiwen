#ifndef __FA_BAO_DEF_H__
#define __FA_BAO_DEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#pragma pack(push, 4)

static const int MAX_FABAO_COUNT = 43;// ����װ��������
static const int FABAO_INDEX_NO = -1;

enum FABAO_TYPE
{
	FABAO_TYPE_NO = 0,				//��
	FABAO_TYPE_ACTIVE = 1,			//��������
	FABAO_TYPE_PASSIVE = 2,			//��������
	FABAO_TYPE_BATTLE_PASSIVE = 3,			//ս����������
};

enum FABAO_TARGET
{
	FABAO_TARGET_ROLE = 1,			//������
	FABAO_TARGET_PET = 2,			//�Գ���
	FABAO_TARGET_SKILL = 3,			//����
};

static const int MAX_FABAO_EQUIPMENT_BAR = 4;// ������󴩴�����
UNSTD_STATIC_CHECK(MAX_FABAO_EQUIPMENT_BAR == 4);


struct FaBaoEquip
{
	FaBaoEquip() { this->Reset(); }
	void Reset()
	{
		fabao_index = FABAO_INDEX_NO;
		fabao_type = FABAO_TYPE_NO;
		fabao_jie = 0;
		fabao_level = 0;
		level_effect = -1;
		reserve_sh = 0;
		cur_exp = 0;
	}

	short fabao_index;
	short fabao_type;
	short fabao_jie;
	short fabao_level;
	short level_effect;
	short reserve_sh;
	int cur_exp;
};

struct FaBaoParam
{
	FaBaoParam()
	{
		this->Reset();
	}

	void Reset()
	{
		week_use_coin_times = 0;
		for (int i = 0; i < MAX_FABAO_EQUIPMENT_BAR; ++i)
		{
			m_fabao_equipment_bar[i] = -1;
		}
		for (int i = 0; i < MAX_FABAO_COUNT; ++i)
		{
			m_fabao_list[i].Reset();
		}
	}

	void WeekReset() 
	{
		week_use_coin_times = 0;
	}

	int week_use_coin_times;								//���ܽ��ʹ�ô���
	int m_fabao_equipment_bar[MAX_FABAO_EQUIPMENT_BAR];			//����װ����-��Ӧװ���ķ���index
	FaBaoEquip m_fabao_list[MAX_FABAO_COUNT];				//������Ϣ
};
typedef char FaBaoParamHex[sizeof(FaBaoParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(FaBaoParamHex) < 1536);

const static int FA_BAO_DRAW_MATERIAL_MAX_NUM = 5;			//���ϸ���
const static int FA_BAO_DRAW_ITEM_MAX_NUM = 4;			//�齱������
const static int FA_BAO_DRAW_MAX_RATE = 100;			//���ʺ�

struct FaBaoDrawItemInfo
{
	FaBaoDrawItemInfo()
	{
		this->Reset();
	}

	void Reset()
	{
		is_active = 0;
		reserved_ch = 0;
		reserved_sh = 0;
		end_time = 0;
		speed_up_time = 0;
		material_num = 0;
		for (int i = 0; i < ARRAY_LENGTH(material_item_id); ++i)
		{
			material_item_id[i] = 0;
		}
	}

	void ResetMaterial()
	{
		reserved_ch = 0;
		reserved_sh = 0;
		end_time = 0;
		speed_up_time = 0;
		material_num = 0;
		for (int i = 0; i < ARRAY_LENGTH(material_item_id); ++i)
		{
			material_item_id[i] = 0;
		}
	}
	
	unsigned char is_active;
	char reserved_ch;
	short reserved_sh;
	unsigned int end_time;			//����ʱ�䣨ʱ�����
	unsigned int speed_up_time;		//���� ��
	int material_num;		//��������
	int material_item_id[FA_BAO_DRAW_MATERIAL_MAX_NUM];		//��������id
};

struct FaBaoDrawParam			//����
{
	FaBaoDrawParam()
	{
		this->Reset();
	}

	void Reset()
	{
		for (int i = 0; i < ARRAY_LENGTH(item_info); ++i)
		{
			item_info[i].Reset();
		}

		item_info[0].is_active = 1;  //��һ��¯��Ĭ�ϼ���
	}
	
	FaBaoDrawItemInfo item_info[FA_BAO_DRAW_ITEM_MAX_NUM];
};

typedef char FaBaoDrawParamHex[sizeof(FaBaoDrawParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(FaBaoDrawParamHex) < 512);


#pragma pack(pop)

#endif
