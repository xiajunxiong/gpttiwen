#ifndef __MSG_COW_REPORT_GOOD_NEWS_HPP__
#define __MSG_COW_REPORT_GOOD_NEWS_HPP__

#include "gamedef.h"
#include "servercommon/struct/cowreportgoodnewsparam.h"
#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push, 4)

namespace Protocol
{
	enum COW_REPORT_GOOD_NEWS_CLIENT_REQ_TYPE
	{
		COW_REPORT_GOOD_NEWS_CLIENT_REQ_TYPE_BUY = 0,							// ����
		COW_REPORT_GOOD_NEWS_CLIENT_REQ_TYPE_GET_REWARD_LIST = 1,				// ��ȡ������
	};

	class CSCowReportGoodNewsClientReq						//!< 4090 - �ͻ��˲�������
	{
	public:
		CSCowReportGoodNewsClientReq();
		MessageHeader header;

		int req_type;						// ��������
	};

	class SCCowReportGoodNewsRewardListInfo					//!< 4091 - ��ȡ�н���Ա����
	{
	public:
		SCCowReportGoodNewsRewardListInfo();
		MessageHeader header;

		struct UserInfo
		{
			UserInfo():uid(0) , avatar_type(0), headshot_id(0)
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

	class SCCowReportGoodNewsInfo								//!< 4092 - ��ţ��ϲ��ϵͳ��Ϣ
	{
	public:
		SCCowReportGoodNewsInfo();
		MessageHeader header;

		int curren_status;				// ��ǰ�Ļ״̬

		unsigned int buy_start_times;			// ������ʱ��
		unsigned int buy_end_times;				// �������ʱ��
		unsigned int show_start_times;			// չʾ�ڿ���ʱ��
		unsigned int show_end_times;			// չʾ�ڽ���ʱ��
	};


	class SCCowReportGoodNewsRoleBuyInfo							//!< 4093 - ��ҵĹ�����Ϣ
	{
	public:
		SCCowReportGoodNewsRoleBuyInfo();
		MessageHeader header;

		int is_buy;													// ����Լ��Ƿ���� 0 û�� 1 �ѹ���
	};
}

#pragma pack(pop)
#endif	// __MSG_COW_REPORT_GOOD_NEWS_HPP__