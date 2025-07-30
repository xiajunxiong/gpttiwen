#ifndef __USER_MAIL_HPP__
#define __USER_MAIL_HPP__

#include <map>

#include "servercommon/maildef.hpp"

#include "mailitem.hpp"
#include "servercommon/struct/global/maillistparam.hpp"

#include "inetworkmodule.h"

class UserMailManager;
class RMIGetSystemMailsBackObjectImpl;
class RMIInitMailsBackObjectImpl;
class Role;

class UserMail
{
	friend class RMIGetSystemMailsBackObjectImpl;
	friend class RMIInitMailsBackObjectImpl;

public:
	UserMail(UserMailManager *usermail_manager, const UserID &user_id);
	~UserMail();

	void * operator new(size_t c);
	void operator delete(void *m);

	const UserID & GetUserID() { return m_user_id; }

	void SetLastActiveTime(time_t now) { m_last_active_time = now; }
	time_t GetLastActiveTime() { return m_last_active_time; }

	void Update(time_t now);
	bool IsDirty() { return true == m_is_change; }
	bool SaveMailList();

	bool AddMail(const MailParam &new_param, bool send_to_client);
	bool RemoveMail(int mail_index, bool is_force);

	void LoadSystemMailListByCmd(bool force = false);

	void OnUserLogin(Role *role);

private:
	bool CheckStateReady();

	void LoadMailList();
	void InitMailList(int result, const MailListParam &mail_param);

	bool LoadSystemMailList(long long id_from = 0);
	void AddSystemMailList(const UserSystemMailListParam &mail_param);
	void RemoveSystemMailList(const UserSystemMailListParam &mail_param);
	void ModifySystemMailListBeforeAddToUser(UserSystemMailListParam &mail_param);

	bool RemoveMailHelper(int mail_index, bool is_force);

	MailItem * GetFreeMailItemSlot(int kind);
	MailItem * CmpPriority(MailItem *mail1, MailItem *mail2);

	void GetChangeMailList(MailListParam *p);
	void ClearDirtyMark();

	void Release();

	void TransferMailList(Role* role);

	void LogMailContent(const MailParam& param, const char* caller);

private:
	//禁止拷贝复制（请不要公有化以下这两个函数， 除非做好深拷贝， 因为在析构时会delete掉某些成员变量）
	UserMail(const UserMail&);
	UserMail& operator=(const UserMail &);

	//特殊处理道具有效时间(目前仅限万能直购卡) 道具在发出去时就开始计时
	void OnSpeicalItemHandleValidTime(MailParam & mail_param);		

	UserMailManager *m_usermail_manager;

	UserID m_user_id;

	int m_data_state;
	time_t m_get_data_time;

	int m_lock_mail_count;
	int m_mail_count;
	MailItem *m_mail_list[MAX_MAIL_NUM];

	bool m_old_state[MAX_MAIL_NUM];
	bool m_dirty_mark[MAX_MAIL_NUM];
	bool m_is_change;

	time_t m_last_save_time;				// 上次回写DB时间 
	time_t m_last_get_sys_mail_time;		// 上次拉取运营邮件的时间
	time_t m_last_check_state_time;			// 上次检查状态时间 
	time_t m_last_active_time;				// 上次活跃时间 
};

#endif // __USER_MAIL_HPP__

