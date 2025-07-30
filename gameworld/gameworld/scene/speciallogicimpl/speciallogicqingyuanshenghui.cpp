#include "other/rolemodulemanager.hpp"
#include "obj/character/role.h"
#include "global/team/team.hpp"
#include "global/team/teammanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "battle/battle_manager_local.hpp"
#include "world.h"

#include "servercommon/errornum.h"

#include "protocol/msgqingyuanshenghui.h"
#include "config/activityconfig/qingyuanshenghui/activityqingyuanshenghuiconfig.hpp"
#include "speciallogicqingyuanshenghui.hpp"

const int static GET_NUM = 2;

SpecialLogicQingYuanShengHui::SpecialLogicQingYuanShengHui(Scene * scene) : SpecialLogic(scene)
{
	this->Reset();
}

SpecialLogicQingYuanShengHui::~SpecialLogicQingYuanShengHui()
{
}

void SpecialLogicQingYuanShengHui::Reset()
{
	m_is_finish = false;
	m_is_start = false;
	m_role_num_change = false;
	m_phase = 0;
	m_phase_start_time = 0;
	m_phase_end_time = 0;
	m_scene_reward_next_time = 0;
}

void SpecialLogicQingYuanShengHui::Update(unsigned long interval, time_t now_second)
{
	if (m_is_finish || !m_is_start)
	{
		return;
	}

	if (m_role_num_change)
	{
		m_role_num_change = false;
		this->SendSceneInfo();
	}

	this->UpdateSceneRewards(interval, now_second);
	this->UpdatePhase(interval, now_second);
	this->UpdateQuestion(interval, now_second);
	this->UpdateMatch(interval, now_second);
	this->UpdateReadyFight(interval, now_second);
}

void SpecialLogicQingYuanShengHui::UpdateSceneRewards(unsigned long interval, time_t now_second)
{
	const QingYuanShengHuiOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityQingYuanShengHuiConfig()->GetQingYuanShengHuiOtherCfg();
	if (m_scene_reward_next_time == 0)
	{
		m_scene_reward_next_time = now_second + other_cfg.scene_reward_interval;
	}

	if (now_second < m_scene_reward_next_time)
	{
		return;
	}

	m_scene_reward_next_time = now_second + other_cfg.scene_reward_interval;
	for (int i = 0; i < (int)m_scene->RoleNum(); ++i)
	{
		Role * role = m_scene->GetRoleByIndex(i);
		if (NULL != role)
		{
			const QingYuanShengHuiGroupRewardItemCfg * reward_cfg = LOGIC_CONFIG->GetActivityQingYuanShengHuiConfig()->GetRewardByGroupId(other_cfg.scene_reward_group_id);
			this->PutReward(role, reward_cfg);
		}
	}
}

void SpecialLogicQingYuanShengHui::UpdatePhase(unsigned long interval, time_t now_second)
{
	if (now_second > m_phase_end_time)
	{
		const QingYuanShengHuiPhaseCfg * cfg = LOGIC_CONFIG->GetActivityQingYuanShengHuiConfig()->GetQingYuanShengHuiPhaseCfg(m_phase + 1);
		if (!cfg)
		{
			m_is_finish = true;
		}
		else
		{
			m_phase += 1;
			m_phase_start_time = m_phase_end_time;
			m_phase_end_time = now_second + cfg->phase_length_of_time;

			if (m_phase_start_time == 0)
			{
				m_phase_start_time = now_second;
			}
		}

		this->SendSceneInfo();
	}
}

void SpecialLogicQingYuanShengHui::UpdateQuestion(unsigned long interval, time_t now_second)
{
	const QingYuanShengHuiOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityQingYuanShengHuiConfig()->GetQingYuanShengHuiOtherCfg();

	for (std::map<QingYuanShengHuiRoleGroup, QingYuanShengHuiQuestiontInfo>::iterator question_info_iter = m_question_info.begin(); question_info_iter != m_question_info.end(); ++question_info_iter)
	{
		QingYuanShengHuiQuestiontInfo & question_info = question_info_iter->second;
		std::set<int> role_group = question_info_iter->first.role_group;

		bool need_send = false;

		if (question_info.is_open_ui != question_info.next_is_open_ui)
		{
			question_info.is_open_ui = question_info.next_is_open_ui;

			if (question_info.is_open_ui)
			{
				need_send = true;
				question_info.next_question_time = now_second + other_cfg.question_interval;
			}
		}

		if (!question_info.is_open_ui)
		{
			continue;
		}

		need_send = need_send || this->CheckQingYuanShengHuiQuestiontInfo(question_info, now_second);

		if (need_send)
		{
			this->SendQuestionInfo(question_info);
		}

	}
}


