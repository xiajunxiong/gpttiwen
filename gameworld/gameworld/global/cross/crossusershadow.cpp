#include "crossusershadow.hpp"

#include "internalcomm.h"
#include "servercommon/serverconfig/serverconfigpool.h"
#include "servercommon/serverconfig/localconfig.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/internalprotocal/crossglobalprotocal.h"
#include "obj/character/role.h"
#include "world.h"
#include "scene/scene.h"
#include "scene/scenemanager.hpp"
#include "scene/delayloginmanager.hpp"
#include "other/event/eventhandler.hpp"

CrossUserShadow::CrossUserShadow()
{

}

CrossUserShadow::~CrossUserShadow()
{

}

CrossUserShadow & CrossUserShadow::Instance()
{
	static CrossUserShadow cus;
	return cus;
}

void CrossUserShadow::OnSyncCrossUser(crossgameprotocal::CrossGameSyncCrossUser *cgscu)
{
	switch (cgscu->sync_type)
	{
		case crossgameprotocal::CrossGameSyncCrossUser::SYNC_CROSS_USER_INIT:
		case crossgameprotocal::CrossGameSyncCrossUser::SYNC_CROSS_USER_ADD:
			{
				if (crossgameprotocal::CrossGameSyncCrossUser::SYNC_CROSS_USER_INIT == cgscu->sync_type)
				{
					m_in_cross_user_set.clear();
					m_entering_cross_user_set.clear();
				}
				
				for (int i = 0; i < cgscu->count && i < crossgameprotocal::CrossGameSyncCrossUser::MAX_IN_CROSS_USER_NUM; ++ i)
				{
					m_in_cross_user_set.insert(cgscu->in_cross_uid_list[i]);
					m_entering_cross_user_set.erase(cgscu->in_cross_uid_list[i]);

					Role *user = World::GetInstWorld()->GetSceneManager()->GetRole(cgscu->in_cross_uid_list[i]);
					if (NULL != user && NULL != user->GetScene())
					{
						EngineAdapter::Instance().NetDisconnect(user->GetNetId(), user->GetUserId(), user->GetName(), user->GetScene()->GetSceneID(), "RoleInCross");
					}

					if (crossgameprotocal::CrossGameSyncCrossUser::SYNC_CROSS_USER_ADD == cgscu->sync_type)
					{
						EventHandler::Instance().OnUserLoginCrossNoticeGame(IntToUid(cgscu->in_cross_uid_list[i]));
					}
				}
			}
			break;

		case crossgameprotocal::CrossGameSyncCrossUser::SYNC_CROSS_USER_REMOVE:
			{
				for (int i = 0; i < cgscu->count && i < crossgameprotocal::CrossGameSyncCrossUser::MAX_IN_CROSS_USER_NUM; ++ i)
				{
					m_in_cross_user_set.erase(cgscu->in_cross_uid_list[i]);
					m_entering_cross_user_set.erase(cgscu->in_cross_uid_list[i]);
					DelayLoginManager::GetInstance().OnCrossUserKicked(IntToUid(cgscu->in_cross_uid_list[i]));
					EventHandler::Instance().OnUserLogoutCrossNoticeGame(IntToUid(cgscu->in_cross_uid_list[i]));
				}
			}
			break;
	}
}

void CrossUserShadow::OnUserEnteringHidden(crossgameprotocal::CrossGameUserEnteringHidden* cgueh)
{
	bool is_allow = false;
	if (this->IsUserInCross(cgueh->entering_uid))
	{
		is_allow = true;
	}
	else
	{
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(cgueh->entering_uid));
		if (NULL != role)
		{
			EngineAdapter::Instance().NetDisconnect(role->GetNetId(), role->GetUserId(), role->GetName(), role->GetScene()->GetSceneID(), "RoleInCross");
		}
		else
		{
			if (DelayLoginManager::GetInstance().IsUserDelayLoginInfoExist(IntToUid(cgueh->entering_uid)))
			{
				DelayLoginManager::GetInstance().RemoveUserDelayLoginRequest(IntToUid(cgueh->entering_uid));
			}
		}

		m_entering_cross_user_set.insert(cgueh->entering_uid);
		is_allow = true;
	}

	if (is_allow)
	{
		static crossgameprotocal::GameCrossUserAllowedEnterHidden gcuaeh;
		gcuaeh.allowed_uid = cgueh->entering_uid;
		gcuaeh.allowed_new_uid = cgueh->new_uid;

		InternalComm::Instance().SendToCross((const char *)&gcuaeh, sizeof(gcuaeh));
	}
}

void CrossUserShadow::OnDisconnectFromCrossServer()
{
	m_in_cross_user_set.clear();
	m_entering_cross_user_set.clear();
}

void CrossUserShadow::OnUserAttemptLogin(int uid)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	InCrossUserSet::iterator it = m_in_cross_user_set.find(uid);
	if (it != m_in_cross_user_set.end())
	{
		crossgameprotocal::GameCrossKickCrossUser gckcu;
		gckcu.origin_plat_type = LocalConfig::Instance().GetPlatType();
		gckcu.origin_server_id = LocalConfig::Instance().GetMergeServerId();
		gckcu.origin_uid = uid;
		InternalComm::Instance().SendToCross((const char *)&gckcu, sizeof(gckcu));
	}
	else
	{
		EnteringCrossUserSet::iterator it2 = m_entering_cross_user_set.find(uid);
		if (it2 != m_entering_cross_user_set.end())
		{
			crossgameprotocal::GameCrossKickCrossUser gckcu;
			gckcu.origin_plat_type = LocalConfig::Instance().GetPlatType();
			gckcu.origin_server_id = LocalConfig::Instance().GetMergeServerId();
			gckcu.origin_uid = uid;
			InternalComm::Instance().SendToCross((const char *)&gckcu, sizeof(gckcu));
		}
	}
}

void CrossUserShadow::OnUserLogin(const UserID& user_id)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	int role_id = UidToInt(user_id);
	InCrossUserSet::iterator it = m_in_cross_user_set.find(role_id);
	if (it != m_in_cross_user_set.end())
	{
		crossgameprotocal::GameCrossKickCrossUser gckcu;
		gckcu.origin_plat_type = LocalConfig::Instance().GetPlatType();
		gckcu.origin_server_id = LocalConfig::Instance().GetMergeServerId();
		gckcu.origin_uid = role_id;
		InternalComm::Instance().SendToCross((const char *)&gckcu, sizeof(gckcu));
	}
	else
	{
		EnteringCrossUserSet::iterator it2 = m_entering_cross_user_set.find(role_id);
		if (it2 != m_entering_cross_user_set.end())
		{
			crossgameprotocal::GameCrossKickCrossUser gckcu;
			gckcu.origin_plat_type = LocalConfig::Instance().GetPlatType();
			gckcu.origin_server_id = LocalConfig::Instance().GetMergeServerId();
			gckcu.origin_uid = role_id;
			InternalComm::Instance().SendToCross((const char *)&gckcu, sizeof(gckcu));
		}
	}
	
}

bool CrossUserShadow::IsUserInCross(int uid)
{
	return m_in_cross_user_set.find(uid) != m_in_cross_user_set.end() ||
		m_entering_cross_user_set.find(uid) != m_entering_cross_user_set.end();
}

void CrossUserShadow::OnKickCrossUserAck(crossgameprotocal::CrossGameKickUserAck* msg)
{
	m_in_cross_user_set.erase(msg->bekick_origin_uid);
	m_entering_cross_user_set.erase(msg->bekick_origin_uid);
	DelayLoginManager::GetInstance().OnCrossUserKicked(msg->bekick_origin_uid);
}

