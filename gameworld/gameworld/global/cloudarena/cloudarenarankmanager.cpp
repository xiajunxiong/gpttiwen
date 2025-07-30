#include "cloudarenarankmanager.hpp"
#include "cloudarenarank.hpp"
#include "cloudarenauser.hpp"
#include "protocol/msgcloudarena.h"
#include "servercommon/serverconfig/localconfig.hpp"
#include "world.h"
#include "cloudarenaconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "global/cloudarena/cloudarena.hpp"
#include "global/usercache/usercache.hpp"

CloudArenaRankManager::CloudArenaRankManager()
{
	m_total_rank = new CloudArenaRank();

	memset(m_dan_rank_list, 0, sizeof(m_dan_rank_list));
	for (int dan_type = CLOUD_ARENA_DAN_TYPE_INVALID + 1; dan_type < CLOUD_ARENA_DAN_TYPE_MAX; ++dan_type)
	{
		m_dan_rank_list[dan_type] = new CloudArenaRank();
	}
}

CloudArenaRankManager::~CloudArenaRankManager()
{
	this->Release();
}

void CloudArenaRankManager::OnUserUpdateRankInfo(const CloudArenaUser* user)
{
	if (NULL == user) return;

	CloudArenaRankItem rank_item;
	this->ConstructRankItem(&rank_item, user);

	m_total_rank->UpdateRankItem(rank_item);

	if (!user->IsRobot())
	{
		this->TryUpdateServerRank(user->GetUID(), rank_item);
	}
	else
	{
		this->RobotUpdateAllServerRank(rank_item);
	}
}

void CloudArenaRankManager::OnUserUpdateDanRank(const CloudArenaUser* user, int old_dan, int new_dan)
{
	if (NULL == user) return;

	if (old_dan != new_dan)
	{
		if (old_dan > CLOUD_ARENA_DAN_TYPE_INVALID && old_dan < CLOUD_ARENA_DAN_TYPE_MAX)
		{
			m_dan_rank_list[old_dan]->RemoveFromRankList(user->GetUID());
		}		
	}
	
	if (new_dan > CLOUD_ARENA_DAN_TYPE_INVALID && new_dan < CLOUD_ARENA_DAN_TYPE_MAX)
	{
		CloudArenaRankItem rank_item;
		this->ConstructRankItem(&rank_item, user);
		m_dan_rank_list[new_dan]->UpdateRankItem(rank_item);
	}
}

void CloudArenaRankManager::OnRobotUpdateServerRankInfo(const CloudArenaUser* user, const UniqueServerID& usid)
{
	if (!user->IsRobot()) return;

	CloudArenaRankItem rank_item;
	this->ConstructRankItem(&rank_item, user);

	CloudArenaRank* rank_inst = this->GetServerRankInstance(usid);
	if (NULL == rank_inst) return;

	rank_inst->UpdateRankItem(rank_item);
}

void CloudArenaRankManager::AssembleTotalRankMsg(int uid, Protocol::SCCloudArenaRankList* msg)
{
	msg->rank_type = CLOUD_ARENA_RANK_TYPE_TOTAL;

	this->AssembleRankMsg(m_total_rank, uid, msg);
}

void CloudArenaRankManager::AssembleDanRankMsg(int uid, int dan_type, Protocol::SCCloudArenaRankList* msg)
{
	if (dan_type <= CLOUD_ARENA_DAN_TYPE_INVALID || dan_type >= CLOUD_ARENA_DAN_TYPE_MAX) return;

	msg->rank_type = CLOUD_ARENA_RANK_TYPE_DAN;

	this->AssembleRankMsg(m_dan_rank_list[dan_type], uid, msg);
}

