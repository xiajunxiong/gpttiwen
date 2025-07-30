#ifndef __SPECIAL_LOGIC_UTA_HPP__
#define __SPECIAL_LOGIC_UTA_HPP__

#include "scene/speciallogic.hpp"
#include "servercommon/struct/bigcross/universeteamarena/universeteamarenadef.hpp"

#ifdef _DEBUG
//#define UTA_BATTLE_TEST
#endif

struct UTASpecialLogicParam
{
	UTASpecialLogicParam() : schedule_id(0), team_unique_id(0), opponent_team_unique_id(0),
		is_main_match(false), is_win_group(false), round_idx(0), destroy_timestamp(0u),
		start_battle_timestamp(0u), is_attacker(false)
	{
		memset(member_uid_list, 0, sizeof(member_uid_list));
		memset(team_name, 0, sizeof(team_name));
		memset(opponent_team_name, 0, sizeof(opponent_team_name));
		
#ifdef UTA_BATTLE_TEST
		memset(opponent_uid_list, 0, sizeof(opponent_uid_list));
#endif
	}

	int member_uid_list[UTA_TEAM_MEMBER_NUM];
	long long schedule_id;
	long long team_unique_id;
	GameName team_name;
	long long opponent_team_unique_id;
	GameName opponent_team_name;
	bool is_main_match;
	bool is_win_group;
	int round_idx;
	unsigned int destroy_timestamp;
	unsigned int start_battle_timestamp;
	bool is_attacker;

#ifdef UTA_BATTLE_TEST
	int opponent_uid_list[UTA_TEAM_MEMBER_NUM];
#endif
};

struct UTATeamMemberDetailInfo
{
	UTATeamMemberDetailInfo() : profession(0), avatar_type(0), level(0), headshot_id(0)
	{
		memset(member_name, 0, sizeof(member_name));
	}

	GameName member_name;
	int profession;
	short avatar_type;
	short level;
	short headshot_id;
};

namespace bigchgameprotocol
{
	struct BigCHGameUTAScheduleTeamMemberDetailResp;
}

class Role;
class Team;
class SpecialLogicUTA : public SpecialLogic
{
public:
	SpecialLogicUTA(Scene* scene);
	virtual ~SpecialLogicUTA();

	void InitBusinessParam(const UTASpecialLogicParam& param);

	virtual void Update(unsigned long interval, time_t now_second);

	virtual void OnRoleEnterScene(Role *role);					//!< 主角进入场景
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);	//!< 主角离开场景

	virtual bool CanDestroy();
	virtual bool CanEnterTeam(Role * role, Team * team, bool is_from_create);
	virtual bool CanInvitationRole(Role *role, int uid);
	virtual bool CanChangeTeamLeader(Role * role, Team * team);
	virtual bool CanTeamMemberGotoLeaderFromOtherScene(Role* member, Role* leader);
	virtual bool CanRoleStartFight(Role * role, CanRoleStartFightParam &fight_param);

	void ResetForNextBattle();
	bool IsTeamMember(int uid);
	void OnTeamMemberDetailResp(bigchgameprotocol::BigCHGameUTAScheduleTeamMemberDetailResp* msg);
	void OnBattleDataChanged(Role* role);

	void OnBattleEndDelayKickoutAll();
private:
	void RequestTeamMemberDetailInfo();
	void SendBattleVSInfo();
	void SyncRoleBattleInfoToBigCross(Role* role);
	void CheckRoleNeedsSyncBattleData(Role* role, bool ignore_current_time);
	void CheckAllRoleNeedsSyncBattleData(unsigned int now);
	void SendSceneInfo(Role* role = NULL);

#ifdef UTA_BATTLE_TEST
	void MakeRobotDataForOpponent();
#endif

	int m_member_uid_list[UTA_TEAM_MEMBER_NUM];
	long long m_schedule_id;
	long long m_team_unique_id;
	GameName m_team_name;
	long long m_opponent_team_unique_id;
	GameName m_opponent_team_name;
	bool m_is_main_match;
	bool m_is_win_group;
	int m_round_idx;
	unsigned int m_destroy_timestamp;
	bool m_can_destroy;
	unsigned int m_start_battle_timestamp;
	bool m_is_attacker;	// 我方是否攻方
	bool m_has_send_vs_info;

	UTATeamMemberDetailInfo m_ally_member_infos[UTA_TEAM_MEMBER_NUM];
	UTATeamMemberDetailInfo m_opponent_member_infos[UTA_TEAM_MEMBER_NUM];

#ifdef UTA_BATTLE_TEST
	int m_opponent_uid_list[UTA_TEAM_MEMBER_NUM];
#endif

	unsigned int m_role_next_sync_battle_data_timestamps[UTA_TEAM_MEMBER_NUM];

};

#endif