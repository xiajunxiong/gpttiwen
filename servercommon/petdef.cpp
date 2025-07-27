#include "petdef.h"

bool GodPrintItemListParam::Serialize(TLVSerializer & outstream) const
{
	bool ret = outstream.Push(count);
	if (!ret)
	{
		return false;
	}

	for (int i = 0; i < count && i < NEW_MAX_PET_GOD_PRONT_BAG_NUM; ++i)
	{
		ret = outstream.Push(item_list[i].change_state_item)
			&& outstream.Push(item_list[i].index);
		if (!ret)
		{
			return false;
		}

		TLVSerializer itemparam_data;
		itemparam_data.Reset((void*)&item_list[i].item_data, sizeof(GodPrintItem));
		itemparam_data.MoveCurPos(sizeof(GodPrintItem));

		ret = outstream.Push(itemparam_data);

		if (!ret) return false;
	}

	return ret;
}

bool GodPrintItemListParam::Unserialize(TLVUnserializer & instream)
{
	bool ret = instream.Pop(&count);
	if (!ret)
	{
		return false;
	}

	for (int i = 0; i < count && i < NEW_MAX_PET_GOD_PRONT_BAG_NUM; ++i)
	{
		ret = instream.Pop(&item_list[i].change_state_item)
			&& instream.Pop(&item_list[i].index);
		if (!ret)
		{
			return false;
		}

		static TLVUnserializer itemparam_data;
		if (!instream.Pop(&itemparam_data)) return false;

		if (itemparam_data.Size() <= sizeof(GodPrintItem))
		{
			memcpy(&item_list[i].item_data, itemparam_data.Ptr(), itemparam_data.Size());
		}
	}

	return ret;
}
