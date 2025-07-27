#ifndef __CYCLE_ACTIVITY_LANTERN_RIDDLE_DEF__
#define __CYCLE_ACTIVITY_LANTERN_RIDDLE_DEF__

#include <string.h>
#include "servercommon/servercommon.h"

static const int MAX_SAVE_QUESTION_NUM = 10;



#pragma pack(push, 4)

struct TeamQuestionParam
{
	TeamQuestionParam() { this->Reset(); }

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

struct SALanternRiddieParam
{
	SALanternRiddieParam() { this->Reset(); }

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
		activity_start_timestamp = 0;
		special_card = 0;
		error_answer = 0;
		is_praise = 0;
		word_id = 0;
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
	
	short cur_question_id;			//当前题目id
	char team_answering;			//队伍答题中
	char team_used_card;			//本题队伍中是否使用了特殊卡
	short cur_satisfaction;			//当前满意度
	short is_receive_last_reward;	//是否已领取最终奖励
	unsigned int answer_timeout_timestamp;		//答题超时时间戳

	char start_task_flag;			//今日是否接过任务
	char cur_question_answered;		//该题已作答
	short used_question_count;		//已回答过的题目数量
	short used_questions[MAX_SAVE_QUESTION_NUM];	//已回答过的题目id

	unsigned int activity_start_timestamp;

	short special_card;				// 特殊卡片
	short error_answer;				// 排错卡排除的答案id
	short is_praise;				// 当前题目是否为其他玩家点赞过
	short word_id;					// 当前题目点赞气泡id
	TeamQuestionParam team_info;	// 当前队伍内信息,退出队伍后重置
};
typedef char SALanternRiddieParamHex[sizeof(SALanternRiddieParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(SALanternRiddieParamHex) < 128);





#pragma  pack(pop)

#endif