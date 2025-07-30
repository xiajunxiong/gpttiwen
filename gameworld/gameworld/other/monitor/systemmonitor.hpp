#ifndef __SYSTEMMONITOR_HPP__
#define __SYSTEMMONITOR_HPP__

#include "servercommon/monitordef.hpp"
#include <map>

struct OriginServerOnlineInfo
{
	OriginServerOnlineInfo() : online_role_num(0), incross_role_num(0),
		crossing_role_num(0), total_online_role_num(0)
	{

	}

	int online_role_num;
	int incross_role_num;
	int crossing_role_num;
	int total_online_role_num;
};

struct PlatOnlineInfo
{
	PlatOnlineInfo() : online_role_num(0), incross_role_num(0)
	{

	}

	int online_role_num;
	int incross_role_num;
};

class Role;

class SystemMonitor
{
public:
	static const int PLAT_ONLINE_MAX = 64;
	typedef std::map<int, PlatOnlineInfo> PlatOnlineMap;
	typedef PlatOnlineMap::iterator PlatOnlineMapIt;

	typedef std::map<UniqueServerID, PlatOnlineMap> CrossPlatOnlineMap;
	typedef std::map<UniqueServerID, OriginServerOnlineInfo> OriginServerOnlineMap;

public:
	static SystemMonitor & Instance();

	void Update(unsigned long interval, unsigned int now_second);

	void AddMonitorNum(short monitor_type, int add_num);

	void OnUserLogin(const UserID& user_id);
	void OnRoleLogin(Role* role);
	void OnRoleLogout(Role *role);

	void OnSyncOnlineInfoFromGame(int plat_type, int server_id, int online_role_num, int incross_role_num,
		int crossing_role_num);
	void OnSyncPlatInCrossNumFromHidden(int sub_plat_type, int incross_num);
	int GetTotalOnlineRoleNum();
	int GetInCrossRoleNum();

private:
	SystemMonitor();
	~SystemMonitor();

	PlatOnlineMap* CrossGetPlatOnlineMap(const UniqueServerID& usid);

	unsigned int m_monitor_time;
	int m_next_day_interval;
	int m_monitor_num_list[MONITOR_TYPE_MAX];

	PlatOnlineMap m_plat_online_map;
	unsigned int m_plat_online_print_time;
	
	CrossPlatOnlineMap m_cross_plat_online_map;
	unsigned int m_cross_plat_online_sync_time;

	OriginServerOnlineMap m_origin_server_online_map;

	unsigned int m_next_sync_online_role_num_timestamp;

	void PrintPlatOnline(unsigned long interval, unsigned int now_second);
	void SyncOnlineInfoToHidden(int online_role_num, int incross_role_num, int crossing_role_num);

	void SyncOnlineRoleNumToLogin();
};

#endif // __SYSTEMMONITOR_HPP__

