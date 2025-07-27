#ifndef __TIGER_BRINGS_BLESSING_PARAM_HPP__
#define __TIGER_BRINGS_BLESSING_PARAM_HPP__

#include "common/tlvprotocol.h"
#include "servercommon/serverdef.h"
#include "servercommon/struct/structcommon.h"

static const int MAX_ONCE_TIGER_BRINGS_BLESSING_COUNT = 1000;				// һ���Բ����������
static const int MAX_TIGER_BRINGS_BLESSING_REWARD_USER_COUNT = 5;			// ���ý��������

enum TIGER_BRINGS_BLESSING_STATUS
{
	TIGER_BRINGS_BLESSING_STATUS_INVALID = 0,
	TIGER_BRINGS_BLESSING_STATUS_BUY = 1,			// ������  (�ɹ���
	TIGER_BRINGS_BLESSING_STATUS_REWARD = 2,			// ������  (���㽱��
	TIGER_BRINGS_BLESSING_STATUS_SHOW = 3,			// չʾ��  (������
	TIGER_BRINGS_BLESSING_STATUS_FINISH = 4,			// ����	   (�������
	TIGER_BRINGS_BLESSING_STATUS_MAX,

};

struct TigerBringsBlessingBuyInfo								//  �������ҵ���Ϣ
{
	TigerBringsBlessingBuyInfo() {}
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

struct TigerBringsBlessingRewardUserInfo					//  �������Ϣ
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

class TigerBringsBlessingBuyListParam
{
public:
	TigerBringsBlessingBuyListParam()
	{
		id_from = 0;
		count = 0;
		memset(user_list, 0, sizeof(user_list));
	}

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	long long id_from;
	int count;
	TigerBringsBlessingBuyInfo user_list[MAX_ONCE_TIGER_BRINGS_BLESSING_COUNT];
};

#pragma pack(pop)

#endif	