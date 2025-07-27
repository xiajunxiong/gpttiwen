#include "cmdresult.h"
#include "servercommon/servercommon.h"

bool CmdResult::Serialize(TLVSerializer &outstream) const
{
	bool ret = outstream.Push(count);
	if (!ret) return false;

	for (int i = 0; i < count && i < CmdResult::MAX_RESULT_NUM; ++i)
	{
		const char* tmp = result_list[i].opresult;
		ret = outstream.Push(result_list[i].cmd_id) &&
			outstream.Push(tmp);
		if (!ret) return false;
	}

	return true;
}

bool CmdResult::Unserialize(TLVUnserializer &instream)
{
	bool ret = instream.Pop(&count);
	if (!ret || count > CmdResult::MAX_RESULT_NUM) return false;

	for (int i = 0; i < count; ++i)
	{
		const char *result_tmp = 0;

		ret = instream.Pop(&result_list[i].cmd_id) &&
			instream.Pop(&result_tmp);
		if (!ret) return false;

		strncpy(result_list[i].opresult, result_tmp, sizeof(result_list[i].opresult));
	}

	return true;
}
