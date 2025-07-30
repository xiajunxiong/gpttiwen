#include "recoverclientheartbeatchecktimer.hpp"
#include "world.h"
#include "servercommon/userprotocal/msgsystem.h"
#include "gamelog.h"

RecoverClientHeartbeatCheckTimer::RecoverClientHeartbeatCheckTimer()
{

}

RecoverClientHeartbeatCheckTimer::~RecoverClientHeartbeatCheckTimer()
{

}

void RecoverClientHeartbeatCheckTimer::OnTimer()
{
	Protocol::SCClientHeartbeatCheckModify msg;
	msg.is_open = 1;

	World::GetInstWorld()->SendToAllGateway((const char*)&msg, sizeof(msg));

	gamelog::g_log_world.printf(LL_INFO, "System::CmdBlockClientHeartBeatCheck open");
}

void RecoverClientHeartbeatCheckTimer::Free()
{
	delete this;
}

