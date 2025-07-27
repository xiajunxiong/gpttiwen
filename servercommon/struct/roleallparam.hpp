#ifndef __ROLE_ALL_PARAM_H__
#define __ROLE_ALL_PARAM_H__

#include "structcommon.h"
#include "servercommon/basedef.h"
#include "common/tlvprotocol.h"

#include "roleinitparam.h"
#include "roleotherinitparam.hpp"

class RoleAllParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);
	
	void Reset();

	RoleInitParam			role_param;
	RoleOtherInitParam		other_param;
};

#endif	// __ROLE_ALL_PARAM_H__

