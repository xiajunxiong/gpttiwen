#include "asynctaskfactorymanager.hpp"
#include "servercommon/multithread/mutex_guard.h"
#include "iasynctaskfactory.hpp"
#include "servercommon/multithread/mutex_lock.h"
#include <cassert>
#include "servercommon/basedef.h"

#ifdef __linux__
static AsyncTaskFactoryManager* g_async_task_factory_mgr_instance = NULL;
static MutexLock g_async_task_factory_mgr_instance_mutex;
#endif

AsyncTaskFactoryManager::~AsyncTaskFactoryManager()
{
	MutexGuard guard(&m_mutex);
	FactoryMap::iterator it = m_factory.begin();
	for (; it != m_factory.end(); ++it)
	{
		IAsyncTaskFactory* factory = it->second;
		delete factory;
		it->second = NULL;
	}
}

AsyncTaskFactoryManager& AsyncTaskFactoryManager::Instance()
{
#ifdef __linux__
	// linux下暂时没有支持c++11,所以采用双检锁+内存屏障保证不出现乱序执行new AsyncTaskManager()导致的问题
	if (NULL == g_async_task_factory_mgr_instance)
	{
		MutexGuard guard(&g_async_task_factory_mgr_instance_mutex);
		barrier();
		if (NULL == g_async_task_factory_mgr_instance)
		{
			g_async_task_factory_mgr_instance = new AsyncTaskFactoryManager();
		}
	}

	return *g_async_task_factory_mgr_instance;
#endif

#ifdef _MSC_VER
	static AsyncTaskFactoryManager instance; // c++11后保证正确线程安全，只会有一个实例
	return instance;
#endif
}

bool AsyncTaskFactoryManager::RegisterFactory(const std::string& name, IAsyncTaskFactory* factory)
{
	MutexGuard guard(&m_mutex);

	if (m_factory.find(name) != m_factory.end())
	{
#ifdef _DEBUG
		assert(0); // 工厂重复了
#endif
		return false; // 重复了
	}

	m_factory.insert(FactoryMap::value_type(name, factory));
	return true;
}

IAsyncTask* AsyncTaskFactoryManager::Create(const std::string& name)
{
	IAsyncTask* ret_task = NULL;

	{
		MutexGuard guard(&m_mutex);
		FactoryMap::iterator it = m_factory.find(name);
		if (it != m_factory.end())
		{
			IAsyncTaskFactory* factory = it->second;
			ret_task = factory->Create();
		}
	}

#ifdef _DEBUG
	assert(NULL != ret_task); // 是不是没有注册对应的工厂
#endif

	return ret_task;
}

