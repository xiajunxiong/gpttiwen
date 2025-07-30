
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

	class CSMailOneKeyFetchAttachment				//!< 9556 һ����ȡ����
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

	class CSMailDelete	//!<9551 ɾ���ʼ�
	{
	public:
		CSMailDelete();
		MessageHeader			header;

		int						mail_index;
	};

	class CSMailGetList	//!<9552 ��ȡ�ʼ��б�
	{
	public:
		CSMailGetList();
		MessageHeader			header;
	};

	class CSMailRead	//!<9553�����ȡ�ʼ�����
	{
	public:
		CSMailRead();
		MessageHeader			header;

		int						mail_index;
	};

	class CSMailFetchAttachment//!<��ȡ����9554
	{
	public:
		CSMailFetchAttachment();
		MessageHeader			header;

		int						mail_index;

		int						item_index;			//!< ץȡ������Ʒʱ��ָ������ ����ץȡ����Ϊ-1
		int						is_last;
	};

	class CSMailClean	//!<һ��ɾ��9555
	{
	public:
		CSMailClean();
		MessageHeader			header;
	};

}

#pragma pack(pop)

#endif

