#ifndef __I_ASYNC_TASK_FACTORY_HPP__
#define __I_ASYNC_TASK_FACTORY_HPP__

class IAsyncTask;
class IAsyncTaskFactory
{
public:
	virtual ~IAsyncTaskFactory() {}
	virtual IAsyncTask* Create() = 0;
};

#endif