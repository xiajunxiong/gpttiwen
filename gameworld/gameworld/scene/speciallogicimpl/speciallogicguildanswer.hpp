#ifndef __SPECIAL_LOGIC_GUILD_ANSWER_HPP__
#define __SPECIAL_LOGIC_GUILD_ANSWER_HPP__

#include "scene/speciallogic.hpp"
#include "config/activityconfig/guildanswer/activityguildanswerconfig.hpp"

#include <set>
#include <vector>

class SpecialLogicGuildAnswer : public SpecialLogic
{
public:
	SpecialLogicGuildAnswer(Scene * scene);
	virtual ~SpecialLogicGuildAnswer();

	virtual bool CanDestroy();
	virtual bool CanLeaveGuild() { return false; }
	virtual void Update(unsigned long interval, time_t now_second);
	virtual void OnRoleEnterScene(Role *role);
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);

	void OnGuildAnswerStandy();
	void OnGuildAnswerBegin(const std::vector<int>& question_vec);
	void OnGuildAnswerOver();
	void CommitAnswer(Role * role, int answer);
	void OnNextQuestion(bool is_ready_state);

private:
	void SendCurQuestionInfo();
	void CheckAnswerMap();		// �����Ҵ𰸱����������ҷ��Ŷ�Ӧ����
	void GiveGuildAnswerAward(Role * role, bool is_correct);
	void SendAnswerResult(Role * role, bool is_correct);	// ���ʹ����� - ��Ի���
	void SendRoleAwardList();

	time_t m_act_end_timestamp;
	time_t m_next_send_question_timestamp;
	int m_cur_question_seq;			// ��ǰ���ʵ��ڼ��� - �α�� 0 ��ʼ
	std::vector<int> m_question_id_vec;
	std::map<UserID, int> m_players_answer_map;		// �����ѡ�Ĵ�
	std::set<int> m_participate_set;				// ��¼��������uid

	std::map<UserID, GuildAnswerAwardTotalCon> m_players_award_map;
	time_t m_close_activity_timestamp;
};

#endif