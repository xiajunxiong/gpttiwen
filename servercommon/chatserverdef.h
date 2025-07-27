/******************************************************************************************
FileName: chatserverdef.hpp
Author: dreamwang
Description: 聊天服务器相关定义 
Version: 1.0
History: 
<author>    <time>					<version >    <desc>
dreamwang   2011/08/08/ 09:04:05     1.0           创建
*****************************************************************************************/

#ifndef __CHATDEF_H__
#define __CHATDEF_H__

#include "servercommon/basedef.h"
#include "servercommon/serverdef.h"
#include "common/tlvprotocol.h"
#include "servercommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/struct/battle/battle_def.hpp"

typedef unsigned int	NetID;
typedef Int32			UID;
typedef UInt32			UserKey;					// 由用户创建用户时赋予用户 用户同时传输一个UserKey 两者必须相等
typedef Int32			ChannelID;
typedef Int32			ChannelType;


static const int WORLD_CHANNEL_ID = 0;
static const int MAX_CHAT_LENGTH = 2048;
static const int MAX_SPEAKER_MSG_LENGTH = 512;

static const int CHANNEL_OPEN_LEVEL_DEFAULT = 10;

static const int SINGLECHAT_OPEN_LEVEL = 40;

static const int CHAT_MSG_ITEM_INFO_MAX_NUM = 3;		//聊天拥有的物品信息最多的数量
static const long long TEMP_CHAT_GROUP_ID = 1LL;
static const int TEMP_CHAT_GROUP_CREATOR_UID = 1;

static const int RECENT_CHAT_SAVE_NUM = 20;
static const int RECENT_NOTICE_SAVE_NUM = 20;

enum CHAT_MSG_ITEM_INFO_TYPE
{
	CHAT_MSG_ITEM_INFO_TYPE_ITEM,		//背包物品	p1 p2 背包物品index
	CHAT_MSG_ITEM_INFO_TYPE_PET,		//宠物  p1 pet_index
	CHAT_MSG_ITEM_INFO_TYPE_EQUIPMENT,	//装备 装备部位
	CHAT_MSG_ITEM_INFO_TYPE_JEWELRY,	//灵饰 灵饰类型
	CHAT_MSG_ITEM_INFO_TYPE_MEDAL,		//勋章 装备index
	CHAT_MSG_ITEM_INFO_TYPE_EMOJ,		//客户端的type5被表情占用了, 这边也加个占用一下
	CHAT_MSG_ITEM_INFO_TYPE_HOLYEQUIP,	//圣器 装备index
};

struct ChatItemInfo  //聊天中物品信息
{
	ChatItemInfo()
	{

	}

	ItemDataWrapper item;
	int atts[BATTLE_ATTR_MAX];
};

#pragma pack(push, 4)
struct ChatMsgItemInfo		//聊天包含的物品信息
{	
	int type;
	int param1;	
	int show;
	int show2;
	long long param2;
};

static const int CHAT_GROUP_MEMBER_MAX = 10;			// 群成员上线，暂定20
static const int ONCE_SEND_CHAT_GROUP_COUNT = 100;		// 每次最多存取100个群
static const int SAVE_GROUP_DATA_INTERVAL = 30;			// 半分钟写一次
static const int CHAT_ITEM_CHECK_SAVE_TIME = 600;		// 道具查询保留10分钟

union ChatItemUniqueSeq
{
	struct
	{
		short server_id;
		unsigned short seq;
		unsigned int time;
	};

	long long unique_seq;
};

enum ChatGroupDataStatus
{
	CHAT_GROUP_DATA_STATUS_INVALID = 0,						// 初始状态
	CHAT_GROUP_DATA_STATUS_LOADING,							// 数据加载中 
	CHAT_GROUP_DATA_STATUS_LOAD_FINISH,						// 数据加载完成 
	CHAT_GROUP_DATA_STATUS_MAX,
};

union ChatGroupUniqueId
{
	struct
	{
		short server_id;
		unsigned short seq;
		unsigned int time;
	};

	long long unique_id;
};

struct ChatGroupUserInfo
{
	ChatGroupUserInfo()
	{
		this->Reset();
	}

	void Reset()
	{
		member_uid = 0;
		member_ava = 0;
		memset(member_name, 0, sizeof(GameName));
		member_prof = 0;
		member_level = 0;
		member_logout_tamp = 0;
	}

