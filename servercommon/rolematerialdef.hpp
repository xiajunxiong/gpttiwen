#ifndef __ROLE_MATERIAL_DEF_HPP__
#define __ROLE_MATERIAL_DEF_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/bitmapfunction.hpp"
#include "servercommon/struct/battle/battle_def.hpp"

static const int MAX_MATERIAL_CHAPTER_NUM = 20;						//�½�Ԥ��20��
static const int MAX_MATERIAL_CHECKPOINT_NUM = 15;					//ÿ���½ڹؿ�Ԥ��15��
static const int MAX_MATERIAL_CONDITON_NUM = 3;						//ɨ���������
static const int MAX_MATERIAL_ACHIEVENMENT_CONDITION_NUM = 3;		//�ɾ��������
static const int MAX_MATERIAL_COND_COUNT_NUM = MAX_MATERIAL_CONDITON_NUM + MAX_MATERIAL_ACHIEVENMENT_CONDITION_NUM;		
static const int MAX_MATERIAL_BOX_FLAG_NUM = 256;					//��������

static const int MATERIAL_FIRST_PASS_BIT_NUM = 7;					//Ĭ���״�ͨ��λ����λ��(ɨ����ʶ)

static const int MAX_MATERIAL_LEVEL_NUM = 400;
UNSTD_STATIC_CHECK(MAX_MATERIAL_LEVEL_NUM >= MAX_MATERIAL_CHAPTER_NUM * MAX_MATERIAL_CHECKPOINT_NUM);
UNSTD_STATIC_CHECK(BATTLE_CONDITION_MAX_NUM >= MAX_MATERIAL_COND_COUNT_NUM);

#pragma pack(push, 4)

struct MaterialLevelNode
{
	MaterialLevelNode() { this->Reset(); }

	void Reset()
	{
		chapter_id = -1;
		level_seq = -1;
		condition_flag = 0;
		achievenment_condition_flag = 0;
		day_pass_times = 0;
		buy_times = 0;
		reserve_sh = 0;
	}

	char chapter_id;
	char level_seq;
	unsigned char condition_flag;						//ɨ��������ɱ�ʶ ������
	unsigned char achievenment_condition_flag;			//�ɾ�������ɱ�ʶ ������
	char day_pass_times;								//��ҽ���ͨ�ش���
	char buy_times;										//��ҽ���Ĺ������
	short reserve_sh;				
};

struct MaterialChapterNode
{
	MaterialChapterNode() { this->Reset(); }
	
	void Reset()
	{
		chapter_id = -1;
		integral_num = 0;
	}
	short chapter_id;
	short integral_num;		
};

struct MaterialParam
{
	MaterialParam() { this->Reset(); }

	void Reset()
	{
		memset(reserve_ll, 0, sizeof(reserve_ll));
		max_chapter_id = -1;
		max_level_seq = -1;
		tips_times = 0;
		box_flag.Reset();
		day_pass_times_count = 0;
		level_count = 0;
		for (int i = 0; i < MAX_MATERIAL_LEVEL_NUM; i++)
		{
			level_data_list[i].Reset();
		}
	}

	long long reserve_ll[5];
	char max_chapter_id;							//��ҵ�ǰ������½�
	char max_level_seq;								//��ҵ�ǰ�����Ӧ�½ڵĹؿ�
	short tips_times;								//����ʾ��Ҵ���
	BitMap<MAX_MATERIAL_BOX_FLAG_NUM> box_flag;		
	short day_pass_times_count;						//������ͨ�ش���
	short level_count;
	MaterialLevelNode level_data_list[MAX_MATERIAL_LEVEL_NUM];
};

typedef char MaterialParamHex[sizeof(MaterialParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(MaterialParamHex) < 7168);


#pragma pack(pop)

#endif