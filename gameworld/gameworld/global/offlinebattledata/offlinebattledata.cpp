#include "offlinebattledata.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "global/colosseum/colosseummanager.hpp"
#include "global/cloudarena/cloudarena.hpp"
#include "global/colosseum/gladiator.hpp"
#include "global/cloudarena/cloudarenauser.hpp"
#include "internalcomm.h"
#include "rmiclient/rmiglobal.hpp"
#include "global/rmibackobjdef.h"
#include "gamelog.h"
#include "servercommon/offlinebattedatadef.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "global/usercache/usercache.hpp"
#include "other/route/offlinerobotroute.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"
#include "other/robot/robotconfig.hpp"
#include "gamelog.h"

#ifdef _DEBUG
	static const int SAVE_INTERVAL_S = 10;
#else
	static const int SAVE_INTERVAL_S = 600;
#endif

OfflineBattleDataMgr& OfflineBattleDataMgr::Instance()
{
	static OfflineBattleDataMgr instance;
	return instance;
}

void RMIInitOfflineUserBattleDataBackObjectImpl::__exception(int error)
{
	gamelog::g_log_world.printf(LL_WARNING, "RMIInitOfflineUserBattleDataBackObjectImpl::__exception error[%d]", error);
	gamelog::g_log_serious_error.printf(LL_WARNING, "RMIInitOfflineUserBattleDataBackObjectImpl::__exception error[%d]", error);
}

void RMIInitOfflineUserBattleDataBackObjectImpl::__timeout()
{
	gamelog::g_log_world.printf(LL_WARNING, "RMIInitOfflineUserBattleDataBackObjectImpl::__timeout ");
	gamelog::g_log_serious_error.printf(LL_WARNING, "RMIInitOfflineUserBattleDataBackObjectImpl::__timeout");
}

void RMISaveOfflineUserBattleDataBackObjectImpl::__exception(int error)
{
	gamelog::g_log_world.printf(LL_WARNING, "RMISaveOfflineUserBattleDataBackObjectImpl::__exception error[%d]", error);
	gamelog::g_log_serious_error.printf(LL_WARNING, "RMISaveOfflineUserBattleDataBackObjectImpl::__exception error[%d]", error);
}

void RMISaveOfflineUserBattleDataBackObjectImpl::__timeout()
{
	gamelog::g_log_world.printf(LL_WARNING, "RMISaveOfflineUserBattleDataBackObjectImpl::__timeout ");
	gamelog::g_log_serious_error.printf(LL_WARNING, "RMISaveOfflineUserBattleDataBackObjectImpl::__timeout");
}

void RMISaveOfflineUserBattleDataBackObjectImpl::SaveRet(int ret)
{
	if (0 != ret)
	{
		gamelog::g_log_world.printf(LL_WARNING, "RMISaveOfflineUserBattleDataBackObjectImpl::SaveRet ret:%d", ret);
		gamelog::g_log_serious_error.printf(LL_WARNING, "RMISaveOfflineUserBattleDataBackObjectImpl::SaveRet ret:%d", ret);
	}
}

void RMIInitOfflineUserBattleDataBackObjectImpl::InitRet(int ret, const OfflineUserBattleDataListForInit& param)
{
	OfflineBattleDataMgr::Instance().OnLoadReturn(ret, param);
}

void OfflineBattleDataMgr::OnServerStart()
{
	this->LoadUserData(0LL);
}

void OfflineBattleDataMgr::OnServerStop()
{
	// 这里没有必要SaveAll 因为这个不是特别重要的数据
	this->Save(false);
}

void OfflineBattleDataMgr::Update(unsigned int now)
{
	if (now > m_query_battle_data_from_other_system_timestamp)
	{
		if (!this->QueryBattleDataFromOtherSystem())
		{
			// 不成功就等待3秒后再次尝试
			m_query_battle_data_from_other_system_timestamp
				= static_cast<unsigned int>(EngineAdapter::Instance().Time()) + 3;
		}
		else
		{
			m_query_battle_data_from_other_system_timestamp = 0u;
		}
	}

	if (now > m_next_save_timestamp)
	{
		this->Save(false);
		m_next_save_timestamp = now + SAVE_INTERVAL_S;
	}
}

void OfflineBattleDataMgr::OnLogin(const UserID& user_id)
{
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
	if (NULL == role) return;

	this->UpdateRoleBattleData(role);
}

void OfflineBattleDataMgr::OnLogout(Role* role)
{
	this->UpdateRoleBattleData(role);

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		this->SyncOfflineBattleDataToHidden(role->GetUID());
	}
}