	int member_uid;
	int member_ava;
	GameName member_name;
	int member_prof;
	int member_level;
	unsigned int member_logout_tamp;	
};

struct ChatGroupInfo
{
	ChatGroupInfo() 
	{
		this->Reset();
	}

	void Reset()
	{
		group_id = 0;
		is_cross = 0;
		creator_uid = 0;
		member_count = 0;
		memset(group_name, 0, sizeof(GameName));
		memset(member_uid_list, 0, sizeof(member_uid_list));
		memset(group_notice, 0, sizeof(GuildNotice));
		memset(member_last_check_notice_time, 0, sizeof(member_last_check_notice_time));
	}

	ChatGroupInfo& operator=(const ChatGroupInfo& _src)
	{
		if (this == &_src) return *this;
		group_id = _src.group_id;
		is_cross = _src.is_cross;
		creator_uid = _src.creator_uid;
		member_count = _src.member_count;
		memcpy(member_uid_list, _src.member_uid_list, sizeof(member_uid_list));
		memcpy(group_name, _src.group_name, sizeof(GameName));
		memcpy(group_notice, _src.group_notice, sizeof(GuildNotice));
		memcpy(member_last_check_notice_time, _src.member_last_check_notice_time, sizeof(member_last_check_notice_time));
		return *this;
	}

	ChatGroupUserInfo * GetUserInfo(int uid, int *member_idx)
	{
		for (int i = 0; i < member_count && i < CHAT_GROUP_MEMBER_MAX; ++i)
		{
			if (member_uid_list[i].member_uid == uid)
			{
				if (NULL != member_idx)
				{
					*member_idx = i;
				}
				return &member_uid_list[i];
			}
		}
		return NULL;
	}

	ChatGroupUserInfo * GetUserInfo(int uid)
	{
		for (int i = 0; i < member_count && i < CHAT_GROUP_MEMBER_MAX; ++i)
		{
			if (member_uid_list[i].member_uid == uid)
			{
				return &member_uid_list[i];
			}
		}
		return NULL;
	}

	const ChatGroupUserInfo* GetConstUserInfo(int uid) const
	{
		for (int i = 0; i < member_count && i < CHAT_GROUP_MEMBER_MAX; ++i)
		{
			if (member_uid_list[i].member_uid == uid)
			{
				return &member_uid_list[i];
			}
		}
		return NULL;
	}

	bool RemoveUserInfo(int uid)
	{
		if (uid == creator_uid)return false;
		for (int i = 0; i < member_count && i < CHAT_GROUP_MEMBER_MAX; ++i)
		{
			if (member_uid_list[i].member_uid != uid)
			{
				continue;
			}
			member_uid_list[i] = member_uid_list[member_count - 1];
			member_uid_list[member_count].member_uid = 0;
			--member_count;
			return true;
		}
		return false;
	}

	bool AddUserInfo(int member_id, int member_ava, int member_level, int member_prof, const GameName member_name)
	{
		if (this->GetUserInfo(member_id) != NULL)return false;
		if (member_count >= CHAT_GROUP_MEMBER_MAX)return false;
		member_uid_list[member_count].member_ava = member_ava;
		member_uid_list[member_count].member_level = member_level;
		member_uid_list[member_count].member_prof = member_prof;
		F_STRNCPY(member_uid_list[member_count].member_name, member_name, sizeof(GameName));
		member_uid_list[member_count++].member_uid = member_id;
		return true;
	}

	long long group_id;
	GameName group_name;
	GuildNotice group_notice;
	int is_cross;
	int creator_uid;
	int member_count;
	ChatGroupUserInfo member_uid_list[CHAT_GROUP_MEMBER_MAX];
	unsigned int member_last_check_notice_time[CHAT_GROUP_MEMBER_MAX];
};
typedef char ChatGroupInfoHex[sizeof(ChatGroupInfo) * 2 + 1];

typedef char ChatGroupUserInfoHex[sizeof(ChatGroupUserInfo) * CHAT_GROUP_MEMBER_MAX * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ChatGroupUserInfoHex) < 4096);

struct ChatGroupParam
{
	ChatGroupParam() : count(0)
	{
		memset(change_state, 0, sizeof(change_state));
	}

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	int count;
	int change_state[ONCE_SEND_CHAT_GROUP_COUNT];
	ChatGroupInfo info_list[ONCE_SEND_CHAT_GROUP_COUNT];
};

#pragma pack(pop)

#endif // __CHATDEF_H__


