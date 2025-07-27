#include "shitudef.h"

bool ShiTuSeekingParam::Serialize(TLVSerializer & outstream) const
{
	bool ret = true;
	for (int i = 0; i < SHI_TU_SEEKING_MAX_COUNT; i++)
	{
		//if(apprentice_list[i].data.Invalid()) continue;

		const ShiTuSeekingEntry &data_item = apprentice_list[i];
		ret = outstream.Push(data_item.index) && outstream.Push(data_item.change_state);
		if (!ret)
		{
			return false;
		}

		const ShiTuSeeking & data_info = data_item.data;
		ret = outstream.Pushf(
			"iiii",
			data_info.uid, data_info.seeking_type, data_info.timestamp, data_info.end_timestamp
		);
		if (!ret) return false;	

		const char * seeking_notice = data_info.seeking_notice;
		ret = outstream.Push(seeking_notice);
		if (!ret) return false;
	}

	for (int i = 0; i < SHI_TU_SEEKING_MAX_COUNT; i++)
	{
		//if (master_list[i].data.Invalid()) continue;

		const ShiTuSeekingEntry &data_item = master_list[i];
		ret = outstream.Push(data_item.index) && outstream.Push(data_item.change_state);
		if (!ret)
		{
			return false;
		}

		const ShiTuSeeking & data_info = data_item.data;
		ret = outstream.Pushf(
			"iiii",
			data_info.uid, data_info.seeking_type, data_info.timestamp, data_info.end_timestamp
		);
		if (!ret) return false;

		const char * seeking_notice = data_info.seeking_notice;
		ret = outstream.Push(seeking_notice);
		if (!ret) return false;
	}


	return true;
}

bool ShiTuSeekingParam::Unserialize(TLVUnserializer & instream)
{
	int ret = true;
	for (int i = 0; i < SHI_TU_SEEKING_MAX_COUNT; i++)
	{
		ShiTuSeekingEntry &data_item = apprentice_list[i];
		ret = instream.Pop(&data_item.index) && instream.Pop(&data_item.change_state);
		if(!ret) return false; 

		ShiTuSeeking & data_info = data_item.data;
		ret = instream.Popf(
			"iiii",
			&data_info.uid, &data_info.seeking_type, &data_info.timestamp, &data_info.end_timestamp
			);
		if(!ret) return false;

		const char * seeking_notice = NULL;
		ret = instream.Pop(&seeking_notice);
		if (!ret) return false;
		STRNCPY(data_info.seeking_notice, seeking_notice, sizeof(data_info.seeking_notice));
	}
	for (int i = 0; i < SHI_TU_SEEKING_MAX_COUNT; i++)
	{
		ShiTuSeekingEntry &data_item = master_list[i];
		ret = instream.Pop(&data_item.index) && instream.Pop(&data_item.change_state);
		if (!ret) return false;

		ShiTuSeeking & data_info = data_item.data;
		ret = instream.Popf(
			"iiii",
			&data_info.uid, &data_info.seeking_type, &data_info.timestamp, &data_info.end_timestamp
		);
		if (!ret) return false;

		const char * seeking_notice = NULL;
		ret = instream.Pop(&seeking_notice);
		if (!ret) return false;
		STRNCPY(data_info.seeking_notice, seeking_notice, sizeof(data_info.seeking_notice));
	}

	return true;
}
