#include "monitor.hpp"
#include "systemmonitor.hpp"
#include "obj/character/role.h"
#include "gameworld/gamelog.h"
#include "engineadapter.h"
#include "item/knapsack.h"
#include "other/rolemodulemanager.hpp"
#include "item/money.h"

Monitor::Monitor()
	: m_module_mgr(NULL), m_next_monitor_time(0)
{
	memset(m_monitor_num_list, 0, sizeof(m_monitor_num_list));
}

Monitor::~Monitor()
{

}

void Monitor::Init(RoleModuleManager* module_mgr, const MonitorParam &monitor_param)
{
	m_module_mgr = module_mgr;

	m_next_monitor_time = monitor_param.next_monitor_time;
	memcpy(m_monitor_num_list, monitor_param.monitor_num_list, sizeof(m_monitor_num_list));
}

void Monitor::GetMonitorParam(MonitorParam *monitor_param)
{
	monitor_param->next_monitor_time = m_next_monitor_time;
	memcpy(monitor_param->monitor_num_list, m_monitor_num_list, sizeof(monitor_param->monitor_num_list));
}

void Monitor::Update(time_t now_second)
{
	if (now_second > m_next_monitor_time)
	{
		this->MonitorLog();
	}
}

void Monitor::OnResetData(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid != now_dayid)
	{
		memset(m_monitor_num_list, 0, sizeof(m_monitor_num_list));
	}
}

void Monitor::AddMonitorNum(short monitor_type, int add_num)
{
	if (monitor_type < 0 || monitor_type >= MONITOR_TYPE_MAX || add_num <= 0 || !this->CanMonitorLog()) return;

	m_monitor_num_list[monitor_type] += add_num;

	SystemMonitor::Instance().AddMonitorNum(monitor_type, add_num);
}

void Monitor::MonitorLog()
{
	if (this->CanMonitorLog())
	{
		Money *money = m_module_mgr->GetMoney();

		gamelog::g_log_monitor.printf(LL_INFO, "[UserMonitor] [user_id:%d user_name:%s] [level:%d] [gold:%lld immortal_coin:%lld coin_bind:%lld]"
			" gold_get:%d gold_consum:%d immortalcoin_get:%d immortalcoin_consum:%d coinbind_get:%d coinbind_consum:%d",
			UidToInt(m_module_mgr->GetRole()->GetUserId()), m_module_mgr->GetRole()->GetName(), m_module_mgr->GetRole()->GetLevel(), money->GetGold(), money->GetImmortalCoin(), money->GetCoinBind(),
			m_monitor_num_list[MONITOR_TYPE_DAY_GOLD_GET], m_monitor_num_list[MONITOR_TYPE_DAY_GOLD_CONSUME], 
			m_monitor_num_list[MONITOR_TYPE_DAY_IMMORTALCOIN_GET], m_monitor_num_list[MONITOR_TYPE_DAY_IMMORTALCOIN_CONSUME],
			m_monitor_num_list[MONITOR_TYPE_DAY_COINBIND_GET], m_monitor_num_list[MONITOR_TYPE_DAY_COINBIND_CONSUME]);
	}

	m_next_monitor_time = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + MONITOR_LOG_INTERVAL;
}

bool Monitor::CanMonitorLog()
{
	return (Role::AUTHORITY_TYPE_GM != m_module_mgr->GetRole()->GetAuthorityType() && Role::AUTHORITY_TYPE_TEST != m_module_mgr->GetRole()->GetAuthorityType());
}
