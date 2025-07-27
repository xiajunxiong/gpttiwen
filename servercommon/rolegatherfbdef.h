#ifndef __ROLE_GATHER_DEF_H__
#define __ROLE_GATHER_DEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"

static const int GATHER_LIST_ITEM_TYPE_MAX = 10;

#pragma pack(push, 4)

struct RoleGatherFbParam
{
	RoleGatherFbParam()
	{
		this->Reset();
	}

	void Reset()
	{
		gather_power = 0;
		last_add_gather_power_time = 0;
		is_first = 0;
	}

	int gather_power;	
	unsigned int last_add_gather_power_time;
	int is_first;								//����Ƿ��Ǹս��� 0 : �ս���   1 : �Ѿ�������ȡ�˵�һ�������
};

typedef char RoleGatherFbParamHex[sizeof(RoleGatherFbParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleGatherFbParamHex) < 128);

#pragma pack(pop)

#endif
