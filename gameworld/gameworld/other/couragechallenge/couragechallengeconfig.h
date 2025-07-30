#ifndef _COURAGE_CHALLENGE_CONFIG_HPP_
#define _COURAGE_CHALLENGE_CONFIG_HPP_

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "servercommon/couragechallengedef.hpp"
#include "servercommon/newcouragechallengedef.hpp"

#include<vector>

struct CourageChallengeLevelCfg
{
	CourageChallengeLevelCfg()
	{
		seq = 0;
		layer_level = 0;
		level_seq = 0;
		level = 0;
		memset(condition, 0 ,sizeof(condition));
		monster_group_id = 0;
		coin = 0;

		attacker_buff_id = 0;
		attacker_buff_level = 0;

		guanqia_num = 0;
	}

	int seq;
	int layer_level;
	int level_seq;
	int level;
	int condition[COURAGE_CHALLENGE_CONDITION_MAX_NUM];
	int monster_group_id;

	int coin;
	std::vector<ItemConfigData> pass_reward;
	std::vector<ItemConfigData> extra_reward;
	
	int attacker_buff_id;
	int attacker_buff_level;

	int guanqia_num; // 关卡数
};

struct CourageChallengeScoreRewardCfg
{
	CourageChallengeScoreRewardCfg()
	{}

	int star;
	std::vector<ItemConfigData> score_reward;
};

struct CourageChallengeOtherCfg
{
	CourageChallengeOtherCfg() : open_level(0), refresh_time(0), scene_id(0), extra_reward_level(0), new_scene_id(0), new_token_reward_prize(0) {};

	int open_level;
	int refresh_time;
	int scene_id;
	int extra_reward_level;
	int new_scene_id;
	int new_token_reward_prize;
};

struct CourageChallengeMaxScoreRewardCfg
{
	CourageChallengeMaxScoreRewardCfg():max_star(0), version(0)
	{}

	int max_star;				//满星星数
	int version;
	std::vector<ItemConfigData> max_score_reward;
};

struct CourageScoreKey    
{
	CourageScoreKey (): layer_level(0), seq(0) {}
	CourageScoreKey (int _layer_level, int _seq) : layer_level(_layer_level), seq(_seq) {}

	bool operator< (const CourageScoreKey& index) const
	{
		if (layer_level != index.layer_level)
		{
			return layer_level < index.layer_level;
		}
		else
		{
			return seq < index.seq;
		}
	}


	int layer_level;
	int seq;
};

struct NewCourageChallengeLevelCfg//弑神之塔
{
	NewCourageChallengeLevelCfg()
	{
		seq = 0;
		layer_level = 0;
		level_seq = 0;
		level = 0;
		memset(condition, 0, sizeof(condition));
		monster_group_id = 0;
	}

	int seq;
	int layer_level;
	int level_seq;
	int level;
	int condition[COURAGE_CHALLENGE_CONDITION_MAX_NUM];
	int monster_group_id;

	std::vector<ItemConfigData> pass_reward;
	std::vector<ItemConfigData> extra_reward;
};

struct NewCourageChallengeRewardCfg//弑神之塔成就奖励
{
	NewCourageChallengeRewardCfg() : seq(-1), star_num(0) {}

	int seq;
	int star_num;
	std::vector<ItemConfigData> reward;
	std::vector<ItemConfigData> token_reward;
};

class CourageChallengeConfig : public ILogicConfig
{
public:
	CourageChallengeConfig( );
	virtual ~CourageChallengeConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const CourageChallengeOtherCfg& GetOtherCfg();
	const CourageChallengeLevelCfg * GetLevelCfg(int layer_level, int level_seq);
	const CourageChallengeLevelCfg * GetLevelCfgByMonsterGroupId(int monster_group_id);
	const std::vector<CourageChallengeLevelCfg> * GetLevelCfgByLayerLevel(int layer_level);
	const CourageChallengeScoreRewardCfg * GetScoreRewardCfg(int layer_level, int level_seq);
	const CourageChallengeMaxScoreRewardCfg * GetMaxScoreRewardCfg(int layer_level);
	int GetMinLayerLevel() { return min_layer_level; }
	int GetPetHelperNum(int clear_level_num);

	const NewCourageChallengeLevelCfg* GetNewLevelCfg(int layer_level, int level_seq)const;
	const NewCourageChallengeLevelCfg* GetNewLevelCfgByMonster(int monster_group_id)const;
	const NewCourageChallengeRewardCfg* GetNewRewardCfg(int reward_seq)const;
	int GetNewRewardCfgSize() { return (int)m_new_reward_cfg.size(); }

private:
	int InitLayerLevelCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitScoreRewardCfg(TiXmlElement *RootElement);
	int InitMaxScoreRewardCfg(TiXmlElement *RootElement);
	int InitPetHelperCfg(TiXmlElement *RootElement);

	int InitNewLevelCfg(TiXmlElement *RootElement);
	int InitNewRewardCfg(TiXmlElement *RootElement);

	CourageChallengeOtherCfg m_other_cfg;
	std::map<CourageScoreKey, CourageChallengeLevelCfg> m_level_cfg;						// 挑战内容
	std::map<int, std::vector<CourageChallengeLevelCfg> > m_level_cfg_by_layer_level;		// 挑战内容 key为layer_leve
	std::map<int, CourageChallengeLevelCfg>				m_level_cfg_by_monster_group_id;	// 挑战内容 key为怪物组ID
	std::map<CourageScoreKey, CourageChallengeScoreRewardCfg> m_score_reward_cfg;			// 评分奖励
	std::map<int, CourageChallengeMaxScoreRewardCfg> m_max_score_reward_cfg;				// 满星奖励 key为layer_level
	int min_layer_level;		//最低分层

	int m_pet_helper_num_clear_level_hash[5];

	std::map<CourageScoreKey, NewCourageChallengeLevelCfg> m_new_level_cfg;					// 弑神之塔挑战内容
	std::map<int, NewCourageChallengeLevelCfg> m_new_level_cfg_by_monster;					// 弑神之塔挑战内容 key为怪物组ID
	std::vector<NewCourageChallengeRewardCfg> m_new_reward_cfg;								// 弑神之塔成就奖励
};



#endif // _COURAGE_CHALLENGE_CONFIG_HPP_