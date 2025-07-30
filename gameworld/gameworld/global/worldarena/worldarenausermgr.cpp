#include "worldarenausermgr.hpp"
#include "worldarena.hpp"
#include "worldarenauser.hpp"
#include "servercommon/serverconfig/localconfig.hpp"
#include "other/robot/robotmanager.hpp"
#include "servercommon/robotdef.hpp"
#include "global/rmibackobjdef.h"
#include "internalcomm.h"
#include "gamelog.h"
#include "other/robot/robotconfig.hpp"
#include "protocol/msgworldarena.hpp"
#include "other/route/mailroute.hpp"
#include "servercommon/string/gameworldstr.h"

WorldArenaUserMgr::WorldArenaUserMgr(WorldArena* wa) : m_arena(wa), m_data_load_status(GLOBAL_SYSTEM_DATA_STATE_INVALID)
{

}

WorldArenaUserMgr::~WorldArenaUserMgr()
{

}

void WorldArenaUserMgr::OnDayChange()
{
	UserMap::iterator it = m_user_map.begin();
	for (; it != m_user_map.end(); ++it)
	{
		WorldArenaUser* user = it->second;
		user->OnDayChange();
	}
}

bool WorldArenaUserMgr::AddUserFromInit(const WorldArenaUserInfo& info)
{
	WorldArenaUser* user = this->GetWorldArenaUser(info.signup_info.uuid);
	if (NULL != user)
	{
		gamelog::g_log_serious_error.printf(LL_WARNING, "WorldArenaUserMgr::AddUserFromInit duplicate user[%lld,%s]",
			info.signup_info.uuid, info.signup_info.role_name);
		gamelog::g_log_world_arena.printf(LL_WARNING, "WorldArenaUserMgr::AddUserFromInit duplicate user[%lld,%s]",
			info.signup_info.uuid, info.signup_info.role_name);
		return false;
	}

	user = this->CreateAndInsertUser(info.signup_info.uuid);
	assert(NULL != user);

	user->SetUserInfoForInit(info);

	gamelog::g_log_world_arena.printf(LL_INFO, "AddUserFromInit user[%d,%lld,%s]", user->GetUID(), user->GetUUID(), user->GetName());
	return true;
}

bool WorldArenaUserMgr::OnUserSignUp(const WorldArenaSignUpInfo& signup_info, const RoleBattleData& rbd)
{
	WorldArenaUser* user = this->CreateAndInsertUser(signup_info.uuid);
	if (NULL == user) return false;

	user->SetSignUpInfo(signup_info);
	user->SetRoleBattleData(rbd);
	user->SetSeasonIndex(m_arena->GetCurSeasonIndex());
	return true;
}

WorldArenaUser* WorldArenaUserMgr::CreateAndInsertUser(long long uuid)
{
	WorldArenaUser* user= this->GetWorldArenaUser(uuid);
	if (NULL != user)
	{
		return user;
	}

	user = new WorldArenaUser(false);
	m_user_map.insert(UserMap::value_type(uuid, user));

	return user;
}

WorldArenaUser* WorldArenaUserMgr::CreateAndInsertRobot(long long uuid, int level)
{
	if (NULL != this->GetWorldArenaUser(uuid))
	{
		assert(0);
		return NULL;
	}

	WorldArenaUser* user = new WorldArenaUser(false);
	m_user_map.insert(UserMap::value_type(uuid, user));

	int robot_sid = 0;
	World::GetInstWorld()->RandOneMotherServerId(&robot_sid);

	WorldArenaSignUpInfo robot_info;
	robot_info.uuid = uuid;
	robot_info.plat_type = LongLongToUniqueUserID(uuid).plat_type;
	robot_info.uid = UidToInt(LongLongToUniqueUserID(uuid).user_id);
	RobotParam robot_param;
	RobotManager::Instance().ConstructRobotParamByLevel(level, ROBOT_CONFIG_USE_TYPE_JJC_AND_PLATFORM, true, &robot_param, robot_sid, RobotConfig::ROBOT_NAME_TYPE_ROLE);
	robot_info.headshot_id = robot_param.role_battle_data.appearance.headshot_id;
	robot_info.avatar_type = robot_param.avatar_type;
	robot_info.level = level;
	robot_info.profession = robot_param.profession;
	robot_info.top_level = 0;
	F_STRNCPY(robot_info.role_name, robot_param.robot_name, sizeof(robot_info.role_name));
	user->SetSignUpInfo(robot_info);
	user->SetRoleBattleData(robot_param.role_battle_data);

	return user;
}

WorldArenaUser* WorldArenaUserMgr::GetWorldArenaUser(long long uuid)
{
	UserMap::iterator it = m_user_map.find(uuid);
	if (it != m_user_map.end())
	{
		return it->second;
	}

	return NULL;
}

void WorldArenaUserMgr::OnSeasonRestart()
{
	UserMap::iterator it = m_user_map.begin();
	for (; it != m_user_map.end(); ++it)
	{
		m_to_delete_user_map.insert(UserMap::value_type(it->first, it->second));
	}

	m_user_map.clear();

}

