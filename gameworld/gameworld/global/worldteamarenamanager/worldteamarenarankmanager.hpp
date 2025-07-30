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

	void OnRoleInfoChange(Role * role);					//角色信息变化同步

	void OnQueryTeamDetailedInfo(Role * role, long long team_unique_id);	//查询战队详细信息

	//游戏服/隐藏服收到跨服查询的排名信息
	void OnGameRecvCrossQueryRankRet(crossgameprotocal::CrossGameWorldTeamArenaQueryRankRet * msg);
	//隐藏服收到原服同步的角色信息
	void OnHiddenRecvGameSynRoleInfo(crossgameprotocal::GameHiddenRoleInfoChangeWorldTeamArenaSyn * msg);

	//活动逻辑
	void OnActivityStandy();		
	void OnActivityEnd(std::vector<WorldTeamArenaRankItem> rank_list);

	void OnRoleTitleInfoChange(int role_id, const char * reason, int team_rank, int title_id);	
	
	//下发最终榜单信息
	void OnSendAllRankInfo(Role * role);
	//隐藏服收到原服同步的角色信息
	void OnHiddenRecvGameFinallyRankReq(crossgameprotocal::GameHiddenWorldTeamArenaFinallyRankReq * msg);

	//获取拥有天下第一比武大会资格的战队(key:hidden_server_idx)
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

	std::map<long long, WorldTeamArenaTeamRankData> m_rank_param;				//战队排名信息
	std::map<int, std::vector<WorldTeamArenaTeamRankData> > m_rank_list;		//排名列表(合服后可能有多个前十队伍) key:hidden_server_idx 
	std::map<long long, char> m_dirty_mark;
	std::map<long long, char> m_old_state;

	bool m_change_flag;
	bool m_load_flag;
	unsigned int m_next_save_db_timestamp;								// 下次保存到数据库的时间戳
};


#endif