void SpecialLogicQingYuanShengHui::UpdateMatch(unsigned long interval, time_t now_second)
{
	if (m_match_team_index.size() < 2)
	{
		return;
	}


	std::vector<int> rand_list;
	for (std::set<int>::iterator iter = m_match_team_index.begin(); iter != m_match_team_index.end(); ++iter)
	{
		int team_index = *iter;
		Team * team = TeamManager::Instance().GetTeamByTeamIndex(team_index);
		if (!team)
		{
			continue;
		}

		rand_list.push_back(team_index);
	}
	m_match_team_index.clear();

	std::random_shuffle(rand_list.begin(), rand_list.end());
	int remain_num = rand_list.size() % GET_NUM;
	int size = (int)(rand_list.size() - remain_num);

	for (std::vector<int>::reverse_iterator iter = rand_list.rbegin(); iter != rand_list.rend() && remain_num > 0; ++iter, remain_num -= 1)
	{
		m_match_team_index.insert(*iter);
	}

	const time_t start_battle_time = now_second + 5;
	for (int all_list_index = 0; all_list_index < size; all_list_index += GET_NUM)
	{
		std::vector<Team * > team_list;
		std::vector<int> team_index_list;
		for (int num = 0; num < GET_NUM && all_list_index + num < size; num += 1)
		{
			int team_index = rand_list[all_list_index + num];
			Team * team_1 = TeamManager::Instance().GetTeamByTeamIndex(team_index);
			if (!team_1)
			{
				continue;
			}

			team_list.push_back(team_1);
			team_index_list.push_back(team_index);
		}

		if (team_list.size() != GET_NUM)
		{
			continue;
		}



		for (int i = 0; i < (int)team_list.size(); ++i)
		{
			Team * curr = team_list[i];
			std::vector<MsgRoleBaseInfo> msg_role_infos = curr->GetAllMsgRoleBaseInfo();

			Protocol::SCQingYuanShengHuiOpponentInfo opponent_info;
			opponent_info.start_battle_time = (unsigned int)start_battle_time;
			for (int k = 0; k < ARRAY_LENGTH(opponent_info.role_info) && k < (int)msg_role_infos.size(); ++k)
			{
				opponent_info.role_info[k] = msg_role_infos[k];
			}

			for (int k = 0; k < (int)team_list.size(); ++k)
			{
				if (k == i) continue;

				team_list[k]->SendToMember(&opponent_info, sizeof(opponent_info));
			}

		}

		QingYuanShengHuiReadyFightInfo item;
		item.time = start_battle_time;
		item.team_index_list = team_index_list;
		m_ready_fight_info.push_front(item);
	}
}

void SpecialLogicQingYuanShengHui::UpdateReadyFight(unsigned long interval, time_t now_second)
{
	std::vector<std::list<QingYuanShengHuiReadyFightInfo>::iterator> del;

	for (std::list<QingYuanShengHuiReadyFightInfo>::iterator iter = m_ready_fight_info.begin(); iter != m_ready_fight_info.end(); ++iter)
	{
		if (now_second < iter->time)
		{
			continue;
		}

		del.push_back(iter);

		std::vector<Role * > attackers;
		std::vector<Role * > defenders;
		for (int i = 0; i < (int)iter->team_index_list.size(); ++i)
		{
			int team_index = iter->team_index_list[i];
			Team * team = TeamManager::Instance().GetTeamByTeamIndex(team_index);
			if (!team)
			{
				continue;
			}

			for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
			{
				TeamMember * team_member = team->GetMemberInfo(i);
				if (!team_member)
				{
					continue;
				}

				Role * tmp_role = m_scene->GetRoleByUID(team_member->uid);
				if (!tmp_role)
				{
					continue;
				}

				if (attackers.empty())
				{
					attackers.push_back(tmp_role);
				}
				else
				{
					defenders.push_back(tmp_role);
				}
			}

			if (!attackers.empty() && !defenders.empty())
			{
				break;
			}
		}

		BattleManagerLocal::GetInstance().StartQingYuanShengHuiReq(attackers, defenders);		
	}

	for (int i = 0; i < (int)del.size(); ++i)
	{
		m_ready_fight_info.erase(del[i]);
	}
}

