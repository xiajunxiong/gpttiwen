#include "babydataparam.hpp"

bool AllBabyDataParam::Serialize(TLVSerializer & outstream) const
{
	bool ret = outstream.Push(count);
	if (!ret)
	{
		return false;
	}

	for (int i = 0; i < count && i < MAX_ONCE_BABY_COUNT; ++i)
	{
		TLVSerializer addition_info_data;
		addition_info_data.Reset((void *)&addition_info[i], sizeof(addition_info[i]));
		addition_info_data.MoveCurPos(sizeof(addition_info[i]));


		TLVSerializer all_baby_data;
		all_baby_data.Reset((void * )&baby_list[i], sizeof(baby_list[i]));
		all_baby_data.MoveCurPos(sizeof(baby_list[i]));

		ret = outstream.Push(addition_info_data) && outstream.Push(all_baby_data);
		if (!ret)
		{
			return false;
		}
	}

	return true;
}

bool AllBabyDataParam::Unserialize(TLVUnserializer & instream)
{
	bool ret = instream.Pop(&count);

	if (!ret)
	{
		return false;
	}

	for (int i = 0; i < count && i < MAX_ONCE_BABY_COUNT; ++i)
	{
		TLVUnserializer addition_info_data;
		TLVUnserializer all_baby_data;
	
		ret = instream.Pop(&addition_info_data) && instream.Pop(&all_baby_data);

		if (addition_info_data.Size() <= sizeof(addition_info[i]))
		{
			memcpy(&addition_info[i], addition_info_data.Ptr(), addition_info_data.Size());
		}

		if (all_baby_data.Size() <= sizeof(baby_list[i]))
		{
			memcpy(&baby_list[i], all_baby_data.Ptr(), all_baby_data.Size());
		}

		if (!ret)
		{
			return false;
		}
	}

	return true;
}