void OfflineBattleDataMgr::OnRoleLevelUp(Role* role)
{
	this->UpdateRoleBattleData(role);
}

const UserOfflineBattleData* OfflineBattleDataMgr::GetUserBattleData(int uid) const
{
	UserMap::const_iterator it = m_user_map.find(uid);
	if (it != m_user_map.end())
	{
		return &it->second;
	}
	else
	{
		return NULL;
	}
}

void OfflineBattleDataMgr::OnDayChange()
{
	UserMap::iterator it = m_user_map.begin();
	for (; it != m_user_map.end(); ++it)
	{
		UserOfflineBattleData* data = &it->second;
		if (data->today_offline_robot_times > 0)
		{
			data->today_offline_robot_times = 0;
			data->is_dirty = true;

			gamelog::g_log_offline_battle_data.printf(LL_INFO, 
				"OnDayChange ResetRobotTimes, role[%d,%s] times[%d]",
				it->first, data->rbd.role_character_data.name, 
				data->today_offline_robot_times);
		}
	}
}

bool OfflineBattleDataMgr::IsLoadFinish() const
{
	return GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH == m_data_load_status;
}

void OfflineBattleDataMgr::OnLoadReturn(int ret, const OfflineUserBattleDataListForInit& param)
{
	if (0 != ret)
	{
		gamelog::g_log_serious_error.printf(LL_ERROR, "%s_1 ret[%d]", __FUNCTION__, ret);
		gamelog::g_log_world.printf(LL_ERROR, "%s_1 ret[%d]", __FUNCTION__, ret);
		ServerLogic::GetInstServerLogic()->StopServer(STRING_SPRINTF("function[%s]_1 ret[%d]", __FUNCTION__, ret).c_str());
		return;
	}

	for (size_t i = 0; i < param.paramlist.size(); ++i)
	{
		if (!this->InitRoleBattleDataFromDB(param.paramlist[i]))
		{
			gamelog::g_log_serious_error.printf(LL_ERROR, "%s_2 ret[%d] uid[%d] duplicate", __FUNCTION__, ret, param.paramlist[i].uid);
			gamelog::g_log_world.printf(LL_ERROR, "%s_2 ret[%d] uid[%d] duplicate", __FUNCTION__, ret, param.paramlist[i].uid);
			//ServerLogic::GetInstServerLogic()->StopServer(STRING_SPRINTF("function[%s]_2 ret[%d] uid[%d] duplicate", __FUNCTION__, ret, param.paramlist[i].uid).c_str());

			continue;
		}
		else
		{
			gamelog::g_log_offline_battle_data.printf(LL_INFO,
				"OnLoadReturn PrintData, role[%d,%s] times[%d]",
				param.paramlist[i].uid, 
				param.paramlist[i].rbd.role_character_data.name,
				param.paramlist[i].today_offline_reward_times);
		}
	}

	if (!param.paramlist.empty())
	{
		this->LoadUserData(param.next_id_from);
	}
	else
	{
		this->OnLoadAllFinish();
	}
}

void OfflineBattleDataMgr::OnQueryOfflineBattleDataForRobot(crossgameprotocal::GameHiddenQueryOfflineBattleDataForRobot* msg, bool is_from_origin_server)
{
	std::vector<UserCacheNode*> candidates;
	std::vector<UserCacheNode*> best_candidates;
	candidates.reserve(50);
	best_candidates.reserve(50);

	for (UserMap::iterator it = m_user_map.begin(); it != m_user_map.end(); ++it)
	{
		int uid = it->first;
		if (m_in_use_set.find(uid) != m_in_use_set.end())
		{
			continue;
		}

		UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(msg->exclude_uid_list) == 5);
		if (uid == msg->exclude_uid_list[0]
			|| uid == msg->exclude_uid_list[1]
			|| uid == msg->exclude_uid_list[2]
			|| uid == msg->exclude_uid_list[3]
			|| uid == msg->exclude_uid_list[4])
		{
			continue;
		}

		if (UserCacheManager::Instance().IsUserOnline(uid))
		{
			continue;
		}

		UserCacheNode* node = UserCacheManager::Instance().GetUserNode(uid);
		if (NULL == node)
		{
			continue;
		}

		unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		if (node->last_online_timestamp + 300 > now)
		{
			continue;
		}

		int level = it->second.rbd.role_character_data.level;
		if (level >= msg->level_min && level <= msg->level_max)
		{
			candidates.push_back(node);
		}

		if (level >= msg->best_level_min && level <= msg->best_level_max)
		{
			best_candidates.push_back(node);
		}
	}

	if (!best_candidates.empty())
	{
		// 在best_candidates中找一个
		UserCacheNode* node = best_candidates[rand() % best_candidates.size()];
		RoleBattleData& rbd = m_user_map[node->uid].rbd;
		OfflineRobotRoute::ResponseOfflineBattleDataForRobot(msg->server_id, msg->team_id, node, rbd, is_from_origin_server);
	
		gamelog::g_log_offline_battle_data.printf(LL_INFO,
			"OnQueryOfflineBattleDataForRobot_1 BeginSummon, role[%d,%s]",
			node->uid,
			node->role_name);

		m_in_use_set.insert(node->uid);
	}
	else if (!candidates.empty())
	{
		// 在candidates中找一个
		UserCacheNode* node = candidates[rand() % candidates.size()];
		RoleBattleData& rbd = m_user_map[node->uid].rbd;
		OfflineRobotRoute::ResponseOfflineBattleDataForRobot(msg->server_id, msg->team_id, node, rbd, is_from_origin_server);
		
		gamelog::g_log_offline_battle_data.printf(LL_INFO,
			"OnQueryOfflineBattleDataForRobot_2 BeginSummon, role[%d,%s]",
			node->uid,
			node->role_name);

		m_in_use_set.insert(node->uid);
	}
}

