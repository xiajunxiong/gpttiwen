#include "universeteamarenadef.hpp"
#include "common/tlvprotocol.h"
#include "servercommon/servercommon.h"
#include "servercommon/protobuf/universe_team_arena_team_data.pb.h"
#include "servercommon/msgserialize.hpp"
#include <sstream>

bool UTABaseParam::Serialize(TLVSerializer &outstream) const
{
	bool ret = outstream.Push(cur_status)
		&& outstream.Push(qualification_round_idx)
		&& outstream.Push(main_round_idx)
		&& outstream.Push(season_idx)
		&& outstream.Push(next_status_timestamp)
		&& outstream.Push(prepare_status_timestamp)
		&& outstream.Push(qualification_status_timestamp)
		&& outstream.Push(main_status_timestamp)
		&& outstream.Push(calculate_status_timestamp)
		&& outstream.Push(rest_status_timestamp)
		&& outstream.Push(qualification_round_timestamp)
		&& outstream.Push(main_round_timestamp)
		&& outstream.Push(next_make_main_schedule_timestamp)
		&& outstream.Push(last_season_overview_buffer_length)
		&& outstream.Push(active_reward_begin_timestamp)
		&& outstream.Push(active_reward_end_timestamp);
	
	if (!ret)
	{
		return false;
	}

	for (int i = 0; i < ARRAY_LENGTH(qualification_timestamp_list); ++i)
	{
		ret = ret && outstream.Push(qualification_timestamp_list[i]);
	}

	for (int i = 0; i < ARRAY_LENGTH(main_timestamp_list); ++i)
	{
		ret = ret && outstream.Push(main_timestamp_list[i]);
	}

	{
		TLVSerializer serializer;
		serializer.Reset((void*)last_season_overview_buffer, sizeof(last_season_overview_buffer));
		serializer.MoveCurPos(last_season_overview_buffer_length);
		ret = outstream.Push(serializer);
		if (!ret) return false;
	}

	return ret;
}

bool UTABaseParam::Unserialize(TLVUnserializer &instream)
{
	bool ret = instream.Pop(&cur_status)
		&& instream.Pop(&qualification_round_idx)
		&& instream.Pop(&main_round_idx)
		&& instream.Pop(&season_idx)
		&& instream.Pop(&next_status_timestamp)
		&& instream.Pop(&prepare_status_timestamp)
		&& instream.Pop(&qualification_status_timestamp)
		&& instream.Pop(&main_status_timestamp)
		&& instream.Pop(&calculate_status_timestamp)
		&& instream.Pop(&rest_status_timestamp)
		&& instream.Pop(&qualification_round_timestamp)
		&& instream.Pop(&main_round_timestamp)
		&& instream.Pop(&next_make_main_schedule_timestamp)
		&& instream.Pop(&last_season_overview_buffer_length)
		&& instream.Pop(&active_reward_begin_timestamp)
		&& instream.Pop(&active_reward_end_timestamp);

	if (!ret)
	{
		return false;
	}

	for (int i = 0; i < ARRAY_LENGTH(qualification_timestamp_list); ++i)
	{
		ret = ret && instream.Pop(&qualification_timestamp_list[i]);
	}

	for (int i = 0; i < ARRAY_LENGTH(main_timestamp_list); ++i)
	{
		ret = ret && instream.Pop(&main_timestamp_list[i]);
	}

	{
		TLVUnserializer unserializer;
		ret = instream.Pop(&unserializer);
		if (!ret)
		{
			return false;
		}

		if (unserializer.Size() <= sizeof(last_season_overview_buffer))
		{
			memcpy(last_season_overview_buffer, unserializer.Ptr(), unserializer.Size());
			last_season_overview_buffer_length = static_cast<int>(unserializer.Size());
		}
		else
		{
			return false;
		}
	}

	return ret;
}

