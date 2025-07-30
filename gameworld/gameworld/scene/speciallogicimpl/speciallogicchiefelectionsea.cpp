#include "config/logicconfigmanager.hpp"
#include "scene/scene.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "engineadapter.h"
#include "global/activity/activitymanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "gamelog.h"
#include "battle/battle_manager_local.hpp"
#include "global/usercache/usercache.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"

#include "servercommon/errornum.h"
#include "protocol/msgchiefelection.h"
#include "config/activityconfig/chiefelection/activitychiefelectionconfig.hpp"
#include "global/activity/activityimpl/activitychiefelection.hpp"
#include "speciallogicchiefelectionsea.hpp"
#include "other/event/eventhandler.hpp"

SpecialLogicChiefElectionSea::SpecialLogicChiefElectionSea(Scene * scene) :SpecialLogic(scene)
{
	m_which_prof_match = 0;
	this->Reset();
}

SpecialLogicChiefElectionSea::~SpecialLogicChiefElectionSea()
{

}

void SpecialLogicChiefElectionSea::Reset()
{
	m_exist_role_num = 0;
	m_start_fight = false;
	m_is_finish = false;

	m_need_update_rank = false;
	m_next_update_rank_time = 0;
	m_rank_info.clear();
	m_role_info.clear();
	m_participate_map.clear();
	m_defeat_protect_time.clear();
}

void SpecialLogicChiefElectionSea::OnRoleEnterScene(Role * role)
{
	if (m_is_finish)
	{
		this->DelayKickOutRole(role);
		return;
	}

	if (m_start_fight)
	{
		std::map<int, ChiefElectionSeaRoleInfo>::iterator iter = m_role_info.find(role->GetUID());
		if (iter == m_role_info.end() || !iter->second.re_link || iter->second.is_lose)
		{
			this->DelayKickOutRole(role);
			return;
		}
	}
	int role_id = role->GetUID();

	MapAdd(m_role_info, role_id, role_id);

	m_exist_role_num += 1;
	this->SendSceneInfo();
	this->SendRoleInfo(role);
	this->SendRankInfo(role);
	this->SendAdvanceFetchInfo(role);
}

void SpecialLogicChiefElectionSea::OnRoleLeaveScene(Role * role, bool is_logout)
{
	if (!m_is_finish && m_start_fight && is_logout)
	{
		std::map<int, ChiefElectionSeaRoleInfo>::iterator iter = m_role_info.find(role->GetUID());
		if (iter != m_role_info.end())
		{
			iter->second.re_link = true;
		}
	}

	m_exist_role_num -= 1;
	this->SendSceneInfo();
	this->CheckFinish();
}

void SpecialLogicChiefElectionSea::OnRemoveObj(Obj * obj)
{
}

bool SpecialLogicChiefElectionSea::CanEnterTeam(Role * role, Team * team, bool is_from_create)
{
	return false;
}

bool SpecialLogicChiefElectionSea::CanInvitationRole(Role * role, int uid)
{
	return false;
}

bool SpecialLogicChiefElectionSea::CanChallengeOthers(Role * role, Role * target)
{
	if (m_is_finish || !m_start_fight)
	{
		return false;
	 }


	std::map<int, time_t>::iterator iter = m_defeat_protect_time.find(target->GetUID());
	if (iter != m_defeat_protect_time.end() && EngineAdapter::Instance().Time() <= iter->second)
	{
		role->NoticeNum(errornum::EN_TARGET_FIGHT_PROTECT);
		return false;
	}

	std::map<int, time_t>::iterator it = m_defeat_protect_time.find(role->GetUID());
	if (it != m_defeat_protect_time.end() && EngineAdapter::Instance().Time() <= it->second)
	{
		role->NoticeNum(errornum::EN_MY_FIGHT_PROTECT);
		return false;
	}

	return true;
}

void SpecialLogicChiefElectionSea::OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack)
{
	if (m_is_finish)
	{
		return;
	}

	if (BATTLE_MODE_CHIEF_ELECTION_SEA != ack->battle_mode)
	{
		return;
	}

	if (ack->attacker_list.role_count > 1 || ack->defender_list.role_count > 1) return;		

	int attack_uid = ack->attacker_list.role_list[0].uid;
	int defender_uid = ack->defender_list.role_list[0].uid;

	switch (ack->battle_result)
	{
		case RESULT_TYPE_ATTACKER_WIN:
			{
				this->SetResult(attack_uid, CHIEF_ELECTION_SEA_RESULT_TYPE_WIN);
				this->SetResult(defender_uid, CHIEF_ELECTION_SEA_RESULT_TYPE_LOSE);
			}
			break;
		case RESULT_TYPE_DEFENDER_WIN:
			{
				this->SetResult(attack_uid, CHIEF_ELECTION_SEA_RESULT_TYPE_LOSE);
				this->SetResult(defender_uid, CHIEF_ELECTION_SEA_RESULT_TYPE_WIN);
			}
			break;
		default:
			{
				this->SetResult(attack_uid, CHIEF_ELECTION_SEA_RESULT_TYPE_DRAW);
				this->SetResult(defender_uid, CHIEF_ELECTION_SEA_RESULT_TYPE_DRAW);
			}
			break;
	}

	m_need_update_rank = true;
}