void OfflineBattleDataMgr::OnNotifyOfflineBattleDataForRobotResult(crossgameprotocal::GameHiddenNotifyOfflineBattleDataForRobotResult* msg)
{
	if (msg->result == crossgameprotocal::GameHiddenNotifyOfflineBattleDataForRobotResult::FAIL)
	{
		gamelog::g_log_offline_battle_data.printf(LL_INFO,
			"OnNotifyOfflineBattleDataForRobotResult_FAILED EndSummon, uid[%d]", msg->uid);

		m_in_use_set.erase(msg->uid);
	}
	else
	{
		gamelog::g_log_offline_battle_data.printf(LL_INFO,
			"OnNotifyOfflineBattleDataForRobotResult_SUCC EndSummon, uid[%d]", msg->uid);

		m_in_use_set.insert(msg->uid);
		this->OnUserBeingUsedOnRobot(msg->uid);
	}
}

void OfflineBattleDataMgr::OnUserBeingUsedOnRobot(int uid)
{
	UserOfflineBattleData* data = this->MutableUserBattleData(uid);
	if (NULL == data) return;

	if (0 == data->today_offline_robot_times)
	{
		this->GiveOfflineRobotReward(uid);
	}
	data->today_offline_robot_times += 1;

	gamelog::g_log_offline_battle_data.printf(LL_INFO,
		"OnUserBeingUsedOnRobot AddTimes, role[%d,%s] times[%d]",
		uid, data->rbd.role_character_data.name, data->today_offline_robot_times);
}

void OfflineBattleDataMgr::OnRealRoleRobotLeaveTeamNotice(crossgameprotocal::GameHiddenRealRoleRobotLeaveTeamNotice* msg)
{
	m_in_use_set.erase(msg->uid);
}

void OfflineBattleDataMgr::OnSyncOfflineBattleData(crossgameprotocal::GameHiddenSyncOfflineBattleData* msg)
{
	UserOfflineBattleData* data = this->MutableUserBattleData(msg->uid);
	if (NULL == data)
	{
		this->InsertRoleBattleData(msg->uid, msg->rbd);
	}
	else
	{
		data->rbd = msg->rbd;
	}
}

void OfflineBattleDataMgr::OnGameServerConnect(const UniqueServerID& mother_server_id, const std::set<int>& server_id_set)
{
	if (m_pull_jjc_data_flag && CrossConfig::Instance().IsHiddenServer())
	{
		this->QueryBattleDataFromOriginServer(mother_server_id, server_id_set);
	}
}

void OfflineBattleDataMgr::OnQueryBattleDataForHiddenOfflineBattleDataSystem(crossgameprotocal::HiddenGameQueryBattleDataForOfflineBattleDataSystem* msg)
{
	std::set<int> all_user_uid_set = ColosseumManager::Instance().GetAllUserUidSet();
	for (std::set<int>::iterator it = all_user_uid_set.begin(); it != all_user_uid_set.end(); ++it)
	{
		int uid = *it;

		if (msg->exclude_uid_set.find(uid) != msg->exclude_uid_set.end()) continue;

		Gladiator* gladiator = ColosseumManager::Instance().GetUserGladiator(IntToUid(uid));
		if (NULL == gladiator) continue;

		const RoleBattleData& battle_data = gladiator->GetRoleBattleData();
		crossgameprotocal::GameHiddenSyncOfflineBattleData ghsobd;
		ghsobd.uid = uid;
		ghsobd.rbd = battle_data;
		InternalComm::Instance().SendSerializeMsgToHiddenThroughCross(&ghsobd);
	}
}


