#ifndef __KE_JU_EXAM_CONFIG_HPP__
#define __KE_JU_EXAM_CONFIG_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"

#include <map>
#include <vector>
#include <set>

struct KeJuExamQuestionResultCfg
{
	KeJuExamQuestionResultCfg() :satisfaction_up(0)
	{}

	enum RESULT
	{
		WRONG = 0,
		CORRECT = 1,
	};

	int satisfaction_up;
	std::vector<ItemConfigData> reward_list;
};

struct KeJuExamCardCfg
{
	KeJuExamCardCfg() :card_type(0), rate(0)
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

struct KeJuExamOtherCfg
{
	KeJuExamOtherCfg() :answer_time(0), question_num(0), max_satisfaction(0), min_member(0), monster_group(0), fail_satisfaction(0)
	{}

	int answer_time;	//答题时间
	int question_num;	//需答题数量
	int max_satisfaction;	//最大满意度
	int min_member;			//队伍最低人数
	int monster_group;	//怪物组id
	int fail_satisfaction;	//战斗失败扣分
};

////////////////////////////////////////////////////////////////////////////////////////
class CycleKeJuExamConfig : public ILogicConfig
{
public:

	enum WORD_TYPE
	{
		WORD_TYPE_ANSWER = 0,		// 答题
		WORD_TYPE_SUGGEST = 1,		// 建议
		WORD_TYPE_PRAISE = 2,		// 点赞
		WORD_TYPE_SEEK_HELP = 3,	// 求助

		WORD_TYPE_MAX,
	};

public:
	CycleKeJuExamConfig();
	virtual ~CycleKeJuExamConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	int RandQuestionCfg(std::set<int>& used_questions)const;
	const KeJuExamQuestionResultCfg * GetAnswerResultCfg(int question, int answer, int& result) const;
	const std::vector<ItemConfigData>* GetLastRewardCfg(int satisfaction)const;
	int GetRandNPCCfg(int last_npc_seq)const;
	int GetTrueAnswer(int question)const;
	int RandCardCfg()const;
	int RandWordCfg(int word_type, int word_id = 0)const;
	int GetOrderCfg(int& group, int& order)const;		// group或order为0 直接重新随机
	const KeJuExamOtherCfg& GetOtherCfg()const { return m_other_cfg; }

private:
	int InitQuertionCfg(TiXmlElement *RootElement, std::string& errormsg);	
	int InitResultRewardCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitLastRewardCfg(TiXmlElement *RootElement, std::string& errormsg);	
	int InitNPCCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitCardCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitWordCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitOrderCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitOtherCfg(TiXmlElement *RootElement, std::string& errormsg);

	std::map<int, int> m_question_cfg;			// key-题目id, value-正确答案
	std::vector<int> m_quetion_vec_cfg;			// 题目id集合

	std::map<int, KeJuExamQuestionResultCfg> m_result_reward_cfg;	// 答题奖励, key-答案是否正确
	std::map<int, std::vector<ItemConfigData> > m_last_reward_cfg;	// 最终满意度奖励, key-满意度

	std::vector<int> m_npc_cfg;			//npc索引集合

	int m_total_rate;	// 卡片总权重
	std::vector<KeJuExamCardCfg> m_card_cfg;		// 特殊卡片

	std::map<int, std::vector<std::pair<int, int> > > m_word_cfg;	// 气泡配置, map_key-气泡类型, pair<气泡id, 唯一索引index>

	std::vector<int> m_order_group_list_cfg;		// 顺序组列表
	std::map<int, std::vector<std::pair<int, int> > > m_order_cfg;	// 顺序配置, map1_key-顺序组, map2_key-顺序, value-答题者队伍中位置

	KeJuExamOtherCfg m_other_cfg;
};

#endif