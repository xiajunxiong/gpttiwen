#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "item/knapsack.h"
#include "engineadapter.h"
#include "item/knapsack.h"
#include "global/team/team.hpp"

#include "protocol/msgmonsterwave.h"
#include "other/monster_wave/monsterwaveconfig.h"
#include "monsterwave.h"

MonsterWave::MonsterWave() : m_role_module_mgr(NULL)
{
}

MonsterWave::~MonsterWave()
{
}

void MonsterWave::Init(RoleModuleManager * mgr, const MonsterWaveParam & param)
{
	m_role_module_mgr = mgr;
	m_param = param;
}

void MonsterWave::GetInitParam(MonsterWaveParam * param)
{
	*param = m_param;
}

void MonsterWave::OnResetData(unsigned int old_dayid, unsigned int now_dayid)
{
	
}

void MonsterWave::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	m_param.play_times = 0;
	m_param.today_pass_wave = 0;
	this->SendInfo();
}

bool MonsterWave::IsPlay()
{
	if (m_param.play_times != 0)
	{
		return true;
	}

	return false;
}

void MonsterWave::AddPlayTimes()
{
	m_param.play_times += 1;
}

//当队员等级与队长等级段不符时 该队员在打完怪以后不发奖励【该设定已取消】
void MonsterWave::SendPassReward(int pass_wave)
{
	this->AddPlayTimes();
	/*int leader_role_level = 0;
	Team * team = m_role_module_mgr->GetRole()->GetMyTeam();
	if (team != NULL)
	{
		Role * leader = team->GetLeaderRole();
		if (leader != NULL)
		{
			leader_role_level = leader->GetLevel();
		}
	}*/
	int role_level = m_role_module_mgr->GetRole()->GetLevel();
	const MonsterWaveLevelCfg * cfg = LOGIC_CONFIG->GetMonsterWaveConfig()->GetLevelCfg(role_level);
	//const MonsterWaveLevelCfg * cfg2 = LOGIC_CONFIG->GetMonsterWaveConfig()->GetLevelCfg(leader_role_level);
	if (cfg == NULL /*|| (leader_role_level != 0 && cfg != cfg2)*/)
	{
		return;
	}
	if(m_param.today_pass_wave >= pass_wave) return;

	std::vector<ItemConfigData> rewards;
	std::map<int, std::vector<MonsterWaveLevelCfg::ItemCfg> >::const_iterator find_it = cfg->all_wave_reward.find(pass_wave);
	if (find_it == cfg->all_wave_reward.end()) return;

	find_it++;
	std::map<int, std::vector<MonsterWaveLevelCfg::ItemCfg> >::const_iterator it = cfg->all_wave_reward.begin();
	for (;it != find_it; it++)
	{
		if(it->first <= m_param.today_pass_wave) continue;
		
		for (int i = 0; i < (int)it->second.size(); i++)
		{
			if (role_level >= it->second[i].min_level && role_level <= it->second[i].max_Level)
			{
				for (int k = 0; k < (int)it->second[i].wave_reward.size(); ++k)
				{
					LOGIC_CONFIG->GetMonsterWaveConfig()->GetRandReward(it->second[i].wave_reward[k], rewards);
				}
			}
		}

	}

	if (rewards.size() > 0)
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)rewards.size(), &rewards[0], PUT_REASON_SYSTEM_MONSTER_WAVE);
		m_role_module_mgr->GetKnapsack()->NotifyGetItemList((short)rewards.size(), &rewards[0], GET_REASON_MONSTER_WAVE);
	}

	if (pass_wave > m_param.today_pass_wave)
	{
		m_param.today_pass_wave = pass_wave;
	}

	this->SendInfo();
	this->SendFinishInfo(cfg->need_role_level,pass_wave);
}

void MonsterWave::SendFinishInfo(int role_level, int pass_wave)
{
	Protocol::SCMonsterWaveFinishInfo protocol;

	protocol.role_level = role_level;
	protocol.pass_wave = pass_wave;

	EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), &protocol, sizeof(protocol));
}

void MonsterWave::SendInfo()
{
	Protocol::SCMonsterWaveInfo protocol;

	protocol.play_times = m_param.play_times;
	protocol.today_pass_wave = m_param.today_pass_wave;

	EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), &protocol, sizeof(protocol));
}
