#include "friendparam.h"
#include "servercommon/struct/structcommon.h"

bool FriendListParam::Serialize(TLVSerializer &outstream)const
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

	for (int i = 0;i < count && i < MAX_ONCE_FRIENDS_COUNT; ++i)
	{
		ret = outstream.Push(friend_list[i].change_state) && outstream.Push(friend_list[i].role_id) && outstream.Push(friend_list[i].friend_uid);
		if (!ret) return false;

		if (friend_list[i].change_state == structcommon::CS_DELETE) continue;

		const char *role_name_tmp = friend_list[i].role_name;

		ret = outstream.Push(friend_list[i].friend_uid) &&
			outstream.Push(friend_list[i].intimacy) &&
			outstream.Push(friend_list[i].battle_add_intimacy) &&
			outstream.Push(friend_list[i].group_type) &&
			outstream.Push(role_name_tmp) &&
			outstream.Push(friend_list[i].sex) &&
			outstream.Push(friend_list[i].prof) &&
			outstream.Push(friend_list[i].level) &&
			outstream.Push(friend_list[i].avatar_timestamp) &&
			outstream.Push(friend_list[i].avatar_type) &&
			outstream.Push(friend_list[i].headshot_id) &&
			outstream.Push(friend_list[i].last_online_timestamp) ;

		if (!ret)
		{
			return false;
		}
	}

	return ret;
};

bool FriendListParam::Unserialize(TLVUnserializer &instream)
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

	for (int i = 0; i < count && i < MAX_ONCE_FRIENDS_COUNT; ++i)
	{
		ret = instream.Pop(&friend_list[i].change_state) &&instream.Pop(&friend_list[i].role_id) &&instream.Pop(&friend_list[i].friend_uid);
		if (!ret) return false;

		if (friend_list[i].change_state == structcommon::CS_DELETE) continue;

		const char *role_name_tmp = NULL;

		ret = instream.Pop(&friend_list[i].friend_uid) &&
			instream.Pop(&friend_list[i].intimacy) &&
			instream.Pop(&friend_list[i].battle_add_intimacy) &&
			instream.Pop(&friend_list[i].group_type) && 
			instream.Pop(&role_name_tmp) &&
			instream.Pop(&friend_list[i].sex) &&
			instream.Pop(&friend_list[i].prof) &&
			instream.Pop(&friend_list[i].level) &&
			instream.Pop(&friend_list[i].avatar_timestamp) &&
			instream.Pop(&friend_list[i].avatar_type) &&
			instream.Pop(&friend_list[i].headshot_id) &&
			instream.Pop(&friend_list[i].last_online_timestamp);

		if (!ret)
		{
			return false;
		}

		F_STRNCPY((char *)friend_list[i].role_name, role_name_tmp, sizeof(friend_list[i].role_name));
	}

	return ret;
}

bool BlacklistsParam::Serialize(TLVSerializer & outstream) const
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

	for (int i = 0; i < count && i < MAX_ONCE_FRIENDS_COUNT; ++i)
	{
		ret = outstream.Push(black_list[i].change_state) && outstream.Push(black_list[i].role_id) && outstream.Push(black_list[i].uid);
		if (!ret) return false;

		if (black_list[i].change_state == structcommon::CS_DELETE) continue;

	}

	return ret;
}

bool BlacklistsParam::Unserialize(TLVUnserializer & instream)
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

	for (int i = 0; i < count && i < MAX_ONCE_FRIENDS_COUNT; ++i)
	{
		ret = instream.Pop(&black_list[i].change_state) && instream.Pop(&black_list[i].role_id) && instream.Pop(&black_list[i].uid);
		if (!ret) return false;

		if (black_list[i].change_state == structcommon::CS_DELETE) continue;
	}

	return ret;
}

bool FriendRelationParam::Serialize(TLVSerializer &outstream) const
{
	if (!outstream.Push(count))
	{
		return false;	
	}

	if (!outstream.Push(last_inc_index))
	{
		return false;
	}

	for (int i = 0; i < count && i < FRIEND_RELATION_LIST_ONCE_OPER_RECORD_COUNT; ++i)
	{
		if (!outstream.Push(pair_list[i].role_id) || 
			!outstream.Push(pair_list[i].friend_id))
		{
			return false;
		}
	}

	return true;
}

bool FriendRelationParam::Unserialize(TLVUnserializer &instream)
{
	if (!instream.Pop(&count) || count < 0)
	{
		return false;
	}

	if (!instream.Pop(&last_inc_index))
	{
		return false;
	}

	for (int i = 0; i < count && i < FRIEND_RELATION_LIST_ONCE_OPER_RECORD_COUNT; ++i)
	{
		if (!instream.Pop(&pair_list[i].role_id) || 
			!instream.Pop(&pair_list[i].friend_id))
		{
			return false;
		}
	}

	return true;
}