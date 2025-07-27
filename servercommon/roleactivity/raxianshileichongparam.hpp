#ifndef __RA_XIAN_SHI_LEI_CHONG_PARAM_HPP__
#define __RA_XIAN_SHI_LEI_CHONG_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//////////////////////////////////////////// 2169 ���� - ��ʱ�۳�  //////////////////////////////////////////

#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

struct RAXianShiLeiChongParam
{
	RAXianShiLeiChongParam() { this->Reset(); }
	void Reset()
	{
		last_timestamp = 0;
		can_fetch_flag = 0;
		flag = 0;
		total_gold = 0;
	}

	unsigned int last_timestamp;		// ��һ�β�����ʱ���
	int can_fetch_flag;					// ����ȡ�ı��
	int flag;							// ����ȡ���
	int total_gold;						// �ۼƳ�ֵ�Ľ��
};

UNSTD_STATIC_CHECK(sizeof(RAXianShiLeiChongParam) <= sizeof(RoleActivityRoleData))


#pragma pack(pop)


#endif	//__RA_XIAN_SHI_LEI_CHONG_PARAM_HPP__
