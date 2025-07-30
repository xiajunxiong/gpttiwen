#include "worldteamarenasignupmanager.hpp"
#include "gamelog.h"
#include "internalcomm.h"

#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "servercommon/uniqueidgenerator.hpp"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "global/rmibackobjdef.h"
#include "global/team/team.hpp"
#include "global/guild/guild.hpp"
#include "global/team/teammanager.hpp"
#include "global/guild/guildmanager.hpp"
#include "global/worldstatus/worldstatus.hpp"
#include "global/activity/activitymanager.hpp"
#include "global/activity/activityimpl/activityworldteamarena.hpp"

#include "item/money.h"
#include "obj/character/role.h"
#include "friend/friendmanager.h"
#include "config/logicconfigmanager.hpp"
#include "config/activityconfig/worldteamarena/activityworldteamarenaconfig.hpp"

#include "other/rolemodulemanager.hpp"
#include "other/flyupsystem/flyupsystem.hpp"


WorldTeamArenaSignUpManager & WorldTeamArenaSignUpManager::Instance()
{
	static WorldTeamArenaSignUpManager world_team_arena_manager;
	return world_team_arena_manager;
}

void WorldTeamArenaSignUpManager::Update(unsigned int now_second)
{
	//数据只存跨服
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}
	if (now_second >= m_next_save_db_timestamp && this->IsDirty())
	{
		this->Save();
		this->UpdateRankList();
		m_next_save_db_timestamp = now_second + ACTIVITY_WORLD_TEAM_ARENA_SAVE_DB_TIME;
	}
}

void WorldTeamArenaSignUpManager::OnServerStart()
{
	this->LoadWorldTeamArenaAllTeamData(0);
}

void WorldTeamArenaSignUpManager::OnServerStop()
{
	//数据只存跨服
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}
	this->Save();
}

bool WorldTeamArenaSignUpManager::LoadWorldTeamArenaAllTeamData(long long id_from)
{
	RMIInitWorldTeamArenaSignUpDataBackObjectImpl * impl = new RMIInitWorldTeamArenaSignUpDataBackObjectImpl();

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	return ac.InitWorldTeamArenaSignUpDataAsyn(id_from, impl);
}

void RMIInitWorldTeamArenaSignUpDataBackObjectImpl::InitWorldTeamArenaSignUpDataRet(int ret, const WorldTeamArenaSignUpParam & list_param)
{
	if (ret < 0)
	{
		printf("%s Failed \n", __FUNCTION__);
	}
	else
	{
		WorldTeamArenaSignUpManager & world_team_arena_sign_up_manager = WorldTeamArenaSignUpManager::Instance();

		world_team_arena_sign_up_manager.Init(list_param);
		world_team_arena_sign_up_manager.LoadWorldTeamArenaAllTeamSucc();
		printf("%s:%d\n", __FUNCTION__, ret);
	}
}

void WorldTeamArenaSignUpManager::OnUserLogin(Role * role)
{
	this->SendOtherInfo(role);
}

void WorldTeamArenaSignUpManager::OnUserLogout(Role * user)
{
	if (!CrossConfig::Instance().IsHiddenServer() || NULL == user || m_pre_team_sign_up_list.empty())
	{
		return;
	}

	for (int i = 0; i < (int)m_pre_team_sign_up_list.size(); i++)
	{
		for (int k = 0; k < ARRAY_LENGTH(m_pre_team_sign_up_list[i].member_list); k++)
		{
			PreTeamSignUpInfo::MemberInfo & member_info = m_pre_team_sign_up_list[i].member_list[k];
			if (user->GetUID() == member_info.role_id)
			{
				Team * team = user->GetMyTeam();
				if (NULL != team)
				{
					this->SendNoticeInfo(team, Protocol::SCWorldTeamArenaNoticeInfo::NOTICE_TYPE_SIGN_UP_FAILED_CLOSE, 0, 0);
					int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_world_team_arena_team_member_logout,
						user->GetName());
					if (length > 0)
					{
						World::GetInstWorld()->GetSceneManager()->SystemMsgTeamMember(team, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
					}
				}
				m_pre_team_sign_up_list.erase(m_pre_team_sign_up_list.begin() + i);
				return;
			}
		}
	}
}

void WorldTeamArenaSignUpManager::OnUserLoginOriginGame(int role_id)
{
	this->SendOtherInfoToGame(role_id);
}

void WorldTeamArenaSignUpManager::OnLeaveTeam(Role * user, int team_index)
{
	if (!CrossConfig::Instance().IsHiddenServer() || NULL == user || m_pre_team_sign_up_list.empty())
	{
		return;
	}

	for (int i = 0; i < (int)m_pre_team_sign_up_list.size(); i++)
	{
		for (int k = 0; k < ARRAY_LENGTH(m_pre_team_sign_up_list[i].member_list); k++)
		{
			PreTeamSignUpInfo::MemberInfo & member_info = m_pre_team_sign_up_list[i].member_list[k];
			if (user->GetUID() == member_info.role_id)
			{
				Team * team = TeamManager::Instance().GetTeamByTeamIndex(team_index);
				if (NULL != team)
				{
					this->SendNoticeInfo(team, Protocol::SCWorldTeamArenaNoticeInfo::NOTICE_TYPE_SIGN_UP_FAILED_CLOSE, 0, 0);
					int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_world_team_arena_team_member_leave_team,
						user->GetName());
					if (length > 0)
					{
						World::GetInstWorld()->GetSceneManager()->SystemMsgTeamMember(team, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
					}
				}
				m_pre_team_sign_up_list.erase(m_pre_team_sign_up_list.begin() + i);
				return;
			}
		}
	}
}

