#ifndef __COURAGE_CHALLENGE_RANK_DEF_HPP__
#define __COURAGE_CHALLENGE_RANK_DEF_HPP__

#include "servercommon/servercommon.h"
#include "common/tlvprotocol.h"

#pragma pack(push, 4)

const static int MAX_COURAGE_CHALLENGE_RANK_NUM = 100;
const static int COURAGE_CHALLENGE_SAVE_DB_TIME = 60;		//每60秒保存一次到数据库

enum COURAGE_CHALLENGE_RANK_TYPE
{
	COURAGE_CHALLENGE_RANK_TYPE_SERVER = 0,				//本服排行
	COURAGE_CHALLENGE_RANK_TYPE_GUILD = 1,				//家族排行
	COURAGE_CHALLENGE_RANK_TYPE_FRIEND = 2,				//好友排行

	COURAGE_CHALLENGE_RANK_TYPE_MAX,
};

struct SynCourageChallengeRankInfo;
struct CourageChallengeRankItem
{
	CourageChallengeRankItem() { this->Reset(); }

	void SetData(SynCourageChallengeRankInfo & role_info);
	void Reset()
	{
		pos = 0;
		uid = 0;
		memset(user_name, 0, sizeof(user_name));
		avatar_type = 0;
		headshot_id = 0;
		prof = 0;
		reserve_sh = 0;
		star = 0;
		layer_level = 0;
		get_timestamp = 0;
	}

	bool IsValid()
	{
		return uid > 0;
	}

	char change_state;
	int pos;

	int uid;
	GameName user_name;
	short avatar_type;
	short headshot_id;
	short prof;
	short reserve_sh;
	int star;
	int layer_level;
	long long get_timestamp;
};

struct SynCourageChallengeRankInfo
{
	SynCourageChallengeRankInfo() : uid(0), avatar_type(0), headshot_id(0), prof(0), reserve_sh(0), star(0), layer_level(0), get_timestamp(0)
	{
		memset(user_name, 0, sizeof(user_name));
	}

	SynCourageChallengeRankInfo(CourageChallengeRankItem & role_info)
	{
		uid = role_info.uid;
		STRNCPY(user_name, role_info.user_name, sizeof(user_name));
		avatar_type = role_info.avatar_type;
		headshot_id = role_info.headshot_id;
		prof = role_info.prof;
		reserve_sh = role_info.reserve_sh;
		star = role_info.star;
		layer_level = role_info.layer_level;
		get_timestamp = role_info.get_timestamp;
	}

	int uid;
	GameName user_name;
	short avatar_type;
	short headshot_id;
	short prof;
	short reserve_sh;
	int star;
	int layer_level;
	long long get_timestamp;
};

#pragma pack(pop)

struct CourageChallengeRankParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	std::map<int, CourageChallengeRankItem> data_list;
};

#endif