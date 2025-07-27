#include "serverfirstkilldef.h"
#include "servercommon/struct/structcommon.h"

bool ServerFirstKillParam::Serialize(TLVSerializer & outstream) const
{
	bool ret = true;
	for (int i = 0; i < FIRST_KILL_TYPE_MAX; ++i)
	{
		for (int k = 0; k < FIRST_KILL_BOSS_TYPE_MAX_NUM; k++)
		{
			const ServerFirstKillSaveEntry & data_item = data_list[i][k];

			ret = outstream.Push(data_item.id) && outstream.Push(data_item.change_state);
			if (!ret)
			{
				return false;
			}
			const ServerFirstKillEntry &data_info = data_item.first_kill_info;
			ret = outstream.Pushf(
				"hhhhhh"
				"iikkk",
				data_info.type, data_info.level_group, data_info.seq, data_info.member_count, data_info.round_num, data_info.kill_flag,
				data_info.server_id, data_info.monster_group_id, data_info.kill_timestamp, data_info.kill_game_timestamp, data_info.battle_video_record_file_id
			);
			if (!ret) return false;


			for (int j = 0; j < MEMBER_NAME_MAX_NUM && j < data_item.first_kill_info.member_count; j++)
			{
				const char *user_name_tmp = data_item.first_kill_info.member_name[j];
				ret = outstream.Push(data_item.first_kill_info.uid[j]) && outstream.Push(user_name_tmp);
				if (!ret) return false;
			}


			TLVSerializer role_data;
			role_data.Reset((char *)&data_item.first_kill_info.reward_data, sizeof(data_item.first_kill_info.reward_data));
			role_data.MoveCurPos(sizeof(data_item.first_kill_info.reward_data));
			ret = outstream.Push(role_data);

			if (!ret)
			{
				return false;
			}
		}
	}

	return true;
}

bool ServerFirstKillParam::Unserialize(TLVUnserializer & instream)
{
	bool ret = true;

	for (int i = 0; i < FIRST_KILL_TYPE_MAX; ++i)
	{
		for (int k = 0; k < FIRST_KILL_BOSS_TYPE_MAX_NUM; k++)
		{

			ServerFirstKillSaveEntry & data_item = data_list[i][k];

			ret = instream.Pop(&data_item.id) && instream.Pop(&data_item.change_state);
			if (!ret) return false;

			ServerFirstKillEntry &data_info = data_item.first_kill_info;
			ret = instream.Popf(
				"hhhhhh"
				"iikkk",
				&data_info.type, &data_info.level_group, &data_info.seq, &data_info.member_count, &data_info.round_num, &data_info.kill_flag,
				&data_info.server_id, &data_info.monster_group_id, &data_info.kill_timestamp, &data_info.kill_game_timestamp, &data_info.battle_video_record_file_id
			);
			if (!ret) return false;

			for (int j = 0; j < MEMBER_NAME_MAX_NUM && j < data_item.first_kill_info.member_count; j++)
			{
				const char *user_name_tmp = NULL;
				ret = instream.Pop(&data_item.first_kill_info.uid[j]) && instream.Pop(&user_name_tmp);
				if (!ret) return false;

				STRNCPY((char *)data_item.first_kill_info.member_name[j], user_name_tmp, sizeof(GameName));
			}

			TLVUnserializer role_data;
			ret = instream.Pop(&role_data);
			if (!ret)
			{
				return false;
			}

			if (role_data.Size() <= sizeof(data_item.first_kill_info.reward_data))
			{
				memcpy((void *)&data_item.first_kill_info.reward_data, role_data.Ptr(), role_data.Size());
			}
		}
	}

	return true;
}
