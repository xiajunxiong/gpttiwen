#ifndef __ASYNC_REDIS_TEST_HPP__
#define __ASYNC_REDIS_TEST_HPP__

#include "servercommon/asynctask/iasynctask.hpp"
#include "servercommon/asynctask/iasynctaskfactory.hpp"
#include <string>


class AsyncRedisTestFactory;
class AsyncRedisTest : public IAsyncTask
{
public:
	friend class AsyncRedisTestFactory;

	AsyncRedisTest();
	virtual ~AsyncRedisTest();

	void *	operator new(size_t c);
	void	operator delete(void *m);

	virtual int DoTask();
	virtual int DoCallBack();
	virtual void Destroy();

	int role_id;
	bool is_succ;
	std::string ret_value;
};

class AsyncRedisTestFactory : public IAsyncTaskFactory
{
public:
	virtual IAsyncTask* Create();
};

#endif