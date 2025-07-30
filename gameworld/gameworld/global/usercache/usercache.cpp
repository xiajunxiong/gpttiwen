#include <algorithm>
#include "obj/character/role.h"
#include "world.h"
#include "usercache.hpp"
#include "gamelog.h"
#include "common/bintohex.h"
#include "friend/friendmanager.h"
#include "friend/friendrecommend.hpp"
#include "other/rolemodulemanager.hpp"
#include "servercommon/dbhandler.h"
#include "internalcomm.h"
#include "global/rmibackobjdef.h"
#include "rmibackobjdef.h"
#include "global/rank/rankmanager.hpp"
#include "servercommon/struct/roleinitparam.h"
#include "servercommon/struct/roleotherinitparam.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "protocol/msgrole.h"
#include <map>

#include "global/datasynccheck/datasynccheck.hpp"

static const int CAN_CREAT_AUTO_ROBOT = 20;

UserCacheManager::UserCacheManager() : m_highest_role_level(0)
{
	m_can_creat_static_robot = 0;
}

UserCacheManager::~UserCacheManager()
{

}

UserCacheManager & UserCacheManager::Instance()
{
	static UserCacheManager ws;
	return ws;
}

bool UserCacheManager::LoadAllUser(TMysqlStatement &stMysqlStatement)
{
	char szErrMsg[1024];
	int iRet = 0;

	int iErrno;
	stMysqlStatement.iQueryType = SQL_TYPE_SELECT;
	strncpy(stMysqlStatement.szSQLString, 
		"SELECT A.`role_id`, A.`role_name`, A.`profession`, A.`level`, A.`avatar_type`, A.`last_save_time`, A.`top_level`, A.`next_top_level_refresh_timestamp`, B.`avatar_timestamp`, B.`capability`, B.`appearance_data`\
		FROM `role` as A, `role_attr_detail` as B WHERE A.`role_id` > 0 AND A.`role_id` = B.`role_id`",
		sizeof(stMysqlStatement.szSQLString) - 1);//query
	
	iRet = CMysqlUtility::ExecSelect(&stMysqlStatement, &iErrno, szErrMsg, ARRAY_LENGTH(szErrMsg));
	if(0 != iRet)
	{
		CMysqlUtility::FreeResult(&stMysqlStatement);

		gamelog::g_log_world.printf(LL_MAINTANCE, "UserCacheManager::LoadAllUser select failed with :%d errmsg:%s",
			iErrno, szErrMsg);

		return false;
	}

	int iFetchRows = stMysqlStatement.iResNum;

	enum field
	{
		field_uid = 0,
		field_name,
		field_prof,
		field_level,
		field_avatar_type,
		field_last_save_time,
		field_top_level,
		filed_next_top_level_refresh_timestamp,
		field_avatar_timestamp,
		field_capability,
		field_appearance,
	};

	for (int i = 0; i < iFetchRows; ++i)
	{
		iRet = CMysqlUtility::FetchRow(&stMysqlStatement, szErrMsg);
		if(0 != iRet)
		{
			gamelog::g_log_world.printf(LL_MAINTANCE, "UserCacheManager::LoadAllUser when fetched data has error :%s,index:%d",
				szErrMsg, i);

			break;
		}

		UserCacheNode user_node;

		if (stMysqlStatement.stRow[field_uid])
		{
			user_node.uid = atoi(stMysqlStatement.stRow[field_uid]);
		}

		if (stMysqlStatement.stRow[field_name] && stMysqlStatement.alFieldsLength[field_name] > 0)
		{
			const char* user_name = stMysqlStatement.stRow[field_name];
			SNPRINTF(user_node.role_name, sizeof(user_node.role_name) - 1, "%s", user_name);
		}

		if (stMysqlStatement.stRow[field_prof])
		{
			user_node.profession = atoi(stMysqlStatement.stRow[field_prof]);
		}

		if (stMysqlStatement.stRow[field_level])
		{
			user_node.level = atoi(stMysqlStatement.stRow[field_level]);
		}

		if (stMysqlStatement.stRow[field_avatar_type])
		{
			user_node.avatar_type = atoi(stMysqlStatement.stRow[field_avatar_type]);
		}

		if (stMysqlStatement.stRow[field_last_save_time])
		{
			user_node.last_online_timestamp = (unsigned int)atol(stMysqlStatement.stRow[field_last_save_time]);
		}

		if (stMysqlStatement.stRow[field_top_level])
		{
			user_node.top_level = atoi(stMysqlStatement.stRow[field_top_level]);
		}

		if (stMysqlStatement.stRow[filed_next_top_level_refresh_timestamp])
		{
			user_node.next_top_level_refresh_timestamp = (unsigned int)atoi(stMysqlStatement.stRow[filed_next_top_level_refresh_timestamp]);
		}

		if (stMysqlStatement.stRow[field_avatar_timestamp])
		{
			user_node.avatar_timestamp = atoll(stMysqlStatement.stRow[field_avatar_timestamp]);
		}

		if (stMysqlStatement.stRow[field_capability])
		{
			user_node.capability = atoi(stMysqlStatement.stRow[field_capability]);
		}
		
		if (stMysqlStatement.stRow[field_appearance] && stMysqlStatement.alFieldsLength[field_appearance] > 0)
		{
			int app_len = stMysqlStatement.alFieldsLength[field_appearance];
			const char* app_str = stMysqlStatement.stRow[field_appearance];
			HexToBin(app_str, app_len, (char *)&user_node.appearance);
		}

		this->UpdateCacheNode(user_node, true);
		this->UpdateUserOnlineStatus(user_node.uid, UserStatusNode::STATUS_TYPE_OFFLINE);
		this->UpdateUserNameCache(user_node.role_name, user_node.uid);
	}

	CMysqlUtility::FreeResult(&stMysqlStatement);

	if (0 != iRet)
	{
		return false;
	}

	gamelog::g_log_world.printf(LL_MAINTANCE, "UserCacheManager::LoadAllUser total user count:%d",
		this->CountOfUser());
	return true;
}

