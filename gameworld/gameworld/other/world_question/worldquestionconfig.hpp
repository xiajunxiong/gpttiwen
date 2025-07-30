#ifndef __WORLD_QUESTION_CONFIG_HPP__
#define __WORLD_QUESTION_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "other/attributelist.hpp"
#include <map>
#include <vector>
#include "servercommon/struct/itemlistparam.h"

struct WorldQuestionBank
{
	WorldQuestionBank(): question_groups(0),question_id(0),question(0),answer(0){}
	int question_groups;
	int question_id;
	int question;
	int answer;
};

struct WorldQuestionReward
{
	WorldQuestionReward() : role_level(0), exp(0), coin(0),world_question_score(0){}
	int role_level;
	int exp;
	int coin;
	int world_question_score;
	std::vector<int> item_list;
};

struct WorldQuestionRewardGroup
{
	WorldQuestionRewardGroup() : group(0),rate(0){}
	int group;
	int rate;
	ItemConfigData reward_item;
};

struct WorldQuestionOther
{
	WorldQuestionOther() : start_time(0), end_time(0), refresh_time(0), question_time(0), answer_time(0) {}
	int start_time;
	int end_time;
	int refresh_time;
	int question_time;
	int answer_time;
};

struct WorldAnswerGroup
{
	const static int MAX_ANSWER_ID_NUM = 4;
	WorldAnswerGroup() :answer_true(0) {}
	int answer_true;
	int answer_id[MAX_ANSWER_ID_NUM];
};

class WorldQuestionConfig : public ILogicConfig
{
public:
	WorldQuestionConfig();
	virtual ~WorldQuestionConfig();
	virtual bool Init(const std::string &configname, std::string *err);

	bool RandQuestionCdg(int group_id, WorldQuestionBank *now_question);
	const WorldQuestionBank * GetQuestion(int question_id);
	const WorldQuestionReward * GetReward(int level);
	const WorldQuestionRewardGroup * RandRewardGroup(int group_id);
	const WorldQuestionOther * GetOhterCfg() { return &m_other_cfg; }
	const WorldAnswerGroup * GetAnswerGroupCfg(int group_id);

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitQuestionBankCfg(TiXmlElement *RootElement);
	int InitQuestionRewardCfg(TiXmlElement *RootElement);
	int InitQuestionRewardGroupCfg(TiXmlElement *RootElement);
	int InitAnswerGroupCfg(TiXmlElement *RootElement);

	WorldQuestionOther m_other_cfg;
	std::map<int, std::vector<WorldQuestionBank> > m_question_bank_cfg;
	std::map<int, WorldQuestionReward> m_reward_cfg;

	std::map<int, int> m_reward_group_weight;
	std::map<int, std::vector<WorldQuestionRewardGroup> > m_question_reward_group_cfg;

	std::map<int, WorldAnswerGroup> m_answer_group_map;
};

#endif // !__WST_QUESTION_CONFIG_HPP__
