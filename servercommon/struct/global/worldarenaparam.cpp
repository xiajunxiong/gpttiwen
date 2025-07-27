#include "worldarenaparam.hpp"
#include "servercommon/protobuf/worldarena_base_data.pb.h"
#include "servercommon/protobuf/worldarena_user_data.pb.h"
#include "servercommon/protobuf/worldarena_schedule_data.pb.h"
#include "servercommon/protobufmanager.hpp"

bool WorldArenaBaseParam::Serialize(TLVSerializer &outstream) const
{
	bool ret = outstream.Push(cur_status)
		&& outstream.Push(season_index)
		&& outstream.Push(next_status_timestamp)
		&& outstream.Push(next_signup_status_timestamp)
		&& outstream.Push(next_schedule_status_timestamp)
		&& outstream.Push(next_battle_status_timestamp)
		&& outstream.Push(next_calculate_status_timestamp)
		&& outstream.Push(next_rest_status_timestamp)
		&& outstream.Push(robot_index_generator)
		&& outstream.Push(round_battle_start_timestamp)
		&& outstream.Push(next_round_battle_start_timestamp);

	return ret;
}

bool WorldArenaBaseParam::Unserialize(TLVUnserializer &instream)
{
	bool ret = instream.Pop(&cur_status)
		&& instream.Pop(&season_index)
		&& instream.Pop(&next_status_timestamp)
		&& instream.Pop(&next_signup_status_timestamp)
		&& instream.Pop(&next_schedule_status_timestamp)
		&& instream.Pop(&next_battle_status_timestamp)
		&& instream.Pop(&next_calculate_status_timestamp)
		&& instream.Pop(&next_rest_status_timestamp)
		&& instream.Pop(&robot_index_generator)
		&& instream.Pop(&round_battle_start_timestamp)
		&& instream.Pop(&next_round_battle_start_timestamp);

	return ret;
}

bool WorldArenaBaseParam::GetPBData(ARG_OUT PB_WorldArenaBaseData* out_data) const
{
	out_data->set_cur_status(cur_status);
	out_data->set_season_index(season_index);
	out_data->set_next_status_timestamp(next_status_timestamp);
	out_data->set_next_signup_status_timestamp(next_signup_status_timestamp);
	out_data->set_next_schedule_status_timestamp(next_schedule_status_timestamp);
	out_data->set_next_battle_status_timestamp(next_battle_status_timestamp);
	out_data->set_next_calculate_status_timestamp(next_calculate_status_timestamp);
	out_data->set_next_rest_status_timestamp(next_rest_status_timestamp);
	out_data->set_robot_index_generator(robot_index_generator);
	out_data->set_round_battle_start_timestamp(round_battle_start_timestamp);
	out_data->set_next_round_battle_start_timestamp(next_round_battle_start_timestamp);

	return true;
}

bool WorldArenaBaseParam::SetDataFromPB(const PB_WorldArenaBaseData& pb_data)
{
	cur_status = pb_data.cur_status();
	season_index = pb_data.season_index();
	next_status_timestamp = pb_data.next_status_timestamp();
	next_signup_status_timestamp = pb_data.next_signup_status_timestamp();
	next_schedule_status_timestamp = pb_data.next_schedule_status_timestamp();
	next_battle_status_timestamp = pb_data.next_battle_status_timestamp();
	next_calculate_status_timestamp = pb_data.next_calculate_status_timestamp();
	next_rest_status_timestamp = pb_data.next_rest_status_timestamp();
	robot_index_generator = pb_data.robot_index_generator();
	round_battle_start_timestamp = pb_data.round_battle_start_timestamp();
	next_round_battle_start_timestamp = pb_data.next_round_battle_start_timestamp();

	return true;
}

bool WorldArenaSignUpInfo::Serialize(char* buffer, int buffer_size, int* out_length)
{
	bool ret = true;
	ret = WRITE_INT(uid)
		&& WRITE_INT(plat_type)
		&& WRITE_LL(uuid)
		&& WRITE_INT(profession)
		&& WRITE_SHORT(avatar_type)
		&& WRITE_SHORT(headshot_id)
		&& WRITE_SHORT(level)
		&& WRITE_SHORT(season_index)
		&& WRITE_INT(top_level)
		&& WRITE_STRN(role_name, sizeof(role_name))
		&& WRITE_STRN(guild_name, sizeof(guild_name));

	return ret;
}

