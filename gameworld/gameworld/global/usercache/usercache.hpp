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
	short avatar_type;				// ������������
	int capability;					// ս����
	int uid;
	int profession;
	int top_level;					// �۷�ȼ�
	unsigned int next_top_level_refresh_timestamp;
	unsigned int last_online_timestamp;
	GameName role_name;
	long long avatar_timestamp;		// �û����ϴ���ͷ��
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
	std::vector<const UserStatusNode*> m_all_user_list;//�����û��б�Ԫ��ָ��m_uid_status_map�����mapԪ�ؽ���ɾ���� Ӧͬʱɾ��list�е�Ԫ�أ���ֵΪNULL

	std::map<int, UserStatusNode> m_other_server_uid_status_map; // ��¼���������Ƿ����ߣ�����ʱ��ͬ����ֻ����ҵ�¼������ʱͬ��һ�Σ����Բ��Ǿ�����ȷ�ģ������ڲ�ǿ����ȷ�Ե������

	int m_highest_role_level;	// ��ߵȼ�
	//int m_creat_role_num;		// ��������������(�ǿ��)
	int m_can_creat_static_robot;// �ɴ��������� ���泬��20��(д��) ���ڿ������Ӧ�ľ�̬������
};

#endif

