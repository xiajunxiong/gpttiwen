#ifndef __TEAM_HPP__
#define __TEAM_HPP__

#include <vector>
#include <string>
#include <set>
#include <map>

#include "servercommon/servercommon.h"
#include "gamedef.h"
#include "protocol/msgscene.h"
#include "teamvote.hpp"
#include "protocol/msgteam.h"
#include "protocol/msgother.h"

class Role;
class Robot;
class Character;

enum AGREE_BEFORE_ENTER_TYPE
{
	AGREE_BEFORE_ENTER_TYPE_INVALID,		
	AGREE_BEFORE_ENTER_TYPE_FB ,				//����
	AGREE_BEFORE_ENTER_TYPE_BATTLE,				//ֱ�ӿ����
	AGREE_BEFORE_ENTER_TYPE_TASK_BATTLE,		//��ս����
};


struct TeamMember
{
	TeamMember() { this->Reset(); }
	bool Invalid() { return uid == 0; };
	void Reset()
	{
		uid = 0;
		level = 0;
		profession = 0;
		avatar_type = 0;
		headshot_id = 0;
		memset(member_name, 0, sizeof(member_name));
		is_online = true;
		is_follow = true;
		appearance.Reset();
		//role = NULL;
		//robot = NULL;
		notice_on_double_speed = false;
	}

	bool IsRobot()
	{
		return IntToUid(uid).db_index == ROBOT_DB_INDEX;
	}

	bool IsFollow()
	{
		return is_follow;
	}

	int			uid;
	int			level;
	int			profession;
	short		avatar_type;
	short		headshot_id;
	GameName	member_name;
	bool		is_online;
	bool		is_follow;
	bool		is_follow_before_logout;
	Appearance	appearance;
	//Role*		role;
	//Robot*		robot;

	bool		notice_on_double_speed;	
};

struct JoinReqInfo
{
	JoinReqInfo() : uid(0), level(0), profession(0), avatar_timestamp(0), req_tmestamp(0), avatar_type(0), headshot_id(0),
	 is_cross_req(false), origin_plat_type(0), origin_server_id(0), req_index(0) {}

	int uid;
	int level;
	int profession;
	long long avatar_timestamp;
	unsigned int req_tmestamp;
	GameName name;
	short avatar_type;
	short headshot_id;	
	bool is_cross_req;
	int origin_plat_type;
	int origin_server_id;
	int req_index;
};

struct MemberPosInfo
{
	MemberPosInfo() : id(0), member_index(-1) {}

	bool IsValid() { return id > 0; }
	void Reset()
	{
		id = 0;
		member_index = -1;
	}

	int id;				// uid
	int member_index;	// Ϊroleʱ��Ч
};

struct TeamHuanJieParam
{
	TeamHuanJieParam()
	{
		is_open = 0;
	}

	int is_open;
};

namespace Protocol
{
	class CSFormationSave;
}

class Scene;
struct TeamItem;
class TemporaryChatGroup;
class Team
{
public:
	Team();
	Team(Team & team);
	~Team();
	
	int UserLogin(Role* user);
	int UserLogout(Role* user);
	int UserLevelUp(Role* user);

	void Update(unsigned long interval, time_t now_second);

	//--------------------------�ӳ��������
	void EjectWindosControl(Role * role, int windows_type);
	
	//--------------------------����ǰ����ͬ����غ���--------------------------------------------
	void SetEnterFbInfo(Role * role, int enter_type, int enter_type_param = 0, int param1 = 0, int param2 = 0, int param3 = 0);
	void OnEnterFbRet(Role * role, bool is_agree);
	void RobotAgreeEnterFB();
	void ResetEnterFbInfo(bool is_notice = true);
	void SendEnterInfo();	
	//----------------------�������ط���ִ�в���---------------------------
	const static int AUTO_JOIN_LIMIT_TIME = 5;
	void SetAutoInfoGotoHidden(int auto_join_type, int join_type_param1, int p1, const std::map<int, int>& all_member_roleid);		//ԭ���μӻ�����ط����Զ����������Ϣ,�ȶ�Ա������Զ�����
	void AutoJoinUpdate(time_t now_second);
	void ResetAutoJoinInfo();
	bool AutoJoinActivityAfterTeamLoginHidden( int activity_type, int p1, int p2);
	void NoticeToggleUpdate(time_t now_second);
	void SetNotice(bool flag, time_t t = 0);
	bool IsNotice();
	//--------------------------------------------------
	void NoticeAllMember(int errornum_id);//ȫ��Ʈ��
	bool IsInFb();

