#include "guildrank.hpp"
#include "global/guild/guild.hpp"
#include "global/guild/guildmanager.hpp"

#include "global/rank/rankmanager.hpp"
#include "internalcomm.h"

#include "protocol/msgcsrank.h"
#include "protocol/msgscrank.h"

#include "engineadapter.h"
#include "servercommon/string/globalstr.h"
#include "gamelog.h"

#include "obj/character/role.h"
#include "servercommon/performancecatch.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "global/datasynccheck/datasynccheck.hpp"

union LongLongGuildLevelRankValue
{
	struct
	{
		unsigned int creat_time;
		short member_count;
		short level;
	};

	long long val;
};

UNSTD_STATIC_CHECK(sizeof(LongLongGuildLevelRankValue) == sizeof(long long));

static long long GuildLevelRankValueToLongLong(short level, short member_count, unsigned int creat_time)
{
	LongLongGuildLevelRankValue ll;

	ll.creat_time = static_cast<unsigned int>(-1) - creat_time;		// 创建时间是越小排越前
	ll.member_count = member_count;
	ll.level = level;

	return ll.val;
}

GuildRank::GuildRank()
	: m_rank_manager(NULL), m_is_init_finish(false)
{
	for (int i = 0; i < GUILD_RANK_TYPE_MAX; ++i)
	{
		m_rank_list[i].SetDefaultMaxSize(GUILD_RANK_MAX_SIZE);
		m_rank_list[i].SetCompare(&GuildRankItem::RankCompare);
	}
}

GuildRank::~GuildRank()
{
}

void GuildRank::Update(time_t now_second)
{
	if (!GuildManager::Instance().IsGuildLoadFinish() || CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (0 == m_next_log_timestamp)
	{
		m_next_log_timestamp = (unsigned int)EngineAdapter::Instance().Time();
	}

	if (now_second > m_next_log_timestamp)
	{
		this->OnRankLog(now_second);
	}
}

void GuildRank::OnWeekChange()
{
	// 家族活跃度排行榜清理, 跨服的由跨服清理 , 所以不需要专门去同步
	m_rank_list[GUILD_RANK_TYPE_WEEK_ACTIVE].Clear();
	m_guild_index_map[GUILD_RANK_TYPE_WEEK_ACTIVE].clear();
}

void GuildRank::OnRankLog(time_t _now_second)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	m_next_log_timestamp = static_cast<unsigned int>(_now_second + EngineAdapter::Instance().NextHourInterval(0, 0));

	//整点记录一次排行榜信息
	for (int i = 0; i < GUILD_RANK_TYPE_MAX; i++)
	{
		SkipList<GuildRankItem>::Iterator it_begin = m_rank_list[i].Begin();
		SkipList<GuildRankItem>::Iterator it_end = m_rank_list[i].End();
		int count = 0;
		for (; it_begin != it_end; ++it_begin)
		{
			gamelog::g_log_guild_rank_snapshot.printf(LL_INFO, "GuildRank::OnRankLog  rank_type[%d] rank_index[%d] guild[%d,%s] "
				"rank_value[%lld] ",
				i, count, it_begin->guild_id, it_begin->guild_name,
				it_begin->rank_val);

			++count;
		}
	}
}

void GuildRank::InitGuildIndex(int _rank_type)
{
	if (_rank_type < 0 || _rank_type >= GUILD_RANK_TYPE_MAX)
	{
		gamelog::g_log_serious_error.printf(LL_CRITICAL, "[GuildRank::InitGuildIndex INVALID RANK TYPE] [rank_type:%d]", (int)_rank_type);
		return;
	}

	SkipList<GuildRankItem>::Iterator  it_begin = m_rank_list[_rank_type].Begin();
	SkipList<GuildRankItem>::Iterator  it_end = m_rank_list[_rank_type].End();
	for (; it_begin != it_end; ++it_begin)
	{
		int rank_index = m_rank_list[_rank_type].GetIndex(*it_begin);
		if (-1 == rank_index)
		{
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[GuildRank::InitGuildIndex ERROR] rank_type[%d] [guild[%d %s] rank_val[%lld] "
				"list_size[%d]  list_size_is_full[%d]]",
				_rank_type, it_begin->guild_id, it_begin->guild_name , it_begin->rank_val ,
				m_rank_list[_rank_type].Size(), m_rank_list[_rank_type].IsFull());
			continue;
		}

		this->AddGuildIndex(_rank_type, *it_begin);
	}
}

