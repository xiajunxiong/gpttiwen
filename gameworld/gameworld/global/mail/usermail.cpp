#include "usermail.hpp"
#include "item/knapsack.h"
#include "other/rolemodulemanager.hpp"
#include "friend/friendmanager.h"
#include "mailmanager.hpp"

#include "global/rmibackobjdef.h"
#include "internalcomm.h"
#include "engineadapter.h"

#include "servercommon/servercommon.h"
#include "servercommon/errornum.h"
#include "servercommon/struct/structcommon.h"
#include "world.h"

#include "servercommon/internalprotocal/ggprotocal.h"
#include "gamelog.h"

#include "other/route/mailroute.hpp"
#include "obj/character/role.h"
#include "global/usercache/usercache.hpp"
#include "item/itempool.h"
#include "other/rolemail/rolemail.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"

UserMail::UserMail(UserMailManager *usermail_manager, const UserID & user_id)
	: m_usermail_manager(usermail_manager), m_user_id(user_id), m_data_state(MAIL_STATE_INVALID), m_get_data_time(0),
	  m_lock_mail_count(0), m_mail_count(0), m_is_change(false), m_last_save_time(0), 
	  m_last_get_sys_mail_time(0), m_last_check_state_time(0), m_last_active_time(0)
{
	memset(m_mail_list, 0, sizeof(m_mail_list));

	this->Release();
}

UserMail::~UserMail()
{
	this->Release();
}

void UserMail::Release()
{
	for (int i = 0; i < MAX_MAIL_NUM; ++i)
	{
		if (NULL != m_mail_list[i])
		{
			delete m_mail_list[i];
		}
	}
	memset(m_mail_list, 0, sizeof(m_mail_list));

	m_data_state = MAIL_STATE_INVALID;

	m_is_change = false;
	memset(m_dirty_mark, 0, sizeof(m_dirty_mark));
	memset(m_old_state, 0, sizeof(m_old_state));
}

void UserMail::Update(time_t now)
{
	if (now - m_last_save_time >= MAIL_SAVE_DB_INTERVAL && m_is_change && MAIL_STATE_LOAD_OK == m_data_state)
	{
		if (this->SaveMailList())
		{
			m_last_save_time = now;
			this->ClearDirtyMark();
		}
	}

	if (now - m_last_get_sys_mail_time >= GET_SYSTEM_MAIL_INTERVAL && MAIL_STATE_LOAD_OK == m_data_state)
	{
		m_last_get_sys_mail_time = now;
		this->LoadSystemMailList();
	}

	if (now - m_last_check_state_time >= MAX_MAIL_CHECK_STATE_INTERVAL)  // 保证异常时候 能够自恢复
	{
		m_last_check_state_time = now;

		if (MAIL_STATE_LOAD_ING == m_data_state && now - m_get_data_time >= MAX_GET_MAIL_TIMEOUT_INTERVAL)
		{
			m_data_state = MAIL_STATE_INVALID;
		}
	}
}

bool UserMail::SaveMailList()
{
	if (!this->CheckStateReady() && !m_is_change) return false;

	RMISaveMailBackObjectImpl *smbo = new RMISaveMailBackObjectImpl();

	static MailListParam mail_param;
	this->GetChangeMailList(&mail_param);

	RMIGlobalClient gc;
	gc.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	return gc.SaveMailsAsyn(UidToInt(m_user_id), mail_param, smbo);
}

void UserMail::GetChangeMailList(MailListParam *p)
{
	if (!m_is_change)
	{
		p->count = 0;
		return;
	}

	int count = 0;

	for (int i = 0; i < MAX_MAIL_NUM && count < MAX_MAIL_NUM; ++i)
	{
		if (m_dirty_mark[i])
		{
			if (NULL == m_mail_list[i] || m_mail_list[i]->Invalid())
			{
				if (!m_old_state[i]) continue;

				p->mail_list[count].change_state = structcommon::CS_DELETE;
				p->mail_list[count].mail_param.mail_index = i;
			}
			else 
			{
				p->mail_list[count].change_state = m_old_state[i] ? structcommon::CS_UPDATE : structcommon::CS_INSERT;
				p->mail_list[count].mail_param = m_mail_list[i]->mail_param;
			}

			++ count;
		}
	}

	p->count = count;
}

