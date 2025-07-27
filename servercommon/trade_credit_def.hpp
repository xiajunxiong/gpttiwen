#ifndef __TRADE_CREDIT_DEF_HPP__
#define __TRADE_CREDIT_DEF_HPP__

#include "servercommon/servercommon.h"

#pragma pack(push)
#pragma pack(4)

enum CREDIT_TYPE
{
	CREDIT_TYPE_INVALID = 0,
	CREDIT_TYPE_LEVEL = 1,				// 等级
	CREDIT_TYPE_CHONGZHI = 2,			// 充值
	CREDIT_TYPE_ACTIVE = 3,				// 活跃度
	CREDIT_TYPE_FIELD_BOSS = 4,			// 野外BOSS
	CREDIT_TYPE_TRADEMARKET_BUY = 5,	// 交易行购买
	CREDIT_TYPE_MAX,
};

static const int MAX_CREDIT_TYPE_NUM = 16;
UNSTD_STATIC_CHECK(CREDIT_TYPE_MAX <= MAX_CREDIT_TYPE_NUM);

struct TradeCreditParam
{
	TradeCreditParam()
	{
		this->Reset();
	}

	void Reset()
	{
		total_credit_num = 0LL;
		memset(today_credit_num, 0, sizeof(today_credit_num));
		memset(reserve, 0, sizeof(reserve));
	}

	long long reserve[4];
	long long total_credit_num;
	int today_credit_num[MAX_CREDIT_TYPE_NUM];
};

typedef char TradeCreditParamHex[sizeof(TradeCreditParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(TradeCreditParamHex) < 256);

#pragma pack(pop)

#endif