void WorldTeamArenaSignUpManager::Init(const WorldTeamArenaSignUpParam & param)
{
	for (int i = 0; i < param.count && i < ARRAY_LENGTH(param.team_sign_up_list); ++i)
	{
		const WorldTeamArenaTeamSignUpData & sign_up_data = param.team_sign_up_list[i];
		if (!sign_up_data.IsVaild())
		{
			gamelog::g_log_world_team_arena.printf(LL_CRITICAL, "[%s] team[%lld, %s]", __FUNCTION__, sign_up_data.team_unique_id, 
				sign_up_data.team_name);
			continue;
		}
		m_team_sign_up_map[sign_up_data.team_unique_id] = sign_up_data;
		m_dirty_mark[sign_up_data.team_unique_id] = false;
		m_old_state[sign_up_data.team_unique_id] = true;
		this->OnInsterSimple(sign_up_data);
	}
	m_load_flag = true;
}

void WorldTeamArenaSignUpManager::GetInitParam(WorldTeamArenaSignUpParam * param)
{
	if (!m_load_flag || !this->IsDirty())
	{
		return;
	}

	param->count = 0;
	for (std::map<long long, WorldTeamArenaTeamSignUpData>::iterator it = m_team_sign_up_map.begin(); it != m_team_sign_up_map.end() && 
		param->count < ARRAY_LENGTH(param->team_sign_up_list); it++)
	{
		if (m_dirty_mark[it->first])
		{
			if (!it->second.IsVaild())
			{
				if(m_old_state[it->first]) continue;

				param->team_sign_up_list[param->count].change_state = structcommon::CS_DELETE;
				param->team_sign_up_list[param->count].team_unique_id = it->first;
				param->count++;
			}
			else
			{
				param->team_sign_up_list[param->count] = it->second;
				param->team_sign_up_list[param->count].change_state = m_old_state[it->first] ? structcommon::CS_UPDATE : structcommon::CS_INSERT;
				param->count++;
			}
		}
	}
}

void WorldTeamArenaSignUpManager::ClearDirtyMark(WorldTeamArenaSignUpParam * param)
{
	if(!m_change_flag) return;

	for (int i = 0; i < param->count && i < ARRAY_LENGTH(param->team_sign_up_list); i++)
	{
		WorldTeamArenaTeamSignUpData & data = param->team_sign_up_list[i];
		if (structcommon::CS_DELETE != data.change_state)
		{
			m_old_state[data.team_unique_id] = true;
			m_dirty_mark[data.team_unique_id] = false;
		}
		else
		{
			m_dirty_mark.erase(data.team_unique_id);
			m_old_state.erase(data.team_unique_id);
			m_team_sign_up_map.erase(data.team_unique_id);
			this->OnEraseSimple(data.team_unique_id);
		}
		data.change_state = structcommon::CS_NONE;
	}

	m_change_flag = false;
}

long long WorldTeamArenaSignUpManager::GetTeamUniqueId(int role_id)
{
	std::map<int, long long>::const_iterator it = m_role_team_map.find(role_id);
	if (it == m_role_team_map.end())
	{
		return 0;
	}

	return it->second;
}

const WorldTeamArenaTeamSignUpData * WorldTeamArenaSignUpManager::GetWorldTeamArenaSignUpDataById(int role_id)
{
	std::map<int, long long>::const_iterator it = m_role_team_map.find(role_id);
	if (it == m_role_team_map.end())
	{
		return NULL;
	}
	return this->GetWorldTeamArenaSignUpDataById(it->second);
}

const WorldTeamArenaTeamSignUpData * WorldTeamArenaSignUpManager::GetWorldTeamArenaSignUpDataById(long long team_unique_id)
{
	std::map<long long, WorldTeamArenaTeamSignUpData>::const_iterator it = m_team_sign_up_map.find(team_unique_id);
	if (it == m_team_sign_up_map.end() || !it->second.IsVaild())
	{
		return NULL;
	}

	return &it->second;
}

bool WorldTeamArenaSignUpManager::IsSameTeamByTeam(Team * team, bool is_notice)
{
	if(NULL == team || !CrossConfig::Instance().IsHiddenServer()) return false;

	bool is_first_member = true;
	long long last_team_unique_id = 0;
	for (int i = 0; i < team->GetMemberCount(); ++i)
	{
		Role* member_role = team->GetMemberRoleByIndex(i);
		if (NULL == member_role)
		{
			continue;
		}
		
		std::map<int, long long>::const_iterator it = m_role_team_map.find(member_role->GetUID());
		if (it == m_role_team_map.end() || (!is_first_member && last_team_unique_id != it->second))
		{
			if (is_notice)
			{
				team->SendToMemberNotice(errornum::EN_WORLD_TEAM_ARENA_FAILED_TEAM_NOT_SAME_TEAM);
			}
			return false;
		}
		is_first_member = false;
		last_team_unique_id = it->second;
	}

	return true;
}

