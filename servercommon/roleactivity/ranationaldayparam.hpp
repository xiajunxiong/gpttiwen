#ifndef __RA_NATIONAL_DAY_PARAM_HPP__
#define __RA_NATIONAL_DAY_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)

static const int MAX_RA_MI_BAO_FANG_DIAN_PHASE_SHOP_ITEM_NUM = 30;		//�ٱ�����׶��ڳ�����Ʒ�������ֵ
static const int MAX_RA_MI_BAO_FANG_DIAN_SHOP_ITEM_NUM = 70;			//�ٱ������ڳ�����Ʒ�������ֵ
//static const int MAX_RA_MI_BAO_FANG_DIAN_PHASE_NUM = 2;					//�ٱ�����׶��������ֵ


//�ٱ�����
struct RAMiBaoFangDianParam
{
	RAMiBaoFangDianParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		is_open_today_flag = 0;
		phase = 0;
		cur_use_open_day = 0;
		memset(reserve_ll, 0, sizeof(reserve_ll));
		memset(today_buy_times, 0, sizeof(today_buy_times));
		memset(cur_phase_activity_buy_times, 0, sizeof(cur_phase_activity_buy_times));
	}

	unsigned int ra_begin_timestamp;
	short is_open_today_flag;																//�����Ƿ�򿪹����� 0:û�� 1���򿪹�
	short phase;																			//��ɫ�׶μ�¼
	int cur_use_open_day;																	//ʹ�ø�������ȷ����������
	long long reserve_ll[3];
	short today_buy_times[MAX_RA_MI_BAO_FANG_DIAN_PHASE_SHOP_ITEM_NUM];						//ÿ���޹�
	short cur_phase_activity_buy_times[MAX_RA_MI_BAO_FANG_DIAN_PHASE_SHOP_ITEM_NUM];		//��ǰ�׶εĻ�޹�	
	//short activity_buy_times[MAX_RA_MI_BAO_FANG_DIAN_SHOP_ITEM_NUM];				//��޹� ����չʾ��һ�׶εĹ���
};
UNSTD_STATIC_CHECK(sizeof(RAMiBaoFangDianParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)

#endif