void GuildRank::SyncRankAllDataToCross(int _rank_type)
{
	if (_rank_type < 0 || _rank_type >= GUILD_RANK_TYPE_MAX || CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	static crossgameprotocal::GameCrossSyncCrossGuildRankInfo sync_info;
	sync_info.count = 0;
	sync_info.from_usid.plat_type = LocalConfig::Instance().GetPlatType();
	sync_info.from_usid.server_id = LocalConfig::Instance().GetMergeServerId();
	sync_info.rank_type = _rank_type;
	sync_info.is_init = true;
	sync_info.is_init_add = 0;
	sync_info.last_change_time = (unsigned int)EngineAdapter::Instance().Time();
	sync_info.reserve_sh = 0;

	SkipList<GuildRankItem>::Iterator  it_begin = m_rank_list[_rank_type].Begin();
	SkipList<GuildRankItem>::Iterator  it_end = m_rank_list[_rank_type].End();
	int count = 0;
	for (; it_begin != it_end && count < CROSS_GUILD_RANK_SYNC_PAGE_ELEMENT_COUNT; ++it_begin)
	{
		sync_info.item_list[count++].rank_info = *it_begin;

		if (count >= CROSS_GUILD_RANK_SYNC_PAGE_ELEMENT_COUNT)
		{
			sync_info.count = count;

			int send_length = sizeof(crossgameprotocal::GameCrossSyncCrossGuildRankInfo) -
				((CROSS_GUILD_RANK_SYNC_PAGE_ELEMENT_COUNT - sync_info.count) * sizeof(sync_info.item_list[0]));

#ifdef _DEBUG
			gamelog::g_log_debug.printf(LL_INFO, "GuildRank::SyncRankAllDataToCross rank_type[%d] length[%d]", _rank_type, send_length);
#endif
			InternalComm::Instance().SendToCross((const char *)&sync_info, send_length);
			DataSyncCheck::Instance().AddCheck(crossgameprotocal::DATA_SYNC_CHECK_TYPE_RANK);
			count = 0;
			sync_info.is_init_add = 1;
		}
	}

	if (count > 0)
	{
		sync_info.count = count;
		int send_length = sizeof(crossgameprotocal::GameCrossSyncCrossGuildRankInfo) -
			((CROSS_GUILD_RANK_SYNC_PAGE_ELEMENT_COUNT - sync_info.count) * sizeof(sync_info.item_list[0]));

#ifdef _DEBUG
		gamelog::g_log_debug.printf(LL_INFO, "GuildRank::SyncRankAllDataToCross rank_type[%d] length[%d]", _rank_type, send_length);
#endif

		InternalComm::Instance().SendToCross((const char *)&sync_info, send_length);
		DataSyncCheck::Instance().AddCheck(crossgameprotocal::DATA_SYNC_CHECK_TYPE_RANK);
	}
}

void GuildRank::SyncRankDataToCross(int _rank_type, int _sync_status, int _old_index, GuildRankItem & _rank_item)
{
	if (_rank_type < 0 || _rank_type >= GUILD_RANK_TYPE_MAX || CrossConfig::Instance().IsHiddenServer()
		|| _sync_status <= crossgameprotocal::SYNC_RANK_TO_CROSS_STATUS_INVALID ||
		_sync_status >= crossgameprotocal::SYNC_RANK_TO_CROSS_STATUS_MAX)
	{
		return;
	}

	static crossgameprotocal::GameCrossSyncCrossGuildRankInfo sync_info;
	sync_info.count = 1;
	sync_info.from_usid.plat_type = LocalConfig::Instance().GetPlatType();
	sync_info.from_usid.server_id = LocalConfig::Instance().GetMergeServerId();
	sync_info.rank_type = _rank_type;
	sync_info.is_init = false;
	sync_info.last_change_time = (unsigned int)EngineAdapter::Instance().Time();
	sync_info.reserve_sh = 0;

	sync_info.item_list[0].rank_info = _rank_item;
	sync_info.item_list[0].index = _old_index;
	sync_info.item_list[0].sync_type = _sync_status;


	int send_length = sizeof(crossgameprotocal::GameCrossSyncCrossGuildRankInfo) -
		((CROSS_PERSON_RANK_SYNC_PAGE_ELEMENT_COUNT - sync_info.count) * sizeof(sync_info.item_list[0]));

#ifdef _DEBUG
	gamelog::g_log_debug.printf(LL_INFO, "GuildRank::SyncRankDataToCross rank_type[%d] length[%d]", _rank_type, send_length);
#endif

	InternalComm::Instance().SendToCross((const char *)&sync_info, send_length);
}

bool GuildRank::AddGuildIndex(int _rank_type, GuildRankItem & _check_item)
{
	if (_rank_type < 0 || _rank_type >= GUILD_RANK_TYPE_MAX) return false;

	GuildIndexMapIt it = m_guild_index_map[_rank_type].find(_check_item.guild_id);
	if (it == m_guild_index_map[_rank_type].end())
	{
		static IndexData index_data;
		index_data.Reset();
		this->ConstructionIndexData(index_data, _check_item);

		m_guild_index_map[_rank_type].insert(std::make_pair(_check_item.guild_id, index_data));
	}

	return true;
}

bool GuildRank::RemoveGuildIndex(int _rank_type, int _guild_id)
{
	if (_rank_type < 0 || _rank_type >= GUILD_RANK_TYPE_MAX) return false;

	m_guild_index_map[_rank_type].erase(_guild_id);

	return true;
}

void GuildRank::ConstructionCheckItem(GuildRankItem & _check_item, const IndexData & _index_data)
{
	_check_item.server_id = _index_data.server_id;
	_check_item.guild_id = _index_data.guild_id;
	_check_item.rank_type = _index_data.rank_type;
	_check_item.rank_val = _index_data.rank_value;
}

void GuildRank::ConstructionIndexData(IndexData & _index_data, const GuildRankItem & _check_item)
{
	_index_data.server_id = _check_item.server_id;
	_index_data.guild_id = _check_item.guild_id;
	_index_data.rank_type = _check_item.rank_type;
	_index_data.rank_value = _check_item.rank_val;
}

long long GuildRank::CalcGuildRankValue(Guild *guild, int rank_type, bool *rank_value_valid)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		if (NULL != rank_value_valid)
		{
			*rank_value_valid = false;
		}
		return 0;
	}

	if (rank_type < GUILD_RANK_TYPE_LEVEL || rank_type >= GUILD_RANK_TYPE_MAX) return 0;

	if (NULL != rank_value_valid)
	{
		*rank_value_valid = true;
	}

	long long rank_value = 0;

	switch (rank_type)
	{
	case GUILD_RANK_TYPE_LEVEL:
		rank_value = GuildLevelRankValueToLongLong(guild->GetGuildLevel(), guild->GetMemberManagerRef().GetMemberCount(), static_cast<unsigned int>(guild->GetGuildCreateTime()));
		break;
	case GUILD_RANK_TYPE_WEEK_ACTIVE:
		rank_value = guild->GetWeekActive();
		break;
	}

	return rank_value;
}

