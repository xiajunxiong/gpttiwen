#include "pettoprankshadow.hpp"
#include "servercommon/servercommon.h"
#include "engineadapter.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "global/rank/petrank.hpp"
#include "global/rank/rankmanager.hpp"
#include "internalcomm.h"
#include "servercommon/serverconfig/localconfig.hpp"
#include "world.h"
#include "protocol/msgother.h"

PetTopRankShadow::PetTopRankShadow() : m_next_sync_top_rank_timestamp(0u),
m_top_rank_num(0)
{

}

PetTopRankShadow::~PetTopRankShadow()
{

}

int PetTopRankShadow::GetPetTopRank(int owner_uid, int pet_id, long long pet_unique_id) const
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return this->GetPetTopRank(owner_uid, pet_id, pet_unique_id, m_top_rank_num, m_top_list);
	}
	else
	{
		UniqueServerID usid(LocalConfig::Instance().GetPlatType(), IntToUid(owner_uid).db_index);
		const UniqueServerID* mother_usid = World::GetInstWorld()->GetMotherServerId(usid);
		if (NULL != mother_usid)
		{
			OriginRankShadowMap::const_iterator it = m_origin_rank_shadow_map.find(mother_usid->server_id);
			if (it != m_origin_rank_shadow_map.end())
			{
				return this->GetPetTopRank(owner_uid, pet_id, 
					pet_unique_id, it->second.top_rank_num, 
					it->second.top_list);
			}
		}
	}

	return -1;
}

void PetTopRankShadow::SendPetAllTopRankInfo(int user_uid)
{
	Protocol::SCPetTopRankInfo msg;

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		memcpy(msg.top_list, m_top_list, sizeof(msg.top_list));
	}
	else
	{
		UniqueServerID usid(LocalConfig::Instance().GetPlatType(), IntToUid(user_uid).db_index);
		const UniqueServerID* mother_usid = World::GetInstWorld()->GetMotherServerId(usid);
		if (NULL != mother_usid)
		{
			OriginRankShadowMap::const_iterator it = m_origin_rank_shadow_map.find(mother_usid->server_id);
			if (it != m_origin_rank_shadow_map.end())
			{
				memcpy(msg.top_list, it->second.top_list, sizeof(msg.top_list));
			}
		}
	}

	World::GetInstWorld()->SendToRole(UidToInt(user_uid), &msg, sizeof(msg));
}

int PetTopRankShadow::GetPetTopRank(int owner_uid, int pet_id, long long pet_unique_id, int list_num, const PetTopRankInfo* rank_list) const
{
	for (int i = 0; i < list_num
		&& i < PET_TOP_RANK_NUM; ++i)
	{
		if (rank_list[i].owner_uid == owner_uid
			&& rank_list[i].pet_id == pet_id
			&& rank_list[i].pet_unique_id == pet_unique_id)
		{
			return i;
		}
	}

	return -1;
}

void PetTopRankShadow::Update(unsigned int now_timestmap)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		// 跨服靠同步数据
		return;
	}

	if (m_next_sync_top_rank_timestamp > 0u
		&& now_timestmap > m_next_sync_top_rank_timestamp)
	{
		m_next_sync_top_rank_timestamp = now_timestmap + PET_TOP_RANK_SYNC_INTERNVAL_S;
		this->SyncPetTopRank();
	}
}

void PetTopRankShadow::OnRankManagerLoadSucc()
{
	m_next_sync_top_rank_timestamp = 
		static_cast<unsigned>(EngineAdapter::Instance().Time()) + PET_TOP_RANK_SYNC_INTERNVAL_S;
}

void PetTopRankShadow::SyncPetTopRankFromOrigin(crossgameprotocal::GameHiddenSyncPetTopRankInfo* msg)
{
	OriginServerPetTopRankShadow& info = m_origin_rank_shadow_map[msg->server_id];
	info.top_rank_num = 0;
	for (size_t i = 0; i < msg->rank_list.size()
		&& info.top_rank_num < PET_TOP_RANK_NUM; ++i)
	{
		info.top_list[info.top_rank_num].owner_uid = msg->rank_list[i].owner_uid;
		info.top_list[info.top_rank_num].pet_id = msg->rank_list[i].pet_id;
		info.top_list[info.top_rank_num].pet_unique_id = msg->rank_list[i].pet_unique_id;
		info.top_rank_num += 1;
	}
}

void PetTopRankShadow::SyncPetTopRank()
{
	static const PetRankItem* rank_list[PET_TOP_RANK_NUM];
	memset(rank_list, 0, sizeof(rank_list));
	int rank_num = 0;
	PetRank* petrank = RankManager::Instance().GetPetRank();
	petrank->GetTopRankList(PET_RANK_TYPE_CAPABILITY,
		PET_TOP_RANK_NUM, rank_list, &rank_num);
	
	//if (this->CheckTopRankChanged(rank_num, rank_list))
	//{
	this->SetTopRankList(rank_num, rank_list);
	this->SyncPetTopRankToHidden();
	//}
	
}

bool PetTopRankShadow::CheckTopRankChanged(int rank_num, const PetRankItem** rank_list)
{
	if (rank_num != m_top_rank_num)
	{
		return true;
	}

	for (int i = 0; i < rank_num && i < PET_TOP_RANK_NUM; ++i)
	{
		if (rank_list[i]->master_info.uid != m_top_list[i].owner_uid
			|| rank_list[i]->pet_param.pet_param.pet_id != m_top_list[i].pet_id
			|| rank_list[i]->pet_param.pet_unique_id != m_top_list[i].pet_unique_id)
		{
			return true;
		}
	}

	return false;
}

void PetTopRankShadow::SetTopRankList(int rank_num, const PetRankItem** rank_list)
{
	for (int i = 0; i < rank_num && i < PET_TOP_RANK_NUM; ++i)
	{
		m_top_list[i].owner_uid = rank_list[i]->master_info.uid;
		m_top_list[i].pet_id = rank_list[i]->pet_param.pet_param.pet_id;
		m_top_list[i].pet_unique_id = rank_list[i]->pet_param.pet_unique_id;
	}
	m_top_rank_num = GetMin(PET_TOP_RANK_NUM, rank_num);
}

void PetTopRankShadow::SyncPetTopRankToHidden()
{
	crossgameprotocal::GameHiddenSyncPetTopRankInfo msg;
	msg.server_id = LocalConfig::Instance().GetMergeServerId();
	for (int i = 0; i < m_top_rank_num 
		&& i < ARRAY_ITEM_COUNT(m_top_list); ++i)
	{
		crossgameprotocal::GameHiddenSyncPetTopRankInfo::RankInfo rankinfo;
		rankinfo.owner_uid = m_top_list[i].owner_uid;
		rankinfo.pet_id = m_top_list[i].pet_id;
		rankinfo.pet_unique_id = m_top_list[i].pet_unique_id;
		msg.rank_list.push_back(rankinfo);
	}

	InternalComm::Instance().SendSerializeMsgToHiddenThroughCross(&msg);
}

