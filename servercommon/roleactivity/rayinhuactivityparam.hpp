#ifndef __RA_YINHU_ACTIVITY_PARAM_HPP__
#define __RA_YINHU_ACTIVITY_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// ����� //////////////////////////////////////////
static const int RA_YINHU_WENSHI_DAY_MAX = 6;			// ����������������
static const int RA_CINIU_YINGXINHU_TYPE_MAX = 5;		// ��ţӭ�»� ����
static const int RA_HU_XING_DA_YUN_USER_NUM = 5;		// ���д��� ���������б�����

struct RACiNiuYinXinHuTypeData
{
	short reward_mark;
	unsigned short param;
};

struct RAHuXingDaYunBeInvitedData
{
	int uid;
	int prof_id;
	short	avatar_type;							// ��������
	short	headshot_id;							// ͷ��ID	
	GameName role_name;								// �������
};

#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////



////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////


struct RAYinHuWenShiParam
{
	RAYinHuWenShiParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		zhigou_flag = 0;
		reward_fetch_mark = 0;
		memset(reward_type, -1, sizeof(reward_type));
	}

	unsigned int ra_begin_timestamp;
	short zhigou_flag;
	short reward_fetch_mark;

	short reward_type[RA_YINHU_WENSHI_DAY_MAX];
};

UNSTD_STATIC_CHECK(sizeof(RAYinHuWenShiParam) <= sizeof(RoleActivityRoleData))

struct RACiNiuYingXinHuParam
{
	RACiNiuYingXinHuParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		awaken_mark = 0;
		awaken_reward_time = 0;

		memset(awaken_param, 0, sizeof(awaken_param));
	}

	unsigned int ra_begin_timestamp;

	short awaken_mark;
	short awaken_reward_time;
	RACiNiuYinXinHuTypeData awaken_param[RA_CINIU_YINGXINHU_TYPE_MAX];
};

UNSTD_STATIC_CHECK(sizeof(RACiNiuYingXinHuParam) <= sizeof(RoleActivityRoleData))

struct RAHuXingDaYunParam
{
	RAHuXingDaYunParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		complete_time = 0;
		task_pos_id = 0;
		task_first_pass = 0;
		is_reward = 0;

		is_be_invited_reward = 0;
		be_invited_times = 0;
		be_invited_complete_time = 0;

		memset(be_invited_list, 0, sizeof(be_invited_list));

		be_invited_bos_id = 0;
	}

	unsigned int ra_begin_timestamp;
	unsigned int complete_time;						// ��Ϸ���ʱ���
	short task_pos_id;								// ��ǰ�����
	short task_first_pass;							// �Ƿ���ͨ
	char is_reward;									// �Ƿ��ߵ������ ������ȡ����

	char is_be_invited_reward;						// �����뽱���Ƿ����ȡ
	short be_invited_times;							// ���������
	unsigned int be_invited_complete_time;			// ������ͬ�� ���ʱ���

	RAHuXingDaYunBeInvitedData be_invited_list[RA_HU_XING_DA_YUN_USER_NUM];

	int be_invited_bos_id;							// �����뿪ʼ�������
};

UNSTD_STATIC_CHECK(sizeof(RAHuXingDaYunParam) <= sizeof(RoleActivityRoleData))

struct RAYinHuBaiSuiParam
{
	RAYinHuBaiSuiParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		zhigou_flag = 0;
		reward_fetch_mark = 0;
	}

	unsigned int ra_begin_timestamp;
	short zhigou_flag;
	short reward_fetch_mark;
};

UNSTD_STATIC_CHECK(sizeof(RAYinHuBaiSuiParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)
#endif	//__RA_YINHU_ACTIVITY_PARAM_HPP__
