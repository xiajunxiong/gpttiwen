#ifndef __PET_TOP_RANK_SHADOW_HPP__
#define __PET_TOP_RANK_SHADOW_HPP__

static const int PET_TOP_RANK_NUM = 10;
static const int PET_TOP_RANK_SYNC_INTERNVAL_S = 3;

#include <map>

struct PetTopRankInfo
{
	PetTopRankInfo() : owner_uid(0), pet_id(0), pet_unique_id(0) {}

	int owner_uid;
	int pet_id;
	long long pet_unique_id;
};

struct OriginServerPetTopRankShadow
{
	int top_rank_num;
	PetTopRankInfo top_list[PET_TOP_RANK_NUM];
};

namespace crossgameprotocal
{
	struct GameHiddenSyncPetTopRankInfo;
}

struct PetRankItem;
class PetTopRankShadow
{
public:
	typedef std::map<int, OriginServerPetTopRankShadow> OriginRankShadowMap;

public:
	PetTopRankShadow();
	~PetTopRankShadow();

	int GetPetTopRank(int owner_uid, int pet_id, long long pet_unique_id) const;

	void SendPetAllTopRankInfo(int user_uid);

	void Update(unsigned int now_timestmap);
	void OnRankManagerLoadSucc();
	void SyncPetTopRankFromOrigin(crossgameprotocal::GameHiddenSyncPetTopRankInfo* msg);
private:
	void SyncPetTopRank();
	bool CheckTopRankChanged(int rank_num, const PetRankItem** rank_list);
	void SetTopRankList(int rank_num, const PetRankItem** rank_list);
	void SyncPetTopRankToHidden();

	int GetPetTopRank(int owner_uid, int pet_id, long long pet_unique_id, int list_num, const PetTopRankInfo* rank_list) const;

	unsigned int m_next_sync_top_rank_timestamp;

	int m_top_rank_num;
	PetTopRankInfo m_top_list[PET_TOP_RANK_NUM];

	OriginRankShadowMap m_origin_rank_shadow_map;
};

#endif