void UserMail::ClearDirtyMark()
{
	memset(m_dirty_mark, 0, sizeof(m_dirty_mark));

	for (int i = 0; i < MAX_MAIL_NUM; ++i)
	{
		m_old_state[i] = (NULL != m_mail_list[i] && !m_mail_list[i]->Invalid());
	}

	m_is_change = false;
}

bool UserMail::CheckStateReady()
{
	if (MAIL_STATE_LOAD_OK != m_data_state) 
	{
		if (MAIL_STATE_LOAD_ING != m_data_state) 
		{
			this->LoadMailList();

			m_data_state = MAIL_STATE_LOAD_ING;
			m_get_data_time = EngineAdapter::Instance().Time();
		}

		return false;
	}

	return true;
}

void UserMail::LoadMailList()
{
	RMIInitMailsBackObjectImpl *gmbo = new RMIInitMailsBackObjectImpl();
	gmbo->user_id = m_user_id;

	RMIGlobalClient gc;
	gc.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	gc.InitMailsAsyn(UidToInt(m_user_id), gmbo);
}

void RMIInitMailsBackObjectImpl::InitMailsRet(int ret, const MailListParam &mail_param)
{
	UserMail *user_mail = UserMailManager::Instance().GetUserMail(user_id);
	if (NULL == user_mail)
	{
		gamelog::g_log_serious_error.printf(LL_CRITICAL, "[RMIInitMailsBackObjectImpl::InitMailsRet] user_mail not existed, uid:%d", UidToInt(user_id));
		return;
	}

	user_mail->InitMailList(ret, mail_param);
}

void UserMail::InitMailList(int result, const MailListParam &mail_param)
{
	if (MAIL_STATE_LOAD_ING != m_data_state) return;    // 当前状态必须是Loading  

	if (result < 0) 
	{ 
		m_data_state = MAIL_STATE_INVALID; 
		return; 
	} 

	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	static MailItem mail_list_temp[MAX_MAIL_NUM];
	memset(mail_list_temp, 0, sizeof(mail_list_temp));

	int mail_temp_count = 0;
	if (m_mail_count > 0) 
	{
		for (int i = 0; i < MAX_MAIL_NUM; ++i)
		{
			if (NULL != m_mail_list[i] && !m_mail_list[i]->Invalid())
			{
				memcpy(&mail_list_temp[mail_temp_count], m_mail_list[i], sizeof(mail_list_temp[0]));
				delete m_mail_list[i];
				m_mail_list[i] = NULL;
				++ mail_temp_count;
			}
		}
	}
	this->Release();

	m_mail_count = 0; m_lock_mail_count = 0;
	for (int i = 0; i < mail_param.count && i < MAX_MAIL_NUM; ++i)
	{
		int index = mail_param.mail_list[i].mail_param.mail_index;
		if (index < 0 || index >= MAX_MAIL_NUM)
		{
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[UserMail::InitMailList] Wrong index, index:%d", index);
			continue;
		}

		if (static_cast<int>(now - mail_param.mail_list[i].mail_param.status.recv_time) > MAX_MAIL_RESERVER_TIME && !mail_param.mail_list[i].mail_param.status.is_lock) 
		{
			m_old_state[index] = true;
			m_dirty_mark[index] = true;
			m_is_change = true; 
		}
		else 
		{
			if (NULL != m_mail_list[index])
			{
				gamelog::g_log_serious_error.printf(LL_CRITICAL, "[UserMail::InitMailList] Repeat index, index:%d", index);
				continue;
			}

			m_mail_list[index] = new MailItem();

			memcpy(&m_mail_list[index]->mail_param, &mail_param.mail_list[i].mail_param, sizeof(m_mail_list[0]->mail_param));
			m_old_state[index] = true;

			++ m_mail_count;
			if (0 != m_mail_list[index]->mail_param.status.is_lock) ++ m_lock_mail_count;
		}
	}

	m_data_state = MAIL_STATE_LOAD_OK;
	m_last_save_time = EngineAdapter::Instance().Time();
	m_last_get_sys_mail_time = m_last_save_time - GET_SYSTEM_MAIL_INTERVAL + 2;

	for (int i = 0; i < mail_temp_count; ++i)	
	{
		if (!mail_list_temp[i].Invalid())
		{
			this->AddMail(mail_list_temp[i].mail_param, false);
		}
	}

	gamelog::g_log_mail.printf(LL_INFO, "UserMail::InitMailList LoadOK role[%d]", UidToInt(m_user_id));

	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(m_user_id);
	if (NULL != role)
	{
		this->TransferMailList(role);
	}
}

