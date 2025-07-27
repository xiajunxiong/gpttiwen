#ifndef __RA_LINKAGE_GIFT_PARAM_HPP__
#define __RA_LINKAGE_GIFT_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 2075 ���� - ��������  //////////////////////////////////////////

enum LINKAGE_GIFT_FETCH_TYPE
{
	LINKAGE_GIFT_FETCH_TYPE_CAN_NOT_FETCH = 0,		// δ��ɲ�����ȡ
	LINKAGE_GIFT_FETCH_TYPE_CAN_FETCH = 1,			// �Ѵ��δ��ȡ
	LINKAGE_GIFT_FETCH_TYPE_ALREADY_FETCH = 2,		// ����ȡ
};

#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

struct RALinkageGiftParam
{
	RALinkageGiftParam() { this->Reset(); }
	void Reset()
	{
		ra_linkage_gfit_can_receive_timestamp = 0;
		ra_linkage_gift_begin_timestamp = 0;
		ra_linkage_gfit_fetch_flag = LINKAGE_GIFT_FETCH_TYPE_CAN_NOT_FETCH;
		ra_linkage_gift_open_activity_level_seq = 0;
	}

	unsigned int ra_linkage_gfit_can_receive_timestamp;					//����ȡʱ���
	unsigned int ra_linkage_gift_begin_timestamp;
	int ra_linkage_gfit_fetch_flag;										//0 δ��ɲ�����ȡ,1�Ѵ��δ��ȡ,2����ȡ
	int ra_linkage_gift_open_activity_level_seq;						//�����ʱ��ҵĵȼ���Ӧ���õ�seq
};

UNSTD_STATIC_CHECK(sizeof(RALinkageGiftParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_LINKAGE_GIFT_PARAM_HPP__