void GuildRank::InitGuildRank(int rank_type)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (rank_type < GUILD_RANK_TYPE_LEVEL || rank_type >= GUILD_RANK_TYPE_MAX) return;

	this->ClearRank(rank_type);

	const GuildManager::GuildMap &guild_map = GuildManager::Instance().GetGuildMap();
	for (GuildManager::GuildMapConstIt it = guild_map.begin(), end = guild_map.end(); it != end; ++it)
	{
		Guild *guild = it->second;
		if (NULL != guild)
		{
			bool rank_value_valid = true;
			long long rank_value = this->CalcGuildRankValue(guild, rank_type, &rank_value_valid);

			if (rank_value_valid && 0 != rank_value)
			{
				static GuildRankItem tmp_data;
				tmp_data.Reset();

				this->UpdateBaseInfo(tmp_data, guild);
				tmp_data.rank_type = rank_type;
				tmp_data.rank_val = rank_value;

				m_rank_list[rank_type].Insert(tmp_data);
			}
		}
	}

	this->InitGuildIndex(rank_type);
}

void GuildRank::InitGuildRank()
{
	if (!m_is_init_finish)
	{
		for (int rank_type = 0; rank_type < GUILD_RANK_TYPE_MAX; ++rank_type)
		{
			this->InitGuildRank(rank_type);
		}

		m_is_init_finish = true;
		RankManager::Instance().LoadGuildRankSucc();
	}
}

