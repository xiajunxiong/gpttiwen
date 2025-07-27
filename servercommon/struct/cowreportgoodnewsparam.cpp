#include "cowreportgoodnewsparam.h"
#include "servercommon/struct/structcommon.h"

bool CowReportGoodNewsBuyListParam::Serialize(TLVSerializer &outstream) const
{
	if (!outstream.Push(id_from))
	{
		return false;
	}

	if (!outstream.Push(count))
	{
		return false;
	}

	bool ret = true;

	for (int i = 0; i < count && i < MAX_ONCE_COW_REPORT_GOOD_NEWS_COUNT; ++i)
	{
		ret = outstream.Push(user_list[i].change_state) && outstream.Push(user_list[i].role_id) ;
		if (!ret) return false;

		if (user_list[i].change_state == structcommon::CS_DELETE) continue;

		ret = outstream.Push(user_list[i].last_notice_timestamp) ;

		if (!ret)
		{
			return false;
		}
	}

	return ret;

}

bool CowReportGoodNewsBuyListParam::Unserialize(TLVUnserializer &instream)
{
	if (!instream.Pop(&id_from))
	{
		return false;
	}

	if (!instream.Pop(&count))
	{
		return false;
	}

	bool ret = true;

	for (int i = 0; i < count && i < MAX_ONCE_COW_REPORT_GOOD_NEWS_COUNT; ++i)
	{
		ret = instream.Pop(&user_list[i].change_state) &&  instream.Pop(&user_list[i].role_id) ;
		if (!ret) return false;

		if (user_list[i].change_state == structcommon::CS_DELETE) continue;

		ret = instream.Pop(&user_list[i].last_notice_timestamp);

		if (!ret)
		{
			return false;
		}

	}

	return ret;

}
