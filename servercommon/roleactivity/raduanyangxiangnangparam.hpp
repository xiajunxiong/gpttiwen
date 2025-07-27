#ifndef __RA_DUAN_YANG_XIANG_NANG_PARAM_HPP__
#define __RA_DUAN_YANG_XIANG_NANG_PARAM_HPP__

#include "servercommon/roleactivitydef.hpp"


#pragma pack(push,4)

static const int DUAN_WU_ACTIVITY_MAX_BITMAP_NUM = 32;

///////////////////////////// �������� ///////////////////////////////////
static const int MAX_TASK_NUM_DUAN_YANG_XIANG_NANG = 25;		// �����������ֵ
struct RADuanYangXiangNangParam
{
	RADuanYangXiangNangParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;

		today_task_finish.Reset();
		is_receive_reward.Reset();
		memset(today_task_count, 0, sizeof(today_task_count));
	}

	void DayReset()
	{
		today_task_finish.Reset();
		is_receive_reward.Reset();
		memset(today_task_count, 0, sizeof(today_task_count));
	}

	unsigned int ra_begin_timestamp;

	BitMap<DUAN_WU_ACTIVITY_MAX_BITMAP_NUM> today_task_finish;		// ÿ�������Ƿ����
	BitMap<DUAN_WU_ACTIVITY_MAX_BITMAP_NUM> is_receive_reward;		// ÿ���������Ƿ�����ȡ
	int today_task_count[MAX_TASK_NUM_DUAN_YANG_XIANG_NANG];		// ÿ���������
};
UNSTD_STATIC_CHECK(sizeof(RADuanYangXiangNangParam) <= sizeof(RoleActivityRoleData))


///////////////////////////// �����͸� ///////////////////////////////////

static const int CLCF_BITMAP_MAX_VALUE = 32;

struct RACangLongCiFuParam
{
	RACangLongCiFuParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		all_light_reward_flag = 0;

		light_up_flag.Reset();
	}

	unsigned int ra_begin_timestamp;

	int all_light_reward_flag;		// ȫ������������ʶ, 0--������ȡ, 1--����ȡ, 2--����ȡ
	BitMap<CLCF_BITMAP_MAX_VALUE> light_up_flag;
};
UNSTD_STATIC_CHECK(sizeof(RACangLongCiFuParam) <= sizeof(RoleActivityRoleData))


///////////////////////////// �������� ///////////////////////////////////
static const int MAX_TASK_NUM_LAN_TANG_CHU_MU = 25;		// �����������ֵ
static const int MAX_HERBS_TYPE_LAN_TANG_CHU_MU = 4;	// ���ҩ������
struct RALanTangChuMuParam
{
	RALanTangChuMuParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		last_reward_flag = 0;

		today_task_finish.Reset();
		is_receive_reward.Reset();
		memset(today_task_count, 0, sizeof(today_task_count));
		memset(add_herbs_num, 0, sizeof(add_herbs_num));
	}

	void DayReset()
	{
		last_reward_flag = 0;

		today_task_finish.Reset();
		is_receive_reward.Reset();
		memset(today_task_count, 0, sizeof(today_task_count));
		memset(add_herbs_num, 0, sizeof(add_herbs_num));
	}

	unsigned int ra_begin_timestamp;

	int last_reward_flag;		// ���ս���, 0--������ȡ, 1--����ȡ, 2--����ȡ

	BitMap<DUAN_WU_ACTIVITY_MAX_BITMAP_NUM> today_task_finish;		// ÿ�������Ƿ����
	BitMap<DUAN_WU_ACTIVITY_MAX_BITMAP_NUM> is_receive_reward;		// ÿ���������Ƿ�����ȡ
	int today_task_count[MAX_TASK_NUM_LAN_TANG_CHU_MU];				// ÿ���������

	short add_herbs_num[MAX_HERBS_TYPE_LAN_TANG_CHU_MU];	// �Ѽ���ҩ������
};
UNSTD_STATIC_CHECK(sizeof(RALanTangChuMuParam) <= sizeof(RoleActivityRoleData))


#pragma pack(pop)

#endif
