#include "config/loginconfig.h"

#include "servercommon/configcommon.h"
#include "engineadapter.h"
#include "utility/configpath.h"
#include "servercommon/serverconfig/nameconfig.h"

LoginConfig *gs_login_config = NULL;

LoginConfig * LoginConfig::Instance()
{
	if (NULL == gs_login_config) 
	{
		gs_login_config = new LoginConfig();
	}

	return gs_login_config;
}

bool LoginConfig::Reload(const std::string& path, std::string* err)
{
	LoginConfig *temp = new LoginConfig();

	if (!temp->Init(path, err))
	{
		delete temp; temp = NULL;
		return false;
	}

	if (NULL != gs_login_config) delete gs_login_config;

	gs_login_config = temp;

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool LoginConfig::Init(const std::string& path, std::string *err)
{
	std::string configpath;

	configpath = path + "openserver.xml";
	if (!m_open_server_config.Init(configpath, err))
	{
		return false;
	}

	return true;
}

OpenServerConfig* LoginConfig::GetOpenServerConfig() 
{ 
	return &m_open_server_config; 
}