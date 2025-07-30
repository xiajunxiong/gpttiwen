#include "animationspeedconfig.hpp"
#include "servercommon/servercommon.h"
#include "servercommon/struct/battle/battle_def.hpp"

AnimationSpeedConfig::AnimationSpeedConfig() 
{

}

AnimationSpeedConfig::~AnimationSpeedConfig()
{

}

bool AnimationSpeedConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;
	LOAD_CONFIG("play_battle", InitDoubleSpeedCfg);
	return true;
}

bool AnimationSpeedConfig::CanTeamOpenDoubleSpeed(int battle_mode) const
{
	return m_double_speed_mode_set.find(battle_mode) 
		!= m_double_speed_mode_set.end();
}

int AnimationSpeedConfig::InitDoubleSpeedCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (NULL != dataElement)
	{
		int battle_mode = 0;
		if (!GetSubNodeValue(dataElement, "type", battle_mode)
			|| battle_mode <= BATTLE_MODE_INVALID
			|| battle_mode >= BATTLE_MODE_MAX)
		{
			return -1;
		}

		m_double_speed_mode_set.insert(battle_mode);

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}
