#ifndef __RA_QUAN_MIN_DISCOUNT_PARAM_HPP__
#define __RA_QUAN_MIN_DISCOUNT_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//////////////////////////////////////////// 2078 开服活动 - 全民团购  //////////////////////////////////////////

static const int QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM = 32;			//商品最大数量
static const int QUAN_MIN_DISCOUNT_DISCOUNT_MAX_NUM = 200;			//最多两百人购买就达到折扣上限
static const int QUAN_MIN_DISCOUNT_PARAM_MAX_COUNT = 200;

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////

class RAQuanMinDiscountSeverParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	struct RAQuanMinDiscountBuyCommondity
	{
		RAQuanMinDiscountBuyCommondity() { this->Reset(); }

		void Reset()
		{
			change_state = 0;
			seq = 0;
			next_add_timestamp = 0;
			buy_count = 0;
			memset(buy_uid, 0, sizeof(buy_uid));
		}

		char change_state;
		int seq;

		unsigned int next_add_timestamp;
		int buy_count;
		int	buy_uid[QUAN_MIN_DISCOUNT_DISCOUNT_MAX_NUM];
	};

	int count;
	long long max_index;
	RAQuanMinDiscountBuyCommondity data_list[QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM];
};

class QuanMinDiscountParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	struct RoleData
	{
		RoleData() { this->Reset(); }

		void Reset()
		{
			change_state = 0;
			uid = 0;

			buy_flag = 0;
			memset(buy_count, 0, sizeof(buy_count));
		}
		char change_state;
		int uid;

		int buy_flag;			//32位 位标记(弃用)
		char buy_count[QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM];
	};

	int count;
	long long next_id;
	RoleData data_list[QUAN_MIN_DISCOUNT_PARAM_MAX_COUNT];
};


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RAQuanMinDiscountParam
{
	RAQuanMinDiscountParam() { this->Reset(); }
	void Reset()
	{
		ra_quan_min_discount_begin_timestamp = 0;
		ra_quan_min_discount_buy_flag.Reset();
		for (int i = 0; i < ARRAY_ITEM_COUNT(ra_quan_min_discount_buy_discount); ++i)
		{
			ra_quan_min_discount_buy_discount[i] = 10;
		}
		memset(ra_quan_min_discount_buy_count, 0, sizeof(ra_quan_min_discount_buy_count));
	}

	unsigned int ra_quan_min_discount_begin_timestamp;
	BitMap<12> ra_quan_min_discount_buy_flag;			//商品购买标识(弃用)
	char ra_quan_min_discount_buy_discount[12];			//购买商品时的折扣(弃用)
	unsigned char ra_quan_min_discount_buy_count[QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM];	//购买次数
};

UNSTD_STATIC_CHECK(sizeof(RAQuanMinDiscountParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_QUAN_MIN_DISCOUNT_PARAM_HPP__
