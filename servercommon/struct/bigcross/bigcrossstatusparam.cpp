#include "bigcrossstatusparam.hpp"

#include "servercommon/servercommon.h"
#include "servercommon/struct/structcommon.h"

bool BigCrossStatusParam::Serialize(TLVSerializer &outstream) const
{
	bool ret = outstream.Push(last_update_timestamp);
	if (!ret) 
		return false;

	return ret;
}

bool BigCrossStatusParam::Unserialize(TLVUnserializer &instream)
{
	bool ret = instream.Pop(&last_update_timestamp);
	if (!ret) 
		return false;

	return ret;
}

void BigCrossStatusParam::Reset()
{
	last_update_timestamp = 0;
}

