#include "offlineregisterparam.hpp"

#include "servercommon/servercommon.h"
#include "servercommon/struct/structcommon.h"

OLD_USE_INTERNAL_NETWORK();

bool OfflineRegisterParam::Serialize(TLVSerializer &outstream) const
{
	bool ret = outstream.Push(count);
	if (!ret) return false;

	for (int i = 0; i < count && i < MAX_OFFLINE_REGISTER_COUNT; ++i)
	{
		TLVSerializer other_param_data;
		other_param_data.Reset((void*)&offline_register_list[i].other_param, sizeof(offline_register_list[i].other_param));
		other_param_data.MoveCurPos(sizeof(offline_register_list[i].other_param));

		ret = outstream.Push(offline_register_list[i].id_offline_register)
			&& outstream.Push(offline_register_list[i].change_state)
			&& outstream.Push(offline_register_list[i].role_id)
			&& outstream.Push(offline_register_list[i].last_divorce_time)
			&& outstream.Push(offline_register_list[i].chongzhi_cmd_flag)
			&& outstream.Push(other_param_data);
		if (!ret)
		{
			return false;
		}

		if (offline_register_list[i].change_state == structcommon::CS_DELETE) continue;
	}

	return true;
}

bool OfflineRegisterParam::Unserialize(TLVUnserializer &instream)
{
	bool ret = instream.Pop(&count);
	if (!ret) return false;

	for (int i = 0; i < count && i < MAX_OFFLINE_REGISTER_COUNT; ++i)
	{
		TLVUnserializer other_param_data;

		ret = instream.Pop(&offline_register_list[i].id_offline_register)
			&& instream.Pop(&offline_register_list[i].change_state)
			&& instream.Pop(&offline_register_list[i].role_id)
			&& instream.Pop(&offline_register_list[i].last_divorce_time)
			&& instream.Pop(&offline_register_list[i].chongzhi_cmd_flag)
			&& instream.Pop(&other_param_data);
		if (!ret)
		{
			return false;
		}

		if (other_param_data.Size() <= sizeof(offline_register_list[i].other_param))
		{
			memcpy(&offline_register_list[i].other_param, other_param_data.Ptr(), other_param_data.Size());
		}

		if (offline_register_list[i].change_state == structcommon::CS_DELETE) continue;

	}

	return true;
}
