#ifndef __LOGIC_CONFIG_RELOADER_HPP__
#define __LOGIC_CONFIG_RELOADER_HPP__

#include <string>
#include "common/platform/thread/thread.h"

class LogicConfigManager;
class LogicConfigReloader
{
public:
	explicit LogicConfigReloader(const std::string& path);
	~LogicConfigReloader();
	
	static DWORD StartReload(void* reloader_ptr);
	void Run();
private:
	std::string m_path;
};

#endif