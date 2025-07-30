#ifndef __HIGH_TIMER_MGR_H__
#define __HIGH_TIMER_MGR_H__

#include <map>
#include "servercommon/serverdef.h"

class HighTimerMgr
{
public:

	static HighTimerMgr & Instance();

	void Update(unsigned long now_pi_time);

private:
	HighTimerMgr();
	~HighTimerMgr();

	unsigned long m_last_update_pi_time;
};

#endif  // __HIGH_TIMER_MGR_H__

