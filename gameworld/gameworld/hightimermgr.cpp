#include "hightimermgr.h"
#include "world.h"

HighTimerMgr & HighTimerMgr::Instance()
{
	static HighTimerMgr timer_mgr;
	return timer_mgr;
}

void HighTimerMgr::Update(unsigned long now_pi_time)
{
	if (0 == m_last_update_pi_time)								// ��һ�ν���
	{
		m_last_update_pi_time = now_pi_time;
		return;
	}
	if (now_pi_time < m_last_update_pi_time + 50)				// ���50���봦��һ��
	{
		return;
	}

	int interval = now_pi_time - m_last_update_pi_time;
	m_last_update_pi_time = now_pi_time;

}

HighTimerMgr::HighTimerMgr() : m_last_update_pi_time(0)
{

}

HighTimerMgr::~HighTimerMgr()
{

}

