#include "reloadconfigmanager.hpp"
#include "utility/configpath.h"
#include "server/spidconfig.h"
#include "engineadapter.h"
#include "server/loginlog.h"
#include "servercommon/serverconfig/appearanceconfig.hpp"

ReloadConfigManager::ReloadConfigManager()
{

}

ReloadConfigManager::~ReloadConfigManager()
{

}

ReloadConfigManager & ReloadConfigManager::Instance()
{
	static ReloadConfigManager inst;
	return inst;
}

bool ReloadConfigManager::ReloadConfig(int reload_cfg_type, int param1, int param2)
{
	std::string config_dir = "../config";
	if (!EngineAdapter::Instance().ConfigGetValue(ROOT / "ConfigDir", &config_dir))
	{
		loginlog::g_log_cmd.printf(LL_ERROR, "SpidConfig::Reload ConfigDir Read Failed!!!");
		return false;
	}

	if (config_dir[config_dir.length() - 1] != '/')
	{
		config_dir += '/';
	}

	std::string err;

	switch (reload_cfg_type)
	{
	case RELOAD_CONFIG_TYPE_SPID_CONFIG: // ÷ÿ∂¡«˛µ¿≈‰÷√ agent_adapt.xml 
		{
			std::string config_name = config_dir + "loginserver/agent_adapt.xml";

			if (!SpidConfig::Reload(config_name, &err))
			{
				loginlog::g_log_cmd.printf(LL_ERROR, err.c_str());
				loginlog::g_log_cmd.printf(LL_ERROR, "SpidConfig::Reload return FALSE.");
				return false;
			}

			loginlog::g_log_cmd.printf(LL_INFO, "SpidConfig::Reload Success.");
		}
		break;
	case RELOAD_CONFIG_TYPE_APPEARANCE_CONFIG:
		{
			std::string appearance_cfg_dir = config_dir + "/gameworld/logicconfig/";

			if (!AppearanceConfig::Instance().Reload(appearance_cfg_dir, "image_change.xml", &err))
			{
				loginlog::g_log_cmd.printf(LL_ERROR, err.c_str());
				loginlog::g_log_cmd.printf(LL_ERROR, "AppearanceConfig::Reload return FALSE.");
				return false;
			}
			loginlog::g_log_cmd.printf(LL_INFO, "AppearanceConfig::Reload Success.");
		}
		break;
	}

	return true;
}
