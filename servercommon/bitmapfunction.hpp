#ifndef _BITMAPFUNCTION_HPP_
#define _BITMAPFUNCTION_HPP_

#include "servercommon.h"

class BitmapFunction
{
public:
	static bool SetBit(unsigned char *bitmap, int byte_count, int which_bit);

	static bool ClearBit(unsigned char *bitmap, int byte_count, int which_bit);

	static bool IsBitSet(unsigned char *bitmap, int byte_count, int which_bit);
};

template<int N>
struct BitMap
{
	static const int MAX_BIT = N;
	static const int TEMP_BYTE_COUNT = MAX_BIT / BIT_COUNT_OF_BYTE + (MAX_BIT % BIT_COUNT_OF_BYTE > 0 ? 1 : 0);
	static const int BYTE_COUNT = TEMP_BYTE_COUNT >= 4 ? TEMP_BYTE_COUNT : 4;

	BitMap() { this->Reset(); }
	~BitMap() {}; // ½ûÖ¹ÉèÎªÐéÎö¹¹

	void Reset()
	{
		memset(byte_list, 0, sizeof(byte_list));
	}

	bool SetBit(int bit_idx)
	{
		return BitmapFunction::SetBit(byte_list, BYTE_COUNT, bit_idx);
	}

	bool UnSetBit(int bit_idx)
	{
		return BitmapFunction::ClearBit(byte_list, BYTE_COUNT, bit_idx);
	}

	bool IsBitSet(int bit_idx)
	{
		return BitmapFunction::IsBitSet(byte_list, BYTE_COUNT, bit_idx);
	}

	unsigned char byte_list[BYTE_COUNT];
};

#endif
