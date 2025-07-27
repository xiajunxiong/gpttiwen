#ifndef __LEVEL_COMPLEMENT_DEF_HPP__
#define __LEVEL_COMPLEMENT_DEF_HPP__

#include "servercommon/servercommon.h"

static const int MAX_LEVEL_COMPLEMENT_RANK_NUM = 50;	//���ȡ����ǰ50��
static const int MAX_LEVEL_COMPLEMENT_MULTIPLE_NUM = 3;	

enum LEVEL_COMPLEMENT_MULTIPLE_TYPE
{
	LEVEL_COMPLEMENT_MULTIPLE_TYPE_0 = 0,		//������ȡ����
	LEVEL_COMPLEMENT_MULTIPLE_TYPE_1 = 1,		//��һ��û��,�ڶ������ӵı���
	LEVEL_COMPLEMENT_MULTIPLE_TYPE_2 = 2,		//ǰ����û��,���������ӵı���

	LEVEL_COMPLEMENT_MULTIPLE_TYPE_MAX
};

UNSTD_STATIC_CHECK(MAX_LEVEL_COMPLEMENT_MULTIPLE_NUM >= LEVEL_COMPLEMENT_MULTIPLE_TYPE_MAX);

#pragma pack(push, 4)

struct RoleNewLevelComplementParam
{
	RoleNewLevelComplementParam() { this->Reset(); }

	void Reset()
	{
		ArrayFillZero(reserve_ll);
		accumulation_exp = 0;
		day_supplement_exp = 0;
		today_last_get_daily_exp_timestamp = 0;
		last_get_daily_exp_timestamp = 0;
		day_buy_exp_count = 0;
		is_first_achieve_fun_open_flag = 0;
		reserve_ch = 0;
	}
	
	long long reserve_ll[3];
	long long accumulation_exp;							//�ۻ�����
	long long day_supplement_exp;						//�����賿����ľ�����ֵ
	unsigned int today_last_get_daily_exp_timestamp;	//�ϴλ���ճ�����ʱ���(����)
	unsigned int last_get_daily_exp_timestamp;			//�ϴλ���ճ�����ʱ���(���ǵ���)
	short day_buy_exp_count;							//���칺�������
	char is_first_achieve_fun_open_flag;				//�Ƿ����״δﵽ�ù��ܿ���  0:�� 1:����
	char reserve_ch;

};
typedef char RoleNewLevelComplementParamHex[sizeof(RoleNewLevelComplementParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleNewLevelComplementParamHex) < 256);


#pragma pack(pop)

#endif