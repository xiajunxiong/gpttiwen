#include "unittest/catch.hpp"
#include "global/usercache/usercache.hpp"
#include "engineadapter.h"
#include "servercommon/serverconfig/localconfig.hpp"

UserCacheNode CreateUserCacheNode(int uid, const char* name)
{
	UserCacheNode node;
	node.uid = uid;
	F_STRNCPY(node.role_name, name, sizeof(node.role_name));
	node.level = RandomNum(15, 120);
	node.avatar_type = 1;
	node.capability = node.level * 100;
	node.profession = 101;
	node.last_online_timestamp = EngineAdapter::Instance().Time();
	node.avatar_timestamp = 0;

	return node;
}

TEST_CASE("Test User Cache And Name Cache", "[UserCache]") {
	
	int server_id = LocalConfig::Instance().GetMergeServerId();

	UserCacheNode node1 = CreateUserCacheNode((server_id  << DB_INDEX_MARK_BIT) + 1, "abced");
	UserCacheNode node2 = CreateUserCacheNode((server_id << DB_INDEX_MARK_BIT) + 2, "agedw");
	UserCacheNode node3 = CreateUserCacheNode((server_id << DB_INDEX_MARK_BIT) + 3, "adeq1");

	UserCacheManager::Instance().UpdateCacheNode(node1);
	UserCacheManager::Instance().UpdateCacheNode(node2);
	UserCacheManager::Instance().UpdateCacheNode(node3);

	UserCacheNode* pnode1 = UserCacheManager::Instance().GetUserNode(node1.uid);
	REQUIRE(NULL != pnode1);
	CHECK(pnode1->uid == node1.uid);

	UserCacheNode* pnode2 = UserCacheManager::Instance().GetUserNode(node2.uid);
	REQUIRE(NULL != pnode2);
	CHECK(pnode2->uid == node2.uid);

	UserCacheNode* pnode3 = UserCacheManager::Instance().GetUserNode(node3.uid);
	REQUIRE(NULL != pnode3);
	CHECK(pnode3->uid == node3.uid);
	
	int count = 0;
	UserCacheNode** nodelist = UserCacheManager::Instance().FuzzySearchUsersByName("ed", &count, 0);
	CHECK(count == 2);

	nodelist = UserCacheManager::Instance().FuzzySearchUsersByName("ed", &count, node1.uid);
	CHECK(count == 1);
	CHECK(nodelist[0]->uid == node2.uid);

	bool is_online = false;
	UserCacheNode* pnode4 = UserCacheManager::Instance().GetUserNodeByName(node1.role_name, &is_online, false);
	REQUIRE(NULL != pnode4);
	CHECK(pnode4->uid == node1.uid);
	CHECK_FALSE(is_online);

	UserCacheManager::Instance().UpdateUserOnlineStatus(node1.uid, UserStatusNode::STATUS_TYPE_ONLINE);
	pnode4 = UserCacheManager::Instance().GetUserNodeByName(node1.role_name, &is_online, false);
	REQUIRE(NULL != pnode4);
	CHECK(pnode4->uid == node1.uid);
	CHECK(is_online);
	CHECK(UserCacheManager::Instance().IsUserOnline(node1.uid));

	UserCacheManager::Instance().UpdateUserOnlineStatus(node1.uid, UserStatusNode::STATUS_TYPE_CROSS);
	pnode4 = UserCacheManager::Instance().GetUserNodeByName(node1.role_name, &is_online, false);
	REQUIRE(NULL != pnode4);
	CHECK(pnode4->uid == node1.uid);
	CHECK(is_online);
	CHECK(UserCacheManager::Instance().IsUserOnline(node1.uid));

	UserCacheManager::Instance().UpdateUserOnlineStatus(node1.uid, UserStatusNode::STATUS_TYPE_OFFLINE);
	pnode4 = UserCacheManager::Instance().GetUserNodeByName(node1.role_name, &is_online, false);
	REQUIRE(NULL != pnode4);
	CHECK(pnode4->uid == node1.uid);
	CHECK_FALSE(is_online);
	CHECK_FALSE(UserCacheManager::Instance().IsUserOnline(node1.uid));

	UserCacheManager::Instance().OnSyncOnlineStatus(node3.uid, UserStatusNode::STATUS_TYPE_ONLINE);
	CHECK(UserCacheManager::Instance().IsUserOnline(node3.uid));

	UserCacheManager::Instance().OnSyncOnlineStatus(node3.uid, UserStatusNode::STATUS_TYPE_OFFLINE);
	CHECK_FALSE(UserCacheManager::Instance().IsUserOnline(node3.uid));

	UserCacheManager::Instance().OnSyncOnlineStatus(node3.uid, UserStatusNode::STATUS_TYPE_CROSS);
	CHECK(UserCacheManager::Instance().IsUserOnline(node3.uid));

	UserCacheManager::Instance().OnSyncOnlineStatus(node3.uid, UserStatusNode::STATUS_TYPE_OFFLINE);
	CHECK_FALSE(UserCacheManager::Instance().IsUserOnline(node3.uid));

	UserCacheManager::Instance().OnResetName(node2.uid, "tony", node2.role_name);
	nodelist = UserCacheManager::Instance().FuzzySearchUsersByName("ed", &count, 0);
	CHECK(count == 1);
	CHECK(nodelist[0]->uid != node2.uid);

	UserCacheNode* pnode5 = UserCacheManager::Instance().GetUserNodeByName(node2.role_name, &is_online, false);
	REQUIRE(NULL == pnode5);
	
	pnode5 = UserCacheManager::Instance().GetUserNodeByName("tony", &is_online, false);
	REQUIRE(NULL != pnode5);
	CHECK(pnode5->uid == node2.uid);
	CHECK(strcmp(pnode5->role_name, "tony") == 0);

	//测试别服玩家status获取
	int other_server_id = 1;
	int other_server_role_id = (other_server_id << DB_INDEX_MARK_BIT) + 1;
	UserCacheManager::Instance().OnSyncOnlineStatus(other_server_role_id, UserStatusNode::STATUS_TYPE_ONLINE);
	CHECK(UserCacheManager::Instance().IsUserOnline(other_server_role_id));
	CHECK(UserStatusNode::STATUS_TYPE_CROSS == UserCacheManager::Instance().GetUserOnlineStatus(other_server_role_id));

	UserCacheManager::Instance().OnSyncOnlineStatus(other_server_role_id, UserStatusNode::STATUS_TYPE_CROSS);
	CHECK(UserCacheManager::Instance().IsUserOnline(other_server_role_id));
	CHECK(UserStatusNode::STATUS_TYPE_CROSS == UserCacheManager::Instance().GetUserOnlineStatus(other_server_role_id));

	UserCacheManager::Instance().OnSyncOnlineStatus(other_server_role_id, UserStatusNode::STATUS_TYPE_OFFLINE);
	CHECK_FALSE(UserCacheManager::Instance().IsUserOnline(other_server_role_id));
	CHECK(UserStatusNode::STATUS_TYPE_OFFLINE == UserCacheManager::Instance().GetUserOnlineStatus(other_server_role_id));
}