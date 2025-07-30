#ifndef __SPECIAL_LOGIC_WORLD_ARENA_STANDBY_ROOM_HPP__
#define __SPECIAL_LOGIC_WORLD_ARENA_STANDBY_ROOM_HPP__

#include "scene/speciallogic.hpp"

class Character;
class Scene;

namespace Protocol
{
	struct MsgWorldArenaShowUpRoleInfo;
}

class SpecialLogicWorldArenaStandbyRoom : public SpecialLogic
{
public:
	enum STATUS
	{
		STATUS_NOT_STANDBY = 0,
		STATUS_BOTH_STANDBY,
		STATUS_START_BATTLE,
		STATUS_BATTLE_END,
	};

public:
	SpecialLogicWorldArenaStandbyRoom(Scene *scene);
	virtual ~SpecialLogicWorldArenaStandbyRoom();

	virtual void Update(unsigned long interval, time_t now_second);

	virtual void OnRoleEnterScene(Role *role);					//!< 主角进入场景
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);	//!< 主角离开场景

	virtual bool CanDestroy();
	virtual bool CanEnterTeam(Role * role, Team * team, bool is_from_create) { return false; }
	virtual bool CanInvitationRole(Role *role, int uid) { return false; }
	virtual bool CanChangeTeamLeader(Role * role, Team * team) { return false; }

	long long GetScheduleID();
	void SetScheduleID(long long schedule_id); 
	void SetAttackerUUID(long long attacker_uuid);
	void SetDefenderUUID(long long defender_uuid);
	void SetAttackerUID(int uid);
	void SetDefenderUID(int uid);
	void SetAttackerName(const char* name);
	void SetDefenderName(const char* name);
	void OnInitFinish();
	void SendStandbyReminder();
	void SendForceStartReminderIfNeeded();
	void SendBattleEndLeaveSceneNotice();
	void OnRoleReady(Role* role, bool is_ready);
	void SendCountDownToFightNotice(int count_down_s);
	void SendSomeOneLeaveSceneWaitNotice();
	bool IsInNotStandbyStatus();
	bool IsInBothStandbyStatus();
	bool IsInFightStartStatus();
	bool IsBothRobot();
	bool IsInFightFinishStatus();
	long long GetAttackerUUID() const;
	long long GetDefenderUUID() const;
	bool HasAttackerEverEntered() const; 
	bool HasDefenderEverEntered() const;
	
	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack);
	virtual void OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack);
	virtual void OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack);

	virtual bool CanRoleStartFight(Role * role, CanRoleStartFightParam &fight_param);
private:
	void CreateRobotIfNeeded();
	void CalculateResultIfTwoRobot();
	void SetStatus(int status, const char* src_func);
	void ForceStartBattleIfNeeded();
	bool TryStartBattle();
	void StartBattle(bool is_forced_start);
	int GetAttackerUID();
	int GetDefenderUID();
	void OnBattleEnd();
	void SendShowUpInfo();
	void AssembleShowUpRoleInfo(long long uuid, Protocol::MsgWorldArenaShowUpRoleInfo* role_info);
	void LogRoomStatus(const char* func);
	void SendNoticeOnRoleEnter(Role* role);
	void SendNoticeOnRobotCreated(const char* robot_name);
	void SendOpponentNotShowupYetNotice(Role* role);
	void SendOpponentShowupNotice(Role* role, const char* opponent_name);
	void SendStartBattleFailedDrawNotice(Character* character);
	void SendStartBattleFailedWinNotice(Character* character);
	void SendStartBattleFailedLoseNotice(Character* character);

	long long m_schedule_id;
	long long m_attacker_uuid;
	long long m_defender_uuid;
	int m_attacker_uid;
	int m_defender_uid;
	GameName m_attacker_name;
	GameName m_defender_name;

	unsigned int m_create_robot_timestamp;
	unsigned int m_two_robot_pk_result_timestamp;
	unsigned int m_next_reminder_timestamp;
	unsigned int m_force_start_battle_reminder_timestamp;
	unsigned int m_force_start_battle_timestamp;
	unsigned int m_start_battle_timestamp;
	unsigned int m_kick_all_out_timestamp;
	unsigned int m_countdown_notice_timestamp;

	Character* m_attacker;
	Character* m_defender;
	bool m_is_attacker_robot;
	bool m_is_defender_robot;

	bool m_attacker_ready;
	bool m_defender_ready;
	int m_status;

	bool m_can_destroy;

	bool m_has_attacker_entered;
	bool m_has_defender_entered;
};


#endif