#ifndef __ROLE_CROSS_DEF_HPP__
#define __ROLE_CROSS_DEF_HPP__

#include "servercommon/servercommon.h"

#pragma pack(push, 4)

struct RoleCrossParam
{
	RoleCrossParam()
	{
		this->Reset();
	}

	void Reset()
	{
		origin_uuid = 0;
		origin_server_id = 0;
		memset(origin_role_name, 0, sizeof(origin_role_name));
		origin_plat_type = 0;
	}

	long long origin_uuid;	// ԭ��uuid, ��ͨ��LongLongToUniqueUserIDת��
	int origin_server_id;	// ���ԭ���ķ�����ID
	GameName origin_role_name;	// ԭ���������
	int origin_plat_type;	// ����
	PlatName origin_pname;	// ԭ��pname ����ģ��session_key
};

typedef char RoleCrossParamHex[sizeof(RoleCrossParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleCrossParamHex) < 512);

#pragma pack(pop)

#endif