bool UserMail::LoadSystemMailList(long long id_from)
{
	RMIGetSystemMailsBackObjectImpl *gsmbo = new RMIGetSystemMailsBackObjectImpl();
	gsmbo->user_id = m_user_id;

	gamelog::g_log_mail.printf(LL_INFO, "UserMail::LoadSystemMailList role[%d]", UidToInt(m_user_id));

	RMIGlobalClient gc;
	gc.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	return gc.GetSystemMailsAsyn(id_from, UidToInt(m_user_id), gsmbo);
}

void RMIGetSystemMailsBackObjectImpl::GetSystemMailsRet(int ret, const UserSystemMailListParam &mail_param)
{
	if (ret < 0 || mail_param.count <= 0) return;

	UserMail *user_mail = UserMailManager::Instance().GetUserMail(user_id);
	if (NULL == user_mail) return;

	gamelog::g_log_mail.printf(LL_INFO, "UserMail::LoadSystemMailList Return role[%d]", UidToInt(user_id));

	user_mail->RemoveSystemMailList(mail_param);
	user_mail->AddSystemMailList(mail_param);
	if (mail_param.count > 0)
	{	
		user_mail->LoadSystemMailList(mail_param.next_id_from);
	}
}

void UserMail::AddSystemMailList(const UserSystemMailListParam &in_mail_param)
{
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	UNSTD_STATIC_CHECK(8 == MAX_ATTACHMENT_ITEM_NUM);

	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(m_user_id);
	int online_status = UserCacheManager::Instance().GetUserOnlineStatus(m_user_id);

	UserSystemMailListParam mail_param(in_mail_param);
	this->ModifySystemMailListBeforeAddToUser(mail_param);
	
	for (int i = 0; i < ARRAY_LENGTH(mail_param.mail_list) && i < mail_param.count; ++i)
	{
		const MailParam &sysmail_param = mail_param.mail_list[i].mail_param;
		static MailParam system_mail_param;
		memcpy(&system_mail_param, &sysmail_param, sizeof(system_mail_param));
		this->OnSpeicalItemHandleValidTime(system_mail_param);

		if (mail_param.mail_list[i].check_uid != UidToInt(m_user_id))
		{
			gamelog::g_log_mail.printf(LL_INFO, "mail->add system mail[id:%d] error[recver[ uid:%d ], check[uid:%d], mail_param[%s]",
				(int)mail_param.mail_list[i].system_mail_id, UidToInt(m_user_id), (int)mail_param.mail_list[i].check_uid, MailParamLog(&system_mail_param));
			continue;
		}

		if (static_cast<int>(now - mail_param.mail_list[i].mail_param.status.recv_time) > MAX_MAIL_RESERVER_TIME)
		{
			gamelog::g_log_mail.printf(LL_INFO, "mail->add system mail[id:%d] timeout[recver[ uid:%d ], mail_param[%s]",
				(int)mail_param.mail_list[i].system_mail_id, UidToInt(m_user_id), MailParamLog(&system_mail_param));
			continue;
		}

		if (NULL != role)
		{
			if (role->GetRoleModuleManager()->GetRoleMail()->AddMail(system_mail_param, true))
			{
				gamelog::g_log_mail.printf(LL_INFO, "mail->add system mail to rolemail succ [mailid:%d] [recver[ uid:%d ], mail_param[%s]",
					(int)mail_param.mail_list[i].system_mail_id, UidToInt(m_user_id), MailParamLog(&system_mail_param));
			}
			else
			{
				gamelog::g_log_mail.printf(LL_INFO, "mail->add system mail to rolemail fail [mailid:%d] [recver[ uid:%d ], mail_param[%s]",
					(int)mail_param.mail_list[i].system_mail_id, UidToInt(m_user_id), MailParamLog(&system_mail_param));
			}
		}
		else
		{
			bool send_to_cross = true;	
			if (CrossConfig::Instance().IsHiddenServer())
			{
				send_to_cross = false;
			}
			//else if (system_mail_param.contentparam.HasAttachment())
			//{
			//	send_to_cross = false;
			//}
			else if (UserStatusNode::STATUS_TYPE_CROSS != online_status)
			{
				send_to_cross = false;
			}

			if (!send_to_cross)
			{
				if (this->AddMail(system_mail_param, true))
				{
					gamelog::g_log_mail.printf(LL_INFO, "mail->add system mail[id:%d] succ[recver[ uid:%d ], mail_param[%s]",
						(int)mail_param.mail_list[i].system_mail_id, UidToInt(m_user_id), MailParamLog(&system_mail_param));
				}
				else
				{
					gamelog::g_log_mail.printf(LL_INFO, "mail->add system mail[id:%d] fail[recver[ uid:%d ], mail_param[%s]",
						(int)mail_param.mail_list[i].system_mail_id, UidToInt(m_user_id), MailParamLog(&system_mail_param));
				}
			}
			else
			{
				crossgameprotocal::GameHiddenSendSystemMail msgmail;
				msgmail.send_to_uid = UidToInt(m_user_id);
				msgmail.mail_param = system_mail_param;
				msgmail.plat_type = LocalConfig::Instance().GetPlatType();
				msgmail.server_id = LocalConfig::Instance().GetMergeServerId();
				InternalComm::Instance().SendToHiddenThroughCross((const char*)&msgmail, sizeof(msgmail));
			}
		}
	}
}

