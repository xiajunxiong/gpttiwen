#include<algorithm>

#include "world.h"
#include "gamelog.h"
#include "scene/scenemanager.hpp"
#include "obj/character/role.h"
#include "engineadapter.h"
#include "global/usercache/usercache.hpp"
#include "global/worldstatus/worldstatus.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"
#include "item/itempool.h"
#include "item/itembase.h"
#include "item/money.h"
#include "other/rolemodulemanager.hpp"
#include "global/activity/activitymanager.hpp"
#include "battle/battle_manager_local.hpp"
#include "global/topicmanager/topicmanager.hpp"
#include "internalcomm.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/activityconfig/chiefpeakbattle/activitychiefpeakbattlenconfig.hpp"
#include "scene/speciallogicimpl/speciallogicchiefpeakbattle.hpp"
#include "protocol/msgchiefpeakbattle.h"
#include "activitychiefpeakbattle.hpp"
#include "other/funopen/funopenconfig.hpp"
#include "other/funopen/funopendef.hpp"
#include "other/event/eventhandler.hpp"


int ActivityChiefPeakBattle::FIGHTER_NUMBER[ACTIVITY_CHIEF_PEAK_BATTLE_FIGTHER_NUM] = { 0 };
int ActivityChiefPeakBattle::m_seq = 1;

ActivityChiefPeakBattle::ActivityChiefPeakBattle(ActivityManager * activity_manager) : Activity(activity_manager, ACTIVITY_TYPE_CHIEF_PEAK_BATTLE)
{
	for (int i = 0; i < ARRAY_LENGTH(FIGHTER_NUMBER); ++i)
	{
		FIGHTER_NUMBER[i] = i + 1;
	}
}

ActivityChiefPeakBattle::~ActivityChiefPeakBattle()
{
}

void ActivityChiefPeakBattle::OnUserLogin(Role * role)
{
	Activity::OnUserLogin(role);
	m_match_info.SendNoticeToRole(role);

	if (!CrossConfig::Instance().IsHiddenServer() && this->IsActivityOpen())
	{
		UniqueUserID uuid = role->GetUniqueUserID();
		crossgameprotocal::GameHiddenChiefPeakBattleReq send_hidden_req;
		send_hidden_req.origin_plat_type = uuid.plat_type;
		send_hidden_req.origin_server_id = LocalConfig::Instance().GetMergeServerId();
		send_hidden_req.origin_role_id = UidToInt(uuid.user_id);
		send_hidden_req.req_type = Protocol::CHIEF_PEAK_BATTLE_REQ_GET_NOTICE;
		send_hidden_req.p1 = 0;
		send_hidden_req.p2 = 0;
		InternalComm::Instance().SendToHiddenThroughCross((const char *)&send_hidden_req, sizeof(send_hidden_req));
	}
}

void ActivityChiefPeakBattle::OnActivityStatusChange(int from_status, int to_status)
{
	Activity::OnActivityStatusChange(from_status, to_status);

	switch (to_status)
	{
		case ACTIVITY_STATUS_CLOSE:
			{
				BattleManagerLocal::GetInstance().ForceFinishBattleMode(BATTLE_MODE_CHIEF_PEAK_BATTLE);
				m_match_info.OnActivityClose();
				this->ClearData();
			}
			break;
		case ACTIVITY_STATUS_STANDY:
			{
				WorldStatus::Instance().ClearPeakBattleInfo();

				this->ClearData();
				m_match_info.CalTopFigther();
			}
			break;

		case ACTIVITY_STATUS_OPEN:
			{
				m_match_info.m_next_state_time = EngineAdapter::Instance().Time();
			}
			break;
	}
}

void ActivityChiefPeakBattle::Update(unsigned long interval, time_t now_second)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	Activity::Update(interval, now_second);

	this->UpdateMatchInfo(now_second);
}

void ActivityChiefPeakBattle::OnBattleServerDisconnect(int battle_server_id)
{
	if (this->IsActivityOpen())
	{
		this->ForceToNextState();
	}
}

void ActivityChiefPeakBattle::SendRoleInfo(Role * role)
{
	if (role == NULL)
	{
		return;
	}

	Protocol::SCChiefPeakBattleRoleInfo protocol;
	ActivityRoleData * role_data = this->GetRoleData(role->GetUID(), false);
	if (role_data == NULL)
	{
		return;
	}
	const ActivityChiefPeakBattleRoleData & peak_battle_role_data = role_data->chief_peak_battle_role_data;

	protocol.bet_type = peak_battle_role_data.bet_type;
	protocol.role_id = peak_battle_role_data.bet_role_id;

	EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
}

