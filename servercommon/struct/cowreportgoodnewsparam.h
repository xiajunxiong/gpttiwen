#ifndef __COW_REPORT_GOOD_NEWS_PARAM_HPP__
#define __COW_REPORT_GOOD_NEWS_PARAM_HPP__

#include "common/tlvprotocol.h"
#include "servercommon/serverdef.h"
#include "servercommon/struct/structcommon.h"

static const int MAX_ONCE_COW_REPORT_GOOD_NEWS_COUNT = 1000;				// һ���Բ����������
static const int MAX_COW_REPORT_GOOD_NEWS_REWARD_USER_COUNT = 5;			// ���ý��������

enum COW_REPORT_GOOD_NEWS_STATUS
{
	COW_REPORT_GOOD_NEWS_STATUS_INVALID = 0 ,
	COW_REPORT_GOOD_NEWS_STATUS_BUY = 1,			// ������  (�ɹ���
	COW_REPORT_GOOD_NEWS_STATUS_REWARD = 2,			// ������  (���㽱��
	COW_REPORT_GOOD_NEWS_STATUS_SHOW = 3,			// չʾ��  (������
	COW_REPORT_GOOD_NEWS_STATUS_FINISH = 4,			// ����	   (�������
	COW_REPORT_GOOD_NEWS_STATUS_MAX,

};

struct CowReportGoodNewsBuyInfo								//  �������ҵ���Ϣ
{
	CowReportGoodNewsBuyInfo() {}
	void Reset()
	{
		change_state = structcommon::CS_NONE;

		role_id = 0;
		last_notice_timestamp = 0;
	}

	char change_state;									// ���״̬

	int role_id;										// ���ID
	unsigned int last_notice_timestamp;					// ����ʱ��Ҫ֪ͨ��� , �����Ҳ������������ʱ��֪ͨ
};

struct CowReportGoodNewsRewardUserInfo					//  �������Ϣ
{
	void Reset()
	{
		role_id = 0;
		avatar_type = 0;
		headshot_id = 0;
		memset(role_name, 0, sizeof(role_name));
	}

	int role_id;										// ���ID
	short  avatar_type;									// ��������
	short  headshot_id;									// ͷ��ID
	GameName role_name;									// �������
};

#pragma pack(push, 4)

class CowReportGoodNewsBuyListParam
{
public:
	CowReportGoodNewsBuyListParam()
	{
		id_from = 0;
		count = 0;
		memset(user_list, 0, sizeof(user_list));
	}

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	long long id_from;
	int count;
	CowReportGoodNewsBuyInfo user_list[MAX_ONCE_COW_REPORT_GOOD_NEWS_COUNT];
};

#pragma pack(pop)

#endif	// __COW_REPORT_GOOD_NEWS_PARAM_HPP__