bool UTATeamMemberInfo::Serialize(TLVSerializer &outstream) const
{
	const char* name = role_name;
	bool ret = outstream.Push(uuid)
		&& outstream.Push(uid)
		&& outstream.Push(profession)
		&& outstream.Push(level)
		&& outstream.Push(top_level)
		&& outstream.Push(avatar_type)
		&& outstream.Push(headshot_id)
		&& outstream.Push(fly_flag)
		&& outstream.Push(today_get_reward_num)
		&& outstream.Push(capablity)
		&& outstream.Push(win_box_fetch_flag)
		&& outstream.Push(name);

	if (!ret) return false;

	{
		static TLVSerializer appearance_data;
		appearance_data.Reset((void *)&appearance, sizeof(appearance));
		appearance_data.MoveCurPos(sizeof(appearance));
		ret = outstream.Push(appearance_data);
		if (!ret) return false;
	}

	return ret;
}

bool UTATeamMemberInfo::Serialize(char* buffer, int buffer_size, int* out_length)
{
	bool ret = WRITE_LL(uuid)
		&& WRITE_INT(uid)
		&& WRITE_INT(profession)
		&& WRITE_SHORT(level)
		&& WRITE_SHORT(top_level)
		&& WRITE_SHORT(avatar_type)
		&& WRITE_SHORT(headshot_id)
		&& WRITE_SHORT(fly_flag)
		&& WRITE_SHORT(today_get_reward_num)
		&& WRITE_INT(capablity)
		&& WRITE_INT(win_box_fetch_flag)
		&& WRITE_STRN(role_name, sizeof(role_name))
		&& WRITE_STRN((const char*)&appearance, sizeof(appearance));

	return ret;
}

bool UTATeamMemberInfo::Deserialize(const char* data, int data_length, int* out_offset)
{
	bool ret = READ_LL(uuid)
		&& READ_INT(uid)
		&& READ_INT(profession)
		&& READ_SHORT(level)
		&& READ_SHORT(top_level)
		&& READ_SHORT(avatar_type)
		&& READ_SHORT(headshot_id)
		&& READ_SHORT(fly_flag)
		&& READ_SHORT(today_get_reward_num)
		&& READ_INT(capablity)
		&& READ_INT(win_box_fetch_flag)
		&& READ_STRN(role_name, sizeof(role_name), sizeof(role_name))
		&& READ_STRN((char*)&appearance, sizeof(appearance), sizeof(appearance));

	return ret;
}

bool UTATeamMemberInfo::Unserialize(TLVUnserializer &instream)
{
	const char* name = NULL;
	bool ret = instream.Pop(&uuid)
		&& instream.Pop(&uid)
		&& instream.Pop(&profession)
		&& instream.Pop(&level)
		&& instream.Pop(&top_level)
		&& instream.Pop(&avatar_type)
		&& instream.Pop(&headshot_id)
		&& instream.Pop(&fly_flag)
		&& instream.Pop(&today_get_reward_num)
		&& instream.Pop(&capablity)
		&& instream.Pop(&win_box_fetch_flag)
		&& instream.Pop(&name);
	
	if (!ret) return false;

	{
		static TLVUnserializer appearance_data;
		ret = instream.Pop(&appearance_data);
		if (!ret || appearance_data.Size() > sizeof(appearance)) return false;

		memcpy(&appearance, appearance_data.Ptr(), appearance_data.Size());
	}

	F_STRNCPY(role_name, name, sizeof(role_name));
	return ret;
}

bool UTATeamMemberInfo::GetPBData(PB_UTATeamMemberData* out_data) const
{
	out_data->set_uuid(uuid);
	out_data->set_uid(uid);
	out_data->set_profession(profession);
	out_data->set_level(level);
	out_data->set_top_level(top_level);
	out_data->set_avatar_type(avatar_type);
	out_data->set_headshot_id(headshot_id);
	out_data->set_fly_flag(fly_flag);
	out_data->set_role_name(role_name);
	out_data->set_appearance(&appearance, sizeof(appearance));
	out_data->set_win_box_fetch_flag(win_box_fetch_flag);
	out_data->set_today_reward_num(today_get_reward_num);
	return true;
}