void CloudArenaRankManager::AssembleServerRankMsg(int uid, Protocol::SCCloudArenaRankList* msg)
{
	msg->rank_type = CLOUD_ARENA_RANK_TYPE_LOCAL_SERVER;

	int db_index = IntToUid(uid).db_index;
	UniqueServerID usid(LocalConfig::Instance().GetPlatType(), db_index);
	const UniqueServerID* mother_usid = World::GetInstWorld()->GetMotherServerId(usid);
	if (NULL != mother_usid)
	{
		CloudArenaRank* rank_inst = this->GetServerRankInstance(*mother_usid);
		if (NULL != rank_inst)
		{
			this->AssembleRankMsg(rank_inst, uid, msg);
		}
	}
}

int CloudArenaRankManager::GetUserRank(int uid)
{
	int ret_rank = -1;
	m_total_rank->FindUserRankItem(uid, &ret_rank);

	return ret_rank;
}

void CloudArenaRankManager::OnGameRegisterCross(const UniqueServerID& usid)
{
	CloudArenaRank* rank_inst = this->GetServerRankInstance(usid);
	if (NULL == rank_inst)
	{
		rank_inst = new CloudArenaRank();
		m_server_rank_map.insert(ServerRankMap::value_type(usid, rank_inst));
	}

	this->RetryUnknownMotherServerUserRankItemUpdate(usid);
}

void CloudArenaRankManager::OnStartNewSeason()
{
	m_total_rank->OnStartNewSeason();
	
	for (int dan_type = CLOUD_ARENA_DAN_TYPE_INVALID + 1; dan_type < CLOUD_ARENA_DAN_TYPE_MAX; ++dan_type)
	{
		m_dan_rank_list[dan_type]->OnStartNewSeason();
	}

	ServerRankMap::iterator it = m_server_rank_map.begin();
	for (; it != m_server_rank_map.end(); ++it)
	{
		it->second->OnStartNewSeason();
	}

	m_unknown_mother_server_user_map.clear();
}

void CloudArenaRankManager::ConstructRankItem(ARG_OUT CloudArenaRankItem* item, const CloudArenaUser* user)
{
	if (NULL == item || NULL == user) return;

	item->uid = user->GetUID();
	item->level = user->GetLevel();
	item->avatar = user->GetAvatar();
	item->profession = user->GetProfession();
	item->headshot_id = user->GetAppearance().headshot_id;
	F_STRNCPY(item->name, user->GetName(), sizeof(item->name));
	item->score = user->GetScore();
	item->reach_score_timestamp = user->GetReachScoreTimestamp();
	item->last_season_rank = user->GetLastSeasonRank();
}

CloudArenaRank* CloudArenaRankManager::GetServerRankInstance(const UniqueServerID& unique_server_id)
{
	ServerRankMap::iterator it = m_server_rank_map.find(unique_server_id);
	if (it != m_server_rank_map.end())
	{
		return it->second;
	}

	return NULL;
}

void CloudArenaRankManager::TryUpdateServerRank(int uid, const CloudArenaRankItem& rank_item)
{
	int db_index = IntToUid(uid).db_index;
	UniqueServerID usid(LocalConfig::Instance().GetPlatType(), db_index);
	const UniqueServerID* mother_usid = World::GetInstWorld()->GetMotherServerId(usid);
	if (NULL == mother_usid)
	{
		this->UpdateUnknownMotherServerUserRankItem(uid, rank_item);	
	}
	else
	{
		CloudArenaRank* rank_inst = this->GetServerRankInstance(*mother_usid);
		if (NULL == rank_inst)
		{
			rank_inst = new CloudArenaRank();
			m_server_rank_map.insert(ServerRankMap::value_type(*mother_usid, rank_inst));
		}
		
		rank_inst->UpdateRankItem(rank_item);
	}
}

void CloudArenaRankManager::RobotUpdateAllServerRank(const CloudArenaRankItem& rank_item)
{
	ServerRankMap::iterator it = m_server_rank_map.begin();
	for (; it != m_server_rank_map.end(); ++it)
	{
		CloudArenaRank* rank_inst = it->second;
		rank_inst->UpdateRankItem(rank_item);
	}
}

