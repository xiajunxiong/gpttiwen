#ifndef __RA_ZHAOCAIJINBAO_PARAM_HPP__
#define __RA_ZHAOCAIJINBAO_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// �вƽ��� //////////////////////////////////////////
static const int MAX_RA_ZHAOCAIJINBAO_BUY_NUM = 10;							// ������������
static const int MAX_RA_ZHAOCAIJINBAO_CHONGZHI_NUM = 15;					// ��ֵ������������
static const int MAX_RA_ZHAOCAIJINBAO_REFRESH_NUM = 10;						// ˢ�½��ص�����
static const int MAX_RA_ZHAOCAIJINBAO_RECORD_NUM = 30;						// �齱��¼����

struct RAZhaoCaiJinBaoRecordInfo			// �齱��¼��Ϣ
{
	RAZhaoCaiJinBaoRecordInfo(): times(0), item_id(0), num(0) {}

	unsigned int times;
	int item_id;			//��ƷID
	int num;
};


#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////



////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////
					
struct RAZhaoCaiJinBaoParam
{
	RAZhaoCaiJinBaoParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		next_refresh_timestamp = 0;

		history_chongzhi = 0;
		day_chongzhi = 0;
		memset(chongzhi_time, 0, sizeof(chongzhi_time));

		memset(pool, 0, sizeof(pool));
		memset(buy_list, 0, sizeof(buy_list));
		memset(chongzhi_list, 0, sizeof(chongzhi_list));

		memset(record_list, 0, sizeof(record_list));

		ArrayCallReset(bao_di_info_list);
	}

	unsigned int ra_begin_timestamp;
	unsigned int next_refresh_timestamp;						// ��һ��ˢ�½���ʱ���

	int history_chongzhi;										// ��ۼƳ�ֵ���
	int day_chongzhi;											// �ÿ���ۻ���ֵ���
	short chongzhi_time[MAX_RA_ZHAOCAIJINBAO_CHONGZHI_NUM];		// ���ʳ�ֵ�������

	short pool[MAX_RA_ZHAOCAIJINBAO_REFRESH_NUM];				// ���� seq
	short buy_list[MAX_RA_ZHAOCAIJINBAO_BUY_NUM];				// �������
	short chongzhi_list[MAX_RA_ZHAOCAIJINBAO_CHONGZHI_NUM];		// ��ֵ��������

	RAZhaoCaiJinBaoRecordInfo record_list[MAX_RA_ZHAOCAIJINBAO_RECORD_NUM];
	BaoDiInfoItem bao_di_info_list[BAO_DI_INFO_ITEM_MAX_NUM];
};

UNSTD_STATIC_CHECK(sizeof(RAZhaoCaiJinBaoParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)
#endif	//__RA_ZHAOCAIJINBAO_PARAM_HPP__
