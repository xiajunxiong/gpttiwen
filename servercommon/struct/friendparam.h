#ifndef FRIENDPARSM_H
#define FRIENDPARSM_H

#include "common/tlvprotocol.h"
#include "servercommon/frienddef.hpp"
#include "servercommon/serverdef.h"
#include "servercommon/struct/structcommon.h"

struct FriendItem
{
	FriendItem() { this->Reset(); }

	void Reset()
	{
		change_state = structcommon::CS_NONE;

		role_id = 0;
		friend_uid = 0;
		intimacy = 0;
		battle_add_intimacy = 0;
		group_type = 0;
		memset(role_name, 0, sizeof(role_name));
		sex = 0;
		prof = 0;
		level = 0;
		reserve_sh = 0;
		avatar_timestamp = 0;
		avatar_type = 0;
		headshot_id = 0;
		last_online_timestamp = 0;
	}

	void GetData(FriendInfo & frien_info) const
	{
		frien_info.user_id = friend_uid;
		F_STRNCPY(frien_info.role_name, role_name, sizeof(frien_info.role_name));
		memset(frien_info.family_name, 0, sizeof(frien_info.family_name));
		frien_info.intimacy = intimacy;
		frien_info.sex = (char)sex;
		frien_info.prof = prof;
		frien_info.level = level;
		frien_info.group_type = group_type;
		frien_info.avatar_timestamp = avatar_timestamp;
		frien_info.avatar_type = avatar_type;
		frien_info.headshot_id = headshot_id;
		frien_info.last_online_timestamp = last_online_timestamp;
	}
	
	char		change_state;			// 变更状态		

	int			role_id;				// 角色uid
	int			friend_uid;				// 好友UID
	int			intimacy;				// 亲密度 [作废了 , 现在由frienintimacymanager 记录所有玩家的亲密度]
	int			battle_add_intimacy;	
	int			group_type;				//!< 0是好友组，1是自定义组1，2是自定义组2
	GameName	role_name;				// 好友名称
	short		sex;					// 性别
	short		prof;					// 职业
	short		level;					// 等级
	short		reserve_sh;				// 备用 , 未序列化 , 如果要是用,需要注意添加序列化
	long long	avatar_timestamp;		// 
	short		avatar_type;			// 体型类型
	short		headshot_id;			// 头像id
	unsigned int last_online_timestamp;	// 最后在线的时间
};

class FriendListParam
{
public:
	FriendListParam() : id_from(0),count(0) {}

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);


	long long id_from;									// 读取数据用的
	int count;											// 表长
	FriendItem friend_list[MAX_ONCE_FRIENDS_COUNT];		// 好友信息列表
};

struct BlackItem
{
	BlackItem() { this->Reset(); }

	void Reset()
	{
		change_state = structcommon::CS_NONE;

		role_id = 0;
		uid = 0;
	}

	char		change_state;			// 变更状态	

	int			role_id;				// 玩家UID
	int			uid;					// 黑名单UID
};

class BlacklistsParam
{
public:
	BlacklistsParam() : id_from(0), count(0) {}

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	long long id_from;									// 读取数据用的
	int count;
	BlackItem black_list[MAX_ONCE_FRIENDS_COUNT];
};

// ----------------------------------------------

static const int FRIEND_RELATION_LIST_ONCE_OPER_RECORD_COUNT = 512;
class FriendRelationParam
{
public:
	FriendRelationParam() : last_inc_index(0), count(0) {}

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	struct FriendRelationPair
	{
		FriendRelationPair() : role_id(0), friend_id(0) {}
		int role_id;
		int friend_id;
	};

	long long last_inc_index;		// 最后一个数据的数据库自增id
	int count;
	FriendRelationPair pair_list[FRIEND_RELATION_LIST_ONCE_OPER_RECORD_COUNT];
};
#endif