void ActivityChiefPeakBattle::SendMatchInfo(Role * role)
{
	if (role == NULL)
	{
		return;
	}

	Protocol::SCChiefPeakBattleMatchInfo protocol;
	protocol.curr_round = m_match_info.curr_match_round;
	protocol.round_state = m_match_info.round_state;
	protocol.end_time = (unsigned int)m_match_info.m_next_state_time;
	protocol.is_finish = (unsigned int)m_match_info.is_match_finish;

	for (int i = 0; i < ARRAY_LENGTH(m_match_info.figther_info) && i < ARRAY_LENGTH(protocol.match_info); ++i)
	{
		protocol.match_info[i].role_id = m_match_info.figther_info[i].role_id;
		protocol.match_info[i].match_round = m_match_info.figther_info[i].match_round;
		protocol.match_info[i].state = m_match_info.figther_info[i].state;

		UserCacheNode * user_cache_node = UserCacheManager::Instance().GetUserNode(m_match_info.figther_info[i].role_id);
		if (NULL != user_cache_node)
		{
			user_cache_node->GetName(protocol.match_info[i].role_name);
			protocol.match_info[i].level = user_cache_node->level;
			protocol.match_info[i].avatar_type = user_cache_node->avatar_type;
			protocol.match_info[i].headshot_id = user_cache_node->appearance.headshot_id;
		}
	}

	EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
}

void ActivityChiefPeakBattle::Bet(Role * role, int bet_type, int figther_index)
{
	if (role == NULL || figther_index < 0 || figther_index >= ACTIVITY_CHIEF_PEAK_BATTLE_FIGTHER_NUM)
	{
		return;
	}

	if (m_match_info.figther_info[figther_index].role_id == 0)
	{
		return;
	}

	const ChiefPeakBattleBetCfg * bet_cfg = LOGIC_CONFIG->GetActivityChiefPeakBattleConfig()->GetChiefPeakBattleBetCfg(bet_type);
	if (bet_cfg == NULL)
	{
		return;
	}

	ActivityRoleData * role_data = Activity::GetRoleData(role->GetUID());
	if (role_data == NULL)
	{
		return;
	}

	if (role_data->chief_peak_battle_role_data.bet_role_id != 0)
	{
		return;
	}

	if (!role->GetRoleModuleManager()->GetMoney()->UseCoinBind(bet_cfg->bet_coin, __FUNCTION__))
	{
		return;
	}

	role_data->chief_peak_battle_role_data.bet_role_id = m_match_info.figther_info[figther_index].role_id;
	role_data->chief_peak_battle_role_data.bet_type = bet_type;
	Activity::SetRoleDataChange(role->GetUID());
}

void ActivityChiefPeakBattle::GotoReadyScene(Role * role)
{
	m_match_info.GotoReadyMatchScene(role);
}

void ActivityChiefPeakBattle::HiddenToGameSendRoleInfo(int role_id)
{
	Protocol::SCChiefPeakBattleRoleInfo protocol;
	ActivityRoleData * role_data = this->GetRoleData(role_id, false);
	if (role_data == NULL)
	{
		return;
	}
	const ActivityChiefPeakBattleRoleData & peak_battle_role_data = role_data->chief_peak_battle_role_data;

	protocol.bet_type = peak_battle_role_data.bet_type;
	protocol.role_id = peak_battle_role_data.bet_role_id;

	InternalComm::Instance().SendToGameUserThroughCross(role_id, &protocol, sizeof(protocol));
}

void ActivityChiefPeakBattle::HiddenToGameSendMatchInfo(int role_id)
{
	Protocol::SCChiefPeakBattleMatchInfo protocol;
	protocol.curr_round = m_match_info.curr_match_round;
	protocol.round_state = m_match_info.round_state;
	protocol.end_time = (unsigned int)m_match_info.m_next_state_time;
	protocol.is_finish = (unsigned int)m_match_info.is_match_finish;

	for (int i = 0; i < ARRAY_LENGTH(m_match_info.figther_info) && i < ARRAY_LENGTH(protocol.match_info); ++i)
	{
		protocol.match_info[i].role_id = m_match_info.figther_info[i].role_id;
		protocol.match_info[i].match_round = m_match_info.figther_info[i].match_round;
		protocol.match_info[i].state = m_match_info.figther_info[i].state;

		UserCacheNode * user_cache_node = UserCacheManager::Instance().GetUserNode(m_match_info.figther_info[i].role_id);
		if (NULL != user_cache_node)
		{
			user_cache_node->GetName(protocol.match_info[i].role_name);
			protocol.match_info[i].level = user_cache_node->level;
			protocol.match_info[i].avatar_type = user_cache_node->avatar_type;
			protocol.match_info[i].headshot_id = user_cache_node->appearance.headshot_id;
		}
	}

	InternalComm::Instance().SendToGameUserThroughCross(role_id, &protocol, sizeof(protocol));
}

void ActivityChiefPeakBattle::HiddenToGameSendNoticeToRole(int role_id)
{
	m_match_info.HiddenToGameSendNoticeToRoleById(role_id);
}

void ActivityChiefPeakBattle::SetFightResult(int role_id, bool is_win)
{
	m_match_info.SetFightResult(role_id, is_win, true);
}

