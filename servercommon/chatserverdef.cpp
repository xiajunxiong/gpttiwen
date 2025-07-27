#include "chatserverdef.h"
#include "servercommon.h"

bool ChatGroupParam::Serialize(TLVSerializer &outstream) const
{
	if (count < 0)
	{
		return false;
	}

	bool ret = outstream.Push(count);
	if (!ret)
	{
		return false;
	}

	for (int i = 0; i < count; ++i)
	{
		TLVSerializer serialize_data;
		serialize_data.Reset((void*)&info_list[i], sizeof(ChatGroupInfo));
		serialize_data.MoveCurPos(sizeof(ChatGroupInfo));

		bool ret = outstream.Push(change_state[i]) &&
			outstream.Push(serialize_data);

		if (!ret)
		{
			return false;
		}
	}

	return true;
}

bool ChatGroupParam::Unserialize(TLVUnserializer &instream)
{
	bool ret = instream.Pop(&count);
	if (!ret)
	{
		return false;
	}

	if (count < 0)
	{
		return false;
	}

	for (int i = 0; i < count; ++i)
	{
		TLVUnserializer unserialize_data;
		bool ret = instream.Pop(&change_state[i]) &&
			instream.Pop(&unserialize_data);

		if (!ret)
		{
			return false;
		}

		if (unserialize_data.Size() <= sizeof(ChatGroupInfo))
		{
			memcpy(&info_list[i], unserialize_data.Ptr(), unserialize_data.Size());
		}
		else
		{
			return false;
		}
	}

	return true;
}
