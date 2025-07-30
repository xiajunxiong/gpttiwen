#include "activityteamfight.hpp"
#include "config/activityconfig/teamfight/activityteamfightconfig.hpp"
#include "obj/character/role.h"
#include "battle/battle_manager_local.hpp"
#include "world.h"
#include "internalcomm.h"
#include "scene/scenemanager.hpp"
#include "global/team/team.hpp"
#include "global/team/teammanager.hpp"
#include "global/activity/activitymanager.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "config/activityconfig/teamfight/activityteamfightconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "gamelog.h"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"
#include "protocol/msgactivity.hpp"
#include "servercommon/errornum.h"
#include "global/usercache/usercache.hpp"
#include "other/funopen/funopen.hpp"
#include "other/title/title.hpp"
#include "other/event/eventhandler.hpp"
#include "protocol/msgmatch.h"
#include "servercommon/matchmanager/matchengine.hpp"
#include "global/offlineregister/offlineregistermanager.hpp"
#include "global/worldstatus/worldstatus.hpp"

bool MatchLevel(MatchTeamData* a, MatchTeamData* b)
{
	if (NULL == a || NULL == b) return false;
	return abs(a->GetTotalLevel() - b->GetTotalLevel()) <= 25;
}

ActivityTeamFight::ActivityTeamFight(ActivityManager * activity_manager) : Activity(activity_manager, ACTIVITY_TYPE_TEAM_FIGHT)
{
	m_match_mgr = new ActivityTeamFightMatchManager(this);
	m_match_mgr->Init();
	m_check_start_battle_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
}

ActivityTeamFight::~ActivityTeamFight()
{
	delete m_match_mgr;
	m_match_mgr = NULL;
}

bool SortRank(const RoleInfo& a, const RoleInfo& b)
{
	return a._score > b._score;
}

