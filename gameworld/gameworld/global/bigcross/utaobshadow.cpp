#include "utaobshadow.hpp"
#include "obj/character/role.h"
#include "servercommon/errornum.h"
#include "servercommon/userprotocal/msgbattle.h"
#include "engineadapter.h"
#include "utaobfightdata.hpp"
#include "servercommon/internalprotocal/bigcrosshgameprotocal.h"
#include "world.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "global/team/team.hpp"

UTAOBShadow& UTAOBShadow::Instance()
{
	static UTAOBShadow instance;
	return instance;
}

void UTAOBShadow::Update(unsigned int now)
{
	this->CheckBattleExpire(now);
}

void UTAOBShadow::StartOB(Role* role, long long schedule_id)
{
	const UTAOBFightData* data = this->GetFightData(schedule_id);
	if (NULL == data)
	{
		if (!this->IsFightEnd(schedule_id))
		{
			role->NoticeNum(errornum::EN_UTA_OB_DATA_EMPTY);
		}
		else
		{
			role->NoticeNum(errornum::EN_UTA_OB_FIGHT_ALREADY_END);
		}
		return;
	}

	Team* team = role->GetMyTeam();
	if (NULL != team && !team->IsLeader(role) && team->IsRoleMemberFollow(role->GetUID()))
	{
		role->NoticeNum(errornum::EN_UTA_OB_IN_TEAM_LIMIT);
		return;
	}

	m_ob_map[role->GetUID()] = schedule_id;
	data->OnRoleStartOB(role);
}

void UTAOBShadow::StopOB(Role* role)
{
	m_ob_map.erase(role->GetUID());

	Protocol::SCBattleEndForAudience msg;
	EngineAdapter::Instance().NetSend(role->GetNetId(), &msg, sizeof(msg));
}

void UTAOBShadow::OnSyncRoundResultData(bigchgameprotocol::BigCHGameUTASyncRoundResultForOB* msg)
{
	UNSTD_STATIC_CHECK(sizeof(msg->data_buffer) > sizeof(Protocol::SCBattleRoundResult));

	Protocol::SCBattleRoundResult* round_result_msg = (Protocol::SCBattleRoundResult*)msg->data_buffer;
	Protocol::SCBattleRoundPhase phase_msg;
	memset(phase_msg.is_second_move_avaliable, 0, sizeof(phase_msg.is_second_move_avaliable));
	phase_msg.round = round_result_msg->round;
	phase_msg.next_round_phase_time = 0u;
	phase_msg.round_phase = ROUND_PHASE_WATCH_ANIMATION;

	ObserverMap::iterator it = m_ob_map.begin();
	for (; it != m_ob_map.end(); ++it)
	{
		if (it->second != msg->schedule_id) continue;
		
		int role_id = it->first;
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
		if (NULL == role) continue;
		
		// 这里是特殊处理，就是要这样发客户端才能正常轮播战报（模拟普通观战）
		phase_msg.round_phase = ROUND_PHASE_COM_CALCULATE;
		EngineAdapter::Instance().NetSend(role->GetNetId(), &phase_msg, sizeof(phase_msg));
		
		EngineAdapter::Instance().NetSend(role->GetNetId(), msg->data_buffer, msg->data_length);
		
		phase_msg.round_phase = ROUND_PHASE_WATCH_ANIMATION;
		EngineAdapter::Instance().NetSend(role->GetNetId(), &phase_msg, sizeof(phase_msg));
	}

	UTAOBFightData* fightdata = this->MutableFightData(msg->schedule_id);
	if (NULL != fightdata)
	{
		fightdata->SetFightResultData(msg->data_buffer, msg->data_length);
	}
}

void UTAOBShadow::OnSyncBattleInfoData(bigchgameprotocol::BigCHGameUTASyncBattleInfoForOB* msg)
{
	UNSTD_STATIC_CHECK(sizeof(msg->sc_battle_info_data) > sizeof(Protocol::SCBattleInfo));
	UNSTD_STATIC_CHECK(sizeof(msg->sc_buff_list_data) > sizeof(Protocol::MsgCharacterBuffInfo) * 20 * 5);
	UNSTD_STATIC_CHECK(sizeof(msg->sc_special_effect_list_data) > sizeof(Protocol::MsgCharacterSpecialEffect) * 20 * 10);
	UNSTD_STATIC_CHECK(sizeof(msg->sc_halo_list_data) > sizeof(Protocol::MsgCharacterHalo) * 20 * 1);

	UTAOBFightData* data = this->MutableFightData(msg->schedule_id);
	if (NULL == data)
	{
		UTAOBFightData* tmp = new UTAOBFightData();
		m_fight_data_map.insert(FightDataMap::value_type(msg->schedule_id, tmp));

		data = this->MutableFightData(msg->schedule_id);
		assert(data != NULL);
		if (NULL == data) return;
	}

	data->SetData(msg->length_sc_battle_info, msg->sc_battle_info_data, 
		msg->length_sc_buff_list, msg->sc_buff_list_data, 
		msg->length_sc_special_effect_list, msg->sc_special_effect_list_data, 
		msg->length_sc_halo_list, msg->sc_halo_list_data);
}

