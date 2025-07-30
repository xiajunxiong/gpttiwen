#ifndef __PARTNER_TOP_RANK_SHADOW_HPP__
#define __PARTNER_TOP_RANK_SHADOW_HPP__

static const int PARTNER_TOP_RANK_NUM = 10;
static const int PARTNER_TOP_RANK_SYNC_INTERNVAL_S = 3;

#include <map>

struct PartnerTopRankInfo
{
	PartnerTopRankInfo() : owner_uid(0), partner_id(0) {}

	int owner_uid;
	int partner_id;
};

struct OriginServerPartnerTopRankShadow
{
	int top_rank_num;
	PartnerTopRankInfo top_list[PARTNER_TOP_RANK_NUM];
};

namespace crossgameprotocal
{
	struct GameHiddenSyncPartnerTopRankInfo;
}

struct PartnerRankItem;
class PartnerTopRankShadow
{
public:
	typedef std::map<int, OriginServerPartnerTopRankShadow> OriginRankShadowMap;

public:
	PartnerTopRankShadow();
	~PartnerTopRankShadow();

	int GetPartnerTopRank(int user_uid, int pet_id) const;
	
	void SendPartnerAllTopRankInfo(int user_uid);

	void Update(unsigned int now_timestmap);
	void OnRankManagerLoadSucc();
	void SyncPartnerTopRankFromOrigin(crossgameprotocal::GameHiddenSyncPartnerTopRankInfo* msg);
private:
	void SyncPartnerTopRank();
	bool CheckTopRankChanged(int rank_num, const PartnerRankItem** rank_list);
	void SetTopRankList(int rank_num, const PartnerRankItem** rank_list);
	void SyncPartnerTopRankToHidden();


	int GetPartnerTopRank(int owner_uid, int partner_id, int list_num, const PartnerTopRankInfo* rank_list) const;

	unsigned int m_next_sync_top_rank_timestamp;

	int m_top_rank_num;
	PartnerTopRankInfo m_top_list[PARTNER_TOP_RANK_NUM];

	OriginRankShadowMap m_origin_rank_shadow_map;
};

#endif