void WorldTeamArenaSignUpManager::OnTeamSignUp(Role * role, GameName team_name)
{
	if(NULL == role) return;

	Team * m_team = role->GetMyTeam();
	if (NULL == m_team)
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
		return;
	}

	if (!m_team->IsLeader(role))
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
		return;
	}

	if (m_team->GetFollowMemberCount() < MAX_TEAM_MEMBER_NUM)
	{
		role->NoticeNum(errornum::EN_WORLD_TEAM_ARENA_SIGN_UP_FAILED_BY_MEMBER);
		return;
	}
	const ActivityWorldTeamArenaOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityWorldTeamArenaConfig()->GetOtherCfg();
	if (WorldStatus::Instance().GetWorldLevel() < other_cfg.need_level)
	{
		return;
	}
	PreTeamSignUpInfo team_info;
	if (!this->OnCheckCanPreSignUp(role, m_team, team_name, team_info))
	{
		return;
	}

	ActivityWorldTeamArena * activity = (ActivityWorldTeamArena* )ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_TEAM_ARENA);
	if (NULL == activity || activity->IsActivityOpen() ||!activity->IsCanSignUp())
	{
		role->NoticeNum(errornum::EN_WORLD_TEAM_ARENA_SIGN_UP_FAILED_BY_TIME);
		return;
	}

	m_pre_team_sign_up_list.push_back(team_info);
	role->NoticeNum(noticenum::NT_WORLD_TEAM_ARENA_SIGN_UP_SUCC);

	Protocol::SCWorldTeamArenaPreTeamSignUpInfo info;
	info.pre_team_sign_up_info = team_info;

	m_team->SendToMember(&info, sizeof(info), true);
}

void WorldTeamArenaSignUpManager::OnTeamMemberSignUp(Role * role)
{
	if(NULL == role || m_pre_team_sign_up_list.empty()) return;

	for (int i = 0; i < (int)m_pre_team_sign_up_list.size(); i++)
	{
		for (int k = 0; k < ARRAY_LENGTH(m_pre_team_sign_up_list[i].member_list); k++)
		{
			PreTeamSignUpInfo::MemberInfo & member_info = m_pre_team_sign_up_list[i].member_list[k];
			if (role->GetUID() == member_info.role_id)
			{
				Team * team = role->GetMyTeam();
				if (NULL == team)
				{
					m_pre_team_sign_up_list.erase(m_pre_team_sign_up_list.begin() + i);
				}
				else
				{
					member_info.determine_flag = MEMBER_DETERMINE_TYPE_YES;
					this->SendNoticeInfo(team, Protocol::SCWorldTeamArenaNoticeInfo::NOTICE_TYPE_MEMBER_SIGN_UP, k, member_info.determine_flag);
					if (this->OnCheckPreAllDet(m_pre_team_sign_up_list[i]))
					{
						m_pre_team_sign_up_list.erase(m_pre_team_sign_up_list.begin() + i);
					}
				}
				return;
			}
		}
	}
}

void WorldTeamArenaSignUpManager::OnCancelTeamSignUp(Role * role)
{
	if (NULL == role || m_pre_team_sign_up_list.empty()) return;

	for (int i = 0; i < (int)m_pre_team_sign_up_list.size(); i++)
	{
		for (int k = 0; k < ARRAY_LENGTH(m_pre_team_sign_up_list[i].member_list); k++)
		{
			PreTeamSignUpInfo::MemberInfo & member_info = m_pre_team_sign_up_list[i].member_list[k];
			if (role->GetUID() == member_info.role_id)
			{
				Team * team = role->GetMyTeam();
				if (NULL != team)
				{
					this->SendNoticeInfo(team, Protocol::SCWorldTeamArenaNoticeInfo::NOTICE_TYPE_SIGN_UP_FAILED_CLOSE, 0, 0);
					int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_world_team_arena_team_member_cancel_sign_up,
						role->GetName());
					if (length > 0)
					{
						World::GetInstWorld()->GetSceneManager()->SystemMsgTeamMember(team, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
					}
				}
				m_pre_team_sign_up_list.erase(m_pre_team_sign_up_list.begin() + i);
				return;
			}
		}
	}
}

void WorldTeamArenaSignUpManager::OnRoleInfoChange(Role * role)
{
	if(NULL == role || !CrossConfig::Instance().IsHiddenServer()) return;

	for (std::map<long long, WorldTeamArenaTeamSignUpData>::iterator it = m_team_sign_up_map.begin(); it != m_team_sign_up_map.end(); ++it)
	{
		if (!it->second.IsVaild()) continue;

		for (int i = 0; i < ARRAY_LENGTH(it->second.member_list); ++i)
		{
			if (role->GetUID() == it->second.member_list[i].role_uid)
			{
				this->FillMemberInfo(role, it->second.member_list[i]);
				m_dirty_mark[it->first] = true;
				m_old_state[it->first] = m_old_state[it->first] ? true : false;
				m_change_flag = true;
				return;
			}
		}
	}
}

void WorldTeamArenaSignUpManager::OnQueryTeamDetailedInfo(Role * role, long long team_unique_id, int type)
{
	if(NULL == role) return;

	std::map<long long, WorldTeamArenaTeamSignUpData>::const_iterator it = m_team_sign_up_map.find(team_unique_id);
	if (it == m_team_sign_up_map.end() || !it->second.IsVaild())
	{
		return;
	}

	int notice_type = (0 == type) ? Protocol::SCWorldTeamArenaTeamSignUpDetailedInfo::NOTICE_TYPE_SIGN_UP : Protocol::SCWorldTeamArenaTeamSignUpDetailedInfo::NOTICE_TYPE_GAME;
	this->QueryTeamDetailedInfo(role, it->second, 0, notice_type);
}

