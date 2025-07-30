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
			FRIEND_REQ_TYPE_INFO			= 0,	//!< ������Ϣ
			FRIEND_REQ_TYPE_ADD				= 1,	//!< ��Ӻ���				p1 roleid
			FRIEND_REQ_TYPE_ADD_REPLY		= 2,	//!< �ظ���Ӻ�������			p1 role_id , p2 �Ƿ����
			FRIEND_REQ_TYPE_DEL				= 3,	//!< ɾ������				p1 role_id
			FRIEND_REQ_TYPE_ADD_BLACK		= 4,	//!< ��ӵ�������				p1 role_id
			FRIEND_REQ_TYPE_DEL_BLACK		= 5,	//!< ɾ��������				p1 role_id
			FRIEND_REQ_DEL_GROUP			= 6,	//!< ɾ������				p1 group_type��1���Զ�����1��2���Զ�����2��
			FRIEND_REQ_TYPE_ASK_LIST		= 7,	//!< ��Ӻ��������б�
			FRIEND_REQ_ADD_RECENTLY_CHAT	= 8,	//!< ��������ϵ��			p1 role_id
			FRIEND_REQ_RECENTLY_CHAT_INFO	= 9,	//!< ���������ϵ�� ��Ϣ
			FRIEND_REQ_RECENTLY_TEAM_INFO	= 10,	//!< ��������� ��Ϣ
			FRIEND_REQ_GIVE_FLOWER			= 11,	//!< ������ͻ�				p1 role_id , p2 seq
			FRIEND_REQ_CHECK_IS_ONLINE		= 12,	//!< ��ѯ����Ƿ�����			p1 role_id
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

		GameName		group_name[MAX_FRIEND_GROUP_NUM];   //!<  �Զ����� ������,��С2
		int				count;
		FriendInfo		friend_list[MAX_FRIEND_NUM];		//!< ������Ϣ����С100
	};

	class SCChangeFriend		//!< 2072
	{
	public:
		enum CHANGE_TYPE
		{
			CHANGE_TYPE_UPDATE,			//!<  ���Ѹ���
			CHANGE_TYPE_DEL,				 //!<  ����ɾ��
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

	class SCAskAddFriendList					//!< 2074 ��Ӻ��������б�
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
			CHANGE_TYPE_BLACK_UPDATE,	 //!< ����������
			CHANGE_TYPE_BLACK_DEL,	//!<  ������ɾ��
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
		RoleItemInfo			black_list[MAX_BLACK_NUM];		//!< ��������Ϣ����С100
	};

	class CSFriendEditGruop		//!< 2077
	{
	public:
		CSFriendEditGruop();
		MessageHeader		header;

		int group_type;
		GameName group_name;
		int role_id[MAX_FRIEND_NUM];		// �ƶ��ĺ��ѽ�ɫid�����ò���100
	};

	class CSFriendMoveGruop		//!< 2078
	{
	public:
		CSFriendMoveGruop();
		MessageHeader		header;

		int group_type;					// �ƶ����ĸ���
		int role_id[MAX_FRIEND_NUM];		// �ƶ��ĺ��ѽ�ɫid�����ò���100
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
		RoleItemInfo recently_team_role[MAX_RECENTLY_TEAM_NUM];  //!<��С10
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

	class SCReplyRoleIsOnline		//!< 2085 - �ظ�����Ƿ�����
	{
	public:
		SCReplyRoleIsOnline();
		MessageHeader header;

		int role_id;			//!< ��ɫID
		int online_status;		//!< ����״̬�� 0 - ���ߣ� 1 - ���ߣ�
	};

	//--------------------------------------�����Ƽ�----------------------------------------------
	class CSFriendRecommendReq		//!< 2090  �����Ƽ������б�	
	{
	public:
		CSFriendRecommendReq();
		MessageHeader		header;
	};

	class SCFriendRecommendRoute	//   //!< 2091 �����Ƽ������б�
	{
	public:
		SCFriendRecommendRoute();
		MessageHeader		header;

		RecommendFriend friend_recommend_list[FriendRecommendParam::RECOMMEND_LIST_MAX]; 
	};

	class CSFriendRecommendAddOnListReq	  //!< 2092 ������к�������
	{
	public:
		CSFriendRecommendAddOnListReq();
		MessageHeader		header;

		short is_add_all;			//!< 1��������� �� 0�����һ��������add_index������
		short add_index;
	};

	class SCFriendRecommendOne			 //!< 2983 �·�һ�������Ƽ���
	{
	public:
		SCFriendRecommendOne();
		MessageHeader		header;

		RecommendFriend friend_recommend;
	};

	class CSRefreshFriendRecommend		//!< 2094  �Ƽ��б�һ��	
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

	class SCGiveFlowerNotice							//!< 2097 �ͻ�֪ͨ,ֻ������Ч�ĲŻ��и�Э���·���֪ͨ��Χ���������
	{
	public:
		SCGiveFlowerNotice();
		MessageHeader header;

		int giver_id;									// ������ID
		GameName giver_name;							// �����˵�����
		int receiver_id;								// ������ID	
		GameName receiver_name;							// �����˵�����
		int give_type;									// �ͻ������� [ֻ������Щ����Ч��]
	};

	struct FriendIntimacy
	{
		FriendIntimacy():friend_id(0), intimacy_val(0), day_frist_to_singchat_timestamp(0), day_frist_from_singchat_timestamp(0){}

		int friend_id;										// ����UID
		int intimacy_val;									// ���ܶ�
		int day_frist_to_singchat_timestamp;				// ÿ����Ҹ����ѷ��͵ĵ�һ����Ϣ��ʱ���
		int day_frist_from_singchat_timestamp;				// ÿ�պ��Ѹ���ҷ��͵ĵ�һ����Ϣ��ʱ���
	};

	class SCFriendIntimacyListInfo							//!< 4180 �������ܶ���Ϣ�б�
	{
	public:
		SCFriendIntimacyListInfo();
		MessageHeader header;

		int count;											// ��
		FriendIntimacy  info[MAX_FRIEND_NUM];				// �������ܶ���Ϣ�б�
	};

	class SCFriendIntimacyInfoChange						//!< 4181 �������ܶ���Ϣ���
	{
	public:
		SCFriendIntimacyInfoChange();
		MessageHeader header;

		FriendIntimacy  info;								// �������ܶ���Ϣ
	};
}

#pragma pack(pop)

#endif // __MSG_BABY_HPP__