UserCacheNode * UserCacheManager::GetUserNode(const UserID &user_id, bool trace_nonexist)
{
	return this->GetUserNode(UidToInt(user_id), trace_nonexist);
}

UserCacheNode * UserCacheManager::GetUserNode(int uid, bool trace_nonexist)
{
	if (uid == 0)
	{
		return NULL;
	}

	std::map<int, UserCacheNode>::iterator iter = m_user_node_map.find(uid);
	if (iter != m_user_node_map.end())
	{
		return &(iter->second);
	}

	if (trace_nonexist)
	{
		gamelog::g_log_serious_error.printf(LL_INFO, "UserCache no uid:%d", uid);
	}

	return NULL;
}

UserCacheNode * UserCacheManager::GetUserNodeByName(std::string user_name, bool *is_online, bool trace_nonexist)
{
	if (NULL == is_online)
	{
		return NULL;
	}

	*is_online = false;

	std::map<std::string, UserCacheNode*>::iterator name_usercache_it = m_name_usercache_map.find(user_name);
	if (name_usercache_it != m_name_usercache_map.end())
	{
		*is_online = this->IsUserOnline(name_usercache_it->second->uid);
		return name_usercache_it->second;
	}

	if (trace_nonexist)
	{
		gamelog::g_log_serious_error.printf(LL_INFO, "UserCacheManager::GetUserNodeByName no name %s", user_name.c_str());
	}

	return NULL;
}

UserCacheNode ** UserCacheManager::FuzzySearchUsersByName(std::string search_name, int * count, int exclude_role_id)
{
	static const int MAX_SEARCH_ROLES_RET = 32;
	static UserCacheNode* users[MAX_SEARCH_ROLES_RET] = { NULL };
	*count = 0;

	std::map<std::string, UserCacheNode*>::iterator it = m_name_usercache_map.begin();
	for ( ;it != m_name_usercache_map.end(); ++it)
	{
		if (it->first.find(search_name) != std::string::npos)
		{
			if(exclude_role_id == it->second->uid) continue;

			UserCacheNode* temp = it->second;
			if (temp != NULL)
			{
				users[*count] = temp;
				*count = *count + 1;
				if (*count >= MAX_SEARCH_ROLES_RET)
				{
					break;
				}
			}
		}
	}

	return users;
}
int UserCacheManager::GetUserTopLevel(const UserID & user_id)
{
	return this->GetUserTopLevel(UidToInt(user_id));
}
int UserCacheManager::GetUserTopLevel(int uid)
{
	std::map<int, UserCacheNode>::iterator iter = m_user_node_map.find(uid);
	if (iter != m_user_node_map.end())
	{
		return iter->second.top_level;
	}

	return 0;
}
void UserCacheManager::UpdateCacheNode(UserCacheNode &node, bool do_not_sync /* = false */)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		std::map<int, UserCacheNode>::iterator it = m_user_node_map.find(node.uid);
		if (it != m_user_node_map.end())
		{
			it->second = node;
		}
		else
		{
			m_user_node_map.insert(std::map<int, UserCacheNode>::value_type(node.uid, node));
			this->UpdateUserNameCache(node.role_name, node.uid);
		}

		if (!do_not_sync)
		{
			this->SynCacheToGame(node.uid);
		}
	}
	else
	{
		std::map<int, UserCacheNode>::iterator it = m_user_node_map.find(node.uid);
		if (it != m_user_node_map.end())
		{
			it->second = node;
			if (!do_not_sync)
			{
				this->SyncCacheToHidden(node.uid, false);
			}
		}
		else
		{
			m_user_node_map.insert(std::map<int, UserCacheNode>::value_type(node.uid, node));
			this->UpdateUserNameCache(node.role_name, node.uid);

			if (!do_not_sync)
			{
				this->SyncCacheToHidden(node.uid, true);
			}

			//todo 改估计计数
			if (0 == m_can_creat_static_robot 
				&& CAN_CREAT_AUTO_ROBOT < (int)m_user_node_map.size())
			{
				m_can_creat_static_robot = 1;
				if (!do_not_sync)
				{
					this->SyncGameHiddenCanCreatRobotInfo();
				}
			}
		}
	}

	this->OnCacheNodeUpdated(node);
}