void SpecialLogicChiefElectionSea::OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway * ack)
{
	this->SetResult(ack->uid, CHIEF_ELECTION_SEA_RESULT_TYPE_LOSE);
}

void SpecialLogicChiefElectionSea::OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight * ack)
{

}

int SpecialLogicChiefElectionSea::GetMineMonsterBattleMode()
{
	return BATTLE_MODE_CHIEF_ELECTION_SEA;
}

bool SpecialLogicChiefElectionSea::CanDestroy()
{
	return false;
}

void SpecialLogicChiefElectionSea::Update(unsigned long interval, time_t now_second)
{
	if (now_second >= m_next_update_rank_time)
	{
		m_next_update_rank_time = now_second + 3;
		this->SortRank();
	}
}

bool SpecialLogicChiefElectionSea::CanChangeProf(Role * role)
{
	return false;
}

void SpecialLogicChiefElectionSea::SendSceneInfo()
{
	Protocol::SCChiefElectionSeaSceneInfo protocol;
	protocol.role_num = m_exist_role_num;

	m_scene->SendToRole((char *)&protocol, sizeof(protocol));
}

void SpecialLogicChiefElectionSea::SendRoleInfo(Role * role)
{
	if (role == NULL)
	{
		return;
	}

	std::map<int, ChiefElectionSeaRoleInfo> ::iterator iter = m_role_info.find(role->GetUID());
	if (iter==m_role_info.end())
	{
		return;
	}

	Protocol::SCChiefElectionSeaRoleInfo protocol;
	protocol.socre = iter->second.socre;
	protocol.win_times = iter->second.win_times;
	protocol.lose_times = iter->second.lose_times;
	protocol.total_times = iter->second.total_times;

	EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
}

void SpecialLogicChiefElectionSea::SendRankInfo(Role * role)
{
	Protocol::SCChiefElectionSeaRank protocol;
	protocol.count = 0;

	for (int i = 0; i < ARRAY_LENGTH(protocol.rank_list) && i < (int)m_rank_info.size(); ++i)
	{
		protocol.rank_list[i].role_id = m_rank_info[i].role_id;
		protocol.rank_list[i].socre = m_rank_info[i].socre;
		protocol.rank_list[i].win_times = m_rank_info[i].win_times;
		protocol.rank_list[i].lose_times = m_rank_info[i].lose_times;
		protocol.rank_list[i].total_times = m_rank_info[i].total_times;

		UserCacheNode * node = UserCacheManager::Instance().GetUserNode(m_rank_info[i].role_id);
		if (NULL != node)
		{
			node->GetName(protocol.rank_list[i].role_name);
		}

		protocol.count += 1;
	}
	
	if (role == NULL)
	{
		m_scene->SendToRole((char *)&protocol, sizeof(protocol));
	}
	else
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
	}	
}

void SpecialLogicChiefElectionSea::SetSceneInfo(int which_prof_match)
{
	this->m_which_prof_match = which_prof_match;
}

void SpecialLogicChiefElectionSea::OnActivityClose()
{
	m_is_finish = true;
	this->DelayKickOutAllRole();
}

void SpecialLogicChiefElectionSea::OnReady()
{
	this->Reset();
	this->DelayKickOutAllRole();
}

void SpecialLogicChiefElectionSea::OnStart()
{
	int role_num = m_scene->RoleNum();
	for (int i = 0; i < role_num; i++)
	{
		Role * role = m_scene->GetRoleByIndex(i);
		if (NULL == role) continue;

		m_participate_map[role->GetUID()] = false;
		EventHandler::Instance().OnParticipateLimitActivity(role, ACTIVITY_TYPE_CHIEF_ELECTION, __FUNCTION__);
	}

	m_start_fight = true;
	this->CheckFinish();
}

void SpecialLogicChiefElectionSea::CheckFinish()
{
	int role_num = m_exist_role_num;

	if (m_start_fight && !m_is_finish && role_num <= ACTIVITY_CHIEF_ELECTION_FIGTHER_NUM)
	{
		this->OnFinish();
	}
}

