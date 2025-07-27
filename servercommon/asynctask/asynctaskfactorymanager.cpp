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
	// linux����ʱû��֧��c++11,���Բ���˫����+�ڴ����ϱ�֤����������ִ��new AsyncTaskManager()���µ�����
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
	static AsyncTaskFactoryManager instance; // c++11��֤��ȷ�̰߳�ȫ��ֻ����һ��ʵ��
	return instance;
#endif
}

bool AsyncTaskFactoryManager::RegisterFactory(const std::string& name, IAsyncTaskFactory* factory)
{
	MutexGuard guard(&m_mutex);

	if (m_factory.find(name) != m_factory.end())
	{
#ifdef _DEBUG
		assert(0); // �����ظ���
#endif
		return false; // �ظ���
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
	assert(NULL != ret_task); // �ǲ���û��ע���Ӧ�Ĺ���
#endif

	return ret_task;
}

