#include "rolefirstkillparam.hpp"
#include "common/tlvprotocol.h"

bool RoleFirstKillParamList::Serialize(TLVSerializer &outstream) const
{
	bool ret = outstream.Push(count);
	if (!ret)
	{
		return false;
	}

	for (int i = 0; i < count && i < MAX_FIRST_KILL_RECORD_NUM; ++i)
	{
		ret = outstream.Push(info_list[i].monster_group_id)
			&& outstream.Push(info_list[i].change_state)
			&& outstream.Push(info_list[i].timestamp)
			&& outstream.Push(info_list[i].team_member_num)
			&& outstream.Push(info_list[i].win_round)
			&& outstream.Push(info_list[i].team_level_high)
			&& outstream.Push(info_list[i].team_level_low)
			&& outstream.Push(info_list[i].team_level_average)
			&& outstream.Push(info_list[i].dead_role_num)
			&& outstream.Push(info_list[i].week_kill_flag);

		if (!ret)
		{
			return false;
		}
	}

	return ret;
}

bool RoleFirstKillParamList::Unserialize(TLVUnserializer &instream)
{
	bool ret = instream.Pop(&count);
	if (!ret)
	{
		return false;
	}

	for (int i = 0; i < count && i < MAX_FIRST_KILL_RECORD_NUM; ++i)
	{
		ret = instream.Pop(&info_list[i].monster_group_id)
			&& instream.Pop(&info_list[i].change_state)
			&& instream.Pop(&info_list[i].timestamp)
			&& instream.Pop(&info_list[i].team_member_num)
			&& instream.Pop(&info_list[i].win_round)
			&& instream.Pop(&info_list[i].team_level_high)
			&& instream.Pop(&info_list[i].team_level_low)
			&& instream.Pop(&info_list[i].team_level_average)
			&& instream.Pop(&info_list[i].dead_role_num)
			&& instream.Pop(&info_list[i].week_kill_flag);

		if (!ret)
		{
			return false;
		}
	}

	return ret;
}

