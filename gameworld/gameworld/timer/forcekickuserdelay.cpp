#include "forcekickuserdelay.hpp"
#include "obj/character/role.h"
#include "world.h"
#include "gamelog.h"
#include "scene/delayloginmanager.hpp"

ForceKickUserDelay::ForceKickUserDelay(const UserID &kick_user_id, int kick_user_reason)
	: m_kick_user_id(kick_user_id), m_kick_user_reason(kick_user_reason)
{

}

void ForceKickUserDelay::OnTimer()
{
//#ifdef _DEBUG
//	printf("ForceKickUserDelay::OnTimer start\n");
//#endif

	Role *role = World::GetInstWorld()->GetSceneManager()->GetRole(m_kick_user_id);
	if (NULL != role && role->IsForceKickDelay())
	{
		gamelog::g_log_debug.printf(LL_INFO, "[ForceKickUserDelay::OnTimer uid:%d rolename:%s kickreason:%d]", role->GetUID(), role->GetName(), m_kick_user_reason);
		
//#ifdef _DEBUG
//		printf("ForceKickUserDelay::OnTimer role[%d,%s]\n", role->GetUID(), role->GetName());
//#endif

		EngineAdapter::Instance().NetDisconnect(role->GetNetId(), role->GetUserId(), role->GetName(), 0, "UserExist");
		World::GetInstWorld()->GetSceneManager()->Logout(role->GetUserId());
	}

//#ifdef _DEBUG
//	printf("ForceKickUserDelay::OnTimer end\n");
//#endif
}

void ForceKickUserDelay::Free()
{
	delete this;
}

