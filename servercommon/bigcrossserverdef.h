#ifndef __BIG_CROSS_SERVER_DEF_H__
#define __BIG_CROSS_SERVER_DEF_H__

#include "servercommon/basedef.h"
#include "servercommon/serverdef.h"
#include "common/tlvprotocol.h"
#include "servercommon.h"
#include <set>


namespace bigcrossdef
{
	static const int BIG_CROSS_CONNECT_ASYN_TIME_OUT_MS = 10000;		// 异步连接big_cross超时限制 毫秒

	struct NetInfo		// 网络信息基础结构
	{
		NetInfo() { this->Reset(); }

		void Reset()
		{
			is_check = false;
			u_hs_id = INVALID_UNIQUE_HIDDEN_SERVER_ID;
			ip_str.clear();
			port = 0;
		}

		bool is_check;									// 是否通过了秘钥验证 , 首包必须是验证包
		UniqueHiddenServerID u_hs_id;

		std::string ip_str;
		Port port;
	};

	struct HiddenGameServerInfo
	{
		HiddenGameServerInfo() { this->Reset(); }

		void Reset()
		{
			net_id = -1;
			server_open_timestamp = 0;
		}

		NetID net_id;							// net_id;
		int server_open_timestamp;				// 对应隐藏服的开服时间
		// 需要什么加什么
		std::set<int> server_id;		// 该隐藏服对应的服务器ID
	};
};

#pragma pack(push, 4)

#pragma pack(pop)

#endif // __BIG_CROSS_SERVER_DEF_H__


