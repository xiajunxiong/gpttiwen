#ifndef __INITIAL_BATTLE_CONFIG_HPP__
#define __INITIAL_BATTLE_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/servercommon.h"

class InitialBattleConfig : public ILogicConfig
{
public:
	InitialBattleConfig();
	virtual ~InitialBattleConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	int GetInitialBattleSeq(int profession);
private:
	int InitJobSeqCfg(TiXmlElement *RootElement);

	int m_job_seq_hash[PROFESSION_TYPE_NUM];
};

#endif