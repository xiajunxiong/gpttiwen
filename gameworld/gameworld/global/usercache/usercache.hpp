#ifndef _USERCACHE_HPP_
#define _USERCACHE_HPP_

#include "servercommon/servercommon.h"
#include "servercommon/serverdef.h"
#include "servercommon/serverconfig/configstruct.h"
#include "servercommon/mysqlutility.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "servercommon/commondata.hpp"

#include <map>
#include <vector>

struct UserCacheNode
{
	UserCacheNode() : level(0), avatar_type(0), capability(0), uid(0), profession(0), top_level(0), next_top_level_refresh_timestamp(0),
		last_online_timestamp(0u), avatar_timestamp(0LL)
	{
		memset(role_name, 0, sizeof(role_name));
	}

	void GetName(GameName & name) const
	{
		if (NULL != name)
		{
			F_STRNCPY(name, role_name, sizeof(GameName));
		}
	}

	const char * GetNamePtr() const
	{
		return role_name;
	}
	
	signed char GetSex()
	{
		return SEX_ARRAY[(int)avatar_type];
	}

	int GetBaseProfession()
	{
		return profession / PROFESSION_BASE * PROFESSION_BASE;
	}

	int GetProfession()
	{
		return profession;
	}

	int GetCapability()
	{
		return capability;
	}

	inline int GetLevel() const
	{
		return level;
	}

	short level;
	short avatar_type;				// 虚拟人物类型
	int capability;					// 战斗力
	int uid;
	int profession;
	int top_level;					// 巅峰等级
	unsigned int next_top_level_refresh_timestamp;
	unsigned int last_online_timestamp;
	GameName role_name;
	long long avatar_timestamp;		// 用户自上传的头像
	Appearance appearance;
};

struct UserStatusNode
{
	UserStatusNode() : uid(0), user_online_status(STATUS_TYPE_BOTH){}

	int uid;
	int user_online_status;

	enum ONLINE_STATUS_TYPE
	{
		STATUS_TYPE_BOTH = -1,
		STATUS_TYPE_OFFLINE = 0,
		STATUS_TYPE_ONLINE = 1,
		STATUS_TYPE_CROSS = 2,
	};
};

class Role;
typedef bool (UserCacheFilterFunc)(const UserCacheNode &user_node, void **func_param);
class UserCacheManager
{
public:
	static UserCacheManager & Instance();

	bool LoadAllUser(TMysqlStatement &stMysqlStatement);

	UserCacheNode * GetUserNode(const UserID &user_id, bool trace_nonexist = true);
	UserCacheNode * GetUserNode(int uid, bool trace_nonexist = true);
	UserCacheNode * GetUserNodeByName(std::string user_name, bool *is_online, bool trace_nonexist = true);
	UserCacheNode ** FuzzySearchUsersByName(std::string search, int* count, int exclude_role_id);
	int GetUserTopLevel(const UserID &user_id);
	int GetUserTopLevel(int uid);
	void UpdateCacheNode(UserCacheNode &node, bool do_not_sync = false);
	void OnCacheNodeUpdated(UserCacheNode& node);
	void OnConnectCrossServerSucc();
	void OnlineRoleNoticeHidden();
	void OnTopLevelReset(unsigned int next_refresh_timestamp);
	void OnTopLevelLog(unsigned int start_timestamp, unsigned int end_timestamp);

	void UpdateUserOnlineStatus(int uid, int online_status);
	void UpdateUserNameCache(const std::string& user_name, int uid);
	void UpdateUserNameCache(Role *role);
	bool IsUserOnline(int uid);
	bool IsUserOnline(UserID user_id);
	int GetUserOnlineStatus(int uid);
	int GetUserOnlineStatus(UserID user_id);

	int CountOfUser();
	inline int GetHighestRoleLevel() const { return m_highest_role_level; }

	void OnResetName(int role_id, GameName new_name, GameName old_name);
	int GetUserIDList(int &from, int user_count, UserID *user_list, UserCacheFilterFunc filter_func = NULL, void **func_param = NULL);
	int GetUidListByStatusMap(int &from, int user_count, int *user_list, int status_type, Role* role);
	int GetUidListByStatusMap(std::vector<int> *user_list, int status_type);

	const std::map<int, UserCacheNode> * GetUserNodeMap() const { return &m_user_node_map; }
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	bool InitOriginAllUserCache(UniqueServerID unique_sid, long long next_id_from = -1);

	void SyncCacheToHidden(int uid, bool is_new_node, GameName old_name = NULL);		
	void OnSyncGameCache(crossgameprotocal::GameHiddenUserCacheSync *sync);

	void SynCacheToGame(int uid);
	void OnSynHiddenCache(crossgameprotocal::HiddenGameUserCacheSync *sync);

	void SyncOnlineStatus(int uid, int online_status);
	void OnSyncOnlineStatus(int uid, int online_status);

	bool SendRoleSimpleInfo(Role * role, int role_id);
	bool SendRoleSimpleInfoToHidden(int send_uid, int info_uid);

	void GmReadAllRoleData(Role *role);
	
	void SyncGameHiddenCanCreatRobotInfo();

	void PrintUserStatus();
	void OnDisconnectOriginServer(const UniqueServerID& usid);
private:
	UserCacheManager();
	~UserCacheManager();

	std::map<int, UserCacheNode> m_user_node_map;
	std::map<std::string, UserCacheNode*> m_name_usercache_map;
	std::map<int, UserStatusNode> m_uid_status_map;
	std::vector<const UserStatusNode*> m_all_user_list;//所有用户列表，元素指向m_uid_status_map，如对map元素进行删除， 应同时删除list中的元素，或赋值为NULL

	std::map<int, UserStatusNode> m_other_server_uid_status_map; // 记录别服的玩家是否在线（不会时刻同步，只在玩家登录或下线时同步一次，所以不是绝对正确的，仅用于不强求正确性的情况）

	int m_highest_role_level;	// 最高等级
	//int m_creat_role_num;		// 服务器缓存人数(非跨服)
	int m_can_creat_static_robot;// 可创建机器人 缓存超过20人(写死) 可在跨服创对应的静态机器人
};

#endif

