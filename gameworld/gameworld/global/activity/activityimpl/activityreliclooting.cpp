#include "activityreliclooting.hpp"
#include "scene/speciallogicimpl/mazeracermanager.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "global/team/team.hpp"
#include "global/team/teammanager.hpp"
#include "scene/fbmanager.hpp"
#include "obj/character/role.h"
#include "scene/speciallogicimpl/speciallogicreliclooting.hpp"
#include "config/activityconfig/reliclooting/activityreliclootingconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "global/rank/rankmanager.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"
#include "internalcomm.h"
#include "gamelog.h"
#include "servercommon/commondata.hpp"
#include "other/rolemodulemanager.hpp"
#include "servercommon/errornum.h"
#include "global/team/teamconfig.hpp"
#include "other/route/mailroute.hpp"
#include "servercommon/matchmanager/matchengine.hpp"

bool MatchLevel3(MatchTeamData* a, MatchTeamData* b)
{
	if (NULL == a || NULL == b) return false;
	return true;
}

bool static SortFunc(const RelicLootRankData &rank_item_1, const RelicLootRankData &rank_item_2)
{
	if (rank_item_1.rank_value > rank_item_2.rank_value)
	{
		return true;
	}
	else if(rank_item_1.rank_value == rank_item_2.rank_value)
	{
		if (rank_item_1.flexible_ll > rank_item_2.flexible_ll)
		{
			return true;
		}
		else if (rank_item_1.flexible_ll == rank_item_2.flexible_ll)
		{
			if (rank_item_1.uid > rank_item_2.uid)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	
}

ActivityRelicLooting::ActivityRelicLooting(ActivityManager * activity_manager) : Activity(activity_manager, ACTIVITY_TYPE_RELIC_LOOTING),
	rank_refresh_tamp(0), m_next_match_time(0)
{
	m_match_logic.SetFunc(MatchLevel3);

	m_match_mgr = new ActivityRelicLootingMatchManager(this);
	m_match_mgr->Init();
}

ActivityRelicLooting::~ActivityRelicLooting()
{
	m_match_logic.Reset();

	delete m_match_mgr;
	m_match_mgr = NULL;
}

void ActivityRelicLooting::OnActivityStatusChange(int from_status, int to_status)
{
	Activity::OnActivityStatusChange(from_status, to_status);

	switch (to_status)
	{
	case ACTIVITY_STATUS_STANDY:
		if (CrossConfig::Instance().IsHiddenServer())
		{
			
		}
		break;
	case ACTIVITY_STATUS_OPEN:
		
		break;
	case ACTIVITY_STATUS_CLOSE:
		if (CrossConfig::Instance().IsHiddenServer())
		{
			//最后一次刷新排名
			this->RefreshRank();
			this->RewardRank();
			this->OnActivityEndReward();
			this->ResetRoleActivityData();
			this->ClearAllRoleData();
			m_next_match_time = 0;
			rank_refresh_tamp = 0;
			activity_rank.clear();
			role_win_times.clear();
			player_map.clear();
		}
		break;
	}
}

void ActivityRelicLooting::Update(unsigned long interval, time_t now_second)
{
	Activity::Update(interval, now_second);
	if (ACTIVITY_STATUS_OPEN == m_activity_status)
	{
		m_match_mgr->Update(interval);
	}


	//排行榜
	if (rank_refresh_tamp > 0 && rank_refresh_tamp < now_second)
	{
		rank_refresh_tamp = 0;
		this->RefreshRank();
	}
}

void ActivityRelicLooting::OnUserLogin(Role * role)
{
	if (NULL == role) return;

	Activity::OnUserLogin(role);
}

void ActivityRelicLooting::OnUserLogout(Role * role)
{
	if(NULL == role) return;

	Activity::OnUserLogout(role);
	this->OnCancelReq(role);
}

int ActivityRelicLooting::IsUserPlay(int role_id)
{
	std::map<int, int>::iterator it = player_map.find(role_id);
	if (it != player_map.end())
	{
		return it->second;
	}
	return -1;
}

void ActivityRelicLooting::OnUserPlayEnd(int role_id)
{
	std::map<int, int>::iterator it = player_map.find(role_id);
	if (it != player_map.end())
	{
		player_map.erase(it);//清除玩家标记
	}
}

void ActivityRelicLooting::OnMatchReq(Role * role)
{
	if (ACTIVITY_STATUS_OPEN != m_activity_status)return;
	if (NULL == role) return;
	if (role->InTeam())
	{
		Team *m_team = role->GetMyTeam();
		if (m_team != NULL)
		{
			if (!m_team->IsLeader(role))
			{
				role->NoticeNum(errornum::EN_NOT_LEADER_CANNT_MATH_REQ_ERR);
				return;
			}

			const TeamTypeCfg* team_type_cfg = LOGIC_CONFIG->GetTeamConfig()->GetTeamTypeCfg(m_team->GetTeamType());
			if (NULL != team_type_cfg)
			{
				if (team_type_cfg->team_type_target_type != TEAM_TYPE_TARGET_TYPE_RELIC_LOOTING)
				{
					role->NoticeNum(errornum::EN_RELICLOOTING_TEAM_TYPE_ERR);
					return;
				}
			}
		}
	}
	m_match_mgr->RoleStartMatchReq(role);
}

void ActivityRelicLooting::OnCancelReq(Role * role)
{
	if (NULL == role) return;
	bool is_notice = true;
	if (!this->IsMatch(role->GetUID()))is_notice = false;
	m_match_mgr->RoleStopMatchReq(role);
	if (role->InTeam())
	{
		Team* role_team = role->GetMyTeam();
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_reliclooting_cancle_notice, role->GetUID(), role->GetName());
		if (NULL != role_team)
		{
			if (length > 0)
			{
				static Protocol::SCSystemMsg sm;
				int sendlen = 0;
				if (is_notice && SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
				{
					role_team->SendToMember((const char*)&sm, sendlen);
				}
			}
		}
	}
}

void ActivityRelicLooting::OnMatchEnemy(MatchTeam* mtA, MatchTeam* mtB)
{
	//TODO 拉进一个队伍里，并开始战斗
	Role* attacker_role_list[MAX_TEAM_MEMBER_NUM];
	Role* defender_role_list[MAX_TEAM_MEMBER_NUM];

	int attacker_count = 0;
	int defender_count = 0;

	for (unsigned int i = 0; i < mtA->user_list.size() && attacker_count < MAX_TEAM_MEMBER_NUM; ++i)
	{
		int role_id = mtA->user_list[i]->uid;
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
		if (NULL == role)
		{
			continue; //TODO 做处理， 重新加入队列匹配
		}
		attacker_role_list[attacker_count++] = role;
	}

	for (unsigned int i = 0; i < mtB->user_list.size() && defender_count < MAX_TEAM_MEMBER_NUM; ++i)
	{
		int role_id = mtB->user_list[i]->uid;
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
		if (NULL == role)
		{
			continue; //TODO 做处理， 重新加入队列匹配
		}
		defender_role_list[defender_count++] = role;
	}

	//随机一队作为遗迹守卫
	bool is_defend = (rand() % 2) == 1;
	if (attacker_count <= 0 || defender_count <= 0)
	{
		return;
	}
	Role* defender_role = NULL;
	if (is_defend)
	{
		if (attacker_role_list[0] == NULL)return;
		defender_role = attacker_role_list[0];
	}
	else
	{
		if (defender_role_list[0] == NULL)return;
		defender_role = defender_role_list[0];
	}
	const RelicLootingOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityRelicLootingConfig()->GetOtherCfg();
	int target_scene_id = other_cfg.scene_id;
	int target_scene_key = defender_role->GetUID();
	SpecialLogicRelicLootingFb * special_logic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicRelicLootingFb(target_scene_id, target_scene_key, false);
	if (NULL != special_logic)//先检查这个key是否已经有副本了
	{
		return;
	}
	
	special_logic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicRelicLootingFb(target_scene_id, target_scene_key);
	if (NULL == special_logic)//创建副本
	{
		return ;
	}

	for (int defender_idx = 0; defender_idx < RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM && defender_idx < defender_count; defender_idx++)
	{
		Role * d_role = is_defend ? attacker_role_list[defender_idx] : defender_role_list[defender_idx];
		if (NULL == d_role)continue;
		//记录玩家-对应场景key
		player_map[d_role->GetUID()] = target_scene_key;
		//记录队伍信息，方便结算离场后重新组队
		Team * d_team = d_role->GetMyTeam();
		if (d_team != NULL)
		{
			if (d_team->IsLeader(d_role))
			{
				special_logic->TeamRecord(d_team);//记录队长的队伍信息
				FBManager::Instance().EnterFB(d_role, FBManager::FB_TYPE_RELIC_LOOTING, target_scene_id, target_scene_key, true);
				continue;
			}
		}
		else
		{
			FBManager::Instance().EnterFB(d_role, FBManager::FB_TYPE_RELIC_LOOTING, target_scene_id, target_scene_key, true);
		}
		gamelog::g_log_activity.printf(LL_INFO, "%s user[%d,%s] is_defender[%d] target_scene_key[%d] idx[%d]", __FUNCTION__, d_role->GetUID(), d_role->GetName(), is_defend ? 1 : 0, target_scene_key, defender_idx);
	}

	for (int attacker_idx = 0; attacker_idx < RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM && attacker_idx < attacker_count; attacker_idx++)
	{
		Role * d_role = is_defend ? defender_role_list[attacker_idx] : attacker_role_list[attacker_idx];
		if (NULL == d_role)continue;
		//记录玩家-对应场景key
		player_map[d_role->GetUID()] = target_scene_key;
		//记录队伍信息，方便结算离场后重新组队
		Team * d_team = d_role->GetMyTeam();
		if (d_team != NULL)
		{
			if (d_team->IsLeader(d_role))
			{
				special_logic->TeamRecord(d_team);//记录队长的队伍信息
				FBManager::Instance().EnterFB(d_role, FBManager::FB_TYPE_RELIC_LOOTING, target_scene_id, target_scene_key, false);
				continue;
			}
		}
		else
		{
			FBManager::Instance().EnterFB(d_role, FBManager::FB_TYPE_RELIC_LOOTING, target_scene_id, target_scene_key, false);
		}
		gamelog::g_log_activity.printf(LL_INFO, "%s user[%d,%s] is_defender[%d] target_scene_key[%d] idx[%d]", __FUNCTION__, d_role->GetUID(), d_role->GetName(), is_defend ? 1 : 0, target_scene_key, attacker_idx);
	}

	//所有玩家都应该已经进入副本了，但是客户端的界面还有可能还没加载好预设10秒等待时间，等客户端返回加载完毕
	special_logic->OnMemberAllEnterFb();
	for (int defender_idx = 0; defender_idx < RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM && defender_idx < defender_count; defender_idx++)
	{
		if (NULL != defender_role_list[defender_idx])m_match_mgr->RoleStopMatchReq(defender_role_list[defender_idx]);
	}
	for (int attacker_idx = 0; attacker_idx < RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM && attacker_idx < attacker_count; attacker_idx++)
	{
		if (NULL != attacker_role_list[attacker_idx])m_match_mgr->RoleStopMatchReq(attacker_role_list[attacker_idx]);
	}
}

void ActivityRelicLooting::OnGetRoleReward(Role * role, int seq)
{
	if (seq < 0 || seq >= MAX_RELIC_LOOTING_REWARD_NUM)return;
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		//发往跨服
		crossgameprotocal::GameHiddenRelicLootingReq req;
		req.uid = role->GetUID();
		req.req_type = 3;
		req.param = seq;
		InternalComm::Instance().SendToHiddenThroughCross((const char*)&req, sizeof(req));
		return;
	}
	const RewardBoxCfg * reward_cfg = LOGIC_CONFIG->GetActivityRelicLootingConfig()->GetRewardCfg(seq);
	if (NULL != reward_cfg)
	{
		ActivityRoleData * data_node = this->GetRoleData(role->GetUID(), true);
		if (NULL != data_node)
		{
			ActivityRelicLootingRoleData &param = data_node->relic_looting_role_data;
			int param_data = 0;
			if (reward_cfg->param_type < ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_0 || reward_cfg->param_type >= ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_MAX)return;
			if (param.reward_status[seq] != 0)
			{
				role->NoticeNum(errornum::EN_RELICLOOTING_REWARD_HAS_GET);
				return;
			}
			switch (reward_cfg->param_type)
			{
			case ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_0:
				{
					param_data = param.param[ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_0];
					if (param_data < reward_cfg->param)
					{
						role->NoticeNum(errornum::EN_RELICLOOTING_PARAM_NOT_ADV);
						return;
					}
				}
				break;
			case ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_1:
				{
					param_data = param.param[ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_1];
					if (param_data < reward_cfg->param)
					{
						role->NoticeNum(errornum::EN_RELICLOOTING_PARAM_NOT_ADV);
						return;
					}
				}
				break;
			case ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_2:
				{
					param_data = param.param[ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_2];
					if (param_data < reward_cfg->param)
					{
						role->NoticeNum(errornum::EN_RELICLOOTING_PARAM_NOT_ADV);
						return;
					}
				}
				break;
			default:
				break;
			}
			if ((short)reward_cfg->m_reward_vec.size() > 0)
			{
				if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPutList((short)reward_cfg->m_reward_vec.size(), &reward_cfg->m_reward_vec[0]))
				{
					role->GetRoleModuleManager()->GetKnapsack()->SendMail(&reward_cfg->m_reward_vec[0], (short)reward_cfg->m_reward_vec.size(), SEND_MAIL_TYPE_RELIC_LOOTING);
					return;
				}
				param.reward_status[seq] = 1;
				bool is_put = role->GetRoleModuleManager()->GetKnapsack()->PutList((short)reward_cfg->m_reward_vec.size(), &reward_cfg->m_reward_vec[0], PUT_REASON_RELIC_LOOTING_REWARD);

				std::string log_str = STRING_SPRINTF("ActivityRelicLooting::OnGetRoleReward user[%d,%s] sn[%d] param_type[%d] param_data[%d] ret[%d] reward_list{",
					role->GetUID(), role->GetName(), seq, reward_cfg->param_type, param_data, is_put ? 1 : 0);
				for (int i = 0; i < (int)reward_cfg->m_reward_vec.size(); i++)
				{
					log_str += STRING_SPRINTF("item_%d[item_id:%d num:%d is_bind:%d] ",
						i, reward_cfg->m_reward_vec[i].item_id, reward_cfg->m_reward_vec[i].num, reward_cfg->m_reward_vec[i].is_bind ? 1 : 0);
				}
				log_str += "}";
				gamelog::g_log_relic_looting_reward.printf(LL_INFO, "%s", log_str.c_str());

			}
			this->OnRoleCheckAchievementReward(role);
		}
	}
}

