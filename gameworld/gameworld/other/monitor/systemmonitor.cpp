#include "systemmonitor.hpp"

#include "gameworld/gamelog.h"
#include "obj/character/role.h"

#include "world.h"
#include "engineadapter.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "global/cross/crossusershadow.hpp"
#include "internalcomm.h"
#include "other/rolecross/rolecross.hpp"
#include "other/rolemodulemanager.hpp"
#include "globalconfig/spidconfig.h"
#include "servercommon/internalprotocal/lgprotocal.h"

SystemMonitor::SystemMonitor()
	: m_monitor_time(0), m_next_day_interval(0), 
	m_plat_online_print_time(0), m_next_sync_online_role_num_timestamp(0)
{
	memset(m_monitor_num_list, 0, sizeof(m_monitor_num_list));

	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	m_plat_online_print_time = now + EngineAdapter::Instance().NextMinuteInterval(0);
	m_cross_plat_online_sync_time = now + EngineAdapter::Instance().NextMinuteInterval(0) - 2; // 提早2秒同步到游戏服

	m_next_sync_online_role_num_timestamp = now + 5;
}

SystemMonitor::~SystemMonitor()
{

}

SystemMonitor::PlatOnlineMap* SystemMonitor::CrossGetPlatOnlineMap(const UniqueServerID& usid)
{
	CrossPlatOnlineMap::iterator it = m_cross_plat_online_map.find(usid);
	if (m_cross_plat_online_map.end() != it)
	{
		return &it->second;
	}
	else
	{
		it = m_cross_plat_online_map.insert(CrossPlatOnlineMap::value_type(usid, PlatOnlineMap())).first;
		return &it->second;
	}
}

SystemMonitor & SystemMonitor::Instance()
{
	static SystemMonitor obj;
	return obj;
}

void SystemMonitor::Update(unsigned long interval, unsigned int now_second)
{
	{
		m_monitor_time += interval;

		if (m_monitor_time > MONITOR_LOG_INTERVAL * 1000)
		{
			gamelog::g_log_monitor.printf(LL_INFO, "[SystemMonitor] gold_get:%d gold_consum:%d immortalcoin_get:%d immortalcoin_consum:%d coinbind_get:%d coinbind_consum:%d",
				m_monitor_num_list[MONITOR_TYPE_DAY_GOLD_GET], m_monitor_num_list[MONITOR_TYPE_DAY_GOLD_CONSUME], 
				m_monitor_num_list[MONITOR_TYPE_DAY_IMMORTALCOIN_GET], m_monitor_num_list[MONITOR_TYPE_DAY_IMMORTALCOIN_CONSUME],
				m_monitor_num_list[MONITOR_TYPE_DAY_COINBIND_GET], m_monitor_num_list[MONITOR_TYPE_DAY_COINBIND_CONSUME]);

			m_monitor_time = 0;
		}
	}

	{
		m_next_day_interval -= interval;

		if (m_next_day_interval <= 0)
		{
			memset(m_monitor_num_list, 0, sizeof(m_monitor_num_list));

			m_next_day_interval = EngineAdapter::Instance().NextDayInterval(0, 0, 0) * 1000;
		}
	}

	{
		this->PrintPlatOnline(interval, now_second);
	}

	if (now_second > m_next_sync_online_role_num_timestamp)
	{
		m_next_sync_online_role_num_timestamp = now_second + 5;
		this->SyncOnlineRoleNumToLogin();
	}
}

void SystemMonitor::AddMonitorNum(short monitor_type, int add_num)
{
	if (monitor_type < 0 || monitor_type >= MONITOR_TYPE_MAX || add_num <= 0) return;

	m_monitor_num_list[monitor_type] += add_num;
}

void SystemMonitor::OnUserLogin(const UserID& user_id)
{
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
	if (NULL != role)
	{
		this->OnRoleLogin(role);
	}
}

