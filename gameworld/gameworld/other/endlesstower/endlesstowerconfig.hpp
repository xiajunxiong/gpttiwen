#ifndef _ENDLESS_TOWER_CONFIG_HPP_
#define _ENDLESS_TOWER_CONFIG_HPP_

#include "servercommon/servercommon.h"
#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"

struct EndlessTowerChallengeCfg
{
	EndlessTowerChallengeCfg() : seq(0), num(0), reward_id(0), monster_group(0) {}
	int seq;
	int num;
	int reward_id;
	int monster_group;
};

struct EndlessTowerRewardCfg
{
	EndlessTowerRewardCfg() : god_reward_id(0) {}
	
	int god_reward_id;
	std::vector<ItemConfigData> reward_vec;
};

struct EndlessTowerRewardGroupCfg
{
	EndlessTowerRewardGroupCfg() {}
	int seq;
	std::vector<EndlessTowerRewardCfg> reward_group;
};

struct OtherCfg
{
	OtherCfg() : open_level(0), scene_id(0) {}
	int open_level;
	int scene_id;
};

struct GodRewardCfg
{
	GodRewardCfg() : god_reward_id(0), rate(0) {}
	int god_reward_id;
	int rate;
	std::vector<ItemConfigData> god_reward;
};

struct GodRewardListCfg
{
	GodRewardListCfg() : weight(0) {}

	int weight;
	std::vector<GodRewardCfg> reward_cfg;
};

struct EndlessTowerRankRewardCfg
{
	EndlessTowerRankRewardCfg() : rank_reward_id(0), min_rank(0), max_rank(0) {}

	int rank_reward_id;
	int	min_rank;
	int	max_rank;
	std::vector<ItemConfigData> reward_vec;
};

const static int ENDLESS_TOWER_MAX_SEQ = 1000;

class RoleModuleManager;
class EndlessTowerConfig : public ILogicConfig
{
public:
	EndlessTowerConfig();
	virtual ~EndlessTowerConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	const OtherCfg & GetOtherCfg() { return m_other_cfg; }
	const EndlessTowerChallengeCfg * GetEndlessTowerChallengeCfg(int seq);//seqÐèÒª - 1
	const EndlessTowerRewardCfg * GetEndLessTowerRewardCfg(int reward_seq);
	const GodRewardCfg * GetGodRewardCfg(int god_seq);
	const EndlessTowerRankRewardCfg * GetRankRewardCfg(int rank_id);
private:
	int InitChallengeCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitOtherCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitRewardCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitGodRewardCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitRankRewardCfg(TiXmlElement * RootElement, std::string& errormsg);

	OtherCfg m_other_cfg;
	EndlessTowerChallengeCfg* m_challeng_list[ENDLESS_TOWER_MAX_SEQ];
	std::vector<EndlessTowerChallengeCfg> m_challenge_cfg;
	std::map<int, EndlessTowerRewardGroupCfg> m_reward_cfg;
	std::map<int, GodRewardListCfg> m_god_reward_cfg;
	
	std::vector<EndlessTowerRankRewardCfg> m_rank_reward_vec;
};
#endif
