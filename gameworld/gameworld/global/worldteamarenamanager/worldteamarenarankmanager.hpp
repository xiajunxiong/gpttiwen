#ifndef __WORLD_TEAM_ARENA_RANK_MANAGER_HPP__
#define __WORLD_TEAM_ARENA_RANK_MANAGER_HPP__

#include "servercommon/struct/global/worldteamarenaparam.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "protocol/msgworldteamarena.hpp"

class Role;
class WorldTeamArenaRankManager
{
	friend class RMIInitWorldTeamArenaTeamRankDataBackObjectImpl;

public:
	static WorldTeamArenaRankManager & Instance();

	void Update(unsigned int now_second);

	void OnServerStart();
	void OnServerStop();
	bool LoadWorldTeamArenaTeamRankData(long long id_from);		
	bool IsLoadFinish() { return m_load_flag; }

	void Init(const WorldTeamArenaTeamRankParam & param);
	void GetInitParam(WorldTeamArenaTeamRankParam * param);
	void ClearDirtyMark(WorldTeamArenaTeamRankParam *param);

	void OnRoleInfoChange(Role * role);					//��ɫ��Ϣ�仯ͬ��

	void OnQueryTeamDetailedInfo(Role * role, long long team_unique_id);	//��ѯս����ϸ��Ϣ

	//��Ϸ��/���ط��յ������ѯ��������Ϣ
	void OnGameRecvCrossQueryRankRet(crossgameprotocal::CrossGameWorldTeamArenaQueryRankRet * msg);
	//���ط��յ�ԭ��ͬ���Ľ�ɫ��Ϣ
	void OnHiddenRecvGameSynRoleInfo(crossgameprotocal::GameHiddenRoleInfoChangeWorldTeamArenaSyn * msg);

	//��߼�
	void OnActivityStandy();		
	void OnActivityEnd(std::vector<WorldTeamArenaRankItem> rank_list);

	void OnRoleTitleInfoChange(int role_id, const char * reason, int team_rank, int title_id);	
	
	//�·����հ���Ϣ
	void OnSendAllRankInfo(Role * role);
	//���ط��յ�ԭ��ͬ���Ľ�ɫ��Ϣ
	void OnHiddenRecvGameFinallyRankReq(crossgameprotocal::GameHiddenWorldTeamArenaFinallyRankReq * msg);

	//��ȡӵ�����µ�һ�������ʸ��ս��(key:hidden_server_idx)
	void GetTopRankTeam(std::map<int, WorldTeamArenaServerTopTeamInfo> & top_team_list);

	void CmdRankToSignUpList(int cur_season, unsigned int sign_up_end_timestamp);

private:
	WorldTeamArenaRankManager();
	~WorldTeamArenaRankManager();

	void FillAllFinallyRankInfo(Protocol::SCActivityWorldTeamArenaFinallyRankInfo & all_rank_info, int hidden_server_idx, int role_id);
	void UpdataRankList();
	void Save(const WorldTeamArenaTeamRankParam * param = NULL);
	void LoadWorldTeamArenaTeamRankSucc();
	bool IsDirty() { return m_change_flag; }

	std::map<long long, WorldTeamArenaTeamRankData> m_rank_param;				//ս��������Ϣ
	std::map<int, std::vector<WorldTeamArenaTeamRankData> > m_rank_list;		//�����б�(�Ϸ�������ж��ǰʮ����) key:hidden_server_idx 
	std::map<long long, char> m_dirty_mark;
	std::map<long long, char> m_old_state;

	bool m_change_flag;
	bool m_load_flag;
	unsigned int m_next_save_db_timestamp;								// �´α��浽���ݿ��ʱ���
};


#endif