void SystemMonitor::OnRoleLogin(Role *role)
{
	int index = role->GetPlatSpid();
	if (0 == index) return;

	PlatOnlineMapIt it = m_plat_online_map.find(index);
	if (m_plat_online_map.end() != it) 
	{
		it->second.online_role_num += 1;
	} 
	else if ((int)m_plat_online_map.size() < PLAT_ONLINE_MAX) 
	{
		m_plat_online_map[index].online_role_num = 1;
	}

	if (CrossConfig::Instance().IsHiddenServer())
	{
		int local_plat_type = role->GetRoleModuleManager()->GetRoleCross()->GetOriginPlatType();
		int local_server_id = role->GetRoleModuleManager()->GetRoleCross()->GetOriginServerId();
		UniqueServerID usid(local_plat_type, local_server_id);
		PlatOnlineMap* pom = this->CrossGetPlatOnlineMap(usid);
		if (NULL != pom)
		{
			(*pom)[index].online_role_num += 1;
		}
	}
}

void SystemMonitor::OnRoleLogout(Role *role)
{
	int index = role->GetPlatSpid();
	if (0 == index) return;

	PlatOnlineMapIt it = m_plat_online_map.find(index);
	if (m_plat_online_map.end() != it) 
	{
		it->second.online_role_num -= 1;
	} 

	if (CrossConfig::Instance().IsHiddenServer())
	{
		int local_plat_type = role->GetRoleModuleManager()->GetRoleCross()->GetOriginPlatType();
		int local_server_id = role->GetRoleModuleManager()->GetRoleCross()->GetOriginServerId();
		UniqueServerID usid(local_plat_type, local_server_id);
		PlatOnlineMap* pom = this->CrossGetPlatOnlineMap(usid);
		if (NULL != pom)
		{
			(*pom)[index].online_role_num -= 1;
		}
	}
}

void SystemMonitor::OnSyncOnlineInfoFromGame(int plat_type, int server_id, int online_role_num, int incross_role_num, int crossing_role_num)
{
	UniqueServerID usid(plat_type, server_id);
	OriginServerOnlineInfo& info = m_origin_server_online_map[usid];

	info.online_role_num = online_role_num;
	info.incross_role_num = incross_role_num;
	info.crossing_role_num = crossing_role_num;
	info.total_online_role_num = online_role_num + incross_role_num + crossing_role_num;
}

void SystemMonitor::OnSyncPlatInCrossNumFromHidden(int sub_plat_type, int incross_num)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	m_plat_online_map[sub_plat_type].incross_role_num = incross_num;

	PlatName sub_plat_name = { 0 };
	globalconfig::SpidConfig::GetPlatNameBySpid(sub_plat_type, sub_plat_name);
	gamelog::g_log_monitor.printf(LL_INFO, "SystemMonitor::OnSyncPlatInCrossNumFromHidden local_plat_type[%s] sub_plat_type[%s] incross_num[%d]",
		LocalConfig::Instance().GetPlatName(), sub_plat_name, incross_num);
}

int SystemMonitor::GetTotalOnlineRoleNum()
{
	int total_num = 0;
	OriginServerOnlineMap::iterator it = m_origin_server_online_map.begin();
	for (; it != m_origin_server_online_map.end(); ++it)
	{
		total_num += it->second.total_online_role_num;
	}
	return total_num;
}

int SystemMonitor::GetInCrossRoleNum()
{
	int total_num = 0;
	OriginServerOnlineMap::iterator it = m_origin_server_online_map.begin();
	for (; it != m_origin_server_online_map.end(); ++it)
	{
		total_num += it->second.incross_role_num;
	}
	return total_num;
}

