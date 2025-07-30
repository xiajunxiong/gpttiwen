#include "mailmanager.hpp"
#include "usermail.hpp"

#include "servercommon/servercommon.h"
#include "servercommon/string/globalstr.h"
#include "servercommon/string/gameworldstr.h"
#include "protocol/msgscmail.hpp"

#include "engineadapter.h"
#include "internalcomm.h"
#include "world.h"
#include "obj/character/role.h"
#include "global/rmibackobjdef.h"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/rolemail/rolemail.hpp"
#include "other/rolemodulemanager.hpp"
#include "global/usercache/usercache.hpp"
#include "gamelog.h"

UserMailManager::UserMailManager() : m_last_update(0)
{
}

UserMailManager::~UserMailManager()
{
	for (UserMailMapIt it = m_user_mail_map.begin(), end = m_user_mail_map.end(); it != end; ++it)
	{
		if (NULL != it->second)
		{
			delete it->second;
		}
	}

	m_user_mail_map.clear();
}

UserMailManager & UserMailManager::Instance()
{
	static UserMailManager umm;
	return umm;
}

void UserMailManager::OnServerStart()
{
	
}

void UserMailManager::OnServerStop()
{
	for (UserMailMapIt it = m_user_mail_map.begin(), end = m_user_mail_map.end(); it != end; ++it)
	{
		it->second->SaveMailList();	
	}
}

void UserMailManager::Update(unsigned long interval, time_t now_second)
{
	if (now_second >= m_last_update + 1)
	{
		m_last_update = now_second;

		for (UserMailMapIt it = m_user_mail_map.begin(), end = m_user_mail_map.end(); it != end; ++it)
		{
			UserMail *user_mail = it->second;

			if (now_second - user_mail->GetLastActiveTime() >= MAIL_UN_ACTIVE_TIMEOUT_INTERVAL && !user_mail->IsDirty())
			{
				int user_status = UserCacheManager::Instance().GetUserOnlineStatus(user_mail->GetUserID());
				if (UserStatusNode::STATUS_TYPE_CROSS != user_status &&
					UserStatusNode::STATUS_TYPE_ONLINE != user_status)
				{
					// 用户不在线，清理掉
					delete user_mail;
					m_user_mail_map.erase(it);
					break;
				}
				else 
				{
					user_mail->Update(now_second);
				}
			}
			else 
			{
				user_mail->Update(now_second);
			}
		}
	}	
}

UserMail * UserMailManager::GetUserMail(const UserID &user_id)
{
	if (user_id.db_index < 0 || user_id.role_id <= 0) return NULL;
	if (!CrossConfig::Instance().IsHiddenServer()
		&& !LocalConfig::Instance().IsBelongServer(UidToInt(user_id)))
	{
		// 若角色id不属于本服，则不要给他发邮件，
		// 否则未来合服时可能会顶掉玩家自己服务器上的邮件
		assert(0);
		return NULL;
	}


	UserMail *user_mail = NULL;

	UserMailMapIt it = m_user_mail_map.find(user_id);
	if (it == m_user_mail_map.end())
	{
		user_mail = new UserMail(this, user_id);
		m_user_mail_map[user_id] = user_mail;
	}
	else 
	{
		user_mail = it->second;
	}
	user_mail->SetLastActiveTime(EngineAdapter::Instance().Time());

	return user_mail;
}

bool UserMailManager::MailToUser(int mail_type, const UserID &recver_uid, int reason, const MailContentParam &contentparam)
{
	UserID recver_list[1];
	recver_list[0] = recver_uid;
	return this->MailToMultUser(mail_type, recver_list, 1, reason, contentparam);
}

bool UserMailManager::MailToMultUser(int mail_type, const UserID *recver_list, int recver_num, int reason, const MailContentParam &contentparam)
{
	return this->MailToMultUser(mail_type, UserID(0, 0), "System", recver_list, recver_num, reason, contentparam);
}

bool UserMailManager::MailToMultUser(int mail_type, const UserID &sender_uid, const GameName sender_name, const UserID *recver_list, int recver_num, int reason, const MailContentParam &contentparam)
{
	return this->SendMail(sender_uid, sender_name, recver_list, recver_num, mail_type, reason, contentparam);
}

void UserMailManager::OnUserLogin(const UserID& user_id)
{
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
	if (NULL != role)
	{
		UserMail *user_mail = this->GetUserMail(role->GetUserId());
		if (NULL != user_mail)
		{
			user_mail->OnUserLogin(role);
		}
	}
}

void UserMailManager::OnUserLoginToCross(const UserID& user_id)
{
	UserCacheNode* node = UserCacheManager::Instance().GetUserNode(user_id);
	if (NULL != node)
	{
		// 仅用于保持UserMail在map中不被删去
		this->GetUserMail(user_id);
	}
}

