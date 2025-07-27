#ifndef __MUTEX_LOCK_H__
#define __MUTEX_LOCK_H__

#ifdef __linux__
#include <pthread.h>
#endif

#ifdef _MSC_VER
#include <mutex>
#endif

class MutexLock
{
public:
	MutexLock();
	~MutexLock();

	bool Lock();
	bool Unlock();

#ifdef __linux__
	pthread_mutex_t& GetMutex() { return m_mutex; }
#endif

#ifdef _MSC_VER
	std::mutex& GetMutex() { return m_mutex; }
#endif

private:
	MutexLock(const MutexLock&);
	MutexLock& operator=(const MutexLock&);

#ifdef __linux__
	pthread_mutex_t m_mutex;
#endif

#ifdef _MSC_VER
	std::mutex m_mutex;
#endif
};

#endif