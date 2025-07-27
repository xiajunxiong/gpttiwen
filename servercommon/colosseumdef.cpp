#include "colosseumdef.hpp"
#include "servercommon/struct/structcommon.h"

bool ChallengeUserListParam::Serialize(TLVSerializer & outstream) const
{
	bool ret = outstream.Push(count);
	if (!ret) return false;

	for (int i = 0; i < count && i < CHALLENGE_FIELD_ONCE_OPER_USER_COUNT; ++i)
	{
		const ChallengeUserListParam::ChallengeUserAttr & info = user_list[i];
		const char* name = info.name;
		ret = outstream.Push(info.change_state) &&
			outstream.Push(info.rankpos) &&
			outstream.Push(info.role_id) &&
			outstream.Push(info.today_fight_count) &&
			outstream.Push(info.fetch_exp_flag) &&
			outstream.Push(info.rank_award_fetch_flag) &&
			outstream.Push(info.level) &&
			outstream.Push(info.profession) &&
			outstream.Push(info.avatar_type) &&
			outstream.Push(name) && 
			outstream.Push(info.highest_rank) &&
			outstream.Push(info.colosseum_clear_check) &&
			outstream.Push(info.total_capability)
			;

		if (!ret)
		{
			return false;
		}

		if (info.change_state == structcommon::CS_DELETE) continue;

		{
			TLVSerializer serializer;
			serializer.Reset((void*)info.battle_data_buff, sizeof(info.battle_data_buff));
			serializer.MoveCurPos(info.battle_data_length);
			ret = outstream.Push(serializer);
		}

		{
			TLVSerializer appearance_data;
			appearance_data.Reset((void*)&info.appearance, sizeof(info.appearance));
			appearance_data.MoveCurPos(sizeof(info.appearance));
			ret = outstream.Push(appearance_data);
		}

		{
			TLVSerializer colosseum_data;
			colosseum_data.Reset((void*)&info.colosseum_param, sizeof(info.colosseum_param));
			colosseum_data.MoveCurPos(sizeof(info.colosseum_param));
			ret = outstream.Push(colosseum_data);
		}

		if (!ret)
		{
			return false;
		}
	}

	ret = outstream.Push(next_id_from);
	if (!ret) return false;

	return true;
}

bool ChallengeUserListParam::Unserialize(TLVUnserializer & instream)
{
	bool ret = instream.Pop(&count);
	if (!ret) return false;

	for (int i = 0; i < count && i < CHALLENGE_FIELD_ONCE_OPER_USER_COUNT; ++i)
	{
		ChallengeUserListParam::ChallengeUserAttr & info = user_list[i];
		const char* name = NULL;
		ret = instream.Pop(&info.change_state)
			&& instream.Pop(&info.rankpos)
			&& instream.Pop(&info.role_id)
			&& instream.Pop(&info.today_fight_count)
			&& instream.Pop(&info.fetch_exp_flag)
			&& instream.Pop(&info.rank_award_fetch_flag)
			&& instream.Pop(&info.level)
			&& instream.Pop(&info.profession)
			&& instream.Pop(&info.avatar_type)
			&& instream.Pop(&name)
			&& instream.Pop(&info.highest_rank)
			&& instream.Pop(&info.colosseum_clear_check)
			&& instream.Pop(&info.total_capability)
			;

		if (!ret)
		{
			return false;
		}

		if (user_list[i].change_state == structcommon::CS_DELETE) continue;

		F_STRNCPY(info.name, name, sizeof(GameName));

		{
			TLVUnserializer battle_data_uns;
			ret = instream.Pop(&battle_data_uns);
			if (!ret)
			{
				return false;
			}

			if (battle_data_uns.Size() <= sizeof(user_list[i].battle_data_buff))
			{
				memcpy(user_list[i].battle_data_buff, battle_data_uns.Ptr(), battle_data_uns.Size());
				user_list[i].battle_data_length = static_cast<int>(battle_data_uns.Size());
			}
			else
			{
				return false;
			}
		}

		{
			TLVUnserializer appearance_data;
			ret = instream.Pop(&appearance_data);
			if (!ret)
			{
				return false;
			}

			if (appearance_data.Size() <= sizeof(user_list[i].appearance))
			{
				memcpy(&user_list[i].appearance, appearance_data.Ptr(), appearance_data.Size());
			}
			else
			{
				return false;
			}
		}

		{
			TLVUnserializer colosseum_data;
			ret = instream.Pop(&colosseum_data);
			if (!ret)
			{
				return false;
			}

			if (colosseum_data.Size() <= sizeof(user_list[i].colosseum_param))
			{
				memcpy(&user_list[i].colosseum_param, colosseum_data.Ptr(), colosseum_data.Size());
			}
			else
			{
				return false;
			}
		}
	}

	ret = instream.Pop(&next_id_from);
	if (!ret) return false;

	return true;
}