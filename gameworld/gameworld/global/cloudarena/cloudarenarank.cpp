#include "cloudarenarank.hpp"
#include "servercommon/cloudarenadef.hpp"

CloudArenaRank::CloudArenaRank()
{
	m_rank_list.SetDefaultMaxSize(CLOUD_ARENA_RANK_LIST_SIZE);
	m_rank_list.SetCompare(&CloudArenaRankItem::RankCompare);
}

CloudArenaRank::~CloudArenaRank()
{
	m_rank_list.Clear();
}

void CloudArenaRank::UpdateRankItem(const CloudArenaRankItem& rank_item)
{
	IndexMap::iterator it = m_index_map.find(rank_item.uid);
	if (it == m_index_map.end())
	{
		// 没有就插入
		m_rank_list.Insert(rank_item);

		CloudArenaRankIndex index;
		index.uid = rank_item.uid;
		index.score = rank_item.score;
		index.reach_score_timestamp = rank_item.reach_score_timestamp;
		index.last_season_rank = rank_item.last_season_rank;
		m_index_map.insert(IndexMap::value_type(index.uid, index));
	}
	else
	{
		CloudArenaRankIndex& index = it->second;
		CloudArenaRankItem check_item;
		this->ConstructRankItem(&check_item, index);

		m_rank_list.UpdateData(check_item, rank_item);

		index.uid = rank_item.uid;
		index.score = rank_item.score;
		index.reach_score_timestamp = rank_item.reach_score_timestamp;
		index.last_season_rank = rank_item.last_season_rank;
	}
}

void CloudArenaRank::UpdateScore(int uid, int score, unsigned int reach_score_timestamp, int last_season_rank)
{
	IndexMap::iterator it = m_index_map.find(uid);
	if (it == m_index_map.end())
	{
		return;
	}
	CloudArenaRankIndex& index = it->second;
	CloudArenaRankItem rank_item;
	this->ConstructRankItem(&rank_item, index);

	RankList::Iterator rank_it = m_rank_list.Find(rank_item);
	if (rank_it != m_rank_list.End())
	{
		rank_item = *rank_it;
		rank_item.score = score;
		rank_item.reach_score_timestamp = reach_score_timestamp;
		rank_item.last_season_rank = last_season_rank;

		m_rank_list.UpdateData(*rank_it, rank_item);

		index.score = score;
		index.reach_score_timestamp = reach_score_timestamp;
		index.last_season_rank = last_season_rank;
	}
}

void CloudArenaRank::GetTopRankItems(int max_num, ARG_OUT int* num, ARG_OUT const CloudArenaRankItem** rank_list)
{
	for (int i = 0; i < static_cast<int>(m_rank_list.Size()) && *num < max_num; ++i)
	{
		RankList::Iterator it = m_rank_list[i];
		if (it == m_rank_list.End()) break;

		rank_list[(*num)++] = &*it;
	}
}

const CloudArenaRankItem* CloudArenaRank::FindUserRankItem(int uid, ARG_OUT int* rank)
{
	IndexMap::iterator it = m_index_map.find(uid);
	if (it == m_index_map.end())
	{
		return NULL;
	}

	CloudArenaRankIndex& index = it->second;
	CloudArenaRankItem rank_item;
	this->ConstructRankItem(&rank_item, index);

	int rank_index = m_rank_list.GetIndex(rank_item);
	if (-1 == rank_index) return NULL;

	RankList::Iterator rank_it = m_rank_list[rank_index];
	if (rank_it == m_rank_list.End()) return NULL;

	if (NULL != rank) *rank = rank_index;

	return &*rank_it;
}

void CloudArenaRank::RemoveFromRankList(int uid)
{
	IndexMap::iterator it = m_index_map.find(uid);
	if (it == m_index_map.end())
	{
		return;
	}

	CloudArenaRankIndex& index = it->second;
	CloudArenaRankItem rank_item;
	this->ConstructRankItem(&rank_item, index);

	m_rank_list.Erase(rank_item);
	m_index_map.erase(uid);
}

void CloudArenaRank::OnStartNewSeason()
{
	m_index_map.clear();
	m_rank_list.Clear();
}

void CloudArenaRank::ConstructRankItem(ARG_OUT CloudArenaRankItem* rank_item, const CloudArenaRankIndex& index)
{
	rank_item->uid = index.uid;
	rank_item->score = index.score;
	rank_item->reach_score_timestamp = index.reach_score_timestamp;
	rank_item->last_season_rank = index.last_season_rank;
}