bool WorldArenaSignUpInfo::Deserialize(const char* data, int data_length, int* out_offset)
{
	bool ret = true;
	ret = READ_INT(uid)
		&& READ_INT(plat_type)
		&& READ_LL(uuid)
		&& READ_INT(profession)
		&& READ_SHORT(avatar_type)
		&& READ_SHORT(headshot_id)
		&& READ_SHORT(level)
		&& READ_SHORT(season_index)
		&& READ_INT(top_level)
		&& READ_STRN(role_name, sizeof(role_name), sizeof(role_name))
		&& READ_STRN(guild_name, sizeof(guild_name), sizeof(guild_name));

	return ret;
}

bool WorldArenaSignUpInfo::GetPBData(ARG_OUT PB_WorldArenaUserSignupData* out_data) const
{
	out_data->set_uid(uid);
	out_data->set_plat_type(plat_type);
	out_data->set_uuid(uuid);
	out_data->set_profession(profession);
	out_data->set_avatar_type(avatar_type);
	out_data->set_headshot_id(headshot_id);
	out_data->set_level(level);
	out_data->set_season_index(season_index);
	out_data->set_top_level(top_level);
	out_data->set_role_name(role_name);
	out_data->set_guild_name(guild_name);

	return true;
}

bool WorldArenaSignUpInfo::SetDataFromPB(const PB_WorldArenaUserSignupData& pb_data)
{
	uid = pb_data.uid();
	plat_type = pb_data.plat_type();
	uuid = pb_data.uuid();
	profession = pb_data.profession();
	avatar_type = pb_data.avatar_type();
	headshot_id = pb_data.headshot_id();
	level = pb_data.level();
	season_index = pb_data.season_index();
	top_level = pb_data.top_level();
	F_STRNCPY(role_name, pb_data.role_name().c_str(), sizeof(role_name));
	F_STRNCPY(guild_name, pb_data.guild_name().c_str(), sizeof(guild_name));

	return true;
}

bool WorldArenaScoreInfo::GetPBData(ARG_OUT PB_WorldArenaUserScoreData* out_data) const
{
	out_data->set_win_num(win_num);
	out_data->set_lose_num(lose_num);
	out_data->set_draw_num(draw_num);
	out_data->set_giveup_num(giveup_num);
	out_data->set_total_score(total_score);
	out_data->set_win_box_reward_flag(win_box_reward_flag);
	out_data->set_total_battle_round(total_battle_round);
	out_data->set_has_fetch_signup_reward(has_fetch_signup_reward > 0);
	out_data->set_daily_reward_fetch_flag(daily_reward_fetch_flag);

	return true;
}

bool WorldArenaScoreInfo::SetDataFromPB(const PB_WorldArenaUserScoreData& pb_data)
{
	win_num = pb_data.win_num();
	lose_num = pb_data.lose_num();
	draw_num = pb_data.draw_num();
	giveup_num = pb_data.giveup_num();
	total_score = pb_data.total_score();
	win_box_reward_flag = pb_data.win_box_reward_flag();
	total_battle_round = pb_data.total_battle_round();
	has_fetch_signup_reward = pb_data.has_fetch_signup_reward() ? 1 : 0;
	daily_reward_fetch_flag = static_cast<char>(pb_data.daily_reward_fetch_flag());

	return true;
}

bool WorldArenaUserScheduleInfo::GetPBData(ARG_OUT PB_WorldArenaUserScheduleData* out_data) const
{
	out_data->set_group_id(group_id);
	out_data->mutable_schedule_list()->Clear();
	out_data->mutable_schedule_list()->Reserve(WORLD_ARENA_ROUND_NUM);
	for (int i = 0; i < WORLD_ARENA_ROUND_NUM && i < ARRAY_ITEM_COUNT(schedule_list); ++i)
	{
		out_data->add_schedule_list(schedule_list[i]);
	}

	return true;
}

bool WorldArenaUserScheduleInfo::SetDataFromPB(const PB_WorldArenaUserScheduleData& pb_data)
{
	group_id = pb_data.group_id();
	memset(schedule_list, 0, sizeof(schedule_list));
	assert(ARRAY_ITEM_COUNT(schedule_list) == pb_data.schedule_list_size());
	for (int i = 0; i < pb_data.schedule_list_size() && i < ARRAY_ITEM_COUNT(schedule_list); ++i)
	{
		schedule_list[i] = pb_data.schedule_list(i);
	}

	return true;
}

bool WorldArenaUserInfo::GetPBData(ARG_OUT PB_WorldArenaUserData* out_data) const
{
	bool ret = true;
	ret = ret && signup_info.GetPBData(out_data->mutable_signup_data());
	ret = ret && score_info.GetPBData(out_data->mutable_score_data());
	ret = ret && schedule_info.GetPBData(out_data->mutable_schedule_data());
	ret = ret && battle_data.GetPBData(out_data->mutable_role_battle_data());

	return ret;
}

