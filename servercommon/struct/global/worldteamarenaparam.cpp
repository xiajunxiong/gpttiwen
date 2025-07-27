#include "worldteamarenaparam.hpp"

bool WorldTeamArenaSignUpParam::Serialize(TLVSerializer & outstream) const
{
	bool ret = true;
	ret = outstream.Push(count);
	if (!ret)
	{
		return false;
	}
	for (int i = 0; i < count && i < ARRAY_LENGTH(team_sign_up_list); i++)
	{
		const WorldTeamArenaTeamSignUpData & sign_up_data = team_sign_up_list[i];
		const char * team_name_tmp = sign_up_data.team_name;
		ret = outstream.Push(sign_up_data.change_state) && outstream.Push(sign_up_data.team_unique_id) && outstream.Push(team_name_tmp);
		if (!ret)
		{
			return false;
		}

		TLVSerializer team_member_data;
		team_member_data.Reset((char *)&sign_up_data.member_list, sizeof(sign_up_data.member_list));
		team_member_data.MoveCurPos(sizeof(sign_up_data.member_list));
		ret = outstream.Push(team_member_data);
	}
	return ret;
}

bool WorldTeamArenaSignUpParam::Unserialize(TLVUnserializer & instream)
{
	bool ret = true;
	ret = instream.Pop(&count);
	if (!ret)
	{
		return false;
	}
	for (int i = 0; i < count && i < ARRAY_LENGTH(team_sign_up_list); i++)
	{
		WorldTeamArenaTeamSignUpData & sign_up_data = team_sign_up_list[i];
		const char * team_name_tmp = NULL;
		ret = instream.Pop(&sign_up_data.change_state) && instream.Pop(&sign_up_data.team_unique_id) && instream.Pop(&team_name_tmp);
		if (!ret)
		{
			return false;
		}
		SNPRINTF((char *)sign_up_data.team_name, sizeof(sign_up_data.team_name), team_name_tmp);

		TLVUnserializer team_member_data;
		ret = instream.Pop(&team_member_data);
		if (!ret)
		{
			return false;
		}

		if (team_member_data.Size() <= sizeof(sign_up_data.member_list))
		{
			memcpy((void *)&sign_up_data.member_list, team_member_data.Ptr(), team_member_data.Size());
		}
	}

	return ret;
}

bool WorldTeamArenaTeamRankParam::Serialize(TLVSerializer & outstream) const
{
	bool ret = true;
	ret = outstream.Push(count);
	if (!ret)
	{
		return false;
	}
	for (int i = 0; i < count && i < ARRAY_LENGTH(team_rank_list); i++)
	{
		const WorldTeamArenaTeamRankData &team_rank_data = team_rank_list[i];
		const char * team_name_tmp = team_rank_data.team_name;
		ret = outstream.Push(team_rank_data.change_state) && outstream.Push(team_rank_data.team_unique_id) && outstream.Push(team_name_tmp);
		if (!ret)
		{
			return false;
		}
		ret = outstream.Pushf(
			"hiiiii",
			team_rank_data.hidden_server_idx, team_rank_data.total_score, team_rank_data.victory_times, team_rank_data.fight_times, 
			team_rank_data.failures_times, team_rank_data.score_change_time
		);
		if (!ret) 
		{
			return false;
		}
		TLVSerializer team_member_data;
		team_member_data.Reset((char *)&team_rank_data.member_list, sizeof(team_rank_data.member_list));
		team_member_data.MoveCurPos(sizeof(team_rank_data.member_list));
		ret = outstream.Push(team_member_data);
	}
	return ret;
}

bool WorldTeamArenaTeamRankParam::Unserialize(TLVUnserializer & instream)
{
	bool ret = true;
	ret = instream.Pop(&count);
	if (!ret)
	{
		return false;
	}
	for (int i = 0; i < count && i < ARRAY_LENGTH(team_rank_list); i++)
	{
		WorldTeamArenaTeamRankData & team_rank_data = team_rank_list[i];
		const char * team_name_tmp = NULL;
		ret = instream.Pop(&team_rank_data.change_state) && instream.Pop(&team_rank_data.team_unique_id) && instream.Pop(&team_name_tmp);
		if (!ret)
		{
			return false;
		}
		SNPRINTF((char *)team_rank_data.team_name, sizeof(team_rank_data.team_name), team_name_tmp);

		ret = instream.Popf(
			"hiiiii",
			&team_rank_data.hidden_server_idx, &team_rank_data.total_score, &team_rank_data.victory_times, &team_rank_data.fight_times, 
			&team_rank_data.failures_times, &team_rank_data.score_change_time
		);
		if (!ret)
		{
			return false;
		}

		TLVUnserializer team_member_data;
		ret = instream.Pop(&team_member_data);
		if (!ret)
		{
			return false;
		}

		if (team_member_data.Size() <= sizeof(team_rank_data.member_list))
		{
			memcpy((void *)&team_rank_data.member_list, team_member_data.Ptr(), team_member_data.Size());
		}
	}

	return ret;
}

void WorldTeamArenaTeamSignUpSimpleInfo::SetData(const WorldTeamArenaTeamSignUpData & data)
{
	team_unique_id = data.team_unique_id;
	memcpy(team_name, data.team_name, sizeof(team_name));
	total_capablity = 0;
	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(member_list) == ARRAY_ITEM_COUNT(data.member_list));
	for (int i = 0; i < ARRAY_LENGTH(member_list); i++)
	{
		member_list[i].role_id = data.member_list[i].role_uid;
		member_list[i].profession = data.member_list[i].profession;
		total_capablity += data.member_list[i].capablity;
		if (0 == i)	//首位为队长
		{
			level = data.member_list[i].level;
			top_level = data.member_list[i].top_level;
			avatar_type = data.member_list[i].avatar_type;
			headshot_id = data.member_list[i].headshot_id;
			fly_flag = data.member_list[i].fly_flag;
		}
	}
	reserve_sh = 0;
}

void WorldTeamArenaUserDetailedInfo::SetData(const WorldTeamArenaUserInfo & data)
{
	role_uid = data.role_uid;
	profession = data.profession;
	level = data.level;
	top_level = data.top_level;
	avatar_type = data.avatar_type;
	headshot_id = data.headshot_id;
	fly_flag = data.fly_flag;
	reserve_sh = data.reserve_sh;
	capablity = data.capablity;
	memcpy(role_name, data.role_name, sizeof(role_name));
}

void WorldTeamArenaRankInfo::SetData(const WorldTeamArenaTeamRankData & data)
{
	team_unique_id = data.team_unique_id;
	memcpy(team_name, data.team_name, sizeof(team_name));
	total_score = data.total_score;
	victory_times = data.victory_times;
	fight_times = data.fight_times;
}

void WorldTeamArenaTeamSignUpData::SetData(const WorldTeamArenaTeamRankData & info)
{
	team_unique_id = info.team_unique_id;
	F_STRNCPY(team_name, info.team_name, sizeof(team_name));
	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(member_list) == ARRAY_ITEM_COUNT(info.member_list));
	for (int i = 0; i < ARRAY_ITEM_COUNT(member_list); ++i)
	{
		member_list[i] = info.member_list[i];
	}
}