void SpecialLogicQingYuanShengHui::OnRoleEnterScene(Role * role)
{
	Team * team = role->GetMyTeam();
	if (!team)
	{
		this->DelayKickOutRole(role);
		return;
	}


	int role_id = role->GetUID();
	int qing_yuan_role_id = 0;
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		TeamMember * team_member = team->GetMemberInfo(i);
		if (team_member == NULL || team_member->uid == role_id)
		{
			continue;
		}

		qing_yuan_role_id = team_member->uid;
		break;
	}

	if (qing_yuan_role_id == 0)
	{
		this->DelayKickOutRole(role);
		return;
	}
	QingYuanShengHuiRoleInfo & role_info = m_role_info[role_id];
	role_info.role_id = role_id;
	role_info.qing_yuan_role_id = qing_yuan_role_id;

	m_role_num_change = true;
	this->SendRoleInfo(role);
}

void SpecialLogicQingYuanShengHui::OnRoleLeaveScene(Role * role, bool is_logout)
{
	this->OnRoleQuestionUiOp(role, false);
	this->OnRoleBattleMatch(role, false);
	this->KickOutQingYuanRole(role);

	m_role_num_change = true;

}

bool SpecialLogicQingYuanShengHui::CanDestroy()
{
	return false;
}

void SpecialLogicQingYuanShengHui::OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack)
{
	if (BATTLE_MODE_QING_YUAN_SHENG_HUI != ack->battle_mode)
	{
		return;
	}

	for (int i = 0; i < ack->attacker_list.role_count; ++i)
	{
		battlegameprotocol::BattleGameFightResultAck::RoleInfo & curr = ack->attacker_list.role_list[i];
		QingYuanShengHuiRoleInfo *info = MapValuesPtr(m_role_info, curr.uid);
		if (!info)
		{
			continue;
		}

		info->battle_times += 1;

		Role * role = m_scene->GetRoleByUID(curr.uid);
		if (role)
		{
			this->SendRoleInfo(role);
		}
	}

	for (int i = 0; i < ack->defender_list.role_count; ++i)
	{
		battlegameprotocol::BattleGameFightResultAck::RoleInfo & curr = ack->defender_list.role_list[i];
		QingYuanShengHuiRoleInfo * info = MapValuesPtr(m_role_info, curr.uid);
		if (!info)
		{
			continue;
		}

		info->battle_times += 1;

		Role * role = m_scene->GetRoleByUID(curr.uid);
		if(role)
		{
			this->SendRoleInfo(role);
		}
	}	
}

void SpecialLogicQingYuanShengHui::OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight * ack)
{
	if (BATTLE_MODE_QING_YUAN_SHENG_HUI != ack->battle_mode)
	{
		return;
	}

	for (int i = 0; i < ack->attacker_list.role_count; ++i)
	{
		battlegameprotocol::BattleGameForceFinishFight::RoleInfo & curr = ack->attacker_list.role_list[i];
		QingYuanShengHuiRoleInfo * info = MapValuesPtr(m_role_info, curr.uid);
		if (!info)
		{
			continue;
		}

		info->battle_times += 1;

		Role * role = m_scene->GetRoleByUID(curr.uid);
		if (role)
		{
			this->SendRoleInfo(role);
		}
	}

	for (int i = 0; i < ack->defender_list.role_count; ++i)
	{
		battlegameprotocol::BattleGameForceFinishFight::RoleInfo & curr = ack->defender_list.role_list[i];
		QingYuanShengHuiRoleInfo * info = MapValuesPtr(m_role_info, curr.uid);
		if (!info)
		{
			continue;
		}

		info->battle_times += 1;

		Role * role = m_scene->GetRoleByUID(curr.uid);
		if (role)
		{
			this->SendRoleInfo(role);
		}
	}
}

void SpecialLogicQingYuanShengHui::OnRoleLeaveTeam(Role * role)
{
	this->OnRoleBattleMatch(role, false);
	this->DelayKickOutRole(role);
	this->KickOutQingYuanRole(role);	
}

bool SpecialLogicQingYuanShengHui::CanTeamMemberGotoLeaderFromOtherScene(Role * member, Role * leader)
{
	return false;
}

bool SpecialLogicQingYuanShengHui::CanInvitationRole(Role * role, int uid)
{
	return false;
}