void ActivityRelicLooting::OnRoleRank(Role * role, int jifen, bool is_win)
{
	if (role == NULL)return;
	bool has_user = false;
	for (std::vector<RelicLootRankData>::iterator it = activity_rank.begin(); it != activity_rank.end(); it++)
	{
		if (it->uid == role->GetUID())
		{
			it->rank_value += jifen;
			has_user = true;
			break;
		}
	}

	if (!has_user)
	{
		RelicLootRankData node;
		node.uid = role->GetUID();
		F_STRNCPY(node.role_name, role->GetName(), sizeof(GameName));
		node.rank_value += jifen;
		node.flexible_ll = EngineAdapter::Instance().Time();
		activity_rank.push_back(node);
	}
	
	ActivityRoleData * node_data = this->GetRoleData(role->GetUID());
	if (node_data == NULL)return;
	ActivityRelicLootingRoleData & param = node_data->relic_looting_role_data;
	if (is_win)
	{
		param.param[ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_0] += 1;
		param.param[ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_1] += 1;
		if (param.param[ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_2] < param.param[ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_1])
		{
			param.param[ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_2] += 1;
		}
	}
	else
	{
		param.param[ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_1] = 0;
		param.param[ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_3] += 1;
	}
	this->SetRoleDataChange(role->GetUID());
	this->NextRankPosi();
}

