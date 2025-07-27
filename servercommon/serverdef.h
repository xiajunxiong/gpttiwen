#ifndef SERVERDEF_H
#define SERVERDEF_H

#include "common/vector2d.h"

#define INVALID_OBJ_ID		((ObjID)(-1))
typedef unsigned short		ObjID;
typedef unsigned short		SceneIndex;
typedef int					GuildID;
typedef unsigned short		ItemID;

typedef int					Axis;
typedef TVector2<Axis>		Posi;

typedef char GameName[64];
typedef char JobName[64];
typedef char SuitName[64];
typedef char ItemName[128];
typedef char GuildName[64];
typedef char GuildBanner[8];
typedef char TeamNotice[256];
typedef char GuildNotice[256];
typedef char SessionKey[32];
typedef char TitleName[256];
typedef char MarriageSeekingNotice[128];
typedef char ShiTuSeekingNotice[128];
typedef char LocationName[128];
typedef char NetValueItemParamData[1024];
typedef char RecycleRuleDetailData[256];
typedef char NetValueBuffParamData[32];
typedef char UniqueKeyStr[24];
typedef char OPResult[8192];

typedef char ReplyInfo[384];
typedef char Signature[64];
typedef char UserDefinedReply[1536];
typedef char SerialNum[64];

static const int MAX_DAYCOUNT_COUNT_ID = 256;
typedef char DayCountData[MAX_DAYCOUNT_COUNT_ID + 1];  //  需要增长的话直接改该数字 然后修改数据库字段长度 其他地方已经检查过不用进行任何修改

static const int CUSTOMSET_MAX_SIZE = 60;
typedef char CustomSetData[CUSTOMSET_MAX_SIZE];

typedef char SublockMD5[32];
typedef char SublockString[sizeof(SublockMD5) + 1];

static const int DB_INDEX_MARK_BIT = 20;		// db_index 最多是2047 role_id最多到1048575
static const int MAX_DB_IDNEX = 2047;
static const int MAX_ROLE_ID = 1048575;

static const int MAX_NORMAL_ROBOT_ID = 1 << 20;
static const int NORMAL_ROBOT_ID_START = 1000;
static const int ROBOT_DB_INDEX = 0;

typedef char NoticeContent[4096];
typedef char NoticeSubject[128];
typedef char NoticeTitle[128];

typedef char InstructionStr[64];

enum SEX
{
	FEMALE = 0,
	MALE,
	SEX_MAX,
};

#define MAX_MSG_SIZE	(4 * 1024 * 1024)		// 最大的消息包绝对不可能超过 4M

struct GSNetID
{
	GSNetID() :gateway_netid(-1), netid(-1) {}
	GSNetID(const GSNetID &v) :gateway_netid(v.gateway_netid), netid(v.netid) {}
	GSNetID(unsigned short gn, unsigned short nid) :gateway_netid(gn), netid(nid) {}
	bool operator<(const GSNetID& right) const
	{
		return gateway_netid < right.gateway_netid || (gateway_netid == right.gateway_netid && netid < right.netid);
	}
	bool operator==(const GSNetID &right) const
	{
		return gateway_netid == right.gateway_netid && netid == right.netid;
	}
	bool operator!=(const GSNetID &right) const
	{
		return gateway_netid != right.gateway_netid || netid != right.netid;
	}
	unsigned short gateway_netid;
	unsigned short netid;
};

static const GSNetID INVALID_GSNETID(-1, -1);

struct BGNetID
{
	BGNetID() :gateway_index(-1), net_id(-1) {}
	BGNetID(const BGNetID &v) :gateway_index(v.gateway_index), net_id(v.net_id) {}
	BGNetID(unsigned short index, unsigned short nid) :gateway_index(index), net_id(nid) {}

	bool operator==(const BGNetID &right) const
	{
		return gateway_index == right.gateway_index && net_id == right.net_id;
	}
	bool operator!=(const BGNetID &right) const
	{
		return gateway_index != right.gateway_index || net_id != right.net_id;
	}

	short gateway_index;
	unsigned short net_id;
};
static const BGNetID INVALID_BGNETID(-1, -1);

