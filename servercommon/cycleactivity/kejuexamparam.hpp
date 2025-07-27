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

	short true_question_num;		// �ش���ȷ��Ŀ����
	short answered_question_num;	// �ѻش���Ŀ����
	int praise_times;				// ����������
	int temp_satisfaction;			// �ڵ�ǰ������������
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

	short cur_question_id;			//��ǰ��Ŀid
	char team_answering;			//�����Ƿ����ڴ���flag
	char team_used_card;			//����������Ƿ�ʹ�������⿨
	short cur_satisfaction;			//��ǰ�����
	short is_receive_last_reward;	//�Ƿ�����ȡ���ս���
	unsigned int answer_timeout_timestamp;		//���ⳬʱʱ���

 	char start_task_flag;			//�����Ƿ�ӹ�����
 	char cur_question_answered;		//��ǰ��Ŀ�Ƿ��Ѿ��ش���
 	short used_question_count;		//�ѻش������Ŀ����
 	short used_questions[KE_JU_MAX_SAVE_QUESTION_NUM];	//�ѻش������Ŀid
 
	int answer_uid;		// ��ǰ��Ŀ������uid
	int group_order[2];		// [0]����˳����,[1]����order

// 	unsigned int activity_start_timestamp;
 
 	short special_card;				// ���⿨Ƭ
 	short error_answer;				// �Ŵ��ų��Ĵ�id
//  	short is_praise;				// ��ǰ��Ŀ�Ƿ�Ϊ������ҵ��޹�
 	int word_id;					// ��ǰ��Ŀ�����������id
	KeJuTeamQuestionParam team_info;	// ��ǰ��������Ϣ,�˳����������
};
typedef char KeJuExamParamHex[sizeof(KeJuExamParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(KeJuExamParamHex) < 256);



#pragma  pack(pop)

#endif
