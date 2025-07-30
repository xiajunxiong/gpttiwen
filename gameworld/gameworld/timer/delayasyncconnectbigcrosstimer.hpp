#ifndef __DELAY_ASYNC_CONNECT_BIGCROSS_TIMER_HPP__
#define __DELAY_ASYNC_CONNECT_BIGCROSS_TIMER_HPP__

#include "servercommon/serverdef.h"
#include "itimermodule.h"

#include <stdlib.h>

class DelayAsyncConnectBigcrossTimer : public ITimerCallback
{
public:
	DelayAsyncConnectBigcrossTimer();
	virtual ~DelayAsyncConnectBigcrossTimer() {}

	virtual void OnTimer();
	virtual void Free();

protected:
	
};

#endif

