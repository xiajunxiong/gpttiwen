#ifndef __MSG_CHALLENGE_FB_H__
#define __MSG_CHALLENGE_FB_H__

#include "gamedef.h"
#include "servercommon/rolechallengefbdef.hpp"
#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push, 4)

namespace Protocol
{
	class SCChallengeFbRoleInfor
	{
	public:
		SCChallengeFbRoleInfor();			//!< 2240 - ��ս���� - ������Ϣ
		MessageHeader header;

		UInt8 highest_pass_chapter[CHALLENGE_FB_ID_MAX];		// ���ͨ���½�
		UInt16 reserve_ush;
		UInt32 next_refresh_timestamp;
	};

	class SCChallengeFbAwardList
	{
	public:
		SCChallengeFbAwardList();			//!< 2241 - ��ս���� - ��������
		MessageHeader header;

		int list_num;
		PtcItemInfo reward_list[2 * ITEM_REWARD_LIST_COUNT];
	};
}

#pragma pack(pop)
#endif