void WorldTeamArenaSignUpManager::OnQueryTeamDetailedInfo(Role * role, GameName name)
{
	if(NULL == role) return;

	for (std::map<long long, WorldTeamArenaTeamSignUpData>::const_iterator it = m_team_sign_up_map.begin(); it != m_team_sign_up_map.end(); ++it)
	{
		if (!it->second.IsVaild()) continue;

		if (!strcmp(it->second.team_name, name))
		{
			this->QueryTeamDetailedInfo(role, it->second);
			return;
		}
		for (int i = 0; i < ARRAY_LENGTH(it->second.member_list); ++i)
		{
			if (!strcmp(it->second.member_list[i].role_name, name))
			{
				this->QueryTeamDetailedInfo(role, it->second);
				return;
			}
		}
	}
}

void WorldTeamArenaSignUpManager::GetQueryTeamUidList(long long team_unique_id, std::set<int>& team_user_id_list)
{
	std::map<long long, WorldTeamArenaTeamSignUpData>::const_iterator it = m_team_sign_up_map.find(team_unique_id);
	if (it == m_team_sign_up_map.end() || !it->second.IsVaild())
	{
		return;
	}
	for (int i = 0; i < ARRAY_LENGTH(it->second.member_list); ++i)
	{
		team_user_id_list.insert(it->second.member_list[i].role_uid);
	}
}

void WorldTeamArenaSignUpManager::GetQueryTeamLevelList(long long team_unique_id, std::set<std::pair<int, int> >& team_user_Level_list)
{
	std::map<long long, WorldTeamArenaTeamSignUpData>::const_iterator it = m_team_sign_up_map.find(team_unique_id);
	if (it == m_team_sign_up_map.end() || !it->second.IsVaild())
	{
		return;
	}
	for (int i = 0; i < ARRAY_LENGTH(it->second.member_list); ++i)
	{
		team_user_Level_list.insert(std::pair<int, int>(it->second.member_list[i].role_uid, it->second.member_list[i].level));
	}
}

void WorldTeamArenaSignUpManager::GetTeamNameById(long long team_unique_id, GameName & team_name)
{
	std::map<long long, WorldTeamArenaTeamSignUpData>::const_iterator it = m_team_sign_up_map.find(team_unique_id);	
	if (it != m_team_sign_up_map.end() || !it->second.IsVaild())
	{
		memcpy(team_name, it->second.team_name, sizeof(team_name));
	}
}

void WorldTeamArenaSignUpManager::OnGameRecvCrossQueryRankRet(crossgameprotocal::CrossGameWorldTeamArenaQueryRankRet * msg)
{
	if(NULL == msg) return;

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(msg->send_uid);
	if(NULL == role) return;

	std::map<long long, WorldTeamArenaTeamSignUpData>::const_iterator it = m_team_sign_up_map.find(msg->team_unique_id);
	if (it == m_team_sign_up_map.end() || !it->second.IsVaild())
	{
		return;
	}

	Protocol::SCWorldTeamArenaTeamSignUpDetailedInfo info;
	info.notice_type = msg->notice_type;
	info.team_detailed_info.team_unique_id = it->second.team_unique_id;
	memcpy(info.team_detailed_info.team_name, it->second.team_name, sizeof(info.team_detailed_info.team_name));

	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(it->second.member_list) == ARRAY_ITEM_COUNT(info.team_detailed_info.member_list));
	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(msg->rank_user_info) == ARRAY_ITEM_COUNT(it->second.member_list));
	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(msg->rank_user_info[0].rank_type_list) == ARRAY_ITEM_COUNT(info.team_detailed_info.member_list[0].rank_list));
	for (int i = 0; i < ARRAY_LENGTH(it->second.member_list); i++)
	{
		info.team_detailed_info.member_list[i].SetData(it->second.member_list[i]);
		memcpy(info.team_detailed_info.member_list[i].rank_list, msg->rank_user_info[i].rank_type_list, sizeof(info.team_detailed_info.member_list[i].rank_list));
	}
	
	EngineAdapter::Instance().NetSend(role->GetNetId(), &info, sizeof(info));
}

void WorldTeamArenaSignUpManager::OnGameRecvCrossQueryRankRetSendSpecialInfo(crossgameprotocal::CrossGameWorldTeamArenaQueryRankRet * msg)
{
	if (NULL == msg) return;

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(msg->send_uid);
	if (NULL == role) return;

	Protocol::SCActivityWorldTeamArenaTeamSpecialInfo info;
	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(msg->rank_user_info[0].rank_type_list) == ARRAY_ITEM_COUNT(info.member_list[0].rank_list));
	for (int i = 0; i < ARRAY_ITEM_COUNT(msg->rank_user_info); i++)
	{
		info.member_list[i].role_uid = msg->rank_user_info[i].role_id;
		memcpy(info.member_list[i].rank_list, msg->rank_user_info[i].rank_type_list, sizeof(info.member_list[i].rank_list));
	}

	EngineAdapter::Instance().NetSend(role->GetNetId(), &info, sizeof(info));
}

void WorldTeamArenaSignUpManager::OnHiddenRecvGameSynRoleInfo(crossgameprotocal::GameHiddenRoleInfoChangeWorldTeamArenaSyn * msg)
{
	if(NULL == msg || !CrossConfig::Instance().IsHiddenServer() || msg->member_info.role_uid <= 0) return;

	for (std::map<long long, WorldTeamArenaTeamSignUpData>::iterator it = m_team_sign_up_map.begin(); it != m_team_sign_up_map.end(); ++it)
	{
		if (!it->second.IsVaild()) continue;

		for (int i = 0; i < ARRAY_LENGTH(it->second.member_list); ++i)
		{
			if (msg->member_info.role_uid == it->second.member_list[i].role_uid)
			{
				it->second.member_list[i] = msg->member_info;
				m_dirty_mark[it->first] = true;
				m_old_state[it->first] = m_old_state[it->first] ? true : false;
				m_change_flag = true;
				return;
			}
		}
	}
}