void ActivityTeamFight::OnActivityStatusChange(int from_status, int to_status)
{
	Activity::OnActivityStatusChange(from_status, to_status);

	switch (to_status)
	{
	case ACTIVITY_STATUS_OPEN:
		{
			m_role_list.clear();
			m_record_list.clear();
		}
		break;

	case ACTIVITY_STATUS_CLOSE:
		{
			// 发放奖励
			std::sort(m_role_list.begin(), m_role_list.end(), SortRank);

			int rank = 1;
			int max_rank = LOGIC_CONFIG->GetActivityTeamFightConfig()->GetMaxRank();
			for (RoleList::const_iterator it = m_role_list.begin(); it != m_role_list.end() && rank < max_rank; ++it, ++rank)
			{
				//参与奖励
				if(0 == it->_join_award_flag)
				{
					
					const TeamFightJoinAwardCfg * join_cfg = LOGIC_CONFIG->GetActivityTeamFightConfig()->GetTeamFightJoinAwardCfg(it->_level);
					if (NULL != join_cfg)
					{
						static MailContentParam contentparam; contentparam.Reset();
						contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = join_cfg->coin;
						contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_EXP] = join_cfg->exp;
						if (!join_cfg->reward_list.empty())
						{
							for (int award_idx = 0; award_idx < (int)join_cfg->reward_list.size() && award_idx < MAX_ATTACHMENT_ITEM_NUM; ++award_idx)
							{
								const ItemConfigData & award_obj = join_cfg->reward_list[award_idx];
								contentparam.item_list[award_idx] = ItemDataWrapper(award_obj.item_id, award_obj.num, award_obj.is_bind ? 1 : 0);
							}
						}
						int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_team_fight_join_reward_subject);
						int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_team_fight_join_reward_content);
						if (lensub > 0 && length > 0)
						{
							MailRoute::Instance().MailToUser(it->_uid, MAIL_REASON_DEFAULT, contentparam);
						}

						unsigned int fetch_reward_zero_timestamp = (unsigned int)EngineAdapter::Instance().Time() + EngineAdapter::Instance().NextDayInterval(0, 0, 0);
						Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(it->_uid);
						if (NULL != role)
						{
							role->GetRoleModuleManager()->GetCommonData().fetch_team_fight_join_reward_flag = 1;
							role->GetRoleModuleManager()->GetCommonData().fetch_special_logic_activity_reward_zero_timestamp[DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_TEAM_FIGHT] = fetch_reward_zero_timestamp;
						}
						else
						{
							crossgameprotocal::HiddenGameSpecialLogicActivityInfoSync hgslais;
							hgslais.role_id = it->_uid;
							hgslais.special_logic_activity_type = DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_TEAM_FIGHT;
							hgslais.fetch_reward_zero_timestamp = fetch_reward_zero_timestamp;

							InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(it->_uid).db_index, (const char *)&hgslais, sizeof(hgslais));
						}
					}
				}

				//未领取的宝箱奖励邮件补发
				{
					if (it->_victory_count >= 1)				//没有配置,写死
					{
						if ((it->_box_fetch_flag & (1 << ACT_TEAM_FIGHT_BOX_TYPE_FIRST_WIN)) == 0)
						{	
							if (LOGIC_CONFIG->GetActivityTeamFightConfig()->MailReissueBoxReward(ACT_TEAM_FIGHT_BOX_TYPE_FIRST_WIN, it->_level, it->_uid))
							{
								gamelog::g_log_team_fight.printf(LL_INFO, "reissue first box Reward Succ user[%d] Rank[%d] Level[%d]", it->_uid, rank, it->_level);
							}
						}
					}
					if (it->_victory_count + it->_defeat_count >= 3)
					{
						if ((it->_box_fetch_flag & (1 << ACT_TEAM_FIGHT_BOX_TYPE_THIRD_BATTLE)) == 0)
						{
							if (LOGIC_CONFIG->GetActivityTeamFightConfig()->MailReissueBoxReward(ACT_TEAM_FIGHT_BOX_TYPE_THIRD_BATTLE, it->_level, it->_uid))
							{
								gamelog::g_log_team_fight.printf(LL_INFO, "reissue three fight box Reward Succ user[%d] Rank[%d] Level[%d]", it->_uid, rank, it->_level);
							}
						}
					}
				}

				const ActTeamFightRankCfg * cfg = LOGIC_CONFIG->GetActivityTeamFightConfig()->GetRankCfg(rank, it->_level);
				if (NULL == cfg)
				{
					gamelog::g_log_team_fight.printf(LL_INFO, "%s Error -1 Rank[%d]", __FUNCTION__, rank);
					continue;
				}

				Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(it->_uid));
				if (NULL != role)
				{
					if (Scene::SCENE_TYPE_TEAM_FIGHT == role->GetScene()->GetSceneType())
					{
						this->SyncActivityDataToRoleScore(role);
						role->NoticeNum(errornum::EN_ACTIVITY_IS_END);
						role->GetScene()->GetSpecialLogic()->DelayKickOutRole(role);
					}

					unsigned int due_time = EngineAdapter::Instance().NextWeekInterval(m_next_open_wday, 0, 0, 0);
					role->GetRoleModuleManager()->GetTitle()->AddTitle(cfg->title_id, __FUNCTION__, due_time);
				}

				// 发排名奖励邮件
				{
					static MailContentParam contentparam; contentparam.Reset();
					contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_EXP] = cfg->exp;
					contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = cfg->bind_coin;

					int item_count = 0;
					for (int i = 0; i < (int)cfg->reward_list.size() && item_count < MAX_ATTACHMENT_ITEM_NUM; ++i)
					{
						
						contentparam.item_list[item_count].item_id = cfg->reward_list[i].item_id;
						contentparam.item_list[item_count].is_bind = cfg->reward_list[i].is_bind ? 1 : 0;
						contentparam.item_list[item_count].num = cfg->reward_list[i].num;
						item_count += 1;
					}
					int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_team_fight_reward_subject);
					int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_team_fight_rank_reward, rank);
					if (lensub > 0 && length > 0)
					{
						MailRoute::MailToUser(IntToUid(it->_uid), MAIL_REASON_DEFAULT, contentparam);
						gamelog::g_log_team_fight.printf(LL_INFO, "Send Rank Reward Succ user[%d] Rank[%d] Level[%d]", it->_uid, rank, it->_level);
					}
					EventHandler::Instance().OnRoleFinishActivityTeamFight(role,rank);
				}
			}

			m_role_list.clear();
			m_record_list.clear();
			WorldStatus::Instance().SetFindEndTimestamp(DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_TEAM_FIGHT);
			BattleManagerLocal::GetInstance().ForceFinishBattleMode(BATTLE_MODE_SQUARD_FIGHT);
		}
		break;
	}
}

