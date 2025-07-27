#ifndef __COLLECTION_DEF_HPP__
#define __COLLECTION_DEF_HPP__
#include "servercommon/servercommon.h"
#include "bitmapfunction.hpp"
#pragma pack(push, 4)

static const int COLLECTION_MAX_NUM = 2000;
static const int INT_MAX_BIT_NUM = 32;
static const int COLLECTION_MAX_NUM_INT = COLLECTION_MAX_NUM / INT_MAX_BIT_NUM + 1;

static const int MAX_ATLAS_CARD_NUM = 100;//图鉴卡最大上限

struct CollectionParam
{
	CollectionParam() { this->Reset(); }
	void Reset()
	{
		collection_activate_flag.Reset();
		memset(collection_included_ready_flag, 0, sizeof(collection_included_ready_flag));
		memset(collection_included_activate_flag, 0, sizeof(collection_included_activate_flag));

		reserve_ll = 0;
		version_id = 0;

		memset(new_card_level, 0, sizeof(new_card_level));
	}

	BitMap<COLLECTION_MAX_NUM> collection_activate_flag;

	unsigned int collection_included_ready_flag[COLLECTION_MAX_NUM_INT];
	unsigned int collection_included_activate_flag[COLLECTION_MAX_NUM_INT];

	long long reserve_ll;
	int version_id;			//版本id

	unsigned short new_card_level[MAX_ATLAS_CARD_NUM];

};
UNSTD_STATIC_CHECK(COLLECTION_MAX_NUM <= 2000);//修改需要转移

typedef char CollectionParamHex[sizeof(CollectionParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(CollectionParamHex) < 2048);

#pragma pack(pop)

#endif // !__COLLECTION_DEF_HPP__