void SpecialLogicChiefElectionSea::OnFinish(bool is_from_activity)
{
	if (m_is_finish)
	{
		return;
	}

	gamelog::g_Log_chief_election.printf(LL_INFO, "%s line:%d | prof:%d finish:%d", __FUNCTION__, __LINE__, m_which_prof_match, (int)is_from_activity);
	if (!is_from_activity)
	{
		ActivityChiefElection * activity = (ActivityChiefElection *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_CHIEF_ELECTION);
		if (activity != NULL)
			activity->SetSeaElectionFinish(m_which_prof_match);
		return;
	}

	m_is_finish = true;
	BattleManagerLocal::GetInstance().ForceFinishBattleMode(BATTLE_MODE_CHIEF_ELECTION_SEA);

	this->SortRank(true);

	std::vector<int> sea_election_win_role;		//选出排行榜前16名
	for (int i = 0; i < (int)m_rank_info.size() && i < ACTIVITY_CHIEF_ELECTION_FIGTHER_NUM; ++i)
	{
		const ChiefElectionSeaRoleInfo & role_info=m_rank_info[i];
		gamelog::g_Log_chief_election.printf(LL_INFO, "%s line:%d | prof:%d rank:%d ChiefElectionSeaRoleInfo[role_id:%d socre:%d total_times:%d win_times:%d lose_times:%d is_out:%d]", __FUNCTION__, __LINE__, 
						     m_which_prof_match,i + 1, role_info.role_id, role_info.socre, role_info.total_times, role_info.win_times, role_info.lose_times, (int)role_info.is_lose);
		sea_election_win_role.push_back(role_info.role_id);
	}

	ActivityChiefElection * activity = (ActivityChiefElection *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_CHIEF_ELECTION);
	if (activity != NULL)
		activity->OnSeaElectionFinish(m_which_prof_match, sea_election_win_role);

	//发参与奖励
	for (std::map<int, bool>::iterator iter = m_participate_map.begin(); iter != m_participate_map.end(); iter++)
	{
		if (iter->second)
		{
			continue;
		}

		int role_level = 1;
		Role * tmp_role = World::GetInstWorld()->GetSceneManager()->GetRole(iter->first);
		if (tmp_role != NULL)
		{
			role_level = tmp_role->GetLevel();
		}
		else
		{
			UserCacheNode * user_cache_node = UserCacheManager::Instance().GetUserNode(iter->first);
			if (NULL != user_cache_node)
			{
				role_level = user_cache_node->level;
			}
		}

		const ActivityChiefElectionSeaRewardCfg * cfg = LOGIC_CONFIG->GetActivityChiefElectionConfig()->GetSeaRewardCfg(role_level);
		if (cfg == NULL)
		{
			continue;
		}

		// 发邮件
		static MailContentParam contentparam; contentparam.Reset();
		contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = cfg->coin;
		contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_EXP] = cfg->exp;

		for (int k = 0; k < (int)cfg->rewards.size() && k < MAX_ATTACHMENT_ITEM_NUM; ++k)
		{
			const ItemConfigData & reward = cfg->rewards[k];

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

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_chief_election_sea_reward_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_chief_election_sea_reward_content);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(iter->first, MAIL_REASON_DEFAULT, contentparam);
		}
		iter->second = true;
		this->SendAdvanceFetchInfo(tmp_role);
	}
}

void SpecialLogicChiefElectionSea::OnAdavanceFetch(Role * role, short op_type, int param1)
{
	if (NULL == role) return;

	ActivityChiefElection * activity = (ActivityChiefElection *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_CHIEF_ELECTION);
	if (NULL == activity) return;

	if (!activity->IsActivityOpen())
	{
		role->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return;
	}
	std::map<int, bool>::iterator iter = m_participate_map.find(role->GetUID());
	if (iter == m_participate_map.end())
	{
		return;
	}
	switch (op_type)
	{
		case Protocol::CSActivityAdvanceFetchReq::ACTIVITY_ADVANCE_FETCH_OP_TYPE_PARTICIP_AWARD:
		{
			if (iter->second)
			{
				role->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
				return;
			}

			const ActivityChiefElectionSeaRewardCfg * cfg = LOGIC_CONFIG->GetActivityChiefElectionConfig()->GetSeaRewardCfg(role->GetLevel());
			if (cfg == NULL)
			{
				return;
			}

			if (!cfg->rewards.empty())
			{
				if (!role->GetRoleModuleManager()->GetKnapsack()->PutList((short)cfg->rewards.size(), &cfg->rewards[0], PUT_REASON_CHIEF_ELECTION))
				{
					role->GetRoleModuleManager()->GetKnapsack()->SendMail(&cfg->rewards[0], (int)cfg->rewards.size(), SNED_MAIL_TYPE_DEFAULT, true);
				}
			}

			role->AddExp(cfg->exp, ADD_EXP_TYPE_CHIEF_ELECTION, __FUNCTION__);
			role->GetRoleModuleManager()->GetMoney()->AddCoinBind(cfg->coin, __FUNCTION__);
			iter->second = true;
			this->SendAdvanceFetchInfo(role);
		}
		break;
	default:
		break;
	}
}