void ActivityRelicLooting::OnUserIdRank(int uid, int jifen, bool is_win)
{
}

void ActivityRelicLooting::OnRoleCheckRankList(Role * role)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		Protocol::SCRelicLootingRankInfo info;
		info.rank_num = 0;
		info.m_pos = 0;//暂无排名
		for (std::vector<RelicLootRankData>::iterator it = activity_rank.begin(); it != activity_rank.end() && info.rank_num < Protocol::SCRelicLootingRankInfo::MAX_RANK_NUM; it++)
		{
			if (it->uid == role->GetUID())
			{
				info.m_pos = (int)std::distance(activity_rank.begin(), it) + 1;
			}
			info.rank_list[info.rank_num++] = (*it);
		}
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&info, sizeof(info) - ((Protocol::SCRelicLootingRankInfo::MAX_RANK_NUM - info.rank_num) * sizeof(RelicLootRankData)));
	}
	else
	{
		//发往跨服请求下发
		crossgameprotocal::GameHiddenRelicLootingReq req;
		req.uid = role->GetUID();
		req.req_type = 1;
		InternalComm::Instance().SendToHiddenThroughCross((const char*)&req, sizeof(req));
	}
}

void ActivityRelicLooting::OnRoleCheckAchievementReward(Role * role)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		Protocol::SCRelicLootingAchievementInfo info;
		ActivityRoleData * node_data = this->GetRoleData(role->GetUID());
		if (node_data != NULL)
		{
			memcpy(info.param, node_data->relic_looting_role_data.param, sizeof(info.param));
			memcpy(info.reward_status, node_data->relic_looting_role_data.reward_status, sizeof(info.reward_status));
			info.show_flag = node_data->relic_looting_role_data.show_flag;
		}
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&info, sizeof(info));
	}
	else
	{
		//发往跨服请求下发
		crossgameprotocal::GameHiddenRelicLootingReq req;
		req.uid = role->GetUID();
		req.req_type = 2;
		InternalComm::Instance().SendToHiddenThroughCross((const char*)&req, sizeof(req));
	}
}

