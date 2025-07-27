#include "condition_variable.h"
#include "mutex_lock.h"

#include <assert.h>

#ifdef _MSC_VER
#include <mutex>
#endif

ConditionVariable::ConditionVariable()
{
#ifdef __linux__
	int err = pthread_cond_init(&m_cond, NULL);
	assert(0 == err);
#endif
}

ConditionVariable::~ConditionVariable()
{
#ifdef __linux__
	int err = pthread_cond_destroy(&m_cond);
	assert(0 == err);
#endif
}

void ConditionVariable::Wait(MutexLock& mutex)
{
#ifdef __linux__
	pthread_cond_wait(&m_cond, &mutex.GetMutex());
#endif

#ifdef _MSC_VER
	std::unique_lock<std::mutex> ul(mutex.GetMutex(), std::defer_lock); // defer_lock表示只拥有锁但不上锁，外部调用Wait前应保证已上锁
	m_cond.wait(ul);
#endif
}

void ConditionVariable::Notify()
{
#ifdef __linux__
	pthread_cond_signal(&m_cond);
#endif

#ifdef _MSC_VER
	m_cond.notify_one();
#endif
}

void ConditionVariable::NotifyAll()
{
#ifdef __linux__
	pthread_cond_broadcast(&m_cond);
#endif

#ifdef _MSC_VER
	m_cond.notify_all();
#endif
}

