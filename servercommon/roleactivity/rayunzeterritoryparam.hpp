#ifndef __RA_YUN_ZE_TERRITORY_PARAM_HPP__
#define  __RA_YUN_ZE_TERRITORY_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


static const int PICTURE_FRAGMENTS_NUM = 7;		// ƴͼ��Ƭ�������
static const int PICTURE_NUM = 3;				// ͼƬ����

#pragma pack(push, 4)

enum REWARD_STATUS
{
	REWARD_STATUS_EXIST = 0,	// ����δ��ȡ
	REWARD_STATUS_LACK = 1,		// ��������ȡ
};

struct RAYunZeTerritoryParam
{
	RAYunZeTerritoryParam() { this->Reset(); }

	void Reset()
	{
		is_get_reward = 0;
		times = 0;
		ra_begin_timestamp = 0;

		memset(light_up,0,sizeof(light_up));
	}

	void OnDayReset()
	{
		is_get_reward = 0;
		times = 0;

		memset(light_up, 0, sizeof(light_up));
	}

	unsigned int ra_begin_timestamp;
	int light_up[PICTURE_FRAGMENTS_NUM];	//��������Ƭ,1-������0-δ����
	short is_get_reward;	// �Ƿ���ȡ���� 1-����ȡ��0-δ��ȡ
	short times;		// ������̽������

};
UNSTD_STATIC_CHECK(sizeof(RAYunZeTerritoryParam) <= sizeof(RoleActivityRoleData));


#pragma pack(pop)

#endif