void UserCacheManager::OnCacheNodeUpdated(UserCacheNode& node)
{
	if (node.GetLevel() > m_highest_role_level)
	{
		m_highest_role_level = node.GetLevel();
	}
}

void UserCacheManager::OnConnectCrossServerSucc()
{
	this->OnlineRoleNoticeHidden();
}

void UserCacheManager::OnlineRoleNoticeHidden()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	for (std::map<int, UserStatusNode>::iterator it = m_uid_status_map.begin(); it != m_uid_status_map.end(); it++)
	{
		if (it->second.user_online_status != UserStatusNode::STATUS_TYPE_ONLINE) continue;

		crossgameprotocal::GameHiddenCommonInfoNoticeHidden send_hidden_req;
		send_hidden_req.type = crossgameprotocal::GAME_HIDDEN_COMMON_INFO_NOTICE_TYPE_ROLE_LOGIN;
		send_hidden_req.server_id = LocalConfig::Instance().GetMergeServerId();
		send_hidden_req.is_all_sync = 1;
		send_hidden_req.param1 = it->second.uid;
		InternalComm::Instance().SendToHiddenThroughCross((const char *)&send_hidden_req, sizeof(send_hidden_req));

		// 连接上跨服时 , 同步玩家的在线状态到隐藏服
		this->SyncOnlineStatus(it->second.uid, UserStatusNode::STATUS_TYPE_ONLINE);

		DataSyncCheck::Instance().AddCheck(crossgameprotocal::DATA_SYNC_CHECK_TYPE_USER_CACHE);
	}
}

void UserCacheManager::OnTopLevelReset(unsigned int next_refresh_timestamp)
{
	for (std::map<int, UserCacheNode>::iterator iter = m_user_node_map.begin(); iter != m_user_node_map.end(); iter++)
	{
		if(iter->second.level < MAX_ROLE_LEVEL || iter->second.top_level <= 0 || iter->second.next_top_level_refresh_timestamp == next_refresh_timestamp) continue;

		std::map<int, UserStatusNode>::iterator it = m_uid_status_map.find(iter->first);
		if (it != m_uid_status_map.end() && it->second.user_online_status >= UserStatusNode::STATUS_TYPE_BOTH && it->second.user_online_status <= UserStatusNode::STATUS_TYPE_OFFLINE)
		{
			iter->second.next_top_level_refresh_timestamp = next_refresh_timestamp;
			if (iter->second.top_level != 1)
			{
				iter->second.top_level = 1;
				RankManager::Instance().OnTopLevelResetSyn(&iter->second);
			}
			this->UpdateCacheNode(iter->second);
		}
	}
}

void UserCacheManager::OnTopLevelLog(unsigned int start_timestamp, unsigned int end_timestamp)
{
	//只在原服
	if (CrossConfig::Instance().IsHiddenServer()) return;
	
	for (std::map<int, UserCacheNode>::iterator iter = m_user_node_map.begin(); iter != m_user_node_map.end(); iter++)
	{
		if (iter->second.level < MAX_ROLE_LEVEL || iter->second.top_level <= 0) continue;

		ROLE_LOG_QUICK16(IntToUid(iter->second.uid).db_index, LOG_TYPE_TOP_LEVEL, iter->second.uid, iter->second.role_name, NULL, iter->second.top_level, 0, __FUNCTION__, NULL, 
			start_timestamp, end_timestamp, 0, 0, 0);
	}
}

void UserCacheManager::UpdateUserOnlineStatus(int uid, int online_status)
{
	UserStatusNode node;
	node.uid = uid;
	node.user_online_status = online_status;

	std::map<int, UserStatusNode>::iterator it = m_uid_status_map.find(uid);
	if (it == m_uid_status_map.end())
	{
		it = m_uid_status_map.insert(std::map<int, UserStatusNode>::value_type(uid, node)).first;
		m_all_user_list.push_back(&it->second);
	}
	else
	{
		it->second = node;
	}
}

void UserCacheManager::UpdateUserNameCache(const std::string& user_name, int uid)
{
	UserCacheNode* node = this->GetUserNode(uid);
	if (NULL != node)
	{
		m_name_usercache_map[user_name] = node;
	}
}

// 登录隐藏服	隐藏服内：cross		原服务器内：cross
// 离线			隐藏服内：offline	原服务器内：offline
// 登录本地服	隐藏服内：offline	原服务器内：online
// 看别服玩家   隐藏服内: cross     原服务器内: cross
void UserCacheManager::UpdateUserNameCache(Role *role)
{
	if (NULL == role)
	{
		return;
	}
	
	std::string user_name = role->GetName();
	int uid = role->GetUID();

	this->UpdateUserNameCache(user_name, uid);
	
	
	//const static int NET_ID_MAX_COUNT = 512;

	//int netid_count = 0;
	//static GSNetID netid_list[NET_ID_MAX_COUNT];

	// TODO 通知好友、队友等， 该玩家上线或下线了
}