void ActivityTeamFight::Update(unsigned long interval, time_t now_second)
{
	Activity::Update(interval, now_second);
	unsigned int now = static_cast<unsigned int>(now_second);
	if (this->IsActivityOpen())
	{
		m_match_mgr->Update(interval);
		if (now >= m_check_start_battle_time)
		{
			this->CheckStartBattle(now);
			m_check_start_battle_time = now + 1;
		}
	}
}

void ActivityTeamFight::OnMonthChange()
{
}

void ActivityTeamFight::OnMatchReq(Role * role)
{
	if (!this->IsActivityOpen())return;
	if (NULL == role) return;

	Team* team = role->GetMyTeam();
	if (NULL != team)
	{
		if (!team->IsLeader(role))
		{
			role->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
			return;
		}

		if (team->HasDismissMember())
		{
			role->NoticeNum(errornum::EN_HAS_DISMISS_MEMBER_IN_TEAM);
			return;
		}
		int team_count = team->GetMemberCount();
		for (int i = 0; i < team_count && i < MAX_TEAM_MEMBER_NUM; i++)
		{
			Role* member_role = team->GetMemberRoleByIndex(i);
			if (NULL != member_role && !team->IsLeader(member_role))
			{
				WorldStatus::Instance().LogFunctionStats(member_role, "ActivityTeamFight");
			}
		}
	}

	m_match_mgr->RoleStartMatchReq(role);
	WorldStatus::Instance().LogFunctionStats(role, "ActivityTeamFight");
}

void ActivityTeamFight::OnCancelReq(Role * role)
{
	if (NULL == role) return;

	m_match_mgr->RoleStopMatchReq(role);
}

void ActivityTeamFight::OnUserLogin(Role * role)
{
	Activity::OnUserLogin(role);	

	{
		// 特殊处理，主角登录时让其处于非匹配状态
		Protocol::SCStopMatch msg;
		msg.business_type = m_match_mgr->GetMatchEngine()->GetMatchLogicType();
		msg.reserve_sh = 0;
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&msg, sizeof(msg));
	}
}

void ActivityTeamFight::OnUserLogout(Role * role)
{
	Activity::OnUserLogout(role);

	m_match_mgr->RoleStopMatchReq(role);
}

void ActivityTeamFight::FightRet(int uid, bool win, int score, bool is_draw)
{
	RoleList::iterator it = std::find(m_role_list.begin(), m_role_list.end(), uid);
	if (it != m_role_list.end())
	{
		it->_score += score;
		if (it->_score < 0) it->_score = 0;

		it->_battle_count += 1;

		if (win)
		{
			// 赢了
			it->_victory_count++;
			it->_continue_victory_count++;
		}
		else if(!is_draw)
		{
			// 输了
			it->_defeat_count++;
			it->_continue_victory_count = 0;
		}
		else
		{
			// 平局
			it->_continue_victory_count = 0;
		}

		gamelog::g_log_activity.printf(LL_INFO, "ActivityTeamFight::FightRet uid[%d] win[%s] mod_score[%d] after_score[%d]", uid, win ? "true" : "false", score, it->_score);

		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(uid);
		if (NULL != role)
		{
			this->SendRoleInfo(role);
		}
	}
}

void ActivityTeamFight::FightRecord(int user_list1[MAX_TEAM_MEMBER_NUM], int user_list2[MAX_TEAM_MEMBER_NUM])
{
	FightRecordData data;
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; i++)
	{
		data.user_list1[i] = user_list1[i];
		data.user_list2[i] = user_list2[i];
	}
	m_record_list.insert(m_record_list.begin(), data);

	if ((int)m_record_list.size() > Protocol::RECORD_MAX_COUNT)
	{
		m_record_list.resize(Protocol::RECORD_MAX_COUNT);
	}

	std::sort(m_role_list.begin(), m_role_list.end(), SortRank);
}

