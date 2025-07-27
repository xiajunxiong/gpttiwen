#ifndef __CMD_RESULT_H__
#define __CMD_RESULT_H__

#include <string>
#include "servercommon/serverdef.h"
#include "common/tlvprotocol.h"

class CmdResult
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	struct __result
	{
		int cmd_id;
		OPResult opresult;
	};

	static const int MAX_RESULT_NUM = 32;

	int count;
	__result result_list[MAX_RESULT_NUM];
};

#endif // __CMD_RESULT_H__