int SpecialLogicQingYuanShengHui::OnRoleGather(Role * role)
{
	int role_id = role->GetUID();
	QingYuanShengHuiRoleInfo * role_info = MapValuesPtr(m_role_info, role_id);
	if (!role_info)
	{
		ret_line;
	}

	time_t now = EngineAdapter::Instance().Time();
	if (now < role_info->can_gather_next_time)
	{
		ret_line;
	}

	const QingYuanShengHuiOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityQingYuanShengHuiConfig()->GetQingYuanShengHuiOtherCfg();

	const QingYuanShengHuiGroupRewardItemCfg * reward_cfg = LOGIC_CONFIG->GetActivityQingYuanShengHuiConfig()->GetGatherReward();
	if (!this->PutReward(role, reward_cfg))
	{

		ret_line;
	}

	role_info->can_gather_next_time = now + other_cfg.gather_interval;

	this->SendRoleInfo(role);

	return 0;
}

int SpecialLogicQingYuanShengHui::OnRoleQuestionUiOp(Role * role, bool is_open_ui)
{
	int role_id = role->GetUID();
	QingYuanShengHuiQuestiontInfo * question_info = this->GetQuestiontInfo(role_id, is_open_ui);
	if (!question_info)
	{
		ret_line;
	}

	if (question_info->is_finish)
	{
		role->NoticeNum(errornum::EN_QING_YUAN_SHENG_HUI_QUESTION_FINISH);
		ret_line;
	}

	question_info->next_is_open_ui = is_open_ui;
	this->SendQuestionOpRoute(question_info->role_group, question_info->next_is_open_ui);

	return 0;
}

int SpecialLogicQingYuanShengHui::OnRoleAnswerQuestion(Role * role, int select_num)
{
	int role_id = role->GetUID();
	QingYuanShengHuiQuestiontInfo * question_info = this->GetQuestiontInfo(role_id, true);
	if (!question_info)
	{
		ret_line;
	}

	question_info->answer_list[role_id] = select_num;

	return 0;
}

int SpecialLogicQingYuanShengHui::OnRoleBattleUi(Role * role, int is_open_ui)
{
	Team * t = role->GetMyTeam();
	if (!t)
	{
		ret_line;
	}

	int team_idx = t->GetTeamIndex();

	Protocol::SCQingYuanShengHuiMatchOpRoute protocol;
	protocol.is_match = 0;
	protocol.is_open = is_open_ui;
	t->SendToMember(&protocol, sizeof(protocol));

	return 0;
}

int SpecialLogicQingYuanShengHui::OnRoleBattleMatch(Role * role, bool is_join, bool is_notice)
{
	Team * t = role->GetMyTeam();
	if (!t)
	{
		ret_line;
	}

	int team_idx = t->GetTeamIndex();

	time_t now = EngineAdapter::Instance().Time();
	if (is_join)
	{
		m_match_team_index.insert(team_idx);
	}
	else
	{
		m_match_team_index.erase(team_idx);
	}

	Protocol::SCQingYuanShengHuiMatchOpRoute protocol;
	protocol.is_match = is_join;
	protocol.is_open = 1;
	protocol.begin_time = is_join ? (unsigned int)now : 0;
	t->SendToMember(&protocol, sizeof(protocol));

	return 0;
}

int SpecialLogicQingYuanShengHui::OnRoleFetchBattleReward(Role * role, int reward_times)
{
	int index = reward_times - 1;

	const QingYuanShengHuiOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityQingYuanShengHuiConfig()->GetQingYuanShengHuiOtherCfg();
	const std::vector<ItemConfigData> * rewards_ptr = MapValuesConstPtr(other_cfg.m_battle_reward, index);
	if (!rewards_ptr || rewards_ptr->empty())
	{
		ret_line;
	}

	int role_id = role->GetUID();
	QingYuanShengHuiRoleInfo * info = MapValuesPtr(m_role_info, role_id);
	if (!info)
	{
		ret_line;
	}

	if (::IsSetBit(info->battle_reward_flag, index))
	{
		ret_line;
	}

	if (!role->GetRoleModuleManager()->GetKnapsack()->PutVec(*rewards_ptr, PUT_REASON_QING_YUAN_SHENG_HUI))
	{
		role->NoticeNum(errornum::EN_HAS_NO_SPACING);
		ret_line;
	}

	::SetBit(info->battle_reward_flag, index);
	this->SendRoleInfo(role);

	return 0;
}