struct UserID
{
	UserID() :db_index(-1), role_id(0) {}
	UserID(const UserID &v) : db_index(v.db_index), role_id(v.role_id) {}
	UserID(int dbi, int rid) : db_index(dbi), role_id(rid) {}
	UserID(int uid)
	{
		int d = uid >> DB_INDEX_MARK_BIT;
		int r = uid - (d << DB_INDEX_MARK_BIT);

		db_index = d;
		role_id = r;
	}

	bool operator<(const UserID& right) const
	{
		return db_index < right.db_index || (db_index == right.db_index && role_id < right.role_id);
	}

	bool operator>(const UserID& right) const
	{
		return db_index > right.db_index || (db_index == right.db_index && role_id > right.role_id);
	}

	bool operator==(const UserID &right) const
	{
		return db_index == right.db_index && role_id == right.role_id;
	}

	bool operator!=(const UserID &right) const
	{
		return db_index != right.db_index || role_id != right.role_id;
	}

	int db_index;
	int role_id;
};

static const UserID INVALID_USER_ID(-1, 0);
static const UserID VIRTUAL_USER_ID(0, 0);


struct UniqueServerID
{
	UniqueServerID() : plat_type(-1), server_id(0) {}
	UniqueServerID(const UniqueServerID &v) : plat_type(v.plat_type), server_id(v.server_id) {}
	UniqueServerID(int _plat_type, int _server_id) : plat_type(_plat_type), server_id(_server_id) {}

	bool operator<(const UniqueServerID &right) const
	{
		return plat_type < right.plat_type || (plat_type == right.plat_type && server_id < right.server_id);
	}

	bool operator==(const UniqueServerID &right) const
	{
		return plat_type == right.plat_type && server_id == right.server_id;
	}

	bool operator!=(const UniqueServerID &right) const
	{
		return plat_type != right.plat_type || server_id != right.server_id;
	}

	int plat_type;												// 平台类型
	int server_id;												// 服ID
};
static const UniqueServerID INVALID_UNIQUE_SERVER_ID(-1, 0);

struct UniqueHiddenServerID
{
	UniqueHiddenServerID() : plat_type(-1), hidden_server_idx(-1) {}
	UniqueHiddenServerID(const UniqueHiddenServerID &v) : plat_type(v.plat_type), hidden_server_idx(v.hidden_server_idx) {}
	UniqueHiddenServerID(int _plat_type, int _server_id) : plat_type(_plat_type), hidden_server_idx(_server_id) {}

	bool operator<(const UniqueHiddenServerID &right) const
	{
		return plat_type < right.plat_type || (plat_type == right.plat_type && hidden_server_idx < right.hidden_server_idx);
	}

	bool operator==(const UniqueHiddenServerID &right) const
	{
		return plat_type == right.plat_type && hidden_server_idx == right.hidden_server_idx;
	}

	bool operator!=(const UniqueHiddenServerID &right) const
	{
		return plat_type != right.plat_type || hidden_server_idx != right.hidden_server_idx;
	}

	int plat_type;												// 平台类型
	int hidden_server_idx;										// 隐藏服ID
};
static const UniqueHiddenServerID INVALID_UNIQUE_HIDDEN_SERVER_ID(-1, -1);

struct UniqueUserID
{
	UniqueUserID() : plat_type(-1), user_id(INVALID_USER_ID) {}
	UniqueUserID(const UniqueUserID &v) : plat_type(v.plat_type), user_id(v.user_id) {}
	UniqueUserID(int _plat_type, const UserID &_user_id) : plat_type(_plat_type), user_id(_user_id) {}

	bool operator<(const UniqueUserID& right) const
	{
		return plat_type < right.plat_type || (plat_type == right.plat_type && user_id < right.user_id);
	}

	bool operator==(const UniqueUserID &right) const
	{
		return plat_type == right.plat_type && user_id == right.user_id;
	}

	bool operator!=(const UniqueUserID &right) const
	{
		return plat_type != right.plat_type || user_id != right.user_id;
	}

	int plat_type;												// 平台类型
	UserID user_id;												// UserID
};

static const UniqueUserID INVALID_UNIQUE_USER_ID(-1, INVALID_USER_ID);



#endif

