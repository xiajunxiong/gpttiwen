#include "crossstatusparam.hpp"

#include "servercommon/servercommon.h"
#include "servercommon/struct/structcommon.h"

bool CrossStatusParam::Serialize(TLVSerializer &outstream) const
{
	bool ret = outstream.Push(last_update_dayid);
	if (!ret) return false;

	return ret;
}

bool CrossStatusParam::Unserialize(TLVUnserializer &instream)
{
	bool ret = instream.Pop(&last_update_dayid);
	if (!ret) return false;

	return ret;
}

void CrossStatusParam::Reset()
{
	last_update_dayid = 0;
}