void UserMail::RemoveSystemMailList(const UserSystemMailListParam &mail_param)
{
	RMIRemoveSystemMailsBackObjectImpl *smbo = new RMIRemoveSystemMailsBackObjectImpl();
	smbo->user_id = m_user_id;

	RMIGlobalClient gc;
	gc.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	gc.RemoveSystemMailsAsyn(UidToInt(m_user_id), mail_param, smbo);
}

void UserMail::ModifySystemMailListBeforeAddToUser(UserSystemMailListParam &mail_param)
{
	for (int i = 0; i < mail_param.count && i < ARRAY_LENGTH(mail_param.mail_list); ++i)
	{
		MailParam& mail = mail_param.mail_list[i].mail_param;
		if (strcmp(mail.mail_serial, "") == 0)
		{
			EngineAdapter::Instance().GenerateSerialNum(LocalConfig::Instance().GetMergeServerId(), UidToInt(m_user_id), mail.mail_serial);
		}

		// 兼容后台接口
		if (mail.contentparam.reserver_coin_bind > 0)
		{
			mail.contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] += mail.contentparam.reserver_coin_bind;
			mail.contentparam.reserver_coin_bind = 0;
		}
		if (mail.contentparam.reserver_gold > 0)
		{
			mail.contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GOLD] += mail.contentparam.reserver_gold;
			mail.contentparam.reserver_gold = 0;
		}
	}
}

bool UserMail::RemoveMailHelper(int mail_index, bool is_force)
{
	if (!this->CheckStateReady()) return false;

	if (
		mail_index < 0                                          || 
		mail_index >= MAX_MAIL_NUM                              || 
		NULL == m_mail_list[mail_index]                         ||
		m_mail_list[mail_index]->Invalid()                      
		)
	{
		return false;
	}

	if (!is_force)
	{
		if (m_mail_list[mail_index]->HasAttachment() && 0 == m_mail_list[mail_index]->mail_param.status.is_fetch)
		{
			return false;
		}
	}
	
	gamelog::g_log_mail.printf(LL_INFO, "mail->remove mail[ret:succ, receiver[uid:%d], mail_param[%s]]",
		UidToInt(m_user_id), MailParamLog(&m_mail_list[mail_index]->mail_param));

	delete m_mail_list[mail_index];
	m_mail_list[mail_index] = NULL;

	m_mail_count--;
	if (m_mail_count <= 0) m_mail_count = 0;
	m_is_change = true;
	m_dirty_mark[mail_index] = true;


	return true;
}