bool WorldArenaUserMgr::Save(bool is_save_all)
{
	WorldArenaUserListParamForSave param;
	
	UserMap::iterator it = m_user_map.begin();
	for (; it != m_user_map.end(); ++it)
	{
		WorldArenaUser* user = it->second;
		if (!is_save_all && !user->IsDirty()) continue;

		param.save_user_list.push_back(user->GetUserInfoForSave());
	}

	it = m_to_delete_user_map.begin();
	for (; it != m_to_delete_user_map.end(); ++it)
	{
		WorldArenaUser* user = it->second;
		param.delete_user_list.push_back(user->GetUUID());
	}
	
	RMISaveWorldArenaUserDataBackObjectImpl* impl = new RMISaveWorldArenaUserDataBackObjectImpl();
	impl->SetDeleteList(param.delete_user_list);

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());

	bool ret = ac.SaveWorldArenaUserDataAsyn(param, impl);
	if (ret)
	{
		this->SetUserLastSaveTimestampMS(param.save_user_list);
	}

	return ret;
}

bool WorldArenaUserMgr::LoadUser(long long id_from)
{
	if (GLOBAL_SYSTEM_DATA_STATE_INVALID == m_data_load_status)
	{
		m_data_load_status = GLOBAL_SYSTEM_DATA_STATE_LOADING;
	}

	RMIInitWorldArenaUserDataBackObjectImpl* impl = new RMIInitWorldArenaUserDataBackObjectImpl();

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());

	return ac.InitWorldArenaUserDataAsyn(id_from, impl);
}

void WorldArenaUserMgr::OnLoadUserSucc()
{
	m_data_load_status = GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH;
}

bool WorldArenaUserMgr::IsLoadFinish()
{
	return GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH == m_data_load_status;
}

std::vector<WorldArenaUser*> WorldArenaUserMgr::GetUsersInGroup(long long group_id)
{
	std::vector<WorldArenaUser*> users;
	users.reserve(WORLD_ARENA_FIGHTER_NUM_PER_GROUP);

	UserMap::iterator it = m_user_map.begin();
	for (; it != m_user_map.end(); ++it)
	{
		WorldArenaUser* user = it->second;
		if (user->GetGroupID() == group_id)
		{
			users.push_back(user);
		}
	}

	return users;
}

void WorldArenaUserMgr::NoticeAllUsersStartEnterScene(int start_round_index, unsigned int end_timestamp)
{
	Protocol::SCWorldArenaStartEnterSceneNotice msg;
	msg.enter_scene_end_timestamp = end_timestamp;
	msg.start_round_index = start_round_index;

	UserMap::iterator it = m_user_map.begin();
	for (; it != m_user_map.end(); ++it)
	{
		WorldArenaUser* user = it->second;
		if (user->IsRobot()) continue;

		long long schedule_id = user->GetScheduleID(start_round_index);
		if (0 == schedule_id) continue;

		int fight_result = m_arena->GetFightResult(schedule_id);
		if (fight_result != WORLD_ARENA_RESULT_INVALID)
		{
			msg.has_finish_this_round = 1;
		}
		else
		{
			msg.has_finish_this_round = 0;
		}

		World::GetInstWorld()->SendToRole(user->GetUID(), &msg, sizeof(msg));
	}
}

void WorldArenaUserMgr::SendScheduleReleaseNoticeMailToAllUsers()
{
	std::vector<UserID> user_list;
	user_list.reserve(m_user_map.size());

	UserMap::iterator it = m_user_map.begin();
	for (; it != m_user_map.end(); ++it)
	{
		WorldArenaUser* user = it->second;
		if (user->IsRobot()) continue;

		user_list.push_back(IntToUid(user->GetUID()));
	}

	MailContentParam contentparam;
	int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_world_arena_schedule_release_notice_subject);
	int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_world_arena_schedule_release_notice_content);
	if (length1 > 0 && length2 > 0 && !user_list.empty())
	{
		MailRoute::MailToMultUser(&user_list[0], (int)user_list.size(), MAIL_REASON_DEFAULT, contentparam);
	}
}

void WorldArenaUserMgr::ClearDirtyMark()
{
	UserMap::iterator it = m_user_map.begin();
	for (; it != m_user_map.end(); ++it)
	{
		WorldArenaUser* user = it->second;
		user->ClearDirtyMark();
	}
}

void WorldArenaUserMgr::ClearDeletedUser(const std::vector<long long>& delete_list)
{
	for (size_t i = 0; i < delete_list.size(); ++i)
	{
		UserMap::iterator it = m_to_delete_user_map.find(delete_list[i]);
		if (it != m_to_delete_user_map.end())
		{
			delete it->second;
			it->second = NULL;
			m_to_delete_user_map.erase(it);
		}
	}
}

void WorldArenaUserMgr::SetUserLastSaveTimestampMS(const std::vector<WorldArenaUserInfo>& save_user_list)
{
	for (size_t i = 0; i < save_user_list.size(); ++i)
	{
		long long uuid = save_user_list[i].signup_info.uuid;
		WorldArenaUser* user = this->GetWorldArenaUser(uuid);
		if (NULL != user)
		{
			user->SetLastSaveTimestampMS(GetNowTime_MS());
		}
	}
}

