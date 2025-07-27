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

	long long origin_uuid;	// 原服uuid, 可通过LongLongToUniqueUserID转换
	int origin_server_id;	// 玩家原服的服务器ID
	GameName origin_role_name;	// 原服的玩家名
	int origin_plat_type;	// 渠道
	PlatName origin_pname;	// 原服pname 用于模拟session_key
};

typedef char RoleCrossParamHex[sizeof(RoleCrossParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleCrossParamHex) < 512);

#pragma pack(pop)

#endif