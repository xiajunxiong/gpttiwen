#ifndef __FRIENDDEF_H__
#define __FRIENDDEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"

static const int MAX_ONCE_FRIENDS_COUNT = 1000; // һ���Բ�������������
static const int MAX_ONCE_BLACK_COUNT = 1000;	// һ���Բ���������������
static const int MAX_FRIEND_NUM = 200;			// ����������
static const int MAX_FRIEND_GROUP_NUM = 2;		// ����Զ�����
static const int MAX_BLACK_NUM = 100;			// ������������
static const int MAX_ASK_ADD_FRIEND_NUM = 6;	// ��������������
static const int MAX_RECENTLY_TEAM_NUM = 10;	// �����ӽ�ɫ ����
static const int MAX_RECENTLY_CHAT_NUM = 10;	// �����ϵ�� ����
static const int MAX_REFRESH_TIME_NUM = 60 * 10;// �Ƽ�����ˢ�¼��10����

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
	char				group_type;   //!< 0�Ǻ����飬1���Զ�����1��2���Զ�����2
	long long			avatar_timestamp;
	short				avatar_type;
	short				headshot_id;
	int					top_level;		//�۷�ȼ�
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

	int					user_uid;						// �Ƽ��ĺ��ѵ�id
	GameName			user_name;						// �Ƽ����ѵ�����
	int					user_level;						// ���ѵĵȼ�
	int					top_level;						// �۷�ȼ�
	char				reserve_ch;						// 
	char				user_prof;						// ְҵ
	short				user_avatar_type;				// ��������
	short				user_headshot_id;				// ͷ��
	short				reserve_sh;						
	long long			user_avatar_timestamp;			// ͷ��ʱ���
};

struct FriendRecommendParam
{
	static const int RECOMMEND_LIST_MAX = 6;				// ����Ƽ�����

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

	GameName		group_name[MAX_FRIEND_GROUP_NUM];   //!<  �Զ����� ������,��С2

	int curr_recently_team_index;
	int recently_team[MAX_RECENTLY_TEAM_NUM];

	int curr_recently_chat_index;						
	int curr_recently_chat[MAX_RECENTLY_CHAT_NUM];			//�����ϵ��
	char is_reset_friend_data_flag;					// 0:δ���  1:�����		����������ӱ���,�Ϻ�ӵ�б�ķ��������Ҫ��һ��
	char reserve_ch;
	short reserve_sh;	

};

typedef char FriendCommonParamHex[sizeof(FriendCommonParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(FriendCommonParamHex) < 512);


#pragma pack(pop)

#endif
