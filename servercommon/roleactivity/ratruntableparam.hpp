#ifndef __RA_TRUN_TABLE_PARAM_HPP__
#define __RA_TRUN_TABLE_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 2051 ����� - ת�̻  //////////////////////////////////////////

static const int RA_TURN_TABLE_MAX_NUM = 8;

#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

struct RATrunTableParam
{
	RATrunTableParam() { this->Reset(); }

	void Reset()
	{
		ra_turn_table_begin_timestamp = 0;
		ra_has_free_turn_table_num = 0;
		ra_free_turn_table_after_timestamp = 0;
		kill_boss_num = 0;
		turn_table_index = -1;
		before_use_turn_table_type = 0;
	}

	unsigned int ra_turn_table_begin_timestamp;
	int ra_has_free_turn_table_num;							// ���ת�̴���
	unsigned int ra_free_turn_table_after_timestamp;		// ��Ѵ�����ȡ����һ�ε�ʱ��
	int kill_boss_num;										// ��ɱBOSS����
	int turn_table_index;									// ��һ�γ�ȡ�ĵ���index
	int before_use_turn_table_type;							// ��һ��ʹ�õ�ת������
};

UNSTD_STATIC_CHECK(sizeof(RATrunTableParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_TRUN_TABLE_PARAM_HPP__
