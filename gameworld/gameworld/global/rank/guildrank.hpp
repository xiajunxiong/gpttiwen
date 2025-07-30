#ifndef __GUILD_RANK_DEF_HPP__
#define __GUILD_RANK_DEF_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/rankdef.hpp"
#include "servercommon/guilddef.hpp"
#include "servercommon/internalprotocal/gglobalprotocal.h"
#include "servercommon/skiplist.hpp"

#include <map>

namespace Protocol
{
	class CSGetGuildRankList;
}

class RankManager;
class Guild;
class Role;

class GuildRank
{
	struct IndexData
	{
		IndexData() { this->Reset(); }

		void Reset()
		{
			guild_id = 0;
			rank_type = -1;
			rank_value = 0;
		}

		bool operator< (const IndexData & _a)
		{
			if (rank_value == _a.rank_value)
			{
				return guild_id < _a.guild_id;
			}
			return rank_value > _a.rank_value;
		}

		UniqueServerID server_id;
		int guild_id;
		int rank_type;
		long long rank_value;
	};
public:
	GuildRank();
	~GuildRank();

	void SetRankManager(RankManager *rank_manager) { m_rank_manager = rank_manager; }

	void Update(time_t now_second);
	void OnWeekChange();

	void OnGetRankList(Role *role, Protocol::CSGetGuildRankList &rank_list_req);

	void OnBaseInfoChange(Guild *guild);

	bool IsRankLoadFinish();
	void OnGameworldAccept();
	void ClearRank(int rank_type);

	// 是否处于排行榜前X名
	bool IsGuildInTopRank(GuildID guild_id, int rank_type, int max_rank);

	void OnGuildRankInfoChange(Guild * _guild, int _rank_type);

	// 解散公会
	void RemoveGuild(int guild_id);

	void InitGuildRank();

private:
	// 整点日志
	void OnRankLog(time_t _now_second);
	// 初始化m_guild_index_map
	void InitGuildIndex(int _rank_type);
	// 同步数据到跨服
	void SyncRankAllDataToCross(int _rank_type);																// 整表
	void SyncRankDataToCross(int _rank_type, int _sync_status, int _old_index, GuildRankItem & _rank_item);		// 单公会

	// 索引
	bool AddGuildIndex(int _rank_type, GuildRankItem & _check_item);
	bool RemoveGuildIndex(int _rank_type,int _guild_id);
	void ConstructionCheckItem(GuildRankItem & _check_item, const IndexData & _index_data);
	void ConstructionIndexData(IndexData & _index_data, const GuildRankItem & _check_item);

	long long CalcGuildRankValue(Guild *guild, int rank_type, bool *rank_value_valid = NULL);

	void UpdateBaseInfo(GuildRankItem &rank_item, Guild *guild);
	void InitGuildRank(int rank_type);

	void UpdateRankList(GuildRankItem &_updata_rank_item);

	RankManager *m_rank_manager;

	SkipList<GuildRankItem>  m_rank_list[GUILD_RANK_TYPE_MAX];

	typedef std::map<int, IndexData> GuildIndexMap;
	typedef std::map<int, IndexData>::iterator GuildIndexMapIt;

	GuildIndexMap m_guild_index_map[GUILD_RANK_TYPE_MAX];

	unsigned int m_next_log_timestamp;												// 下次记录排行榜日志的时间
	bool m_is_init_finish;

};

#endif  // __GUILD_RANK_DEF_HPP__
