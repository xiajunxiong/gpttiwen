#ifndef __MSG_LANTERN_RIDDIE_HPP__
#define __MSG_LANTERN_RIDDIE_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/servercommon.h"
#include "gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/serverdef.h"

#pragma pack(push,4) 

namespace Protocol
{

	class LanternRiddieCSReq		//!<4670 山海灯会 客户端请求
	{
	public:
		LanternRiddieCSReq();
		MessageHeader header;

		int op_type;
		int param_1;
	};


	class SALanternRiddieNPCInfo		//!<4671 山海灯会 NPC及题目生成
	{
	public:
		SALanternRiddieNPCInfo();
		MessageHeader header;

		int npc_seq;
		int question_id;
	};

	class SALanternRiddieTimeOutInfo		//!<4672 山海灯会 答题超时时间戳
	{
	public:
		SALanternRiddieTimeOutInfo();
		MessageHeader header;

		unsigned int answer_timeout_timestamp;
	};

	class SALanternRiddieAnswerResultInfo		//!<4673 山海灯会 答题结果信息
	{
	public:
		SALanternRiddieAnswerResultInfo();
		MessageHeader header;

		int answerer_id;					// 答题者id,   0--超时无人作答		 ///由于某些不可抗力的原因,我们修改了这两行		answerer_id = 答题者答案
		int	answerer_answer;				// 答题者答案, 0--超时无人作答		 ///代码的含义,他们的效果实际上是相反的,现在	answerer_answer = 答题者id
		int cur_satisfaction;				// 当前满意度
		int result;		//是否正确,0-错误
	};

	class SALanternRiddieBaseInfo				//!<4674 山海灯会 登录时下发
	{
	public:
		SALanternRiddieBaseInfo();
		MessageHeader header;

		char start_task_flag;			//当日是否接了任务
		char is_receive_last_reward;	//是否已领取最终奖励
		short used_question_count;		//已答题数量
		int cur_satisfaction;			//当前满意度
		int cur_card;					//卡片
	};

	struct LanternRiddiePraise
	{
		LanternRiddiePraise() { this->Reset(); }

		void Reset()
		{
			uid = 0;
			is_praise = 0;
			cur_card = 0;
			word_id = 0;
		}

		int uid;			// 玩家uid
		int is_praise;		// 是否点过赞
		int cur_card;		// 当前卡片,0-没有
		int word_id;		// 气泡id
	};

	class SCLanternRiddiePraiseInfo		//!<4675 山海灯会 点赞和道具信息下发
	{
	public:
		SCLanternRiddiePraiseInfo();
		MessageHeader header;

		LanternRiddiePraise user_info[MAX_TEAM_MEMBER_NUM];
	};

	class SCLanternRiddieNotifyInfo				//!<4676 山海灯会 漂字
	{
	public:
		SCLanternRiddieNotifyInfo();
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

	class SCLanternRiddieUseCardInfo			//!<4677 山海灯会 使用道具
	{
	public:
		SCLanternRiddieUseCardInfo();
		MessageHeader header;

		int card_type;
		int uid;
		int param;		//param, 换题卡-新题目id,排错卡-错误答案id
	};

	struct LanternRiddieLastResult
	{
		LanternRiddieLastResult()
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

	class SCLanternRiddieLastResultInfo			//!<4678 山海灯会 最终结算
	{
	public:
		SCLanternRiddieLastResultInfo();
		MessageHeader header;

		LanternRiddieLastResult last_result[MAX_TEAM_MEMBER_NUM];
	};

}

#pragma pack(pop)

#endif
