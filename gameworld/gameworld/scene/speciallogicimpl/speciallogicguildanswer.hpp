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
	void CheckAnswerMap();		// 检查玩家答案表，给答题的玩家发放对应奖励
	void GiveGuildAnswerAward(Role * role, bool is_correct);
	void SendAnswerResult(Role * role, bool is_correct);	// 发送答题结果 - 答对或答错
	void SendRoleAwardList();

	time_t m_act_end_timestamp;
	time_t m_next_send_question_timestamp;
	int m_cur_question_seq;			// 当前提问到第几题 - 游标从 0 开始
	std::vector<int> m_question_id_vec;
	std::map<UserID, int> m_players_answer_map;		// 玩家所选的答案
	std::set<int> m_participate_set;				// 记录参与的玩家uid

	std::map<UserID, GuildAnswerAwardTotalCon> m_players_award_map;
	time_t m_close_activity_timestamp;
};

#endif