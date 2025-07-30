#ifndef __MSG_TIGER_BRINGS_BLESSING_HPP__
#define __MSG_TIGER_BRINGS_BLESSING_HPP__

#include "gamedef.h"
#include "servercommon/struct/tigerbringsblessingparam.hpp"
#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push, 4)

namespace Protocol
{
	enum TIGER_BRINGS_BLESSING_CLIENT_REQ_TYPE
	{
		TIGER_BRINGS_BLESSING_CLIENT_REQ_TYPE_BUY = 0,							// ����
		TIGER_BRINGS_BLESSING_CLIENT_REQ_TYPE_GET_REWARD_LIST = 1,				// ��ȡ������
	};

	class CSTigerBringsBlessingClientReq						//!< 4094 �ͻ��˲�������
	{
	public:
		CSTigerBringsBlessingClientReq();
		MessageHeader header;

		int req_type;						// ��������
	};

	class SCTigerBringsBlessingRewardListInfo					//!< 4095 �н���Ա����
	{
	public:
		SCTigerBringsBlessingRewardListInfo();
		MessageHeader header;

		struct UserInfo
		{
			UserInfo() :uid(0), avatar_type(0), headshot_id(0)
			{
				memset(role_name, 0, sizeof(GameName));
			}

			int uid;										// =0 ���ʾ�ǻ�����
			short	avatar_type;							// ��������
			short	headshot_id;							// ͷ��ID
			GameName role_name;								// �������
		};

		UserInfo user_info[MAX_COW_REPORT_GOOD_NEWS_REWARD_USER_COUNT];		// ������
	};

	class SCTigerBringsBlessingInfo								//!< 4096 �����ɸ�ϵͳ��Ϣ
	{
	public:
		SCTigerBringsBlessingInfo();
		MessageHeader header;

		int curren_status;				// ��ǰ�Ļ״̬

		unsigned int buy_start_times;			// ������ʱ��
		unsigned int buy_end_times;				// �������ʱ��
		unsigned int show_start_times;			// չʾ�ڿ���ʱ��
		unsigned int show_end_times;			// չʾ�ڽ���ʱ��
	};


	class SCTigerBringsBlessingRoleBuyInfo							//!< 4097 ��ҵĹ�����Ϣ
	{
	public:
		SCTigerBringsBlessingRoleBuyInfo();
		MessageHeader header;

		int is_buy;													// ����Լ��Ƿ���� 0 û�� 1 �ѹ���
	};
}

#pragma pack(pop)
#endif