void ActivityTeamFight::OnFetchBoxReward(Role * role, int box_type)
{
	if(NULL == role) return;

	if(box_type < ACT_TEAM_FIGHT_BOX_TYPE_FIRST_WIN || box_type > ACT_TEAM_FIGHT_BOX_TYPE_THIRD_BATTLE) return;

	RoleList::iterator it = std::find(m_role_list.begin(), m_role_list.end(), role->GetUID());
	if (it != m_role_list.end())
	{
		if (box_type == ACT_TEAM_FIGHT_BOX_TYPE_FIRST_WIN)
		{
			if (it->_victory_count >= 1)				//没有配置,写死
			{
				if ((it->_box_fetch_flag & (1 << ACT_TEAM_FIGHT_BOX_TYPE_FIRST_WIN)) == 0)
				{
					if (LOGIC_CONFIG->GetActivityTeamFightConfig()->SendBoxReward(role, ACT_TEAM_FIGHT_BOX_TYPE_FIRST_WIN, role->GetLevel()))
					{
						it->_box_fetch_flag |= (1 << ACT_TEAM_FIGHT_BOX_TYPE_FIRST_WIN);
					}
				}
				else
				{
					role->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
				}
			}
		}
		else if(box_type == ACT_TEAM_FIGHT_BOX_TYPE_THIRD_BATTLE)
		{
			if (it->_victory_count + it->_defeat_count >= 3)
			{
				if ((it->_box_fetch_flag & (1 << ACT_TEAM_FIGHT_BOX_TYPE_THIRD_BATTLE)) == 0)
				{
					if (LOGIC_CONFIG->GetActivityTeamFightConfig()->SendBoxReward(role, ACT_TEAM_FIGHT_BOX_TYPE_THIRD_BATTLE, role->GetLevel()))
					{
						it->_box_fetch_flag |= (1 << ACT_TEAM_FIGHT_BOX_TYPE_THIRD_BATTLE);
					}
				}
				else
				{
					role->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
				}
			}
		}
		this->SendRoleInfo(role);
	}
}

void ActivityTeamFight::OnAdavanceFetch(Role * role, short op_type, int param1)
{
	if (NULL == role) return;

	ActivityTeamFight * activity = (ActivityTeamFight *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_TEAM_FIGHT);
	if (NULL == activity) return;

	if (!activity->IsActivityOpen())
	{
		role->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return;
	}

	RoleList::iterator it = std::find(m_role_list.begin(), m_role_list.end(), role->GetUID());
	if(it == m_role_list.end()) return;

	switch (op_type)
	{
		case Protocol::CSActivityAdvanceFetchReq::ACTIVITY_ADVANCE_FETCH_OP_TYPE_PARTICIP_AWARD:
		{
			if (0 != it->_join_award_flag)
			{
				role->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
				return;
			}
			const TeamFightJoinAwardCfg * join_cfg = LOGIC_CONFIG->GetActivityTeamFightConfig()->GetTeamFightJoinAwardCfg(it->_level);
			if (NULL == join_cfg)
			{
				return;
			}
			if (!join_cfg->reward_list.empty())
			{
				if (!role->GetRoleModuleManager()->GetKnapsack()->PutList((short)join_cfg->reward_list.size(), &join_cfg->reward_list[0], PUT_REASON_TEAM_FIGHT))
				{
					role->GetRoleModuleManager()->GetKnapsack()->SendMail(&join_cfg->reward_list[0], (int)join_cfg->reward_list.size(), SNED_MAIL_TYPE_DEFAULT, true);
				}
			}
			role->AddExp(join_cfg->exp, ADD_EXP_TYPE_TEAM_FIGHT, __FUNCTION__);
			role->GetRoleModuleManager()->GetMoney()->AddCoinBind(join_cfg->coin, __FUNCTION__);
			it->_join_award_flag = 1;
			this->SendAdvanceFetchInfo(role);

			unsigned int fetch_reward_zero_timestamp = (unsigned int)EngineAdapter::Instance().Time() + EngineAdapter::Instance().NextDayInterval(0, 0, 0);
			Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(it->_uid);
			if (NULL != role)
			{
				role->GetRoleModuleManager()->GetCommonData().fetch_team_fight_join_reward_flag = 1;
				role->GetRoleModuleManager()->GetCommonData().fetch_special_logic_activity_reward_zero_timestamp[DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_TEAM_FIGHT] = fetch_reward_zero_timestamp;
			}
			else
			{
				crossgameprotocal::HiddenGameSpecialLogicActivityInfoSync hgslais;
				hgslais.role_id = it->_uid;
				hgslais.special_logic_activity_type = DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_TEAM_FIGHT;
				hgslais.fetch_reward_zero_timestamp = fetch_reward_zero_timestamp;

				InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(it->_uid).db_index, (const char *)&hgslais, sizeof(hgslais));
			}
		}
		break;
	default:
		break;
	}
}

