#include "wildbossdef.hpp"

bool WildbossPoker::Serialize(TLVSerializer2 & outstream) const
{
	bool ret = true;

	ret = ret && outstream.Push(index);
	ret = ret && outstream.Push(role_uid);

	TLVSerializer2 role_name_ser;
	role_name_ser.Reset((void *)&role_name, sizeof(role_name));
	role_name_ser.MoveCurPos(sizeof(role_name));
	ret = ret && outstream.Push(role_name_ser);

	ret = ret && outstream.Push(luck_coin);

	reward_info.Serialize(outstream);

	return ret;
}
