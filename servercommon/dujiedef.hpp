#ifndef __DU_JIE_DEF__
#define __DU_JIE_DEF__

#include "servercommon/servercommon.h"

////// �ɽ�

//���ؿ���
static const int DU_JIE_MAX_FB_NUM = 9;

//ǰ���������� (���žͿ�ʼ��¼)
enum DuJieTaskType
{
	DU_JIE_TASK_TYPE_DREAM_NOTES_CHALLENGE = 0, //ͨ����ԨX��	p_1:����
	DU_JIE_TASK_TYPE_COURAGE_CHALLENGE = 1,		//ͨ��X����ħ֮��	p_1:�ȼ���
	DU_JIE_TASK_TYPE_MONSTER_GROUP = 2,			//����սʤ������X	p_1:������
	DU_JIE_TASK_TYPE_LUN_HUI_WANG_CHUAN = 3,	//ͨ���ֻ�����X��	p_1:�ؿ�seq
	DU_JIE_TASK_TYPE_SHAN_HAI_BOSS = 4,			//����P1��սʤ P2�Ѷ�ɽ������	p_1:����  p_2:�Ѷ�(д��ֻ��¼һ���Ѷ�)
	DU_JIE_TASK_TYPE_PRESTIGE = 5,				//����P1����ׯ�����ﵽP2����	p_1:����  p_2:�����ȼ�
	DU_JIE_TASK_TYPE_TOTAL_CAPABILITY = 6,		//�ۺ�ս���ﵽX		p_1:ս��
	DU_JIE_TASK_TYPE_GET_CHIVALROUS = 7,		// �ۼƻ������ֵ param1:����
	DU_JIE_TASK_TYPE_COURSE_LEVEL = 8,			// �ɾ�-���̵ȼ� param1:�ȼ�
	DU_JIE_TASK_TYPE_COUESE_OF_GROWTH = 9,		// �ɾ�-Ŀ��׶� param1:�׶� param2:�Ǽ�
	DU_JIE_TASK_TYPE_ROLE_CAPABILITY = 10,		// ��ɫս�� param1:ս����ֵ

	DU_JIE_TASK_TYPE_MAX,
};

//�������
enum DuJieTaskParam
{
	DU_JIE_TASK_PARAM_SHAN_HAI_BOSS_DAYS,		//P2�Ѷ�ɽ����������ͨ������
	DU_JIE_TASK_PARAM_SHAN_HAI_BOSS_TODAY_PASS,	//P2�Ѷ�ɽ�����ֽ���ͨ��ͨ�ر��
	DU_JIE_TASK_PARAM_SHAN_HAI_BOSS_MAX_DAYS,	//P2�Ѷ�ɽ�������������ͨ������
	DU_JIE_TASK_PARAM_LUN_HUI_WANG_CHUAN,		//�ֻ�������ʷ��߹ؿ�

	DU_JIE_TASK_PARAM_MAX,
};

#pragma pack(push, 4)
struct DuJieParam
{
	DuJieParam()
	{
		this->Reset();
	}
	void Reset()
	{
		hold_ll = 0;
		max_pass_fb_seq = -1;
		next_front_gate_pass = 0;
		memset(task_param_list_, 0, sizeof(task_param_list_));
		memset(hold_list_, 0, sizeof(hold_list_));
		memset(task_param_list_, 0, sizeof(task_param_list_));

	}

	long long hold_ll;
	int max_pass_fb_seq;			//��߹ؿ�
	int next_front_gate_pass;	//ǰ�ùؿ�ͨ��(��߹ؿ���һ�ص�)
	
	int task_param_list_[DU_JIE_TASK_PARAM_MAX];
	int hold_list_[10];
	//������ �ĳɸ��ؿ�����  �õ�0��
	int day_assist_times_[DU_JIE_MAX_FB_NUM];	//���նɽ���ս����[FB_index]
};

typedef char DuJieParamHex[sizeof(DuJieParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(DuJieParamHex) < 256);


#pragma pack(pop)

#endif