void WorldTeamArenaSignUpManager::SendAllSignUpInfo(Role * role, long long team_unqiue_id)
{
	if(NULL == role) return;

	int role_uid = role->GetUID();
	bool is_has_guild = (NULL != role->GetGuild()) ? true : false;
	
	Protocol::SCWorldTeamArenaAllSimpleInfo info;
	info.count = 0;
	info.know_count = 0;
	info.my_team_index = -1;
	for (int i = 0; i < (int)m_rank_list.size() && info.count < ARRAY_LENGTH(info.team_list) && info.know_count < ARRAY_LENGTH(info.know_team_index); i++)
	{
		const WorldTeamArenaTeamSignUpSimpleInfo & simple_info = m_rank_list[i];
		bool is_know_team = true;
		bool is_friend = false;
		for (int k = 0; k < ARRAY_LENGTH(simple_info.member_list); k++)
		{
			if (role_uid == simple_info.member_list[k].role_id)
			{
				info.my_team_index = info.count;
				is_know_team = false;
				break;
			}
			//不是自己战队,并且战队中有自己好友或者战队中的角色都是跟自己同个家族,则为熟识战队
			if (FriendManager::Instance().IsFriend(role_uid, simple_info.member_list[k].role_id))
			{
				//该处不break,让循环跑满,否则可能会出现自己就在该战队中
				is_know_team = true;
				is_friend = true;
			}
			if (!is_friend)
			{
				if (is_has_guild)
				{
					Guild* member_guild = GuildManager::Instance().GetGuild(IntToUid(simple_info.member_list[k].role_id));
					if (NULL == member_guild || member_guild->GetGuildID() != role->GetGuildID())
					{
						is_know_team = false;
					}
				}
				else
				{
					is_know_team = false;
				}
			}
		}
		if (is_know_team)
		{
			info.know_team_index[info.know_count++] = info.count;
		}

		info.team_list[info.count++] = simple_info;
	}

	EngineAdapter::Instance().NetSendSerializeMsg(role->GetNetId(), &info);

	//查询详细信息
	if (0 == info.count)
	{
		return;
	}
	//不指定查询则默认先选中自身战队,如果自身没有战队则选中第一个
	if (0 == team_unqiue_id)
	{
		if (-1 == info.my_team_index)
		{
			team_unqiue_id = info.team_list[0].team_unique_id;
		}
		else
		{
			team_unqiue_id = info.team_list[info.my_team_index].team_unique_id;
		}
	}

	std::map<long long, WorldTeamArenaTeamSignUpData>::const_iterator it = m_team_sign_up_map.find(team_unqiue_id);
	if (it == m_team_sign_up_map.end() || !it->second.IsVaild())
	{
		return;
	}
	this->QueryTeamDetailedInfo(role, it->second);
}

void WorldTeamArenaSignUpManager::SendNoticeInfo(Team * team, int notice_type, int param1, int param2)
{
	if(NULL == team || notice_type < 0 ||notice_type >= Protocol::SCWorldTeamArenaNoticeInfo::NOTICE_TYPE_MAX) return;

	Protocol::SCWorldTeamArenaNoticeInfo info;
	info.notice_type = notice_type;
	info.param1 = param1;
	info.param2 = param2;

	team->SendToMember(&info, sizeof(info));
}

void WorldTeamArenaSignUpManager::OnSendTeamSpecialInfo(Role * role)
{
	if(NULL == role) return;

	Team * team = role->GetMyTeam();
	if (NULL == team)
	{
		return;
	}

	crossgameprotocal::GameCrossWorldTeamArenaQueryRankReq gcwtaqrr;
	gcwtaqrr.server_id = LocalConfig::Instance().GetMergeServerId();
	gcwtaqrr.plat_type = LocalConfig::Instance().GetPlatType();
	gcwtaqrr.team_unqiue_id = 0;
	gcwtaqrr.send_uid = role->GetUID();
	gcwtaqrr.notice_type = 0;
	gcwtaqrr.send_type = 1;
	int index = 0;
	for (int i = 0; i < team->GetMemberCount() && i < MAX_TEAM_MEMBER_NUM && index < ACTIVITY_WORLD_TEAM_ARENA_TEAM_MEMBER_NUM; ++i)
	{
		Role * member = team->GetMemberRoleByIndex(i);
		if(NULL == member)continue;

		gcwtaqrr.rank_user_info[index].role_id = member->GetUID();
		gcwtaqrr.rank_user_info[index].prof = member->GetProfession();
		memcpy(gcwtaqrr.rank_user_info[i].rank_type_list, ACTIVITY_WORLD_TEAM_ARENA_RANK_TYPE_LIST, sizeof(gcwtaqrr.rank_user_info[i].rank_type_list));
		index++;
	}

	InternalComm::Instance().SendToCross((const char *)&gcwtaqrr, sizeof(gcwtaqrr));
}

void WorldTeamArenaSignUpManager::SendOtherInfo(Role * role)
{
	if (NULL == role) return;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		Protocol::SCActivityWorldTeamArenaOtherInfo other_info;
		other_info.is_sign_up = (this->GetTeamUniqueId(role->GetUID()) > 0) ? 1 : 0;

		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&other_info, sizeof(other_info));
	}
	else
	{
		crossgameprotocal::GameHiddenQueryWorldTeamArenaOtherInfo ghqwtaoi;
		ghqwtaoi.role_id = role->GetUID();

		InternalComm::Instance().SendToHiddenThroughCross((const char *)&ghqwtaoi, sizeof(ghqwtaoi));
	}
}

