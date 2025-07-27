#ifndef __KE_JU_EXAM_PARAM_HPP__ 
#define __KE_JU_EXAM_PARAM_HPP__

#include "servercommon/servercommon.h"

static const int KE_JU_MAX_SAVE_QUESTION_NUM = 10;

#pragma pack(push, 4)

struct KeJuTeamQuestionParam
{
	KeJuTeamQuestionParam() { this->Reset(); }

	void Reset()
	{
		true_question_num = 0;
		answered_question_num = 0;
		praise_times = 0;
		temp_satisfaction = 0;
	}

	short true_question_num;		// 回答正确题目数量
	short answered_question_num;	// 已回答题目数量
	int praise_times;				// 被点赞数量
	int temp_satisfaction;			// 在当前队伍里的满意度
};

struct KeJuExamParam
{
	KeJuExamParam() { this->Reset(); }

	void Reset()
	{
		cur_question_id = 0;
		team_answering = 0;
		team_used_card = 0;
		cur_satisfaction = 0;
		is_receive_last_reward = 0;
		answer_timeout_timestamp = 0;
		start_task_flag = 0;
		used_question_count = 0;
		cur_question_answered = 0;
//		activity_start_timestamp = 0;
		special_card = 0;
		error_answer = 0;
//		is_praise = 0;
		word_id = 0;
		answer_uid = 0;
		memset(group_order, 0, sizeof(group_order));
		memset(used_questions, 0, sizeof(used_questions));

		team_info.Reset();
	}

	void DayReset()
	{
		cur_question_id = 0;
		cur_satisfaction = 0;
		is_receive_last_reward = 0;
		used_question_count = 0;
		start_task_flag = 0;
		special_card = 0;

		memset(used_questions, 0, sizeof(used_questions));
	}

	void TeamReset()
	{
		team_info.Reset();
	}

	short cur_question_id;			//当前题目id
	char team_answering;			//队伍是否正在答题flag
	char team_used_card;			//本题队伍中是否使用了特殊卡
	short cur_satisfaction;			//当前满意度
	short is_receive_last_reward;	//是否已领取最终奖励
	unsigned int answer_timeout_timestamp;		//答题超时时间戳

 	char start_task_flag;			//今日是否接过任务
 	char cur_question_answered;		//当前题目是否已经回答了
 	short used_question_count;		//已回答过的题目数量
 	short used_questions[KE_JU_MAX_SAVE_QUESTION_NUM];	//已回答过的题目id
 
	int answer_uid;		// 当前题目答题人uid
	int group_order[2];		// [0]答题顺序组,[1]答题order

// 	unsigned int activity_start_timestamp;
 
 	short special_card;				// 特殊卡片
 	short error_answer;				// 排错卡排除的答案id
//  	short is_praise;				// 当前题目是否为其他玩家点赞过
 	int word_id;					// 当前题目玩家最后的气泡id
	KeJuTeamQuestionParam team_info;	// 当前队伍内信息,退出队伍后重置
};
typedef char KeJuExamParamHex[sizeof(KeJuExamParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(KeJuExamParamHex) < 256);



#pragma  pack(pop)

#endif
