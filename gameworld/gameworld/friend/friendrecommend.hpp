#ifndef __FRIENDRECOMMEND_HPP__
#define __FRIENDRECOMMEND_HPP__

//#include "gameworld/global/usercache/usercache.hpp"		// 不能用，为什么

#include <map>
#include <set>
#include "servercommon/frienddef.hpp"
#include "other/rolemodulemanager.hpp"


enum LAST_USE_TYPE_FLAG
{
	LAST_USE_TYPE_FLAG_BEGIN,		
	LAST_USE_TYPE_FLAG_REFRESH,		// 换一批
	LAST_USE_TYPE_FLAG_IS_ADD_ALL,	// 一键申请(优先级高于换一批)

	LAST_USE_TYPE_FLAG_MAX
};

class Role;
struct UserCacheNode;

class FriendRecommend
{
public:	
	
	static const int TEN_SECOND = 10;
public:
	FriendRecommend();
	~FriendRecommend();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * role_module_manager);							// 初始化
	void GetInitParam(FriendRecommendParam* param);
	bool RefreshFriendRecommendList(bool is_notice = false);		//筛选好友，是否必须在线好友
	void RefreshOneFriendRecommend(int index);						//刷新好友推荐人
	void SendFriendRecommend();										//发送数据到客户端
	bool IsCanRefresh(int type_flag);								//是否可以刷新好友
	void SetNextRefreshTimestamp(int type_flag);					//设置下次刷新好友时间戳
	bool IsLastTimeRefreshUid(int uid);								//判断是否是上次刷新的uid

	void AddAllRecommendFriend();					
	void AddOneRecommendFriend(int index);
	void ClearData();
private:

	RoleModuleManager *	m_role_module_mgr;
	bool m_is_get_recommend_list;

	int m_last_use_type_flag;											// 上一次点击标识  LAST_USE_TYPE_FLAG_MAX
	int m_recommend_uid_count;											// 数组数量
	int m_recommend_uid_list[FriendRecommendParam::RECOMMEND_LIST_MAX];	// 记录玩家上次刷新的uid_list
	FriendRecommendParam 	m_param;									// 推荐列表
	unsigned int m_next_refresh_friend_timestamp;						// 下次刷新推荐好友时间戳
};


#endif	// FRIENDRECOMMEND_HPP