bool UserCacheManager::IsUserOnline(int uid)
{
	return this->GetUserOnlineStatus(uid) > 0;
}

bool UserCacheManager::IsUserOnline(UserID user_id)
{
	return this->GetUserOnlineStatus(user_id) > 0;
}

int UserCacheManager::GetUserOnlineStatus(int uid)
{
	if (CrossConfig::Instance().IsHiddenServer() || LocalConfig::Instance().IsAllowedServerId(IntToUid(uid).db_index))
	{
		std::map<int, UserStatusNode>::iterator uid_status_it = m_uid_status_map.find(uid);
		if (uid_status_it != m_uid_status_map.end())
		{
			return uid_status_it->second.user_online_status;
		}
	}
	else
	{
		std::map<int, UserStatusNode>::iterator uid_status_it = m_other_server_uid_status_map.find(uid);
		if (uid_status_it != m_other_server_uid_status_map.end())
		{
			int online_status = uid_status_it->second.user_online_status;

			// 别服玩家，没有本服在线状态
			if (online_status == UserStatusNode::STATUS_TYPE_ONLINE && !CrossConfig::Instance().IsHiddenServer())
			{
				online_status = UserStatusNode::STATUS_TYPE_CROSS;
			}

			return online_status;
		}
	}
	
	return UserStatusNode::STATUS_TYPE_OFFLINE;
}

int UserCacheManager::GetUserOnlineStatus(UserID user_id)
{
	return this->GetUserOnlineStatus(UidToInt(user_id));
}

int UserCacheManager::CountOfUser()
{
	return static_cast<int>(m_user_node_map.size());
}

void UserCacheManager::OnResetName(int role_id, GameName new_name, GameName old_name)
{
	m_name_usercache_map.erase(old_name);
	this->UpdateUserNameCache(new_name, role_id);

	std::map<int, UserCacheNode>::iterator it = m_user_node_map.find(role_id);
	if (m_user_node_map.end() != it)
	{
		F_STRNCPY(it->second.role_name, new_name, sizeof(it->second.role_name));
	}

	this->SyncCacheToHidden(role_id, false, old_name);
}

int UserCacheManager::GetUserIDList(int &from, int user_count, UserID *user_list, UserCacheFilterFunc filter_func, void **func_param)
{
	if (from < 0 || user_count <= 0 || NULL == user_list || m_uid_status_map.empty() || from >= (int)m_uid_status_map.size())
	{
		return 0;
	}

	int get_count = 0;

	std::map<int, UserCacheNode>::iterator it = m_user_node_map.begin();
	advance(it, from);

	for (; it != m_user_node_map.end() && get_count < user_count; ++ it)
	{
		++ from;

		if (NULL != filter_func && !filter_func(it->second, func_param))
		{
			continue;
		}

		user_list[get_count] = IntToUid(it->first);
		++ get_count;
	}

	return get_count;
}

int UserCacheManager::GetUidListByStatusMap(int &from, int user_count, int *user_list, int status_type, Role* role)
{
	if (from < 0 || user_count <= 0 || NULL == user_list || m_uid_status_map.empty() || m_all_user_list.empty() ||
		status_type < UserStatusNode::STATUS_TYPE_BOTH || status_type > UserStatusNode::STATUS_TYPE_ONLINE || NULL == role)
	{
		return 0;
	}

	int size = (int)m_all_user_list.size();
	if (from >= size)
	{
		from = 0;
	}
	int origin_from = from;

	int get_count = 0;
	std::vector<const UserStatusNode*>::iterator it = m_all_user_list.begin();
	advance(it, from);

	int except_role_id = role->GetUID();
	int run_count = 0;
	for (; get_count < user_count && run_count < 1000000; ++run_count) //run_count防止算法写错导致的死循环
	{
		const UserStatusNode* node_ptr = *it;
		if (NULL == node_ptr || except_role_id == node_ptr->uid || (UserStatusNode::STATUS_TYPE_BOTH != status_type && (node_ptr->user_online_status > 0 ? 1 : 0) != status_type) ||
			FriendManager::Instance().IsBlack(role->GetUID() , node_ptr->uid) ||
			FriendManager::Instance().IsFriend(role->GetUID(), node_ptr->uid) ||
			role->GetRoleModuleManager()->GetFriendRecommend()->IsLastTimeRefreshUid(node_ptr->uid))
		{
			//do nothing
		}
		else
		{
			user_list[get_count] = node_ptr->uid;
			++ get_count;
		}
		
		++ it;
		++ from;

		if (from >= size || it == m_all_user_list.end())
		{
			from = 0;
			it = m_all_user_list.begin();
		}

		if (from == origin_from) //如果走完一圈没找到满足条件的，则退出循环
		{
			break;
		}
	}

	return get_count;
}

