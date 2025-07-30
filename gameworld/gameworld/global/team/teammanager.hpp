#ifndef __TEAM_MANAGER_HPP__
#define __TEAM_MANAGER_HPP__

#include <map>
#include <vector>

#include "common/raobjlist.h"
#include "servercommon/serverdef.h"
#include "protocol/msgteam.h"
#include "servercommon/teamdef.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "teamchannel.hpp"
#include "servercommon/activitydef.hpp"

struct TeamGoToHiddenInfo
{
	int type;
	int p1;
	int leader_id;
	std::map<int, bool> login_flag;
};

typedef void(*TeamMemberGoToFunction)(int role_id, int target_scene_id, int target_scene_key, bool go_ret, void * param);


class Team;
class Role;
class Robot;
class TeamManager
{
public:

	enum TEAM_OPER_RESULT
	{
		TOR_SUC						= 0,	// �ɹ�
		TOR_TEAM_NOT_EXIST			= -1,	// ���鲻����
		TOR_TEAM_NOT_LEADER			= -2,	// ���Ƕӳ�
		TOR_TEAM_NOT_MEMBER			= -3,	// û�иó�Ա
		TOR_JOINED_TEAM				= -4,	// ���ж���
		TOR_NO_TEAM					= -5,	// û�ж���
		TOR_TEAM_ERROR				= -6,	// �������
		TOR_TEAM_IS_FULL			= -7,	// ��������Ա
		TOR_NO_ONLINE				= -8,	// ������
		TOR_PARAM_ERROR				= -9,	// ��������
		TOR_LEVEL_LIMIT				= -10,	// �ȼ����Ʋ�����
		TOR_NOT_ALLOW_APPLY			= -11,	// ��ֹ����
		TOR_UNKNOWN_ERROR			= -12,	// δ֪����
		TOR_CAN_NOT_CHANGE			= -13,	// ����������ӳ�
		TOR_MEMBER_NOT_ONLINE		= -14,	// ��Ա������
		TOR_ACTIVITY_LIMIT			= -15,	// �����
		TOR_NOT_ALLOW_INVITE		= -16,	// ���������������
		TOR_IN_BATTLE				= -17,	// ��ս����
	};

	static TeamManager & Instance();

	void Update(unsigned long interval, time_t now_second);

	//��������
	Team * CreateTeam(Role * leader, short limit_level_low = 1, short limit_level_high = MAX_ROLE_LEVEL, int team_type = TEAM_TYPE_DEFAULT, bool notice_flag = true, bool is_match = false, bool allow_red_join = true);
	//��ɢ����
	int DismissTeam(Role *user);
	//�˳�����
	int ExitTeam(Role * user);
	void ExitTeam2(int uid);
	//����
	int KickOutMember(Role* user, int kick_index);
	//����������
	int JoinTeam(Role* user, int team_index, bool is_apply_cross);
	//�����������
	int JoinReqRet(Role* user, int uid, bool agree);
	//�������
	int InviteRet(Role* user, int uid, bool agree);
	//���ӳ�
	int OnChangeLeader(Role* user, int member_index);
	//�ı��������Ƿ���Ҫ�ӳ�ͨ��
	int OnChangeTeamCheckType(Role* user, int check_type);
	//����
	int OnTemporarilyPart(Role* user);
	//����
	int OnInvitationReq(Role * user, int uid, int invite_type = 0);
	//�������ԭ����ҽ���
	int OnInvitationReqFromHidden(const MsgRoleBaseInfo & inviter_info, const TeamItem & team_item, int cross_team_type, int cross_team_index, int team_member_num, int target_uid, int limit_level_low, int limit_level_high, int invite_type);
	//ԭ����������ҽ���
	int OnInvitationReqFromOrigin(const MsgRoleBaseInfo & inviter_info, const TeamItem & team_item, int cross_team_type, int cross_team_index, int team_member_num, int target_uid, int limit_level_low, int limit_level_high, int invite_type);

	//����ظ�
	int OninvitationRet(Role* user, int uid, bool accept, bool is_from_cross, int team_index);
	
