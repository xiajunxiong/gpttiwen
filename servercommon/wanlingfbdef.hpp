#ifndef __WAN_LING_FB_DEF_HPP__
#define __WAN_LING_FB_DEF_HPP__

#include "servercommon/servercommon.h"

//���黰��
namespace WanLingFB
{
	static const int FB_MAX = 20;

	//�����콱����(���������)
	enum FBFetchType
	{
		FB_FETCH_TYPE_CAN_FETCH = 0,//����
		FB_FETCH_TYPE_NOT_FETCH = 1,//������
	};
};
#pragma pack(push, 4)
struct WanLingFBParam
{
	WanLingFBParam()
	{
		this->Reset();
	}
	void Reset()
	{
		fb_fetch_flag = 0;
		hold_ll = 0;

		day_fetch_battle_reward = 0;
		day_fetch_pass_reward = 0;
	}

	int fb_fetch_flag;				//�����콱���[����_index]
	long long hold_ll;

	int day_fetch_battle_reward;	//������ȡս����������
	int day_fetch_pass_reward;		//������ȡͨ�ؽ�������
};

typedef char WanLingFBParamHex[sizeof(WanLingFBParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(WanLingFBParamHex) < 128);

#pragma pack(pop)

#endif