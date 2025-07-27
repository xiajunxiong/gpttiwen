#include "battle_video_def.hpp"

bool BattleVideoListParam::Serialize(TLVSerializer &outstream) const
{
	bool ret = outstream.Push(count) && outstream.Push(next_id);
	if (!ret)
	{
		return false;
	}

	for (int i = 0; i < count && i < MAX_VIDEO_NUM; ++i)
	{
		ret = outstream.Push(video_list[i].change_state)
			&& outstream.Push(video_list[i].id)
			&& outstream.Push(video_list[i].param.average_score)
			&& outstream.Push(video_list[i].param.battle_mode)
			&& outstream.Push(video_list[i].param.is_server_first_skill)
			&& outstream.Push(video_list[i].param.monster_group_id)
			&& outstream.Push(video_list[i].param.record_timestamp)
			&& outstream.Push(video_list[i].param.version)
			&& outstream.Push(video_list[i].param.video_file_id);

		if (!ret)
		{
			return false;
		}
	}

	return ret;
}

bool BattleVideoListParam::Unserialize(TLVUnserializer &instream)
{
	bool ret = instream.Pop(&count) && instream.Pop(&next_id);
	if (!ret)
	{
		return false;
	}

	for (int i = 0; i < count && i < MAX_VIDEO_NUM; ++i)
	{
		ret = instream.Pop(&video_list[i].change_state)
			&& instream.Pop(&video_list[i].id)
			&& instream.Pop(&video_list[i].param.average_score)
			&& instream.Pop(&video_list[i].param.battle_mode)
			&& instream.Pop(&video_list[i].param.is_server_first_skill)
			&& instream.Pop(&video_list[i].param.monster_group_id)
			&& instream.Pop(&video_list[i].param.record_timestamp)
			&& instream.Pop(&video_list[i].param.version)
			&& instream.Pop(&video_list[i].param.video_file_id);

		if (!ret)
		{
			return false;
		}
	}

	return ret;
}