#ifndef __RA_YUN_ZE_WISH_PARAM_HPP__
#define __RA_YUN_ZE_WISH_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)

static const int YZ_WISH_MAX_REWARD_NUM = 5;			// �����ܵĽ�����

struct RAYunZeWishParam
{
	RAYunZeWishParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		is_can_lottery = 0;
		next_refresh_timestamp = 0;
		end_lock_timestamp = 0;
		lottery_lock = 0;
		is_send_timestamp = 0;
		is_lock_reward.Reset();
	}

	unsigned int ra_begin_timestamp;
	unsigned int next_refresh_timestamp;		// �´�ˢ�½�������ʱ���
	int is_send_timestamp;						// �����ͻ��˵�αʱ�����־λ��0-δ����
	unsigned int end_lock_timestamp;			// ���һ����������ʱ���
	short is_can_lottery;						// �Ƿ�ɳ齱��0-���ܣ�1-����
	short lottery_lock;							// ��־λ����ʶ��ǰʱ����Ƿ�����,0--δ�����1--���
	BitMap<YZ_WISH_MAX_REWARD_NUM> is_lock_reward;	
};
UNSTD_STATIC_CHECK(sizeof(RAYunZeWishParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)

#endif