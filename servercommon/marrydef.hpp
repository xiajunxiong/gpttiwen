#ifndef __MARRIAGE_DEF_HPP__
#define __MARRIAGE_DEF_HPP__
#include "servercommon/servercommon.h"

#pragma pack(push, 4)

enum MARRIAGE_TASK_TYPE
{
	MARRIAGE_TASK_1 = 1,	//����ƴͼ
	MARRIAGE_TASK_2,		//������Ϭ
	MARRIAGE_TASK_3,		//�����ѹ�

	MARRIAGE_TASK_4,		//����ƴͼ
	MARRIAGE_TASK_5,		//�����ĵ�
};

const static int MARRIAGE_SEEKING_MAX_COUNT = 50;			// ��������������

struct MarriageParam
{
	MarriageParam() { this->Reset(); }
	void Reset()
	{
		lover_uid = 0;
		memset(lover_name, 0, sizeof(lover_name));
		last_marry_time = 0;
		last_divorce_time = 0;
		apply_divorce_time = 0;

		puzzle_play_times = 0;
		question_play_times = 0;

		max_pass_fb_wave = 0;
		curr_wave = 0;
		is_auto_fb = 0;
		reserved_ch = 0;
	}

	int lover_uid;								// ����UID
	GameName lover_name;						// ��������
	unsigned int last_marry_time;					// ���ʱ��
	unsigned int last_divorce_time;					// ���ʱ��
	unsigned int apply_divorce_time;				// �������ʱ��

	short puzzle_play_times;			//����ƴͼ��Ĵ���
	short question_play_times;			//������Ĵ���

	int max_pass_fb_wave;		//�����
	short curr_wave;			//��ǰ����
	char is_auto_fb;				//�Ƿ�ɨ��
	char reserved_ch;
};

typedef char MarryParamHex[sizeof(MarriageParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(MarryParamHex) < 1024);

#pragma pack(pop)

#endif // !__MARRIAGE_DEF_HPP__
