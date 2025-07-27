#ifndef __RA_YUAN_QI_JISHIBU_PARAM_HPP__
#define __RA_YUAN_QI_JISHIBU_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// Ԫ�����²� //////////////////////////////////////////
static const int MAX_RA_YUAN_QI_JISHIBU_TASK_COUNT = 10;


enum RA_YUAN_QI_JISHIBU_TASK_TYPE
{
	RA_YUAN_QI_JISHIBU_TASK_TYPE_LOGIN = 1,						// ÿ�յ�½	
	RA_YUAN_QI_JISHIBU_TASK_TYPE_ACTIVE = 2,					// ��Ծ�ﵽ X ��
	RA_YUAN_QI_JISHIBU_TASK_TYPE_VITALITY_HELPS = 3,			// Ԫ������ X ��
	RA_YUAN_QI_JISHIBU_TASK_TYPE_KONGMINDENG = 4,				// ����������X��
	RA_YUAN_QI_JISHIBU_TASK_TYPE_BAOZI_LAILE = 5,				// �������� ͨ��X��
	RA_YUAN_QI_JISHIBU_TASK_TYPE_CHENFENG_POLANG = 6,			// �˷����� �����X��
	RA_YUAN_QI_JISHIBU_TASK_TYPE_BOZHONG_YOULI = 7,				// �������� ���X��
	RA_YUAN_QI_JISHIBU_TASK_TYPE_YUANQI_JINGSU = 8,				// Ԫ������ ͨ��X��
	RA_YUAN_QI_JISHIBU_TASK_TYPE_MIAOBI_SHENGHUA = 9,			// ������� ���X��
	RA_YUAN_QI_JISHIBU_TASK_TYPE_XUNMENG_HUDIE = 10,			// Ѱ�κ��� ���X��

	RA_YUAN_QI_JISHIBU_TASK_TYPE_MAX
};


#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////


struct RAYuanQiJiShiBuParam
{
	RAYuanQiJiShiBuParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		combat = 0;
		is_jinjie_jiangli = 0;
		re_ch = 0;
		now_day_id = 0;

		quanming_reward_fetch_flag = 0;
		jinjie_reward_fetch_flag = 0;

		complete_flag = 0;
		memset(task_list, 0, sizeof(task_list));
	}

	unsigned int ra_begin_timestamp;
	short combat;							// ��õ�����ֵ, �����
	char is_jinjie_jiangli;					// �Ƿ�����׽���
	char re_ch;
	int now_day_id;

	int quanming_reward_fetch_flag;			// ��ѽ���	
	int jinjie_reward_fetch_flag;			// ���׽���
	int complete_flag;					// ���������� ��λ���
	unsigned short task_list[MAX_RA_YUAN_QI_JISHIBU_TASK_COUNT];		// �������
};

UNSTD_STATIC_CHECK(sizeof(RAYuanQiJiShiBuParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)
#endif	//__RA_YUAN_QI_JISHIBU_PARAM_HPP__
