#include "fastforwardbattleconfig.hpp"
#include "servercommon/servercommon.h"
#include "servercommon/struct/battle/battle_def.hpp"

FastForwardBattleConfig::FastForwardBattleConfig() : 
	m_fast_battle_default_rounds(0),
	m_challenge_task_level_limit(0)
{

}

FastForwardBattleConfig::~FastForwardBattleConfig()
{

}

bool FastForwardBattleConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;
	LOAD_CONFIG("quick_battle", InitFastBattleModeRoundsHashCfg);
	LOAD_CONFIG("other", InitOtherCfg);
	return true;
}

bool FastForwardBattleConfig::GetFastBattleRound(int level, int battle_mode, int* fast_forward_rounds)
{
	std::map<int, FastForwardBattleCfg>::iterator it = m_fast_battle_rounds_map.find(battle_mode);
	if (it != m_fast_battle_rounds_map.end() 
		&& level >= it->second.level_limit)
	{
		if (NULL != fast_forward_rounds)
		{
			*fast_forward_rounds = it->second.forward_round_num;
		}
		return true;
	}

	return false;
}

int FastForwardBattleConfig::InitFastBattleModeRoundsHashCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (NULL != dataElement)
	{
		int skip_rounds = 0;
		if (!GetSubNodeValue(dataElement, "skip", skip_rounds) || skip_rounds < 0)
		{
			return -1;
		}

		int battle_mode = 0;
		if (!GetSubNodeValue(dataElement, "type", battle_mode) || battle_mode < 0 || battle_mode >= BATTLE_MODE_MAX)
		{
			return -2;
		}

		int level_limit = 0;
		if (!GetSubNodeValue(dataElement, "open_level", level_limit) || level_limit < 0 || level_limit > MAX_ROLE_LEVEL)
		{
			return -3;
		}

		FastForwardBattleCfg cfg;
		cfg.forward_round_num = skip_rounds;
		cfg.level_limit = level_limit;

		m_fast_battle_rounds_map[battle_mode] = cfg;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int FastForwardBattleConfig::InitOtherCfg(TiXmlElement* RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	if (!GetSubNodeValue(dataElement, "default_round", m_fast_battle_default_rounds))
	{
		return -1;
	}

	if (!GetSubNodeValue(dataElement, "challenge_task_lv", m_challenge_task_level_limit))
	{
		return -2;
	}

	return 0;
}

