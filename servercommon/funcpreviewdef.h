#ifndef __FUNC_PREVIEW_DEF_H__
#define __FUNC_PREVIEW_DEF_H__

#include "servercommon/servercommon.h"

#pragma pack(push, 4)

static const int MAX_FUNC_PREVIEW_NUM = 40;

enum FUNC_PREVIEW_STATUS
{
	FUNC_PREVIEW_CAN_NOT_CLAIM = 0,		// ������ȡ
	FUNC_PREVIEW_OK_TO_CLAIM = 1,		// ����ȡ
	FUNC_PREVIEW_ALREADY_CLAIMED = 2,	// ����ȡ
};

struct FuncPreviewInfo
{
	FuncPreviewInfo()
	{
		this->Reset();
	}

	void Reset()
	{
		status = FUNC_PREVIEW_CAN_NOT_CLAIM;
		version = 0;
		reserve_sh = 0;
		reserve_int = 0;
	}

	char status;			//!< \see FUNC_PREVIEW_STATUS
	char version;			//!< �汾��
	short reserve_sh;		//!< ������
	int reserve_int;		//!< ������
};

struct FuncPreviewParam
{
	FuncPreviewParam()
	{
		this->Reset();
	}

	void Reset()
	{
		memset(reserve_ll, 0, sizeof(reserve_ll));
		for (int i = 0; i < MAX_FUNC_PREVIEW_NUM; ++i)
		{
			reward_info_list[i].Reset();
		}
	}

	long long reserve_ll[4];
	FuncPreviewInfo reward_info_list[MAX_FUNC_PREVIEW_NUM];
};

typedef char FuncPreviewParamHex[sizeof(FuncPreviewParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(FuncPreviewParamHex) < 1024);

#pragma pack(pop)

#endif