#ifndef __CYCLE_ACTIVITY_LANTERN_RIDDLE_CONFIG__
#define __CYCLE_ACTIVITY_LANTERN_RIDDLE_CONFIG__

#include "servercommon/servercommon.h"
#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"

#include <map>
#include <vector>
#include <set>

struct CALRQuestionResultCfg
{
	CALRQuestionResultCfg():satisfaction_up(0)
	{}

	enum RESULT
	{	
		WRONG = 0,
		CORRECT = 1,
	};

	int satisfaction_up;
	std::vector<ItemConfigData> reward_list;
};

struct CALRCardCfg
{
	CALRCardCfg():card_type(0),rate(0)
	{}

	enum CARD_TYPE
	{
		CARD_TYPE_SKIP = 1,				// 免答卡
		CARD_TYPE_REFRESH = 2,			// 换题卡
		CARD_TYPE_REDUCE_ERROR = 3,		// 除错卡

		CARD_TYPE_MAX,
	};

	int card_type;		//卡片类型
	int rate;
};

struct CALROtherCfg
{
	CALROtherCfg():answer_time(0),question_num(0),satisfaction(0), member(0),monster_group(0)
	{}

	int answer_time;	//答题时间
	int question_num;	//需答题数量
	int satisfaction;	//最大满意度
	int member;			//队伍最低人数
	int monster_group;	//怪物组id
	int fail_satisfaction;	//战斗失败扣分
};


class CycleActLanternRiddleConfig : public ILogicConfig
{
public:
	CycleActLanternRiddleConfig();
	virtual ~CycleActLanternRiddleConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	int RandQuestionCfg(std::set<int>& used_questions)const;
	const CALRQuestionResultCfg * GetAnswerResultCfg(int question,int answer, int& result) const;
	const CALRQuestionResultCfg * GetLastSatisfactionCfg()const;
	const std::vector<ItemConfigData>* GetLastRewardCfg(int satisfaction)const;
	const CALROtherCfg& GetOtherCfg()const { return m_other_cfg; }
	int GetRandNPCCfg(int last_npc_seq)const;
	int GetTrueAnswer(int question)const;
	int RandCardCfg()const;
	int RandWordCfg()const;

private:
	int InitQuertionCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitAnswerCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitResultRewardCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitLastRewardCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitOtherCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitNPCCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitCardCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitWordCfg(TiXmlElement *RootElement, std::string& errormsg);

	std::map<int, int> m_question_cfg;								// key-题目id,value-答案组
	std::map<int, int> m_answer_cfg;								// key-答案组,value-正确答案
	std::map<int, CALRQuestionResultCfg> m_result_reward_cfg;		// key-答案是否正确
	std::map<int, std::vector<ItemConfigData> > m_last_reward_cfg;	// key-满意度
	CALROtherCfg m_other_cfg;
	std::vector<int> m_quetion_vec_cfg;	//题目id集合
	std::vector<int> m_npc_cfg;			//npc索引集合
	int m_total_rate;
	std::vector<CALRCardCfg> m_card_cfg;		// 特殊卡片
	std::vector<int> m_word_cfg;				// 气泡
};




#endif
