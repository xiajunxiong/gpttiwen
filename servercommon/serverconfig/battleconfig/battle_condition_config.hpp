#ifndef __BATTLE_CONDITION_CONFIG_HPP__
#define __BATTLE_CONDITION_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/battle/battle_condition_def.hpp"

struct BattleCondCfg
{
	int condition_id;
	BattleCond cond;
};

class BattleConditionConfig
{
public:
	static const int CONDITION_CFG_NUM = 200;//…œœﬁ255£¨uchar£¨

public:
	static BattleConditionConfig* Instance();

	bool Init(const std::string &configname, std::string *err);
	static bool Reload(const std::string &configname, std::string *err);
	const BattleCondCfg* GetCondCfg(int condition_id);
private:
	BattleConditionConfig();
	~BattleConditionConfig();
	BattleConditionConfig(const BattleConditionConfig&);
	BattleConditionConfig& operator=(const BattleConditionConfig &);

	int InitConditionCfg(TiXmlElement *RootElement);
	bool CheckCondCfg(BattleCondCfg& cfg);

	int m_cond_cfg_num;
	BattleCondCfg m_cond_cfg_list[CONDITION_CFG_NUM];
};

#endif