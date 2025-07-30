#ifndef __RECOVER_CLIENT_HEARTBEAT_CHECK_TIMER_HPP__
#define __RECOVER_CLIENT_HEARTBEAT_CHECK_TIMER_HPP__

#include "itimermodule.h"

class RecoverClientHeartbeatCheckTimer : public ITimerCallback
{
public:
	RecoverClientHeartbeatCheckTimer();
	virtual ~RecoverClientHeartbeatCheckTimer();

	virtual void OnTimer();
	virtual void Free();
};


#endif