void ActivityRelicLooting::SendRankListToUID(int uid)
{
	Protocol::SCRelicLootingRankInfo info;
	info.rank_num = 0;
	info.m_pos = 0;//暂无排名
	for (std::vector<RelicLootRankData>::iterator it = activity_rank.begin(); it != activity_rank.end() && info.rank_num < Protocol::SCRelicLootingRankInfo::MAX_RANK_NUM; it++)
	{
		if (it->uid == uid)
		{
			info.m_pos = (int)std::distance(activity_rank.begin(), it) + 1;
		}
		info.rank_list[info.rank_num++] = (*it);
	}
	InternalComm::Instance().SendToGameUserThroughCross(uid, (const char*)&info, sizeof(info) - ((Protocol::SCRelicLootingRankInfo::MAX_RANK_NUM - info.rank_num) * sizeof(RelicLootRankData)));
}

void ActivityRelicLooting::SendRewardAchievementToUID(int uid)
{
	Protocol::SCRelicLootingAchievementInfo info;
	ActivityRoleData * node_data = this->GetRoleData(uid);
	if (node_data != NULL)
	{
		memcpy(info.param, node_data->relic_looting_role_data.param, sizeof(info.param));
		memcpy(info.reward_status, node_data->relic_looting_role_data.reward_status, sizeof(info.reward_status));
		info.show_flag = node_data->relic_looting_role_data.show_flag;
	}
	InternalComm::Instance().SendToGameUserThroughCross(uid, (const char*)&info, sizeof(info));
}

