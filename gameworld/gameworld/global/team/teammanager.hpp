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
		TOR_SUC						= 0,	// 成功
		TOR_TEAM_NOT_EXIST			= -1,	// 队伍不存在
		TOR_TEAM_NOT_LEADER			= -2,	// 不是队长
		TOR_TEAM_NOT_MEMBER			= -3,	// 没有该成员
		TOR_JOINED_TEAM				= -4,	// 已有队伍
		TOR_NO_TEAM					= -5,	// 没有队伍
		TOR_TEAM_ERROR				= -6,	// 队伍错误
		TOR_TEAM_IS_FULL			= -7,	// 队伍已满员
		TOR_NO_ONLINE				= -8,	// 不在线
		TOR_PARAM_ERROR				= -9,	// 参数错误
		TOR_LEVEL_LIMIT				= -10,	// 等级限制不符合
		TOR_NOT_ALLOW_APPLY			= -11,	// 禁止申请
		TOR_UNKNOWN_ERROR			= -12,	// 未知错误
		TOR_CAN_NOT_CHANGE			= -13,	// 不允许更换队长
		TOR_MEMBER_NOT_ONLINE		= -14,	// 队员不在线
		TOR_ACTIVITY_LIMIT			= -15,	// 活动限制
		TOR_NOT_ALLOW_INVITE		= -16,	// 不符合邀请的条件
		TOR_IN_BATTLE				= -17,	// 在战斗中
	};

	static TeamManager & Instance();

	void Update(unsigned long interval, time_t now_second);

	//创建队伍
	Team * CreateTeam(Role * leader, short limit_level_low = 1, short limit_level_high = MAX_ROLE_LEVEL, int team_type = TEAM_TYPE_DEFAULT, bool notice_flag = true, bool is_match = false, bool allow_red_join = true);
	//解散队伍
	int DismissTeam(Role *user);
	//退出队伍
	int ExitTeam(Role * user);
	void ExitTeam2(int uid);
	//踢人
	int KickOutMember(Role* user, int kick_index);
	//申请加入队伍
	int JoinTeam(Role* user, int team_index, bool is_apply_cross);
	//审批入队申请
	int JoinReqRet(Role* user, int uid, bool agree);
	//邀请组队
	int InviteRet(Role* user, int uid, bool agree);
	//换队长
	int OnChangeLeader(Role* user, int member_index);
	//改变队伍加入是否是要队长通过
	int OnChangeTeamCheckType(Role* user, int check_type);
	//暂离
	int OnTemporarilyPart(Role* user);
	//邀请
	int OnInvitationReq(Role * user, int uid, int invite_type = 0);
	//跨服邀请原服玩家进队
	int OnInvitationReqFromHidden(const MsgRoleBaseInfo & inviter_info, const TeamItem & team_item, int cross_team_type, int cross_team_index, int team_member_num, int target_uid, int limit_level_low, int limit_level_high, int invite_type);
	//原服邀请跨服玩家进队
	int OnInvitationReqFromOrigin(const MsgRoleBaseInfo & inviter_info, const TeamItem & team_item, int cross_team_type, int cross_team_index, int team_member_num, int target_uid, int limit_level_low, int limit_level_high, int invite_type);

	//邀请回复
	int OninvitationRet(Role* user, int uid, bool accept, bool is_from_cross, int team_index);
	
	//招募
	int OnTeamRecruitReq(Role* user);
	//直接加入队伍
	int JoinTeamDirectly(Role* user, int team_index);
	//切换到跨服状态
	int OnTeamSwitchToCrossReq(Role * user);
	//切换到跨服状态2
	int OnTeamGotoHidden(Role * user, int join_activity_type = ACTIVITY_TYPE_INVALID, int param1 = 0, int param2 = 0);
	//切换站位
	int OnSwitchPositionReq(Role* user, short pos1, short pos2);
	//召回暂离队员
	int OnSummonMemberReq(Role* user, int summon_role_id);
	//队员回复召回
	int OnReplySummon(Role* user, int is_return);
	//队长带玩家一起离队
	int OnLeaderExitTeamWithSomeMembers(Role* leader, int member_count, int* member_uid_list);

	//获取队伍id
	int GetTeamIndex(Role* user);
	Team* GetTeam(Role * user);
	Team* GetTeamByUid(int uid);
	Team* GetTeamByTeamIndex(int team_index);

	//改队伍设置
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

	void ReqJoinTeamInfo(Role * role, bool is_team_index_in_cross, int team_index,int inviter_uid);		//客户端30请求邀请加入的队伍信息，用来显示接到邀请队伍列表

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
