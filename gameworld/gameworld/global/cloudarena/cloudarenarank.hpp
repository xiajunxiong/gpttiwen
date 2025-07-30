#ifndef __CLOUD_ARENA_RANK_HPP__
#define __CLOUD_ARENA_RANK_HPP__

#include "servercommon/skiplist.hpp"
#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#include <map>

struct CloudArenaRankItem
{
	CloudArenaRankItem() : uid(0), level(0), profession(0), avatar(0), headshot_id(0), score(0), 
		reach_score_timestamp(0u), last_season_rank(-1)
	{
		memset(name, 0, sizeof(name));
	}

	static bool RankCompare(const CloudArenaRankItem & _a, const CloudArenaRankItem & _b)
	{
		if (_a == _b) 
		{
			return true;
		}

		if (_a.score != _b.score)
		{
			return _a.score > _b.score;
		}
		else if (_a.reach_score_timestamp != _b.reach_score_timestamp)
		{
			return _a.reach_score_timestamp < _b.reach_score_timestamp;
		}
		else if (_a.last_season_rank != _b.last_season_rank)
		{
			if (-1 == _a.last_season_rank) return false;
			if (-1 == _b.last_season_rank) return true;
			
			return _a.last_season_rank < _b.last_season_rank;
		}
		else
		{
			return _a.uid <= _b.uid;
		}
	}

	bool operator==(const CloudArenaRankItem & other) const
	{
		if (uid == other.uid && score == other.score && reach_score_timestamp == other.reach_score_timestamp && last_season_rank == other.last_season_rank)
		{
			return true;
		}
		
		return false;
	}

	bool operator< (const CloudArenaRankItem & other) const
	{
		return RankCompare(*this, other);
	}

	bool operator<= (const CloudArenaRankItem& other) const
	{
		return *this < other || *this == other;
	}

	int uid;
	int level;
	int profession;
	int avatar;
	int headshot_id;
	GameName name;
	int score;
	unsigned int reach_score_timestamp;
	int last_season_rank;
};

struct CloudArenaRankIndex
{
	CloudArenaRankIndex() : uid(0), score(0), reach_score_timestamp(0), last_season_rank(0) {}

	int uid;
	int score;
	unsigned int reach_score_timestamp;
	int last_season_rank;
};

class CloudArenaRank
{
public:
	typedef std::map<int, CloudArenaRankIndex> IndexMap;
	typedef SkipList<CloudArenaRankItem> RankList;

public:
	CloudArenaRank();
	~CloudArenaRank();

	void UpdateRankItem(const CloudArenaRankItem& rank_item);
	void UpdateScore(int uid, int score, unsigned int reach_score_timestamp, int last_season_rank);
	void GetTopRankItems(int max_num, ARG_OUT int* num, ARG_OUT const CloudArenaRankItem** rank_list);
	const CloudArenaRankItem* FindUserRankItem(int uid, ARG_OUT int* rank);
	void RemoveFromRankList(int uid);
	void OnStartNewSeason();
private:
	void ConstructRankItem(ARG_OUT CloudArenaRankItem* rank_item, const CloudArenaRankIndex& index);

	RankList m_rank_list;
	IndexMap m_index_map;
};

#endif