void CloudArenaRankManager::UpdateUnknownMotherServerUserRankItem(int uid, const CloudArenaRankItem& rank_item)
{
	m_unknown_mother_server_user_map[uid] = rank_item;
}

void CloudArenaRankManager::RetryUnknownMotherServerUserRankItemUpdate(const UniqueServerID& mother_server_uid)
{
	UnknownMotherServerUserMap::iterator it = m_unknown_mother_server_user_map.begin();
	for (; it != m_unknown_mother_server_user_map.end();)
	{
		int db_index = IntToUid(it->first).db_index;
		UniqueServerID usid(LocalConfig::Instance().GetPlatType(), db_index);
		const UniqueServerID* musid = World::GetInstWorld()->GetMotherServerId(usid);
		if (NULL != musid && *musid == mother_server_uid)
		{
			this->TryUpdateServerRank(it->first, it->second);
			m_unknown_mother_server_user_map.erase(it++);
		}
		else
		{
			++it;
		}
	}
}

void CloudArenaRankManager::Release()
{
	delete m_total_rank;
	m_total_rank = NULL;

	ServerRankMap::iterator it = m_server_rank_map.begin();
	for (; it != m_server_rank_map.end(); ++it)
	{
		delete it->second;
		it->second = NULL;
	}

	m_server_rank_map.clear();

	for (int dan_type = CLOUD_ARENA_DAN_TYPE_INVALID + 1; dan_type < CLOUD_ARENA_DAN_TYPE_MAX; ++dan_type)
	{
		delete m_dan_rank_list[dan_type];
		m_dan_rank_list[dan_type] = NULL;
	}
}

void CloudArenaRankManager::AssembleRankMsg(CloudArenaRank* rank_inst, int uid, Protocol::SCCloudArenaRankList* msg)
{
	msg->rank_num = 0;

	const CloudArenaRankItem* rank_list[ARRAY_ITEM_COUNT(msg->rank_list)];
	memset(rank_list, 0, sizeof(rank_list));
	int num = 0;
	rank_inst->GetTopRankItems(ARRAY_ITEM_COUNT(rank_list), &num, rank_list);

	for (int i = 0; i < num && msg->rank_num < ARRAY_ITEM_COUNT(msg->rank_list); ++i)
	{
		msg->rank_list[msg->rank_num].uid = rank_list[i]->uid;
		msg->rank_list[msg->rank_num].level = rank_list[i]->level;
		msg->rank_list[msg->rank_num].top_level = UserCacheManager::Instance().GetUserTopLevel(rank_list[i]->uid);
		msg->rank_list[msg->rank_num].is_robot = CloudArena::IsRobot(rank_list[i]->uid);
		msg->rank_list[msg->rank_num].profession = rank_list[i]->profession;
		msg->rank_list[msg->rank_num].rank = i;
		msg->rank_list[msg->rank_num].score = rank_list[i]->score;
		F_STRNCPY(msg->rank_list[msg->rank_num].name, rank_list[i]->name, sizeof(msg->rank_list[0].name));
		msg->rank_num += 1;
	}

	int my_rank = -1;
	const CloudArenaRankItem* my_rank_item = rank_inst->FindUserRankItem(uid, &my_rank);
	if (NULL != my_rank_item)
	{
		msg->my_rank.rank = my_rank;
		msg->my_rank.uid = uid;
		msg->my_rank.level = my_rank_item->level;
		msg->my_rank.top_level = UserCacheManager::Instance().GetUserTopLevel(my_rank_item->uid);
		msg->my_rank.is_robot = 0;
		msg->my_rank.profession = my_rank_item->profession;
		F_STRNCPY(msg->my_rank.name, my_rank_item->name, sizeof(msg->my_rank.name));
		msg->my_rank.score = my_rank_item->score;
	}
	else
	{
		msg->my_rank.rank = -1;
		msg->my_rank.uid = uid;
		msg->my_rank.is_robot = 0;
	}
}