void ActivityRelicLooting::GetAchievementReward(int uid, int seq)
{
	const RewardBoxCfg * reward_cfg = LOGIC_CONFIG->GetActivityRelicLootingConfig()->GetRewardCfg(seq);
	if (NULL != reward_cfg)
	{
		ActivityRoleData * data_node = this->GetRoleData(uid);
		if (NULL != data_node)
		{
			ActivityRelicLootingRoleData &param = data_node->relic_looting_role_data;
			int param_data = 0;
			switch (reward_cfg->param_type)
			{
			case ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_0:
				{
					if (param.reward_status[ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_0] != 0)
					{
						return;
					}

					param_data = param.param[ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_0];
					if (param_data < reward_cfg->param)
					{
						return;
					}
				}
				break;
			case ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_1:
				{
					if (param.reward_status[ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_1] != 0)
					{
						return;
					}

					param_data = param.param[ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_1];
					if (param_data < reward_cfg->param)
					{
						return;
					}
				}
				break;
			case ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_2:
				{
					if (param.reward_status[ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_2] != 0)
					{
						return;
					}

					param_data = param.param[ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_2];
					if (param_data < reward_cfg->param)
					{
						return;
					}
				}
				break;
			default:
				break;
			}

			if ((short)reward_cfg->m_reward_vec.size() > 0)
			{
				std::string log_str = STRING_SPRINTF("ActivityRelicLooting::GetAchievementReward user[%d] sn[%d] param_type[%d] param_data[%d] reward_list{",
					uid, seq, reward_cfg->param_type, param_data);
				for (int i = 0; i < (int)reward_cfg->m_reward_vec.size(); i++)
				{
					log_str += STRING_SPRINTF("item_%d[item_id:%d num:%d is_bind:%d] ",
						i, reward_cfg->m_reward_vec[i].item_id, reward_cfg->m_reward_vec[i].num, reward_cfg->m_reward_vec[i].is_bind ? 1 : 0);
				}
				log_str += "}";
				gamelog::g_log_relic_looting_reward.printf(LL_INFO, "%s", log_str.c_str());
				//跨服记录完毕之后返回领取记录给原服
				crossgameprotocal::HiddenGameRelicLootingReq req;
				req.uid = uid;
				req.req_type = 4;
				req.param = seq;
				InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(),IntToUid(uid).db_index,(const char*)&req, sizeof(req));
				this->SendRewardAchievementToUID(uid);
			}
		}
	}
}

