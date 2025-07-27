#ifndef __LOGIN_SYSTEM_DEF_HPP__
#define __LOGIN_SYSTEM_DEF_HPP__

#include <string.h>
#include "servercommon/servercommon.h"
#include "servercommon/bitmapfunction.hpp"
#pragma pack(push, 4)

static const int MAX_LOGIN_SYSTEM_NUM = 32;
UNSTD_STATIC_CHECK(sizeof(int) * 8 == MAX_LOGIN_SYSTEM_NUM);

struct LoginSystemParam
{
	LoginSystemParam() { this->Reset(); }

	void Reset()
	{
		login_day = 0;
		next_add_login_day_tamp = 0;
		reward_flag = 0;
		unlock_reward_flag.Reset();
		is_reward_flag.Reset();
	}
	//---------28���ۼƵ�½
	int login_day;//�ۼƵ�½����
	long long next_add_login_day_tamp;//��һ�ο��ۼƵ�½����ʱ���
	int reward_flag;//������ȡflag - 32λ(����)
	BitMap<MAX_LOGIN_SYSTEM_NUM> unlock_reward_flag;	// ��������flag
	BitMap<MAX_LOGIN_SYSTEM_NUM> is_reward_flag;		// �Ƿ�����ȡ����
};

typedef char LoginSystemParamHex[sizeof(LoginSystemParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(LoginSystemParamHex) < 256);

#pragma pack(pop)

#endif  // __MENTALITY_DEF_HPP__