int SpecialLogicQingYuanShengHui::OnQiYuan(Role * role)
{
	int role_id = role->GetUID();
	QingYuanShengHuiRoleInfo * info = MapValuesPtr(m_role_info, role_id);
	if (!info)
	{
		ret_line;
	}

	if (info->is_get_qi_yuan_rewards)
	{
		ret_line;
	}

	const QingYuanShengHuiOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityQingYuanShengHuiConfig()->GetQingYuanShengHuiOtherCfg();

	const QingYuanShengHuiGroupRewardItemCfg * reward_cfg = LOGIC_CONFIG->GetActivityQingYuanShengHuiConfig()->GetRewardByGroupId(other_cfg.qi_yuan_reward_group_id);
	if (!this->PutReward(role, reward_cfg))
	{

		ret_line;
	}

	info->is_get_qi_yuan_rewards = 1;
	this->SendRoleInfo(role);

	Protocol::SCQingYuanShengHuiQiYuanNotice protocol;
	protocol.x = role->GetPos().x;
	protocol.y = role->GetPos().y;
	protocol.begin_time = (unsigned int)EngineAdapter::Instance().Time();

	m_scene->SendToRole((char *)&protocol, sizeof(protocol));

	return 0;
}

void SpecialLogicQingYuanShengHui::GmOp(int type, int p1, int p2)
{
	time_t now = EngineAdapter::Instance().Time();
	switch (type)
	{
		case 0:
		{
			m_phase_end_time = now;
		}
		break;
	}

}

void SpecialLogicQingYuanShengHui::OnActivityBegin()
{
	this->Reset();
	m_is_start = true;
}

void SpecialLogicQingYuanShengHui::OnActivityClose()
{
	m_is_finish = true;
	this->DelayKickOutAllRole();
}

void SpecialLogicQingYuanShengHui::SendSceneInfo(Role * role)
{
	Protocol::SCQingYuanShengHuiSceneInfo protocol;
	protocol.phase = m_phase;
	protocol.phase_start = (unsigned int)m_phase_start_time;
	protocol.phase_end = (unsigned int)m_phase_end_time;
	protocol.role_num = m_scene->RoleNum();

	if (role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
	}
	else
	{
		m_scene->SendToRole((char *)&protocol, sizeof(protocol));
	}
}

void SpecialLogicQingYuanShengHui::SendRoleInfo(Role * role)
{
	if (!role)
	{
		return;
	}

	int role_id = role->GetUID();
	const QingYuanShengHuiRoleInfo * role_info_ptr = this->GetQingYuanShengHuiRoleInfo(role_id);
	if (!role_info_ptr)
	{
		return;
	}

	const QingYuanShengHuiRoleInfo & role_info = *role_info_ptr;

	Protocol::SCQingYuanShengHuiRoleInfo protocol;
	protocol.can_gather_next_time = (unsigned int)role_info.can_gather_next_time;
	protocol.battle_times = role_info.battle_times;
	protocol.is_get_qi_yuan_rewards = role_info.is_get_qi_yuan_rewards;
	protocol.battle_reward_flag = role_info.battle_reward_flag;

	EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
}

void SpecialLogicQingYuanShengHui::SendQuestionOpRoute(const std::set<int> & send_roles, bool is_open)
{
	Protocol::SCQingYuanShengHuiQuestionRoute protocol;
	protocol.is_open = is_open;

	for (std::set<int>::iterator iter = send_roles.begin(); iter != send_roles.end(); ++iter)
	{
		Role * role = m_scene->GetRoleByUID(*iter);
		if (role)
		{
			EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
		}
	}
}

void SpecialLogicQingYuanShengHui::SendQuestionInfo(const QingYuanShengHuiQuestiontInfo & question_info)
{
	Protocol::SCQingYuanShengHuiQuestionInfo protocol;
	protocol.question_id = question_info.question_id;
	protocol.end_time = (unsigned int)question_info.next_question_time;
	protocol.quest_num = question_info.question_num;

	for (std::set<int>::iterator iter = question_info.role_group.begin(); iter != question_info.role_group.end(); ++iter)
	{
		Role * role = m_scene->GetRoleByUID(*iter);
		if (role)
		{
			EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
		}
	}	
}

QingYuanShengHuiRoleInfo * SpecialLogicQingYuanShengHui::GetQingYuanShengHuiRoleInfo(int role_id, bool is_create)
{
	return MapValuesPtr(m_role_info, role_id);
}

