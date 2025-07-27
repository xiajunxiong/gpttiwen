#ifndef __DAY_REWARD_DEF_HPP__
#define __DAY_REWARD_DEF_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#include "servercommon/bitmapfunction.hpp"

#pragma pack(push, 4)

static const int DAY_REWARD_ONLINE_GIFT_NUM = 32;		//��������������

struct RoleDayRewardParam
{
	RoleDayRewardParam()
	{
		this->Reset();
	}

	void Reset()
	{
		memset(reserve_ll, 0, sizeof(reserve_ll));
		is_old_user_flag = 0;
		is_can_join_online_gift_flag = 0;
		reserve_sh = 0;

		online_gift_join_begin_timestamp = 0;
		online_gift_record_begin_timestamp = 0;
		online_gift_times = 0;
		online_gift_flag.Reset();
	}
	long long reserve_ll[3];
	char is_old_user_flag;								// �Ƿ��жϹ��Ϻű�ʶ 0:δ�ж� 1:���ж�
	char is_can_join_online_gift_flag;					// �Ƿ�����ʸ�μ�������� 0:ľ���ʸ� 
	short reserve_sh;

	unsigned int online_gift_join_begin_timestamp;		// ��ʼ�����������ʱ���
	unsigned int online_gift_record_begin_timestamp;	// ��ʼ��¼�������ʱ���
	unsigned int online_gift_times;						// �ۼ�����ʱ���������ۼ�ʱ��Ϊ��(now-online_gift_record_begin_timestamp +online_gift_times)

	BitMap<DAY_REWARD_ONLINE_GIFT_NUM> online_gift_flag;// ���������ȡ��ʶ
};

typedef char RoleDayRewardParamHex[sizeof(RoleDayRewardParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleDayRewardParamHex) < 128);

#pragma pack(pop)
#endif