void ActivityTeamFight::SendAdvanceFetchInfo(Role * role)
{
	if (NULL == role) return;
	Protocol::SCActivityAdvanceFetchInfo info;
	info.activity_type = ACTIVITY_TYPE_TEAM_FIGHT;
	RoleList::iterator it = std::find(m_role_list.begin(), m_role_list.end(), role->GetUID());
	if (it == m_role_list.end())
	{
		info.level = role->GetLevel();
		info.particip_award_flag = 0;
		info.survival_award_flag = 0;
	}
	else
	{
		info.level = it->_level;
		info.particip_award_flag = it->_join_award_flag;
	}

	info.reserve_sh = 0;
	info.survival_award_flag = 0;
	EngineAdapter::Instance().NetSend(role->GetNetId(), &info, sizeof(info));
}

void ActivityTeamFight::SyncRoleScoreToActivityData(Role* role)
{
	if (NULL == role)return;

	RoleList::iterator it = std::find(m_role_list.begin(), m_role_list.end(), role->GetUID());
	if (it == m_role_list.end())
	{
		RoleInfo info;
		info._uid = role->GetUID();
		info._score = role->GetLevel() * 10;
		info._victory_count = 0;
		info._defeat_count = 0;
		info._battle_count = 0;
		info._box_fetch_flag = 0;
		info._level = role->GetLevel();
		m_role_list.push_back(info);
		std::sort(m_role_list.begin(), m_role_list.end(), SortRank);
	}
	else
	{
		it->_level = role->GetLevel();
	}
}

void ActivityTeamFight::SyncActivityDataToRoleScore(Role* role)
{
	if (NULL == role) return;

	RoleList::iterator it = std::find(m_role_list.begin(), m_role_list.end(), role->GetUID());
	if (it != m_role_list.end())
	{
		this->SyncActivityDataToRoleScore(*it, role);
	}
}

void ActivityTeamFight::SyncActivityDataToRoleScore(const RoleInfo& info, Role* role)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetRoleActivityManager()->GetCommonParam().team_fight_score = info._score;
	//role->GetRoleModuleManager()->GetRoleActivity()->GetAcitivtyParam().team_fight_victory_count = info._victory_count;
	//role->GetRoleModuleManager()->GetRoleActivity()->GetAcitivtyParam().team_fight_defeat_count = info._defeat_count;
	//role->GetRoleModuleManager()->GetRoleActivity()->GetAcitivtyParam().team_fight_box_fetch_flag = info._box_fetch_flag;
}

void ActivityTeamFight::SendRoleInfo(Role * role)
{
	if (NULL == role) return;

	RoleList::iterator it = std::find(m_role_list.begin(), m_role_list.end(), role->GetUID());
	if (it == m_role_list.end())return;

	Protocol::SCActivityTeamFightScore info;
	info.score = it->_score;
	info.rank = (it - m_role_list.begin()) + 1;
	info.victory_count = it->_victory_count;
	info.defeat_count = it->_defeat_count;
	info.continue_victory_count = it->_continue_victory_count;
	info.box_fetch_flag = it->_box_fetch_flag;
	info.battle_count = it->_battle_count;

	EngineAdapter::Instance().NetSend(role->GetNetId(), &info, sizeof(info));
}