bool UTATeamMemberInfo::SetDataFromPB(const PB_UTATeamMemberData& pb_data)
{
	uuid = pb_data.uuid();
	uid = pb_data.uid();
	profession = pb_data.profession();
	level = pb_data.level();
	top_level = pb_data.top_level();
	avatar_type = pb_data.avatar_type();
	headshot_id = pb_data.headshot_id();
	fly_flag = pb_data.fly_flag();
	today_get_reward_num = pb_data.today_reward_num();
	win_box_fetch_flag = pb_data.win_box_fetch_flag();
	F_STRNCPY(role_name, pb_data.role_name().c_str(), sizeof(role_name));

	if (sizeof(appearance) >= pb_data.appearance().size())
	{
		memcpy(&appearance, pb_data.appearance().c_str(), pb_data.appearance().size());
	}
	else
	{
		
#ifdef _DEBUG
		assert(0);
#endif

		return false;
	}

	return true;
}

bool UTATeamInfo::Serialize(TLVSerializer &outstream) const
{
	const char* name = team_name;
	bool ret = outstream.Push(plat_type)
		&& outstream.Push(hidden_idx)
		&& outstream.Push(season_idx)
		&& outstream.Push(team_id_in_hidden_server)
		&& outstream.Push(team_unique_id)
		&& outstream.Push(name)
		&& outstream.Push(rank_left)
		&& outstream.Push(rank_right)
		&& outstream.Push(qualification_score)
		&& outstream.Push(world_team_arena_score)
		&& outstream.Push(last_modify_fight_result_timestamp)
		&& outstream.Push(qualification_win_num)
		&& outstream.Push(qualification_lose_num)
		&& outstream.Push(qualification_giveup_num)
		&& outstream.Push(qualification_draw_num)
		&& outstream.Push(main_win_num)
		&& outstream.Push(main_lose_num)
		&& outstream.Push(cur_continuous_win_num)
		&& outstream.Push(max_continuous_win_num);

	if (!ret)
	{
		return false;
	}
	
	for (int i = 0; i < UTA_TEAM_MEMBER_NUM; ++i)
	{
		ret = member_info_list[i].Serialize(outstream);
		if (!ret)
		{
			return false;
		}
	}

	for (int i = 0; i < ARRAY_ITEM_COUNT(qualification_schedules); ++i)
	{
		ret = outstream.Push(qualification_schedules[i]);
		if (!ret)
		{
			return false;
		}
	}

	for (int i = 0; i < ARRAY_ITEM_COUNT(main_schedules); ++i)
	{
		ret = outstream.Push(main_schedules[i]);
		if (!ret)
		{
			return false;
		}
	}

	return ret;
}

bool UTATeamInfo::Unserialize(TLVUnserializer &instream)
{
	const char* name = NULL;
	bool ret = instream.Pop(&plat_type)
		&& instream.Pop(&hidden_idx)
		&& instream.Pop(&season_idx)
		&& instream.Pop(&team_id_in_hidden_server)
		&& instream.Pop(&team_unique_id)
		&& instream.Pop(&name)
		&& instream.Pop(&rank_left)
		&& instream.Pop(&rank_right)
		&& instream.Pop(&qualification_score)
		&& instream.Pop(&world_team_arena_score)
		&& instream.Pop(&last_modify_fight_result_timestamp)
		&& instream.Pop(&qualification_win_num)
		&& instream.Pop(&qualification_lose_num)
		&& instream.Pop(&qualification_giveup_num)
		&& instream.Pop(&qualification_draw_num)
		&& instream.Pop(&main_win_num)
		&& instream.Pop(&main_lose_num)
		&& instream.Pop(&cur_continuous_win_num)
		&& instream.Pop(&max_continuous_win_num);

	if (!ret)
	{
		return false;
	}
	F_STRNCPY(team_name, name, sizeof(team_name));

	for (int i = 0; i < UTA_TEAM_MEMBER_NUM; ++i)
	{
		ret = member_info_list[i].Unserialize(instream);
		if (!ret)
		{
			return false;
		}
	}

	for (int i = 0; i < ARRAY_ITEM_COUNT(qualification_schedules); ++i)
	{
		ret = instream.Pop(&qualification_schedules[i]);
		if (!ret)
		{
			return false;
		}
	}

	for (int i = 0; i < ARRAY_ITEM_COUNT(main_schedules); ++i)
	{
		ret = instream.Pop(&main_schedules[i]);
		if (!ret)
		{
			return false;
		}
	}

	return ret;
}

