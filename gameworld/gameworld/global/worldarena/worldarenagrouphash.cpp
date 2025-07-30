#include "worldarenagrouphash.hpp"
#include "servercommon/servercommon.h"

WorldArenaGroupHash::WorldArenaGroupHash(WorldArena* wa) : m_arena_ptr(wa)
{

}

WorldArenaGroupHash::~WorldArenaGroupHash()
{

}

void WorldArenaGroupHash::AllGroupIdHashToIdx(const std::vector<long long>& group_id_list, bool is_clear_old_hash /* = true */)
{
	if (is_clear_old_hash)
	{
		m_hash.clear();
	}	

	for (size_t i = 0; i < group_id_list.size(); ++i)
	{
		long long group_id = group_id_list[i];
		int group_idx = static_cast<int>(m_hash.size());
		m_hash.insert(GroupIdToIdxHash::value_type(group_id, group_idx));
	}
}

void WorldArenaGroupHash::OnSeasonRestart()
{
	m_hash.clear();
}

int WorldArenaGroupHash::GetGroupIdx(long long group_id)
{
	GroupIdToIdxHash::iterator it = m_hash.find(group_id);
	if (it != m_hash.end())
	{
		return it->second;
	}

	return -1;
}

int WorldArenaGroupHash::GetTotalGroupNum()
{
	return static_cast<int>(m_hash.size());
}

long long WorldArenaGroupHash::GetGroupId(int group_idx)
{
	GroupIdToIdxHash::iterator it = m_hash.begin();
	for (; it != m_hash.end(); ++it)
	{
		if (group_idx == it->second)
		{
			return it->first;
		}
	}

	return 0;
}

std::vector<long long> WorldArenaGroupHash::GetAllGroupIds()
{
	std::vector<long long> all_groups;
	all_groups.reserve(m_hash.size());
	GroupIdToIdxHash::iterator it = m_hash.begin();
	for (; it != m_hash.end(); ++it)
	{
		all_groups.push_back(it->first);
	}

	return all_groups;
}

