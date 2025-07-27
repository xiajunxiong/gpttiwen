#ifndef __EXCHANGE_DEF_H__
#define __EXCHANGE_DEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#pragma pack(push, 4)

enum SCORE_TYPE
{
	SCORE_TYPE_CHIVALROUS = 0,		// ÏÀÒåÖµ

	SCORE_TYPE_MAX,
};

struct ExchangeLimitData
{
	ExchangeLimitData() :index(0), buy_count(0)
	{}
	unsigned short index;
	unsigned short buy_count;
};

static const int LIMIT_DATA_NUM = 64;
struct ExchangeParam
{
	ExchangeParam() { this->Reset(); }
	void Reset()
	{
		memset(limit_list, 0, sizeof(limit_list));
	}

	void ClearLimitData()
	{
		memset(limit_list, 0, sizeof(limit_list));
	}

	ExchangeLimitData limit_list[LIMIT_DATA_NUM];
	UNSTD_STATIC_CHECK(64 == LIMIT_DATA_NUM);

};

typedef char ExchangeParamHex[sizeof(ExchangeParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ExchangeParamHex) < 1024);
#pragma pack(pop)

#endif // !__SHOP_DEF_H__

