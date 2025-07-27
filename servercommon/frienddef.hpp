#ifndef __FRIENDDEF_H__
#define __FRIENDDEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"

static const int MAX_ONCE_FRIENDS_COUNT = 1000; // 一次性操作最大好友数量
static const int MAX_ONCE_BLACK_COUNT = 1000;	// 一次性操作最大黑名单数量
static const int MAX_FRIEND_NUM = 200;			// 最大好友数量
static const int MAX_FRIEND_GROUP_NUM = 2;		// 最大自定义组
static const int MAX_BLACK_NUM = 100;			// 最大黑名单数量
static const int MAX_ASK_ADD_FRIEND_NUM = 6;	// 最大好友请求数量
static const int MAX_RECENTLY_TEAM_NUM = 10;	// 最近组队角色 数量
static const int MAX_RECENTLY_CHAT_NUM = 10;	// 最近联系人 数量
static const int MAX_REFRESH_TIME_NUM = 60 * 10;// 推荐好友刷新间隔10分钟

#pragma pack(push, 4)

struct FriendInfo
{
	int					user_id;
	GameName			role_name;
	GuildName			family_name;
	int					intimacy;
	char				sex;
	char				is_online;
	short				prof;
	short				level;
	char				vip_level;
	char				group_type;   //!< 0是好友组，1是自定义组1，2是自定义组2
	long long			avatar_timestamp;
	short				avatar_type;
	short				headshot_id;
	int					top_level;		//巅峰等级
	unsigned int		last_online_timestamp;
};

struct RecommendFriend
{
	RecommendFriend()
	{
		this->Reset();
	};

	void Reset()
	{
		user_uid = 0;
		memset(user_name, 0, sizeof(user_name));
		user_level = 0;
		top_level = 0;
		reserve_ch = 0;
		user_prof = 0;
		user_avatar_type = 0;
		user_headshot_id = 0;
		reserve_sh = 0;
		user_avatar_timestamp = 0;
	}

	int					user_uid;						// 推荐的好友的id
	GameName			user_name;						// 推荐好友的名字
	int					user_level;						// 好友的等级
	int					top_level;						// 巅峰等级
	char				reserve_ch;						// 
	char				user_prof;						// 职业
	short				user_avatar_type;				// 虚拟形象
	short				user_headshot_id;				// 头像
	short				reserve_sh;						
	long long			user_avatar_timestamp;			// 头像时间戳
};

struct FriendRecommendParam
{
	static const int RECOMMEND_LIST_MAX = 6;				// 最大推荐个数

	FriendRecommendParam()
	{
		this->Reset();
	}

	void Reset()
	{
		online_offset = 0;
		all_status_offset = 0;
		memset(recommend_list, 0, sizeof(recommend_list));
	}

	int online_offset;
	int all_status_offset;

	RecommendFriend recommend_list[RECOMMEND_LIST_MAX];
};

typedef char FriendRecommendParamHex[sizeof(FriendRecommendParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(FriendRecommendParamHex) < 2048);


struct FriendCommonParam
{
	FriendCommonParam()
	{
		this->Reset();
	}

	void Reset()
	{
		memset(group_name, 0, sizeof(group_name));
		
		curr_recently_team_index = 0;
		memset(recently_team, 0, sizeof(recently_team));
		is_reset_friend_data_flag = 0;
		reserve_ch = 0;
		reserve_sh = 0;
	}

	GameName		group_name[MAX_FRIEND_GROUP_NUM];   //!<  自定义组 的名字,大小2

	int curr_recently_team_index;
	int recently_team[MAX_RECENTLY_TEAM_NUM];

	int curr_recently_chat_index;						
	int curr_recently_chat[MAX_RECENTLY_CHAT_NUM];			//最近联系人
	char is_reset_friend_data_flag;					// 0:未清除  1:已清除		好友数据添加保存,老号拥有别的服务好友需要清一下
	char reserve_ch;
	short reserve_sh;	

};

typedef char FriendCommonParamHex[sizeof(FriendCommonParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(FriendCommonParamHex) < 512);


#pragma pack(pop)

#endif