void ActivityChiefPeakBattle::OnRoleLeaveMatch(int role_id)
{
	m_match_info.OnRoleLeaveMatch(role_id);
}

void ActivityChiefPeakBattle::SendBetReward(int champion_role_id)
{
	if (champion_role_id != 0)
	{
		RetActivityRoleData get_role_data;
		this->BeginRoleDataForeach();
		while (this->GetNextRoleData(get_role_data))
		{
			const ActivityChiefPeakBattleRoleData & role_data = get_role_data.data.chief_peak_battle_role_data;
			if (role_data.bet_role_id == champion_role_id)
			{
				const ChiefPeakBattleBetCfg * reward_cfg = LOGIC_CONFIG->GetActivityChiefPeakBattleConfig()->GetChiefPeakBattleBetCfg(role_data.bet_type);
				if (reward_cfg != NULL)
				{
					// 发邮件
					static MailContentParam contentparam; contentparam.Reset();

					contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = reward_cfg->bet_coin;

					int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_activity_chief_peak_battle_bet_reward_subject);
					int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_activity_chief_peak_battle_bet_reward_content);
					if (lensub > 0 && length > 0) MailRoute::MailToUser(get_role_data.role_id, MAIL_REASON_DEFAULT, contentparam);
				}
			}
		}
	}
}


bool ActivityChiefPeakBattle::IsJoinFighter(int uid, int ARG_OUT * out_index)
{
	for (int figther_index = 0; figther_index < ACTIVITY_CHIEF_PEAK_BATTLE_FIGTHER_NUM; ++figther_index)
	{
		if (uid == m_match_info.figther_info[figther_index].role_id)
		{
			if (out_index != NULL)
			{
				*out_index = figther_index;
			}

			return true;
		}
	}

	return false;
}

void ActivityChiefPeakBattle::UpdateMatchInfo(time_t now_second)
{
	if (!this->IsActivityOpen())
	{
		return;
	}

	m_match_info.UpdateMatchInfo(now_second);
}

void ActivityChiefPeakBattle::ClearData()
{
	m_match_info = ActivityChiefPeakBattleMatchInfo();
	Activity::ClearAllRoleData();
}

int ActivityChiefPeakBattle::GetMatchSeq()
{
	return m_seq++;
}

bool ActivityChiefPeakBattleMatchInfo::SortFuncFinish(const ActivityChiefPeakBattleFigtherInfo & item1, const ActivityChiefPeakBattleFigtherInfo & item2)
{
	if (item1.role_id == 0 && item2.role_id != 0)
		return false;
	if (item1.role_id != 0 && item2.role_id == 0)
		return true;

	if (item1.match_round > item2.match_round)
		return true;
	if (item1.match_round < item2.match_round)
		return false;

	if (item1.state != ACTIVITY_CHIEF_PEAK_BATTLE_MATCH_STATE_LOSE && item2.state == ACTIVITY_CHIEF_PEAK_BATTLE_MATCH_STATE_LOSE)
		return true;
	if (item1.state == ACTIVITY_CHIEF_PEAK_BATTLE_MATCH_STATE_LOSE && item2.state != ACTIVITY_CHIEF_PEAK_BATTLE_MATCH_STATE_LOSE)
		return false;

	if (item1.role_id > item2.role_id)
		return true;
	if (item1.role_id < item2.role_id)
		return false;

	return false;
}

ActivityChiefPeakBattleMatchInfo::ActivityChiefPeakBattleMatchInfo()
{
	curr_match_round = 0;
	round_state = PEAK_BATTLE_ROUND_STATE_INVALID;
	m_next_state_time = 0;
	is_match_finish = false;
	figther_info_num = 0;
	MAX_MATCH_ROUND = 0;

	int remain = ACTIVITY_CHIEF_PEAK_BATTLE_FIGTHER_NUM;
	while (remain > 1)
	{
		MAX_MATCH_ROUND += 1;
		remain /= 2;
	}
}

void ActivityChiefPeakBattleMatchInfo::UpdateMatchInfo(time_t now_second)
{
	if (is_match_finish)
	{
		return;
	}

	if (m_next_state_time != 0 && now_second >= m_next_state_time)
	{
		round_state = (round_state + 1) % PEAK_BATTLE_ROUND_STATE_MAX;

		if (round_state == PEAK_BATTLE_ROUND_STATE_READY)
		{
			const ChiefPeakBattleOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityChiefPeakBattleConfig()->GetOhterCfg();
			int ready_duration = curr_match_round == 0 ? other_cfg.first_ready_time : other_cfg.ready_time;		//第一个回合准备进场时间长一些

			curr_match_round += 1;
			m_next_state_time = now_second + ready_duration;

			gamelog::g_Log_chief_peak.printf(LL_INFO, "%s  line:%d | start_round_ready:%d", __FUNCTION__, __LINE__, curr_match_round);
		}
		else
		{
			gamelog::g_Log_chief_peak.printf(LL_INFO, "%s  line:%d | start_round_fight:%d", __FUNCTION__, __LINE__, curr_match_round);
			m_next_state_time = now_second + ACTIVITY_CHIEF_PEAK_BATTLE_BATTLE_TIME;
			this->StartRoundFight();
		}

		this->CheckFinish();
		this->NoticeRoundInfo();
		this->SendMatchInfoUpdate();
	}
}

