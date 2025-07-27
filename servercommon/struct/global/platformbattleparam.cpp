#include "servercommon/struct/structcommon.h"

#include "platformbattleparam.hpp"

bool PlatformBattleParam::Serialize(TLVSerializer & outstream) const
{
	bool ret = outstream.Push(count);
	if (!ret)
	{
		return false;
	}

	for (int i = 0; i < count; ++i)
	{
		const PlatformBattleSaveEntry & data_item = data_list[i];

		ret = outstream.Push(data_item.change_state) && outstream.Push(data_item.inc_id) && outstream.Push(data_item.rank_item.platform_number) && outstream.Push(data_item.rank_item.platform_rank);
		if (!ret)
		{
			return false;
		}

		if (data_item.change_state == structcommon::CS_DELETE)
		{
			continue;
		}

		{
			TLVSerializer serializer;
			serializer.Reset((void*)data_item.rank_item.role_data_buffer, sizeof(data_item.rank_item.role_data_buffer));
			serializer.MoveCurPos(data_item.rank_item.role_data_buffer_length);
			ret = outstream.Push(serializer);
		}

		if (!ret)
		{
			return false;
		}
	}

	return true;
}

bool PlatformBattleParam::Unserialize(TLVUnserializer & instream)
{
	bool ret = instream.Pop(&count);

	if (!ret)
	{
		return false;
	}

	for (int i = 0; i < count && i < PLATFORM_BATTLE_RANK_ITEM_MAX; ++i)
	{
		PlatformBattleSaveEntry & data_item = data_list[i];

		ret = instream.Pop(&data_item.change_state) && instream.Pop(&data_item.inc_id) && instream.Pop(&data_item.rank_item.platform_number) && instream.Pop(&data_item.rank_item.platform_rank);
		if (!ret)
		{
			return false;
		}

		if (data_item.change_state == structcommon::CS_DELETE)
		{
			continue;
		}

		{
			TLVUnserializer battle_data_uns;
			ret = instream.Pop(&battle_data_uns);
			if (!ret)
			{
				return false;
			}

			if (battle_data_uns.Size() <= sizeof(data_item.rank_item.role_data_buffer))
			{
				memcpy(data_item.rank_item.role_data_buffer, battle_data_uns.Ptr(), battle_data_uns.Size());
				data_item.rank_item.role_data_buffer_length = static_cast<int>(battle_data_uns.Size());
			}
			else
			{
				return false;
			}
		}

	}

	return true;
}