void SystemMonitor::PrintPlatOnline(unsigned long interval, unsigned int now_second)
{
	if (m_plat_online_print_time < now_second)  // 每分钟的0秒整打印一次
	{
		m_plat_online_print_time = now_second + 30;

		int online_role_num = World::GetInstWorld()->GetSceneManager()->GetOnlineRoleNum();
		int max_role_num = World::GetInstWorld()->GetSceneManager()->GetMaxRoleNum();

		int incross_role_num = 0;
		int crossing_role_num = 0;
		if (!CrossConfig::Instance().IsHiddenServer())
		{
			incross_role_num = CrossUserShadow::Instance().GetInCrossUserNum();
			crossing_role_num =  CrossUserShadow::Instance().GetEnteringCrossUserNum();
		}
		int total_role_num = online_role_num + incross_role_num + crossing_role_num;

		gamelog::g_log_role_num.buff_printf("[%d/%d] ", total_role_num, max_role_num); // 子后台用
		gamelog::g_log_role_num.commit_buff(LL_INFO);

		LOG_QUICK8(gamelog::g_log_quick, LOG_TYPE_ONLINE_ROLE_NUM, total_role_num, NULL, NULL, 0, 0, NULL, NULL);  // 总在线

		for (PlatOnlineMapIt it = m_plat_online_map.begin(); m_plat_online_map.end() != it; ++it)  // 平台在线
		{
			int spid = it->first;
			char plat[sizeof(int)] = { 0 };
			globalconfig::SpidConfig::GetPlatNameBySpid(spid, plat);
			plat[sizeof(plat) - 1] = '\0';
			LOG_QUICK8(gamelog::g_log_quick, LOG_TYPE_PLAT_ONLINE, it->second.incross_role_num + it->second.online_role_num, plat, NULL, 0, 0, NULL, NULL);
		}

		if (!CrossConfig::Instance().IsHiddenServer())
		{
			this->SyncOnlineInfoToHidden(online_role_num, incross_role_num, crossing_role_num);
		}
	}

	if (CrossConfig::Instance().IsHiddenServer())
	{
		if (m_cross_plat_online_sync_time < now_second)
		{
			m_cross_plat_online_sync_time = now_second + EngineAdapter::Instance().NextMinuteInterval(0) - 2; // 提早2秒同步到游戏服
			
			CrossPlatOnlineMap::iterator it = m_cross_plat_online_map.begin();
			for (; it != m_cross_plat_online_map.end(); ++it)
			{
				PlatOnlineMap::iterator iter = it->second.begin();
				for (; iter != it->second.end(); ++iter)
				{
					crossgameprotocal::HiddenGameSyncPlatInCrossRoleNum msg;
					msg.incross_role_num = iter->second.online_role_num; // 跨服中的online_num就是原服的incross_num
					msg.sub_plat_type = iter->first;
					
					InternalComm::Instance().SendToGameThroughCross(it->first.plat_type, it->first.server_id, (const char*)&msg, sizeof(msg));
				
					PlatName plat_name = { 0 };
					PlatName sub_plat_name = { 0 };
					globalconfig::SpidConfig::GetPlatNameBySpid(it->first.plat_type, plat_name);
					globalconfig::SpidConfig::GetPlatNameBySpid(msg.sub_plat_type, sub_plat_name);
					gamelog::g_log_monitor.printf(LL_INFO, "SystemMonitor::PrintPlatOnline plat_type[%s] sub_plat_type[%s] incross_num[%d]",
						plat_name, sub_plat_name, msg.incross_role_num);
				}
			}
		}
	}
}

void SystemMonitor::SyncOnlineInfoToHidden(int online_role_num, int incross_role_num, int crossing_role_num)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::GameHiddenSyncOnlineInfo msg;
	msg.plat_type = LocalConfig::Instance().GetPlatType();
	msg.server_id = LocalConfig::Instance().GetMergeServerId();

	msg.online_role_num = online_role_num;
	msg.incross_role_num = incross_role_num;
	msg.crossing_role_num = crossing_role_num;

	InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
}

void SystemMonitor::SyncOnlineRoleNumToLogin()
{
	lgprotocal::GLSyncOnlineRoleNum msg;
	int online_role_num = World::GetInstWorld()->GetSceneManager()->GetOnlineRoleNum();
	int incross_role_num = 0;
	int crossing_role_num = 0;
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		incross_role_num = CrossUserShadow::Instance().GetInCrossUserNum();
		crossing_role_num = CrossUserShadow::Instance().GetEnteringCrossUserNum();
	}
	msg.online_role_num = online_role_num + incross_role_num + crossing_role_num;

	NetID netid = InternalComm::Instance().m_login_server_netid;
	InternalComm::Instance().NetSend(netid, (const char*)&msg, sizeof(msg));
}

