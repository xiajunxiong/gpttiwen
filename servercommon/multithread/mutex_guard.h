#ifndef __MUTEX_GUARD_H__
#define __MUTEX_GUARD_H__

#ifdef _MSC_VER
#include <mutex>
#endif

class MutexLock;
class MutexGuard 
{
public:
	explicit MutexGuard(MutexLock* mutex);
	~MutexGuard();

private:
	MutexGuard(const MutexGuard&);
	MutexGuard& operator=(const MutexGuard&);

	MutexLock* m_mutex_ptr;
	
#ifdef _MSC_VER
	std::unique_lock<std::mutex> m_unique_lock;
#endif
};

#endif