	void CheckMemberItem(Role * leader,int item_id,int num);

	int GetLeaderLevel();
	void SetTeamIndex(int team_index);
	void SetTeamLimit(short limit_level_low, short limit_level_high);
	void SetTeamNotice(TeamNotice team_notice);
	void SetLeader(int index);
	bool SetTeamType(int type);
	bool SetTeamAllowRedJoin(bool team_allow_red_join);
	void SetTeamCheckType(int check_type);
	bool AutoSetLeader(bool is_from_create = false, bool find_not_in_battle = false);
	bool AddMember(Role* user, bool is_from_create = false);
	void DismissTeamHelper();
	int OnTemporarilyPart(Role* user);
	void AddRobot();
	void AddRobot(Robot* robot);
	void SetIsCross(bool is_cross) { m_is_cross = is_cross; }
	bool GetIsCross() const { return m_is_cross; }

	void LeaveTeamByUid(int role_id, bool is_notice_team = true);
	void LeaveTeam(const UserID& user_id, bool is_notice_team = true);
	void LeaveTeam(int member_index, bool is_notice_team = true);
	int JoinReq(Role* user);
	int CrossJoinReq(int origin_uid, int origin_plat_type, int origin_server_id, int level, 
		int profession, long long avatar_timestamp, GameName name, short avatar_type, short headshot_id);

	int JoinReqRet(int uid, bool agree);

	bool IsLeader(Role* user);
	bool IsMember(Role* user);
	bool IsMember(int uid);

	int GetLeaderUID();
	const char* GetLeaderName();
	Role* GetLeaderRole();
	Role* GetMemberRoleByIndex(int member_index);
	Role* GetLastFollowRole();
	Robot* GetMemberRobotByIndex(int member_idx);
	TeamMember* GetMemberInfoByIndex(int member_index);
	TeamMember* GetMemberInfoByUID(int uid, ARG_OUT int* member_index = NULL);
	inline int GetMemberCount() { return m_cur_member_num; }
	int GetMemberCountExcludeRobot();
	int GetFollowMemberCount();
	inline int GetTeamType() { return m_team_type; }
	inline int GetCheckType() { return m_check_type; }
	inline int GetLimitLevelLow() const { return m_limit_level_low; }
	inline int GetLimitLevelHigh() const { return m_limit_level_high; }
	inline unsigned int GetTeamCreateTime() const { return m_team_create_time; }

	bool GetTeamInfo(int *team_index, short *limit_level_low, short *limit_level_high, short *team_type);
	TeamMember* GetMemberInfo(Role* role);
	TeamMember* GetMemberInfo(int index);
	int GetMemberIndex(Role* role);
	int GetMemberIndexByRoleId(int role_id);
	bool CanAutoJoin();
	bool CanApplyJoin();
	bool IsReachLevel(int role_level);
	void SendTeamInfo(Role* user = NULL);
	void SendTeamSimpleInfoChangeNotice(Role* user = NULL);
	void SendTeamMemberBaseInfoChangeNotice(Role* member, Role* send_to_role = NULL);
	inline int GetTeamIndex() const { return m_team_index; }
	int GetTeamSceneKey();
	unsigned int GetLastRecruitTimestamp() const { return m_last_recruit_timestamp; }
	void SetLastRecruitTimestamp(unsigned int timestamp) { m_last_recruit_timestamp = timestamp; }
	int GetAllTeamMemberGuildID();
	/**
	* @brief ��ȡ����Ķ���
	* @param scene ����
	* @param list ���ص�Role*����
	* @param max �������
	* @return ��ȡ�������� 0Ϊʧ��
	*/
	int GetMemberRoleListByFollow(Role** list, int max);

