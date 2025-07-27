#include "animalracedef.hpp"
#include "servercommon/struct/structcommon.h"


bool AnimalRaceParam::Serialize(TLVSerializer & outstream) const
{
	bool ret = true;

	ret = outstream.Push(count);
	if (!ret) return false;

	for (int i = 0; i < (int)data_list.size() && i < count; i++)
	{
		const AnimalRaceSaveEntry & data_item = data_list[i];
		ret = outstream.Push(data_item.id) && outstream.Push(data_item.change_state);
		if (!ret)
		{
			return false;
		}
		const AnimalRaceEntry & data_info = data_item.animal_race_info;
		ret = outstream.Pushf(
			"iiiik",
			data_info.server_id, data_info.date_id, data_info.win_numner, data_info.pirze_pool, data_info.time_stamp
		);
		if (!ret) return false;

		for (int j = 0; j < MAX_ANIMAL_RACE_NUM; j++)
		{
			ret = outstream.Push(data_info.animal_race_num[j]) && outstream.Push(data_info.response_rate_num[j]);
			if (!ret) return false;
		}

		TLVSerializer data;
		data.Reset((void *)&data_info.animation_data, sizeof(data_info.animation_data));
		data.MoveCurPos(sizeof(data_info.animation_data));
		ret = outstream.Push(data);

		if (!ret) return false;
	}

	return true;
}

bool AnimalRaceParam::Unserialize(TLVUnserializer & instream)
{
	bool ret = true;
	std::vector<AnimalRaceSaveEntry>().swap(data_list);

	ret = instream.Pop(&count);
	if (!ret) return false;

	for (int i = 0; i < count; i++)
	{
		AnimalRaceSaveEntry data_item;
		ret = instream.Pop(&data_item.id) && instream.Pop(&data_item.change_state);
		if(!ret) return false;

		AnimalRaceEntry & data_info = data_item.animal_race_info;
		ret = instream.Popf(
			"iiiik",
			&data_info.server_id, &data_info.date_id, &data_info.win_numner, &data_info.pirze_pool, &data_info.time_stamp
		);
		if (!ret) return false;

		for (int j = 0; j < MAX_ANIMAL_RACE_NUM; j++)
		{
			ret = instream.Pop(&data_info.animal_race_num[j]) && instream.Pop(&data_info.response_rate_num[j]);
			if (!ret) return false;
		}

		TLVUnserializer data;
		ret = instream.Pop(&data);
		if (!ret)
		{
			return false;
		}

		if (data.Size() <= sizeof(data_info.animation_data))
		{
			memcpy((void *)&data_info.animation_data, data.Ptr(), data.Size());
		}

		if ((int)data_list.size() > i)
		{
			data_list[i] = data_item;
		}
		else
		{
			data_list.push_back(data_item);
		}
	}

	return true;
}
