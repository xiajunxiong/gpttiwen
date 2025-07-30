#ifndef __SCHOOL_EXERCISE_CONFIG_HPP__
#define __SCHOOL_EXERCISE_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/schoolexercisedef.hpp"



struct SchoolExerciseChallengeCfg
{
	SchoolExerciseChallengeCfg() : seq(0), unlock_level(0), monster_group_id(0), group(0), layer(0), pre_seq(-1),
		npc_demo_seq(0), coin_reward(0), item_reward_num(0){}
	
	int seq;
	int unlock_level;
	int monster_group_id;
	int group;
	int layer;
	int pre_seq;			// «∞÷√seq
	int npc_demo_seq;
	int coin_reward;
	int item_reward_num;
	ItemConfigData item_reward_list[5];
};

class SchoolExerciseConfig : public ILogicConfig
{
public:
	SchoolExerciseConfig();
	virtual ~SchoolExerciseConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const SchoolExerciseChallengeCfg* GetChallengeCfg(int seq);

private:
	int InitChallengeCfg(TiXmlElement * RootElement);

	int m_challenge_cfg_num;
	SchoolExerciseChallengeCfg m_challenge_cfg_list[MAX_SCHOOL_EXERCISE_NUM];
};

#endif