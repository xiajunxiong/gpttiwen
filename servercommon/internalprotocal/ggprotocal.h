#ifndef __GGPROTOCAL_H__
#define __GGPROTOCAL_H__

#include "servercommon/maildef.hpp"
#include "servercommon/basedef.h"
#include "servercommon/servercommon.h"

#pragma pack(push) 
#pragma pack(4)

// ���͸�����������ͨ��UserID�����е�ͨѶЭ��

namespace ggprotocal
{
	// MAX_GGPROTOCAL_LENGTH 1024 ������servercommon.h��
	enum _MsgType
	{
		MT_INVALID,

		MT_GG_BEGIN = 22000,
		MT_MAIL_ATTACHMENT,									// ֪ͨgameworld��ȡ�ʼ����� 
	};

	struct MessageHeader
	{
		MessageHeader() : msg_type(MT_INVALID) {}
		MessageHeader(int type) : msg_type(type) {}

		int msg_type;
	};

	struct MailAttachment
	{
		MailAttachment() : header(MT_MAIL_ATTACHMENT) {}

		MessageHeader header;

		int sender_uid;
		int mail_index;
		int item_index;
		int is_last;

		int item_count;
		ItemDataWrapper item_list[MAX_ATTACHMENT_ITEM_NUM];

		MailVirtualItemList virtual_item_list;
		MailSubject subject;
		MailContentTxt contenttxt;
		SerialNum serial_num;

		char item_status[MAX_ATTACHMENT_ITEM_NUM + MAX_VIRTUAL_ITEM_NUM + 4];				//��� �����Ƿ�����ȡ״̬������ȡ=0��δ��ȡ=1;�±�һһ��Ӧ����	
		short reserved;
	};

}
#pragma pack(pop)

#endif // __GGPROTOCAL_H__


