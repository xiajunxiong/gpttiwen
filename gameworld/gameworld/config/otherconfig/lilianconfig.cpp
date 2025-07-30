#include "lilianconfig.hpp"
#include "servercommon/servercommon.h"
#include "servercommon/struct/battle/battle_def.hpp"

LilianConfig::LilianConfig() : m_fast_battle_default_rounds(0)
{

}

LilianConfig::~LilianConfig()
{

}

bool LilianConfig::Init(const std::string &configname, std::string *err)
{
	//PRE_LOAD_CONFIG;
	//LOAD_CONFIG("experience_view", InitFastBattleModeRoundsHashCfg);

	// 已转用FastForwardBattleConfig，
	// 如果历练的配置需要服务端读，去掉assert即可
	assert(0);
	return false;
}

bool LilianConfig::GetFastBattleRound(int battle_mode, int* fast_forward_rounds)
{
	std::map<int, int>::iterator it = m_fast_battle_rounds_map.find(battle_mode);
	if (it != m_fast_battle_rounds_map.end())
	{
		if (NULL != fast_forward_rounds) *fast_forward_rounds = it->second;
		return true;
	}

	return false;
}

int LilianConfig::InitFastBattleModeRoundsHashCfg(TiXmlElement * RootElement)
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

		m_fast_battle_rounds_map[battle_mode] = skip_rounds;
		if (0 == battle_mode)
		{
			m_fast_battle_default_rounds = skip_rounds;
		}
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

