#ifndef __WHERE_IS_THE_FAIRY_CONFIG_HPP__
#define __WHERE_IS_THE_FAIRY_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "gamedef.h"

struct WhereIsTheFairyBaseCfg
{
	WhereIsTheFairyBaseCfg() : npc_id(0), live_time(0)
	{}

	int npc_id;
	int live_time;
};

struct WhereIsTheFairySceneCfg
{
	WhereIsTheFairySceneCfg() : npc_pos(0, 0), npc_dir(0), scene_id(0)
	{}

	Posi npc_pos;
	Dir npc_dir;
	int scene_id;
};

struct WhereIsTheFairyRewardCfg
{
	WhereIsTheFairyRewardCfg()
	{}

	std::vector<ItemConfigData> reward_list;
};

class WhereIsTheFairyConfig : public ILogicConfig
{
public:
	WhereIsTheFairyConfig();
	virtual ~WhereIsTheFairyConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const WhereIsTheFairyBaseCfg & GetBaseCfg() { return m_base_cfg; }
	const WhereIsTheFairyRewardCfg & GetRewardCfg() { return m_reward_cfg; }
	bool GetRandSceneCfg(unsigned int refresh_time, WhereIsTheFairySceneCfg & out_cfg) const ;


private:
	int InitBaseCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitSceneCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitRewardCfg(TiXmlElement * RootElement, std::string& errormsg);

	WhereIsTheFairyBaseCfg m_base_cfg;
	std::vector<WhereIsTheFairySceneCfg> m_scene_cfg;		//≥°æ∞◊¯±Í≈‰÷√
	WhereIsTheFairyRewardCfg m_reward_cfg;
};


#endif