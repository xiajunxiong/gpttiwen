#ifndef __FRIEMD_INTIMACY_PARAM_HPP__
#define __FRIEMD_INTIMACY_PARAM_HPP__

#include "common/tlvprotocol.h"
#include "servercommon/serverdef.h"
#include "servercommon/struct/structcommon.h"

static const int MAX_ONCE_FRIENDS_INTIMACY_COUNT = 1000; // 一次性操作最大好友数量

class FriendIntimacyInfo								//  玩家的好友亲密度信息
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
		
	char change_state;									// 变更状态

	int role_id;										// 玩家ID
	int friend_id;										// 好友UID
	int intimacy_val;									// 亲密度
	int day_frist_to_singchat_timestamp;				// 每日玩家给好友发送的第一条消息的时间戳
	int day_frist_from_singchat_timestamp;				// 每日好友给玩家发送的第一条消息的时间戳
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

