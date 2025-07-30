#include "servercommon/redis/redismanager.hpp"
#include "asyncredistest.hpp"
#include "gamelog.h"

AsyncRedisTest::AsyncRedisTest()
{
	
}

AsyncRedisTest::~AsyncRedisTest()
{

}

int AsyncRedisTest::DoTask()
{
	std::string key = STRING_SPRINTF("AsyncTest_%d", role_id);
	if (!RedisManager::Instance().SET(key, "1"))
	{
		is_succ = false;
		return -1;
	}

	if (!RedisManager::Instance().GET(key, ret_value))
	{
		is_succ = false;
		return -2;
	}

	return 0;
}

int AsyncRedisTest::DoCallBack()
{
	gamelog::g_log_debug.printf(LL_INFO, "AsyncRedisTest::DoCallBack role_id[%d] is_succ[%s] ret_value[%s]",
		role_id, is_succ ? "Y" : "N", ret_value.c_str());

	return 0;
}

void AsyncRedisTest::Destroy()
{
	delete this;
}

IAsyncTask* AsyncRedisTestFactory::Create()
{
	return new AsyncRedisTest();
}
