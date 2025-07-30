#ifndef __SPECIAL_LOGIC_QING_YUAN_SHENG_HUI_HPP__
#define __SPECIAL_LOGIC_QING_YUAN_SHENG_HUI_HPP__

#include "scene/speciallogic.hpp"
#include <map>
#include <set>


struct QingYuanShengHuiRoleInfo
{
	QingYuanShengHuiRoleInfo()
	{
		role_id = 0;
		qing_yuan_role_id = 0;
		can_gather_next_time = 0;
		battle_times = 0;		//战斗次数
		is_get_qi_yuan_rewards = 0;		//是否已经祈愿了
		battle_reward_flag = 0;
	}

	int role_id;
	int qing_yuan_role_id;
	time_t can_gather_next_time;
	int  battle_times;		//战斗次数
	int  is_get_qi_yuan_rewards;		//是否已经祈愿了
	int battle_reward_flag;
};

struct QingYuanShengHuiRoleGroup
{
	QingYuanShengHuiRoleGroup(int role_id_1, int role_id_2)
	{
		role_group.insert(role_id_1);
		role_group.insert(role_id_2);
	}

	bool operator<(const QingYuanShengHuiRoleGroup & other)const
	{
		return role_group < other.role_group;
	}

	std::set<int> role_group;
};

struct QingYuanShengHuiQuestiontInfo
{
	QingYuanShengHuiQuestiontInfo()
	{
		is_open_ui = false;
		next_is_open_ui = false;

		is_finish = false;
		question_id = 0;
		next_question_time = 0;
		question_num = 0;
	}

	std::set<int> role_group;

	bool is_open_ui;
	bool next_is_open_ui;

	bool is_finish;
	int question_id;
	time_t next_question_time;
	int question_num;
	std::set<int> answered_question_id_list;  //回答过题目列表，不重复出题
	std::map<int, int> answer_list;		//玩家选择答案
};

struct QingYuanShengHuiReadyFightInfo
{
	time_t time;
	std::vector<int> team_index_list;
};

struct QingYuanShengHuiGroupRewardItemCfg;
class SpecialLogicQingYuanShengHui : public SpecialLogic
{
public:
	SpecialLogicQingYuanShengHui(Scene * scene);
	virtual ~SpecialLogicQingYuanShengHui();

	void Reset();

	virtual void Update(unsigned long interval, time_t now_second);
	virtual void UpdateSceneRewards(unsigned long interval, time_t now_second);
	virtual void UpdatePhase(unsigned long interval, time_t now_second);
	virtual void UpdateQuestion(unsigned long interval, time_t now_second);
	virtual void UpdateMatch(unsigned long interval, time_t now_second);
	virtual void UpdateReadyFight(unsigned long interval, time_t now_second);

	virtual void OnRoleEnterScene(Role * role);	//!< 主角进入场景
	virtual void OnRoleLeaveScene(Role * role, bool is_logout);	//!< 主角离开场景

	virtual bool CanDestroy();
	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack);
	virtual void OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight * ack);
	virtual void OnRoleLeaveTeam(Role * role);
	virtual bool CanTeamMemberGotoLeaderFromOtherScene(Role * member, Role * leader);
	virtual bool CanInvitationRole(Role * role, int uid);

	virtual int GetMineMonsterBattleMode() { return BATTLE_MODE_QING_YUAN_SHENG_HUI; }
	virtual bool IsForceEncounterMineMonster() { return false; }

	int OnRoleGather(Role * role);

	int OnRoleQuestionUiOp(Role * role, bool is_open_ui);
	int OnRoleAnswerQuestion(Role * role, int select_num);

	int OnRoleBattleUi(Role * role, int is_open_ui);
	int OnRoleBattleMatch(Role * role, bool is_join, bool is_notice = false);
	int OnRoleFetchBattleReward(Role * role, int reward_times);
	int OnQiYuan(Role * role);

	void GmOp(int type, int p1, int p2);
public:
	void OnActivityBegin();
	void OnActivityClose();
	void SendSceneInfo(Role * role = NULL);
	void SendRoleInfo(Role * role);
	void SendQuestionOpRoute(const std::set<int> & send_roles, bool is_open);
	void SendQuestionInfo(const QingYuanShengHuiQuestiontInfo & question_info);
private:
	QingYuanShengHuiRoleInfo * GetQingYuanShengHuiRoleInfo(int role_id, bool is_create = false);
	QingYuanShengHuiQuestiontInfo * GetQuestiontInfo(int role_id, bool is_create = false);
	bool CheckQingYuanShengHuiQuestiontInfo(QingYuanShengHuiQuestiontInfo & question_info, time_t now_second);
	bool IsInMatching(int team_index);
	bool PutReward(Role * role, const QingYuanShengHuiGroupRewardItemCfg * reward_cfg);
	void KickOutQingYuanRole(Role * role);
private:
	bool m_is_start;
	bool m_is_finish;
	bool m_role_num_change;
	int m_phase;
	time_t m_phase_start_time;
	time_t m_phase_end_time;
	time_t m_scene_reward_next_time;

	std::map<int, QingYuanShengHuiRoleInfo> m_role_info;
	std::map<QingYuanShengHuiRoleGroup, QingYuanShengHuiQuestiontInfo> m_question_info;

	std::set<int> m_match_team_index;
	std::list<QingYuanShengHuiReadyFightInfo> m_ready_fight_info;
};

#endif