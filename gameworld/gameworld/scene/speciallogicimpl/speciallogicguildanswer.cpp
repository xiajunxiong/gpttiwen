#include "speciallogicguildanswer.hpp"
#include "world.h"
#include "gamelog.h"
#include "config/logicconfigmanager.hpp"
#include "config/activityconfig/guildanswer/activityguildanswerconfig.hpp"
#include "protocol/msgguild.hpp"
#include "scene/scene.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "item/money.h"
#include "item/knapsack.h"
#include "global/activity/activitymanager.hpp"
#include "global/activity/activityimpl/activityguildanswer.hpp"
#include "global/team/teammanager.hpp"
#include "global/team/team.hpp"
#include "global/guild/guild.hpp"
#include "global/worldstatus/worldstatus.hpp"
#include "other/event/eventhandler.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/roleactivity/roleactivity.hpp"

SpecialLogicGuildAnswer::SpecialLogicGuildAnswer(Scene * scene) : SpecialLogic(scene), m_act_end_timestamp(0), m_next_send_question_timestamp(0), m_cur_question_seq(0), m_close_activity_timestamp(0u)
{
}

SpecialLogicGuildAnswer::~SpecialLogicGuildAnswer()
{
}

bool SpecialLogicGuildAnswer::CanDestroy()
{
	ActivityGuildAnswer * act_ref = (ActivityGuildAnswer *)(ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_GUILD_ANSWER));
	return (NULL == act_ref || act_ref->IsActivityClose()) && m_players_award_map.empty();
}

void SpecialLogicGuildAnswer::Update(unsigned long interval, time_t now_second)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}
	SpecialLogic::Update(interval, now_second);

	if (m_next_send_question_timestamp > 0u && now_second >= m_next_send_question_timestamp)
	{
		if (-1 == m_cur_question_seq)	//入场时间结束
		{
			int role_num = m_scene->RoleNum();
			for (int i = 0; i < role_num; i++)
			{
				Role * role = m_scene->GetRoleByIndex(i);
				if (NULL == role) continue;

				m_participate_set.insert(role->GetUID());
				EventHandler::Instance().OnParticipateLimitActivity(role, ACTIVITY_TYPE_GUILD_ANSWER, __FUNCTION__);
			}
		}
		this->CheckAnswerMap();
		this->OnNextQuestion(false);
	}

	if (m_act_end_timestamp > 0u && now_second >= m_act_end_timestamp)
	{
		this->SendRoleAwardList();		// 发送总结算列表；
		this->DelayKickOutAllRole();	// 活动结束，把玩家踢了；

		m_act_end_timestamp = now_second + 5;	// 5 秒检测一次；
	}

	if (m_close_activity_timestamp > 0u && now_second >= m_close_activity_timestamp)
	{
		m_close_activity_timestamp = 0u;
		Activity* act = ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_GUILD_ANSWER);
		if (NULL != act && act->IsActivityOpen())
		{
			act->ForceToCloseState();
		}
	}
}

void SpecialLogicGuildAnswer::OnRoleEnterScene(Role * role)
{
	if (NULL == role) return;
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		this->DelayKickOutRole(role);
	}

	// 如果当前是活动进行阶段，发送当前题目；
	ActivityGuildAnswer * act_ref = (ActivityGuildAnswer *)(ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_GUILD_ANSWER));
	if (NULL == act_ref || act_ref->IsActivityClose())
	{
		this->DelayKickOutRole(role);
	}
	else if (act_ref->IsActivityOpen())
	{
		this->SendCurQuestionInfo();
		std::set<int>::iterator it = m_participate_set.find(role->GetUID());
		if (it == m_participate_set.end())
		{
			m_participate_set.insert(role->GetUID());
			EventHandler::Instance().OnParticipateLimitActivity(role, ACTIVITY_TYPE_GUILD_ANSWER, __FUNCTION__);
		}
	}
}

void SpecialLogicGuildAnswer::OnRoleLeaveScene(Role * role, bool is_logout)
{
	if (NULL == role) return;

	// 把玩家的答题信息清除掉
	std::map<UserID, int>::iterator answer_it = m_players_answer_map.find(role->GetUserId());
	if (m_players_answer_map.end() != answer_it) m_players_answer_map.erase(answer_it);
}