void SpecialLogicChiefElectionSea::SendAdvanceFetchInfo(Role * role)
{
	if (NULL == role) return;
	Protocol::SCActivityAdvanceFetchInfo info;
	info.activity_type = ACTIVITY_TYPE_CHIEF_ELECTION;
	std::map<int, bool>::iterator iter = m_participate_map.find(role->GetUID());
	if(iter == m_participate_map.end())
	{
		info.particip_award_flag = 0;
	}
	else
	{
		info.particip_award_flag = iter->second ? 1 : 0;
	}
	info.level = role->GetLevel();
	info.reserve_sh = 0;
	info.survival_award_flag = 0;
	EngineAdapter::Instance().NetSend(role->GetNetId(), &info, sizeof(info));
}

void SpecialLogicChiefElectionSea::SetResult(int role_id, int result_type)
{
	if (role_id == 0)
	{
		return;
	}

	ChiefElectionSeaRoleInfo & role_info = m_role_info[role_id];
	role_info.role_id = role_id;
	role_info.total_times += 1;
	role_info.time =EngineAdapter::Instance().Time();
	
	const ChiefElectionOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityChiefElectionConfig()->GetOhterCfg();
	if (CHIEF_ELECTION_SEA_RESULT_TYPE_WIN == result_type)
	{
		role_info.socre += other_cfg.win_score;
		role_info.win_times += 1;
	}

	if (CHIEF_ELECTION_SEA_RESULT_TYPE_LOSE == result_type)
	{
		role_info.socre += other_cfg.lose_score;
		role_info.lose_times += 1;
	}

	if (CHIEF_ELECTION_SEA_RESULT_TYPE_DRAW == result_type)
	{
		role_info.socre += other_cfg.draw_score;
	}

	Role * role = m_scene->GetRoleByUID(role_id);

	this->SendRoleInfo(role);

	if (role_info.lose_times >= other_cfg.lose_times_kickout)
	{
		role_info.is_lose = true;
		this->DelayKickOutRole(role);
	}

	gamelog::g_Log_chief_election.printf(LL_INFO, "%s line:%d | prof:%d ChiefElectionSeaRoleInfo[role_id:%d socre:%d total_times:%d win_times:%d lose_times:%d is_out:%d] result_type:%d", __FUNCTION__, __LINE__, m_which_prof_match,
					     role_info.role_id, role_info.socre, role_info.total_times, role_info.win_times, role_info.lose_times, (int)role_info.is_lose, result_type);

	m_defeat_protect_time[role_id] = EngineAdapter::Instance().Time() + 30;
}

void SpecialLogicChiefElectionSea::SortRank(bool is_focre)
{
	if (!m_need_update_rank && !is_focre)
	{
		return;
	}
	m_need_update_rank = false;

	m_rank_info.clear();

	for (std::map<int, ChiefElectionSeaRoleInfo>::iterator iter = m_role_info.begin(); iter != m_role_info.end(); ++iter)
	{
		Role * role = m_scene->GetRoleByUID(iter->second.role_id);
		if (role == NULL)
		{
			continue;
		}

		m_rank_info.push_back(iter->second);
	}

	std::sort(m_rank_info.begin(), m_rank_info.end());
	this->SendRankInfo();
}

bool ChiefElectionSeaRoleInfo::operator<(const ChiefElectionSeaRoleInfo & other) const
{
	if (socre > other.socre)
		return true;
	if (socre < other.socre)
		return false;

	double win_rate = win_times / (double)total_times;
	double other_win_rate = other.win_times / (double)other.total_times;

	if (win_rate > other_win_rate)
		return true;
	if (win_rate < other_win_rate)
		return false;

	if (time < other.time)
		return true;
	if (time > other.time)
		return false;

	if (role_id < other.role_id)
		return true;
	if (role_id > other.role_id)
		return false;

	return false;
}

ChiefElectionSeaRoleInfo::ChiefElectionSeaRoleInfo(int role_id)
{
	this->role_id = role_id;
	this->socre = 0;
	this->win_times = 0;
	this->lose_times = 0;
	this->total_times = 0;
	this->time = EngineAdapter::Instance().Time();
	re_link = false;
	is_lose = false;
}

ChiefElectionSeaRoleInfo::ChiefElectionSeaRoleInfo()
{
	this->role_id = 0;
	this->socre = 0;
	this->win_times = 0;
	this->lose_times = 0;
	this->total_times = 0;
	this->time = EngineAdapter::Instance().Time();
	re_link = false;
	is_lose = false;
}
