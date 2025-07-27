#ifndef __FLY_UP_SYSTEM_DEF_H__
#define __FLY_UP_SYSTEM_DEF_H__

#include "servercommon/bitmapfunction.hpp"
#include "servercommon/activitydef.hpp"

#pragma pack(push, 4)

const static int MAX_FLY_UP_TASK_IDX = 10;//ÿ����������������

struct FlyUpSystemParam
{
	/*************** **********************/
	FlyUpSystemParam() { this->Reset(); }

	void Reset()
	{
		task_idx = 0;
		fly_up_flag = 0;
		is_shilian = 0;
		fly_skill_level = 0;
		skill_exp = 0;
		memset(element, 0, sizeof(element));
		memset(organ, 0, sizeof(organ));
	}

	enum FLY_UP_STAG
	{
		FLY_UP_STAG_NO = 0,	//��
		FLY_UP_STAG_1 = 1,	//�����׶�1
		FLY_UP_STAG_2 = 2,	//�����׶�2
		FLY_UP_STAG_MAX,
	};

	enum FLY_UP_SHILIAN
	{
		FLY_UP_SHILIAN_NO = 0,
		FLY_UP_SHILIAN_YES = 1,
		FLY_UP_SHILIAN_MAX,
	};

	enum FLY_UP_ELEMENT
	{
		FLY_UP_ELEMENT_0 = 0,//Ԫ�ػ�
		FLY_UP_ELEMENT_1 = 1,//Ԫ�ط�
		FLY_UP_ELEMENT_2 = 2,//Ԫ��ˮ
		FLY_UP_ELEMENT_3 = 3,//Ԫ����
		FLY_UP_ELEMENT_MAX,
	};

	enum FLY_UP_ORGAN
	{
		FLY_UP_ORGAN_0 = 0,//��
		FLY_UP_ORGAN_1,//��
		FLY_UP_ORGAN_2,//�
		FLY_UP_ORGAN_3,//��
		FLY_UP_ORGAN_4,//��
		FLY_UP_ORGAN_MAX,
	};

	bool ElementAtleastValue(int value)//Ԫ����Ͳ�С��vlaue
	{
		for (int i = 0; i < FLY_UP_ELEMENT_MAX; i++)
		{
			if (element[i] < value)
			{
				return false;
			}
		}
		return true;
	}

	bool OrganAtleastValue(int value)//��Ͳ�С��vlaue
	{
		for (int i = 0; i < FLY_UP_ORGAN_MAX; i++)
		{
			if (organ[i] < value)
			{
				return false;
			}
		}
		return true;
	}

	void GetElementNotFullList(int max_value ,int *out_num, int *out_element[FLY_UP_ELEMENT_MAX], int out_idx[FLY_UP_ELEMENT_MAX])
	{
		*out_num = 0;
		for (int idx = 0; idx < FLY_UP_ELEMENT_MAX; idx++)
		{
			if (element[idx] < max_value)
			{
				out_element[*out_num] = &element[idx];
				out_idx[*out_num] = idx;
				*out_num += 1;
			}
		}
	}

	void GetOrganNotFullList(int max_value, int *out_num, int *out_organ[FLY_UP_ORGAN_MAX], int out_idx[FLY_UP_ORGAN_MAX])
	{
		*out_num = 0;
		for (int idx = 0; idx < FLY_UP_ORGAN_MAX; idx++)
		{
			if (organ[idx] < max_value)
			{
				out_organ[*out_num] = &organ[idx];
				out_idx[*out_num] = idx;
				*out_num += 1;
			}
		}
	}
	//����
	int task_idx;//��N������/������ɴ���
	int fly_up_flag;//�����׶�
	int is_shilian;//�Ƿ�������
	int fly_skill_level;
	int skill_exp;

	//����1
	int element[FLY_UP_ELEMENT_MAX];
	//����2
	int organ[FLY_UP_ORGAN_MAX];

	int add_value;
	int add_idx;
};

typedef char FlyUpSystemParamHex[sizeof(FlyUpSystemParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(FlyUpSystemParamHex) < 256);

#pragma pack(pop)
#endif