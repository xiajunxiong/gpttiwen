#ifndef __RA_SHAN_HAI_BAO_DAI_PARAM_HPP__
#define __RA_SHAN_HAI_BAO_DAI_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/bitmapfunction.hpp"
#include "servercommon/roleactivitydef.hpp"

//////////////////////////////////////////// 2159 ���� - ɽ������  //////////////////////////////////////////
const int static RA_SHAN_HAI_BAO_DAI_MAX_ITEM = 20;
const int static RA_SHAN_HAI_BAO_DAI_BUY_MAX_ITEM = 32;
UNSTD_STATIC_CHECK(RA_SHAN_HAI_BAO_DAI_BUY_MAX_ITEM >= RA_SHAN_HAI_BAO_DAI_MAX_ITEM);

#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

struct RAShanHaiBaoDaiParam
{
	RAShanHaiBaoDaiParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		can_buy_seq = 0;
		has_free_reward = 0;
		for (int i = 0; i < RA_SHAN_HAI_BAO_DAI_MAX_ITEM; ++i)
		{
			ra_day_buy_times[i] = 0;
			ra_buy_times[i] = 0;
		}
		buy_flag.Reset();
	}

	unsigned int ra_begin_timestamp;
	int can_buy_seq;							//����������seq��ֻ�й����ǰһ�������������
	unsigned short ra_day_buy_times[RA_SHAN_HAI_BAO_DAI_MAX_ITEM];			//ÿ����Ʒÿ���޹�����
	unsigned short ra_buy_times[RA_SHAN_HAI_BAO_DAI_MAX_ITEM];				//ÿ����Ʒ�޹�����
	BitMap<RA_SHAN_HAI_BAO_DAI_BUY_MAX_ITEM> buy_flag;						//�����ʶ 0:δ���� 1:�ѹ���
	int has_free_reward;													//��ѱ��䣬0-δ��ȡ��1-����ȡ
};

UNSTD_STATIC_CHECK(sizeof(RAShanHaiBaoDaiParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)

#endif	//__RA_SHAN_HAI_BAO_DAI_PARAM_HPP__
