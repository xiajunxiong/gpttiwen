#include "gatherdreamdef.hpp"

bool GatherDreamRecordParam::Serialize(TLVSerializer & outstream) const
{
	bool ret = false;
	ret = outstream.Push((int)get_data_list.size());
	if (!ret)
	{
		return false;
	}
	for (std::map<int, std::vector<GatherDreamRecordSaveEntry> >::const_iterator it = get_data_list.begin(); it != get_data_list.end(); it++)
	{
		ret = outstream.Push((int)it->second.size()) && outstream.Push((int)it->first);
		if (!ret)
		{
			return false;
		}
		for (int i = 0; i < (int)it->second.size(); i++)
		{
			TLVSerializer gather_dream_record_data;
			gather_dream_record_data.Reset((char *)&it->second[i], sizeof(it->second[i]));
			gather_dream_record_data.MoveCurPos(sizeof(it->second[i]));
			ret = outstream.Push(gather_dream_record_data);

			if (!ret)
			{
				return false;
			}
		}
	}

	ret = outstream.Push((int)gift_data_list.size());
	if (!ret)
	{
		return false;
	}
	for (std::map<int, std::vector<GatherDreamRecordSaveEntry> >::const_iterator it = gift_data_list.begin(); it != gift_data_list.end(); it++)
	{
		ret = outstream.Push((int)it->second.size()) && outstream.Push((int)it->first);
		if (!ret)
		{
			return false;
		}
		
		for (int i = 0; i < (int)it->second.size(); i++)
		{
			TLVSerializer gather_dream_record_data;
			gather_dream_record_data.Reset((char *)&it->second[i], sizeof(it->second[i]));
			gather_dream_record_data.MoveCurPos(sizeof(it->second[i]));
			ret = outstream.Push(gather_dream_record_data);

			if (!ret)
			{
				return false;
			}

		}
	}	
	return true;
}

bool GatherDreamRecordParam::Unserialize(TLVUnserializer & instream)
{
	this->Reset();

	bool ret = false;
	int count = 0;
	ret = instream.Pop(&count);
	if (!ret)
	{
		return false;
	}
	int size = 0;
	int role_id = 0;
	for (int i = 0; i < count; i++)
	{
		ret = instream.Pop(&size) && instream.Pop(&role_id);
		if (!ret)
		{
			return false;
		}
		for (int k = 0; k < size; k++)
		{
			TLVUnserializer gather_dream_record_data;
			ret = instream.Pop(&gather_dream_record_data);
			if (!ret)
			{
				return false;
			}
			GatherDreamRecordSaveEntry data_info;
			if (gather_dream_record_data.Size() <= sizeof(GatherDreamRecordSaveEntry))
			{
				memcpy((void *)&data_info, gather_dream_record_data.Ptr(), gather_dream_record_data.Size());
			}
			get_data_list[role_id].push_back(data_info);
		}

	}

	ret = instream.Pop(&count);
	if (!ret)
	{
		return false;
	}
	for (int i = 0; i < count; i++)
	{
		ret = instream.Pop(&size) && instream.Pop(&role_id);
		if (!ret)
		{
			return false;
		}
		for (int k = 0; k < size; k++)
		{
			TLVUnserializer gather_dream_record_data;
			ret = instream.Pop(&gather_dream_record_data);
			if (!ret)
			{
				return false;
			}
			GatherDreamRecordSaveEntry data_info;
			if (gather_dream_record_data.Size() <= sizeof(GatherDreamRecordSaveEntry))
			{
				memcpy((void *)&data_info, gather_dream_record_data.Ptr(), gather_dream_record_data.Size());
			}
			gift_data_list[role_id].push_back(data_info);
		}
	}

	return true;
}

void GatherDreamRecordInfo::SetData(const SynGatherDreamRecord info)
{
	F_STRNCPY(name, info.name, sizeof(name));
	headshot_id = info.headshot_id;
	post = info.post;
	avatar_type = info.avatar_type;
}
