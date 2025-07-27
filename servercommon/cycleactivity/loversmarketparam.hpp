#ifndef __LOVERS_MARKET_PARAM_HPP__
#define __LOVERS_MARKET_PARAM_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"

#pragma pack(push, 4)

static const int MAX_LOVERS_MARKET_STORE_COUNT = 20;				// ��Ե�̵���Ʒ��������
static const int MAX_LOVERS_MARKET_LIMIT_COUNT = 200;			// ÿ�ֵ��ߵ��޹��������ܳ���256

struct LoversMarketParam
{
	LoversMarketParam()
	{
		this->Reset();
	}

	void Reset()
	{
		memset(store_list, 0, sizeof(store_list));
	}

	unsigned char store_list[MAX_LOVERS_MARKET_STORE_COUNT];
};

typedef char LoversMarketParamHex[sizeof(LoversMarketParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(LoversMarketParamHex) < 64);

#pragma pack(pop)
#endif
