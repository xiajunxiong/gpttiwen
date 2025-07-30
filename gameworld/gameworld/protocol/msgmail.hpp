
#ifndef __MSGMAIL_HPP_
#define __MSGMAIL_HPP_

#include "servercommon/userprotocal/msgheader.h"

#include "gamedef.h"
#include "servercommon/serverdef.h"

#include "servercommon/maildef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{

	//  C -> S

	class CSMailOneKeyFetchAttachment				//!< 9556 一键提取附件
	{
	public:
		CSMailOneKeyFetchAttachment();
		MessageHeader			header;
	};

	class CSMailSend
	{
	public:
		CSMailSend();
		MessageHeader			header;

		void ResetMailContentTxt()
		{
			memset(contenttxt, 0, sizeof(contenttxt));
		}

		int						recver_uid;
		int						gold;

		short					item_count;
		short					item_knapindex_list[MAX_ATTACHMENT_ITEM_NUM];
		int						item_comsume_num[MAX_ATTACHMENT_ITEM_NUM];					

		MailSubject				subject;
		int						contenttxt_len;
		MailContentTxt			contenttxt;
	};

	class CSMailDelete	//!<9551 删除邮件
	{
	public:
		CSMailDelete();
		MessageHeader			header;

		int						mail_index;
	};

	class CSMailGetList	//!<9552 获取邮件列表
	{
	public:
		CSMailGetList();
		MessageHeader			header;
	};

	class CSMailRead	//!<9553请求读取邮件内容
	{
	public:
		CSMailRead();
		MessageHeader			header;

		int						mail_index;
	};

	class CSMailFetchAttachment//!<领取邮箱9554
	{
	public:
		CSMailFetchAttachment();
		MessageHeader			header;

		int						mail_index;

		int						item_index;			//!< 抓取单个物品时候指定索引 整体抓取设置为-1
		int						is_last;
	};

	class CSMailClean	//!<一键删除9555
	{
	public:
		CSMailClean();
		MessageHeader			header;
	};

}

#pragma pack(pop)

#endif

