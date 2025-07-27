#include "raquanmindiscountparam.hpp"

#include "servercommon/struct/structcommon.h"

bool RAQuanMinDiscountSeverParam::Serialize(TLVSerializer & outstream) const
{
	bool ret = outstream.Push(count);
	if (!ret)
	{
		return false;
	}

	for (int i = 0; i < count; ++i)
	{
		const RAQuanMinDiscountBuyCommondity & data_item = data_list[i];

		ret = outstream.Push(data_item.change_state) && outstream.Push(data_item.seq);
		if (!ret)
		{
			return false;
		}

		if (data_item.change_state == structcommon::CS_DELETE)
		{
			continue;
		}

		TLVSerializer role_data;
		role_data.Reset((void *)&data_item.buy_uid, sizeof(data_item.buy_uid));
		role_data.MoveCurPos(sizeof(data_item.buy_uid));
		ret = outstream.Push(role_data);

		if (!ret)
		{
			return false;
		}
	}

	return true;
}

bool RAQuanMinDiscountSeverParam::Unserialize(TLVUnserializer & instream)
{
	bool ret = instream.Pop(&count);

	if (!ret)
	{
		return false;
	}

	for (int i = 0; i < count && i < QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM; ++i)
	{
		RAQuanMinDiscountBuyCommondity & data_item = data_list[i];

		ret = instream.Pop(&data_item.change_state) && instream.Pop(&data_item.seq);
		if (!ret)
		{
			return false;
		}

		if (data_item.change_state == structcommon::CS_DELETE)
		{
			continue;
		}

		TLVUnserializer role_data;
		ret = instream.Pop(&role_data);
		if (!ret)
		{
			return false;
		}

		if (role_data.Size() <= sizeof(data_item.buy_uid))
		{
			memcpy(&data_item.buy_uid, role_data.Ptr(), role_data.Size());
		}

	}

	return true;
}


bool QuanMinDiscountParam::Serialize(TLVSerializer & outstream) const
{
	bool ret = outstream.Push(count) && outstream.Push(next_id);
	if (!ret)
	{
		return false;
	}

	for (int i = 0; i < count && i < QUAN_MIN_DISCOUNT_PARAM_MAX_COUNT; ++i)
	{
		ret = outstream.Push(data_list[i].change_state) && outstream.Push(data_list[i].uid) && outstream.Push(data_list[i].buy_flag);
		if (!ret)
		{
			return false;
		}

		TLVSerializer buy_count;
		buy_count.Reset((char *)&data_list[i].buy_count, sizeof(data_list[i].buy_count));
		buy_count.MoveCurPos(sizeof(data_list[i].buy_count));
		ret = outstream.Push(buy_count);
		if (!ret)
		{
			return false;
		}
	}

	return ret;
}

bool QuanMinDiscountParam::Unserialize(TLVUnserializer & instream)
{
	bool ret = instream.Pop(&count) && instream.Pop(&next_id);
	if (!ret)
	{
		return false;
	}

	for (int i = 0; i < count && i < QUAN_MIN_DISCOUNT_PARAM_MAX_COUNT; ++i)
	{
		ret = instream.Pop(&data_list[i].change_state) && instream.Pop(&data_list[i].uid) && instream.Pop(&data_list[i].buy_flag);
		if (!ret)
		{
			return false;
		}

		TLVUnserializer buy_count;
		ret = instream.Pop(&buy_count);
		if (!ret)
		{
			return false;
		}

		if (buy_count.Size() <= sizeof(data_list[i].buy_count))
		{
			memcpy((void *)&data_list[i].buy_count, buy_count.Ptr(), buy_count.Size());
		}
	}

	return ret;
}