void GuildRank::UpdateRankList(GuildRankItem & _updata_rank_item)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (_updata_rank_item.rank_type < 0 || _updata_rank_item.rank_type >= GUILD_RANK_TYPE_MAX)
		return;

	// 存在则更新 , 不存在则插入
	int rank_type = _updata_rank_item.rank_type;
	int guild_id = _updata_rank_item.guild_id;
	bool need_insert = true;
	bool is_error_insert = false;		// 是否是容错插入 , 用于日志查BUG
	bool is_del_index_insert = false;  // 是否是删除索引后添加 , 用于日志查BUG

	static GuildRankItem check_item;
	int index = -1;
	GuildIndexMapIt it = m_guild_index_map[rank_type].find(_updata_rank_item.guild_id);
	if (it != m_guild_index_map[rank_type].end())
	{
		need_insert = false;
		
		check_item.Reset();
		this->ConstructionCheckItem(check_item, it->second);
		index = m_rank_list[rank_type].GetIndex(check_item);
		if (-1 == index)
		{
			// 因为UPDATE的时候被人把数据挤出了排行榜  , 但是索引里依然存在 
			// 所以如果根据索引无法找到对应的数据时 ,  
			// 所以则清除对应的索引  然后再进行新数据添加
			// 后续由新数据添加来做判断
			need_insert = true;
			is_del_index_insert = true;
			this->RemoveGuildIndex(rank_type, guild_id);
		}
	}

	// UPDATE 相关逻辑
	if (!need_insert)
	{
		// 当前角色在排行榜上有数据那么需要刷新数据
		SkipList<GuildRankItem>::Iterator skip_it = m_rank_list[rank_type][index];
		if (m_rank_list[rank_type].End() != skip_it)
		{
			// 如果相等则不需要更新 , 玩家上线的时候会重算战斗力 , 此时和排行榜内数据理论上无变化
			if (*skip_it == _updata_rank_item)
			{
				long long elapse_us = PerformanceCatch::Instance().GetElapseUsEnd(PERFORMANCE_TEST_PERSONRANK_UPDATA_RANK);
				gamelog::g_log_debug.printf(LL_INFO, "GuildRank::UpdateRankList[RETURN_BY_EQUAL] updata one data complete type[%d]  need_insert[%d]  is_error_insert[%d]  skip_list_size[%d] elapse[%lld]us=[%lld]ms",
					rank_type, need_insert, is_error_insert, m_rank_list[rank_type].Size(), elapse_us, elapse_us / 1000);
				return;
			}

			// 注意 updata后  原迭代器可能失效 ,如需使用需要重新获取新的迭代器
			m_rank_list[rank_type].UpdateData(*skip_it, _updata_rank_item);
			int new_index = m_rank_list[rank_type].GetIndex(_updata_rank_item);
			if (-1 != new_index)
			{
				// updata后更新一下索引数据
				this->ConstructionIndexData(it->second, _updata_rank_item);

				// 同步更新信息到跨服
				this->SyncRankDataToCross(rank_type, crossgameprotocal::SYNC_RANK_TO_CROSS_STATUS_UPDATA,
					index, _updata_rank_item);
			}
			else
			{
				// 记录错误日志 
				gamelog::g_log_serious_error.printf(LL_CRITICAL, "[GuildRank::UpdateRankList UPDATA ERROR] [check_guild[%d %s]  new_guild[%d %s]  rank_type[%d]  "
					"old_index[%d]  old_rank_val[%lld]  new_rank_val[%lld]]",
					check_item.guild_id, check_item.guild_name, _updata_rank_item.guild_id, _updata_rank_item.guild_name, rank_type,
					index, check_item.rank_val, _updata_rank_item.rank_val);

				// 容错
				need_insert = true;
				is_error_insert = true;
				this->RemoveGuildIndex(rank_type, guild_id);
			}
		}
		else
		{
			// 记录错误日志 
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[GuildRank::UpdateRankList operation[] ERROR] [check_guild[%d %s]  new_guild[%d %s]  rank_type[%d]   "
				"list_size[%d]  old_index[%d]  check_rank_val[%lld]  new_rank_val[%lld]]",
				check_item.guild_id, check_item.guild_name, _updata_rank_item.guild_id, _updata_rank_item.guild_name, rank_type,
				m_rank_list[rank_type].Size(), index, check_item.rank_val, _updata_rank_item.rank_val);

			// 容错
			need_insert = true;
			is_error_insert = true;
			this->RemoveGuildIndex(rank_type, guild_id);
		}
	}

	// INSERT 相关逻辑
	if (need_insert)
	{
		if (m_rank_list[rank_type].IsFull())
		{
			// 如果表已经满了, 
			SkipList<GuildRankItem>::Reverse_Iterator r_iter = m_rank_list[rank_type].RBegin();
			if (m_rank_list[rank_type].REnd() != r_iter)
			{
				// 新入数据大于末尾数据
				if (_updata_rank_item > *r_iter)
				{
					this->SyncRankDataToCross(rank_type, crossgameprotocal::SYNC_RANK_TO_CROSS_STATUS_DELETE,
						m_rank_list[rank_type].Size() - 1, *r_iter);

					m_rank_list[rank_type].Erase(*r_iter);
					m_rank_list[rank_type].Insert(_updata_rank_item);
					int index = m_rank_list[rank_type].GetIndex(_updata_rank_item);
					if (-1 != index)
					{
						// 同步插入信息到跨服
						this->SyncRankDataToCross(rank_type, crossgameprotocal::SYNC_RANK_TO_CROSS_STATUS_INSERT,
							-1, _updata_rank_item);

						this->AddGuildIndex(rank_type, _updata_rank_item);

						// 插入日志
						gamelog::g_log_rank.printf(LL_INFO, "[GuildRank::UpdateRankList Insert_SUCC_1]  is_del_index_insert:%d   [new_guild[%d %s]  rank_type[%d]  rank_index[%d]  list_size[%d] "
							"rank_value[%lld] ",
							is_del_index_insert , _updata_rank_item.guild_id, _updata_rank_item.guild_name, rank_type, index, m_rank_list[rank_type].Size(),
							_updata_rank_item.rank_val);
					}
					else
					{
						// 错误日志 , 如果出现了需要查逻辑
						gamelog::g_log_rank.printf(LL_INFO, "[GuildRank::UpdateRankList Insert_ERROR]  [new_guild[%d %s]  rank_type[%d]  rank_index[%d]  list_size[%d] "
							"rank_value[%lld] ",
							_updata_rank_item.guild_id, _updata_rank_item.guild_name, rank_type, index, m_rank_list[rank_type].Size(),
							_updata_rank_item.rank_val);
					}
				}
			}
		}
		else
		{
			// 没满直接插入
			m_rank_list[rank_type].Insert(_updata_rank_item);
			int index = m_rank_list[rank_type].GetIndex(_updata_rank_item);
			if (-1 != index)
			{
				// 同步插入信息到跨服
				this->SyncRankDataToCross(rank_type, crossgameprotocal::SYNC_RANK_TO_CROSS_STATUS_INSERT,
					-1, _updata_rank_item);

				this->AddGuildIndex(rank_type, _updata_rank_item);

				// 插入日志
				gamelog::g_log_rank.printf(LL_INFO, "[GuildRank::UpdateRankList Insert_SUCC_2] is_del_index_insert:%d  [new_guild[%d %s]  is_error_insert[%d]  rank_type[%d]  rank_index[%d]  list_size[%d] "
					"rank_value[%lld] ",
					is_del_index_insert ,_updata_rank_item.guild_id, _updata_rank_item.guild_name, is_error_insert, rank_type, index, m_rank_list[rank_type].Size(),
					_updata_rank_item.rank_val);
			}
			else
			{
				// 错误日志 , 如果出现了需要查逻辑
				gamelog::g_log_rank.printf(LL_INFO, "[GuildRank::UpdateRankList Insert_ERROR]  [new_user[%d %s]  is_error_insert[%d]  rank_type[%d]  rank_index[%d]  list_size[%d] "
					"rank_value[%lld] ",
					_updata_rank_item.guild_id, _updata_rank_item.guild_name, is_error_insert, rank_type, index, m_rank_list[rank_type].Size(),
					_updata_rank_item.rank_val);
			}
		}
	}

	long long elapse_us = PerformanceCatch::Instance().GetElapseUsEnd(PERFORMANCE_TEST_GUILDRANK_UPDATA_RANK);
	gamelog::g_log_debug.printf(LL_INFO, "GuildRank::UpdateRankList updata one data complete type[%d]  need_insert[%d]  is_error_insert[%d]  skip_list_size[%d] elapse[%lld]us=[%lld]ms",
		rank_type, need_insert, is_error_insert, m_rank_list[rank_type].Size(), elapse_us, elapse_us / 1000);
}

