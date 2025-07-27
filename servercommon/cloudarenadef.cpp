#include "cloudarenadef.hpp"
#include "servercommon/struct/structcommon.h"
#include "servercommon/protobuf/cloud_arena_fight_record_user_info.pb.h"
#include "servercommon/protobuf/cloud_arena_fight_record.pb.h"

bool CloudArenaParam::Serialize(TLVSerializer& outstream) const
{
	bool ret = outstream.Push(season_info.season_timestamp)
		&& outstream.Push(season_info.season_end_timestamp)
		&& outstream.Push(season_info.hidden_server_idx)
		&& outstream.Push(other_info.defense_buff_id)
		&& outstream.Push(season_info.has_release_season_reward)
		&& outstream.Push(season_info.season_idx);

	return ret;
}

bool CloudArenaParam::UnSerialize(TLVUnserializer& instream)
{
	bool ret = instream.Pop(&season_info.season_timestamp)
		&& instream.Pop(&season_info.season_end_timestamp)
		&& instream.Pop(&season_info.hidden_server_idx)
		&& instream.Pop(&other_info.defense_buff_id)
		&& instream.Pop(&season_info.has_release_season_reward)
		&& instream.Pop(&season_info.season_idx);
	
	return ret;
}

bool CloudArenaUserListParam::Serialize(TLVSerializer &outstream) const
{
	bool ret = outstream.Push(next_id_from)
		&& outstream.Push(count);
	if (!ret) return false;

	for (int i = 0; i < count && i < ARRAY_ITEM_COUNT(user_list); ++i)
	{
		const CloudArenaUserData& data = user_list[i];
		const char* name = data.name;
		ret = outstream.Push(data.change_state) &&
			outstream.Push(data.uid) &&
			outstream.Push(data.level) &&
			outstream.Push(data.avatar) &&
			outstream.Push(data.profession) &&
			outstream.Push(name) &&
			outstream.Push(data.capability) &&
			outstream.Push(data.score) &&
			outstream.Push(data.remain_fight_times) &&
			outstream.Push(data.target_uid_list[0]) &&
			outstream.Push(data.target_uid_list[1]) &&
			outstream.Push(data.target_uid_list[2]) &&
			outstream.Push(data.target_uid_list[3]) &&
			outstream.Push(data.today_buy_fighttimes_times) &&
			outstream.Push(data.next_recover_fighttimes_timtstamp) &&
			outstream.Push(data.battle_data_length) &&
			outstream.Push(data.fight_record_head) &&
			outstream.Push(data.fight_record_tail) &&
			outstream.Push(data.fight_record_data_length) &&
			outstream.Push(data.reach_score_timestamp) &&
			outstream.Push(data.last_season_rank) &&
			outstream.Push(data.fetch_ranking_reward_flag) &&
			outstream.Push(data.reach_ranking_flag) &&
			outstream.Push(data.season_final_rank) &&
			outstream.Push(data.season_initial_score) &&
			outstream.Push(data.day_reward_status) &&
			outstream.Push(data.season_timestamp) &&
			outstream.Push(data.hidden_server_idx) &&
			outstream.Push(data.season_highest_dan_id) &&
			outstream.Push(data.season_highest_rank) &&
			outstream.Push(data.last_day_score);

		UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(data.target_uid_list) >= 4);
		
		if (!ret) return false;

		if (data.change_state == structcommon::CS_DELETE)
		{
			continue;
		}
		
		{
			TLVSerializer serializer;
			serializer.Reset((void*)&data.appearance, sizeof(data.appearance));
			serializer.MoveCurPos(sizeof(data.appearance));
			ret = outstream.Push(serializer);
			if (!ret) return false;
		}
		
		{
			TLVSerializer serializer;
			serializer.Reset((void*)data.battle_data_buff, sizeof(data.battle_data_buff));
			serializer.MoveCurPos(data.battle_data_length);
			ret = outstream.Push(serializer);
			if (!ret) return false;
		}
		
		{
			TLVSerializer serializer;
			serializer.Reset((void*)data.fight_record_data_buff, sizeof(data.fight_record_data_buff));
			serializer.MoveCurPos(data.fight_record_data_length);
			ret = outstream.Push(serializer);
			if (!ret) return false;
		}

		{
			TLVSerializer serializer;
			serializer.Reset((void*)&data.achievement_param, sizeof(data.achievement_param));
			serializer.MoveCurPos(sizeof(data.achievement_param));
			ret = outstream.Push(serializer);
			if (!ret) return false;
		}

		{
			TLVSerializer serializer;
			serializer.Reset((void*)&data.today_challenge_list, sizeof(data.today_challenge_list));
			serializer.MoveCurPos(sizeof(data.today_challenge_list));
			ret = outstream.Push(serializer);
			if (!ret) return false;
		}

		{
			TLVSerializer serializer;
			serializer.Reset((void*)&data.other_param, sizeof(data.other_param));
			serializer.MoveCurPos(sizeof(data.other_param));
			ret = outstream.Push(serializer);
			if (!ret) return false;
		}
	}

	return true;
}

