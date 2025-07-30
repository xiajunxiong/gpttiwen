#include "logicconfigreloader.hpp"
#include "logicconfigmanager.hpp"
#include "servercommon/multithread/mutex_guard.h"
#include "servercommon/multithread/mutex_lock.h"
#include "common/platform/thread/thread.h"
#include "gamelog.h"

LogicConfigReloader::LogicConfigReloader(const std::string& path) :m_path(path)
{

}

LogicConfigReloader::~LogicConfigReloader()
{

}

DWORD LogicConfigReloader::StartReload(void* reloader_ptr)
{
	LogicConfigReloader* reloader = (LogicConfigReloader*)reloader_ptr;
	reloader->Run();
	delete reloader;
	return NULL;
}

LogicConfigManager *g_async_logicconfig_manager = NULL;
volatile bool g_async_reload_logicconfig_done = false;
MutexLock g_LogicConfigReloadMutex;
void LogicConfigReloader::Run()
{
	LogicConfigManager *temp = new LogicConfigManager();
	std::string errormsg;
	
	// Init似乎存在线程不安全的成份，若临界区不包含Init，连续多次重读配置会造成宕机
	//（但临界区包含Init时,在连续多次重读配置时逻辑线程会阻塞）
	MutexGuard guard(&g_LogicConfigReloadMutex);
	barrier();
	if (g_async_logicconfig_manager != NULL && g_async_reload_logicconfig_done)
	{
		// 如果已重读配置，则放弃本次重读（这样可以避免连续多次重读配置的情况）
		gamelog::g_log_world.printf(LL_INFO, "LogicConfigReloader::Run Already Reload By Another Thread, Abort This Time, Cur Thread Id[%lu]", Thread::GetCurrentThreadID());
		delete temp; temp = NULL;
		return;
	}

	if (!temp->Init(m_path, &errormsg, false, true))
	{
		delete temp; temp = NULL;

		gamelog::g_log_world.printf(LL_INFO, "LogicConfigReloader::Run Reload Failed errormsg[%s], Cur Thread Id[%lu]", errormsg.c_str(), Thread::GetCurrentThreadID());
		return;
	}

	barrier();
	if (NULL != g_async_logicconfig_manager) delete g_async_logicconfig_manager;

	g_async_logicconfig_manager = temp;
	g_async_reload_logicconfig_done = true;
	
	gamelog::g_log_world.printf(LL_INFO, "LogicConfigReloader::Run Reload succ, Cur Thread Id[%lu], Wait for Substitution", Thread::GetCurrentThreadID());
}

