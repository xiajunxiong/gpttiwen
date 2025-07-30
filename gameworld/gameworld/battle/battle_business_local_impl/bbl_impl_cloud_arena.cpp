#include "bbl_impl_cloud_arena.hpp"
#include "obj/character/role.h"
#include "global/cloudarena/cloudarena.hpp"

BBLImplCloudArena::BBLImplCloudArena() : m_attacker_uid(0)
{

}

BBLImplCloudArena::~BBLImplCloudArena()
{

}

void BBLImplCloudArena::OnBattlePrepare(int battle_mode, int role_count, Role** role_list, RoleBattleData** role_data_list)
{
	if (role_count > 0)
	{
		m_attacker_uid = role_list[0]->GetUID();
		m_attacker_battle_data = *role_data_list[0];
	}
}

void BBLImplCloudArena::OnBattleStart(battlegameprotocol::BattleGameStartFightAck* ack)
{
	CloudArena::Instance().OnFightStart(m_attacker_uid);
}

void BBLImplCloudArena::OnBattleFinish(battlegameprotocol::BattleGameFightResultAck* ack)
{
	bool is_win = RESULT_TYPE_ATTACKER_WIN == ack->battle_result;
	bool is_draw = 
		RESULT_TYPE_DRAW == ack->battle_result || 
		RESULT_TYPE_EXCEED_MAX_ROUND == ack->battle_result || 
		RESULT_TYPE_EXCEED_MAX_TIME == ack->battle_result;

	CloudArena::Instance().OnFightFinish(m_attacker_uid, ack->business_data.cloud_arena_target_uid, is_win, is_draw, m_attacker_battle_data, ack->statistic_data);
}

void BBLImplCloudArena::OnBattleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack)
{
	CloudArena::Instance().OnFightFinish(m_attacker_uid, ack->business_data.cloud_arena_target_uid, false, false, m_attacker_battle_data, ack->statistic_data);
}

void BBLImplCloudArena::OnBattleForceFinish(battlegameprotocol::BattleGameForceFinishFight* ack)
{
	CloudArena::Instance().OnFightFinish(m_attacker_uid, ack->business_data.cloud_arena_target_uid, false, false, m_attacker_battle_data, ack->statistic_data);
}

