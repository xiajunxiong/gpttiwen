#include "utaobfightdata.hpp"
#include "servercommon/servercommon.h"
#include "engineadapter.h"
#include "obj/character/role.h"
#include "servercommon/serverclmempool.h"
#include "servercommon/userprotocal/msgbattle.h"
#include "gamelog.h"

UTAOBFightData::UTAOBFightData() : m_create_timestamp(0u)
{
	m_create_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
}

UTAOBFightData::~UTAOBFightData()
{
	this->Release();
}

bool UTAOBFightData::IsReadyForOB() const
{
	return m_cur_round_data.IsReadyForOB(); // || m_last_round_data.IsReadyForOB();
}

void UTAOBFightData::OnRoleStartOB(Role* role) const
{
	//if (m_last_round_data.IsReadyForOB())
	//{
	//	this->SendRoundResultData(role, m_last_round_data);
	//	if (m_cur_round_data.IsReadyForOB())
	//	{
	//		this->SendRoundResultData(role, m_cur_round_data, false);
	//	}
	//}
	//else 

	if (m_cur_round_data.IsReadyForOB())
	{
		this->SendRoundResultData(role, m_cur_round_data);
	}	
}

void UTAOBFightData::SetData(int sc_battle_info_len, const char* sc_battle_info, int sc_buff_list_len, const char* sc_buff_list, int sc_special_effect_list_len, const char* sc_special_effect_list, int sc_halo_list_len, const char* sc_halo_list)
{
	if (sc_battle_info_len <= 0 || sc_buff_list_len <= 0 || sc_special_effect_list_len <= 0 || sc_halo_list_len <= 0)
	{
		assert(0);
		return;
	}

	if (NULL == sc_battle_info || NULL == sc_buff_list || NULL == sc_halo_list || NULL == sc_special_effect_list)
	{
		assert(0);
		return;
	}

	//if (m_cur_round_data.IsReadyForOB())
	//{
	//	this->TransferData(m_cur_round_data, m_last_round_data);
	//}

	// 设置当前回合数据
	this->SetDataToRoundData(m_cur_round_data, 
		sc_battle_info_len, sc_battle_info, 
		sc_buff_list_len, sc_buff_list, 
		sc_special_effect_list_len, sc_special_effect_list, 
		sc_halo_list_len, sc_halo_list);
}

void UTAOBFightData::SetFightResultData(const char* msg, int length)
{
	if (NULL == msg || length <= 0)
	{
		assert(0);
		return;
	}

	this->SetFightResultToRoundData(m_cur_round_data, msg, length);
}

void UTAOBFightData::Release()
{
	this->ReleaseRoundData(m_cur_round_data);
	this->ReleaseFightResultData(m_cur_round_data);
	//this->ReleaseRoundData(m_last_round_data);
	//this->ReleaseFightResultData(m_last_round_data);
}

void UTAOBFightData::ReleaseRoundData(UTAOBRoundData& round_data)
{
	if (NULL != round_data.sc_battle_info)
	{
		ServerMsgMem *tmp = (ServerMsgMem*)round_data.sc_battle_info;
		delete[]tmp;
		round_data.sc_battle_info = NULL;
		round_data.length_sc_battle_info = 0;
	}

	if (NULL != round_data.sc_buff_list)
	{
		ServerMsgMem *tmp = (ServerMsgMem*)round_data.sc_buff_list;
		delete[]tmp;
		round_data.sc_buff_list = NULL;
		round_data.length_sc_buff_list = 0;
	}

	if (NULL != round_data.sc_special_effect_list)
	{
		ServerMsgMem *tmp = (ServerMsgMem*)round_data.sc_special_effect_list;
		delete[]tmp;
		round_data.sc_special_effect_list = NULL;
		round_data.length_sc_special_effect_list = 0;
	}

	if (NULL != round_data.sc_halo_list)
	{
		ServerMsgMem *tmp = (ServerMsgMem*)round_data.sc_halo_list;
		delete[]tmp;
		round_data.sc_halo_list = NULL;
		round_data.length_sc_halo_list = 0;
	}
}

void UTAOBFightData::ReleaseFightResultData(UTAOBRoundData& round_data)
{
	if (NULL != round_data.sc_fight_result[0])
	{
		ServerMsgMem *tmp = (ServerMsgMem*)round_data.sc_fight_result[0];
		delete[]tmp;
		round_data.sc_fight_result[0] = NULL;
		round_data.length_fight_result[0] = 0;
	}

	if (NULL != round_data.sc_fight_result[1])
	{
		ServerMsgMem *tmp = (ServerMsgMem*)round_data.sc_fight_result[1];
		delete[]tmp;
		round_data.sc_fight_result[1] = NULL;
		round_data.length_fight_result[1] = 0;
	}
}

