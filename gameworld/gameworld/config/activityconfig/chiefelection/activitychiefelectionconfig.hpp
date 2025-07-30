#ifndef __ACTIVITY_CHIEF_ELECTION_CONFIG__
#define __ACTIVITY_CHIEF_ELECTION_CONFIG__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/activitydef.hpp"

#include <set>
#include <map>
#include <vector>

struct ChiefElectionReadySceneCfg
{
	ChiefElectionReadySceneCfg()
	{
		scene_id = 0;
	}

	int scene_id;
};

struct ChiefElectionRewardCfg
{
	ChiefElectionRewardCfg()
	{
	}

	int title_id;
	std::vector<ItemConfigData> rewards;
};

struct ChiefElectionOtherCfg
{
	ChiefElectionOtherCfg()
	{
		max_round = 0;
		first_ready_time = 60;
		ready_time = 30;

		sea_election_ready_time = 0;
		sea_election_fight_time = 0;
		memset(sea_election_scene_id, 0, sizeof(sea_election_scene_id));
		win_score = 0;
		lose_score = 0;
		draw_score = 0;
		lose_times_kickout = 0;
		field_buff_id = 0;
	};

	int GetSeaElectionSceneid(int which_prof) const
	{
		int index = which_prof / PROFESSION_BASE;
		if (index < 0 || index >= ARRAY_LENGTH(sea_election_scene_id))
		{
			return 0;
		}

		return sea_election_scene_id[index];
	}

	bool IsSeaElectionSceneid(int scene_id) const
	{
		for (int i = 0; i < ARRAY_LENGTH(sea_election_scene_id); ++i)
		{
			if (sea_election_scene_id[i] == scene_id)
			{
				return true;
			}			
		}

		return false;
	}

	int max_round;
	int first_ready_time;
	int ready_time;

	//--------------混战海选字段-------------
	int sea_election_ready_time;
	int sea_election_fight_time;
	int sea_election_scene_id[PROFESSION_TYPE_NUM];
	int win_score;
	int lose_score;
	int draw_score;
	int lose_times_kickout;

	int field_buff_id;
};

struct ActivityChiefElectionBattleRewardCfg  //每场战斗奖励配置
{
	ActivityChiefElectionBattleRewardCfg()
	{
		min_role_level = 0;
		max_role_level = 0;

		winner_exp = 0;
		winner_coin = 0;
		loser_exp = 0;
		loser_coin = 0;
	}

	int min_role_level;
	int max_role_level;

	int winner_exp;
	int winner_coin;
	int loser_exp;
	int loser_coin;
	std::vector<ItemConfigData> loser_rewards;
};

struct ActivityChiefElectionSeaRewardCfg  //每场战斗奖励配置
{
	ActivityChiefElectionSeaRewardCfg()
	{
		min_role_level = 0;
		max_role_level = 0;


	}

	int min_role_level;
	int max_role_level;

	int exp;
	int coin;
	std::vector<ItemConfigData> rewards;
};

class ActivityChiefElectionConfig : public ILogicConfig
{
public:
	ActivityChiefElectionConfig();
	virtual ~ActivityChiefElectionConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	int GetTitleId(int prof_base);
	const ChiefElectionRewardCfg * GetRewardCfg(int prof_base);
	int GetReadySceneId(int round);
	const ChiefElectionOtherCfg & GetOhterCfg();
	const ActivityChiefElectionBattleRewardCfg * GetPerBattleRewardCfg(int role_level,int match_round);
	const ActivityChiefElectionSeaRewardCfg * GetSeaRewardCfg(int role_level);
	bool IsElectionChiefScene(int scene_id);
	Posi GetSeaElectionPos();
private:
	int InitSceneCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitBattleRewardCfg(TiXmlElement * RootElement);
	int InitSeaElectionRewardCfg(TiXmlElement * RootElement);
	int InitSeaElectionPosiCfg(TiXmlElement * RootElement);

	std::vector<ChiefElectionReadySceneCfg> m_ready_scene_cfg;
	std::map<int, ChiefElectionRewardCfg> m_reward_cfg;
	ChiefElectionOtherCfg m_other_cfg;
	std::map<int,std::vector<ActivityChiefElectionBattleRewardCfg> > m_battle_reward_cfg; //每场战斗奖励配置
	std::vector<ActivityChiefElectionSeaRewardCfg> m_sea_election_reward_cfg; //海选参与奖励
	
	size_t m_birth_pos_index;
	std::vector<Posi > m_birth_pos;		//海选出生地点
};

#endif // !__ACTIVITY_MAZE_RACER_CONFIG__