int UserCacheManager::GetUidListByStatusMap(std::vector<int> * user_list, int status_type)
{
	if(NULL == user_list || status_type < UserStatusNode::STATUS_TYPE_BOTH || status_type > UserStatusNode::STATUS_TYPE_CROSS) return 0;

	user_list->reserve(m_uid_status_map.size());
	for (std::map<int, UserStatusNode>::iterator it = m_uid_status_map.begin(); it != m_uid_status_map.end(); it++)
	{
		if(it->second.user_online_status != status_type) continue;

		user_list->push_back(it->second.uid);
	}

	return (int)user_list->size();
}

void UserCacheManager::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid)
	{
		return;
	}
	std::random_shuffle(m_all_user_list.begin(), m_all_user_list.end());
}

bool UserCacheManager::InitOriginAllUserCache(UniqueServerID unique_sid, long long next_id_from /* = -1 */)
{
	const DBHandler* db_handler = InternalComm::Instance().GetHiddenDBHandler(unique_sid);
	if (NULL == db_handler)
	{
		gamelog::g_log_world.printf(LL_CRITICAL, "World::InitOriginAllUserCache db_handler is NULL !!! plat_type: %d, server_id: %d",
			unique_sid.plat_type, unique_sid.server_id);
		return false;
	}

	const rmi::Session session = db_handler->GetRoleDB();
	if (NULL == session.handle)
	{
		gamelog::g_log_world.printf(LL_CRITICAL, "World::InitOriginAllUserCache session.handle is NULL !!! plat_type: %d, server_id: %d",
			unique_sid.plat_type, unique_sid.server_id);
		return false;
	}

	RMIInitOriginUserCacheBackObjectImpl *back_obj = new RMIInitOriginUserCacheBackObjectImpl();
	back_obj->usid = unique_sid;

	RMIGlobalClient gc;
	gc.__bind_session(session);
	bool ret = gc.InitOriginUserCacheAsyn(next_id_from, back_obj);
	if (!ret)
	{
		gamelog::g_log_world.printf(LL_MAINTANCE, "UserCacheManager::InitOriginAllUserCache total node:%d fail", (int)m_user_node_map.size());
		return false;
	}

	return true;
}

void RMIInitOriginUserCacheBackObjectImpl::InitOriginUserCacheRet(long long from_id, int count, UserCacheParam user_cache_node[MAX_INIT_USERCACHE_COUNT])
{
	if (count <= 0)
	{
		return;
	}

	for (int i = 0; i < count && i < MAX_INIT_USERCACHE_COUNT; ++i)
	{
		UserCacheNode user_node;
		user_node.appearance = user_cache_node[i].appearance;
		user_node.avatar_timestamp = user_cache_node[i].avatar_timestamp;
		user_node.capability = user_cache_node[i].capability;
		user_node.avatar_type = user_cache_node[i].avatar_type;
		user_node.last_online_timestamp = user_cache_node[i].last_online_timestamp;
		user_node.level = user_cache_node[i].level;
		user_node.top_level = user_cache_node[i].top_level;
		user_node.next_top_level_refresh_timestamp = user_cache_node[i].next_top_level_refresh_timestamp;
		user_node.profession = user_cache_node[i].profession;
		SNPRINTF(user_node.role_name, sizeof(GameName), "%s_s%d", user_cache_node[i].role_name, IntToUid(user_cache_node->uid).db_index);
		user_node.uid = user_cache_node[i].uid;

		UserCacheManager::Instance().UpdateCacheNode(user_node);
		UserCacheManager::Instance().UpdateUserOnlineStatus(user_node.uid, UserStatusNode::STATUS_TYPE_OFFLINE);
		UserCacheManager::Instance().UpdateUserNameCache(user_node.role_name, user_node.uid);
	}

	if (CAN_CREAT_AUTO_ROBOT <= count)
	{
		World::GetInstWorld()->SetCreatAutoRobotServerID(usid, 1);
	}

	UserCacheManager::Instance().InitOriginAllUserCache(usid, from_id);
}

void RMIRoleMuteBackObjectImpl::RoleMuteRet(int ret, unsigned int forbid_time, int forbid_talk_type)
{
	UserCacheNode * user_node = UserCacheManager::Instance().GetUserNode(role_id);
	if (NULL == user_node)
	{
		gamelog::g_log_forbid.printf(LL_INFO, "[%s][%d] forbid_time:%u forbid_talk_type:%d reason:Command last_msg:Command", __FUNCTION__,
			role_id, forbid_time, forbid_talk_type);
		return;
	}

	gamelog::g_log_forbid.printf(LL_INFO, "[%s][%d][%s] forbid_time:%u forbid_talk_type:%d reason:Command last_msg:Command", __FUNCTION__,
		role_id, (const char*)user_node->GetNamePtr(), forbid_time, forbid_talk_type);

	ROLE_LOG_QUICK16(IntToUid(role_id).db_index, LOG_TYPE_FORBID_TALK, role_id, user_node->GetNamePtr(), NULL, forbid_time, forbid_talk_type, "Command", __FUNCTION__,
		0, 0, 0, 0, 0);
}

