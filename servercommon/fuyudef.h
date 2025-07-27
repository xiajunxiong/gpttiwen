#ifndef __FU_YU_DEF__
#define __FU_YU_DEF__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"

#pragma pack(push, 4)

static const int WEAR_FU_YU_WEAR_NUM = 3;		// ����������װ��������
static const int FU_YU_STRENGTHEN_TOTAL_RATE = 10000;
static const int MAX_FU_YU_RANK_NUM = 15;		// ����������

static const int HUN_YUAN_FU_YU_EXTRA_SP_LEVEL = 10;	// ��Ԫ���������Ч�ȼ�
static const int HUA_SHEN_FU_YU_EXTRA_SP_LEVEL = 15;	// ������������Ч�ȼ�

enum FU_YU_SP_LIMIT_TYPE
{
	FU_YU_SP_NON_LIMIT = 0,			// ������, param1-��Ч
	FU_YU_SP_PROF_LIMIT = 1,		// ְҵ����, param1-ְҵ����100-800
	FU_YU_SP_PARNTER_LIMIT = 2,		// ����޶�, param1-���id

	FU_YU_SP_LIMIT_TYPE_MAX,
};

struct FuYuItemInfo
{
	FuYuItemInfo()
	{
		partner_id = 0;
		wear_idx = -1;
		item_id = 0;
		num = 0;
		is_bind = 0;
		has_param = 0;
		invalid_time = 0;
		item_unique_id = 0LL;
		param_length = 0;
		memset(param_data, 0, sizeof(param_data));
	}

	int partner_id;				//!< ���id
	int wear_idx;				//!< ����index(0-2)
	ItemID item_id;				//!< ��Ʒid
	short num;					//!< ����
	char is_bind;				//!< �Ƿ��
	char has_param;				//!< �Ƿ��о�ֵ����
	UInt32 invalid_time;		//!< ����ʱ��
	long long item_unique_id;	//!< ����ΨһID
	short param_length;			//!< ��ֵ���������ݳ��� ����has_paramΪ1ʱ�ɶ���
	NetValueItemParamData param_data;	//!< ��ֵ���������ݣ�����param_length��ֵ����ȡ ����has_paramΪ1ʱ�ɶ���
};

struct FuYuCommonParam
{
	FuYuCommonParam()
	{
		this->Reset();
	}

	void Reset()
	{
		reserve_sh = 0;
	}

	long long reserve_sh;		//������
};

typedef char FuYuCommonParamHex[sizeof(FuYuCommonParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(FuYuCommonParamHex) < 128);


#pragma pack(pop)
#endif 
