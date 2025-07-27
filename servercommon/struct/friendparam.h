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
	
	char		change_state;			// ���״̬		

	int			role_id;				// ��ɫuid
	int			friend_uid;				// ����UID
	int			intimacy;				// ���ܶ� [������ , ������frienintimacymanager ��¼������ҵ����ܶ�]
	int			battle_add_intimacy;	
	int			group_type;				//!< 0�Ǻ����飬1���Զ�����1��2���Զ�����2
	GameName	role_name;				// ��������
	short		sex;					// �Ա�
	short		prof;					// ְҵ
	short		level;					// �ȼ�
	short		reserve_sh;				// ���� , δ���л� , ���Ҫ����,��Ҫע��������л�
	long long	avatar_timestamp;		// 
	short		avatar_type;			// ��������
	short		headshot_id;			// ͷ��id
	unsigned int last_online_timestamp;	// ������ߵ�ʱ��
};

class FriendListParam
{
public:
	FriendListParam() : id_from(0),count(0) {}

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);


	long long id_from;									// ��ȡ�����õ�
	int count;											// ��
	FriendItem friend_list[MAX_ONCE_FRIENDS_COUNT];		// ������Ϣ�б�
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

	char		change_state;			// ���״̬	

	int			role_id;				// ���UID
	int			uid;					// ������UID
};

class BlacklistsParam
{
public:
	BlacklistsParam() : id_from(0), count(0) {}

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	long long id_from;									// ��ȡ�����õ�
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

	long long last_inc_index;		// ���һ�����ݵ����ݿ�����id
	int count;
	FriendRelationPair pair_list[FRIEND_RELATION_LIST_ONCE_OPER_RECORD_COUNT];
};
#endif

