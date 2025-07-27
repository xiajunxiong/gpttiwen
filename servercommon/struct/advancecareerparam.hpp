#ifndef __ADVANCE_CAREER_PARAM_HPP__
#define __ADVANCE_CAREER_PARAM_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#pragma pack(push, 4)

static const int MAX_ADVANCE_CAREER_SUB_COUNT = 9;	// ְҵ�����ӽ���

/////////////  ����

struct AdvanceCareerParam								//ְҵ����
{
	AdvanceCareerParam()
	{
		this->Reset();
	}

	void Reset()
	{
		is_history_data = 0;
		sub_level = 0;
		memset(cond_complete_flag, 0, sizeof(cond_complete_flag));
	}

	int is_history_data;												// �Ƿ����ʷ��������ͳ��
	int sub_level;														// ��ǰ�ӽ׼�
	int cond_complete_flag[PROFESSION_ADVANCE_TIMES + 1];				// ÿ�׵������������ɵļ�¼[��λ]

};

typedef char AdvanceCareerParamHex[sizeof(AdvanceCareerParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(AdvanceCareerParamHex) < 128);


#pragma pack(pop)

#endif	// __ADVANCE_CAREER_PARAM_HPP__
