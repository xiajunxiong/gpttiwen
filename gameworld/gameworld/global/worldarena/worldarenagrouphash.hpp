#ifndef __WORLD_ARENA_GROUP_HASH_HPP__
#define __WORLD_ARENA_GROUP_HASH_HPP__

#include <map>
#include <vector>

//用于long long的group_id离散化成从0开始每次+1的int group_idx
class WorldArena;
class WorldArenaGroupHash
{
public:
	typedef std::map<long long, int> GroupIdToIdxHash;

public:
	WorldArenaGroupHash(WorldArena* wa);
	~WorldArenaGroupHash();

	void AllGroupIdHashToIdx(const std::vector<long long>& group_id_list, bool is_clear_old_hash = true);
	void OnSeasonRestart();
	int GetGroupIdx(long long group_id);
	int GetTotalGroupNum();
	long long GetGroupId(int group_idx);
	std::vector<long long> GetAllGroupIds();
private:
	WorldArena* m_arena_ptr;
	GroupIdToIdxHash m_hash;
};


#endif