void UTAOBShadow::OnBattleEndStopOB(long long schedule_id)
{
	std::vector<int> erase_vec;

	ObserverMap::iterator it = m_ob_map.begin();
	for (; it != m_ob_map.end(); ++it)
	{
		if (it->second != schedule_id) continue;

		erase_vec.push_back(it->first);

		int role_id = it->first;
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
		if (NULL == role) continue;

		Protocol::SCBattleEndForAudience msg;
		EngineAdapter::Instance().NetSend(role->GetNetId(), &msg, sizeof(msg));
	}

	for (size_t i = 0; i < erase_vec.size(); ++i)
	{
		m_ob_map.erase(erase_vec[i]);
	}

	this->RemoveFightData(schedule_id);
	this->AddEndedFightToSet(schedule_id);
}

void UTAOBShadow::OnDisconnectFromBigCrossServer()
{
	this->StopAllOBAndRemoveAllFightData();
}

void UTAOBShadow::OnDisconnectFromCrossServer()
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	this->StopAllOBAndRemoveAllFightData();
}

bool UTAOBShadow::IsFightEnd(long long schedule_id)
{
	return m_ended_fight_set.find(schedule_id) != m_ended_fight_set.end();
}

const UTAOBFightData* UTAOBShadow::GetFightData(long long schedule_id)
{
	return this->MutableFightData(schedule_id);
}

bool UTAOBShadow::IsRoleOBIng(Role* role)
{
	return m_ob_map.find(role->GetUID()) != m_ob_map.end();
}

UTAOBShadow::~UTAOBShadow()
{
	this->Release();
}

UTAOBShadow::UTAOBShadow()
{

}

void UTAOBShadow::Release()
{
	FightDataMap::iterator it = m_fight_data_map.begin();
	for (; it != m_fight_data_map.end(); ++it)
	{
		if (NULL != it->second)
		{
			delete it->second;
			it->second = NULL;
		}
	}

	m_fight_data_map.clear();
}

void UTAOBShadow::CheckBattleExpire(unsigned int now)
{
	std::vector<long long> expire_list;

	FightDataMap::iterator it = m_fight_data_map.begin();
	for (; it != m_fight_data_map.end(); ++it)
	{
		if (now > it->second->GetCreateTimestamp() + 30 * 60)
		{
			expire_list.push_back(it->first);
		}
	}

	for (size_t i = 0; i < expire_list.size(); ++i)
	{
		m_fight_data_map.erase(expire_list[i]);
	}
}

UTAOBFightData* UTAOBShadow::MutableFightData(long long schedule_id)
{
	FightDataMap::iterator it = m_fight_data_map.find(schedule_id);
	if (it != m_fight_data_map.end())
	{
		return it->second;
	}

	return NULL;
}

void UTAOBShadow::RemoveFightData(long long schedule_id)
{
	FightDataMap::iterator it = m_fight_data_map.find(schedule_id);
	if (it != m_fight_data_map.end())
	{
		delete it->second;
		m_fight_data_map.erase(it);
	}
}

void UTAOBShadow::StopAllOBAndRemoveAllFightData()
{
	{
		ObserverMap::iterator it = m_ob_map.begin();
		for (; it != m_ob_map.end(); ++it)
		{
			int role_id = it->first;
			Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
			if (NULL == role) continue;

			Protocol::SCBattleEndForAudience msg;
			EngineAdapter::Instance().NetSend(role->GetNetId(), &msg, sizeof(msg));
		}
		m_ob_map.clear();
	}
	
	{
		FightDataMap::iterator it = m_fight_data_map.begin();
		for (; it != m_fight_data_map.end(); ++it)
		{
			if (NULL != it->second)
			{
				delete it->second;
				it->second = NULL;
			}
		}

		m_fight_data_map.clear();
	}	
}

void UTAOBShadow::AddEndedFightToSet(long long schedule_id)
{
	m_ended_fight_set.insert(schedule_id);
}

