#ifndef __MAILSEND_HPP__
#define __MAILSEND_HPP__

#include <stdlib.h>

#include "servercommon/maildef.hpp"
#include "servercommon/serverdef.h"

#include "protocol/msgmail.hpp"

#include "servercommon/internalprotocal/ggprotocal.h"

class Role;
struct UserCacheNode;

typedef bool (UserCacheFilterFunc)(const UserCacheNode &user_node, void **func_param);

int FillMailContentParam(MailContentParam & contentparam,const ItemConfigData & item);
int FillMailContentParam(MailContentParam & contentparam, const std::vector<ItemConfigData> & item_list);

class MailRoute
{
public:
	static MailRoute & Instance();
	
	void SendMail(Role *role, Protocol::CSMailSend *mailsend);
	void RemoveMail(Role *role, int mail_index);
	void GetMailList(Role *role);
	void ReadMail(Role *role, int mail_index);
	void FetchMailAttachment(Role *role, Protocol::CSMailFetchAttachment *attachment);
	void OnMailAttachment(Role *role, ggprotocal::MailAttachment *attachment);
	void CleanMail(Role *role);
	void OneKeyFetchAttachment(Role* role);

	static void MailToUser(const UserID &user_id, int reason, const MailContentParam &contentparam, int mail_kind = MAIL_TYPE_SYSTEM);
	static void MailToUser(const UserID &user_id, MailSubject subject, MailContentTxt contenttxt, int mail_kind = MAIL_TYPE_SYSTEM);

	static void MailToMultUser(UserID *user_list, int user_num , int reason, const MailContentParam &contentparam, int mail_kind = MAIL_TYPE_SYSTEM);
	static void MailToMultUser(UserID *user_list, int user_num , MailSubject subject, MailContentTxt contenttxt, int mail_kind = MAIL_TYPE_SYSTEM);

	//禁止在游戏服中直接发全服邮件，容易造成多次合服后出现表中数据过多的情况 - by chenyulin G6
	//static void MailToAllUser(int reason, const MailContentParam &contentparam, UserCacheFilterFunc filter_func = NULL, void **func_param = NULL);
	//static void MailToAllUser(int reason, MailSubject subject, MailContentTxt contenttxt, UserCacheFilterFunc filter_func = NULL, void **func_param = NULL);

	void LogMailContent(Role* role, const ggprotocal::MailAttachment* ma, const char* caller);
private:
	MailRoute() {}
	~MailRoute() {}
};

bool UserCacheLevelFilter(const UserCacheNode &user_node, void **func_param);

#endif

