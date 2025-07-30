#ifndef __ACTIVITY_GUILD_ANSWER_CONFIG_HPP__
#define __ACTIVITY_GUILD_ANSWER_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/activitydef.hpp"
#include "servercommon/commondata.hpp"

#include <set>
#include <map>
#include <vector>

static const int GUILD_ANSWER_REWARD_ITEM_MAX = 5;

struct GuildAnswerOthers
{
	GuildAnswerOthers() : ready_time(0),question_time(0), question_numb(0), scene_id(0), min_level(0) {}

	int ready_time;			//开始答题前准备时间
	int question_time;
	int question_numb;
	int scene_id;
	int min_level;
};

struct GuildAnswerReward
{
	GuildAnswerReward() : exp(0), coin(0), contribution(0), family_coin(0), list_count(0)
	{
		memset(reward_group_list, 0, sizeof(reward_group_list));
	}

	int exp;
	int coin;
	int contribution;
	int family_coin;

	int list_count;
	int reward_group_list[GUILD_ANSWER_REWARD_ITEM_MAX];
};

struct GuildAnswerItemRand
{
	GuildAnswerItemRand() : prob(0) {}

	int prob;
	ItemConfigData reward_item;
};

struct GuildAnswerFindCfg
{
	GuildAnswerFindCfg() :min_level(0), max_level(0), exp(0), coin(0), family_gongxian(0), group_id(0)
	{}

	int min_level;
	int max_level;
	int exp;
	int coin;
	int family_gongxian;
	int group_id;
};

struct GuildAnswerFindGroupCfg
{
	GuildAnswerFindGroupCfg()
	{}

	std::vector<GuildAnswerFindCfg> node_list;
};

struct GuildAnswerFindRewardGroupCfg
{
	GuildAnswerFindRewardGroupCfg():rate(0)
	{}

	int rate;
	ItemConfigData reward;
};

class ActivityGuildAnswerConfig : public ILogicConfig
{
public:
	ActivityGuildAnswerConfig();
	virtual ~ActivityGuildAnswerConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const GuildAnswerOthers & GetGuildAnswerOtherConfig() { return m_guild_answer_other_cfg; }
	const GuildAnswerReward * GetAnswerReward(int level, bool is_correct);
	const ItemConfigData * RandItemAward(int group_id);

	int GetRightAnswer(int question_id);
	bool RandQuestionList(std::vector<int> & out_list);

	const GuildAnswerFindCfg * GetFindCfg(int find_type, int role_level)const;
	const GuildAnswerFindRewardGroupCfg * GetFindRewardGroupCfg(int group_id)const;

private:
	int InitOtherInformaCfg(TiXmlElement * RootElement);
	int InitQuestionListCfg(TiXmlElement * RootElement);
	int InitRandAwardGroupList(TiXmlElement * RootElement);
	int InitRightAnswerAward(TiXmlElement * RootElement);
	int InitWrongAnswerAward(TiXmlElement * RootElement);
	int InitGuildAnswerFindRewardGroupCfg(TiXmlElement * RootElement);
	int InitGuildAnswerFindCfg(TiXmlElement * RootElement);

	GuildAnswerOthers m_guild_answer_other_cfg;
	std::map<int, int> m_question_answer_map;					// 题目 <-> 答案 映射表
	std::map<int, GuildAnswerReward> m_right_reward_map;		// 玩家等级 <-> 答题奖励 映射表 - 答对的奖励
	std::map<int, GuildAnswerReward> m_wrong_reward_map;		// 玩家等级 <-> 答题奖励 映射表 - 答错的奖励
	std::map<int, std::vector<GuildAnswerItemRand> > m_item_rand_group;		// 道具随机表：键 - 道具组ID

	std::map<int, std::vector<GuildAnswerFindRewardGroupCfg> > m_find_reward_group_map;			// 找回道具组:key - group_id
	GuildAnswerFindGroupCfg m_find_cfg[RB_TYPE_MAX];			// 奖励找回
};

#endif