void ActivityChiefPeakBattleMatchInfo::NoticeRoundInfo()
{
	if (is_match_finish)
	{
		return;
	}

	Protocol::SCChiefPeakBattleNotice protocol;
	protocol.curr_round = curr_match_round;
	protocol.round_state = round_state;
	protocol.end_time = (unsigned int)m_next_state_time;

	for (int i = 0; i < ARRAY_LENGTH(figther_info); ++i)
	{
		if (figther_info[i].match_round == curr_match_round && figther_info[i].state != ACTIVITY_CHIEF_PEAK_BATTLE_MATCH_STATE_LOSE)
		{
			int role_id = figther_info[i].role_id;
			Role * tmp_role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
			if (tmp_role != NULL)
			{
				EngineAdapter::Instance().NetSend(tmp_role->GetNetId(), &protocol, sizeof(protocol));
			}
			else
			{
				InternalComm::Instance().SendToGameUserThroughCross(role_id, &protocol, sizeof(protocol));
			}
		}
	}
}

void ActivityChiefPeakBattleMatchInfo::SendNoticeToRole(Role * role)
{
	if (role == NULL || is_match_finish)
	{
		return;
	}

	Protocol::SCChiefPeakBattleNotice protocol;
	protocol.curr_round = curr_match_round;
	protocol.round_state = round_state;
	protocol.end_time = (unsigned int)m_next_state_time;

	for (int i = 0; i < ARRAY_LENGTH(figther_info); ++i)
	{
		if (figther_info[i].role_id == role->GetUID())
		{
			if (figther_info[i].match_round == curr_match_round && figther_info[i].state != ACTIVITY_CHIEF_PEAK_BATTLE_MATCH_STATE_LOSE)
			{
				EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
			}

			break;
		}
	}
}

void ActivityChiefPeakBattleMatchInfo::HiddenToGameSendNoticeToRoleById(int role_id)
{
	if (is_match_finish)
	{
		return;
	}

	Protocol::SCChiefPeakBattleNotice protocol;
	protocol.curr_round = curr_match_round;
	protocol.round_state = round_state;
	protocol.end_time = (unsigned int)m_next_state_time;

	for (int i = 0; i < ARRAY_LENGTH(figther_info); ++i)
	{
		if (figther_info[i].role_id == role_id)
		{
			if (figther_info[i].match_round == curr_match_round && figther_info[i].state != ACTIVITY_CHIEF_PEAK_BATTLE_MATCH_STATE_LOSE)
			{
				InternalComm::Instance().SendToGameUserThroughCross(role_id, &protocol, sizeof(protocol));
			}

			break;
		}
	}
}

void ActivityChiefPeakBattleMatchInfo::CheckFinish()
{
	int win_num = 0;				//比赛中获胜的人还有多少个，小于等于 1 就能结束这场比赛
	int round_role_num = 0;			//当前回合还没打完的人数
	for (int i = 0; i < ARRAY_LENGTH(figther_info); ++i)
	{
		const ActivityChiefPeakBattleFigtherInfo & info = figther_info[i];

		if (info.role_id == 0)
		{
			continue;
		}

		if (info.match_round > curr_match_round || info.state != ACTIVITY_CHIEF_PEAK_BATTLE_MATCH_STATE_LOSE)
		{
			win_num += 1;
		}

		if (info.match_round == curr_match_round && info.state != ACTIVITY_CHIEF_PEAK_BATTLE_MATCH_STATE_LOSE)
		{
			round_role_num += 1;
		}
	}

	if (win_num <= 1 && round_role_num == 0)
	{
		this->OnMatchFinish();
	}

	if (round_role_num <= 0)
	{
		m_next_state_time = EngineAdapter::Instance().Time();
	}
}

void ActivityChiefPeakBattleMatchInfo::OnActivityClose()
{
	for (int i = 0; i < ARRAY_LENGTH(figther_info); ++i)
	{
		ActivityChiefPeakBattleFigtherInfo & figher_item = figther_info[i];

		if (figher_item.role_id == 0 || figher_item.state == ACTIVITY_CHIEF_PEAK_BATTLE_MATCH_STATE_LOSE)
		{
			continue;
		}

		SpecialLogicChiefPeakBattle * special_lobgic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicChiefPeakBattle(figher_item.scene_info.scene_id, figher_item.scene_info.scene_key);
		if (special_lobgic != NULL)
		{
			special_lobgic->OnActivityClose();
		}

		figher_item.state = ACTIVITY_CHIEF_PEAK_BATTLE_MATCH_STATE_LOSE;
		this->OnMatchFinish();
	}
}

