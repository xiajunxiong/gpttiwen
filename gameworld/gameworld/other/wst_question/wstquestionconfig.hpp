#ifndef __WST_QUESTION_CONFIG_HPP__
#define __WST_QUESTION_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "other/attributelist.hpp"
#include <map>
#include <vector>

struct WSTQuestionOtherCfg
{
	WSTQuestionOtherCfg()
	{
		exp_reduction_ratio = 0;
		coin_reduction_ratio = 0;
		question_num = 0;
	}

	int exp_reduction_ratio;
	int coin_reduction_ratio;
	int question_num;

};

struct WSTQuestionRewardCfg
{
	WSTQuestionRewardCfg()
	{
		level = 0;
		exp = 0;
		coin = 0;
		exp_buy = 0;
		gold = 0;
		num = 0;
	}

	int level;
	int exp;
	int coin;
	int exp_buy;
	int gold;
	int num;
};

struct WSTQuestionBankCfg
{
	WSTQuestionBankCfg()
	{
		group_id = 0;
		question_id = 0;
		answer_id = 0;
	}
	int group_id;
	int question_id;
	int answer_id;
};

struct WstQuestionFindCfg
{
	WstQuestionFindCfg()
	{
		min_level = 0;
		max_level = 0;
		exp = 0;
		coin = 0;
		find_type = 0;
		reward_group = 0;
	}
	int min_level;
	int max_level;
	int exp;
	int coin;
	int find_type;
	int reward_group;
};

class WstQuestionConfig : public ILogicConfig
{
public:
	WstQuestionConfig();
	virtual ~WstQuestionConfig();
	virtual bool Init(const std::string &configname, std::string *err);

	const WSTQuestionOtherCfg* GetOtherCfg() { return &m_other_cfg; }
	const WSTQuestionRewardCfg* GetRewardCfg(int level);
	const WSTQuestionBankCfg* GetQuestionBankCfg(int id);
	const WSTQuestionBankCfg* GetQuestionBankByGroupCfg(int group_id);
	int GetQuestionBankNum() { return m_question_bank_cfg.size(); }
	bool GetFindReward(int level, int type, int *coin, int *exp);
	const int GetRewardGroup(int _role_level);
	const int GetFindType(int _role_level);

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitQuestionBankCfg(TiXmlElement *RootElement);
	int InitFindRewardCfg(TiXmlElement *RootElement);

	WSTQuestionOtherCfg m_other_cfg;
	std::map<int, WSTQuestionRewardCfg> m_reward_cfg;
	std::map<int, WSTQuestionBankCfg> m_question_bank_cfg;
	std::map<int, std::vector<WSTQuestionBankCfg> > m_question_bank_group_cfg;
	std::map<int, std::map<int, WstQuestionFindCfg> > m_find_reward_map;

	std::vector<WstQuestionFindCfg> m_wqfcfg_v;
};

#endif // !__WST_QUESTION_CONFIG_HPP__
