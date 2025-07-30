#ifndef __DELAY_RELOAD_CONFIG_TIMER_HPP__
#define __DELAY_RELOAD_CONFIG_TIMER_HPP__

#include "servercommon/serverdef.h"
#include "itimermodule.h"

#include <stdlib.h>

class DelayReloadConfigTimer : public ITimerCallback
{
public:
	DelayReloadConfigTimer(int reload_type, int param1, int param2);
	virtual ~DelayReloadConfigTimer() {}

	virtual void OnTimer();
	virtual void Free();

protected:
	int m_reload_type;
	int m_param1;
	int m_param2;
};

#endif

