#ifndef __YAO_XING_SHI_LIAN_DEF_HPP__
#define __YAO_XING_SHI_LIAN_DEF_HPP__

#include <string.h>
#include "servercommon/servercommon.h"
#include "servercommon/bitmapfunction.hpp"

static const int MAX_YAOXINGSHILIAN_REWARD_LIST = 512;						// 成就奖励最大list
static const int MAX_YAOXINGSHILIAN_REWARD2_LIST = 512;						// 成就奖励2最大list

#pragma pack(push, 4)

struct YaoXingShiLianParam
{
	YaoXingShiLianParam() { this->Reset(); }

	void Reset()
	{
		pass_battle_seq = 0;
		m_reward_bit_map.Reset();
		m_reward_bit_map2.Reset();
		m_cmd_map.Reset();
	}

	int pass_battle_seq;
	BitMap<MAX_YAOXINGSHILIAN_REWARD_LIST> m_reward_bit_map;
	BitMap<MAX_YAOXINGSHILIAN_REWARD2_LIST> m_reward_bit_map2;
	BitMap<32> m_cmd_map;
};

typedef char YaoXingShiLianParamHex[sizeof(YaoXingShiLianParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(YaoXingShiLianParamHex) < 512);

#pragma pack(pop)

#endif  // __MENTALITY_DEF_HPP__