bool CloudArenaUserListParam::Unserialize(TLVUnserializer &instream)
{
	bool ret = instream.Pop(&next_id_from)
		&& instream.Pop(&count);
	if (!ret) return false;

	for (int i = 0; i < count && i < ARRAY_ITEM_COUNT(user_list); ++i)
	{
		CloudArenaUserData& data = user_list[i];
		const char* name = 0;
		ret = instream.Pop(&data.change_state) &&
			instream.Pop(&data.uid) &&
			instream.Pop(&data.level) &&
			instream.Pop(&data.avatar) &&
			instream.Pop(&data.profession) &&
			instream.Pop(&name) &&
			instream.Pop(&data.capability) &&
			instream.Pop(&data.score) &&
			instream.Pop(&data.remain_fight_times) &&
			instream.Pop(&data.target_uid_list[0]) &&
			instream.Pop(&data.target_uid_list[1]) &&
			instream.Pop(&data.target_uid_list[2]) &&
			instream.Pop(&data.target_uid_list[3]) &&
			instream.Pop(&data.today_buy_fighttimes_times) &&
			instream.Pop(&data.next_recover_fighttimes_timtstamp) &&
			instream.Pop(&data.battle_data_length) &&
			instream.Pop(&data.fight_record_head) &&
			instream.Pop(&data.fight_record_tail) &&
			instream.Pop(&data.fight_record_data_length) &&
			instream.Pop(&data.reach_score_timestamp) &&
			instream.Pop(&data.last_season_rank) &&
			instream.Pop(&data.fetch_ranking_reward_flag) &&
			instream.Pop(&data.reach_ranking_flag) &&
			instream.Pop(&data.season_final_rank) &&
			instream.Pop(&data.season_initial_score) &&
			instream.Pop(&data.day_reward_status) &&
			instream.Pop(&data.season_timestamp) &&
			instream.Pop(&data.hidden_server_idx) &&
			instream.Pop(&data.season_highest_dan_id) &&
			instream.Pop(&data.season_highest_rank) &&
			instream.Pop(&data.last_day_score);

		UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(data.target_uid_list) >= 4);

		F_STRNCPY(data.name, name, sizeof(data.name));

		if (!ret) return false;

		if (data.change_state == structcommon::CS_DELETE)
		{
			continue;
		}

		{
			TLVUnserializer unserializer;
			ret = instream.Pop(&unserializer);
			if (!ret) return false;

			if (unserializer.Size() <= sizeof(data.appearance))
			{
				memcpy(&data.appearance, unserializer.Ptr(), unserializer.Size());
			}
			else
			{
				return false;
			}
		}

		{
			TLVUnserializer battle_data_uns;
			ret = instream.Pop(&battle_data_uns);
			if (!ret)
			{
				return false;
			}

			if (battle_data_uns.Size() <= sizeof(data.battle_data_buff))
			{
				memcpy(data.battle_data_buff, battle_data_uns.Ptr(), battle_data_uns.Size());
				data.battle_data_length = static_cast<int>(battle_data_uns.Size());
			}
			else
			{
				return false;
			}
		}

		{
			TLVUnserializer fight_record_uns;
			ret = instream.Pop(&fight_record_uns);
			if (!ret)
			{
				return false;
			}

			if (fight_record_uns.Size() <= sizeof(data.fight_record_data_buff))
			{
				memcpy(data.fight_record_data_buff, fight_record_uns.Ptr(), fight_record_uns.Size());
				data.fight_record_data_length = static_cast<int>(fight_record_uns.Size());
			}
			else
			{
				return false;
			}
		}

		{
			TLVUnserializer unserializer;
			ret = instream.Pop(&unserializer);
			if (!ret) return false;

			if (unserializer.Size() <= sizeof(data.achievement_param))
			{
				memcpy(&data.achievement_param, unserializer.Ptr(), unserializer.Size());
			}
			else
			{
				return false;
			}
		}

		{
			TLVUnserializer unserializer;
			ret = instream.Pop(&unserializer);
			if (!ret) return false;

			if (unserializer.Size() <= sizeof(data.today_challenge_list))
			{
				memcpy(&data.today_challenge_list, unserializer.Ptr(), unserializer.Size());
			}
			else
			{
				return false;
			}
		}

		{
			TLVUnserializer unserializer;
			ret = instream.Pop(&unserializer);
			if (!ret) return false;

			if (unserializer.Size() <= sizeof(data.other_param))
			{
				memcpy(&data.other_param, unserializer.Ptr(), unserializer.Size());
			}
			else
			{
				return false;
			}
		}
	}

	return true;
}

