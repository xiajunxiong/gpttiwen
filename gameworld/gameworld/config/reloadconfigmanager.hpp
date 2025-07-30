#ifndef __RELOAD_CONFIG_MANAGER_HPP__
#define __RELOAD_CONFIG_MANAGER_HPP__

class ReloadConfigManager
{
public:
	static ReloadConfigManager & Instance();

	bool ReloadConfig(int reload_cfg_type, int param1, int param2);
	void OnStartCalcTime(bool is_check_reload_times, int catch_type);
	long long OnEndCalcTime(bool is_check_reload_times, int catch_type, const char * reason, int param = -1);

private:
	ReloadConfigManager();
	~ReloadConfigManager();
};

#endif //__RELOAD_CONFIG_MANAGER_HPP__