QingYuanShengHuiQuestiontInfo * SpecialLogicQingYuanShengHui::GetQuestiontInfo(int role_id, bool is_create)
{
	const QingYuanShengHuiRoleInfo * role_info_ptr = MapValuesConstPtr(m_role_info, role_id);
	if (!role_info_ptr)
	{
		return NULL;
	}

	QingYuanShengHuiRoleGroup key(role_info_ptr->role_id, role_info_ptr->qing_yuan_role_id);
	QingYuanShengHuiQuestiontInfo * question_info_ptr = MapValuesPtr(m_question_info, key);
	if (!question_info_ptr && is_create)
	{
		QingYuanShengHuiQuestiontInfo tmp;
		tmp.role_group = key.role_group;
		std::pair<std::map<QingYuanShengHuiRoleGroup, QingYuanShengHuiQuestiontInfo>::iterator, int> ret_iter = m_question_info.insert(std::make_pair(key, tmp));
		question_info_ptr = &ret_iter.first->second;
	}

	return question_info_ptr;
}

bool SpecialLogicQingYuanShengHui::CheckQingYuanShengHuiQuestiontInfo(QingYuanShengHuiQuestiontInfo & question_info, time_t now_second)
{
	if (question_info.is_finish)
	{
		return false;
	}

	if (question_info.question_id != 0)
	{
		bool is_next = false;
		bool is_same_answer = false;
		if (question_info.answer_list.size() >= 2)
		{
			is_next = true;
			is_same_answer = question_info.answer_list.begin()->second == question_info.answer_list.rbegin()->second;
		}

		if (!is_next && now_second > question_info.next_question_time)
		{
			is_next = true;
		}

		if (!is_next)
		{
			return false;
		}

		std::vector<ItemConfigData> rewards;
		const QingYuanShengHuiQuestionCfg * question_cfg = LOGIC_CONFIG->GetActivityQingYuanShengHuiConfig()->GetQingYuanShengHuiQuestionCfg(question_info.question_id);
		if (question_cfg)
		{
			rewards = is_same_answer ? question_cfg->same_answer_rewards : question_cfg->diff_answer_rewards;
		}

		Protocol::SCQingYuanShengHuiQuestionResult question_result;
		question_result.is_same = is_same_answer;
		question_result.question_id = question_info.question_id;

		const std::set<int> & role_group = question_info.role_group;
		for (std::set<int>::iterator iter = role_group.begin(); iter != role_group.end(); ++iter)
		{
			Role * role = m_scene->GetRoleByUID(*iter);
			if (!role)
			{
				continue;
			}

			role->GetRoleModuleManager()->GetKnapsack()->PutVec(rewards, PUT_REASON_QING_YUAN_SHENG_HUI);
			EngineAdapter::Instance().NetSend(role->GetNetId(), &question_result, sizeof(question_result));
		}
	}

	const QingYuanShengHuiOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityQingYuanShengHuiConfig()->GetQingYuanShengHuiOtherCfg();
	if (question_info.question_num >= other_cfg.max_question_num)
	{
		question_info.is_finish = true;
		return true;
	}

	int question_id = LOGIC_CONFIG->GetActivityQingYuanShengHuiConfig()->GetRandQuestionId(question_info.answered_question_id_list);

	if (question_id != 0)
	{
		question_info.question_id = question_id;
		question_info.answered_question_id_list.insert(question_id);
	}

	question_info.answer_list.clear();
	question_info.question_num += 1;
	question_info.next_question_time = now_second + other_cfg.question_interval;

	return true;
}

bool SpecialLogicQingYuanShengHui::PutReward(Role * role, const QingYuanShengHuiGroupRewardItemCfg * reward_cfg)
{
	if (!role || !reward_cfg)
	{
		return false;
	}
	
	if (!role->GetRoleModuleManager()->GetKnapsack()->PutVec(reward_cfg->rewards, PUT_REASON_QING_YUAN_SHENG_HUI))
	{
		role->NoticeNum(errornum::EN_HAS_NO_SPACING);
		return false;
	}

	if (reward_cfg->buff_id > 0)
	{
		//todo gfp
	}

	return true;
}

void SpecialLogicQingYuanShengHui::KickOutQingYuanRole(Role * role)
{
	int uid = role->GetUID();
	QingYuanShengHuiRoleInfo * info = this->GetQingYuanShengHuiRoleInfo(uid);
	if (!info)
	{
		return;
	}

	Role * qing_yuan_role = m_scene->GetRoleByUID(info->qing_yuan_role_id);
	this->DelayKickOutRole(qing_yuan_role);
}
