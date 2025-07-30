#include "loginsystem.hpp"
#include "engineadapter.h"
#include "config/logicconfigmanager.hpp"
#include "other/rolemodulemanager.hpp"
#include "obj/character/role.h"
#include "gameworld/gamelog.h"
#include "protocol/msgother.h"
#include "global//randactivity/randactivitymanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "item/knapsack.h"
#include <vector>

LoginSystem::LoginSystem() : m_role_module_mgr(NULL)
{
	m_param.Reset();
}

LoginSystem::~LoginSystem()
{
}

void LoginSystem::Init(RoleModuleManager *role_module_manager,const LoginSystemParam &param)
{
	m_role_module_mgr = role_module_manager;
	m_param = param;

	if (m_param.reward_flag != 0)
	{
		for (int i = 0; i < MAX_LOGIN_SYSTEM_NUM; ++i)
		{
			if (m_param.reward_flag & (1 << i))
			{
				m_param.is_reward_flag.SetBit(i);
			}
		}
		m_param.reward_flag = 0;
	}
}

void LoginSystem::GetOtherInitParam(LoginSystemParam * param)
{
	*param = m_param;
}

void LoginSystem::OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;
}

void LoginSystem::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;

	m_param.login_day += 1;

	this->OnRefresh();
}

void LoginSystem::OnRoleLogin()
{
	this->OnRefresh();
}

void LoginSystem::OnGetReward(int login_day)
{
	if (m_param.login_day < login_day) return;

	int login_day_index = login_day - 1;
	if (login_day_index < 0 || login_day_index >= 32)return;
	if (m_param.is_reward_flag.IsBitSet(login_day_index)) return;

	const RALoginSystemRewardCfg * cfg = LOGIC_CONFIG->GetRandActivityConfigManager()->GetLoginSystemRewardBySeq(login_day, m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (cfg == NULL) return;

	if (m_role_module_mgr->GetKnapsack()->CheckForPutList2(cfg->reward_list))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(cfg->reward_list, PUT_REASON_RA_LOGIN_SYSTEM_REWARD);

		std::string gamelog_str;
		for (int j = 0; j < (int)cfg->reward_list.size(); ++j)
		{
			gamelog_str = STRING_SPRINTF("{item[%d, %d]} ", cfg->reward_list[j].item_id, cfg->reward_list[j].num);
		}
		gamelog::g_log_activity.printf(LL_INFO, "LoginSystem::OnGetReward role[%s,%d] items[%s]",
			m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetUID(), gamelog_str.c_str());

		m_param.is_reward_flag.SetBit(login_day_index);
	}

	this->SendLoginSystemInfo();
}

bool LoginSystem::IsOpenActivity()
{
	return true;
}

void LoginSystem::ResetDataByActivityOpen()
{
	m_param.Reset();
}

void LoginSystem::SendLoginSystemInfo()
{
	if (!this->IsOpenActivity())return;
	static Protocol::SCLoginSystemInfo oea;

	oea.login_day = m_param.login_day;
	oea.is_reward_flag = m_param.is_reward_flag;

	m_role_module_mgr->NetSend((const char *)&oea, sizeof(oea));
}

void LoginSystem::OnRefresh()
{
	for (int i = 0; i < m_param.login_day; ++i)
	{
		if (!m_param.unlock_reward_flag.IsBitSet(i))
		{
			m_param.unlock_reward_flag.SetBit(i);
		}
	}

	this->SendLoginSystemInfo();
}