bool UTATeamListParamForInit::Serialize(TLVSerializer &outstream) const
{
	bool ret = outstream.Push(next_id_from)
		&& outstream.Push(static_cast<int>(team_list.size()));
	if (!ret) return false;

	for (size_t i = 0; i < team_list.size(); ++i)
	{
		ret = team_list[i].Serialize(outstream);
		if (!ret) return false;
	}

	return true;
}

bool UTATeamListParamForInit::Unserialize(TLVUnserializer &instream)
{
	int team_num = 0;
	bool ret = instream.Pop(&next_id_from)
		&& instream.Pop(&team_num);
	if (!ret) return false;

	team_list.reserve(team_num);
	for (int i = 0; i < team_num; ++i)
	{
		team_list.push_back(UTATeamInfo());
		UTATeamInfo& info = team_list[team_list.size() - 1];

		if (!info.Unserialize(instream)) return false;
	}

	return true;
}

bool UTATeamListParamForSave::Serialize(TLVSerializer &outstream) const
{
	bool ret = outstream.Push(static_cast<int>(save_team_list.size()))
		&& outstream.Push(static_cast<int>(delete_team_list.size()));
	if (!ret) return false;

	for (int i = 0; i < static_cast<int>(save_team_list.size()); ++i)
	{
		if (!save_team_list[i].Serialize(outstream)) return false;
	}

	for (int i = 0; i < static_cast<int>(delete_team_list.size()); ++i)
	{
		if (!outstream.Push(delete_team_list[i])) return false;
	}

	return true;
}

bool UTATeamListParamForSave::Unserialize(TLVUnserializer &instream)
{
	int save_team_num = 0;
	int delete_team_num = 0;
	bool ret = instream.Pop(&save_team_num)
		&& instream.Pop(&delete_team_num);
	if (!ret) return false;

	{
		save_team_list.clear();
		save_team_list.reserve(save_team_num);
		for (int i = 0; i < save_team_num; ++i)
		{
			save_team_list.push_back(UTATeamInfo());
			UTATeamInfo& info = save_team_list[save_team_list.size() - 1];

			if (!info.Unserialize(instream)) return false;
		}
	}

	{
		delete_team_list.clear();
		delete_team_list.reserve(delete_team_num);
		for (int i = 0; i < delete_team_num; ++i)
		{
			long long delete_team_id = 0;
			if (!instream.Pop(&delete_team_id)) return false;

			delete_team_list.push_back(delete_team_id);
		}
	}

	return true;
}

bool UTAScheduleListParamForSave::Serialize(TLVSerializer &outstream) const
{
	bool ret = outstream.Push(static_cast<int>(save_schedule_list.size()))
		&& outstream.Push(static_cast<int>(delete_schedule_list.size()));
	if (!ret) return false;

	for (int i = 0; i < static_cast<int>(save_schedule_list.size()); ++i)
	{
		if (!save_schedule_list[i].Serialize(outstream)) return false;
	}

	for (int i = 0; i < static_cast<int>(delete_schedule_list.size()); ++i)
	{
		if (!outstream.Push(delete_schedule_list[i])) return false;
	}

	return true;
}