std::vector<const UserOfflineBattleData *> OfflineBattleDataMgr::GetAllUserOfflineBattleData() const
{
	return MapValuesConstPtrAsVector(m_user_map);
}

OfflineBattleDataMgr::OfflineBattleDataMgr() :
	m_query_battle_data_from_other_system_timestamp(0u),
	m_next_save_timestamp(0u),
	m_data_load_status(GLOBAL_SYSTEM_DATA_STATE_INVALID),
	m_pull_jjc_data_flag(false)
{

}

void OfflineBattleDataMgr::OnLoadAllFinish()
{
	m_data_load_status = GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH;

	m_query_battle_data_from_other_system_timestamp = 
		static_cast<unsigned int>(EngineAdapter::Instance().Time()) + 3;

	m_next_save_timestamp = 
		static_cast<unsigned int>(EngineAdapter::Instance().Time()) + SAVE_INTERVAL_S;

	// 数据库中没有数据，则本次开服期间向所有连上的原服都请求竞技场数据
	if (m_user_map.empty() && CrossConfig::Instance().IsHiddenServer())
	{
		m_pull_jjc_data_flag = true;
	}
}

UserOfflineBattleData* OfflineBattleDataMgr::MutableUserBattleData(int uid)
{
	UserMap::iterator it = m_user_map.find(uid);
	if (it != m_user_map.end())
	{
		it->second.is_dirty = true;
		return &it->second;
	}
	else
	{
		return NULL;
	}
}

void OfflineBattleDataMgr::UpdateRoleBattleData(Role* role)
{
	if (NULL == role) return;

	UserOfflineBattleData* uobd = this->MutableUserBattleData(role->GetUID());
	if (NULL == uobd)
	{
		RoleBattleData rbd;
		role->GetRoleModuleManager()->CollectRoleBattleData(&rbd, true);
		this->InsertRoleBattleData(role->GetUID(), rbd);
	}
	else
	{
		role->GetRoleModuleManager()->CollectRoleBattleData(&uobd->rbd, true);
	}
}

bool OfflineBattleDataMgr::QueryBattleDataFromOtherSystem()
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return this->QueryBattleDataFromColosseum();
	}
	else
	{
		return this->QueryBattleDataFromCloudArena();
	}
}

bool OfflineBattleDataMgr::QueryBattleDataFromColosseum()
{
	if (!ColosseumManager::Instance().IsLoadFinish()) return false;

	std::set<int> all_user_uid_set = ColosseumManager::Instance().GetAllUserUidSet();
	for (std::set<int>::iterator it = all_user_uid_set.begin(); it != all_user_uid_set.end(); ++it)
	{
		int uid = *it;
		if (NULL != this->GetUserBattleData(uid)) continue;

		Gladiator* gladiator = ColosseumManager::Instance().GetUserGladiator(IntToUid(uid));
		if (NULL == gladiator) continue;

		this->InsertRoleBattleData(gladiator->GetUid(), gladiator->GetRoleBattleData());
	}

	return true;
}

bool OfflineBattleDataMgr::QueryBattleDataFromCloudArena()
{
	if (!CloudArena::Instance().IsLoadFinish()) return false;

	std::set<int> all_user_uid_set = CloudArena::Instance().GetAllUserUidSet();
	for (std::set<int>::iterator it = all_user_uid_set.begin(); it != all_user_uid_set.end(); ++it)
	{
		int uid = *it;
		if (NULL != this->GetUserBattleData(uid)) continue;

		const CloudArenaUser* user = CloudArena::Instance().GetUser(uid);
		if (NULL == user) continue;

		this->InsertRoleBattleData(uid, user->GetBattleData());
	}

	return true;
}

void OfflineBattleDataMgr::QueryBattleDataFromOriginServer(const UniqueServerID& mother_server_id, const std::set<int>& server_id_set)
{
	crossgameprotocal::HiddenGameQueryBattleDataForOfflineBattleDataSystem msg;
	
	for (UserMap::iterator it = m_user_map.begin(); it != m_user_map.end(); ++it)
	{
		int uid = it->first;
		if (server_id_set.find(IntToUid(uid).db_index) != server_id_set.end())
		{
			msg.exclude_uid_set.insert(uid); 
		}
	}

	InternalComm::Instance().SendSerializeMsgToGameThroughCross(&msg, mother_server_id.server_id);
}

