#ifndef __MINIGAME_NIULANG_CONFIG_HPP__
#define __MINIGAME_NIULANG_CONFIG_HPP__

#include "servercommon/configcommon.h"

struct MiniGameNiuLangLevelCfg
{
	MiniGameNiuLangLevelCfg() : seq(0), monster_group_id(0), npc_demo_seq(0) {}

	int seq;
	int monster_group_id;
	int npc_demo_seq;
};

class MiniGameNiuLangConfig : public ILogicConfig
{
public:
	static const int MAX_LEVEL_CFG_NUM = 50;

public:
	MiniGameNiuLangConfig();
	virtual ~MiniGameNiuLangConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	
	int GetNpcSeq(int seq);
	int GetMaxStage() const;
	int GetStageNum() const { return m_level_num; }
private:
	int InitLevelCfg(TiXmlElement* root_element, std::string& err);

	int m_level_num;
	MiniGameNiuLangLevelCfg m_level_cfg[MAX_LEVEL_CFG_NUM];
};



#endif