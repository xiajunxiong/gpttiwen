#include "story.hpp"
#include "engineadapter.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "server/protocal/msglogin.h"
#include "server/loginlog.h"

Story& Story::Instance()
{
	static Story inst;
	return inst;
}

void Story::OnStoryBattleInfoReq(NetID netid)
{
	// 暂时未有剧情数据
	if (m_battle_info_msg.battle_mode != BATTLE_MODE_STORY)
	{
		static Protocol::SCStoryNotAvaliable notice;
		EngineAdapter::Instance().NetSend(netid, (const char*)&notice, sizeof(notice));
		
		loginlog::g_log_login.printf(LL_MAINTANCE, "OnStoryBattleInfoReq no battle_info_msg");
		return;
	}
		

	int send_len = sizeof(m_battle_info_msg) - sizeof(m_battle_info_msg.character_info_list) + m_battle_info_msg.character_num * sizeof(m_battle_info_msg.character_info_list[0]);

	EngineAdapter::Instance().NetSend(netid, (const char*)&m_battle_info_msg, send_len);
}

void Story::OnStoryRoundResultReq(NetID netid, int round)
{
	// 客户端的请求从0开始
	if (round < 0 || round >= BATTLE_STORY_ROUND_NUM) return;

	// 暂时未有剧情数据
	if (m_result_msg_list[round].tree_num <= 0) 
	{
		static Protocol::SCStoryNotAvaliable notice;
		EngineAdapter::Instance().NetSend(netid, (const char*)&notice, sizeof(notice));

		loginlog::g_log_login.printf(LL_MAINTANCE, "OnStoryBattleInfoReq no round_result");
		return;
	}

	int send_len = sizeof(m_result_msg_list[round]) - sizeof(m_result_msg_list[round].result_buffer) + m_result_msg_list[round].total_data_len;
	EngineAdapter::Instance().NetSend(netid, (const char*)&m_result_msg_list[round], send_len);

	Protocol::SCBattleRoundPhase phase;
	memset(phase.is_second_move_avaliable, 0, sizeof(phase.is_second_move_avaliable));
	phase.next_round_phase_time = 0;
	phase.round = round + 1;
	phase.round_phase = ROUND_PHASE_WATCH_ANIMATION;

	EngineAdapter::Instance().NetSend(netid, (const char*)&phase, sizeof(phase));
}

void Story::OnSyncBattleInfoMsg(const char* data, int length)
{
	if (length < static_cast<int>(sizeof(m_battle_info_msg)) - static_cast<int>(sizeof(m_battle_info_msg.character_info_list)))
	{
#ifdef _DEBUG
		assert(0);
#endif
		return;
	}

	int cur_len = sizeof(m_battle_info_msg) - sizeof(m_battle_info_msg.character_info_list);
	memcpy(&m_battle_info_msg, data, cur_len);
	
	Protocol::SCBattleInfo* msg = (Protocol::SCBattleInfo*)data;
	for (int i = 0; i < msg->character_num; ++i)
	{
		if (length < cur_len + static_cast<int>(sizeof(msg->character_info_list[0])))
		{
#ifdef _DEBUG
			assert(0);
#endif
			break;
		}

		m_battle_info_msg.character_info_list[i] = msg->character_info_list[i];
		cur_len += static_cast<int>(sizeof(msg->character_info_list[0]));
	}
}

void Story::OnSyncRoundResultMsg(int round, const char* data, int length)
{
	if (round <= 0 || round > BATTLE_STORY_ROUND_NUM) return;

	Protocol::SCBattleRoundResult& msg = m_result_msg_list[round - 1];

	if (length < static_cast<int>(sizeof(msg)) - static_cast<int>(sizeof(msg.result_buffer)))
	{
#ifdef _DEBUG
		assert(0);
#endif
		return;
	}

	int cur_len = sizeof(msg) - sizeof(msg.result_buffer);
	memcpy(&msg, data, cur_len);

	if (cur_len + msg.total_data_len != length)
	{
#ifdef _DEBUG
		assert(0);
#endif
		return;
	}

	memcpy(msg.result_buffer, data + cur_len, msg.total_data_len);

	//printf("total_data_len=%d\n", msg.total_data_len);
	//for (size_t i = 0; i < msg.total_data_len; ++i)
	//{
	//	printf("%d ", msg.result_buffer[i]);
	//}
	//printf("\n\n\n");
}

Story::Story()
{

}

Story::~Story()
{

}

