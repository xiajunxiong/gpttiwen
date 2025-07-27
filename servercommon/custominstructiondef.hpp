#ifndef __CUSTOM_INSTRUCTION_DEF_HPP__
#define __CUSTOM_INSTRUCTION_DEF_HPP__

#include "servercommon/servercommon.h"

static const int CUSTOM_INSTRUCTION_NUM = 8;

#pragma pack(push)
#pragma pack(4)

struct InstructionInfo
{
	InstructionInfo()
	{
		this->Reset();
	}

	void Reset()
	{
		memset(reserve_ll, 0, sizeof(reserve_ll));
		memset(istr, 0, sizeof(istr));
	}

	long long reserve_ll[4];
	InstructionStr istr;
};

struct CustomInstructionParam
{
	CustomInstructionParam()
	{
		this->Reset();
	}

	void Reset()
	{
		memset(reserve_ll, 0, sizeof(reserve_ll));
		reserve_sh = 0;
		instruction_num = 0;
		for (int i = 0; i < CUSTOM_INSTRUCTION_NUM; ++i)
		{
			instruction_list[i].Reset();
		}
	}

	long long reserve_ll[4];
	short reserve_sh;
	short instruction_num;
	InstructionInfo instruction_list[CUSTOM_INSTRUCTION_NUM];
};

typedef char CustomInstructionParamHex[sizeof(CustomInstructionParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(CustomInstructionParamHex) < 2048);

#pragma pack(pop)

#endif