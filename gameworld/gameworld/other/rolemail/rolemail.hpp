#ifndef __ROLE_MAIL_HPP__
#define __ROLE_MAIL_HPP__

#include "servercommon/maildef.hpp"
#include "servercommon/struct/global/maillistparam.hpp"
#include <queue>

class RoleModuleManager;
class MailItem;
class RoleMail
{
public:
	typedef std::queue<int> EmptySlotQueue;

public:
	RoleMail();
	~RoleMail();

	void* operator new(size_t c);
	void  operator delete(void* m);

	void Init(RoleModuleManager* mgr, const MailListParam& mail_list);
	bool AddMail(const MailParam &new_param, bool send_to_client);
	bool RemoveMail(int mail_index);

	void Update(time_t now_second);
	void GetChangeMailList(MailListParam *p);
	void ClearDirtyMark();

	int ClearGodBeastAdventItem(ItemID item_id);		// 清除玩家邮箱里的月灵币

	// 玩家操作
	bool FetchMailList(bool force = false);
	bool FetchMailDetail(int mail_index);
	bool FetchAttachment(int mail_index, int item_index, int is_last);
	bool FetchAttachmentAffirm(int mail_index, int result, int item_index, int is_last);
	void OneKeyFetchAttachment();
	bool CleanMail();

private:
	void SendMailAck(const MailItem *mail_item, bool is_succ);
	void Release();
	MailItem * GetFreeMailItemSlot(int kind);
	MailItem * CmpPriority(MailItem *mail1, MailItem *mail2);
	void CheckMailItemDataVersion(MailParam& mp);

	bool RemoveMailHelper(int mail_index);
	void CheckUnReadMail();

	void MailItemCheckMedal(ARG_OUT MailItem *mailitem);
	void LogMailContent(const MailParam& param, const char* caller);
private:
	RoleModuleManager* m_mgr;

	MailItem* m_mail_list[MAX_MAIL_NUM];
	bool m_old_state[MAX_MAIL_NUM];
	bool m_dirty_mark[MAX_MAIL_NUM];
	bool m_is_change;

	EmptySlotQueue m_empty_slot_queue;

	unsigned int m_last_get_list_time;			// 上次客户端拉取邮件列表的时间
	unsigned int m_last_one_key_fetch_time;		// 上次一键领取时间
};

#endif