#ifndef __SMART_MOUNTS_SYSTEM_DEF_HPP__
#define __SMART_MOUNTS_SYSTEM_DEF_HPP__

#include "servercommon/servercommon.h"

#pragma pack(push, 4)

static const int MAX_SMART_MOUNTS_SYSTEM_NUM = 10;						// �����������ֵ
static const int MAX_SMART_MOUNTS_SYSTEM_ATTR_NUM = 5;					// ������������

static const int MAX_SMART_MOUNTS_SYSTEM_EQUIP_NUM = 4;					// ����װ��������� (Ŀǰ2��)
static const int MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_NUM = 6;			// ��������������ֵ	(Ŀǰ3��)
//static const int MAX_SMART_MOUNTS_SYSTEM_EXCLUSIVE_SKILL_NUM = 4;		// ����ר���������ֵ (Ŀǰ2��)  Ԥ��˫��
static const int MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_NUM = 4;			// ����ͳ���������ֵ (Ŀǰ2��)

static const int MAX_SMART_MOUNTS_SYSTEM_EQUIP_CUR_NUM = 2;				// ����װ����ǰ����				//����������
static const int MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM = 3;		// ����������ܵ�ǰ���ֵ			//�����������ó���
//static const int MAX_SMART_MOUNTS_SYSTEM_EXCLUSIVE_SKILL_CUR_NUM = 2;	// ����ר�����ܵ�ǰ���ֵ			//ר��������������
static const int MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_CUR_NUM = 2;		// ����ͳ�����ﵱǰ���ֵ

static const int MAX_SMART_MOUNTS_SYSTEM_CUR_SKILL_NUM = MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM; //+ MAX_SMART_MOUNTS_SYSTEM_EXCLUSIVE_SKILL_CUR_NUM;

UNSTD_STATIC_CHECK(MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM <= MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_NUM);
//UNSTD_STATIC_CHECK(MAX_SMART_MOUNTS_SYSTEM_EXCLUSIVE_SKILL_CUR_NUM <= MAX_SMART_MOUNTS_SYSTEM_EXCLUSIVE_SKILL_NUM);
UNSTD_STATIC_CHECK(MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_CUR_NUM <= MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_NUM);

static const int MAX_SMART_MOUNTS_SYSTEM_JEWELRY_LEVEL_NUM = 10;		//����ǿ�����ȼ�
static const int MAX_SMART_MOUNTS_SYSTEM_REVEAL_LEVEL_NUM = 20;			//����㻯��ߵȼ�

enum SMART_MOUNTS_SYSTEM_SPECIAL_FLAG
{
	SMART_MOUNTS_SYSTEM_SPECIAL_FLAG_BIT_BASE = 0,
	SMART_MOUNTS_SYSTEM_SPECIAL_FLAG_BIT_SKILL = 1,

	SMART_MOUNTS_SYSTEM_SPECIAL_FLAG_BIT_BASE_MAX
};
UNSTD_STATIC_CHECK(SMART_MOUNTS_SYSTEM_SPECIAL_FLAG_BIT_BASE_MAX < sizeof(char) * 8);

struct SmartMountsSystemBaseSkillParam
{
	SmartMountsSystemBaseSkillParam() { this->Reset(); }
	SmartMountsSystemBaseSkillParam(UInt16 _skill_id, char _skill_level): skill_id(_skill_id), skill_level(_skill_level), is_unlock(0) {}

	void Reset()
	{
		skill_id = 0;
		skill_level = 0;
		is_unlock = 0;
	}
	UInt16 skill_id;
	char skill_level;
	char is_unlock;			// 0:δ���� 1:�ѽ���
};

struct SmartMountsSystemItemData
{
	SmartMountsSystemItemData() { this->Reset(); }

	void Reset()
	{
		is_active_flag = 0;
		strength_level = 0;
		reserve_sh = 0;
	}

	char is_active_flag;				//�����ʶ 0:δ���� 1:�Ѽ��� 
	unsigned char strength_level;		//ǿ���ȼ�
	short reserve_sh;
};

struct SmartMountsSystemSingleParam
{
	SmartMountsSystemSingleParam() { this->Reset(); }

