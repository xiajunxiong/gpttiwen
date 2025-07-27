#include "roleallparam.hpp"

bool RoleAllParam::Serialize(TLVSerializer &outstream) const
{
	bool ret = role_param.Serialize(outstream);

	return ret && 
		other_param.Serialize(outstream);
}

bool RoleAllParam::Unserialize(TLVUnserializer &instream)
{
	bool ret = role_param.Unserialize(instream);

	return ret && 
		other_param.Unserialize(instream);
}

void RoleAllParam::Reset()
{
	memset(&role_param, 0, sizeof(role_param));
}