MailItem * UserMail::GetFreeMailItemSlot(int kind)
{
	if (kind <= MAIL_TYPE_INVALID || kind >= MAIL_TYPE_MAX) return NULL;

	for (int i = 0; i < MAX_MAIL_NUM; ++i)
	{
		if (NULL == m_mail_list[i])
		{
			m_mail_list[i] = new MailItem();
			m_mail_list[i]->mail_param.mail_index = i;
			++ m_mail_count;
			return m_mail_list[i];
		}
	}

	// CmpPriority 这个函数要求邮件必须满 

	MailItem *mail_tmp = m_mail_list[0];

	for (int i = 0; i < MAX_MAIL_NUM; ++i)
	{
		mail_tmp = this->CmpPriority(mail_tmp, m_mail_list[i]);
	}

	return mail_tmp;
}

MailItem * UserMail::CmpPriority(MailItem *mail1, MailItem *mail2)
{
	if (NULL == mail1 || NULL == mail2) return NULL;

	if (mail1->mail_param.status.is_lock == mail2->mail_param.status.is_lock)
	{
		if (mail1->HasAttachment() == mail2->HasAttachment())
		{
			if (mail1->mail_param.status.is_read == mail2->mail_param.status.is_read)
			{
				if (mail1->mail_param.status.kind == mail2->mail_param.status.kind)
				{
					if (mail1->mail_param.status.recv_time > mail2->mail_param.status.recv_time) return mail2;
					return mail1;
				}
				else
				{
					if (mail1->mail_param.status.kind == MAIL_TYPE_PERSONAL && mail2->mail_param.status.kind != MAIL_TYPE_PERSONAL) return mail1;
					return mail2;
				}
			}
			else 
			{
				if (mail1->mail_param.status.is_read && !mail2->mail_param.status.is_read) return mail1;
				return mail2;
			}
		}
		else 
		{
			if (mail1->HasAttachment() && !mail2->HasAttachment()) return mail2;
			return mail1;
		}
	}
	else 
	{
		if (mail1->mail_param.status.is_lock && !mail2->mail_param.status.is_lock) return mail2;
		return mail1;
	}
}

bool UserMail::AddMail(const MailParam &new_param, bool send_to_client)
{
	bool mail_load_ok = this->CheckStateReady();  // 不返回 先插入 等DB数据回来再合并 

	bool ret = true;

	MailItem *mailitem = this->GetFreeMailItemSlot(new_param.status.kind);  // 邮箱已满
	if (NULL != mailitem)									
	{
		int index = mailitem->mail_param.mail_index;
		if (index >= 0 && index < MAX_MAIL_NUM)
		{
			memcpy(&mailitem->mail_param, &new_param, sizeof(mailitem->mail_param));
			mailitem->mail_param.mail_index = index;

			// 兼容后台接口
			if (mailitem->mail_param.contentparam.reserver_coin_bind > 0)
			{
				mailitem->mail_param.contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] += mailitem->mail_param.contentparam.reserver_coin_bind;
			}
			if (mailitem->mail_param.contentparam.reserver_gold > 0)
			{
				mailitem->mail_param.contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GOLD] += mailitem->mail_param.contentparam.reserver_gold;
			}

			if (strcmp(mailitem->mail_param.mail_serial, "") == 0)
			{
				EngineAdapter::Instance().GenerateSerialNum(LocalConfig::Instance().GetMergeServerId(), UidToInt(m_user_id), mailitem->mail_param.mail_serial);
			}

			m_dirty_mark[index] = true;
			m_is_change = true;

			ret = true;

			if (mailitem->mail_param.contentparam.HasAttachment())
			{
				this->LogMailContent(new_param, __FUNCTION__);
			}
		}

		if (!mailitem->mail_param.contentparam.IsInvalid())
		{
			if (mail_load_ok)
			{
				gamelog::g_log_mail.printf(LL_INFO, "mail->add mail[ret:%s, receiver[uid:%d], mail_param[%s]]", ret ? "succ" : "full",
										   UidToInt(m_user_id), MailParamLog(&mailitem->mail_param));
			}
		}
	}
	else
	{
		MailItem mail_item;
		mail_item.mail_param = new_param;

		// 兼容后台接口
		if (mail_item.mail_param.contentparam.reserver_coin_bind > 0)
		{
			mail_item.mail_param.contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] += mail_item.mail_param.contentparam.reserver_coin_bind;
		}
		if (mail_item.mail_param.contentparam.reserver_gold > 0)
		{
			mail_item.mail_param.contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GOLD] += mail_item.mail_param.contentparam.reserver_gold;
		}

		ret = false;

		if (!mail_item.mail_param.contentparam.IsInvalid())
		{
			if (mail_load_ok)
			{
				gamelog::g_log_mail.printf(LL_INFO, "mail->add mail[ret:%s, receiver[uid:%d], mail_param[%s]]", ret ? "succ" : "full",
										   UidToInt(m_user_id), MailParamLog(&mail_item.mail_param));
			}
		}
	}

	return ret;
}

