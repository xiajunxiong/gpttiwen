#ifndef __RELOAD_CONFIG_MANAGER_HPP__
#define __RELOAD_CONFIG_MANAGER_HPP__

class ReloadConfigManager
{
public:
	static ReloadConfigManager & Instance();

	bool ReloadConfig(int reload_cfg_type, int param1, int param2);

private:
	ReloadConfigManager();
	~ReloadConfigManager();
};

#endif //__RELOAD_CONFIG_MANAGER_HPP__