void SpecialLogicGuildAnswer::OnGuildAnswerStandy()
{
	m_act_end_timestamp = 0;
	m_players_award_map.clear();
}

void SpecialLogicGuildAnswer::OnGuildAnswerBegin(const std::vector<int>& question_vec)
{
	m_act_end_timestamp = 0;
	m_players_award_map.clear();
	m_participate_set.clear();

	m_question_id_vec = question_vec;

	this->OnNextQuestion(true);
}

void SpecialLogicGuildAnswer::OnGuildAnswerOver()
{
	m_act_end_timestamp = EngineAdapter::Instance().Time() + 3;	// 延迟 3 秒结束
}

void SpecialLogicGuildAnswer::CommitAnswer(Role * role, int answer)
{
	if (NULL == role) return;

	m_players_answer_map[role->GetUserId()] = answer;	// 覆盖，记入玩家所选答案表；
}

void SpecialLogicGuildAnswer::OnNextQuestion(bool is_ready_state)
{
	const GuildAnswerOthers & ga_cfg = LOGIC_CONFIG->GetActivityGuildAnswerConfig()->GetGuildAnswerOtherConfig();

	time_t next_during_time = ga_cfg.question_time;
	m_cur_question_seq += 1;
	if (is_ready_state)
	{
		next_during_time = ga_cfg.ready_time;
		m_cur_question_seq = -1;
	}

	if (m_cur_question_seq >= (int)m_question_id_vec.size())
	{
		m_next_send_question_timestamp = 0u;
		m_close_activity_timestamp = EngineAdapter::Instance().Time() + 10;
	}
	else
	{
		m_next_send_question_timestamp = EngineAdapter::Instance().Time() + next_during_time;
	}
	
	this->SendCurQuestionInfo();
}

void SpecialLogicGuildAnswer::SendCurQuestionInfo()
{
	int question_id = -1;
	if (0 <= m_cur_question_seq && m_cur_question_seq < (int)m_question_id_vec.size())
	{
		question_id = m_question_id_vec[m_cur_question_seq];
	}

	Protocol::SCActivityGuildAnswerCurQuestion agacq;
	agacq.cur_seq = m_cur_question_seq;
	agacq.question_id = question_id;
	agacq.next_time = (unsigned int)m_next_send_question_timestamp;

	m_scene->SendToRole((const char *)&agacq, sizeof(agacq));
}

void SpecialLogicGuildAnswer::CheckAnswerMap()
{
	if (m_cur_question_seq < 0 || m_cur_question_seq >= (int)m_question_id_vec.size()) return;

	int correct_answer = LOGIC_CONFIG->GetActivityGuildAnswerConfig()->GetRightAnswer(m_question_id_vec[m_cur_question_seq]);
	for (std::map<UserID, int>::iterator loop = m_players_answer_map.begin(); loop != m_players_answer_map.end(); ++loop)
	{
		if (0 == loop->second) continue;	// 如果答 0，说明没作答，逻辑继续；

		Role * role = m_scene->GetRoleByUID(UidToInt(loop->first));
		if (NULL == role) continue;			// 如果不在这个场景里，答案无效；

		bool is_correct = loop->second == correct_answer;
		Team * team = role->GetMyTeam();
		if (NULL == team || (!role->IsLeader() && !role->IsFollow()))
		{
			this->GiveGuildAnswerAward(role, is_correct);
		}
		else if (role->IsLeader())		// 是队长
		{
			int mem_num = team->GetMemberCount();
			for (int mem_idx = 0; mem_idx < mem_num; ++mem_idx)
			{
				Role * member = team->GetMemberRoleByIndex(mem_idx);
				if (NULL != member && member->IsFollow()) this->GiveGuildAnswerAward(member, is_correct);
			}
		}
	}
}