bool UserMail::RemoveMail(int mail_index, bool is_force)
{
	if (!this->CheckStateReady()) return false;

	return RemoveMailHelper(mail_index, is_force);
}

void UserMail::LoadSystemMailListByCmd(bool force)
{
	if (force)
	{
		m_last_get_sys_mail_time = 0;
	}
	else 
	{
		m_last_get_sys_mail_time = EngineAdapter::Instance().Time() - GET_SYSTEM_MAIL_INTERVAL + rand() % 15;
	}
}

void UserMail::OnUserLogin(Role *role)
{
	if (NULL == role)
	{
		return;
	}

	if (!this->CheckStateReady())
	{
		return;
	}

	this->TransferMailList(role);
}

void UserMail::TransferMailList(Role* role)
{
	for (int i = 0; i < MAX_MAIL_NUM; ++i)
	{
		MailItem* mail = m_mail_list[i];
		if (NULL == mail || mail->Invalid()) continue;

		if (role->GetRoleModuleManager()->GetRoleMail()->AddMail(mail->mail_param, true))
		{
			gamelog::g_log_mail.printf(LL_INFO, "UserMail::OnUserLogin role[%d,%s] transfer mail to rolemail, mail_index[%d] mailparam[%s]",
				role->GetUID(), role->GetName(), i, MailParamLog(&mail->mail_param));
			if (!this->RemoveMail(i, true))
			{
				gamelog::g_log_mail.printf(LL_ERROR, "UserMail::OnUserLogin role[%d,%s] transfer mail failed on remove!!! mail_index[%d] mail_serial[%s]",
					role->GetUID(), role->GetName(), i, mail->mail_param.mail_serial);
			}
		}
		else
		{
			gamelog::g_log_mail.printf(LL_INFO, "UserMail::OnUserLogin role[%d,%s] transfer mail failed on addmail!!! mail_index[%d] mail_serial[%s]",
				role->GetUID(), role->GetName(), i, mail->mail_param.mail_serial);
		}
	}
}

void UserMail::LogMailContent(const MailParam& param, const char* caller)
{
	gamelog::g_log_mail.buff_printf("UserMail::AddMail caller[%s] user[%d] serial_num[%s] attachments[",
		caller, UidToInt(m_user_id), param.mail_serial);
	for (int i = 0; i < MAX_ATTACHMENT_ITEM_NUM; ++i)
	{
		const MailContentParam& p = param.contentparam;
		if (p.item_list[i].Invalid()) continue;

		const ItemDataWrapper& item = p.item_list[i];
		gamelog::g_log_mail.buff_printf("{item[%d,%d,%d] param[%s]}",
			item.item_id, item.num, item.is_bind,
			NetValueItemParam::GetParamLog(item.item_id, item.param_data));
	}

	gamelog::g_log_mail.buff_printf("]");
	gamelog::g_log_mail.commit_buff(LL_INFO);
}

void UserMail::OnSpeicalItemHandleValidTime(MailParam & mail_param)
{
	for (int i = 0; i < ARRAY_ITEM_COUNT(mail_param.contentparam.item_list); ++i)
	{
		if (mail_param.contentparam.item_list[i].Invalid()) continue;

		ItemID item_id = mail_param.contentparam.item_list[i].item_id;
		const ItemBase* item = ITEMPOOL->GetItem(item_id);
		if (NULL == item || 27998 != item_id || mail_param.contentparam.item_list[i].invalid_time > 0) continue;

		UInt32 vaild_timestamp = item->CalInvalidTime();
		if (vaild_timestamp <= 0)
		{
			continue;
		}
		mail_param.contentparam.item_list[i].invalid_time = vaild_timestamp;
	}
}

