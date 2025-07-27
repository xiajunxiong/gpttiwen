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
	// ��ʱ����������ƶ���ֵ��m_unique_lock����ʱ��������ʱ�������ˣ�
	m_unique_lock = std::unique_lock<std::mutex>(mutex->GetMutex());
}
#endif

#ifdef _MSC_VER
MutexGuard::~MutexGuard()
{
	m_unique_lock.unlock();
}
#endif