void ActivityChiefPeakBattleMatchInfo::GotoReadyMatchScene(Role * role)
{
	if (role == NULL)
	{
		return;
	}

	if (is_match_finish || round_state != PEAK_BATTLE_ROUND_STATE_READY || curr_match_round != 1)
	{
		return;
	}

	int figther_index = this->GetGetFighterIndex(role->GetUID());
	if (figther_index < 0 || figther_index >= ARRAY_LENGTH(figther_info))
	{
		return;
	}

	ActivityChiefPeakBattleFigtherInfo & self_info = figther_info[figther_index];
	if (self_info.state == ACTIVITY_CHIEF_PEAK_BATTLE_MATCH_STATE_LOSE)
	{
		return;
	}

	self_info.scene_info.scene_key = ActivityChiefPeakBattle::GetMatchSeq();
	self_info.scene_info.scene_id = LOGIC_CONFIG->GetActivityChiefPeakBattleConfig()->GetReadySceneId(self_info.match_round);

	ActivityChiefPeakBattleFigtherInfo *  opponent_info = this->GetOpponentInfo(figther_index);
	if (opponent_info != NULL)
	{
		self_info.opponent_number = opponent_info->number;
		opponent_info->opponent_number = self_info.number;

		if (opponent_info->scene_info.scene_id != 0)
		{
			self_info.scene_info = opponent_info->scene_info;
		}
	}
	WorldStatus::Instance().LogFunctionStats(role, "ActivityChiefPeakReadyMatchScene");
	WorldStatus::Instance().SendNoticeLog(role->GetUID(), role->GetName(), role->GetPlatName(), ACTIVITY_TYPE_CHIEF_PEAK_BATTLE, 0, __FUNCTION__);
	World::GetInstWorld()->GetSceneManager()->GoTo(role, self_info.scene_info.scene_id, self_info.scene_info.scene_key, Posi(0, 0));
}

void ActivityChiefPeakBattleMatchInfo::GotoMatchScene(int figther_index)
{
	if (figther_index < 0 || figther_index >= ARRAY_LENGTH(figther_info))
	{
		return;
	}

	ActivityChiefPeakBattleFigtherInfo & self_info = figther_info[figther_index];
	if (self_info.state == ACTIVITY_CHIEF_PEAK_BATTLE_MATCH_STATE_LOSE)
	{
		return;
	}


	self_info.scene_info.scene_key = ActivityChiefPeakBattle::GetMatchSeq();
	self_info.scene_info.scene_id = LOGIC_CONFIG->GetActivityChiefPeakBattleConfig()->GetReadySceneId(self_info.match_round);

	ActivityChiefPeakBattleFigtherInfo *  opponent_info = this->GetOpponentInfo(figther_index);
	if (opponent_info != NULL)
	{
		self_info.opponent_number = opponent_info->number;
		opponent_info->opponent_number = self_info.number;

		if (opponent_info->scene_info.scene_id != 0)
		{
			self_info.scene_info = opponent_info->scene_info;
		}
	}

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(self_info.role_id);
	if (role != NULL)
	{
		WorldStatus::Instance().LogFunctionStats(role, "ActivityChiefPeakMatchScene");
		World::GetInstWorld()->GetSceneManager()->GoTo(role, self_info.scene_info.scene_id, self_info.scene_info.scene_key, Posi(0, 0));
	}
}

void ActivityChiefPeakBattleMatchInfo::StartRoundFight()
{
	for (int i = 0; i < ARRAY_LENGTH(figther_info); ++i)
	{
		ActivityChiefPeakBattleFigtherInfo & figher_item = figther_info[i];

		if (figher_item.role_id == 0 || figher_item.match_round != curr_match_round || figher_item.state == ACTIVITY_CHIEF_PEAK_BATTLE_MATCH_STATE_LOSE)
		{
			continue;
		}

		gamelog::g_Log_chief_peak.printf(LL_INFO, "%s  line:%d | role_id:%d scene_id:%d scene_key:%d", __FUNCTION__, __LINE__, figher_item.role_id, figher_item.scene_info.scene_id, figher_item.scene_info.scene_key);

		SpecialLogicChiefPeakBattle * special_lobgic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicChiefPeakBattle(figher_item.scene_info.scene_id, figher_item.scene_info.scene_key);
		if (special_lobgic != NULL)
		{
			special_lobgic->StartRoundFight(figher_item.role_id, this->GetFighterRoleId(figher_item.opponent_number));
		}
		else
		{
			gamelog::g_Log_chief_peak.printf(LL_INFO, "%s  line:%d | scene_id:%d scene_key:%d is NULL", __FUNCTION__, __LINE__, figher_item.scene_info.scene_id, figher_item.scene_info.scene_key);
			this->SetFightResult(figher_item.role_id, false);
			this->SetFightResult(this->GetFighterRoleId(figher_item.opponent_number), false);
		}
	}

	this->CheckFinish();
}

