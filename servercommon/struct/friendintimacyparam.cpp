#include "friendintimacyparam.h"

bool FriendIntimacyListParam::Serialize(TLVSerializer &outstream) const
{
	if (!outstream.Push(id_from))
	{
		return false;
	}

	if (!outstream.Push(count))
	{
		return false;
	}

	bool ret = true;

	for (int i = 0; i < count && i < MAX_ONCE_FRIENDS_INTIMACY_COUNT; ++i)
	{
		ret = outstream.Push(intimacy_info_arr[i].change_state) && outstream.Push(intimacy_info_arr[i].role_id) && outstream.Push(intimacy_info_arr[i].friend_id);
		if (!ret) return false;

		if (intimacy_info_arr[i].change_state == structcommon::CS_DELETE) continue;

		ret = outstream.Push(intimacy_info_arr[i].intimacy_val) &&
			outstream.Push(intimacy_info_arr[i].day_frist_to_singchat_timestamp) &&
			outstream.Push(intimacy_info_arr[i].day_frist_from_singchat_timestamp);

		if (!ret)
		{
			return false;
		}
	}

	return ret;
}

bool FriendIntimacyListParam::Unserialize(TLVUnserializer &instream)
{
	if (!instream.Pop(&id_from))
	{
		return false;
	}

	if (!instream.Pop(&count))
	{
		return false;
	}

	bool ret = true;

	for (int i = 0; i < count && i < MAX_ONCE_FRIENDS_INTIMACY_COUNT; ++i)
	{
		ret = instream.Pop(&intimacy_info_arr[i].change_state) && instream.Pop(&intimacy_info_arr[i].role_id) && instream.Pop(&intimacy_info_arr[i].friend_id);
		if (!ret) return false;

		if (intimacy_info_arr[i].change_state == structcommon::CS_DELETE) continue;

		const char *role_name_tmp = NULL;

		ret = instream.Pop(&intimacy_info_arr[i].intimacy_val) &&
			instream.Pop(&intimacy_info_arr[i].day_frist_to_singchat_timestamp) &&
			instream.Pop(&intimacy_info_arr[i].day_frist_from_singchat_timestamp);

		if (!ret)
		{
			return false;
		}

	}

	return ret;
}
