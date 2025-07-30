#ifndef __DELAY_QUERY_USER_CHAT_RECORD_TIMER_HPP__
#define __DELAY_QUERY_USER_CHAT_RECORD_TIMER_HPP__

#include "servercommon/serverdef.h"
#include "itimermodule.h"

#include <stdlib.h>

class DelayQueryUserChatRecordTimer : public ITimerCallback
{
public:
	DelayQueryUserChatRecordTimer(int uid);
	virtual ~DelayQueryUserChatRecordTimer() {}

	void *	operator new(size_t c);
	void	operator delete(void *m);

	virtual void OnTimer();
	virtual void Free();

protected:
	int m_uid;
};

#endif

