#ifndef __BIG_CROSS_SERVER_DEF_H__
#define __BIG_CROSS_SERVER_DEF_H__

#include "servercommon/basedef.h"
#include "servercommon/serverdef.h"
#include "common/tlvprotocol.h"
#include "servercommon.h"
#include <set>


namespace bigcrossdef
{
	static const int BIG_CROSS_CONNECT_ASYN_TIME_OUT_MS = 10000;		// �첽����big_cross��ʱ���� ����

	struct NetInfo		// ������Ϣ�����ṹ
	{
		NetInfo() { this->Reset(); }

		void Reset()
		{
			is_check = false;
			u_hs_id = INVALID_UNIQUE_HIDDEN_SERVER_ID;
			ip_str.clear();
			port = 0;
		}

		bool is_check;									// �Ƿ�ͨ������Կ��֤ , �װ���������֤��
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
		int server_open_timestamp;				// ��Ӧ���ط��Ŀ���ʱ��
		// ��Ҫʲô��ʲô
		std::set<int> server_id;		// �����ط���Ӧ�ķ�����ID
	};
};

#pragma pack(push, 4)

#pragma pack(pop)

#endif // __BIG_CROSS_SERVER_DEF_H__