bool WorldArenaUserInfo::SetDataFromPB(const PB_WorldArenaUserData& pb_data)
{
	bool ret = true;
	ret = ret && signup_info.SetDataFromPB(pb_data.signup_data());
	ret = ret && schedule_info.SetDataFromPB(pb_data.schedule_data());
	ret = ret && score_info.SetDataFromPB(pb_data.score_data());
	ret = ret && battle_data.SetDataFromPB(pb_data.role_battle_data());

	return ret;
}

bool WorldArenaUserInfo::Serialize(TLVSerializer &outstream) const
{
	PB_WorldArenaUserData* pb_data = (PB_WorldArenaUserData*)ProtobufMgr::Instance()->GetProtoMessage(PROTO_WORLD_ARENA_USER_DATA);
	if (NULL == pb_data) return false;

	if (!this->GetPBData(pb_data)) return false;

	{
		int data_length;
		char data_buffer[sizeof(WorldArenaUserInfo) * 2] = { 0 };
		if (!pb_data->SerializeToArray(data_buffer, sizeof(data_buffer)))
		{
			assert(0);
			return false;
		}
		data_length = pb_data->GetCachedSize();

		if (!outstream.Push(data_length)) return false;

		TLVSerializer serializer;
		serializer.Reset(data_buffer, data_length);
		serializer.MoveCurPos(data_length);
		if (!outstream.Push(serializer)) return false;
	}

	return true;
}

bool WorldArenaUserInfo::Unserialize(TLVUnserializer &instream)
{
	int data_length = 0;
	char data_buffer[sizeof(WorldArenaUserInfo) * 2] = { 0 };

	if (!instream.Pop(&data_length)) return false;

	{
		TLVUnserializer data_uns;
		if (!instream.Pop(&data_uns)) return false;

		if (data_uns.Size() > sizeof(data_buffer)) return false;

		memcpy(data_buffer, data_uns.Ptr(), data_uns.Size());
		data_length = static_cast<int>(data_uns.Size());

		PB_WorldArenaUserData* pb_data = (PB_WorldArenaUserData*)ProtobufMgr::Instance()->GetProtoMessage(PROTO_WORLD_ARENA_USER_DATA);
		if (NULL == pb_data) return false;

		if (!pb_data->ParseFromArray(data_buffer, data_length)) return false;

		if (!this->SetDataFromPB(*pb_data)) return false;
	}

	return true;
}

bool WorldArenaUserListParamForInit::Serialize(TLVSerializer &outstream) const
{
	bool ret = outstream.Push(next_id_from)
		&& outstream.Push(static_cast<int>(user_list.size()));
	if (!ret) return false;

	for (int i = 0; i < static_cast<int>(user_list.size()); ++i)
	{		
		if (!user_list[i].Serialize(outstream)) return false;
	}

	return true;
}

bool WorldArenaUserListParamForInit::Unserialize(TLVUnserializer &instream)
{
	int user_num = 0;
	bool ret = instream.Pop(&next_id_from)
		&& instream.Pop(&user_num);
	if (!ret) return false;

	user_list.reserve(user_num);
	for (int i = 0; i < user_num; ++i)
	{
		user_list.push_back(WorldArenaUserInfo());
		WorldArenaUserInfo& info = user_list[user_list.size() - 1];

		if (!info.Unserialize(instream)) return false;
	}

	return true;
}

bool WorldArenaScheduleListParamForInit::Serialize(TLVSerializer &outstream) const
{
	bool ret = outstream.Push(next_id_from)
		&& outstream.Push(static_cast<int>(schedule_list.size()));
	if (!ret) return false;

	for (int i = 0; i < static_cast<int>(schedule_list.size()); ++i)
	{
		if (!schedule_list[i].Serialize(outstream)) return false;
	}

	return true;
}

bool WorldArenaScheduleListParamForInit::Unserialize(TLVUnserializer &instream)
{
	int schedule_num = 0;
	bool ret = instream.Pop(&next_id_from)
		&& instream.Pop(&schedule_num);
	if (!ret) return false;

	schedule_list.reserve(schedule_num);
	for (int i = 0; i < schedule_num; ++i)
	{
		schedule_list.push_back(WorldArenaScheduleInfo());
		WorldArenaScheduleInfo& info = schedule_list[schedule_list.size() - 1];

		if (!info.Unserialize(instream)) return false;
	}

	return true;
}

bool WorldArenaScheduleInfo::GetPBData(ARG_OUT PB_WorldArenaScheduleData* out_data) const
{
	out_data->set_group_id(group_id);
	out_data->set_schedule_id(schedule_id);
	out_data->set_attacker_uuid(attacker_uuid);
	out_data->set_defender_uuid(defender_uuid);
	out_data->set_round_index(round_index);
	out_data->set_fight_result(fight_result);
	out_data->set_check_result_timestamp(check_result_timestamp);

	return true;
}