void GuildRank::OnGetRankList(Role *role, Protocol::CSGetGuildRankList &rank_list_req)
{
	if (NULL == role ||CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	int rank_type = rank_list_req.rank_type;

	if (rank_type < GUILD_RANK_TYPE_LEVEL || rank_type >= GUILD_RANK_TYPE_MAX) return;

	if (rank_list_req.rank_start < 0 || rank_list_req.rank_end < 0 || rank_list_req.rank_start  > rank_list_req.rank_end)
	{
		return;
	}


	static Protocol::SCGetGuildRankListAck rank_ack;

	rank_ack.rank_type = rank_type;
	rank_ack.start_rank_index = rank_list_req.rank_start;
	rank_ack.rank_count = 0;

	int rank_count = 0, target_get_count = rank_list_req.rank_end - rank_list_req.rank_start + 1;
	SkipList<GuildRankItem>::Iterator  it_begin = m_rank_list[rank_type][rank_list_req.rank_start];
	SkipList<GuildRankItem>::Iterator  it_end = m_rank_list[rank_type].End();
	for (; it_begin != it_end &&rank_count < GUILD_RANK_MAX_SIZE && rank_count < target_get_count; ++it_begin)
	{
		rank_ack.rank_list[rank_count].guild_id = it_begin->guild_id;
		rank_ack.rank_list[rank_count].tuan_zhang_uid = UidToInt(it_begin->tuan_zhang_user_id);
		F_STRNCPY(rank_ack.rank_list[rank_count].guild_name, it_begin->guild_name, sizeof(rank_ack.rank_list[0].guild_name));
		F_STRNCPY(rank_ack.rank_list[rank_count].tuan_zhang_name, it_begin->tuan_zhang_name, sizeof(rank_ack.rank_list[0].tuan_zhang_name));
		rank_ack.rank_list[rank_count].guild_level = it_begin->guild_level;
		rank_ack.rank_list[rank_count].camp = it_begin->camp;
		rank_ack.rank_list[rank_count].member_count = it_begin->member_count;
		rank_ack.rank_list[rank_count].max_member_count = it_begin->max_member_count;
		rank_ack.rank_list[rank_count].rank_value = it_begin->rank_val;

		++rank_count;
	}
	rank_ack.rank_count = rank_count;

	unsigned int sendlen = sizeof(rank_ack) - (GUILD_RANK_MAX_SIZE - rank_ack.rank_count) * sizeof(rank_ack.rank_list[0]);
	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&rank_ack, sendlen);
}