void SpecialLogicGuildAnswer::GiveGuildAnswerAward(Role * role, bool is_correct)
{
	const GuildAnswerReward * reward = LOGIC_CONFIG->GetActivityGuildAnswerConfig()->GetAnswerReward(role->GetLevel(), is_correct);
	if (NULL == reward) return;

	if (reward->exp > 0) role->AddExp(reward->exp, ADD_EXP_TYPE_QUESTION);
	if (reward->coin > 0) role->GetRoleModuleManager()->GetMoney()->AddCoinBind(reward->coin, __FUNCTION__);
	if (reward->contribution > 0) role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(reward->contribution, MONEY_TYPE_GUILD_CONTRIBUTION, __FUNCTION__);
	if (reward->family_coin > 0)
	{
		Guild * guild = role->GetGuild();
		if (NULL != guild)
		{
			guild->AddExp(reward->family_coin);
		}
	}

	GuildAnswerAwardTotalCon tmp_award(reward->coin, reward->exp, reward->contribution);
	for (int i = 0; i < reward->list_count && i < GUILD_ANSWER_REWARD_ITEM_MAX; ++i)
	{
		const ItemConfigData * reward_item = LOGIC_CONFIG->GetActivityGuildAnswerConfig()->RandItemAward(reward->reward_group_list[i]);
		if (NULL == reward_item) continue;	// 有可能为空
		
		if (reward_item->num > 0) tmp_award.AddItemGet(*reward_item);
		role->GetRoleModuleManager()->GetKnapsack()->Put(*reward_item, PUT_REASON_GUILD_ANSWER);
	}

	std::map<UserID, GuildAnswerAwardTotalCon>::iterator user_it = m_players_award_map.find(role->GetUserId());
	if (m_players_award_map.end() != user_it)
	{
		user_it->second += tmp_award;
	}
	else
	{
		m_players_award_map[role->GetUserId()] = tmp_award;
	}
	if (is_correct)
	{
		role->GetRoleModuleManager()->OnCorrectGuildAnswer();
	}
	role->GetRoleModuleManager()->GetCommonData().guild_answer_reply_count += 1;
	role->GetRoleModuleManager()->GetCommonData().fetch_special_logic_activity_reward_zero_timestamp[DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_GUILD_ANSWER] = 
	(unsigned int)EngineAdapter::Instance().Time() + EngineAdapter::Instance().NextDayInterval(0, 0, 0);
	this->SendAnswerResult(role, is_correct);
}

void SpecialLogicGuildAnswer::SendAnswerResult(Role * role, bool is_correct)
{
	if (NULL == role) return;

	Protocol::SCActivityGuildAnswerResultInfor agari;
	agari.answer_result = is_correct ? 1 : 0;
	role->GetRoleModuleManager()->NetSend((const void *)&agari, sizeof(agari));
}

void SpecialLogicGuildAnswer::SendRoleAwardList()
{
	if (m_players_award_map.empty()) return;

	Protocol::SCActivityGuildAnswerAwardSettle agaas;

	for (std::map<UserID, GuildAnswerAwardTotalCon>::iterator uid_it = m_players_award_map.begin(); uid_it != m_players_award_map.end();)
	{
		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(uid_it->first);
		if (NULL == role)		// 如果玩家处于离线状态，直接不发了；
		{
			m_players_award_map.erase(uid_it++);
		}
		else if (role->GetRoleStatusManager()->IsInBattleStatus())	// 如果在战斗中，继续；
		{
			++uid_it;
		}
		else	// 否则不在战斗中，发送协议；
		{
			const GuildAnswerAwardTotalCon & data = uid_it->second;
			agaas.coin = data.coin;
			agaas.exp = data.experience;
			agaas.contribution = data.guild_contribution;

			agaas.item_num = 0;
			for (std::map<int, std::map<int, int> >::const_iterator item_it = data.item_list.begin(); item_it != data.item_list.end(); ++item_it)
			{
				for (std::map<int, int>::const_iterator bind_it = item_it->second.begin(); bind_it != item_it->second.end(); ++bind_it)
				{
					if (agaas.item_num < 0 || agaas.item_num >= ACTIVITY_AWARD_NUM_MAX) break;
					agaas.item_list[agaas.item_num++] = Protocol::PtcItemCon(item_it->first, bind_it->first, bind_it->second);
				}
			}

			role->GetRoleModuleManager()->NetSend((const void *)&agaas, sizeof(agaas));
			m_players_award_map.erase(uid_it++);
		}
	}
}