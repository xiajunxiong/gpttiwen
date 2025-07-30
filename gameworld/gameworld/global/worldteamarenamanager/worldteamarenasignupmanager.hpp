#ifndef __WORLD_TEAM_ARENA_SIGN_UP_MANAGER_HPP__
#define __WORLD_TEAM_ARENA_SIGN_UP_MANAGER_HPP__

#include "servercommon/struct/global/worldteamarenaparam.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "protocol/msgworldteamarena.hpp"
#include <string>

class Role;
class Team;
class WorldTeamArenaSignUpManager
{
	friend class RMIInitWorldTeamArenaSignUpDataBackObjectImpl;

public:
	static WorldTeamArenaSignUpManager & Instance();

	void Update(unsigned int now_second);

	void OnServerStart();
	void OnServerStop();
	bool LoadWorldTeamArenaAllTeamData(long long id_from);		//读取所有报名战队
	bool IsLoadFinish() { return m_load_flag; }

	void OnUserLogin(Role *role);
	void OnUserLogout(Role* user);
	void OnUserLoginOriginGame(int role_id);		//隐藏服调用
	void OnLeaveTeam(Role* user, int team_index);

	void Init(const WorldTeamArenaSignUpParam & param);
	void GetInitParam(WorldTeamArenaSignUpParam * param);
	void ClearDirtyMark(WorldTeamArenaSignUpParam *param);

	long long GetTeamUniqueId(int role_id);
	const WorldTeamArenaTeamSignUpData * GetWorldTeamArenaSignUpDataById(int role_id);
	const WorldTeamArenaTeamSignUpData * GetWorldTeamArenaSignUpDataById(long long team_unique_id);
	bool IsSameTeamByTeam(Team * team, bool is_notice = true);

	void OnTeamSignUp(Role * role, GameName team_name);				//战队报名
	void OnTeamMemberSignUp(Role * role);							//成员确认报名
	void OnCancelTeamSignUp(Role * role);							//取消战队报名(此处是在队员还没全部完成确认方可取消)

	void OnRoleInfoChange(Role * role);								//角色信息变化同步

	void OnQueryTeamDetailedInfo(Role * role, long long team_unique_id, int type);	//查询战队详细信息
	void OnQueryTeamDetailedInfo(Role * role, GameName name);				//查询战队详细信息 (不支持模糊信息)

	void GetQueryTeamUidList(long long team_unique_id, std::set<int> & team_user_id_list);	//获取该战队的角色成员ID列表		
	void GetQueryTeamLevelList(long long team_unique_id, std::set<std::pair<int, int> > & team_user_Level_list);	//获取该战队的角色成员等级		
	void GetTeamNameById(long long team_unique_id, GameName & team_name);

	//游戏服/隐藏服收到跨服查询的排名信息(战队详细信息)
	void OnGameRecvCrossQueryRankRet(crossgameprotocal::CrossGameWorldTeamArenaQueryRankRet * msg);
	//游戏服/隐藏服收到跨服查询的排名信息(队伍特殊信息)
	void OnGameRecvCrossQueryRankRetSendSpecialInfo(crossgameprotocal::CrossGameWorldTeamArenaQueryRankRet * msg);
	//隐藏服收到原服同步的角色信息
	void OnHiddenRecvGameSynRoleInfo(crossgameprotocal::GameHiddenRoleInfoChangeWorldTeamArenaSyn * msg);
	

	void SendAllSignUpInfo(Role * role, long long team_unqiue_id = 0);
	void SendNoticeInfo(Team * team, int notice_type, int param1, int param2);
	void OnSendTeamSpecialInfo(Role * role);
	void SendOtherInfo(Role * role);
	void SendOtherInfoToGame(int role_id);		//隐藏服调用发送回玩家原服

	//Gm命令
	void OnGmAllTeamSignUp(Role * role);		//全部队员确认报名

	//活动逻辑
	void OnActivityEnd();

	void FillMemberInfo(Role * role, WorldTeamArenaUserInfo & member_info);

	void OnCmdRankToSignUpList(std::map<int, std::vector<WorldTeamArenaTeamRankData> > & rank_map);

private:
	WorldTeamArenaSignUpManager();
	~WorldTeamArenaSignUpManager();

	void Save();
	void LoadWorldTeamArenaAllTeamSucc();
	bool IsDirty() { return m_change_flag; }

	//查询战队详细信息
	void QueryTeamDetailedInfo(Role * role, const WorldTeamArenaTeamSignUpData & team_info, int send_type = 0, int notice_type = Protocol::SCWorldTeamArenaTeamSignUpDetailedInfo::NOTICE_TYPE_SIGN_UP);	

	void UpdateRankList();
	//检测队员与战队名称是否符合
	bool OnCheckCanPreSignUp(Role * role, Team * team, GameName team_name, PreTeamSignUpInfo & team_info);
	//检测是否全员确认
	bool OnCheckPreAllDet(PreTeamSignUpInfo & team_info);
	void OnInsterSimple(const WorldTeamArenaTeamSignUpData & team_data);
	void OnEraseSimple(long long team_unique_id);


	std::vector<PreTeamSignUpInfo> m_pre_team_sign_up_list;						//预备报名队伍列表

	std::vector<WorldTeamArenaTeamSignUpSimpleInfo> m_rank_list;		
	std::map<int, long long> m_role_team_map;									//角色战队映射 key:role_id  second:team_unique_id
	std::map<long long, WorldTeamArenaTeamSignUpData> m_team_sign_up_map;		//报名战队信息 key:team_unique_id
	std::map<long long, bool> m_dirty_mark;
	std::map<long long, bool> m_old_state;

	bool m_change_flag;
	bool m_load_flag;
	unsigned int m_next_save_db_timestamp;								// 下次保存到数据库的时间戳
};



#endif