void ActivityChiefPeakBattleMatchInfo::SetFightResult(int role_id, bool is_win, bool is_check_finish)
{
	if (is_match_finish || role_id == 0)
	{
		return;
	}

	gamelog::g_Log_chief_peak.printf(LL_INFO, "%s  line:%d | role_id:%d is_win:%d", __FUNCTION__, __LINE__, role_id, (int)is_win);

	int figther_index = this->GetGetFighterIndex(role_id);
	if (figther_index < 0 || figther_index >= ARRAY_LENGTH(figther_info))
	{
		return;
	}

	ActivityChiefPeakBattleFigtherInfo & info = figther_info[figther_index];

	SpecialLogicChiefPeakBattle * special_lobgic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicChiefPeakBattle(info.scene_info.scene_id, info.scene_info.scene_key, false);
	if (special_lobgic != NULL)
	{
		special_lobgic->OnFinish(role_id);
	}

	if (info.role_id == role_id && info.match_round == curr_match_round && info.state != ACTIVITY_CHIEF_PEAK_BATTLE_MATCH_STATE_LOSE)
	{
		if (is_win)
		{
			info.match_round += 1;
			this->GotoMatchScene(figther_index);
		}
		else
		{
			info.state = ACTIVITY_CHIEF_PEAK_BATTLE_MATCH_STATE_LOSE;
		}

		this->SendBattleResultNotice(role_id, is_win);
		this->SendMatchInfoUpdate();
	}

	this->SendBattleReward(role_id, is_win);
	
	if (is_check_finish)
	{
		this->CheckFinish();
	}
}

void ActivityChiefPeakBattleMatchInfo::SendBattleReward(int role_id, bool is_win)
{
	int role_level = 1;

	Role * tmp_role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
	if (tmp_role != NULL)
	{
		role_level = tmp_role->GetLevel();
	}
	else
	{
		UserCacheNode * user_cache_node = UserCacheManager::Instance().GetUserNode(role_id);
		if (NULL != user_cache_node)
		{
			role_level = user_cache_node->level;
		}
	}

	const ActivityChiefPeakBattleBattleRewardCfg * cfg = LOGIC_CONFIG->GetActivityChiefPeakBattleConfig()->GetPerBattleRewardCfg(role_level);
	if (cfg == NULL)
	{
		return;
	}

	// 发邮件
	static MailContentParam contentparam; contentparam.Reset();

	contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = is_win ? cfg->winner_coin : cfg->loser_coin;
	contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_EXP] = is_win ? cfg->winner_exp : cfg->loser_exp;

	int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_activity_chief_peak_battle_battle_reward_subject);
	int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_activity_chief_peak_battle_battle_reward_content);
	if (lensub > 0 && length > 0)
	{
		MailRoute::MailToUser(role_id, MAIL_REASON_DEFAULT, contentparam);
	}
}

void ActivityChiefPeakBattleMatchInfo::SendBattleResultNotice(int role_id, int is_win)
{
	Protocol::SCChiefPeakBattleFighterFinishNotice protocol;
	protocol.role_id = role_id;
	protocol.is_win = is_win;

	for (int i = 0; i < ARRAY_LENGTH(figther_info); ++i)
	{
		if (figther_info[i].match_round >= curr_match_round || figther_info[i].role_id == role_id)
		{
			Role * tmp_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(figther_info[i].role_id));
			if (tmp_role != NULL)
			{
				EngineAdapter::Instance().NetSend(tmp_role->GetNetId(), &protocol, sizeof(protocol));
			}
		}
	}
}

void ActivityChiefPeakBattleMatchInfo::SendMatchInfoUpdate()
{
	Protocol::SCChiefPeakBattleMatchInfo protocol;
	protocol.curr_round = curr_match_round;
	protocol.round_state = round_state;
	protocol.end_time = (unsigned int)m_next_state_time;
	protocol.is_finish = (unsigned int)is_match_finish;

	for (int i = 0; i < ARRAY_LENGTH(figther_info) && i < ARRAY_LENGTH(protocol.match_info); ++i)
	{
		protocol.match_info[i].role_id = figther_info[i].role_id;
		protocol.match_info[i].match_round = figther_info[i].match_round;
		protocol.match_info[i].state = figther_info[i].state;

		UserCacheNode * user_cache_node = UserCacheManager::Instance().GetUserNode(figther_info[i].role_id);
		if (NULL != user_cache_node)
		{
			user_cache_node->GetName(protocol.match_info[i].role_name);
			protocol.match_info[i].level = user_cache_node->level;
			protocol.match_info[i].avatar_type = user_cache_node->avatar_type;
			protocol.match_info[i].headshot_id = user_cache_node->appearance.headshot_id;
		}
	}

	TopicManager::Instance()->OnTopicUpdate(TOPIC_TYPE_CHIEF_PEAK_BATTLE, (const char*)&protocol, sizeof(protocol));
}

void ActivityChiefPeakBattleMatchInfo::OnMatchFinish()
{
	if (is_match_finish)
	{
		return;
	}
	is_match_finish = true;

	this->SendReward();
}