	void * operator new(size_t c);
	void operator delete(void *m);

	void SendToMemberNotice(int notice_num, bool notice_leader = true);
	void SendToMember(const void* info, int len, bool notie_leader = true);
	void SendToFollowMember(const void* info, int len, bool notie_leader = true);
	void SendStepOutInfoToMember(Role *user);

	bool HasDismissMember();
	bool HasRobotMember(ARG_OUT int* member_index);
	int GetRobotMemberNum();

	bool IsAllMemberFollow(bool is_notice = true);
	bool IsAllMemberEnoughLevel(int require_level, bool is_notice = true);
	bool IsAllMemberLevelLess(int require_level, bool is_notice = true);

	void OnUserStartEnterTeam(const UniqueUserID& uuid, bool is_from_origin_to_cross);

	void GetTeamItemInfo(TeamItem* out_ti);

	bool SwitchPosition(Role* leader, short pos1, short pos2);

	void OnMemberAppearanceChange(Role* role);

	void OnTeamCreate();
	void OnTeamChangeLeader();
	void OnMemberLeaveTeam();
	void FillPosList();
	//void FillOnePosWithPartner(int pos_index);
	void FillOnePosWithMember(int pos_index, int member_index);
	void ResetOnePosByUid(int uid);
	int GetEmptyPos();
	int GetOnePosForMember();
	void SendPosInfo(Role* leader_role);
	int GetFollowMemberMinLevel();
	int GetFollowMemberMaxLevel();
	int GetFollowMemberMaxLevelDiff();
	bool IsPartnerInTeam(int partner_id);
	void GetPosIdList(int* pos_id_list, int max_list_num, int is_need_partner);

	void OnLeaderCurrentFormationChange(Role* leader);
	void OnRoleRunAwayFromBattle(Role* role, bool is_leave_team);
	void OnRobotRunAwayFromBattle(Robot* robot, bool is_leave_team);

	void OnTeamStartMatch();
	void OnTeamStopMatch();

	bool TryFollowRole(Character* follower, Role* leader, bool is_from_login = false);
	void OnStopFollow(Character* follower);
	void OnAllStopFollow(Role* leader);
	void OnLeaderChangeUpdateFollow(Role* leader);
	void SendUpdateFollowListMsg(Character* character, Role* send_to_role = NULL);
	void UpdateMemberRolePointer(Role* member_role);

	void OnRoleLaunchLeaderVote(Role* member_role);
	void OnRoleVote(Role* member_role, bool is_yes);
	int GetAverageLevel();//��ȡ����ƽ���ȼ�

	void OnTeamLeaderLoadNPC(const char *msg);
	void OnTeamLeaderDeleteNPC(const char *msg);
	void OnTeamLeaderAddNPC(const char *msg);
	void OnSendTeamLeaderInfo(int info_type,Role *member_role = NULL,bool is_one = false,int npc_seq = -1, bool send_to_leader = false);
	
	bool IsRoleMemberFollow(int uid);
	
	void SendLeaderBattlePartnerInfo(Role* role);

	void OnRoleResetName(Role* role);

	//--------֪ͨ����������-------
	void SendLeaderIncompatibleNotice(int member_index, int notice_type, int param1 = 0, int param2 = 0);
	void SendAllMemberIncompatibleNotice(int member_index, int notice_type, int param1 = 0, int param2 = 0);
	bool CheckAllTeamMemberItem(int item_id, int num, bool check_dismiss_member = true);
	bool CheckAllTeamMemberTaskRecord(int task_id, bool check_dismiss_member = true);//��������¼�Ƿ��Ѿ�ͨ������
	bool ConsumeAllTeamMemberItem(int item_id, int num,const char* resone);
	void NoticeMemberLevelNotEnough(const char* name, int level);					//�㲥�����Ա�ȼ����㲻�ܲ�����ս

