#ifndef __FAST_FORWARD_BATTLE_CONFIG_HPP__
#define __FAST_FORWARD_BATTLE_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include <map>

struct FastForwardBattleCfg
{
	FastForwardBattleCfg() : level_limit(0), forward_round_num(0) {}

	int level_limit;
	int forward_round_num;
};

class FastForwardBattleConfig : public ILogicConfig
{
public:
	FastForwardBattleConfig();
	virtual ~FastForwardBattleConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	bool GetFastBattleRound(int level, int battle_mode, int* fast_forward_rounds);
	int GetFastBattleDefaultRounds() const { return m_fast_battle_default_rounds; }
	int GetChallengeTaskLevelLimit() const { return m_challenge_task_level_limit; }

private:
	int InitFastBattleModeRoundsHashCfg(TiXmlElement * RootElement);
	int InitOtherCfg(TiXmlElement* RootElement);

	std::map<int, FastForwardBattleCfg> m_fast_battle_rounds_map;	// ÀŸ’Ω
	int m_fast_battle_default_rounds;
	int m_challenge_task_level_limit;
};

#endif