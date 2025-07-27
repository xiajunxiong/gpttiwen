
#include "itemlistparam.h"
#include "servercommon/configcommon.h"

bool ItemConfigData::ReadConfigNoCheck(TiXmlElement *element)
{
	if (!GetSubNodeValue(element, "item_id", item_id)) return false;

	if (!GetSubNodeValue(element, "is_bind", is_bind)) return false;

	if (!GetSubNodeValue(element, "num", num) || num <= 0) return false;

	return true;
}

bool ItemListParam::Serialize(TLVSerializer &outstream) const
{
	bool ret = outstream.Push(count);
	if (!ret)
	{
		return false;
	}

	for (int i = 0; i < count && i < ARRAY_ITEM_COUNT(item_list); ++i)
	{
		ret = outstream.Push(item_list[i].change_state_item) 
			&& outstream.Push(item_list[i].index);
		if (!ret)
		{
			return false;
		}

		if (!item_list[i].item_wrapper.Serialize(outstream)) return false;
	}

	return ret;
}

bool ItemListParam::Unserialize(TLVUnserializer &instream)
{
	bool ret = instream.Pop(&count);
	if (!ret)
	{
		return false;
	}

	for (int i = 0; i < count && i < ARRAY_ITEM_COUNT(item_list); ++i)
	{
		ret = instream.Pop(&item_list[i].change_state_item)
			&& instream.Pop(&item_list[i].index);
		if (!ret)
		{
			return false;
		}

		if (!item_list[i].item_wrapper.Unserialize(instream)) return false;
	}

	return ret;
}

bool QueryItemListParam::Serialize(TLVSerializer &outstream) const
{
	bool ret = outstream.Push(count);
	if (!ret)
	{
		return false;
	}

	for (int i = 0; i < count && i < ARRAY_ITEM_COUNT(item_list); ++i)
	{
		ret = outstream.Push(item_list[i].change_state_item)
			&& outstream.Push(item_list[i].index);
		if (!ret)
		{
			return false;
		}

		if (!item_list[i].item_wrapper.Serialize(outstream)) return false;
	}

	return ret;
}

bool QueryItemListParam::Unserialize(TLVUnserializer &instream)
{
	bool ret = instream.Pop(&count);
	if (!ret)
	{
		return false;
	}

	for (int i = 0; i < count && i < ARRAY_ITEM_COUNT(item_list); ++i)
	{
		ret = instream.Pop(&item_list[i].change_state_item)
			&& instream.Pop(&item_list[i].index);
		if (!ret)
		{
			return false;
		}

		if (!item_list[i].item_wrapper.Unserialize(instream)) return false;
	}

	return ret;
}
