#ifndef __RANK_MANAGER_DEF_HPP__
#define __RANK_MANAGER_DEF_HPP__

#include "personrank.hpp"
#include "teamrank.hpp"
#include "guildrank.hpp"
#include "petrank.hpp"
#include "protocol/msgcsrank.h"
#include "partnerrank.hpp"
#include "global/rank/pettoprankshadow.hpp"
#include "global/rank/partnertoprankshadow.hpp"

namespace crossgameprotocal
{
	struct GameHiddenSyncPetTopRankInfo;
	struct GameHiddenSyncPartnerTopRankInfo;
}

class RankManager
{
	friend class RMIInitPersonRankBackObjectImpl;
	friend class RMIInitTeamRankBackObjectImpl;
	friend class RMIInitPetRankBackObjectImpl;
	friend class RMIInitPartnerRankBackObjectImpl;

public:
	static RankManager & Instance();

	bool IsLoadFinish();

	PersonRank * GetPersonRank() { return &m_person_rank; }
	TeamRank * GetTeamRank() { return &m_team_rank; }
	GuildRank * GetGuildRank() { return &m_guild_rank; }
	PetRank *GetPetRank() { return &m_pet_rank; }
	PartnerRank *GetPartnerRank() { return &m_partner_rank; }
	const PetTopRankShadow* GetPetTopRankShadow() const { return &m_pet_top_rank_shadow; }
	const PartnerTopRankShadow* GetPartnerTopRankShadow() const { return &m_partner_top_rank_shadow; }

	void OnServerStart();
	void OnServerStop();

	void Update(unsigned long interval, time_t now_second);
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnWeekChange();

	void OnUserLogin(Role *user);

	void OnGetPersonRankList(Role *user, Protocol::CSGetPersonRankList *rank_list_req);
	void OnHiddenGameGetRankList(crossgameprotocal::HiddenGameGetRankListReq * req);

	void OnHiddenGameGetRankFriendList(crossgameprotocal::HiddenGameGetFriendRankListReq *req);

	void OnGetPetRankList(Role *role, int rank_type, int from_index, int to_index);
	void OnGetPetRankDetailedInfo(Role *role, int target_uid, int rank_type, unsigned long long pet_unique_item_id);
	void OnGetPersonRankTopUser(Role *user, Protocol::CSGetPersonRankTopUser *top_user_req);
	void SyncPersonBaseInfo(Role *user, int old_prof, int cur_prof);
	void OnRoleResetName(Role * _role);
	void SyncPersonRankInfo(Role * _role, int _rank_type);
	void OnTopLevelResetSyn(UserCacheNode *node);

	void OnGetPersonRankUidList(int rank_type, int count, std::vector<int> & rank_uid_list);

	void SyncPetRankInfo(Role * _role, int _rank_type, int _index , bool _is_force = false);
	void SyncPartnerRankInfo(Role * _role, int _rank_type, int partner_id, bool _is_force = false);

	void SendPartnerTopRankInfo(int user_uid);
	void SendPetTopRankInfo(int user_uid);

	void SyncRemovePet(Role *role, unsigned long long pet_unique_id);

	void OnGetGuildRankList(Role *user, Protocol::CSGetGuildRankList *rank_list_req);
	void SendRefreshTimestamp(Role * role);

	void OnGetTeamRankList(Role *user, Protocol::CSGetTeamRankList *rank_list_req);
	void SyncTeamRankInfo(gglobalprotocal::SyncTeamRankInfo *team_rank_info);

	void SyncGuildRankInfo(Guild * _guild , int _rank_type);

	void ClearPersonRankType(int rank_type);
	void ClearGuildRankType(int rank_type);
	void ClearTeamRankType(int rank_type);
	void ClearPetRankType(int rank_type);
	void ClearPartnerRankType(int rank_type);

	void OnConnectCrossSucc();

	void OnHiddenGameRoleInfoChange(crossgameprotocal::HiddenGameRoleInfoChange * _msg);
	void OnHiddenGamePetInfoChange(crossgameprotocal::HiddenGamePetInfoChange * _msg);

	// 隐藏服收到游戏服同步的等级助力相关信息
	void OnHiddenRecvGameSynLevelComplementInfo(crossgameprotocal::GameHiddenSynLevelComplementInfo * msg);

	int GetAverageByLevelComplement(int server_id);

	void LoadGuildRankSucc();
	void SyncPetTopRankFromOrigin(crossgameprotocal::GameHiddenSyncPetTopRankInfo* msg);
	void SyncPartnerTopRankFromOrigin(crossgameprotocal::GameHiddenSyncPartnerTopRankInfo* msg);
private:
	RankManager();
	~RankManager();

	void LoadPersonRankSucc();
	void LoadTeamRankSucc();
	void LoadPetRankSucc();
	void LoadPartnerRankSucc();
	void LoadAllRankSucc();
	void SetRankShapshotTimestamp();

	void Save();

	PersonRank m_person_rank;
	TeamRank m_team_rank;
	GuildRank m_guild_rank;
	PetRank m_pet_rank;
	PartnerRank m_partner_rank;
	PetTopRankShadow m_pet_top_rank_shadow;
	PartnerTopRankShadow m_partner_top_rank_shadow;
	
	time_t m_rank_data_next_save_time;
	unsigned int m_rank_shapshot_timestamp;
};

#endif  // __RANK_MANAGER_DEF_HPP__