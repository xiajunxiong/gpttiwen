#ifndef __FRIEMD_INTIMACY_PARAM_HPP__
#define __FRIEMD_INTIMACY_PARAM_HPP__

#include "common/tlvprotocol.h"
#include "servercommon/serverdef.h"
#include "servercommon/struct/structcommon.h"

static const int MAX_ONCE_FRIENDS_INTIMACY_COUNT = 1000; // һ���Բ�������������

class FriendIntimacyInfo								//  ��ҵĺ������ܶ���Ϣ
{
public:
	FriendIntimacyInfo() {}
	void Reset()
	{
		change_state = structcommon::CS_NONE;

		role_id = 0;
		friend_id = 0;
		intimacy_val = 0;
		day_frist_to_singchat_timestamp = 0;
		day_frist_from_singchat_timestamp = 0;
	}
		
	char change_state;									// ���״̬

	int role_id;										// ���ID
	int friend_id;										// ����UID
	int intimacy_val;									// ���ܶ�
	int day_frist_to_singchat_timestamp;				// ÿ����Ҹ����ѷ��͵ĵ�һ����Ϣ��ʱ���
	int day_frist_from_singchat_timestamp;				// ÿ�պ��Ѹ���ҷ��͵ĵ�һ����Ϣ��ʱ���
};

class FriendIntimacyListParam
{
public:
	FriendIntimacyListParam()
	{
		id_from = 0;
		count = 0;
		memset(intimacy_info_arr, 0, sizeof(intimacy_info_arr));
	}

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	long long id_from;
	int count;
	FriendIntimacyInfo intimacy_info_arr[MAX_ONCE_FRIENDS_INTIMACY_COUNT];
};

#endif	// __FRIEMD_INTIMACY_PARAM_HPP__