void WorldTeamArenaSignUpManager::SendOtherInfoToGame(int role_id)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	Protocol::SCActivityWorldTeamArenaOtherInfo other_info;
	other_info.is_sign_up = (WorldTeamArenaSignUpManager::Instance().GetTeamUniqueId(role_id) > 0) ? 1 : 0;

	InternalComm::Instance().SendToGameUserThroughCross(role_id, (const char *)&other_info, sizeof(other_info));
}

void WorldTeamArenaSignUpManager::OnGmAllTeamSignUp(Role * role)
{
	if(NULL == role) return;

	Team * team = role->GetMyTeam();
	if(NULL == team) return;

	for (int i = 0; i < team->GetMemberCount(); i++)
	{
		Role *member = team->GetMemberRoleByIndex(i);
		if(NULL == member) continue;;

		this->OnTeamMemberSignUp(member);
	}
}

void WorldTeamArenaSignUpManager::OnActivityEnd()
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}
	m_pre_team_sign_up_list.clear();
	for (std::map<long long, WorldTeamArenaTeamSignUpData>::iterator it = m_team_sign_up_map.begin(); it != m_team_sign_up_map.end(); ++it)
	{
		it->second.Reset();
		m_dirty_mark[it->first] = true;
		m_old_state[it->first] = false;
		m_change_flag = true;
	}
	Protocol::SCActivityWorldTeamArenaOtherInfo other_info;
	other_info.is_sign_up = 0;

	InternalComm::Instance().SendToAllGameAllUserThroughCross((const char *)&other_info, sizeof(other_info));
	if (this->IsDirty())
	{
		m_next_save_db_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	}
}

WorldTeamArenaSignUpManager::WorldTeamArenaSignUpManager() : m_change_flag(false), m_load_flag(false), m_next_save_db_timestamp(0)
{
	m_pre_team_sign_up_list.clear();
	m_rank_list.clear();
	m_role_team_map.clear();
	m_team_sign_up_map.clear();
	m_dirty_mark.clear();
	m_old_state.clear();

}

WorldTeamArenaSignUpManager::~WorldTeamArenaSignUpManager()
{
}

void WorldTeamArenaSignUpManager::Save()
{
	if (!this->IsLoadFinish()) return;

	static WorldTeamArenaSignUpParam world_team_arena_sign_up_param;
	this->GetInitParam(&world_team_arena_sign_up_param);

	RMISaveWorldTeamArenaSignUpDataBackObjectImpl *backobj = new RMISaveWorldTeamArenaSignUpDataBackObjectImpl();
	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());

	bool ret = ac.SaveWorldTeamArenaSignUpDataAsyn(world_team_arena_sign_up_param, backobj);
	if (ret)
	{
		this->ClearDirtyMark(&world_team_arena_sign_up_param);
	}
}

void WorldTeamArenaSignUpManager::LoadWorldTeamArenaAllTeamSucc()
{
	if (this->IsLoadFinish())
	{
		m_next_save_db_timestamp = (unsigned int)(EngineAdapter::Instance().Time() + ACTIVITY_WORLD_TEAM_ARENA_SAVE_DB_TIME);
		this->UpdateRankList();
	}
}

bool SortRank(const WorldTeamArenaTeamSignUpSimpleInfo & a, const WorldTeamArenaTeamSignUpSimpleInfo & b)
{
	if (a.total_capablity != b.total_capablity) return a.total_capablity > b.total_capablity;

	return a.team_unique_id < b.team_unique_id;
}

void WorldTeamArenaSignUpManager::QueryTeamDetailedInfo(Role * role, const WorldTeamArenaTeamSignUpData & team_info, int send_type, int notice_type)
{
	if(NULL == role) return;

	crossgameprotocal::GameCrossWorldTeamArenaQueryRankReq gcwtaqrr;
	gcwtaqrr.server_id = LocalConfig::Instance().GetMergeServerId();
	gcwtaqrr.plat_type = LocalConfig::Instance().GetPlatType();
	gcwtaqrr.team_unqiue_id = team_info.team_unique_id;
	gcwtaqrr.send_uid = role->GetUID();
	gcwtaqrr.notice_type = notice_type;
	gcwtaqrr.send_type = send_type;

	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(team_info.member_list) == ARRAY_ITEM_COUNT(gcwtaqrr.rank_user_info));
	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(gcwtaqrr.rank_user_info[0].rank_type_list) == ARRAY_ITEM_COUNT(ACTIVITY_WORLD_TEAM_ARENA_RANK_TYPE_LIST));
	for (int i = 0; i < ARRAY_LENGTH(gcwtaqrr.rank_user_info); i++)
	{
		gcwtaqrr.rank_user_info[i].role_id = team_info.member_list[i].role_uid;
		gcwtaqrr.rank_user_info[i].prof = team_info.member_list[i].profession;
		memcpy(gcwtaqrr.rank_user_info[i].rank_type_list, ACTIVITY_WORLD_TEAM_ARENA_RANK_TYPE_LIST, sizeof(gcwtaqrr.rank_user_info[i].rank_type_list));
	}

	InternalComm::Instance().SendToCross((const char *)&gcwtaqrr, sizeof(gcwtaqrr));
}

