#ifndef __MSG_COURAGE_CHALLENGE_H__
#define __MSG_COURAGE_CHALLENGE_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/couragechallengedef.hpp"
#include "servercommon/struct/global/couragechallengerankdef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{

	class CSCourageChallengeOp						//!<2140 勇气挑战请求
	{
	public:
		CSCourageChallengeOp();
		MessageHeader		header;

		enum OP_TYPE
		{
			OP_TYPE_INFO,				//!< 信息
			OP_TYPE_FETCH,				//!< 领取评分奖励 p1:layer_level p2:level_seq
			OP_TYPE_FETCH_MAX_REWARD,	//!< 领取满星大奖 p1:layer_level
			OP_TYPE_OPEN_UI,			//!< 打开界面
		};

		int op_type;
		int param1;
		int param2;		
	};

	class SCCourageChallengeAllInfo	: public IMessageSerializer					//!< 2141 勇气挑战所有信息
	{
	public:
		SCCourageChallengeAllInfo();
		MessageHeader		header;
		
		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char * buffer, int buffer_size, int * out_length)
		{
			bool result = true;
			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteInt(count, buffer, buffer_size, out_length);
			for (int i = 0; i < count && i < ARRAY_LENGTH(layer_data_node); ++i)
			{
				LayerLevelNode & data_node = layer_data_node[i];

				result = result && WriteShort(data_node.layer_level, buffer, buffer_size, out_length);
				result = result && WriteChar(data_node.level_seq, buffer, buffer_size, out_length);
				result = result && WriteChar(data_node.flag, buffer, buffer_size, out_length);
			}

			result = result && WriteInt(score_count, buffer, buffer_size, out_length);
			for (int i = 0; i < score_count && i < ARRAY_LENGTH(score_level_node); ++i)
			{
				ScoreLayerLevelNode & score_node = score_level_node[i];
				
				result = result && WriteInt(score_node.layer_level, buffer, buffer_size, out_length);
				//result = result && WriteShort(score_node.star, buffer, buffer_size, out_length);  
				for (int k = 0; k < ARRAY_LENGTH(score_node.flag.byte_list); k++)
				{
					result = result && WriteUChar(score_node.flag.byte_list[k], buffer, buffer_size, out_length);
				}

				UNSTD_STATIC_CHECK(sizeof(score_node.flag) == 8);
			}

			return result;
		}

		int count;
		LayerLevelNode layer_data_node[MAX_COURAGE_CHALLENGE_LEVEL_NUM];					// 玩家锢魔之塔所有层数信息   MAX_COURAGE_CHALLENGE_LEVEL_NUM -- 1800

		int score_count;
		ScoreLayerLevelNode score_level_node[MAX_COURAGE_CHALLENGE_SCORE_NUM];	// 玩家锢魔之塔所有等级段评分信息   MAX_COURAGE_CHALLENGE_SCORE_NUM -- 120

	};

	class SCCourageChallengeFirstKillInfo						//!< 2142 勇气挑战首杀信息
	{
	public:
		SCCourageChallengeFirstKillInfo();
		MessageHeader		header;

		struct RoleItem
		{
			RoleItem()
			{
			}

			GameName role_name;
		};

		int count ;
		RoleItem first_kill_list[COURAGE_CHALLENGE_MAX_LEVEL];
	};

	class SCCourageChallenegeSingleInfo				//!< 2143 勇气挑战 单条信息
	{	
	public:
		SCCourageChallenegeSingleInfo();
		MessageHeader header;

		short layer_level;					// 分层等级段
		char layer_level_seq;				// 该分层等级段的seq
		unsigned char flag;					// 二进制位运算[0,7],0-6 条件完成标识, 7默认是是首杀标识
	};

	class SCCourageChallengeScoreSingleInfo				//!< 2144 勇气挑战 评分信息
	{
	public:
		SCCourageChallengeScoreSingleInfo();
		MessageHeader header;

		int layer_level;					// 分层等级段
		BitMap<COURAGE_CHALLENGE_SCORE_LAYER_LEVEL_MAX_NUM> score_flag;		// 该分层等级段评分领取奖励标识   COURAGE_CHALLENGE_SCORE_LAYER_LEVEL_MAX_NUM -- 64
	};

	class CSCourageChallengeRankListReq					//!< 10060 勇气挑战(锢魔之塔)排行榜请求
	{
	public:
		CSCourageChallengeRankListReq();
		MessageHeader header;

		short rank_type;
		short count;
		int role_id[MAX_COURAGE_CHALLENGE_RANK_NUM];
	};
	UNSTD_STATIC_CHECK(MAX_COURAGE_CHALLENGE_RANK_NUM == 100);

	class SCCourageChallengeRankListInfo : public IMessageSerializer		//!< 10061勇气挑战(锢魔之塔)排行信息下发
	{
	public:
		SCCourageChallengeRankListInfo(); 
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char * buffer, int buffer_size, int * out_length)
		{
			bool result = true;
			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteShort(rank_type, buffer, buffer_size, out_length);
			result = result && WriteShort(count, buffer, buffer_size, out_length);
			for (int i = 0; i < count && i < ARRAY_LENGTH(rank_list); ++i)
			{
				const SynCourageChallengeRankInfo & rank_item = rank_list[i];

				result = result && WriteInt(rank_item.uid, buffer, buffer_size, out_length);
				result = result && WriteStrN(rank_item.user_name, sizeof(rank_item.user_name), buffer, buffer_size, out_length);
				result = result && WriteShort(rank_item.avatar_type, buffer, buffer_size, out_length);
				result = result && WriteShort(rank_item.headshot_id, buffer, buffer_size, out_length);
				result = result && WriteShort(rank_item.prof, buffer, buffer_size, out_length);
				result = result && WriteInt(rank_item.star, buffer, buffer_size, out_length);
				result = result && WriteInt(rank_item.layer_level, buffer, buffer_size, out_length);
			}

			return result;
		}

		short rank_type;				
		short count;	
		SynCourageChallengeRankInfo rank_list[MAX_COURAGE_CHALLENGE_RANK_NUM];
	};

	class SCCourageChallengePetHelperNotice		//!< 2152 通知开启助战
	{
	public:
		SCCourageChallengePetHelperNotice();
		MessageHeader header;

		int pet_helper_num;
	};
}

#pragma pack(pop)

#endif

