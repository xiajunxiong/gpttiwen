#ifndef __RA_DAN_BI_FAN_LI_PARAM_HPP__
#define __RA_DAN_BI_FAN_LI_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push,4)

////////////////////////////////// ���ʷ��� ///////////////////////////////////////
static const int MAX_RA_DAN_BI_FAN_LI_POOL_NUM = 10;			//Ԫ��������

struct RADanBiFanLiParam
{
	RADanBiFanLiParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		memset(reserve_ll, 0, sizeof(reserve_ll));
		m_total_chongzhi = 0;

		memset(today_buy_times_pool_list, 0, sizeof(today_buy_times_pool_list));
		memset(activity_buy_times_pool_list, 0, sizeof(activity_buy_times_pool_list));
	}


	unsigned int ra_begin_timestamp;
	long long reserve_ll[3];

	int m_total_chongzhi;			// ��ڼ��ܳ�ֵ���
	short today_buy_times_pool_list[MAX_RA_DAN_BI_FAN_LI_POOL_NUM];			//���칺�����
	short activity_buy_times_pool_list[MAX_RA_DAN_BI_FAN_LI_POOL_NUM];		//��������
};
UNSTD_STATIC_CHECK(sizeof(RADanBiFanLiParam) <= sizeof(RoleActivityRoleData));


#pragma pack(pop)
#endif	// __RA_DAN_BI_FAN_LI_PARAM_HPP__