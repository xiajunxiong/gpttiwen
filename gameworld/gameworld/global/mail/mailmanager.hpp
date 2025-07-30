#ifndef __MAIL_MANAGER_HPP__
#define __MAIL_MANAGER_HPP__

#include <map>

#include "servercommon/maildef.hpp"
#include "servercommon/internalprotocal/gglobalprotocal.h"

class Role;
class UserMail;

class UserMailManager 
{
	friend class RMIInitMailsBackObjectImpl;
	friend class RMIGetSystemMailsBackObjectImpl;

public:
	static UserMailManager & Instance();
	
	void OnServerStart();
	void OnServerStop();

	void Update(unsigned long interval, time_t now_second);

	bool MailToUser(int mail_type, const UserID &recver_uid, int reason, const MailContentParam &contentparam);
	bool MailToMultUser(int mail_type, const UserID *recver_list, int recver_num, int reason, const MailContentParam &contentparam);
	bool MailToMultUser(int mail_type, const UserID &sender_uid, const GameName sender_name, const UserID *recver_list, int recver_num, int reason, const MailContentParam &contentparam);

	void OnUserLogin(const UserID& user_id);
	void OnUserLoginToCross(const UserID& user_id);

	void LoadUserSystemMailListByCmd(const UserID &user_id);
	void LoadAllUserSystemMailListByCmd();

	void SyncMailFromHidden(const UserID& user_id, const MailParam& param);
private:
	UserMailManager();
	~UserMailManager();

	UserMail * GetUserMail(const UserID & user_id);
	bool SendMail(const UserID &sender_uid, const GameName sender_name, const UserID *recver_list, int recver_num, int mail_type, int reason, const MailContentParam &contentparam);

	typedef std::map<UserID, UserMail*> UserMailMap;
	typedef std::map<UserID, UserMail*>::iterator UserMailMapIt;
	UserMailMap m_user_mail_map;

	time_t m_last_update;
};

#endif  // __MAIL_MANAGER_HPP__

