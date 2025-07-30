#ifndef __LILIAN_CONFIG_HPP__
#define __LILIAN_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include <map>

class LilianConfig : public ILogicConfig
{
public:
	LilianConfig();
	virtual ~LilianConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	bool GetFastBattleRound(int battle_mode, int* fast_forward_rounds);
	int GetFastBattleDefaultRounds() const { return m_fast_battle_default_rounds; }
private:
	int InitFastBattleModeRoundsHashCfg(TiXmlElement * RootElement);

	std::map<int, int> m_fast_battle_rounds_map;	// ÀŸ’Ω
	int m_fast_battle_default_rounds;
};

#endif