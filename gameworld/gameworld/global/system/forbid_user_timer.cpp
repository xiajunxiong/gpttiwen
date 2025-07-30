#include "forbid_user_timer.hpp"

#include "obj/character/role.h"

#include "world.h"

ForbidUserTimer::ForbidUserTimer(const UserID &kick_user_id, const std::string &reason)
	: m_forbid_user_id(kick_user_id), m_forbid_reason(reason)
{

}

void ForbidUserTimer::OnTimer()
{
	Role *role = World::GetInstWorld()->GetSceneManager()->GetRole(m_forbid_user_id);
	if (NULL != role)
	{
		//gamelog::g_log_debug.printf(LL_INFO, "[ForbidUserTimer::OnTimer uid:%d rolename:%s]", role->GetUID(), role->GetName());
		Protocol::SCForbidReason msg;
		msg.forbid_time_log = 999 * SECOND_PER_DAY;
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&msg, sizeof(msg));
		role->ForbidRole(m_forbid_reason.c_str(), 999 * SECOND_PER_DAY);
	}
}

void ForbidUserTimer::Free()
{
	delete this;
}

