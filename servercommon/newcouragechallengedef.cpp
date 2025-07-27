#include "newcouragechallengedef.hpp"
#include "common/tlvprotocol.h"

bool NewCourageChallengeLevelParamList::Serialize(TLVSerializer & outstream) const
{
	bool ret = outstream.Push(count);
	if (!ret)
	{
		return false;
	}

	for (int i = 0; i < count && i < MAX_NEW_COURAGE_CHALLENGE_LIST_NUM; ++i)
	{
		ret = outstream.Push(data_list[i].change_state)
			&& outstream.Push(data_list[i].info.layer_level)
			&& outstream.Push(data_list[i].info.level_seq)
			&& outstream.Push(data_list[i].info.flag);

		if (!ret)
		{
			return false;
		}
	}

	return ret;
}

bool NewCourageChallengeLevelParamList::Unserialize(TLVUnserializer & instream)
{
	bool ret = instream.Pop(&count);
	if (!ret)
	{
		return false;
	}

	for (int i = 0; i < count && i < MAX_NEW_COURAGE_CHALLENGE_LIST_NUM; ++i)
	{
		ret = instream.Pop(&data_list[i].change_state)
			&& instream.Pop(&data_list[i].info.layer_level)
			&& instream.Pop(&data_list[i].info.level_seq)
			&& instream.Pop(&data_list[i].info.flag);

		if (!ret)
		{
			return false;
		}
	}

	return ret;
}