void ActivityRelicLooting::SetShow(int uid)
{
	ActivityRoleData * data_node = this->GetRoleData(uid);
	if (NULL != data_node)
	{
		data_node->relic_looting_role_data.show_flag = 1;
	}
	this->SendRewardAchievementToUID(uid);
}

void ActivityRelicLooting::CrossBack(int uid, int seq)
{
	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(uid));
	if (role != NULL)
	{
		const RewardBoxCfg * reward_cfg = LOGIC_CONFIG->GetActivityRelicLootingConfig()->GetRewardCfg(seq);
		if (NULL != reward_cfg)
		{
			ActivityRoleData * data_node = this->GetRoleData(uid);
			if (NULL != data_node)
			{
				ActivityRelicLootingRoleData &param = data_node->relic_looting_role_data;
				int param_data = 0;
				switch (reward_cfg->param_type)
				{
				case ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_0:
					{
						if (param.reward_status[ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_0] != 0)
						{
							return;
						}

						param_data = param.param[ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_0];
						if (param_data < reward_cfg->param)
						{
							return;
						}
					}
					break;
				case ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_1:
					{
						if (param.reward_status[ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_1] != 0)
						{
							return;
						}

						param_data = param.param[ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_1];
						if (param_data < reward_cfg->param)
						{
							return;
						}
					}
					break;
				case ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_2:
					{
						if (param.reward_status[ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_2] != 0)
						{
							return;
						}

						param_data = param.param[ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_2];
						if (param_data < reward_cfg->param)
						{
							return;
						}
					}
					break;
				default:
					break;
				}

				if ((short)reward_cfg->m_reward_vec.size() > 0)
				{
					if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPutList((short)reward_cfg->m_reward_vec.size(), &reward_cfg->m_reward_vec[0]))
					{
						role->GetRoleModuleManager()->GetKnapsack()->SendMail(&reward_cfg->m_reward_vec[0], (short)reward_cfg->m_reward_vec.size(), SEND_MAIL_TYPE_RELIC_LOOTING);
						return;
					}

					bool is_put = role->GetRoleModuleManager()->GetKnapsack()->PutList((short)reward_cfg->m_reward_vec.size(), &reward_cfg->m_reward_vec[0], PUT_REASON_RELIC_LOOTING_REWARD);
					std::string log_str = STRING_SPRINTF("ActivityRelicLooting::CrossBack user[%d,%s] sn[%d] param_type[%d] param_data[%d] ret[%d] reward_list{",
						role->GetUID(), role->GetName(), seq, reward_cfg->param_type, param_data, is_put ? 1 : 0);
					for (int i = 0; i < (int)reward_cfg->m_reward_vec.size(); i++)
					{
						log_str += STRING_SPRINTF("item_%d[item_id:%d num:%d is_bind:%d] ",
							i, reward_cfg->m_reward_vec[i].item_id, reward_cfg->m_reward_vec[i].num, reward_cfg->m_reward_vec[i].is_bind ? 1 : 0);
					}
					log_str += "}";
					gamelog::g_log_relic_looting_reward.printf(LL_INFO, "%s", log_str.c_str());
				}
			}
		}
	}
	else
	{
		gamelog::g_log_relic_looting_reward.printf(LL_INFO, "ActivityRelicLooting::CrossBack fail user[%d] seq[%d]", uid, seq);
	}
}

