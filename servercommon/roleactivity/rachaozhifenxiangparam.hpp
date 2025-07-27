#ifndef __RA_CHAOZHI_FENXIANG_PARAM_HPP__
#define __RA_CHAOZHI_FENXIANG_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// ��ֵ���� //////////////////////////////////////////
enum RA_CHAOZHI_FENXIANG_TASK_TYPE
{
	RA_CHAOZHI_FENXIANG_TASK_TYPE_LOGIN = 0,				// ���������¼1��
	RA_CHAOZHI_FENXIANG_TASK_TYPE_YUNZEHUAJING = 1,			// ���󻭾� ̽��X��
	RA_CHAOZHI_FENXIANG_TASK_TYPE_ZAIZHANYUNZE = 2,			// ��ս���� ��������X��
	RA_CHAOZHI_FENXIANG_TASK_TYPE_YUNZE_23 = 3,				// ��������� ���X��
	RA_CHAOZHI_FENXIANG_TASK_TYPE_YUNZEFANHUALU = 4,		// ���󷱻�¼ ���X����
	RA_CHAOZHI_FENXIANG_TASK_TYPE_MOZUZAINALI = 5,			// ħ�������� ����X��ħ��
	RA_CHAOZHI_FENXIANG_TASK_TYPE_JINGLING_DAZHUABU = 6,	// �����ץ�� ץ������

	RA_CHAOZHI_FENXIANG_TASK_TYPE_MAX
};

#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

struct RAChaoZhiFenXiangParam
{
	RAChaoZhiFenXiangParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		task_fetch_mark = 0;
		zhigou_mark = 0;

		memset(task_comple, 0, sizeof(task_comple));
		re_sh = 0;
	}

	unsigned int ra_begin_timestamp;
	short task_fetch_mark;
	short zhigou_mark;

	short task_comple[RA_CHAOZHI_FENXIANG_TASK_TYPE_MAX];
	short re_sh;
};

UNSTD_STATIC_CHECK(sizeof(RAChaoZhiFenXiangParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)

#endif	//__RA_CHAOZHI_FENXIANG_PARAM_HPP__