void OfflineBattleDataMgr::InsertRoleBattleData(int uid, const RoleBattleData& rbd)
{
	UserOfflineBattleData tmp;
	tmp.is_dirty = true;
	tmp.today_offline_robot_times = 0;
	tmp.rbd = rbd;
	m_user_map[uid] = tmp;
}

bool OfflineBattleDataMgr::InitRoleBattleDataFromDB(const OfflineUserBattleDataParam& param)
{
	if (this->GetUserBattleData(param.uid))
	{
		return false; // 重复了
	}

	UserOfflineBattleData tmp;
	tmp.is_dirty = false;
	tmp.today_offline_robot_times = param.today_offline_reward_times;
	tmp.rbd = param.rbd;
	m_user_map[param.uid] = tmp;

	return true;
}

void OfflineBattleDataMgr::Save(bool is_save_all)
{
	OfflineUserBattleDataListForSave param;
	UserMap::iterator it = m_user_map.begin();
	for (; it != m_user_map.end(); ++it)
	{
		UserOfflineBattleData& user = it->second;
		if (!is_save_all && !user.is_dirty) continue;

		OfflineUserBattleDataParam tmp;
		tmp.uid = it->first;
		tmp.today_offline_reward_times = user.today_offline_robot_times;
		tmp.rbd = user.rbd;
		param.paramlist.push_back(tmp);

		if (param.paramlist.size() >= OFFLINE_USER_BATTLE_DATA_ONCE_OPER_COUNT)
		{
			RMISaveOfflineUserBattleDataBackObjectImpl* impl = new RMISaveOfflineUserBattleDataBackObjectImpl();
			RMIGlobalClient ac;
			ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
			ac.SaveOfflineUserBattleDataAsync(param, impl);
			param.paramlist.clear();
		}
	}

	if (!param.paramlist.empty())
	{
		RMISaveOfflineUserBattleDataBackObjectImpl* impl = new RMISaveOfflineUserBattleDataBackObjectImpl();
		RMIGlobalClient ac;
		ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
		ac.SaveOfflineUserBattleDataAsync(param, impl);
		param.paramlist.clear();
	}
	
	this->ClearDirtyFlag();
}

void OfflineBattleDataMgr::ClearDirtyFlag()
{
	UserMap::iterator it = m_user_map.begin();
	for (; it != m_user_map.end(); ++it)
	{
		it->second.is_dirty = false;
	}
}

bool OfflineBattleDataMgr::LoadUserData(long long next_id_from)
{
	if (GLOBAL_SYSTEM_DATA_STATE_INVALID == m_data_load_status)
	{
		m_data_load_status = GLOBAL_SYSTEM_DATA_STATE_LOADING;
	}

	RMIInitOfflineUserBattleDataBackObjectImpl* impl = new RMIInitOfflineUserBattleDataBackObjectImpl();

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());

	return ac.InitOfflineUserBattleDataAsync(next_id_from, impl);
}

void OfflineBattleDataMgr::GiveOfflineRobotReward(int uid)
{
	static MailContentParam contentparam;
	contentparam.Reset();
	int len_1 = SNPRINTF(contentparam.subject,
		sizeof(contentparam.subject),
		gamestring::g_real_role_robot_reward_subject);
	int len_2 = SNPRINTF(contentparam.contenttxt,
		sizeof(contentparam.contenttxt),
		gamestring::g_real_role_robot_reward_content);

	const RobotOtherCfg& cfg = ROBOTCONFIG->GetOtherCfg();
	for (size_t i = 0; i < cfg.real_role_robot_rewards.size()
		&& i < ARRAY_ITEM_COUNT(contentparam.item_list); ++i)
	{
		contentparam.item_list[i].item_id = cfg.real_role_robot_rewards[i].item_id;
		contentparam.item_list[i].is_bind = cfg.real_role_robot_rewards[i].is_bind ? 1 : 0;
		contentparam.item_list[i].num = cfg.real_role_robot_rewards[i].num;
	}

	MailRoute::MailToUser(IntToUid(uid), MAIL_REASON_DEFAULT, contentparam);

	gamelog::g_log_offline_battle_data.printf(LL_INFO,
		"GiveOfflineRobotReward GiveMail, role[%d]", uid);
}

void OfflineBattleDataMgr::SyncOfflineBattleDataToHidden(int uid)
{
	const UserOfflineBattleData* data = this->GetUserBattleData(uid);
	if (NULL == data) return;

	crossgameprotocal::GameHiddenSyncOfflineBattleData msg;
	msg.uid = uid;
	msg.rbd = data->rbd;

	InternalComm::Instance().SendSerializeMsgToHiddenThroughCross(&msg);
}

OfflineBattleDataMgr::~OfflineBattleDataMgr()
{

}