bool UTAScheduleListParamForSave::Unserialize(TLVUnserializer &instream)
{
	int save_schedule_num = 0;
	int delete_schedule_num = 0;
	bool ret = instream.Pop(&save_schedule_num)
		&& instream.Pop(&delete_schedule_num);
	if (!ret) return false;

	{
		save_schedule_list.clear();
		save_schedule_list.reserve(save_schedule_num);
		for (int i = 0; i < save_schedule_num; ++i)
		{
			save_schedule_list.push_back(UTAScheduleInfo());
			UTAScheduleInfo& info = save_schedule_list[save_schedule_list.size() - 1];

			if (!info.Unserialize(instream)) return false;
		}
	}

	{
		delete_schedule_list.clear();
		delete_schedule_list.reserve(delete_schedule_num);
		for (int i = 0; i < delete_schedule_num; ++i)
		{
			long long delete_schedule_id = 0;
			if (!instream.Pop(&delete_schedule_id)) return false;

			delete_schedule_list.push_back(delete_schedule_id);
		}
	}

	return true;
}

bool UTAScheduleListParamForInit::Serialize(TLVSerializer &outstream) const
{
	bool ret = outstream.Push(next_id_from)
		&& outstream.Push(static_cast<int>(schedule_list.size()));
	if (!ret) return false;

	for (size_t i = 0; i < schedule_list.size(); ++i)
	{
		ret = schedule_list[i].Serialize(outstream);
		if (!ret) return false;
	}

	return true;
}

bool UTAScheduleListParamForInit::Unserialize(TLVUnserializer &instream)
{
	int schedule_num = 0;
	bool ret = instream.Pop(&next_id_from)
		&& instream.Pop(&schedule_num);
	if (!ret) return false;

	schedule_list.reserve(schedule_num);
	for (int i = 0; i < schedule_num; ++i)
	{
		schedule_list.push_back(UTAScheduleInfo());
		UTAScheduleInfo& info = schedule_list[schedule_list.size() - 1];

		if (!info.Unserialize(instream)) return false;
	}

	return true;
}

bool UTAScheduleInfo::Serialize(TLVSerializer &outstream) const
{
	return outstream.Push(type)
		&& outstream.Push(fight_result)
		&& outstream.Push(round_idx)
		&& outstream.Push(match_idx)
		&& outstream.Push(schedule_id)
		&& outstream.Push(atk_team_unique_id)
		&& outstream.Push(def_team_unique_id)
		&& outstream.Push(season_idx)
		&& outstream.Push(is_win_group)
		&& outstream.Push(video_file_id);
}

bool UTAScheduleInfo::Unserialize(TLVUnserializer &instream)
{
	return instream.Pop(&type)
		&& instream.Pop(&fight_result)
		&& instream.Pop(&round_idx)
		&& instream.Pop(&match_idx)
		&& instream.Pop(&schedule_id)
		&& instream.Pop(&atk_team_unique_id)
		&& instream.Pop(&def_team_unique_id)
		&& instream.Pop(&season_idx)
		&& instream.Pop(&is_win_group)
		&& instream.Pop(&video_file_id);
}

std::string GetScheduleListString(int num, const long long* schedule_list)
{
	std::stringstream ss;
	for (int i = 0; i < num; ++i)
	{
		if (i > 0) ss << ",";

		ss << schedule_list[i];
	}

	return ss.str();
}

bool SetScheduleListFromString(const std::string& str, int num, long long* schedule_list)
{
	if (str.empty()) return true;

	std::stringstream ss(str);
	
	for (int i = 0; i < num; ++i)
	{
		if (i < num - 1)
		{
			char tmp;
			ss >> schedule_list[i] >> tmp;
		}
		else
		{
			ss >> schedule_list[i];
		}

		if (ss.bad())
		{
			return false;
		}
	}

	return true;
}
