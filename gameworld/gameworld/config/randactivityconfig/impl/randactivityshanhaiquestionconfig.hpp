#ifndef __RAND_ACTIVITY_SHAN_HAI_QUESTION_CONFIG_HPP__
#define __RAND_ACTIVITY_SHAN_HAI_QUESTION_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"

class RandActivityManager;

static const int MAX_REWARD_ITEM_LIST = 3;
static const int MAX_QUESTION_NUM = 20;


struct BasisCfg
{
	BasisCfg(): section_start(0), section_end(0), suc_reward_group(0), fault_reward_group(0)
	{}

	int section_start;			// 开始时间
	int section_end;			// 结束时间
	int suc_reward_group;		// 正确奖励组
	int fault_reward_group;		// 错误奖励组
	std::vector<int> question_id;
};

struct RewardGroupCfg
{
	RewardGroupCfg(): seq(0), reward_group(0),exp(0),coin(0),rate(0)
	{}
	static const int EXP_BASE_NUM = 10000;

	int seq;
	int reward_group;		// 奖励组
	int exp;				
	int coin;
	int rate;	
	std::vector<ItemConfigData> reward_item;
};

struct QuestionBankCfg
{
	QuestionBankCfg(): question_id(0),anwser_id(0),rate(0), rate_section(0)
	{}

	int question_id;
	int anwser_id;
	int rate;				//权重占比
	int rate_section;		//权重位
};


struct QuestionOtherCfg
{
	QuestionOtherCfg():start_times(0),end_times(0),ans_times(0)
	{}

	int start_times;	// 开始时间
	int end_times;		// 结束时间
	int ans_times;		// 可答题次数
};


class RandActivityShanHaiQuestionConfig : public RandActivityConfig
{
public:
	RandActivityShanHaiQuestionConfig();
	virtual ~RandActivityShanHaiQuestionConfig();
	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	bool IsSucAnswer(int question_id, int answer_id)const;			// 匹配答案
	int GetTotalRate()const { return m_total_rate; }
	int GetBasisSucCfg(int rand_open_day)const;
	int GetBasisFaultCfg(int rand_open_day)const;
	const RewardGroupCfg * GetRewardGroupCfg(int reward_group)const;
	const RewardGroupCfg * GetRewardGroupBySeqCfg(int reward_seq)const;
	const QuestionBankCfg * GetQuestionBankCfg(int question_id)const;
	const QuestionBankCfg * GetRandQuestionCfg(int rand_rate)const;
	const QuestionOtherCfg & GetQuertionOtherCfg()const { return m_other_cfg; }
	
private:
	int InitBasisCfg(TiXmlElement * RootElement);
	int InitRewardCfg(TiXmlElement * RootElement);
	int InitQuestionCfg(TiXmlElement * RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);

	std::vector<BasisCfg> m_basis_cfg;					
	std::vector<RewardGroupCfg> m_reward_group_cfg;	// 奖励组

	int m_total_rate;		// 总权重
	std::map<int, QuestionBankCfg> m_question_bank_cfg;	// key-问题id
	QuestionOtherCfg m_other_cfg;
};



#endif
