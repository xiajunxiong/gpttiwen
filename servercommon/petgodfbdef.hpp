#ifndef __PET_GOD_FB_DEF_HPP__
#define __PET_GOD_FB_DEF_HPP__

#include "servercommon/servercommon.h"

//��ӡ֮��
static const int PET_GOD_REWARD_GROUP_BASE_RATE = 10000;		//�������������(��ֱ�)

static const int PET_GOD_FB_MAX_TYPE_NUM = 4;//��������


static const int PET_GOD_FB_DRAW_NUM = 5;	//��������


#pragma pack(push, 4)
//��ӡ֮�ؽ�ɫ����
struct PetGodFBParam
{
	PetGodFBParam()
	{
		this->Reset();
	}
	void Reset()
	{
		day_pass_times = 0;
		day_buy_times = 0;
		hold_ll = 0;
		memset(fb_info_list, 0, sizeof(fb_info_list));
	}

	struct FBInfo
	{
		FBInfo()
		{
			max_history_pass_level = 0;
			hold_i = 0;
		}
		int max_history_pass_level;		//��ʷ���ͨ�ز���
		int hold_i;
	};


	int day_pass_times;		//����ͨ�ش���
	int day_buy_times;		//���չ������

	long long hold_ll;

	//������Ϣ[�������� - 1]
	FBInfo fb_info_list[PET_GOD_FB_MAX_TYPE_NUM];

};

typedef char PetGodFBParamHex[sizeof(PetGodFBParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(PetGodFBParamHex) < 128);


#pragma pack(pop)

#endif