#include "miyidef.hpp"

bool MiYiPoker::Serialize(TLVSerializer2 & outstream) const
{
	bool ret = true;

	ret = ret && outstream.Push(index);
	ret = ret && outstream.Push(role_uid);

	TLVSerializer2 role_name_ser;
	role_name_ser.Reset((void *)&role_name, sizeof(role_name));
	role_name_ser.MoveCurPos(sizeof(role_name));
	ret = ret && outstream.Push(role_name_ser);

	ret = ret && outstream.Push(fixed_reward.reward_id);
	ret = ret && outstream.Push(fixed_reward.num);
	ret = ret && outstream.Push(fixed_reward.is_bind);

	poker_info.Serialize(outstream);

	return ret;
}