void GuildRank::OnBaseInfoChange(Guild *guild)
{
	if (NULL == guild || CrossConfig::Instance().IsHiddenServer()) return;

	for (int i = 0; i < GUILD_RANK_TYPE_MAX; ++i)
	{
		GuildIndexMapIt it = m_guild_index_map[i].find(guild->GetGuildID());
		if (it != m_guild_index_map[i].end())
		{
			static GuildRankItem check_item;
			check_item.Reset();
			this->ConstructionCheckItem(check_item, it->second);

			int rank_index = m_rank_list[i].GetIndex(check_item);
			if (-1 == rank_index)
			{
				continue;
			}

			SkipList<GuildRankItem>::Iterator rank_it = m_rank_list[i][rank_index];
			if (m_rank_list[i].End() != rank_it)
			{
				this->UpdateBaseInfo(*rank_it, guild);
				this->SyncRankDataToCross(i, crossgameprotocal::SYNC_RANK_TO_CROSS_STATUS_UPDATA, rank_index, *rank_it);
			}
			else
			{
				// 这是不应该出现的问题 , 如果出现了需要查逻辑和map相关的调用
				gamelog::g_log_serious_error.printf(LL_CRITICAL, "[GuildRank::OnBaseInfoChange Find Rank_info ERROR] [guild[%d %s]  "
					"rank_type[%d]  rank_index[%d]",
					guild->GetGuildID(), guild->GetName() , i, rank_index);
			}
		}
	}
}

bool GuildRank::IsRankLoadFinish()
{
	return m_is_init_finish;
}

