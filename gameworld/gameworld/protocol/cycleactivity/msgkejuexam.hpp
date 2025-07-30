#ifndef __MSG_KE_JU_EXAM_HPP__
#define __MSG_KE_JU_EXAM_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/servercommon.h"
#include "gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/serverdef.h"

#pragma pack(push,4) 

namespace Protocol
{
	class CSKeJuExamCSReq		//!<5135 科举考试 客户端请求
	{
	public:
		CSKeJuExamCSReq();
		MessageHeader header;

		enum OP_TYPE
		{
			OP_TYPE_SEND_INFO = 0,		// 请求基础信息
			OP_TYPE_CREATE_NPC = 1,		// 请求NPC位置
			OP_TYPE_START_ANSWERING = 2,// 开始答题
			OP_TYPE_ANSWER = 3,			// 作答  p1:选择的答案
			OP_TYPE_USED_CARD = 4,		// 使用特殊道具卡
			OP_TYPE_PRAISE = 5,			// 玩家点赞,  p1:被点赞人的uid
			OP_TYPE_LAST_BATTLE = 6,	// 请求战斗	
			OP_TYPE_APPLY_FRIEND = 7,	// 好友申请, p1-被申请人uid
			OP_TYPE_SEEK_HELP = 8,		// 答题者求助
			OP_TYPE_SUGGEST = 9,		// 非答题者建议, p1-建议的答案
		};

		int op_type;
		int param_1;
	};

	class SCKeJuExamNPCInfo		//!<5136 科举考试 NPC及题目生成
	{
	public:
		SCKeJuExamNPCInfo();
		MessageHeader header;

		int npc_seq;
		int question_id;
	};

	class SCKeJuExamTimeOutInfo		//!<5137 科举考试 答题超时时间戳
	{
	public:
		SCKeJuExamTimeOutInfo();
		MessageHeader header;

		int answerer_role_id;
		unsigned int answer_timeout_timestamp;
	};

	class SCKeJuExamAnswerResultInfo		//!<5138 科举考试 答题结果信息
	{
	public:
		SCKeJuExamAnswerResultInfo();
		MessageHeader header;

		int	answer;			// 答案, 0--超时未作答
		int result;			// 是否正确,0-错误
	};

	class SCKeJuExamBaseInfo				//!<5139 科举考试基础信息 登录时下发
	{
	public:
		SCKeJuExamBaseInfo();
		MessageHeader header;

		char start_task_flag;			//当日是否接了任务
		char is_receive_last_reward;	//是否已领取最终奖励
		short used_question_count;		//已答题数量
		int cur_satisfaction;			//当前满意度
		int cur_card;					//卡片
	};


	struct KeJuExamPraise
	{
		KeJuExamPraise() { this->Reset(); }

		void Reset()
		{
			uid = 0;
			cur_card = 0;
			word_id = 0;
		}

		int uid;			// 玩家uid
		int cur_card;		// 当前卡片,0-没有
		int word_id;		// 气泡id
	};

	class SCKeJuExamPraiseInfo		//!<5140 科举考试 点赞和道具信息下发
	{
	public:
		SCKeJuExamPraiseInfo();
		MessageHeader header;

		KeJuExamPraise user_info[MAX_TEAM_MEMBER_NUM];
	};


	class SCKeJuExamNotifyInfo				//!<5141 科举考试 漂字
	{
	public:
		SCKeJuExamNotifyInfo();
		MessageHeader header;

		enum OP_TYPE
		{
			OP_TYPE_PRAISE = 1,	//点赞
			OP_TYPE_FRIEND = 2,	//好友申请
			OP_TYPE_USECARD = 3,//使用道具卡
		};

		int op_type;
		GameName name;
		int param;		// type=3时表示道具卡id
	};

	class SCKeJuExamUseCardInfo			//!<5142 科举考试 使用道具
	{
	public:
		SCKeJuExamUseCardInfo();
		MessageHeader header;

		int card_type;
		int uid;
		int param;		//param, 换题卡-新题目id,排错卡-错误答案id
	};


	struct KeJuExamLastResult
	{
		KeJuExamLastResult()
		{}

		void Reset()
		{
			uid = 0;
			cur_satisfaction = 0;
			true_question_num = 0;
			answered_question_num = 0;
			praise_times = 0;
		}

		int uid;
		int cur_satisfaction;			// 满意度
		short true_question_num;		// 回答正确题目数量 
		short answered_question_num;	// 已回答题目数量
		int praise_times;				// 被点赞数量
	};

	class SCKeJuExamLastResultInfo			//!<5143 科举考试 最终结算
	{
	public:
		SCKeJuExamLastResultInfo();
		MessageHeader header;

		KeJuExamLastResult last_result[MAX_TEAM_MEMBER_NUM];
	};

};


#pragma pack(pop) 

#endif