bool ActivityRelicLooting::IsMatch(int uid)
{
	if (m_match_mgr->GetMatchEngine()->IsUserMatching(uid))
	{
		return true;
	}
	return false;
}

void ActivityRelicLooting::NextRankPosi()
{
	if (rank_refresh_tamp <= 0 || EngineAdapter::Instance().Time() >= rank_refresh_tamp)
	{
		rank_refresh_tamp = EngineAdapter::Instance().Time() + 2;
	}
}

void ActivityRelicLooting::RefreshRank()
{
	std::sort(activity_rank.begin(), activity_rank.end(), &SortFunc);
}

void ActivityRelicLooting::RewardRank()
{
	//发放排名奖励
	int rank_pos = 0;
	for (std::vector<RelicLootRankData>::iterator it = activity_rank.begin(); it != activity_rank.end(); it++)
	{
		rank_pos = std::distance(activity_rank.begin(), it);
		const RankRewardCfg * reward_cfg = LOGIC_CONFIG->GetActivityRelicLootingConfig()->GetRankRewardCfg(rank_pos + 1);
		if (reward_cfg != NULL)
		{
			if (reward_cfg->m_rank_reward_vec.size() > 0)
			{
				static MailContentParam contentparam; contentparam.Reset();
				std::string log_str = STRING_SPRINTF("ActivityRelicLooting::RewardRank user[%d] rank_pos[%d] rank_value[%d] reward_list{", it->uid, rank_pos, it->rank_value);
				for (int i = 0; i < (int)reward_cfg->m_rank_reward_vec.size() && i < MAX_ATTACHMENT_ITEM_NUM; i++)
				{
					contentparam.item_list[i].item_id = reward_cfg->m_rank_reward_vec[i].item_id;
					contentparam.item_list[i].num = reward_cfg->m_rank_reward_vec[i].num;
					contentparam.item_list[i].is_bind = (reward_cfg->m_rank_reward_vec[i].is_bind) ? 1 : 0;
					log_str += STRING_SPRINTF("item_%d[item_id:%d num:%d is_bind:%d] ",
						i, contentparam.item_list[i].item_id, contentparam.item_list[i].num, contentparam.item_list[i].is_bind);
				}
				log_str += "}";
				int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_relic_looting_rank_reward_subject);
				int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_relic_looting_rank_reward_content, rank_pos + 1);
				if (lensub > 0 && length > 0)
				{
					MailRoute::MailToUser(it->uid, MAIL_REASON_DEFAULT, contentparam);
					gamelog::g_log_relic_looting_reward.printf(LL_INFO, "%s", log_str.c_str());
				}
			}
		}
	}
}

