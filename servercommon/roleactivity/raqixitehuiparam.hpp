#ifndef __RA_QI_XI_TE_HUI_PARAM_HPP__
#define __RA_QI_XI_TE_HUI_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 2209 ���� - ��Ϧ�ػ�  //////////////////////////////////////////

static const int RA_QI_XI_TE_HUI_MAX_BASE_NUM = 32;				//һ��ʱ��׶�����ػ��������
static const int RA_QI_XI_TE_HUI_MAX_PHASE_NUM = 32;			//һ��ʱ��׶����׶ν�������

UNSTD_STATIC_CHECK(RA_QI_XI_TE_HUI_MAX_BASE_NUM % 4 == 0);

#pragma pack(push, 4)

////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

struct RAQiXiTeHuiParam
{
	RAQiXiTeHuiParam() { this->Reset(); }
	void Reset()
	{
		ra_qi_xi_te_hui_begin_timestamp = 0;
		reserve_ll = 0;
		is_init_open_interface_flag = 0;
		reserve_ch = 0;
		activity_buy_count = 0;
		phase_reward_flag.Reset();
		for (int i = 0; i < ARRAY_LENGTH(today_buy_times); ++i)
		{
			today_buy_times[i] = 0;
		}

		for (int i = 0; i < ARRAY_LENGTH(activity_buy_times); ++i)
		{
			activity_buy_times[i] = 0;
		}
	}

	unsigned int ra_qi_xi_te_hui_begin_timestamp;
	long long reserve_ll;
	char is_init_open_interface_flag;									//�Ƿ��ʼ�򿪹����� 1:�Ѵ�
	char reserve_ch;
	short activity_buy_count;											//��ڼ乺����ܴ���
	BitMap<RA_QI_XI_TE_HUI_MAX_PHASE_NUM> phase_reward_flag;			//�׶ν�����ȡ��ʶ
	short today_buy_times[RA_QI_XI_TE_HUI_MAX_BASE_NUM];				//ÿ���޹�
	short activity_buy_times[RA_QI_XI_TE_HUI_MAX_BASE_NUM];				//��޹�
};

UNSTD_STATIC_CHECK(sizeof(RAQiXiTeHuiParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_QI_XI_TE_HUI_PARAM_HPP__
