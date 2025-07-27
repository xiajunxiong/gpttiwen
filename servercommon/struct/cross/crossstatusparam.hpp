#ifndef __CROSS_STATUS_PARAM_HPP__
#define __CROSS_STATUS_PARAM_HPP__

#include "common/tlvprotocol.h"
#include "servercommon/serverdef.h"
#include "servercommon/basedef.h"
#include "servercommon/crossdef.hpp"

class CrossStatusParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	void Reset();

	unsigned int last_update_dayid;

};

#endif // __CROSS_STATUS_PARAM_HPP__

