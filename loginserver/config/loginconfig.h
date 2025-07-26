#ifndef __LOGINCONFIG_H__
#define __LOGINCONFIG_H__

#include "servercommon/serverconfig/openserverconfig.hpp"
#include "servercommon/serverconfig/nameconfig.h"

#define LOGIN_CONFIG LoginConfig::Instance()

class LoginConfig
{
public:
	static LoginConfig * Instance();

	bool Init(const std::string& path, std::string *err);
	static bool Reload(const std::string& path, std::string* err);

	OpenServerConfig * GetOpenServerConfig();

private:
	LoginConfig() {}
	~LoginConfig() {}

	OpenServerConfig m_open_server_config;
};

#endif
