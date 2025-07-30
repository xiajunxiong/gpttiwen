#include "delayreloadconfigtimer.hpp"
#include "config/reloadconfigmanager.hpp"

DelayReloadConfigTimer::DelayReloadConfigTimer(int reload_type, int param1, int param2)
	: m_reload_type(reload_type), m_param1(param1), m_param2(param2)
{

}

void DelayReloadConfigTimer::OnTimer()
{
	ReloadConfigManager::Instance().ReloadConfig(m_reload_type, m_param1, m_param2);
}

void DelayReloadConfigTimer::Free()
{
	delete this;
}