void WorldTeamArenaSignUpManager::UpdateRankList()
{
	m_rank_list.clear();

	for (std::map<long long, WorldTeamArenaTeamSignUpData>::const_iterator it = m_team_sign_up_map.begin(); it != m_team_sign_up_map.end(); ++it)
	{
		if(!it->second.IsVaild()) continue;

		WorldTeamArenaTeamSignUpSimpleInfo simple_info;
		simple_info.SetData(it->second);
		m_rank_list.push_back(simple_info);
	}
	std::sort(m_rank_list.begin(), m_rank_list.end(), SortRank);
}

bool WorldTeamArenaSignUpManager::OnCheckCanPreSignUp(Role * role, Team * team, GameName team_name, PreTeamSignUpInfo & team_info)
{
	if(NULL == role || NULL == team) return false;

	std::set<int> uid_list;
	std::set<std::string> team_name_list;

	for (int i = 0; i < (int)m_pre_team_sign_up_list.size(); i++)
	{
		team_name_list.insert(m_pre_team_sign_up_list[i].team_name);
		for (int k = 0; k < ARRAY_LENGTH(m_pre_team_sign_up_list[i].member_list); ++k)
		{
			uid_list.insert(m_pre_team_sign_up_list[i].member_list[k].role_id);
		}
	}
	for (std::map<long long, WorldTeamArenaTeamSignUpData>::const_iterator it = m_team_sign_up_map.begin(); it != m_team_sign_up_map.end(); ++it)
	{
		if (!it->second.IsVaild()) continue;

		team_name_list.insert(it->second.team_name);
		for (int i = 0; i < ARRAY_LENGTH(it->second.member_list); ++i)
		{
			uid_list.insert(it->second.member_list[i].role_uid);
		}
	}
	if (team_name_list.end() != team_name_list.find(team_name))
	{
		role->NoticeNum(errornum::EN_WORLD_TEAM_ARENA_SIGN_UP_FAILED_BY_NAME_REPEAT);
		return false;
	}

	memcpy(team_info.team_name, team_name, sizeof(team_info.team_name));
	const ActivityWorldTeamArenaOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityWorldTeamArenaConfig()->GetOtherCfg();
	for (int i = 0; i < team->GetFollowMemberCount() && i < MAX_TEAM_MEMBER_NUM && i < ARRAY_LENGTH(team_info.member_list); i++)
	{
		TeamMember * member = team->GetMemberInfo(i);
		if (NULL == member)
		{
			role->NoticeNum(errornum::EN_WORLD_TEAM_ARENA_SIGN_UP_FAILED_BY_MEMBER);
			return false;
		}

		if (member->level < other_cfg.need_level)
		{
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_world_team_arena_team_member_level_not_enough, other_cfg.need_level);
			if (length > 0)
			{
				World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(role, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
				return false;
			}
		}
		if (uid_list.end() != uid_list.find(member->uid))
		{
			role->NoticeNum(errornum::EN_WORLD_TEAM_ARENA_SIGN_UP_FAILED_BY_MEMBER_SIGN_UP);
			return false;
		}

		team_info.member_list[i].role_id = member->uid;
		if (role->GetUID() == member->uid)
		{
			team_info.member_list[i].determine_flag = MEMBER_DETERMINE_TYPE_YES;	//队长申请时就确认了
			if (!role->GetRoleModuleManager()->GetMoney()->MoneyTypeMoreThan(other_cfg.registration_type, other_cfg.registration_price, false))
			{
				team->SendToMemberNotice(errornum::EN_WORLD_TEAM_ARENA_SIGN_UP_FAILED_BY_MONEY_NOT_ENOUGH);
				return false;
			}
			if (0 != i)	//队长固定首位
			{
				std::swap(team_info.member_list[0], team_info.member_list[i]);
			}
		}
	}

	return true;
}