void UTAOBFightData::SendRoundResultData(Role* role, const UTAOBRoundData& round_data, bool is_send_battle_info /* = true */) const
{
	if (is_send_battle_info)
	{
#ifdef _MSC_VER
		gamelog::g_log_world.printf(LL_MAINTANCE, "SendRoundResultData SC_BATTLE_INFO");
#endif
		EngineAdapter::Instance().NetSend(role->GetNetId(), round_data.sc_battle_info, round_data.length_sc_battle_info);
		EngineAdapter::Instance().NetSend(role->GetNetId(), round_data.sc_buff_list, round_data.length_sc_buff_list);
		EngineAdapter::Instance().NetSend(role->GetNetId(), round_data.sc_special_effect_list, round_data.length_sc_special_effect_list);
		EngineAdapter::Instance().NetSend(role->GetNetId(), round_data.sc_halo_list, round_data.length_sc_halo_list);
	}

	if (round_data.sc_fight_result[0] != NULL)
	{
//#ifdef _MSC_VER
//		Protocol::SCBattleRoundResult* result = (Protocol::SCBattleRoundResult*)round_data.sc_fight_result[0];
//		gamelog::g_log_world.printf(LL_MAINTANCE, "SendRoundResultData SC_FIGHT_RESULT[0] length[%d] round[%d]", round_data.length_fight_result[0], result->round);
//#endif

//		this->SendFightResultData(role, round_data.sc_fight_result[0], round_data.length_fight_result[0]);
		
		// PS: 0不发，仅做判断，因为客户端无法处理同一回合两次战报的问题（准备阶段和战斗阶段都会有战报）
		
		// 只有0满足了才有可能满足1
		if (round_data.sc_fight_result[1] != NULL)
		{
#ifdef _MSC_VER
			Protocol::SCBattleRoundResult* result = (Protocol::SCBattleRoundResult*)round_data.sc_fight_result[1];
			gamelog::g_log_world.printf(LL_MAINTANCE, "SendRoundResultData SC_FIGHT_RESULT[1] length[%d] round[%d]", round_data.length_fight_result[1], result->round);
#endif

			this->SendFightResultData(role, round_data.sc_fight_result[1], round_data.length_fight_result[1]);
		}
	}
}

void UTAOBFightData::SendFightResultData(Role* role, const char* msg, int length) const
{
	if (NULL == msg || length <= 0) return;

	Protocol::SCBattleRoundResult* round_result_msg = (Protocol::SCBattleRoundResult*)msg;
	Protocol::SCBattleRoundPhase phase_msg;
	memset(phase_msg.is_second_move_avaliable, 0, sizeof(phase_msg.is_second_move_avaliable));
	phase_msg.round = round_result_msg->round;
	phase_msg.next_round_phase_time = 0u;
	phase_msg.round_phase = ROUND_PHASE_WATCH_ANIMATION;

	// 这里是特殊处理，就是要这样发客户端才能正常轮播战报（模拟普通观战）
	phase_msg.round_phase = ROUND_PHASE_COM_CALCULATE;
	EngineAdapter::Instance().NetSend(role->GetNetId(), &phase_msg, sizeof(phase_msg));

	EngineAdapter::Instance().NetSend(role->GetNetId(), msg, length);

	phase_msg.round_phase = ROUND_PHASE_WATCH_ANIMATION;
	EngineAdapter::Instance().NetSend(role->GetNetId(), &phase_msg, sizeof(phase_msg));
}

void UTAOBFightData::SetDataToRoundData(UTAOBRoundData& round_data, int sc_battle_info_len, const char* sc_battle_info, int sc_buff_list_len, const char* sc_buff_list, int sc_special_effect_list_len, const char* sc_special_effect_list, int sc_halo_list_len, const char* sc_halo_list)
{
	this->ReleaseRoundData(round_data);
	this->ReleaseFightResultData(round_data);

	round_data.sc_battle_info = (char *)(new ServerMsgMem[sc_battle_info_len]);
	memcpy(round_data.sc_battle_info, sc_battle_info, sc_battle_info_len);
	round_data.length_sc_battle_info = sc_battle_info_len;

	round_data.sc_buff_list = (char *)(new ServerMsgMem[sc_buff_list_len]);
	memcpy(round_data.sc_buff_list, sc_buff_list, sc_buff_list_len);
	round_data.length_sc_buff_list = sc_buff_list_len;

	round_data.sc_special_effect_list = (char *)(new ServerMsgMem[sc_special_effect_list_len]);
	memcpy(round_data.sc_special_effect_list, sc_special_effect_list, sc_special_effect_list_len);
	round_data.length_sc_special_effect_list = sc_special_effect_list_len;

	round_data.sc_halo_list = (char *)(new ServerMsgMem[sc_halo_list_len]);
	memcpy(round_data.sc_halo_list, sc_halo_list, sc_halo_list_len);
	round_data.length_sc_halo_list = sc_halo_list_len;
}

void UTAOBFightData::SetFightResultToRoundData(UTAOBRoundData& round_data, const char* msg, int length)
{
	if (NULL == round_data.sc_fight_result[0])
	{
		round_data.sc_fight_result[0] = (char *)(new ServerMsgMem[length]);
		memcpy(round_data.sc_fight_result[0], msg, length);
		round_data.length_fight_result[0] = length;
	}
	else if (NULL == round_data.sc_fight_result[1])
	{
		round_data.sc_fight_result[1] = (char *)(new ServerMsgMem[length]);
		memcpy(round_data.sc_fight_result[1], msg, length);
		round_data.length_fight_result[1] = length;
	}
	else
	{
		assert(0);
	}
}

void UTAOBFightData::TransferData(UTAOBRoundData& source, UTAOBRoundData& target)
{
	// 把当前回合数据先转移到上回合
	this->SetDataToRoundData(target,
		source.length_sc_battle_info, source.sc_battle_info,
		source.length_sc_buff_list, source.sc_buff_list,
		source.length_sc_special_effect_list, source.sc_special_effect_list,
		source.length_sc_halo_list, source.sc_halo_list);
	this->SetFightResultToRoundData(target, source.sc_fight_result[0], source.length_fight_result[0]);
	this->SetFightResultToRoundData(target, source.sc_fight_result[1], source.length_fight_result[1]);
}

bool UTAOBRoundData::IsReadyForOB() const
{
	return length_sc_battle_info > 0
		&& length_sc_buff_list > 0
		&& length_sc_halo_list > 0
		&& length_sc_special_effect_list > 0;
}