void ActivityTeamFight::SendRankInfo(Role * role)
{
	if (NULL == role) return;

	static Protocol::SCActivityTeamFightRank info;
	info.count = 0;

	for (RoleList::iterator it = m_role_list.begin(); it != m_role_list.end() && info.count < Protocol::RANK_MAX_COUNT; ++it)
	{
		UserCacheNode * node = UserCacheManager::Instance().GetUserNode(it->_uid);
		if (NULL != node)
		{
			Protocol::SCActivityTeamFightRank::UserData& data = info.rank_list[info.count];
			node->GetName(data.name);
			data.score = it->_score;
			int total_count = it->_victory_count + it->_defeat_count;
			data.win_rate = total_count > 0 ? (int)(it->_victory_count * 100 / total_count) : 0;
			data.level = it->_level;
			info.count++;
		}
	}

	int len = sizeof(info) - sizeof(info.rank_list) + (info.count * sizeof(Protocol::SCActivityTeamFightRank::UserData));
	EngineAdapter::Instance().NetSend(role->GetNetId(), &info, len);
}

void ActivityTeamFight::SendRecordInfo(Role * role, bool only_me)
{
	static Protocol::SCActivityTeamFightRecord info;
	info.count = 0;

	for (FightRecordList::const_iterator it = m_record_list.begin(); it != m_record_list.end() && info.count < Protocol::RECORD_MAX_COUNT; ++it)
	{
		bool is_push = false;
		if (only_me)
		{
			for (int i = 0; i < MAX_TEAM_MEMBER_NUM; i++)
			{
				if (it->user_list1[i] == role->GetUID() ||
					it->user_list2[i] == role->GetUID())
				{
					is_push = true;
					break;
				}
			}
		}
		else
		{
			is_push = true;
		}

		if (is_push)
		{
			Protocol::SCActivityTeamFightRecord::UserData& data = info.record_list[info.count];
			UserCacheNode * node = UserCacheManager::Instance().GetUserNode(it->user_list1[0]);
			if (NULL != node)
			{
				node->GetName(data.name1);
			}
			node = UserCacheManager::Instance().GetUserNode(it->user_list2[0]);
			if (NULL != node)
			{
				node->GetName(data.name2);
			}

			info.count++;
		}
	}

	int len = sizeof(info) - sizeof(info.record_list) + (info.count * sizeof(Protocol::SCActivityTeamFightRecord::UserData));
	EngineAdapter::Instance().NetSend(role->GetNetId(), &info, len);
}

void ActivityTeamFight::OnMatchEnemy(MatchTeam* mtA, MatchTeam* mtB)
{
	ActTeamFightMatchGroup mg;
	mg.attacker_list.reserve(mtA->user_list.size());
	mg.defender_list.reserve(mtB->user_list.size());
	mg.fight_start_time = 
		LOGIC_CONFIG->GetActivityTeamFightConfig()->GetOtherCfg()->start_battle_countdown 
		+ static_cast<unsigned int>(EngineAdapter::Instance().Time());

	static Role* attacker_role_list[MAX_TEAM_MEMBER_NUM];
	static Role* defender_role_list[MAX_TEAM_MEMBER_NUM];

	int atker_count = 0;
	int defer_count = 0;
	for (size_t i = 0; i < mtA->user_list.size(); ++i)
	{
		mg.attacker_list.push_back(mtA->user_list[i]->uid);
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(mtA->user_list[i]->uid));
		if (NULL != role)
		{
			attacker_role_list[atker_count++] = role;
		}
	}
	for (size_t i = 0; i < mtB->user_list.size(); ++i)
	{
		mg.defender_list.push_back(mtB->user_list[i]->uid);
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(mtB->user_list[i]->uid));
		if (NULL != role)
		{
			defender_role_list[defer_count++] = role;
		}
	}
	m_match_group.push(mg);

	for (int i = 0; i < atker_count && i < MAX_TEAM_MEMBER_NUM; i++)
	{
		m_match_mgr->RoleStopMatchReq(attacker_role_list[i]);
	}
	for (int i = 0; i < defer_count && i < MAX_TEAM_MEMBER_NUM; i++)
	{
		m_match_mgr->RoleStopMatchReq(defender_role_list[i]);
	}
}

