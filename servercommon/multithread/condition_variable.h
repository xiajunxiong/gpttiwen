#ifndef __CONDITION_VARIABLE_H__
#define __CONDITION_VARIABLE_H__

#ifdef __linux__
#include <pthread.h>
#endif

#ifdef _MSC_VER
#include <condition_variable>
#endif

class MutexLock;
class ConditionVariable
{
public:
	ConditionVariable();
	~ConditionVariable();

	void Wait(MutexLock& mutex);
	void Notify();
	void NotifyAll();

private:

#ifdef __linux__
	pthread_cond_t m_cond;
#endif

#ifdef _MSC_VER
	std::condition_variable m_cond;
#endif

};

#endif