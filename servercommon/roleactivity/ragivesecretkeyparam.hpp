#ifndef __RA_GIVE_SECRET_KEY_PARAM_HPP__
#define __RA_GIVE_SECRET_KEY_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"
#include "servercommon/rolecrystalfbdef.h"


//////////////////////////////////////////// 2099 ���� - ��Ѱ������  //////////////////////////////////////////

#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

struct RAGiveSecretKeyParam
{
	RAGiveSecretKeyParam() { this->Reset(); }

	void Reset()
	{
		ra_give_secret_key_begin_timestamp = 0;
		ra_reward_flag = 0;
		memset(frist_pass_flag, 0, sizeof(frist_pass_flag));
	}

	unsigned int ra_give_secret_key_begin_timestamp;
	int ra_reward_flag;										// ������ȡ�������ƣ�
	unsigned int frist_pass_flag[CRYSTAL_FB_FIRST_FLAG];
};

UNSTD_STATIC_CHECK(sizeof(RAGiveSecretKeyParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)

#endif	//__RA_GIVE_SECRET_KEY_PARAM_HPP__
