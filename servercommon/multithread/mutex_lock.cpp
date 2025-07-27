#include "mutex_lock.h"
#include <cassert>

#ifdef __linux__
MutexLock::MutexLock()
{
	pthread_mutex_init(&m_mutex, NULL);
}
#endif

#ifdef _MSC_VER
MutexLock::MutexLock()
{
	
}
#endif

MutexLock::~MutexLock()
{
#ifdef __linux__
	pthread_mutex_destroy(&m_mutex);
#endif
}

bool MutexLock::Lock()
{
#ifdef _MSC_VER	
	m_mutex.lock();
	return true;
#endif

#ifdef __linux__
	int err = pthread_mutex_lock(&m_mutex);
	if (0 != err)
	{
		return false;
	}
	return true;
#endif
}

bool MutexLock::Unlock()
{
#ifdef _MSC_VER
	m_mutex.unlock();
	return true;
#endif

#ifdef __linux__
	int err = pthread_mutex_unlock(&m_mutex);
	if (0 != err)
	{
		return false;
	}
	return true;
#endif
}

