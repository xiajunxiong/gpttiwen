#ifndef __FORBID_USER_TIMER_HPP__
#define __FORBID_USER_TIMER_HPP__

#include "servercommon/serverdef.h"
#include "itimermodule.h"

#include <stdlib.h>
#include <string>

class ForbidUserTimer : public ITimerCallback
{
public:
	ForbidUserTimer(const UserID &kick_user_id, const std::string &reason);
	virtual ~ForbidUserTimer() {}

	virtual void OnTimer();
	virtual void Free();

protected:
	UserID			m_forbid_user_id;
	std::string		m_forbid_reason;
};

#endif