bool UserMailManager::SendMail(const UserID &sender_uid, const GameName sender_name, const UserID *recver_list, int recver_num,
							   int mail_type, int reason, const MailContentParam &contentparam)
{
	if (NULL == recver_list || recver_num <= 0) return false;

	static MailParam mail_param; 
	mail_param.Reset();

	// 更新status 
	mail_param.status.sender_uid = UidToInt(sender_uid);
	F_STRNCPY(mail_param.status.sender_name, sender_name, sizeof(mail_param.status.sender_name));
	mail_param.status.recv_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	mail_param.status.kind = mail_type;

	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(sender_uid);
	if (NULL != role)
	{
		mail_param.status.sender_avatar = 0;
		mail_param.status.sender_level = role->GetLevel();
	}
	
	mail_param.status.is_lock = 0;
	mail_param.status.is_read = 0;
	mail_param.status.reason = reason;

	// 更新content
	mail_param.contentparam = contentparam;

	for (int i = 0; i < ARRAY_LENGTH(mail_param.contentparam.item_list); ++i)
	{
		if (mail_param.contentparam.item_list[i].item_id == 0)
		{
			continue;
		}

		ItemDataWrapper & curr = mail_param.contentparam.item_list[i];
		const ItemBase * itembase = ITEMPOOL->GetItem(curr.item_id);
		if (NULL == itembase)
		{
			continue;
		}

		if (itembase->GetItemType() == I_TYPE_MEDAL)
		{
			NetValueItemParam tmp_item_param;
			tmp_item_param.SetData(curr.param_data);
			if (!itembase->HasRandomNetValue(&tmp_item_param))
			{
				itembase->RandomNetValueOnPutItem(NULL, &tmp_item_param,0);
				itembase->SetRandomNetValueFlag(&tmp_item_param);
				tmp_item_param.GetData(curr.param_data);
			}
			curr.has_param = 1;
		}
	}

	for (int i = 0; i < recver_num; ++i)
	{
		EngineAdapter::Instance().GenerateSerialNum(LocalConfig::Instance().GetMergeServerId(), UidToInt(recver_list[i]), mail_param.mail_serial);

		gamelog::g_log_mail.printf(LL_INFO, "UserMailManager::SendMail mail[sender:[%d,%s], receiver[uid:%d], mail_param[%s]]", 
			UidToInt(sender_uid), sender_name, UidToInt(recver_list[i]), MailParamLog(&mail_param));

		Role* receiver_role = World::GetInstWorld()->GetSceneManager()->GetRole(UidToInt(recver_list[i]));
		if (NULL != receiver_role)
		{
			receiver_role->GetRoleModuleManager()->GetRoleMail()->AddMail(mail_param, true);
		}
		else
		{
			if (CrossConfig::Instance().IsHiddenServer() && World::GetInstWorld()->IsOriginServerConnected(LocalConfig::Instance().GetPlatType(), recver_list[i].db_index))
			{
				crossgameprotocal::HiddenGameSendMailTransfer hgsmt;
				hgsmt.origin_plat_type = LocalConfig::Instance().GetPlatType();
				hgsmt.origin_server_id = recver_list[i].db_index;
				hgsmt.recieve_uid = UidToInt(recver_list[i]);
				memcpy(&hgsmt.mail_param, &mail_param, sizeof(MailParam));

				InternalComm::Instance().SendToGameThroughCross(hgsmt.origin_plat_type, hgsmt.origin_server_id, (char *)&hgsmt, sizeof(hgsmt));
			}
			else if (!CrossConfig::Instance().IsHiddenServer()
				&& !LocalConfig::Instance().IsBelongServer(UidToInt(recver_list[i])))
			{
				crossgameprotocal::GameHiddenSendMailTransferToAnotherServer hgsmt;
				hgsmt.origin_plat_type = LocalConfig::Instance().GetPlatType();
				hgsmt.origin_server_id = recver_list[i].db_index;
				hgsmt.recieve_uid = UidToInt(recver_list[i]);
				memcpy(&hgsmt.mail_param, &mail_param, sizeof(MailParam));

				InternalComm::Instance().SendToHiddenThroughCross((char *)&hgsmt, sizeof(hgsmt));
			
				gamelog::g_log_mail.printf(LL_INFO, "UserMailManager::SendMail Transfer Mail To Another Server, mail[sender:[%d,%s], receiver[uid:%d], mail_param[%s]]",
					UidToInt(sender_uid), sender_name, UidToInt(recver_list[i]), MailParamLog(&mail_param));
			}
			else
			{
				UserMail *user_mail = this->GetUserMail(recver_list[i]);
				if (NULL == user_mail) continue;

				user_mail->AddMail(mail_param, true);
			}
		}
	}

	return true;
}

void UserMailManager::LoadUserSystemMailListByCmd(const UserID &user_id)
{
	UserMail *user_mail = this->GetUserMail(user_id);
	if (NULL != user_mail) 
	{
		user_mail->LoadSystemMailListByCmd(true);
		gamelog::g_log_mail.printf(LL_INFO, "UserMailManager::LoadUserSystemMailListByCmd role[%d]", UidToInt(user_id));
	}
}

void UserMailManager::LoadAllUserSystemMailListByCmd()
{
	for (UserMailMapIt iter = m_user_mail_map.begin(); iter != m_user_mail_map.end(); ++ iter)
	{
		UserMail *user_mail = iter->second;
		if (NULL != user_mail) 
		{
			user_mail->LoadSystemMailListByCmd(false);
		}
	}
}

void UserMailManager::SyncMailFromHidden(const UserID& user_id, const MailParam& param)
{
	UserMail* um = this->GetUserMail(user_id);
	if (NULL != um)
	{
		um->AddMail(param, true);
	}
	else
	{
		gamelog::g_log_mail.printf(LL_ERROR, "UserMailManager::SyncMailFromHidden user[%d] not found!!", UidToInt(user_id));
	}
}