bool CloudArenaFightRecordUserInfo::GetPBData(ARG_OUT PB_CloudArenaFightRecordUserInfo* out_data) const
{
	out_data->set_uid(uid);
	out_data->set_level(level);
	out_data->set_profession(profession);
	out_data->set_avatar(avatar);
	out_data->set_headshot_id(headshot_id);
	out_data->set_name(name);
	out_data->set_change_score(change_score);
	out_data->set_change_arena_coin(change_arena_coin);
	out_data->set_new_rank(new_rank);
	out_data->set_old_rank(old_rank);
	out_data->set_old_score(old_score);

	return true;
}

bool CloudArenaFightRecordUserInfo::SetDataFromPB(const PB_CloudArenaFightRecordUserInfo& pb_data)
{
	uid = pb_data.uid();
	level = pb_data.level();
	profession = pb_data.profession();
	avatar = pb_data.avatar();
	headshot_id = pb_data.headshot_id();
	F_STRNCPY(name, pb_data.name().c_str(), sizeof(name));
	change_score = pb_data.change_score();
	change_arena_coin = pb_data.change_arena_coin();
	new_rank = pb_data.new_rank();
	old_rank = pb_data.old_rank();
	old_score = pb_data.old_score();

	return true;
}

bool CloudArenaFightRecord::GetPBData(ARG_OUT PB_CloudArenaFightRecord* out_data) const
{
	out_data->set_record_id(record_id);
	out_data->set_record_timestamp(record_timestamp);
	out_data->set_is_attacker_win(is_attacker_win);
	attacker_info.GetPBData(out_data->mutable_attacker_info());
	defender_info.GetPBData(out_data->mutable_defender_info());

	return true;
}

bool CloudArenaFightRecord::SetDataFromPB(const PB_CloudArenaFightRecord& pb_data)
{
	record_id = pb_data.record_id();
	record_timestamp = pb_data.record_timestamp();
	is_attacker_win = pb_data.is_attacker_win();
	attacker_info.SetDataFromPB(pb_data.attacker_info());
	defender_info.SetDataFromPB(pb_data.defender_info());

	return true;
}
