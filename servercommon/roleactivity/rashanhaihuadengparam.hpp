#ifndef __RA_SHANHAI_HUADENG_PARAM_HPP__
#define __RA_SHANHAI_HUADENG_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/bitmapfunction.hpp"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)

static const int MAX_RA_SHANHAI_HUADENG_TASK_NUM = 32;				//ɽ��������������
static const int MAX_RA_SHANHAI_HUADENG_LIGHT_TYPE_NUM = 10;		//ɽ����������

//ɽ������
struct RAShanHaiHuaDengParam
{
	RAShanHaiHuaDengParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		last_start_day_id = 0;
		game_end_timestamp = 0;
		single_score_count = 0;
		last_game_score_count = 0;
		day_single_score_count = 0;
		activity_single_score_count = 0;
		day_join_times = 0;
		activity_join_times = 0;
		day_score_count = 0;
		reserve_sh = 0;
		activity_score_count = 0;
		m_task_list_flag.Reset();
	}

	unsigned int ra_begin_timestamp;
	unsigned int last_start_day_id;									//��ʼ��Ϸʱ������ID
	unsigned int game_end_timestamp;								//��Ϸ����ʱ���
	short single_score_count;										//������Ϸ����
	short last_game_score_count;									//��һ����Ϸ��õĻ���
	short day_single_score_count;									//���쵥�λ������ֵ
	short activity_single_score_count;								//��ڵ��λ������ֵ
	short day_join_times;											//���ղ������
	short activity_join_times;										//��ڲ������
	short day_score_count;											//���ջ�õ��ܷ�
	short reserve_sh;
	int activity_score_count;										//��ڻ�õ��ܻ���
	BitMap<MAX_RA_SHANHAI_HUADENG_TASK_NUM> m_task_list_flag;		//�����б��ʶ 0:δ��� 1:�����
};
UNSTD_STATIC_CHECK(sizeof(RAShanHaiHuaDengParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)

#endif