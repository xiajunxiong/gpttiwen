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
		CARD_TYPE_SKIP = 1,				// ���
		CARD_TYPE_REFRESH = 2,			// ���⿨
		CARD_TYPE_REDUCE_ERROR = 3,		// ����

		CARD_TYPE_MAX,
	};

	int card_type;		//��Ƭ����
	int rate;
};

struct KeJuExamOtherCfg
{
	KeJuExamOtherCfg() :answer_time(0), question_num(0), max_satisfaction(0), min_member(0), monster_group(0), fail_satisfaction(0)
	{}

	int answer_time;	//����ʱ��
	int question_num;	//���������
	int max_satisfaction;	//��������
	int min_member;			//�����������
	int monster_group;	//������id
	int fail_satisfaction;	//ս��ʧ�ܿ۷�
};

////////////////////////////////////////////////////////////////////////////////////////
class CycleKeJuExamConfig : public ILogicConfig
{
public:

	enum WORD_TYPE
	{
		WORD_TYPE_ANSWER = 0,		// ����
		WORD_TYPE_SUGGEST = 1,		// ����
		WORD_TYPE_PRAISE = 2,		// ����
		WORD_TYPE_SEEK_HELP = 3,	// ����

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
	int GetOrderCfg(int& group, int& order)const;		// group��orderΪ0 ֱ���������
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

	std::map<int, int> m_question_cfg;			// key-��Ŀid, value-��ȷ��
	std::vector<int> m_quetion_vec_cfg;			// ��Ŀid����

	std::map<int, KeJuExamQuestionResultCfg> m_result_reward_cfg;	// ���⽱��, key-���Ƿ���ȷ
	std::map<int, std::vector<ItemConfigData> > m_last_reward_cfg;	// ��������Ƚ���, key-�����

	std::vector<int> m_npc_cfg;			//npc��������

	int m_total_rate;	// ��Ƭ��Ȩ��
	std::vector<KeJuExamCardCfg> m_card_cfg;		// ���⿨Ƭ

	std::map<int, std::vector<std::pair<int, int> > > m_word_cfg;	// ��������, map_key-��������, pair<����id, Ψһ����index>

	std::vector<int> m_order_group_list_cfg;		// ˳�����б�
	std::map<int, std::vector<std::pair<int, int> > > m_order_cfg;	// ˳������, map1_key-˳����, map2_key-˳��, value-�����߶�����λ��

	KeJuExamOtherCfg m_other_cfg;
};

#endif