bool ActivityTeamFight::StartBattle(const ActTeamFightMatchGroup& mg)
{
	static Role* attacker_role_list[MAX_TEAM_MEMBER_NUM];
	static Role* defender_role_list[MAX_TEAM_MEMBER_NUM];

	int atker_count = 0;
	int defer_count = 0;

	for (size_t i = 0; i < mg.attacker_list.size(); ++i)
	{
		int role_id = mg.attacker_list[i];
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
		if (NULL != role)
		{
			attacker_role_list[atker_count++] = role;
		}
		else
		{
			this->FightRet(role_id, false, -3, false);
		}
	}
	for (size_t i = 0; i < mg.defender_list.size(); ++i)
	{
		int role_id = mg.defender_list[i];
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
		if (NULL != role)
		{
			defender_role_list[defer_count++] = role;
		}
		else
		{
			this->FightRet(role_id, false, -3, false);
		}
	}

	if (atker_count <= 0 && defer_count > 0)
	{
		// 有一方全掉线
		for (int i = 0; i < defer_count; ++i)
		{
			this->FightRet(defender_role_list[i]->GetUID(), true, 6, false);
			defender_role_list[i]->NoticeNum(errornum::EN_ACTIVITY_TEAM_FIGHT_ENEMY_ALL_OFFLINE);
		}
	}
	else if (defer_count <= 0 && atker_count > 0)
	{
		// 有一方全掉线
		for (int i = 0; i < atker_count; ++i)
		{
			this->FightRet(attacker_role_list[i]->GetUID(), true, 6, false);
			attacker_role_list[i]->NoticeNum(errornum::EN_ACTIVITY_TEAM_FIGHT_ENEMY_ALL_OFFLINE);
		}
	}
	else
	{
		// 正常开始战斗
		BattleManagerLocal::GetInstance().StartTeamPVPReq(atker_count, attacker_role_list, defer_count, defender_role_list, BATTLE_MODE_SQUARD_FIGHT);
	}

	return true;
}

void ActivityTeamFight::CheckStartBattle(unsigned int now)
{
	if (m_match_group.empty()) return;

	while (!m_match_group.empty())
	{
		ActTeamFightMatchGroup& mg = m_match_group.front();
		if (now < mg.fight_start_time)
		{
			return;
		}

		this->StartBattle(mg);
		m_match_group.pop();
	}
}

void ActivityTeamFight::OnRoleFightRet(int uid, bool is_attacker, int battle_result, int atk_alive_num, int def_alive_num)
{
	if (is_attacker)
	{
		if (RESULT_TYPE_ATTACKER_WIN == battle_result)
		{
			this->FightRet(uid, true, atk_alive_num + 3, false);
		}
		else if (RESULT_TYPE_DEFENDER_WIN == battle_result)
		{
			this->FightRet(uid, false, -def_alive_num, false);
		}
		else if (RESULT_TYPE_DRAW == battle_result
			|| RESULT_TYPE_EXCEED_MAX_ROUND == battle_result)
		{
			this->FightRet(uid, false, 0, true);
		}
	}
	else
	{
		if (RESULT_TYPE_ATTACKER_WIN == battle_result)
		{
			this->FightRet(uid, false, -atk_alive_num, false);
		}
		else if (RESULT_TYPE_DEFENDER_WIN == battle_result)
		{
			this->FightRet(uid, true, def_alive_num + 3, false);
		}
		else if (RESULT_TYPE_DRAW == battle_result
			|| RESULT_TYPE_EXCEED_MAX_ROUND == battle_result)
		{
			this->FightRet(uid, false, 0, true);
		}
	}
}
