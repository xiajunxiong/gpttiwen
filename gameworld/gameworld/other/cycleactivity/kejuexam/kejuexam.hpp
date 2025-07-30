#ifndef __KE_JU_EXAM__
#define __KE_JU_EXAM__

#include "servercommon/cycleactivity/kejuexamparam.hpp"
#include "servercommon/servercommon.h"
#include "servercommon/serverdef.h"
#include "protocol/cycleactivity/msgkejuexam.hpp"
#include "gameworld/other/rolemodulemanager.hpp"
#include "kejuexamconfig.hpp"
#include "other/cycleactivity/kejuexam/kejuexammanager.hpp"

#include "config/logicconfigmanager.hpp"
#include "gameworld/obj/character/role.h"
#include "servercommon/errornum.h"
#include "global/team/team.hpp"
#include "world.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "gameworld/internalcomm.h"
#include "gameworld/gamelog.h"
#include "battle/battle_business_local.hpp"
#include "gameworld/battle/battle_manager_local.hpp"
#include "global/zhouqimanager/zhouqimanager.h"
#include "servercommon/zhouqidef.hpp"
#include "global/zhouqimanager/zhouqiconfig.hpp"

#include <vector>
#include <set>


class KeJuExam
{
public:
	KeJuExam();
	~KeJuExam();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager *module_mgr, const KeJuExamParam & param);
	void GetInitParam(KeJuExamParam * param);

	void OnLogin();
	void Update(unsigned long interval, time_t now_second);
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	void OnClientOpReq(Protocol::CSKeJuExamCSReq* req);
	bool IsCanOperator();

	// ��ȡ��ҵ�ǰ�Ѵ�������
	int GetAnswerCount()const { return m_param.used_question_count; }
	// ��ȡ��ǰ��Ŀid
	int GetTeamQuestion() { return m_param.cur_question_id; }
	// ��ȡ����Ѿ���������б�, ����ӵ�set
	void GetUsedQuestion(std::set<int>& used_questions);
	// ��ȡ/����npc_id, ��������npcʱ���������ظ�����ͬһ��
	int GetCurTeamNpc() { return m_cur_npc; }
	void SetCurTeamNpc(int npc_id) { m_cur_npc = npc_id; }
	// ��ȡ/���ô���˳����
	std::pair<int, int> GetGroupOrder() { return std::make_pair(m_param.group_order[0], m_param.group_order[1]); }
	void SetGroupOrder(std::pair<int, int> group_order);

	// ���µ�ǰnpc����Ŀ
	void SetNpcQuestion(int npc_seq, int question_id);
	// ��ʼ���Ⲣ���ó�ʱʱ��
	void SetTimeOut(int answer_uid, int timeout_timestamp);
	// ���⽱��
	void AnswerReward(const KeJuExamQuestionResultCfg& reward_cfg, int answer, int result);
	// ���ⳬʱ
	void AnswerTimeOut();

	// ����
	void Praise(int user_id);
	// ��������
	void SetWord(int word_id) { m_param.word_id = word_id; }
	// ������
	void AddPraiseNum() { m_param.team_info.praise_times += 1; };		

	// ����/����
	void SeekHelp();
	void Suggest(int suggest_answer);

	// ��Ӻ���
	void ApplyFriend(int uid);

	// ս��
	void OnStartLastBattle();
	void OnBattleFinish(bool is_win);

	//ÿ����ȡ���������ÿ�Ƭ
	void DayGetCard();

	void UsedCard();
	bool SkipCard();			//���
	bool RefreshCard();			//���⿨
	bool ReduceErrorCard();		//����

	// ����/��ȡ/�������ʹ�ÿ�Ƭ��Ϣ
	void SetTeamUsedCard() { m_param.team_used_card = 1; }
	bool IsUsedSpecialCard() { return !!m_param.team_used_card; }
	void ClearUsedCard() { m_param.team_used_card = 0; }

	// ����/��ȡ��������ų��Ĵ�
	void SetTeamReduceErrorCard(int error_answer) { m_param.error_answer = error_answer; }
	int GetTeamReduceErrorCard() { return m_param.error_answer; }

	// ͬ�����⿨ˢ�µ���Ŀ
	void SetQuestion(int question_id) { m_param.cur_question_id = question_id; }	

	// ͬ���Ŵ��ų��Ĵ�
	void SetErrorAnswer(int error_answer) { m_param.error_answer = error_answer; }	

	// �����Ƿ����ڴ���, true-���ڴ���, ���ɼ������
	bool IsAnswering() { return !!m_param.team_answering; }	
	// �Ƿ��ͬ����Ŀ��Ϣ
	bool CanSynTeam();
	// ��ȡ������uid
	int GetAnswerUid() { return m_param.answer_uid; }

	// �����һ�����Ϣ
	void ClearPastQuestionInfo();
	// ����/����������һ��������Ĵ�����Ϣ
	void ClearTeamInfo();
	// ��װ��ǰ������ڵ�����Ϣ
	void AssemblePraiseInfo(KeJuExamPraise & info);	
	// ��װ��������Ϣ
	void AssembleLastResultInfo(KeJuExamLastResult &last_result);	

private:
	void SendTimeOutInfo();
	void SendNpcInfo();
	void SendInfo();

	RoleModuleManager * m_module_mgr;

	int m_cur_npc;
	KeJuExamParam m_param;
};


#endif