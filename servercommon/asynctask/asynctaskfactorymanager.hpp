#ifndef __ASYNC_TASK_FACTORY_MANAGER_HPP__
#define __ASYNC_TASK_FACTORY_MANAGER_HPP__

#include <map>
#include <string>
#include "servercommon/multithread/mutex_lock.h"

class IAsyncTaskFactory;
class IAsyncTask;
class AsyncTaskFactoryManager
{
public:
	typedef std::map<std::string, IAsyncTaskFactory*> FactoryMap;

	AsyncTaskFactoryManager() {}
	~AsyncTaskFactoryManager();

	static AsyncTaskFactoryManager& Instance();
	bool RegisterFactory(const std::string& name, IAsyncTaskFactory* factory);
	IAsyncTask* Create(const std::string& name);
private:
	AsyncTaskFactoryManager& operator=(const AsyncTaskFactoryManager&);
	AsyncTaskFactoryManager(const AsyncTaskFactoryManager&);

	FactoryMap m_factory;
	MutexLock m_mutex;
};

#endif