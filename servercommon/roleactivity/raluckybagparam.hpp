#ifndef __RA_LUCKY_BAG_PARAM_HPP__
#define __RA_LUCKY_BAG_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 2062 ���� - ����  //////////////////////////////////////////


#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

struct LuckyBagBatchBuyItem
{
	LuckyBagBatchBuyItem() { this->Reset(); }
	void Reset()
	{
		end_time = 0;
		last_send_reward_time = 0;
	}


	unsigned int end_time;
	unsigned int last_send_reward_time;	
};

struct RALuckyBagParam
{
	RALuckyBagParam() { this->Reset(); }
	void Reset()
	{
		ra_lucky_bag_begin_timestamp = 0;
		ra_lucky_bag_buy_flag = 0;
		ra_lucky_bag_is_fetch_reward = 0;

		batch_buy_info.Reset();
	}

	unsigned int ra_lucky_bag_begin_timestamp;
	int ra_lucky_bag_buy_flag;				//����ÿ����ĵ�λ
	int ra_lucky_bag_is_fetch_reward;		//�Ƿ���ȡÿ�ս���
	LuckyBagBatchBuyItem batch_buy_info;  //һ�������������������ս����������ţ�����������ȡ�򣩣����ռ�֮��ÿ��0��ͨ���ʼ����Ž���
};

UNSTD_STATIC_CHECK(sizeof(RALuckyBagParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_LUCKY_BAG_PARAM_HPP__
