#ifndef __ANIMATION_SPEED_CONFIG_HPP__
#define __ANIMATION_SPEED_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include <set>

class AnimationSpeedConfig : public ILogicConfig
{
public:
	AnimationSpeedConfig();
	virtual ~AnimationSpeedConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	bool CanTeamOpenDoubleSpeed(int battle_mode) const;

private:
	int InitDoubleSpeedCfg(TiXmlElement * RootElement);

	std::set<int> m_double_speed_mode_set;
};

#endif