bool WorldArenaScheduleInfo::SetDataFromPB(const PB_WorldArenaScheduleData& pb_data)
{
	group_id = pb_data.group_id();
	schedule_id = pb_data.schedule_id();
	attacker_uuid = pb_data.attacker_uuid();
	defender_uuid = pb_data.defender_uuid();
	round_index = pb_data.round_index();
	fight_result = pb_data.fight_result();
	check_result_timestamp = pb_data.check_result_timestamp();

	return true;
}

bool WorldArenaScheduleInfo::Serialize(TLVSerializer &outstream) const
{
	PB_WorldArenaScheduleData* pb_data = (PB_WorldArenaScheduleData*)ProtobufMgr::Instance()->GetProtoMessage(PROTO_WORLD_ARENA_SCHEDULE_DATA);
	if (NULL == pb_data) return false;

	if (!this->GetPBData(pb_data)) return false;

	{
		int data_length;
		char data_buffer[sizeof(WorldArenaUserInfo) * 2] = { 0 };
		if (!pb_data->SerializeToArray(data_buffer, sizeof(data_buffer)))
		{
			assert(0);
			return false;
		}
		data_length = pb_data->GetCachedSize();

		if (!outstream.Push(data_length)) return false;

		TLVSerializer serializer;
		serializer.Reset(data_buffer, data_length);
		serializer.MoveCurPos(data_length);
		if (!outstream.Push(serializer)) return false;
	}

	return true;
}

bool WorldArenaScheduleInfo::Unserialize(TLVUnserializer &instream)
{
	int data_length = 0;
	char data_buffer[sizeof(WorldArenaScheduleInfo) * 2] = { 0 };

	if (!instream.Pop(&data_length)) return false;

	{
		TLVUnserializer data_uns;
		if (!instream.Pop(&data_uns)) return false;

		if (data_uns.Size() > sizeof(data_buffer)) return false;

		memcpy(data_buffer, data_uns.Ptr(), data_uns.Size());
		data_length = static_cast<int>(data_uns.Size());

		PB_WorldArenaScheduleData* pb_data = (PB_WorldArenaScheduleData*)ProtobufMgr::Instance()->GetProtoMessage(PROTO_WORLD_ARENA_SCHEDULE_DATA);
		if (NULL == pb_data) return false;

		if (!pb_data->ParseFromArray(data_buffer, data_length)) return false;

		if (!this->SetDataFromPB(*pb_data)) return false;
	}

	return true;
}

bool WorldArenaUserListParamForSave::Serialize(TLVSerializer &outstream) const
{
	bool ret = outstream.Push(static_cast<int>(save_user_list.size()))
		&& outstream.Push(static_cast<int>(delete_user_list.size()));
	if (!ret) return false;

	for (int i = 0; i < static_cast<int>(save_user_list.size()); ++i)
	{
		if (!save_user_list[i].Serialize(outstream)) return false;
	}
	
	for (int i = 0; i < static_cast<int>(delete_user_list.size()); ++i)
	{
		if (!outstream.Push(delete_user_list[i])) return false;
	}

	return true;
}

bool WorldArenaUserListParamForSave::Unserialize(TLVUnserializer &instream)
{
	int update_user_num = 0;
	int delete_user_num = 0;
	bool ret = instream.Pop(&update_user_num)
		&& instream.Pop(&delete_user_num);
	if (!ret) return false;

	{
		save_user_list.clear();
		save_user_list.reserve(update_user_num);
		for (int i = 0; i < update_user_num; ++i)
		{
			save_user_list.push_back(WorldArenaUserInfo());
			WorldArenaUserInfo& info = save_user_list[save_user_list.size() - 1];

			if (!info.Unserialize(instream)) return false;
		}
	}

	{
		delete_user_list.clear();
		delete_user_list.reserve(delete_user_num);
		for (int i = 0; i < delete_user_num; ++i)
		{
			long long delete_uuid = 0;
			if (!instream.Pop(&delete_uuid)) return false;

			delete_user_list.push_back(delete_uuid);
		}
	}
	
	return true;
}

bool WorldArenaScheduleListParamForSave::Serialize(TLVSerializer &outstream) const
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

bool WorldArenaScheduleListParamForSave::Unserialize(TLVUnserializer &instream)
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
			save_schedule_list.push_back(WorldArenaScheduleInfo());
			WorldArenaScheduleInfo& info = save_schedule_list[save_schedule_list.size() - 1];

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
