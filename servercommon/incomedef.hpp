#ifndef __INCOME_DEF__
#define __INCOME_DEF__

#include "servercommon/servercommon.h"

///////////////////////����ͳ��///////////////////
//��������
enum IncomeRoleType						//			��ɫ			ȫ��
{										//			param			p_ll	p			Э�����
	INCOME_ROLE_TYPE_DAY_EXP = 0,		//ÿ��-����	�����ջ�		����	�μ�����	ƽ����
	INCOME_ROLE_TYPE_DAY_COIN_BIND,		//ÿ��-ͭ��	�����ջ�		����	�μ�����	ƽ����
	INCOME_ROLE_TYPE_DAY_SILVER_COIN,	//ÿ��-����	�����ջ�		����	�μ�����	ƽ����

	INCOME_ROLE_TYPE_MAX,
};

#pragma pack(push, 4)
//��ɫ����
struct RoleIncomeParam
{
	RoleIncomeParam()
	{
		this->Reset();
	}
	void Reset()
	{
		memset(hold_ll_list, 0, sizeof(hold_ll_list));
		memset(income_info_list_, 0, sizeof(income_info_list_));
	}

	long long hold_ll_list[2];

	long long income_info_list_[INCOME_ROLE_TYPE_MAX];

};
typedef char RoleIncomeParamHex[sizeof(RoleIncomeParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleIncomeParamHex) < 256);

#pragma pack(pop)

#endif