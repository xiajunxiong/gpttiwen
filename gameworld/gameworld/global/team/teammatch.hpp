#ifndef __TEAM_MATCH_HPP__
#define __TEAM_MATCH_HPP__

#include <map>
#include "servercommon/servercommon.h"

enum MATCH_STATUS
{
	MATCH_STATUS_INVALID = 0,
	MATCH_STATUS_MATCHING,		// ����ƥ��
	MATCH_STATUS_SUSPEND,		// ��ͣƥ��
};

static const int MATCH_INTERVAL_S = 1;

class Team;
class Role;
struct TeamTypeRobotCfg;
struct MatchTeamInfo
{
	MatchTeamInfo() : is_auto_add_robot(true), team_index(0), team(NULL), match_status(MATCH_STATUS_INVALID),
		start_match_time(0u), suspend_end_time(0u), next_add_robot_time(0u), 
		try_real_role_robot_time(0u), creator_guild_id(0){}

	bool IsSuspend();
	bool IsFullForMatch();
	MatchTeamInfo* Clone();
	void AddUserToEnteringList(const UniqueUserID& uuid);
	void RemoveUserFromEnteringList(const UniqueUserID& uuid);

	void * operator new(size_t c);
	void operator delete(void *m);

	bool is_auto_add_robot;				// �Ƿ��Զ��ӻ�����
	int team_index;						// �����ʶ
	Team* team;							// ����ָ��
	int match_status;					// ƥ��״̬
	unsigned int start_match_time;		// ��¼��ʼƥ���ʱ��
	unsigned int suspend_end_time;		// �����ͣƥ���ʱ��
	unsigned int next_add_robot_time;	// �´μ��������ʱ��
	unsigned int try_real_role_robot_time;	// ��ʼ���Լ���ʵ��һ�����ʱ��
	int creator_guild_id;				// �������id
	std::vector<UniqueUserID> entering_list; // ��¼���ڽ��������б�
};

struct MatchUserInfo
{
	MatchUserInfo() : match_status(MATCH_STATUS_INVALID), start_match_time(0u), 
		start_enter_team_time(0u), enter_team_index(0), match_team_type(0),
		user_level(0), user_guild_id(0), is_cross_matching(false), is_auto_create(false) {}

	bool IsSuspend();

	void * operator new(size_t c);
	void operator delete(void *m);

	UniqueUserID uuid;					// Ҫ���ֺ�uuid�������ط����Ǳ���
	int match_status;					// ���ƥ��״̬
	unsigned int start_match_time;		// ��¼��ʼƥ���ʱ��
	unsigned int start_enter_team_time; // ��¼��ʼ���ӵ�ʱ��
	int enter_team_index;				// ��¼Ҫ����
	int match_team_type;				// Ҫƥ��Ķ���Ŀ������
	int user_level;						// ��ҵĵȼ�
	int user_guild_id;					// ��Ҽ���ID
	bool is_cross_matching;				// �Ƿ����ڿ��ƥ�䣨��ԭ���ã�
	bool is_auto_create;				// һ��ʱ��ƥ�䲻�����Ƿ��Զ���������[0:�� 1 : ��]
};

namespace crossgameprotocal
{
	struct HiddenGameQueryOfflineBattleDataForRobot;
}

struct RoleBattleData;
class TeamMatch
{
public:
	typedef std::map<int, MatchTeamInfo*> MatchTeamInfoMap;
	typedef std::map<UniqueUserID, MatchUserInfo*> MatchUserInfoMap;	

public:
	static TeamMatch& Instance();

	void Update(long long intervals, unsigned int now);

	void OnUserLogin(const UserID& user_id);
	void OnUserLogout(Role* role);

	bool OnTeamStartMatch(Team* team);
	bool OnTeamStopMatch(Team* team);

	bool OnUserStartMatch(const UniqueUserID& uuid, int user_level, int match_team_type, GuildID user_guild_id, bool is_auto_create = false);
	bool OnUserStartCrossMatch(const UniqueUserID& uuid, int user_level, int match_team_type, GuildID user_guild_id, bool is_auto_create = false);
	void OnUserStopMatch(const UniqueUserID& uuid);
	void OnUserStopCrossMatch(const UniqueUserID& uuid);
	void OnUserStopLocalMatch(const UniqueUserID& uuid);

	MatchTeamInfo* GetMatchTeamInfo(int team_index);
	MatchUserInfo* GetMatchUserInfo(const UniqueUserID& uuid);
	
	void OnEnterTeamSucc(const UniqueUserID& uuid, int team_index);
	void OnEnterTeamFail(const UniqueUserID& uuid, bool continue_match = true);

	void SendMatchInfo(Role* user, int is_matching);
	void GmAddUserToMatch();

	void OnOfflineBattleDataForRobotResp(crossgameprotocal::HiddenGameQueryOfflineBattleDataForRobot* msg);
private:
	void RemoveMatchTeamInfo(MatchTeamInfo** mti);
	void RemoveMatchUserInfo(MatchUserInfo** mui);
	void Release();
	void DoMatch();
	void DoAutoAddRobot();
	void DoSuspendEndRestartMatch();
	void OnMatchSucc(MatchUserInfo* mui);
	bool OnAddRobotToTeam(MatchTeamInfo* mti);
	bool OnAddRobotToUser(MatchUserInfo* mui);
	bool TryMatchTeamWithRobot(MatchUserInfo* mui);
	void CheckExpireMatchUser();
	void AddNormalRobotToTeam(int add_robot_num, Team* team, Role* leader_role, const TeamTypeRobotCfg* cfg);
	void TryAddRealRoleRobotToTeam(int add_robot_num, Team* team, Role* leader_role, const TeamTypeRobotCfg* cfg);
	bool OnAddRealRoleRobotToTeam(MatchTeamInfo* mti, int uid, const RoleBattleData& rbd);
	bool IsWaitTooLongForRealRoleRobot(const MatchTeamInfo* mti, unsigned int now) const;
	void ResetTryRealRoleRobotTime(MatchTeamInfo* mti);
	void SetTryRealRoleRobotTime(MatchTeamInfo* mti, unsigned int now);

	MatchUserInfoMap m_matching_user_map;	// ����ƥ�������б�
	MatchUserInfoMap m_suspend_user_map;	// �Ѿ��ҵ����飬���ڽ��ӵ�����б�

	MatchTeamInfoMap m_matching_team_map;	// ����ƥ��Ķ����б�
	MatchTeamInfoMap m_suspend_team_map;	// ��ʱ�Ѽ����˵��б����˽���֮ǰ���ȵȼ�����ܼ���ȥƥ��



	unsigned int m_next_match_time;

	INSTANCE_FORBID_COPY_AND_EXTERIAL_CREATE(TeamMatch);
};

#endif