#ifndef __RESEARCH_TASK_DEF_H__
#define __RESEARCH_TASK_DEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"

#pragma pack(push, 4)

struct ResearchTaskInfo
{
	ResearchTaskInfo() { this->Reset(); }

	void Reset()
	{
		task_id = 0;
		progress = 0;
		image_monster_id = 0;
		battle_group_id = 0;
		scene_id = 0;
		posi_x = 0;
		posi_y = 0;
	}

	unsigned short task_id;		// ����ID
	unsigned short progress;	// �������

	int image_monster_id;		// ��ʾ�Ĺ���ID
	int battle_group_id;		// ս���Ĺ�����

	int scene_id;				// ����ID
	int posi_x;					// ������
	int posi_y;					// ������
};

struct ResearchTaskParam
{
	ResearchTaskParam() { this->Reset(); }

	void Reset()
	{
		next_refresh_timestamp = 0;
		today_finish_times = 0;
		today_reward_fetch = 0;
		task_info.Reset();
		residual_reward_back_times = 0;
		m_add_exp_per_notice = 0;
	}

	long long next_refresh_timestamp;	// �´�����ˢ�µ�ʱ���
	int today_finish_times;				// ������������еĴ���
	int today_reward_fetch;				// ��������ȡ���еı��

	ResearchTaskInfo task_info;

	int residual_reward_back_times;		// ʣ��Ľ����һصĴ���
	long long m_add_exp_per_notice;		// �·�һ�־����ȡ��
};
typedef char ResearchTaskParamHex[sizeof(ResearchTaskParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ResearchTaskParamHex) < 128);

#pragma pack(pop)
#endif // !__RESEARCH_TASK_DEF_H__