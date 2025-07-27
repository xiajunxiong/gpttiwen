#ifndef __RA_JIAN_DAN_QIN_XIN_PARAM_HPP__
#define __RA_JIAN_DAN_QIN_XIN_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//////////////////////////////////////////// 2073 ���� - ��������  //////////////////////////////////////////

const static int MAX_RA_JIANDNAQINXIN_DAY_NUM = 7;					//һ�쿪һ���ؿ�,�ܹ�����
const static int MAX_RA_JIANDANQINXIN_CHECKPOINT_NUM = 14;			//�ܹ��ؿ�Ϊ7,һ���ؿ��ֻ��ָߵ��Ѷ�
const static int MAX_RA_JIANDANQINXIN_REWARD_NUM = 5;				//���ĵ��߽�������

enum JIAN_DAN_QIN_XIN_TYPE
{
	JIAN_DAN_QIN_XIN_TYPE_COMMON = 0,		//��ͨ�Ѷ�
	JIAN_DAN_QIN_XIN_TYPE_DIFFICULT = 1,	//�����Ѷ�

	JIAN_DAN_QIN_XIN_TYPE_MAX,
};
UNSTD_STATIC_CHECK(JIAN_DAN_QIN_XIN_TYPE_MAX * MAX_RA_JIANDNAQINXIN_DAY_NUM == MAX_RA_JIANDANQINXIN_CHECKPOINT_NUM)

#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

struct RAJianDanQinXinParam
{
	RAJianDanQinXinParam() { this->Reset(); }
	void Reset()
	{
		ra_jiandanqinxin_begin_timestamp = 0;
		ra_jiandanqinxin_flag.Reset();
	}

	unsigned int ra_jiandanqinxin_begin_timestamp;
	//ǰ�߸�Ϊ�ͼ��Ѷ�,���߸�Ϊ���Ѷ� (�±�Ϊ����seq1)
	BitMap<MAX_RA_JIANDANQINXIN_CHECKPOINT_NUM> ra_jiandanqinxin_flag;				
};

UNSTD_STATIC_CHECK(sizeof(RAJianDanQinXinParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_JIAN_DAN_QIN_XIN_PARAM_HPP__
