#ifndef __MSG_NEW_COURAGE_CHALLENGE_H__
#define __MSG_NEW_COURAGE_CHALLENGE_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/newcouragechallengedef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{

	class CSNewCourageChallengeOp					//!< 5330 ߱��֮������
	{
	public:
		CSNewCourageChallengeOp();
		MessageHeader		header;

		enum OP_TYPE
		{
			OP_TYPE_LEVEL_INFO = 0,			//!< �ؿ��б���Ϣ
			OP_TYPE_REWARD_INFO = 1,		//!< ������Ϣ
			OP_TYPE_FETCH_COMMON = 2,		//!< ��ȡ��ͨ���� param:seq
			OP_TYPE_FETCH_TOKEN = 3,		//!< ��ȡս��� param:seq
			OP_TYPE_FETCH_ALL = 4,			//!< һ����ȡ
		};

		int op_type;
		int param;
	};

	class SCNewCourageChallengeLevelListInfo		//!< 5331 ߱��֮���ؿ��б���Ϣ
	{
	public:
		SCNewCourageChallengeLevelListInfo();
		MessageHeader		header;

		int count;
		NewLayerLevelNode list[MAX_NEW_COURAGE_CHALLENGE_LIST_NUM];
	};

	class SCNewCourageChallengeLevelSignleInfo		//!< 5332 ߱��֮���ؿ�������Ϣ
	{
	public:
		SCNewCourageChallengeLevelSignleInfo();
		MessageHeader header;

		short layer_level;					// �ֲ�ȼ���
		char layer_level_seq;				// �÷ֲ�ȼ��ε�seq
		unsigned char flag;					// ������λ����[0,7],0-3 ������ɱ�ʶ, 7Ĭ��������ɱ��ʶ
	};

	class SCNewCourageChallengeRewardInfo			//!< 5333 ߱��֮��������Ϣ
	{
	public:
		SCNewCourageChallengeRewardInfo();
		MessageHeader header;

		char token_is_buy;					// 1���ѹ���
		char sh_reserve[3];					// ����
		BitMap<MAX_NEW_COURAGE_CHALLENGE_REWARD_NUM> common_reward_flag;
		BitMap<MAX_NEW_COURAGE_CHALLENGE_REWARD_NUM> token_reward_flag;
	};
	UNSTD_STATIC_CHECK(MAX_NEW_COURAGE_CHALLENGE_REWARD_NUM == 512);
}

#pragma pack(pop)

#endif