void RMIRoleCmdSetAuthorityBackObjectImpl::RoleCmdSetAuthorityRet(int ret, int authority_type)
{
	UserCacheNode * user_node = UserCacheManager::Instance().GetUserNode(role_id);
	if (NULL == user_node)
	{
		ROLE_LOG_QUICK16(IntToUid(role_id).db_index, LOG_TYPE_CHANGE_AUTHORITY_TYPE, role_id, NULL, NULL, authority_type, 0, "Command", __FUNCTION__,
			0, 0, 0, 0, 0);
		return;
	}

	ROLE_LOG_QUICK16(IntToUid(role_id).db_index, LOG_TYPE_CHANGE_AUTHORITY_TYPE, role_id, user_node->GetNamePtr(), NULL, authority_type, 0, "Command", __FUNCTION__,
		0, 0, 0, 0, 0);
}

void UserCacheManager::SyncCacheToHidden(int uid, bool is_new_node, GameName old_name /* = NULL */)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	UserCacheNode *node = this->GetUserNode(uid);
	if (NULL == node)
	{
		return;
	}

	int db_index = IntToUid(uid).db_index;
	crossgameprotocal::GameHiddenUserCacheSync sync;
	sync.is_name_changed = 0;
	sync.is_new_node = is_new_node ? 1 : 0;
	sync.online_status = this->GetUserOnlineStatus(uid);
	memset(sync.old_name, 0, sizeof(sync.old_name));
	if (NULL != old_name)
	{
		sync.is_name_changed = 1;
		SNPRINTF(sync.old_name, sizeof(GameName), "%s_s%d", old_name, db_index);
	}
	sync.usercache_param.appearance = node->appearance;
	sync.usercache_param.avatar_timestamp = node->avatar_timestamp;
	sync.usercache_param.avatar_type = node->avatar_type;
	sync.usercache_param.last_online_timestamp = node->last_online_timestamp;
	sync.usercache_param.level = node->level;
	sync.usercache_param.top_level = node->top_level;
	sync.usercache_param.next_top_level_refresh_timestamp = node->next_top_level_refresh_timestamp;
	sync.usercache_param.profession = node->profession;
	sync.usercache_param.uid = node->uid;
	sync.usercache_param.capability = node->capability;
	sync.usercache_param.headshot_id = node->appearance.headshot_id;
	SNPRINTF(sync.usercache_param.role_name, sizeof(GameName), "%s_s%d", node->role_name, db_index);

	InternalComm::Instance().SendToHiddenThroughCross((char *)&sync, sizeof(sync));
}

void UserCacheManager::OnSyncGameCache(crossgameprotocal::GameHiddenUserCacheSync *sync)
{
	int uid = sync->usercache_param.uid;
	if (sync->is_new_node)
	{
		m_user_node_map.insert(std::map<int, UserCacheNode>::value_type(sync->usercache_param.uid, UserCacheNode()));
	}

	UserCacheNode *node = this->GetUserNode(uid);
	if (NULL == node)
	{
		return;
	}

	if (sync->is_name_changed > 0)
	{
		m_name_usercache_map.erase(sync->old_name);
	}

	node->appearance = sync->usercache_param.appearance;
	node->avatar_timestamp = sync->usercache_param.avatar_timestamp;
	node->avatar_type = sync->usercache_param.avatar_type;
	node->last_online_timestamp = sync->usercache_param.last_online_timestamp;
	node->level = sync->usercache_param.level;
	node->top_level = sync->usercache_param.top_level;
	node->next_top_level_refresh_timestamp = sync->usercache_param.next_top_level_refresh_timestamp;
	node->profession = sync->usercache_param.profession;
	node->uid = sync->usercache_param.uid;
	node->capability = sync->usercache_param.capability;
	STRNCPY(node->role_name, sync->usercache_param.role_name, sizeof(GameName));

	//UserCacheManager::Instance().UpdateCacheNode(user_node);
	UserCacheManager::Instance().UpdateUserOnlineStatus(uid, sync->online_status);
	UserCacheManager::Instance().UpdateUserNameCache(node->role_name, uid);
}

void UserCacheManager::SynCacheToGame(int uid)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}
	UserCacheNode *node = this->GetUserNode(uid);
	if (NULL == node)
	{
		return;
	}
	crossgameprotocal::HiddenGameUserCacheSync sync;
	sync.online_status = this->GetUserOnlineStatus(uid);
	sync.usercache_param.appearance = node->appearance;
	sync.usercache_param.avatar_timestamp = node->avatar_timestamp;
	sync.usercache_param.avatar_type = node->avatar_type;
	sync.usercache_param.last_online_timestamp = node->last_online_timestamp;
	sync.usercache_param.level = node->level;
	sync.usercache_param.top_level = node->top_level;
	sync.usercache_param.next_top_level_refresh_timestamp = node->next_top_level_refresh_timestamp;
	sync.usercache_param.profession = node->profession;
	sync.usercache_param.uid = node->uid;
	sync.usercache_param.capability = node->capability;
	sync.usercache_param.headshot_id = node->appearance.headshot_id;
	//请不要同步跨服名字回去,跨服中无法改名

	InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(node->uid).db_index,(char *)&sync, sizeof(sync));
}

