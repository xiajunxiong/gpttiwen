#include "servercommon/servercommon.h"
#include "engineadapter.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "global/rank/rankmanager.hpp"
#include "global/rank/partnerrank.hpp"
#include "internalcomm.h"
#include "servercommon/serverconfig/localconfig.hpp"
#include "world.h"
#include "protocol/msgother.h"

PartnerTopRankShadow::PartnerTopRankShadow() : m_next_sync_top_rank_timestamp(0u),
m_top_rank_num(0)
{

}

PartnerTopRankShadow::~PartnerTopRankShadow()
{

}

int PartnerTopRankShadow::GetPartnerTopRank(int owner_uid, int partner_id) const
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return this->GetPartnerTopRank(owner_uid, partner_id, m_top_rank_num, m_top_list);
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
				return this->GetPartnerTopRank(owner_uid, partner_id,
					it->second.top_rank_num,
					it->second.top_list);
			}
		}
	}

	return -1;
}

void PartnerTopRankShadow::SendPartnerAllTopRankInfo(int user_uid)
{
	Protocol::SCPartnerTopRankInfo msg;

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


int PartnerTopRankShadow::GetPartnerTopRank(int owner_uid, int partner_id, int list_num, const PartnerTopRankInfo* rank_list) const
{
	for (int i = 0; i < list_num
		&& i < PARTNER_TOP_RANK_NUM; ++i)
	{
		if (rank_list[i].owner_uid == owner_uid
			&& rank_list[i].partner_id == partner_id)
		{
			return i;
		}
	}

	return -1;
}

void PartnerTopRankShadow::Update(unsigned int now_timestmap)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		// 跨服靠同步数据
		return;
	}

	if (m_next_sync_top_rank_timestamp > 0u
		&& now_timestmap > m_next_sync_top_rank_timestamp)
	{
		m_next_sync_top_rank_timestamp = now_timestmap + PARTNER_TOP_RANK_SYNC_INTERNVAL_S;
		this->SyncPartnerTopRank();
	}
}

void PartnerTopRankShadow::OnRankManagerLoadSucc()
{
	m_next_sync_top_rank_timestamp = 
		static_cast<unsigned>(EngineAdapter::Instance().Time()) + PARTNER_TOP_RANK_SYNC_INTERNVAL_S;
}

void PartnerTopRankShadow::SyncPartnerTopRankFromOrigin(crossgameprotocal::GameHiddenSyncPartnerTopRankInfo* msg)
{
	OriginServerPartnerTopRankShadow& info = m_origin_rank_shadow_map[msg->server_id];
	info.top_rank_num = 0;
	for (size_t i = 0; i < msg->rank_list.size()
		&& info.top_rank_num < PARTNER_TOP_RANK_NUM; ++i)
	{
		info.top_list[info.top_rank_num].owner_uid = msg->rank_list[i].owner_uid;
		info.top_list[info.top_rank_num].partner_id = msg->rank_list[i].partner_id;
		info.top_rank_num += 1;
	}
}

void PartnerTopRankShadow::SyncPartnerTopRank()
{
	static const PartnerRankItem* rank_list[PARTNER_TOP_RANK_NUM];
	memset(rank_list, 0, sizeof(rank_list));
	int rank_num = 0;
	PartnerRank* partnerrank = RankManager::Instance().GetPartnerRank();
	partnerrank->GetTopRankList(PARTNER_RANK_TYPE_CAPABILITY,
		PARTNER_TOP_RANK_NUM, rank_list, &rank_num);

	//if (this->CheckTopRankChanged(rank_num, rank_list))
	//{
		this->SetTopRankList(rank_num, rank_list);
		this->SyncPartnerTopRankToHidden();
	//}
}

bool PartnerTopRankShadow::CheckTopRankChanged(int rank_num, const PartnerRankItem** rank_list)
{
	if (rank_num != m_top_rank_num)
	{
		return true;
	}

	for (int i = 0; i < rank_num && i < PARTNER_TOP_RANK_NUM; ++i)
	{
		if (rank_list[i]->user_uid != m_top_list[i].owner_uid
			|| rank_list[i]->partner_id != m_top_list[i].partner_id)
		{
			return true;
		}
	}

	return false;
}

void PartnerTopRankShadow::SetTopRankList(int rank_num, const PartnerRankItem** rank_list)
{
	for (int i = 0; i < rank_num && i < PARTNER_TOP_RANK_NUM; ++i)
	{
		m_top_list[i].owner_uid = rank_list[i]->user_uid;
		m_top_list[i].partner_id = rank_list[i]->partner_id;
	}
	m_top_rank_num = GetMin(PARTNER_TOP_RANK_NUM, rank_num);
}

void PartnerTopRankShadow::SyncPartnerTopRankToHidden()
{
	crossgameprotocal::GameHiddenSyncPartnerTopRankInfo msg;
	msg.server_id = LocalConfig::Instance().GetMergeServerId();
	for (int i = 0; i < m_top_rank_num
		&& i < ARRAY_ITEM_COUNT(m_top_list); ++i)
	{
		crossgameprotocal::GameHiddenSyncPartnerTopRankInfo::RankInfo rankinfo;
		rankinfo.owner_uid = m_top_list[i].owner_uid;
		rankinfo.partner_id = m_top_list[i].partner_id;
		msg.rank_list.push_back(rankinfo);
	}

	InternalComm::Instance().SendSerializeMsgToHiddenThroughCross(&msg);
}
