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
	bool LoadWorldTeamArenaAllTeamData(long long id_from);		//��ȡ���б���ս��
	bool IsLoadFinish() { return m_load_flag; }

	void OnUserLogin(Role *role);
	void OnUserLogout(Role* user);
	void OnUserLoginOriginGame(int role_id);		//���ط�����
	void OnLeaveTeam(Role* user, int team_index);

	void Init(const WorldTeamArenaSignUpParam & param);
	void GetInitParam(WorldTeamArenaSignUpParam * param);
	void ClearDirtyMark(WorldTeamArenaSignUpParam *param);

	long long GetTeamUniqueId(int role_id);
	const WorldTeamArenaTeamSignUpData * GetWorldTeamArenaSignUpDataById(int role_id);
	const WorldTeamArenaTeamSignUpData * GetWorldTeamArenaSignUpDataById(long long team_unique_id);
	bool IsSameTeamByTeam(Team * team, bool is_notice = true);

	void OnTeamSignUp(Role * role, GameName team_name);				//ս�ӱ���
	void OnTeamMemberSignUp(Role * role);							//��Աȷ�ϱ���
	void OnCancelTeamSignUp(Role * role);							//ȡ��ս�ӱ���(�˴����ڶ�Ա��ûȫ�����ȷ�Ϸ���ȡ��)

	void OnRoleInfoChange(Role * role);								//��ɫ��Ϣ�仯ͬ��

	void OnQueryTeamDetailedInfo(Role * role, long long team_unique_id, int type);	//��ѯս����ϸ��Ϣ
	void OnQueryTeamDetailedInfo(Role * role, GameName name);				//��ѯս����ϸ��Ϣ (��֧��ģ����Ϣ)

	void GetQueryTeamUidList(long long team_unique_id, std::set<int> & team_user_id_list);	//��ȡ��ս�ӵĽ�ɫ��ԱID�б�		
	void GetQueryTeamLevelList(long long team_unique_id, std::set<std::pair<int, int> > & team_user_Level_list);	//��ȡ��ս�ӵĽ�ɫ��Ա�ȼ�		
	void GetTeamNameById(long long team_unique_id, GameName & team_name);

	//��Ϸ��/���ط��յ������ѯ��������Ϣ(ս����ϸ��Ϣ)
	void OnGameRecvCrossQueryRankRet(crossgameprotocal::CrossGameWorldTeamArenaQueryRankRet * msg);
	//��Ϸ��/���ط��յ������ѯ��������Ϣ(����������Ϣ)
	void OnGameRecvCrossQueryRankRetSendSpecialInfo(crossgameprotocal::CrossGameWorldTeamArenaQueryRankRet * msg);
	//���ط��յ�ԭ��ͬ���Ľ�ɫ��Ϣ
	void OnHiddenRecvGameSynRoleInfo(crossgameprotocal::GameHiddenRoleInfoChangeWorldTeamArenaSyn * msg);
	

	void SendAllSignUpInfo(Role * role, long long team_unqiue_id = 0);
	void SendNoticeInfo(Team * team, int notice_type, int param1, int param2);
	void OnSendTeamSpecialInfo(Role * role);
	void SendOtherInfo(Role * role);
	void SendOtherInfoToGame(int role_id);		//���ط����÷��ͻ����ԭ��

	//Gm����
	void OnGmAllTeamSignUp(Role * role);		//ȫ����Աȷ�ϱ���

	//��߼�
	void OnActivityEnd();

	void FillMemberInfo(Role * role, WorldTeamArenaUserInfo & member_info);

	void OnCmdRankToSignUpList(std::map<int, std::vector<WorldTeamArenaTeamRankData> > & rank_map);

private:
	WorldTeamArenaSignUpManager();
	~WorldTeamArenaSignUpManager();

	void Save();
	void LoadWorldTeamArenaAllTeamSucc();
	bool IsDirty() { return m_change_flag; }

	//��ѯս����ϸ��Ϣ
	void QueryTeamDetailedInfo(Role * role, const WorldTeamArenaTeamSignUpData & team_info, int send_type = 0, int notice_type = Protocol::SCWorldTeamArenaTeamSignUpDetailedInfo::NOTICE_TYPE_SIGN_UP);	

	void UpdateRankList();
	//����Ա��ս�������Ƿ����
	bool OnCheckCanPreSignUp(Role * role, Team * team, GameName team_name, PreTeamSignUpInfo & team_info);
	//����Ƿ�ȫԱȷ��
	bool OnCheckPreAllDet(PreTeamSignUpInfo & team_info);
	void OnInsterSimple(const WorldTeamArenaTeamSignUpData & team_data);
	void OnEraseSimple(long long team_unique_id);


	std::vector<PreTeamSignUpInfo> m_pre_team_sign_up_list;						//Ԥ�����������б�

	std::vector<WorldTeamArenaTeamSignUpSimpleInfo> m_rank_list;		
	std::map<int, long long> m_role_team_map;									//��ɫս��ӳ�� key:role_id  second:team_unique_id
	std::map<long long, WorldTeamArenaTeamSignUpData> m_team_sign_up_map;		//����ս����Ϣ key:team_unique_id
	std::map<long long, bool> m_dirty_mark;
	std::map<long long, bool> m_old_state;

	bool m_change_flag;
	bool m_load_flag;
	unsigned int m_next_save_db_timestamp;								// �´α��浽���ݿ��ʱ���
};



#endif