#include <memory>
#include "mailitem.hpp"

#include "servercommon/servercommon.h"
#include "engineadapter.h"

MailItem::MailItem()
	: last_fetch_time(0)
{
}

MailItem::~MailItem()
{

}

bool MailItem::OperTimeIsValid()
{
	unsigned int now_sceond = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (now_sceond >= last_fetch_time + 2)
	{
		last_fetch_time = now_sceond;	
		return true;
	}

	return false;
}

bool MailItem::HasAttachment() const
{ 
	UNSTD_STATIC_CHECK(8 == MAX_ATTACHMENT_ITEM_NUM);
	
	for (int i = 0; i < MAX_VIRTUAL_ITEM_NUM; i++)
	{
		if (0 != mail_param.contentparam.virtual_item_list.virtual_item[i])
		{
			return true;
		}
	}

	for (int i = 0; i < MAX_ATTACHMENT_ITEM_NUM; ++i)
	{
		if (!mail_param.contentparam.item_list[i].Invalid())
		{
			return true;
		}
	}

	return false;
}

bool MailItem::HasFetched() const
{
	return mail_param.status.is_fetch > 0;
}

void MailItem::ClearAttachment()
{
	for (int i = 0; i < MAX_ATTACHMENT_ITEM_NUM; i++)
	{
		//mail_param.contentparam.item_list[i].Reset();
		mail_param.contentparam.item_list[i].is_bind = 0;
		mail_param.contentparam.item_list[i].has_param = 0;
		mail_param.contentparam.item_list[i].invalid_time = 0;
		mail_param.contentparam.item_list[i].gold_price = 0;
		mail_param.contentparam.item_list[i].num = 0;
	}

	memset((void*)&mail_param.contentparam.virtual_item_list, 0, sizeof(mail_param.contentparam.virtual_item_list));
	//memset((void*)&mail_param.contentparam.item_status, 0,sizeof(mail_param.contentparam.item_status));
}

bool MailItem::Invalid()
{
	return mail_param.status.kind <= MAIL_TYPE_INVALID || mail_param.status.kind >= MAIL_TYPE_MAX;
}

