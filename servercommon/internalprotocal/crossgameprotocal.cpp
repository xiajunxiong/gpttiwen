#include "crossgameprotocal.h"
#include "servercommon/msgserialize.hpp"

namespace crossgameprotocal 
{
	bool WriteBegin(int msg_type, char* buffer, int max_len, int* out_len)
	{
		// 初始化
		if (*out_len + 4 >= max_len) return false;

		return WriteInt(msg_type, buffer, max_len, out_len);
	}

	bool ReadBegin(int& msg_type, const char* buffer, int max_len, int* out_len)
	{
		// 初始化
		if (*out_len + 4 >= max_len) return false;

		return ReadInt(msg_type, buffer, max_len, out_len);
	}
}