void UserCacheManager::OnSynHiddenCache(crossgameprotocal::HiddenGameUserCacheSync * sync)
{
	if (CrossConfig::Instance().IsHiddenServer() || NULL == sync)
	{
		return;
	}
	UserCacheNode *node = this->GetUserNode(sync->usercache_param.uid);
	if (NULL == node)
	{
		return;
	}
	node->appearance = sync->usercache_param.appearance;
	node->avatar_timestamp = sync->usercache_param.avatar_timestamp;
	node->avatar_type = sync->usercache_param.avatar_type;
	node->last_online_timestamp = sync->usercache_param.last_online_timestamp;
	node->level = sync->usercache_param.level;
	node->top_level = sync->usercache_param.top_level;
	node->next_top_level_refresh_timestamp = sync->usercache_param.next_top_level_refresh_timestamp;
	node->profession = sync->usercache_param.profession;
	node->uid = sync->usercache_param.uid;
	node->capability = sync->usercache_param.capability;
	//请不要同步跨服名字回去,跨服中无法改名

	UserCacheManager::Instance().UpdateUserOnlineStatus(node->uid, sync->online_status);
	UserCacheManager::Instance().UpdateUserNameCache(node->role_name, node->uid);
}

void UserCacheManager::SyncOnlineStatus(int uid, int online_status)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		// 同步玩家上下线消息到其他原服
		crossgameprotocal::HiddenGameOtherServerRoleOnlineStatusSync sync;
		sync.role_id = uid;
		sync.online_status = online_status;

		InternalComm::Instance().SendToAllGameThroughCross(&sync, sizeof(sync));
	}
	else
	{
		// 同步玩家上下线消息
		crossgameprotocal::GameHiddenOnlineStatusSync sync;
		sync.role_id = uid;
		sync.online_status = online_status;

		InternalComm::Instance().SendToHiddenThroughCross((char *)&sync, sizeof(sync));
	}
}

void UserCacheManager::OnSyncOnlineStatus(int uid, int online_status)
{
	if (CrossConfig::Instance().IsHiddenServer() || LocalConfig::Instance().IsAllowedServerId(IntToUid(uid).db_index))
	{
		std::map<int, UserStatusNode>::iterator it = m_uid_status_map.find(uid);
		if (it == m_uid_status_map.end())
		{
			UserStatusNode node;
			node.uid = uid;
			node.user_online_status = online_status;
			m_uid_status_map.insert(std::map<int, UserStatusNode>::value_type(uid, node));
			const UserStatusNode* node_ptr = &m_uid_status_map[uid];
			m_all_user_list.push_back(node_ptr);
		}
		else
		{
			it->second.user_online_status = online_status;
		}
	}
	else
	{
		std::map<int, UserStatusNode>::iterator it = m_other_server_uid_status_map.find(uid);
		if (it == m_other_server_uid_status_map.end())
		{
			UserStatusNode node;
			node.uid = uid;
			node.user_online_status = online_status;
			m_other_server_uid_status_map.insert(std::map<int, UserStatusNode>::value_type(uid, node));
		}
		else
		{
			it->second.user_online_status = online_status;
		}
	}
}

bool UserCacheManager::SendRoleSimpleInfo(Role * role, int role_id)
{
	if(NULL == role) return false;

	UserCacheNode * user_cache_node = this->GetUserNode(role_id);
	if(NULL == user_cache_node) return false;

	Protocol::SCQueryRoleSimpleInfo info;
	info.role_id = user_cache_node->uid;
	user_cache_node->GetName(info.role_name);
	info.level = user_cache_node->level;
	info.top_level = user_cache_node->top_level;
	info.capability = user_cache_node->capability;
	info.appearance_info = user_cache_node->appearance;

	role->GetRoleModuleManager()->NetSend(&info, sizeof(info));
	return true;
}

bool UserCacheManager::SendRoleSimpleInfoToHidden(int send_uid, int info_uid)
{
	UserCacheNode * user_cache_node = this->GetUserNode(info_uid);
	if (NULL == user_cache_node) return false;

	Protocol::SCQueryRoleSimpleInfo info;
	info.role_id = user_cache_node->uid;
	user_cache_node->GetName(info.role_name);
	info.level = user_cache_node->level;
	info.top_level = user_cache_node->top_level;
	info.capability = user_cache_node->capability;
	info.appearance_info = user_cache_node->appearance;

	InternalComm::Instance().SendToHiddenUserThroughCross(send_uid, (const char*)&info, sizeof(info));
	return true;
}