	void SaveFormationReq(Protocol::CSFormationSave* req);
	void OnTempGroupChat(const char* msg, int total_len);
	void TempGroupBecomeOfficialReq(Role* role, GameName group_name);
	void OnGetTempGroupDetailInfoReq(Role* role);

	//�ý�ս������
	void LeaderViewHuanJie(int is_open);
	void SendTeamInfoHuanJie();

	//�����������
	void SetMemberKillValue(int member_index, int value);
	bool TeamIsRedName() { return m_team_is_red_name; }
	bool TeamIsAllowRedNameJoin() { return m_team_allow_red_join; }

	//-----------------
	MsgRoleBaseInfo GetMsgRoleBaseInfo(int role_id);
	std::vector<MsgRoleBaseInfo> GetAllMsgRoleBaseInfo();

	void OnRoleEnterTeamKickRealRoleRobot(int uid);
	void OnTeamInfoChangedCheckRealRoleRobotLeave();
private:		
	bool RemoveApplyInfo(int role_id);
	void SendApplyListToLeader();
	void NoticeMemberLeave(const char* name);
	void NoticeAddMember(const char* name);
	void NoticeLeaderChange(const char* name);

	short		m_limit_level_low;
	short		m_limit_level_high;

	int			m_team_index;
	int			m_team_scene_key_inc;

	int			m_leader_index;
	int			m_cur_member_num;
	TeamMember	m_member_list[MAX_TEAM_MEMBER_NUM];

	int			m_team_type;					// ��������
	TeamNotice	m_team_notice;

	bool		m_auto_join;
	bool		m_is_cross;						// �Ƿ���

	int			m_check_type;					// �����������

	int			m_join_req_count;
	std::map<int, JoinReqInfo> m_join_req_map;		// �������

	//--------------------------����ǰ����ͬ������ֶ�--------------------------------------------
	enum ENTER_FB_DETERMINE_TYPE
	{
		ENTER_FB_DETERMINE_TYPE_PENDING,
		ENTER_FB_DETERMINE_TYPE_NO,
		ENTER_FB_DETERMINE_TYPE_YES,
	};

	int m_member_determine_flag[MAX_TEAM_MEMBER_NUM];		//��Ա���븱��ȷ����Ϣ
	unsigned int m_agree_enter_end_time;	
	int m_enter_type;
	int m_enter_type_param;
	int m_enter_param1;
	int m_enter_param2;
	int m_enter_param3;
	int m_enter_param[MAX_TEAM_MEMBER_NUM];					//�������Ͳ����������滻param3
	//-------------------------------------------


	unsigned int m_last_recruit_timestamp;
	unsigned int m_team_create_time;

	MemberPosInfo m_pos_list[MAX_TEAM_MEMBER_NUM];

	TeamVote m_team_vote;

	int leader_npc_seq_count;
	std::vector<int> leader_npc_seq_list;

	//------------------------------�������ط���ִ�в���-------------------------------
	bool m_is_notice;			//Ϊtrue����һЩƮ��
	int m_auto_join_type;		//CrossData_Business ����������
	int m_auto_join_type_param1; ////CrossData_Business ����������  ����m_auto_join_type�ж�m_auto_join_type_param1��ʲô����
	int m_auto_join_p1;			////CrossData_Business ����������
	std::map<int, int> m_need_wait_role_id_list; //��������������ʱ����¼Ҫ�ȴ���Ա���б�update������жϣ��˵��������Ӧ������������뵽��Ӧ�����
	unsigned int m_auto_join_time;			//��¼ʲôʱ�俪ʼ������һ��ʱ���Աû����Ͳ�������Ӧ����
	unsigned int m_notice_toggle_time;	//��ǰʱ�����m_notice_toggle_time��m_is_notice�л��෴	
	//------------------------------------------------------------------------------


	TemporaryChatGroup* m_temp_group;	// ��ʱ����Ⱥ��

	TeamHuanJieParam  m_huan_jie_param;

	bool m_team_allow_red_join;

	bool m_team_is_red_name;
	int m_member_kill_value[MAX_TEAM_MEMBER_NUM];
};

#endif // __TEAM_HPP__

