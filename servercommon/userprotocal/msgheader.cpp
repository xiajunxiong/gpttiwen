#include "msgheader.h"
#include "servercommon/msgserialize.hpp"

namespace Protocol
{
	bool WriteBegin(unsigned short msg_type, char* buffer, int max_len, int* out_len)
	{
		// ³õÊ¼»¯
		if (*out_len + 4 >= max_len) return false;

		return WriteUShort(msg_type, buffer, max_len, out_len) && WriteShort(0, buffer, max_len, out_len);
	}

	bool ReadBegin(unsigned short& msg_type, const char* buffer, int max_len, int* out_len)
	{
		if (*out_len + 4 >= max_len) return false;

		short tmp;
		return ReadUShort(msg_type, buffer, max_len, out_len) && ReadShort(tmp, buffer, max_len, out_len);
	}
}