bool WorldTeamArenaSignUpManager::OnCheckPreAllDet(PreTeamSignUpInfo & team_info)
{
	bool is_all_confirm = true;
	for (int i = 0; i < ARRAY_LENGTH(team_info.member_list); i++)
	{
		if (MEMBER_DETERMINE_TYPE_YES != team_info.member_list[i].determine_flag)
		{
			is_all_confirm = false;
			break;
		}
	}
	if (!is_all_confirm)
	{
		return false;
	}

	Role * leader = World::GetInstWorld()->GetSceneManager()->GetRole(team_info.member_list[0].role_id);
	if (NULL == leader)
	{
		return true;
	}
	Team * m_team = leader->GetMyTeam();
	if (NULL == m_team || !m_team->IsLeader(leader))
	{
		m_team->SendToMemberNotice(errornum::EN_WORLD_TEAM_ARENA_SIGN_UP_FAILED_BY_MEMBER_CHANGE);
		return true;
	}
	WorldTeamArenaTeamSignUpData team_sign_up_info;
	const ActivityWorldTeamArenaOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityWorldTeamArenaConfig()->GetOtherCfg();
	for (int i = 0; i < ARRAY_LENGTH(team_info.member_list); i++)
	{
		if (0 == i)
		{
			if(!leader->GetRoleModuleManager()->GetMoney()->MoneyTypeMoreThan(other_cfg.registration_type, other_cfg.registration_price, false))
			{	
				m_team->SendToMemberNotice(errornum::EN_WORLD_TEAM_ARENA_SIGN_UP_FAILED_BY_MONEY_NOT_ENOUGH);
				return true;
			}
			UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(team_sign_up_info.member_list) == ARRAY_ITEM_COUNT(team_info.member_list));
			this->FillMemberInfo(leader, team_sign_up_info.member_list[i]);
			continue;
		}

		Role * member = World::GetInstWorld()->GetSceneManager()->GetRole(team_info.member_list[i].role_id);
		if (NULL == member || !m_team->IsMember(member))	//找不到则由外部移除
		{
			m_team->SendToMemberNotice(errornum::EN_WORLD_TEAM_ARENA_SIGN_UP_FAILED_BY_MEMBER_CHANGE);
			return true;
		}

		this->FillMemberInfo(member, team_sign_up_info.member_list[i]);
	}
	
	memcpy(team_sign_up_info.team_name, team_info.team_name, sizeof(team_sign_up_info.team_name));
	if (!UniqueIDGenerator::Instance()->GenUniqueID(&team_sign_up_info.team_unique_id))
	{
		return true;
	}
	if (!leader->GetRoleModuleManager()->GetMoney()->UseMoneyType(other_cfg.registration_type, other_cfg.registration_price, __FUNCTION__))
	{
		m_team->SendToMemberNotice(errornum::EN_WORLD_TEAM_ARENA_SIGN_UP_FAILED_BY_MONEY_NOT_ENOUGH);
		return true;
	}
	
	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_world_team_arena_create_team_succ_notice,
		team_sign_up_info.team_name);
	if (length > 0)
	{
		World::GetInstWorld()->GetSceneManager()->SystemMsgTeamMember(m_team, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
	}
	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(team_sign_up_info.member_list) == 5);
	length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_world_team_arena_create_team_succ_borcast,
		team_sign_up_info.member_list[0].role_name, team_sign_up_info.member_list[1].role_name, team_sign_up_info.member_list[2].role_name, 
		team_sign_up_info.member_list[3].role_name, team_sign_up_info.member_list[4].role_name, team_sign_up_info.team_name);
	if (length > 0)
	{
		World::GetInstWorld()->GetSceneManager()->SystemMsgAllGame(gamestring::GAMESTRING_BUF, length, 
			GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
	}

	m_team_sign_up_map[team_sign_up_info.team_unique_id] = team_sign_up_info;
	m_dirty_mark[team_sign_up_info.team_unique_id] = true;
	m_old_state[team_sign_up_info.team_unique_id] = false;
	m_change_flag = true;
	this->OnInsterSimple(team_sign_up_info);
	this->UpdateRankList();
	//下发协议
	for (int i = 0; i < ARRAY_ITEM_COUNT(team_sign_up_info.member_list); ++i)
	{
		Role * user = World::GetInstWorld()->GetSceneManager()->GetRole(team_sign_up_info.member_list[i].role_uid);
		if (NULL != user)
		{
			this->SendAllSignUpInfo(user, team_sign_up_info.team_unique_id);
		}
	}
	std::string role_string = "member_list{";
	for (int i = 0; i < ARRAY_ITEM_COUNT(team_sign_up_info.member_list); ++i)
	{
		role_string += STRING_SPRINTF(" user_%d[%d, %s]", i + 1, team_sign_up_info.member_list[i].role_uid, team_sign_up_info.member_list[i].role_name);
	}
	role_string += " }";
	gamelog::g_log_world_team_arena.printf(LL_INFO, "%s team_name[%s] team_id[%lld] %s", __FUNCTION__, team_sign_up_info.team_name, team_sign_up_info.team_unique_id,
		role_string.c_str());

	return true;
}

void WorldTeamArenaSignUpManager::FillMemberInfo(Role * role, WorldTeamArenaUserInfo & member_info)
{
	if(NULL == role) return;

	member_info.role_uid = role->GetUID();
	member_info.profession = role->GetProfession();
	member_info.level = role->GetLevel();
	member_info.top_level = role->GetTopLevel();
	member_info.avatar_type = role->GetAvatarType();
	member_info.headshot_id = role->GetHeadshotID();
	member_info.fly_flag = role->GetRoleModuleManager()->GetFlyUpSystem()->GetFlyUpFlag();
	member_info.reserve_sh = 0;
	member_info.capablity = role->GetCapability();
	role->GetName(member_info.role_name);
}

void WorldTeamArenaSignUpManager::OnCmdRankToSignUpList(std::map<int, std::vector<WorldTeamArenaTeamRankData> >& rank_map)
{
	std::map<int, std::vector<WorldTeamArenaTeamRankData> >::const_iterator it = rank_map.begin();
	for (; it != rank_map.end(); ++it)
	{
		for (int i = 0; i < (int)it->second.size(); ++i)
		{
			WorldTeamArenaTeamSignUpData sign_up_data;
			sign_up_data.SetData(it->second[i]);
			m_team_sign_up_map[sign_up_data.team_unique_id] = sign_up_data;
			m_dirty_mark[sign_up_data.team_unique_id] = true;
			m_old_state[sign_up_data.team_unique_id] = false;
			m_change_flag = true;
			this->OnInsterSimple(sign_up_data);
		}
	}

	this->UpdateRankList();
}

void WorldTeamArenaSignUpManager::OnInsterSimple(const WorldTeamArenaTeamSignUpData & team_data)
{
	if(team_data.team_unique_id <= 0) return;

	for (int i = 0; i < ARRAY_ITEM_COUNT(team_data.member_list); ++i)
	{
		m_role_team_map[team_data.member_list[i].role_uid] = team_data.team_unique_id;
	}
}

void WorldTeamArenaSignUpManager::OnEraseSimple(long long team_unique_id)
{
	for (std::map<int, long long>::iterator it = m_role_team_map.begin(); it != m_role_team_map.end();)
	{
		if (it->second == team_unique_id)
		{
			m_role_team_map.erase(it++);
		}
		else
		{
			it++;
		}
	}
}