	//��ļ
	int OnTeamRecruitReq(Role* user);
	//ֱ�Ӽ������
	int JoinTeamDirectly(Role* user, int team_index);
	//�л������״̬
	int OnTeamSwitchToCrossReq(Role * user);
	//�л������״̬2
	int OnTeamGotoHidden(Role * user, int join_activity_type = ACTIVITY_TYPE_INVALID, int param1 = 0, int param2 = 0);
	//�л�վλ
	int OnSwitchPositionReq(Role* user, short pos1, short pos2);
	//�ٻ������Ա
	int OnSummonMemberReq(Role* user, int summon_role_id);
	//��Ա�ظ��ٻ�
	int OnReplySummon(Role* user, int is_return);
	//�ӳ������һ�����
	int OnLeaderExitTeamWithSomeMembers(Role* leader, int member_count, int* member_uid_list);

	//��ȡ����id
	int GetTeamIndex(Role* user);
	Team* GetTeam(Role * user);
	Team* GetTeamByUid(int uid);
	Team* GetTeamByTeamIndex(int team_index);

	//�Ķ�������
	int SetTeamInfo(Role * user, short limit_level_low = 0, short limit_level_high = MAX_ROLE_LEVEL, int team_type = TEAM_TYPE_DEFAULT, bool allow_red_join = true);

	void TeamDismiss(Team* team);
	void TeamBackFromTeamRegister(Team * team);

	int UserLogin(const UserID& user_id);
	int UserLogout(Role* user);

	void ClearTeamMatch(Team* team);
	void ClearUserMatch(Role* user);
	void ChangePartnerFightState(Role* user);

	int SendUnfullTeamListToUser(Role* user, int team_type);
	void QueryUnfullChivalrousTeamList(Role* user);

	void OnTeamInfoChange(Team* team);
	void SyncAllCrossTeamInfo(int to_plat_type, int to_server_id);
	void OnSyncCrossTeamInfo(crossgameprotocal::HiddenGameSyncCrossTeamInfo* msg);
	int ApplyJoinCrossTeam(crossgameprotocal::GameHiddenUserApplyJoinCrossTeam* msg);

	TeamChannel* GetTeamChannel() { return &m_team_channel; }

	bool RobotJoinTeam(Role* role, Robot* robot);
	Team* GetJoinableTeamByTeamType(int team_type);

	void OnRoleEnterTeam(int uid, int team_index);
	void OnRoleExitTeam(int uid);

	void OnLeaveFB(Role * role);

	void OnDisconnectFromCrossServer();

	void OnTeamSwitchToStaticScene(Team* team);
	void OnTeamSwitchToSpecialScene(Team* team);

	void OnUserLoginOriginGame(int role_id);

	void ReqJoinTeamInfo(Role * role, bool is_team_index_in_cross, int team_index,int inviter_uid);		//�ͻ���30�����������Ķ�����Ϣ��������ʾ�ӵ���������б�

	bool TeamGotoScene(Role * role, int target_scene_id, int target_scene_key, Posi  target_pos, bool is_set_target_pos = false);
	void TeamGotoScene(Team * team, int target_scene_id, int target_scene_key, Posi  target_pos, bool is_set_target_pos = false, TeamMemberGoToFunction func = NULL, void * func_param = NULL);
	void TeamGotoSceneByTeamIndex(int team_index, int target_scene_id, int target_scene_key, Posi  target_pos, bool is_set_target_pos = false, TeamMemberGoToFunction func = NULL, void * func_param = NULL);

	void SyncCrossTeamInfo(Team* team, int sync_reason);
private:
	TeamManager();
	~TeamManager();

	void OnTeamCreate(Team* team, bool is_start_match);
	void OnTeamDismiss(Team* team);
	void SendErrorNumToOriginServer(int origin_uid, int origin_plat_type, int origin_server_id, int errnum);

	int m_team_index;
	typedef std::map<int, Team*> TeamList;
	typedef std::map<int, Team*>::iterator TeamListIt;
	TeamList m_team_list;
	TeamList m_delay_delete_list;

	typedef std::map<int, int> UserTeamIndexMap;
	typedef std::map<int, int>::iterator UserTeamIndexMapIt;
	UserTeamIndexMap m_user_teamindex_map;

	typedef std::map<int, TeamItem> CrossTeamInfoMap;
	CrossTeamInfoMap m_cross_team_info_map;
	
	TeamChannel m_team_channel;

	std::map<int, TeamGoToHiddenInfo> m_team_goto_info;
};

#endif // __TEAM_MANAGER_HPP__