void ActivityRelicLooting::ResetRoleActivityData()
{
	RetActivityRoleData get_role_data;
	this->BeginRoleDataForeach();
	while (this->GetNextRoleData(get_role_data))
	{
		int role_id = get_role_data.role_id;
		ActivityRelicLootingRoleData & role_data = get_role_data.data.relic_looting_role_data;
		std::string log_str = STRING_SPRINTF("ActivityRelicLooting::ResetRoleActivityData user[%d] param_list{", role_id);
		for (int i = 0; i < ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_MAX; i++)
		{
			log_str += STRING_SPRINTF("param_data_%d[param:%d reward_status:%d] ", i, role_data.param[i], role_data.reward_status[i]);
		}
		log_str += "}";
		role_data.Reset();
		gamelog::g_log_relic_looting_reward.printf(LL_INFO, "%s data_clear_succ, log[%s]", __FUNCTION__, log_str.c_str());
	}
}

void ActivityRelicLooting::OnActivityEndReward()
{
	RetActivityRoleData get_role_data;
	this->BeginRoleDataForeach();
	while (this->GetNextRoleData(get_role_data))
	{
		ActivityRoleData * data_node = &get_role_data.data;
		ActivityRelicLootingRoleData &param = data_node->relic_looting_role_data;
		for (int seq = 0; seq < MAX_RELIC_LOOTING_REWARD_NUM; seq++)
		{
			const RewardBoxCfg * reward_cfg = LOGIC_CONFIG->GetActivityRelicLootingConfig()->GetRewardCfg(seq);
			if (NULL != reward_cfg)
			{
				int param_data = 0;
				if (reward_cfg->param_type < ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_0 || reward_cfg->param_type >= ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_MAX)return;
				if (param.reward_status[seq] != 0)continue;
				static MailContentParam contentparam;
				contentparam.Reset();
				int length1 = 0;
				int length2 = 0;
				switch (seq)
				{
				case 1:
					{
						length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_relic_looting_rank_subject_compensate_1);
						length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_relic_looting_rank_content_compensate_1);
					}
					break;
				case 2:
					{
						length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_relic_looting_rank_subject_compensate_2);
						length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_relic_looting_rank_content_compensate_2);
					}
					break;
				case 3:
					{
						length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_relic_looting_rank_subject_compensate_3);
						length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_relic_looting_rank_content_compensate_3);
					}
					break;
				default:
					break;
				}
				switch (reward_cfg->param_type)
				{
				case ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_0:
					{
						param_data = param.param[ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_0];
						if (param_data < reward_cfg->param)continue;
						
					}
					break;
				case ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_1:
					{
						param_data = param.param[ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_1];
						if (param_data < reward_cfg->param)continue;
					}
					break;
				case ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_2:
					{
						param_data = param.param[ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_2];
						if (param_data < reward_cfg->param)continue;
					}
					break;
				default:
					break;
				}
				if ((short)reward_cfg->m_reward_vec.size() > 0)
				{
					param.reward_status[seq] = 1;
					FillMailContentParam(contentparam, reward_cfg->m_reward_vec);
					if(length1 > 0 && length2 > 0)MailRoute::Instance().MailToUser(IntToUid(get_role_data.role_id), MAIL_REASON_DEFAULT, contentparam);

					std::string log_str = STRING_SPRINTF("ActivityRelicLooting::OnGetRoleReward user[%d] sn[%d] param_type[%d] param_data[%d] reward_list{",
						get_role_data.role_id, seq, reward_cfg->param_type, param_data);
					for (int i = 0; i < (int)reward_cfg->m_reward_vec.size(); i++)
					{
						log_str += STRING_SPRINTF("item_%d[item_id:%d num:%d is_bind:%d] ",
							i, reward_cfg->m_reward_vec[i].item_id, reward_cfg->m_reward_vec[i].num, reward_cfg->m_reward_vec[i].is_bind ? 1 : 0);
					}
					log_str += "}";
					gamelog::g_log_relic_looting_reward.printf(LL_INFO, "%s", log_str.c_str());

				}
			}
		}
	}
}
