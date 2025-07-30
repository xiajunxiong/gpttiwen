#ifndef __MSG_SC_MAIL_HPP_
#define __MSG_SC_MAIL_HPP_

#include "servercommon/userprotocal/msgheader.h"

#include "servercommon/maildef.hpp"
#include "servercommon/struct/itemlistparam.h"
#include "item/itembase.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{

	// S -> C

	class SCMailSendAck
	{
	public:
		SCMailSendAck();
		MessageHeader			header;

		int						ret;
	};

	class SCMailDeleteAck
	{
	public:
		SCMailDeleteAck();
		MessageHeader			header;

		int						mail_index; 
		int						ret;		// 0是成功 其他失败 
	};

	class SCMailLockAck
	{
	public:
		SCMailLockAck();
		MessageHeader			header;

		int						mail_index; 
		int						ret;		// 0是成功 其他失败 
	};

	class SCMailUnlockAck
	{
	public:
		SCMailUnlockAck();
		MessageHeader			header;

		int						mail_index; 
		int						ret;		// 0是成功 其他失败 
	};

	class SCMailListAck : public IMessageSerializer //!<9504 下发邮件列表
	{
	public:
		SCMailListAck();
		MessageHeader			header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WRITE_BEGIN(header.msg_type);
			result = result && WRITE_SHORT(is_init);
			result = result && WRITE_SHORT(count);
			for (int i = 0; i < count; ++i)
			{
				MailBrief& mb = mails[i];
				result = result && WRITE_INT(mb.mail_index);
				result = result && WRITE_SHORT(mb.has_attachment);
				result = result && WRITE_SHORT(mb.subject_length);
				result = result && WRITE_UINT(mb.mail_status.recv_time);
				result = result && WRITE_CHAR(mb.mail_status.kind);
				result = result && WRITE_CHAR(mb.mail_status.is_read);
				result = result && WRITE_CHAR(mb.mail_status.is_lock);
				result = result && WRITE_CHAR(mb.mail_status.is_fetch);
				result = result && WRITE_STRN(mb.subject, mb.subject_length);
			}

			return result;
		}
		
		short					is_init;
		short					count;
		MailBrief				mails[MAX_MAIL_SEND_LIST_SIZE];
	};

	class SCMailDetailAck : public IMessageSerializer //!<9505下发邮件内容
	{
	public:
		SCMailDetailAck();
		MessageHeader			header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteShort(mail_index, buffer, buffer_size, out_length);
			result = result && WriteShort(mail_reason, buffer, buffer_size, out_length);

			result = result && WriteStrN(subject, sizeof(MailSubject), buffer, buffer_size, out_length);
			result = result && WriteStrN(contenttxt, sizeof(MailContentTxt), buffer, buffer_size, out_length);

			result = result && WriteInt(item_count, buffer, buffer_size, out_length);
			for (int i = 0; i < item_count; ++i)
			{
				result = result && WriteUShort(item_list[i].item_id, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].column, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].index, buffer, buffer_size, out_length);
				result = result && WriteChar(item_list[i].is_market_buy_flag, buffer, buffer_size, out_length);
				result = result && WriteInt(item_list[i].num, buffer, buffer_size, out_length);
				result = result && WriteChar(item_list[i].is_bind, buffer, buffer_size, out_length);
				result = result && WriteChar(item_list[i].has_param, buffer, buffer_size, out_length);
				result = result && WriteUInt(item_list[i].invalid_time, buffer, buffer_size, out_length);
				result = result && WriteLL(item_list[i].item_unique_id, buffer, buffer_size, out_length);

				if (1 == item_list[i].has_param)
				{
					result = result && WriteShort(item_list[i].param_length, buffer, buffer_size, out_length);
					result = result && WriteStrN(item_list[i].param_data, item_list[i].param_length, buffer, buffer_size, out_length);
				}
			}

			for (int i = 0; i < MAIL_VIRTUAL_ITEM_MAX; ++i)
			{
				result = result && WriteInt(virtual_item_list.virtual_item[i], buffer, buffer_size, out_length);
			}
			return result;
		}

		short mail_index;
		short mail_reason;

		MailSubject subject;									//邮件主题
		MailContentTxt contenttxt;								//文本内容

		int item_count;
		InfoType item_list[MAX_ATTACHMENT_ITEM_NUM];		//获得物品列表

		MailVirtualItemList virtual_item_list;					//虚拟物品列表，用于存放金币，等没有实体的物品
	};

	class SCFetchAttachmentAck //!<9506 领取附件
	{
	public:
		SCFetchAttachmentAck();
		MessageHeader			header;

		int						mail_index; 
		int						item_index;
		int						ret;
	};
	
	class SCRecvNewMail : public IMessageSerializer	//!<9507 新邮件通知
	{
	public:
		SCRecvNewMail();
		MessageHeader			header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WRITE_BEGIN(header.msg_type);
			
			{
				MailBrief& mb = mail_brief;
				result = result && WRITE_INT(mb.mail_index);
				result = result && WRITE_SHORT(mb.has_attachment);
				result = result && WRITE_SHORT(mb.subject_length);
				result = result && WRITE_UINT(mb.mail_status.recv_time);
				result = result && WRITE_CHAR(mb.mail_status.kind);
				result = result && WRITE_CHAR(mb.mail_status.is_read);
				result = result && WRITE_CHAR(mb.mail_status.is_lock);
				result = result && WRITE_CHAR(mb.mail_status.is_fetch);
				result = result && WRITE_STRN(mb.subject, mb.subject_length);
			}

			return result;
		}

		MailBrief mail_brief; 
	};

	class SCHasUnReadMail
	{
	public:
		SCHasUnReadMail();
		MessageHeader			header;

		short					unread_num;
		short					chongzhi_mail_num;
	};
}

#pragma pack(pop)

#endif