void ActivityChiefPeakBattleMatchInfo::OnRoleLeaveMatch(int role_id)
{
	this->SetFightResult(role_id, false);

	for (int index = 0; index < ARRAY_LENGTH(figther_info); ++index)
	{
		if (figther_info[index].role_id == role_id)
		{
			this->SetFightResult(this->GetFighterRoleId(figther_info[index].opponent_number), true);
			break;
		}
	}
}

void ActivityChiefPeakBattleMatchInfo::SendReward()
{
	std::vector<ActivityChiefPeakBattleFigtherInfo> temp_figther_info(figther_info, figther_info + ARRAY_LENGTH(figther_info));		//复制一下，原本那个客户端还要显示
	std::sort(temp_figther_info.begin(), temp_figther_info.end(), &ActivityChiefPeakBattleMatchInfo::SortFuncFinish);

	//发送排名奖励
	for (int i = 0; i < (int)temp_figther_info.size(); ++i)
	{
		const ActivityChiefPeakBattleFigtherInfo & curr_info = temp_figther_info[i];
		if (curr_info.role_id != 0)
		{
			int pass_round = curr_info.state != ACTIVITY_CHIEF_PEAK_BATTLE_MATCH_STATE_LOSE && i == 0 ? ACTIVITY_CHIEF_PEAK_BATTLE_FIGTHER_NUM : curr_info.match_round;		//第一个没失败就是冠军
			
			const ChiefPeakBattleRankRewardCfg * cfg = LOGIC_CONFIG->GetActivityChiefPeakBattleConfig()->GetChiefPeakBattleRewardCfg(pass_round);
			if (cfg != NULL)
			{
				static MailContentParam contentparam; contentparam.Reset();
				contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] += cfg->coin;
				for (int k = 0; k < (int)cfg->reward_list.size() && k < MAX_ATTACHMENT_ITEM_NUM; ++k)
				{
					const ItemConfigData & reward = cfg->reward_list[k];

					const ItemBase * item_base = ITEMPOOL->GetItem(reward.item_id);
					if (item_base == NULL)
					{
						continue;
					}

					contentparam.item_list[k].item_id = reward.item_id;
					contentparam.item_list[k].num = reward.num;
					contentparam.item_list[k].is_bind = (reward.is_bind) ? 1 : 0;
					contentparam.item_list[k].invalid_time = item_base->CalInvalidTime();
				}

				int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_activity_chief_peak_battle_rank_reward_subject);
				int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_activity_chief_peak_battle_rank_reward_content, pass_round);
				if (lensub > 0 && length > 0) MailRoute::MailToUser(temp_figther_info[i].role_id, MAIL_REASON_DEFAULT, contentparam);
			}


			WorldStatus::Instance().OnChiefPeakBattleFinish(curr_info.role_id, pass_round);
		}
	}
	ActivityChiefPeakBattle * activity = (ActivityChiefPeakBattle *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_CHIEF_PEAK_BATTLE);
	if (NULL == activity)
	{
		return;
	}
	const ActivityChiefPeakBattleFigtherInfo & top_info = temp_figther_info[0];
	if (top_info.role_id != 0 && top_info.state != ACTIVITY_CHIEF_PEAK_BATTLE_MATCH_STATE_LOSE)
	{
		int need_send_battle_reward_times = MAX_MATCH_ROUND - (top_info.match_round - 1); //胜利者会下发剩余战斗场次奖励，2个人打的时候，可能经过1轮就结束，正常来说要打多轮
		while (need_send_battle_reward_times > 0)
		{
			need_send_battle_reward_times -= 1;
			this->SendBattleReward(top_info.role_id, true);
		}

		int champion_role_id = top_info.role_id;
		
		Role * tmp = World::GetInstWorld()->GetSceneManager()->GetRole(champion_role_id);
		if (tmp != NULL)
		{
			Protocol::SCChiefPeakBattleChampion protocol;
			EngineAdapter::Instance().NetSend(tmp->GetNetId(), (const char *)&protocol, sizeof(protocol));
		}

		activity->SendBetReward(champion_role_id);

		SpecialLogicChiefPeakBattle * special_lobgic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicChiefPeakBattle(top_info.scene_info.scene_id, top_info.scene_info.scene_key);
		if (special_lobgic != NULL)
		{
			special_lobgic->OnFinish(0, true);//手动将胜利者踢出场景
		}

		//传闻
		UserCacheNode * uc = UserCacheManager::Instance().GetUserNode(top_info.role_id);
		if (NULL != uc)
		{
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_chief_peak_battle_champion_broadcast, top_info.role_id, uc->GetNamePtr());
			if (length > 0)
			{
				World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT),
				 0, 0, activity->GetActivityLimitLevel(activity->GetActivityType()), 0);
			}
		}
	}
	else
	{
		// 传闻
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_chief_peak_battle_nobody_join_broadcast);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT),
			 0, 0, activity->GetActivityLimitLevel(activity->GetActivityType()), 0);
		}
		return;
	}
}