void GuildRank::OnGameworldAccept()
{
	if (!m_is_init_finish || CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	for (int i = 0; i < GUILD_RANK_TYPE_MAX; ++i)
	{
		this->SyncRankAllDataToCross(i);
	}
}

void GuildRank::ClearRank(int rank_type)
{
	if (rank_type < 0 || rank_type >= GUILD_RANK_TYPE_MAX || CrossConfig::Instance().IsHiddenServer()) return;

	m_rank_list[rank_type].Clear();
	m_guild_index_map[rank_type].clear();
}

bool GuildRank::IsGuildInTopRank(GuildID guild_id, int rank_type, int max_rank)
{
	if (INVALID_GUILD_ID == guild_id || rank_type < 0 || rank_type >= GUILD_RANK_TYPE_MAX || max_rank <= 0 || CrossConfig::Instance().IsHiddenServer())
		return false;

	Guild *guild = GuildManager::Instance().GetGuild(guild_id);
	if (NULL == guild) return false;

	GuildIndexMapIt it = m_guild_index_map[rank_type].find(guild->GetGuildID());
	if (m_guild_index_map[rank_type].end() != it)
	{
		static GuildRankItem check_item;
		check_item.Reset();
		this->ConstructionCheckItem(check_item, it->second);

		int rank_index = m_rank_list[rank_type].GetIndex(check_item);

		if (rank_index >= 0 && rank_index <= max_rank)
		{
			return true;
		}
	}

	return false;
}

void GuildRank::OnGuildRankInfoChange(Guild * _guild, int _rank_type)
{
	if (NULL == _guild || _rank_type < 0 || _rank_type >= GUILD_RANK_TYPE_MAX || CrossConfig::Instance().IsHiddenServer())
		 return;

	static GuildRankItem rank_item;
	rank_item.Reset();
	this->UpdateBaseInfo(rank_item, _guild);

	switch (_rank_type)
	{
	case GUILD_RANK_TYPE_LEVEL:
		{
			rank_item.rank_type = GUILD_RANK_TYPE_LEVEL;
			rank_item.rank_val = GuildLevelRankValueToLongLong(_guild->GetGuildLevel(), _guild->GetMemberManagerRef().GetMemberCount(), static_cast<unsigned int>(_guild->GetGuildCreateTime()));
		}
		break;
	case GUILD_RANK_TYPE_WEEK_ACTIVE:
		{
			rank_item.rank_type = GUILD_RANK_TYPE_WEEK_ACTIVE;
			rank_item.rank_val = _guild->GetWeekActive();;
		}
		break;
	default:
		break;
	}

	this->UpdateRankList(rank_item);
}

void GuildRank::RemoveGuild(int guild_id)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	for (int i = 0; i < GUILD_RANK_TYPE_MAX; ++i)
	{
		GuildIndexMapIt it = m_guild_index_map[i].find(guild_id);
		if (it != m_guild_index_map[i].end())
		{
			static GuildRankItem check_item;
			check_item.Reset();
			this->ConstructionCheckItem(check_item, it->second);

			int new_index = m_rank_list[i].GetIndex(check_item);
			// 同步到跨服删除对应帮派的排行榜信息
			this->SyncRankDataToCross(i, crossgameprotocal::SYNC_RANK_TO_CROSS_STATUS_DELETE,
				new_index, check_item);
			m_rank_list[i].Erase(check_item);

			// 删除索引
			this->RemoveGuildIndex(i ,guild_id);
		}
	}
}

void GuildRank::UpdateBaseInfo(GuildRankItem &rank_item, Guild *guild)
{
	rank_item.server_id = LocalConfig::Instance().GetUniqueServerID();
	rank_item.guild_id = guild->GetGuildID();

	guild->GetGuildName(rank_item.guild_name);
	memcpy(rank_item.guild_banner, guild->GetBanner(), sizeof(GuildBanner));

	rank_item.tuan_zhang_user_id = guild->GetMemberManagerRef().GetPatriarchUserID();
	guild->GetMemberManagerRef().GetPatriarchName(rank_item.tuan_zhang_name);

	rank_item.guild_level = guild->GetGuildLevel();
	rank_item.camp = 0;
	rank_item.member_count = guild->GetMemberManagerRef().GetMemberCount();;
	rank_item.max_member_count = guild->GetMaxMemberCount();
}