void UserCacheManager::GmReadAllRoleData(Role * role)
{
	if(NULL == role || CrossConfig::Instance().IsHiddenServer()) return;
	SceneManager * scene_manager = World::GetInstWorld()->GetSceneManager();
	for (std::map<int, UserCacheNode>::iterator it = m_user_node_map.begin(); it != m_user_node_map.end(); it++)
	{
		if(role->GetUID() == it->second.uid) continue;
		if(NULL != scene_manager->GetRole(it->second.uid)) continue;

		RMIRoleReadDataObjectImpl *rrrdoi = new RMIRoleReadDataObjectImpl();
		RMIRoleClient rc;
		bool ret = false;
		rc.__bind_session(InternalComm::Instance().GetDBHandler().GetRoleDB());
		ret = rc.RoleReadDataAsyn(it->second.uid, rrrdoi);
	}
}

void UserCacheManager::SyncGameHiddenCanCreatRobotInfo()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::GameHiddenCreatAutoRobotInfo info;
	info.mother_server_usid = LocalConfig::Instance().GetUniqueServerID();
	info.can_creat_static_robot = m_can_creat_static_robot;

	InternalComm::Instance().SendToHiddenThroughCross((char *)&info, sizeof(info));
}

void UserCacheManager::PrintUserStatus()
{
	std::map<int, UserCacheNode>::iterator it = m_user_node_map.begin();
	for (; it != m_user_node_map.end(); ++it)
	{
		gamelog::g_log_world.printf(LL_INFO, "PrintUserStatus user[%d %s] status[%d]", it->second.uid, it->second.role_name,
			this->GetUserOnlineStatus(it->second.uid));
	}	
}

void UserCacheManager::OnDisconnectOriginServer(const UniqueServerID& usid)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	const UniqueServerID* mother_usid = World::GetInstWorld()->GetMotherServerId(usid);
	if (NULL == mother_usid) return;

	std::map<int, UserStatusNode>::iterator it = m_uid_status_map.begin();
	for (; it != m_uid_status_map.end(); ++it)
	{
		UniqueServerID user_usid(LocalConfig::Instance().GetPlatType(), IntToUid(it->first).db_index);
		const UniqueServerID* user_mother_usid = World::GetInstWorld()->GetMotherServerId(user_usid);
		if (NULL == user_mother_usid) continue;

		if (*user_mother_usid == *mother_usid)
		{
			it->second.user_online_status = UserStatusNode::STATUS_TYPE_OFFLINE; // 原服断线，跨服中的角色状态需设置为不在线
		}
	}
}

void RMIRoleReadDataObjectImpl::__timeout()
{
	printf("%s faild\n", __FUNCTION__);
}

void RMIRoleReadDataObjectImpl::__exception(int error)
{
	printf("%s error[%d]\n", __FUNCTION__, error);
}

void RMIRoleReadDataObjectImpl::RoleReadDataRet(int ret, const RoleInitParam &p,
	const RoleOtherInitParam &other_p,
	const ItemListParam& item_list_param,
	const SkillListParam& skill_list_param,
	const MailListParam& maillist_param,
	const RoleFirstKillParamList& first_kill_list,
	const RandActivityRoleParamList & rand_activity_role_list,
	const  RoleCommonSaveDataParam & role_common_save_list,
	const RoleCourageChallengeLevelParamList & role_courage_challenge_list,
	const GodPrintItemListParam & god_print_item_list,
	const NewCourageChallengeLevelParamList & new_courage_challenge_list,
	long long last_save_time)
{
	if (0 != ret)
	{
		printf("%s faild ret[%d]", __FUNCTION__, ret);
	}
	if(CrossConfig::Instance().IsHiddenServer()) return;

	SceneManager * scene_manager = World::GetInstWorld()->GetSceneManager();
	if (NULL != scene_manager->GetRole(p.role_id)) return;

	Role *role = new Role(GSNetID(), p.role_name);
	role->Init(p.plat_spid, NULL, p.role_id, NULL, p, other_p, item_list_param, skill_list_param,  maillist_param, first_kill_list, rand_activity_role_list, 
		role_common_save_list, role_courage_challenge_list, god_print_item_list, new_courage_challenge_list);
	Scene * scene = scene_manager->GetSceneById(108, COMMON_SCENE_KEY);		//默认使用玄都
	if (NULL == scene)
	{
		delete role;
		return;
	}

	scene->AddObj(role);
	role->GetRoleModuleManager()->ReCalcAttr(AttributeList::RECALC_REASON_INVALID, __FUNCTION__, true);

	RMIRoleSaveDataObjectImpl * rrsdoi = new RMIRoleSaveDataObjectImpl();
	rrsdoi->uid = p.role_id;

	RMIRoleClient rc;
	rc.__bind_session(InternalComm::Instance().GetDBHandler().GetRoleDB());
	rc.RoleSaveDataAsyn(p.role_id, role->GetRoleModuleManager()->GetCommonData(), rrsdoi);

	scene->DeleteObj(role->GetId());
}

void RMIRoleSaveDataObjectImpl::RoleSaveDataRet(int ret)
{
	if (0 != ret)
	{
		gamelog::g_log_save_error.printf(LL_ERROR, "%s  uid[%d] reason[%d] ret[%d]", __FUNCTION__, uid, GAME_ERROR_EXECUTE, ret);
	}
}
