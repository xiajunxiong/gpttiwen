#ifndef __MSG_FRIEND_HPP__
#define __MSG_FRIEND_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"
#include "servercommon/frienddef.hpp"
#include "servercommon/struct/friendintimacyparam.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class CSFriendReq					//!< 2070
	{
	public:
		enum FRIEND_REQ_TYPE
		{
			FRIEND_REQ_TYPE_INFO			= 0,	//!< 请求信息
			FRIEND_REQ_TYPE_ADD				= 1,	//!< 添加好友				p1 roleid
			FRIEND_REQ_TYPE_ADD_REPLY		= 2,	//!< 回复添加好友请求			p1 role_id , p2 是否接受
			FRIEND_REQ_TYPE_DEL				= 3,	//!< 删除好友				p1 role_id
			FRIEND_REQ_TYPE_ADD_BLACK		= 4,	//!< 添加到黑名单				p1 role_id
			FRIEND_REQ_TYPE_DEL_BLACK		= 5,	//!< 删除黑名单				p1 role_id
			FRIEND_REQ_DEL_GROUP			= 6,	//!< 删除分组				p1 group_type（1是自定义组1，2是自定义组2）
			FRIEND_REQ_TYPE_ASK_LIST		= 7,	//!< 添加好友请求列表
			FRIEND_REQ_ADD_RECENTLY_CHAT	= 8,	//!< 添加最近联系人			p1 role_id
			FRIEND_REQ_RECENTLY_CHAT_INFO	= 9,	//!< 请求最近联系人 信息
			FRIEND_REQ_RECENTLY_TEAM_INFO	= 10,	//!< 添加最近组队 信息
			FRIEND_REQ_GIVE_FLOWER			= 11,	//!< 给玩家送花				p1 role_id , p2 seq
			FRIEND_REQ_CHECK_IS_ONLINE		= 12,	//!< 查询玩家是否在线			p1 role_id
		}; 

		CSFriendReq();
		MessageHeader		header;

		int req_type;
		int param1;
		int param2;
	};

	class SCFriendInfo		//!< 2071
	{
	public:
		SCFriendInfo();
		MessageHeader	header;

		GameName		group_name[MAX_FRIEND_GROUP_NUM];   //!<  自定义组 的名字,大小2
		int				count;
		FriendInfo		friend_list[MAX_FRIEND_NUM];		//!< 好友信息，大小100
	};

	class SCChangeFriend		//!< 2072
	{
	public:
		enum CHANGE_TYPE
		{
			CHANGE_TYPE_UPDATE,			//!<  好友更新
			CHANGE_TYPE_DEL,				 //!<  好友删除
		};

		SCChangeFriend();
		MessageHeader		header;

		int change_type;
		FriendInfo info;
	};

	struct AskAddInfo
	{
		GameName			req_gamename;
		int 				req_user_id;
		short				req_avatar_type;
		short				req_prof;
		int					req_level;
		int					headshot_id;
		int					top_level;
		long long			req_avatar_timestamp;
	};

	class SCAddFriendRoute					//!< 2073
	{
	public:
		SCAddFriendRoute();
		MessageHeader		header;

		AskAddInfo ask_add_info;
	};

	class SCAskAddFriendList					//!< 2074 添加好友请求列表
	{
	public:
		SCAskAddFriendList();
		MessageHeader		header;

		int ask_list_num;
		AskAddInfo ask_add_list[MAX_ASK_ADD_FRIEND_NUM];
	};

	struct RoleItemInfo
	{
		int					user_id;
		GameName			gamename;
		GameName			family_name;

		short				avatar_type;
		char				is_online;
		char				vip_level;
		short				level;
		short				prof;
		short				headshot_id;
		short				reserve_sh;
		int					top_level;
		long long			avatar_timestamp;
		unsigned int		last_online_timestamp;
	};

	class SCChangeBlack		//!< 2075
	{
	public:
		enum CHANGE_TYPE
		{
			CHANGE_TYPE_BLACK_UPDATE,	 //!< 黑名单更新
			CHANGE_TYPE_BLACK_DEL,	//!<  黑名单删除
		};

		SCChangeBlack();
		MessageHeader		header;

		int change_type;
		RoleItemInfo info;
	};

	class SCBlackInfo		//!< 2076
	{
	public:
		SCBlackInfo();
		MessageHeader		header;

		int				count;
		RoleItemInfo			black_list[MAX_BLACK_NUM];		//!< 黑名单信息，大小100
	};

	class CSFriendEditGruop		//!< 2077
	{
	public:
		CSFriendEditGruop();
		MessageHeader		header;

		int group_type;
		GameName group_name;
		int role_id[MAX_FRIEND_NUM];		// 移动的好友角色id，不用补齐100
	};

	class CSFriendMoveGruop		//!< 2078
	{
	public:
		CSFriendMoveGruop();
		MessageHeader		header;

		int group_type;					// 移动到哪个组
		int role_id[MAX_FRIEND_NUM];		// 移动的好友角色id，不用补齐100
	};

	class CSFriendRenameGruop		//!< 2079
	{
	public:
		CSFriendRenameGruop();
		MessageHeader		header;

		int group_type;
		GameName group_name;
	};

	class SCFriendGroupInfo		//!< 2080
	{
	public:
		SCFriendGroupInfo();
		MessageHeader		header;

		GameName		group_name[MAX_FRIEND_GROUP_NUM];
	};

	class SCRecentlyTeamInfo		//!< 2081
	{
	public:
		SCRecentlyTeamInfo();
		MessageHeader		header;

		int recently_team_role_count;
		RoleItemInfo recently_team_role[MAX_RECENTLY_TEAM_NUM];  //!<大小10
	};

	class SCOneRecentlyTeamInfo		//!< 2082
	{
	public:
		SCOneRecentlyTeamInfo();
		MessageHeader		header;

		int del_role_id;
		RoleItemInfo recently_team_role;
	};

	class SCRecentlyChatChange		//!< 2083
	{
	public:
		SCRecentlyChatChange();
		MessageHeader		header;

		int del_role_id;
		RoleItemInfo recently_chat_role;
	};

	class SCRecentlyChatInfo	//!< 2084
	{
	public:
		SCRecentlyChatInfo();
		MessageHeader		header;

		int recently_chat_count;
		RoleItemInfo recently_chat_role[MAX_RECENTLY_CHAT_NUM];
	};

	class SCReplyRoleIsOnline		//!< 2085 - 回复玩家是否在线
	{
	public:
		SCReplyRoleIsOnline();
		MessageHeader header;

		int role_id;			//!< 角色ID
		int online_status;		//!< 在线状态： 0 - 离线； 1 - 在线；
	};

	//--------------------------------------好友推荐----------------------------------------------
	class CSFriendRecommendReq		//!< 2090  请求推荐好友列表	
	{
	public:
		CSFriendRecommendReq();
		MessageHeader		header;
	};

	class SCFriendRecommendRoute	//   //!< 2091 发送推荐好友列表
	{
	public:
		SCFriendRecommendRoute();
		MessageHeader		header;

		RecommendFriend friend_recommend_list[FriendRecommendParam::RECOMMEND_LIST_MAX]; 
	};

	class CSFriendRecommendAddOnListReq	  //!< 2092 添加所有好友请求
	{
	public:
		CSFriendRecommendAddOnListReq();
		MessageHeader		header;

		short is_add_all;			//!< 1是添加所有 ； 0是添加一个，下面add_index起作用
		short add_index;
	};

	class SCFriendRecommendOne			 //!< 2983 下发一个好友推荐人
	{
	public:
		SCFriendRecommendOne();
		MessageHeader		header;

		RecommendFriend friend_recommend;
	};

	class CSRefreshFriendRecommend		//!< 2094  推荐列表换一批	
	{
	public:
		CSRefreshFriendRecommend();
		MessageHeader		header;
	};

	class CSFuzzySearchRoleByName //!< 2095
	{
	public:
		CSFuzzySearchRoleByName();
		MessageHeader header;

		GameName name;
	};

	struct FuzzySearchRoleInfoRet
	{
		int role_id;
		GameName role_name;
		short avatar_type;	
		short prof;
		int headshot_id;
		int level;
		int top_level;
		long long avatar_timestamp;
	};

	class SCFuzzySearchRolesInfo						//!< 2096
	{
	public:
		SCFuzzySearchRolesInfo();
		MessageHeader header;

		int count;
		FuzzySearchRoleInfoRet role_info[30];
	};

	class SCGiveFlowerNotice							//!< 2097 送花通知,只有有特效的才会有该协议下发来通知范围内所有玩家
	{
	public:
		SCGiveFlowerNotice();
		MessageHeader header;

		int giver_id;									// 赠送人ID
		GameName giver_name;							// 赠与人的名字
		int receiver_id;								// 接受者ID	
		GameName receiver_name;							// 接受人的名字
		int give_type;									// 送花的类型 [只代表那些有特效的]
	};

	struct FriendIntimacy
	{
		FriendIntimacy():friend_id(0), intimacy_val(0), day_frist_to_singchat_timestamp(0), day_frist_from_singchat_timestamp(0){}

		int friend_id;										// 好友UID
		int intimacy_val;									// 亲密度
		int day_frist_to_singchat_timestamp;				// 每日玩家给好友发送的第一条消息的时间戳
		int day_frist_from_singchat_timestamp;				// 每日好友给玩家发送的第一条消息的时间戳
	};

	class SCFriendIntimacyListInfo							//!< 4180 好友亲密度信息列表
	{
	public:
		SCFriendIntimacyListInfo();
		MessageHeader header;

		int count;											// 表长
		FriendIntimacy  info[MAX_FRIEND_NUM];				// 好友亲密度信息列表
	};

	class SCFriendIntimacyInfoChange						//!< 4181 好友亲密度信息变更
	{
	public:
		SCFriendIntimacyInfoChange();
		MessageHeader header;

		FriendIntimacy  info;								// 好友亲密度信息
	};
}

#pragma pack(pop)

#endif // __MSG_BABY_HPP__