void ActivityChiefPeakBattleMatchInfo::CalTopFigther()
{
	//用首席竞选的出线的人进行打斗

	std::vector<int> top_role_list = WorldStatus::Instance().GetChiefElectionTopRole();


	int real_figther_num = (int)top_role_list.size() < ACTIVITY_CHIEF_PEAK_BATTLE_FIGTHER_NUM ? (int)top_role_list.size() : ACTIVITY_CHIEF_PEAK_BATTLE_FIGTHER_NUM;
	std::random_shuffle(ActivityChiefPeakBattle::FIGHTER_NUMBER, ActivityChiefPeakBattle::FIGHTER_NUMBER + ARRAY_LENGTH(ActivityChiefPeakBattle::FIGHTER_NUMBER));

	figther_info_num = real_figther_num;
	for (int i = 0; i < real_figther_num; ++i)
	{
		ActivityChiefPeakBattleFigtherInfo & info = figther_info[ActivityChiefPeakBattle::FIGHTER_NUMBER[i] - 1];
		info.role_id = top_role_list[i];
		info.number = ActivityChiefPeakBattle::FIGHTER_NUMBER[i];

		gamelog::g_Log_chief_peak.printf(LL_INFO, "%s  line:%d | %d role_id:%d number:%d", __FUNCTION__, __LINE__, i + 1, info.role_id, info.number);
	}
}


ActivityChiefPeakBattleFigtherInfo * ActivityChiefPeakBattleMatchInfo::GetOpponentInfo(int figther_index)
{
	if (figther_index < 0 || figther_index >= ARRAY_LENGTH(figther_info))
	{
		return NULL;
	}

	const ActivityChiefPeakBattleFigtherInfo & info = figther_info[figther_index];
	for (int opponent_index = 0; opponent_index < ARRAY_LENGTH(figther_info); ++opponent_index)
	{
		ActivityChiefPeakBattleFigtherInfo & opponent_info = figther_info[opponent_index];

		if (opponent_info.role_id == 0 || opponent_info.role_id == info.role_id || opponent_info.match_round != info.match_round)
		{
			continue;
		}

		int flag = ~0 << info.match_round;
		if ((figther_index & flag) == (opponent_index & flag))
		{
			return &opponent_info;
		}
	}

	return NULL;
}

int ActivityChiefPeakBattleMatchInfo::GetFighterRoleId(int fighter_number)
{
	int figther_index = fighter_number - 1;
	if (figther_index < 0 || figther_index >= ARRAY_LENGTH(figther_info))
	{
		return 0;
	}

	return figther_info[figther_index].role_id;
}

int ActivityChiefPeakBattleMatchInfo::GetGetFighterIndex(int role_id)
{
	for (int i = 0; i < ARRAY_LENGTH(figther_info) && role_id != 0; ++i)
	{
		if (role_id == figther_info[i].role_id)
		{
			return i;
		}
	}

	return -1;
}

//UserCacheNode * ActivityChiefPeakBattle::GetRole(int role_id)
//{
//	return UserCacheManager::Instance().GetUserNode(role_id);
//}
//
//UserCacheNode * ActivityChiefPeakBattleMatchInfo::GetRole(int role_id)
//{
//	return UserCacheManager::Instance().GetUserNode(role_id);
//}
//
//std::string UTF8_2_GBK(std::string utf8Str);
//
//
//void ActivityChiefPeakBattleMatchInfo::OutBattleInfo()
//{
//	mTrace("回合:%d 对战信息\n", curr_match_round);
//	for (int i = 0; i < ARRAY_LENGTH(figther_info); ++i)
//	{
//		const auto & self_info = figther_info[i];
//		const auto *  opponent_info = this->GetOpponentInfo(i);
//
//		if (self_info.role_id == 0 || self_info.match_round < curr_match_round)
//		{
//			continue;
//		}
//
//		if (opponent_info != NULL)
//		{
//			mTrace("%s<-------->%s | %d %d | num:%d<------>%d\n", UTF8_2_GBK(GetRole(self_info.role_id)->role_name).c_str(), UTF8_2_GBK(GetRole(opponent_info->role_id)->role_name).c_str(),
//			       self_info.role_id, opponent_info->role_id, self_info.number, opponent_info->number);
//		}
//		else
//		{
//			mTrace("%s<-------->轮空 | %d | num:%d<------>轮空\n", UTF8_2_GBK(GetRole(self_info.role_id)->role_name).c_str(), self_info.role_id, self_info.number);
//		}
//	}
//}
//
//void ActivityChiefPeakBattleMatchInfo::mTrace(const char * str, ...)
//{
//	va_list vlist;
//	va_start(vlist, str);
//	char a[4096] = { 0 };
//	vsprintf(a, str, vlist);
//	OutputDebugStringA(a);
//	va_end(vlist);
//}
