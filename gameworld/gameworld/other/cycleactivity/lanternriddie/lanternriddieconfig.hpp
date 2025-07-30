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
		CARD_TYPE_SKIP = 1,				// ���
		CARD_TYPE_REFRESH = 2,			// ���⿨
		CARD_TYPE_REDUCE_ERROR = 3,		// ����

		CARD_TYPE_MAX,
	};

	int card_type;		//��Ƭ����
	int rate;
};

struct CALROtherCfg
{
	CALROtherCfg():answer_time(0),question_num(0),satisfaction(0), member(0),monster_group(0)
	{}

	int answer_time;	//����ʱ��
	int question_num;	//���������
	int satisfaction;	//��������
	int member;			//�����������
	int monster_group;	//������id
	int fail_satisfaction;	//ս��ʧ�ܿ۷�
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

	std::map<int, int> m_question_cfg;								// key-��Ŀid,value-����
	std::map<int, int> m_answer_cfg;								// key-����,value-��ȷ��
	std::map<int, CALRQuestionResultCfg> m_result_reward_cfg;		// key-���Ƿ���ȷ
	std::map<int, std::vector<ItemConfigData> > m_last_reward_cfg;	// key-�����
	CALROtherCfg m_other_cfg;
	std::vector<int> m_quetion_vec_cfg;	//��Ŀid����
	std::vector<int> m_npc_cfg;			//npc��������
	int m_total_rate;
	std::vector<CALRCardCfg> m_card_cfg;		// ���⿨Ƭ
	std::vector<int> m_word_cfg;				// ����
};




#endif
