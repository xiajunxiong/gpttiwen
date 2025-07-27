#include "rapaimaihangparam.hpp"

bool RARoleParMaiHangDBParam::Serialize(TLVSerializer & outstream) const
{
	bool ret = true;
	ret = outstream.Push((int)data_list.size());
	if (!ret)
	{
		return false;
	}
	for (std::map<int, RARoleMaiHangDBItem>::const_iterator it = data_list.begin(); it != data_list.end(); it++)
	{
		ret = outstream.Push(it->first);
		if (!ret)
		{
			return false;
		}
		ret = outstream.Push(it->second.change_state) && outstream.Push(it->second.record_count);
		if (!ret)
		{
			return false;
		}

		TLVSerializer record_data;
		record_data.Reset((char *)&it->second.record_list, sizeof(it->second.record_list));
		record_data.MoveCurPos(sizeof(it->second.record_list));
		ret = outstream.Push(record_data);
		if (!ret)
		{
			return false;
		}
	}
	return ret;
}

bool RARoleParMaiHangDBParam::Unserialize(TLVUnserializer & instream)
{
	std::map<int, RARoleMaiHangDBItem>().swap(data_list);

	bool ret = true;
	int count = 0;
	ret = instream.Pop(&count);
	if (!ret)
	{
		return false;
	}
	int uid = 0;
	for (int i = 0; i < count; i++)
	{
		ret = instream.Pop(&uid);
		if (!ret)
		{
			return false;
		}

		RARoleMaiHangDBItem & data_node = data_list[uid];
		data_node.uid = uid;

		ret = instream.Pop(&data_node.change_state) && instream.Pop(&data_node.record_count);
		if (!ret)
		{
			return false;
		}

		TLVUnserializer record_data;
		ret = instream.Pop(&record_data);
		if (!ret)
		{
			return false;
		}

		if (record_data.Size() <= sizeof(data_node.record_list))
		{
			memcpy((void *)&data_node.record_list, record_data.Ptr(), record_data.Size());
		}
	}
	return ret;
}
