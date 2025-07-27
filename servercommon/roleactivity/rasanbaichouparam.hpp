#ifndef __RA_SANBAI_CHOU_PARAM_HPP__
#define __RA_SANBAI_CHOU_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// ���ٳ�Ƥ�� //////////////////////////////////////////

static const int MAX_RA_SANBAI_CHOU_TYPE_NUM = 10;			// �齱�ȼ����䲻�ܳ���10��
static const int MAX_RA_SANBAI_CHOU_DRAW_NUM = 10;			// ʮ����

struct RASanBaiChouTypeParam
{
	RASanBaiChouTypeParam() : times(0), select_seq(-1) 
	{
		memset(pool, -1, sizeof(pool));
	}

	void Reset()
	{
		times = 0;
		select_seq = -1;
		memset(pool, -1, sizeof(pool));
	}

	short times;
	short select_seq;
	short pool[MAX_RA_SANBAI_CHOU_DRAW_NUM];
};

#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////
struct RASanBaiChouParam
{
	RASanBaiChouParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		draw_times = 0;
		baodi_total_times = 0;

		reward_item_seq = -1;
		
		for (int i = 0; i < MAX_RA_SANBAI_CHOU_TYPE_NUM; i++)
		{
			select[i].Reset();
		}
	}

	unsigned int ra_begin_timestamp;
	int draw_times;													// �ܹ��齱����
	short baodi_total_times;										// û�г鵽��ϡ�������ۻ������������ж��Ƿ���ͱ�	
	short reward_item_seq;											// ����ѡ��Ľ���

	RASanBaiChouTypeParam select[MAX_RA_SANBAI_CHOU_TYPE_NUM];		// ���Ͳ���
};

UNSTD_STATIC_CHECK(sizeof(RASanBaiChouParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)
#endif	//__RA_SANBAI_CHOU_PARAM_HPP__
