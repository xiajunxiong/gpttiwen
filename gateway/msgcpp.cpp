#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/userprotocal/systemmsgcode.h"
#include "servercommon/userprotocal/msgsystem.h"
#include "servercommon/userprotocal/msgcode.h"

namespace Protocol
{
	SCHeartbeatResp::SCHeartbeatResp() : header(MT_HEARTBEAT_SC) {}
}