	void Reset()
	{
		bicycle_id = 0;
		level = 0;
		remain_skill_point = 0;
		energy_value = 0;
		is_use_advanced_flag = 0;
		holy_strength_level = 0;
		is_breach_flag = 0;
		is_refine_flag = 0;
		capability = 0;
		reveal_level = 0;
		reserve_sh = 0;
		for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_NUM; i++)
		{
			dominance_pet_index_list[i] = -1;
		}
		//memset(exclusive_level_list, 0, sizeof(exclusive_level_list));
		for (int i = 0; i < MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_NUM; i++)
		{
			base_skill_list[i].Reset();
		}
		memset(new_base_skill_list, 0, sizeof(new_base_skill_list));
		jewelry_data.Reset();
		for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_EQUIP_NUM; i++)
		{
			equip_list[i].Reset();
		}
		memset(reserve_ll, 0, sizeof(reserve_ll));
	}

	int GetReachSkillLevelCount(int skill_level)
	{
		if(skill_level <= 0) return 0;

		int reach_count = 0;
		/*	for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_EXCLUSIVE_SKILL_NUM; i++)
			{
				if (exclusive_level_list[i] >= skill_level)
				{
					reach_count++;
				}
			}*/
		for (int i = 0; i < MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_NUM; i++)
		{
			if (base_skill_list[i].skill_level >= skill_level)
			{
				reach_count++;
			}
		}
		return reach_count;
	}

	short bicycle_id;										
	UInt16 level;																				// ����ȼ�
	short remain_skill_point;																	// ʣ�༼�ܵ�(����)
	short energy_value;																			// ����ֵ
	char is_breach_flag;																		// �Ƿ񾭹�ͻ�� 1:���ͻ��
	char is_refine_flag;																		// �Ƿ����ϴ�� 1:��ϴ������(����)
	char is_use_advanced_flag;																	// ����ʹ�ñ�ʶ 0:δʹ�ý������� 1:ʹ�ý�������
	char holy_strength_level;																	// ʥ��ǿ���ȼ�
	int capability;																				// ����
	UInt16 reveal_level;																		// �㻯�ȼ�
	short reserve_sh;
	char dominance_pet_index_list[MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_NUM];					// ͳ������index
	//short exclusive_level_list[MAX_SMART_MOUNTS_SYSTEM_EXCLUSIVE_SKILL_NUM];					// ר�����ܵȼ�
	SmartMountsSystemBaseSkillParam base_skill_list[MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_NUM];	// ��������
	UInt16 new_base_skill_list[MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_NUM];							// ϴ����������ID(����)
	SmartMountsSystemItemData jewelry_data;														// ��������
	SmartMountsSystemItemData equip_list[MAX_SMART_MOUNTS_SYSTEM_EQUIP_NUM];					// װ������

	long long reserve_ll[3];
};

struct SmartMountsSystemParam
{
	SmartMountsSystemParam() { this->Reset(); }

	void Reset()
	{
		is_finish_fun_guide_flag = 0;
		ride_index = -1;
		reserve_sh = 0;
		memset(reserve_ll, 0, sizeof(reserve_ll));
		for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_NUM; i++)
		{
			smart_mounts_list[i].Reset();
		}
	}
	char is_finish_fun_guide_flag;			//�Ƿ���ɹ���ָ�� 0:δ��� 1:�����
	char ride_index;						//��Ҵ�ʱ��˵����� -1:δ��� 
	short reserve_sh;

	long long reserve_ll[3];
	SmartMountsSystemSingleParam smart_mounts_list[MAX_SMART_MOUNTS_SYSTEM_NUM];
};
typedef char SmartMountsSystemParamHex[sizeof(SmartMountsSystemParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(SmartMountsSystemParamHex) < 4096);

struct SmartMountsSystemPro
{
	SmartMountsSystemPro() { this->Reset(); }

	void SetData(const SmartMountsSystemSingleParam & data)
	{
		bicycle_id = data.bicycle_id;
		level = data.level;
		holy_strength_level = data.holy_strength_level;
		reveal_level = data.reveal_level;
		capability = data.capability;

		for (int i = 0; i < ARRAY_LENGTH(base_skill_list) && i < ARRAY_LENGTH(data.base_skill_list); i++)
		{
			base_skill_list[i] = data.base_skill_list[i];
		}
		jewelry_data = data.jewelry_data;
		for (int i = 0; i < ARRAY_LENGTH(equip_list) && i < ARRAY_LENGTH(data.equip_list); i++)
		{
			equip_list[i] = data.equip_list[i];
		}
	}

	void Reset()
	{
		bicycle_id = 0;
		level = 0;
		holy_strength_level = 0;
		reveal_level = 0;
		capability = 0;

		memset(attr_list, 0, sizeof(attr_list));
		for (int i = 0; i < ARRAY_LENGTH(base_skill_list); i++)
		{
			base_skill_list[i].Reset();
		}
		jewelry_data.Reset();
		for (int i = 0; i < ARRAY_LENGTH(equip_list); i++)
		{
			equip_list[i].Reset();
		}
	}

	short bicycle_id;					// ����ID
	UInt16 level;						// ����ȼ�
	short holy_strength_level;			// ʥ��ǿ���ȼ�
	UInt16 reveal_level;				// �㻯�ȼ�
	int capability;						// ����

	AttrInfo attr_list[MAX_SMART_MOUNTS_SYSTEM_ATTR_NUM];			//��������
	SmartMountsSystemBaseSkillParam base_skill_list[MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM];	// ��������
	SmartMountsSystemItemData jewelry_data;											// ��������
	SmartMountsSystemItemData equip_list[MAX_SMART_MOUNTS_SYSTEM_EQUIP_CUR_NUM];	// װ������
};

#pragma pack(pop)

#endif