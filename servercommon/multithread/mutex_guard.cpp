#include "mutex_guard.h"
#include "mutex_lock.h"

#ifdef __linux__
MutexGuard::MutexGuard(MutexLock* mutex) : m_mutex_ptr(mutex)
{
	m_mutex_ptr->Lock();
}
#endif

#ifdef __linux__
MutexGuard::~MutexGuard()
{
	m_mutex_ptr->Unlock();
}
#endif

#ifdef _MSC_VER
MutexGuard::MutexGuard(MutexLock* mutex) : m_mutex_ptr(mutex)
{
	// 临时变量构造后移动赋值给m_unique_lock（临时变量构造时就上锁了）
	m_unique_lock = std::unique_lock<std::mutex>(mutex->GetMutex());
}
#endif

#ifdef _MSC_VER
MutexGuard::~MutexGuard()
{
	m_unique_lock.unlock();
}
#endif


