#ifndef __HONG_BAO_DEF_HPP__
#define __HONG_BAO_DEF_HPP__


#include "servercommon/servercommon.h"
#include "common/tlvprotocol.h"

#include <set>

// 红包系统
// 无论什么系统类型的红包 , 红包类型数值不能复用 , 会影响到角色

static const int MAX_HONG_BAO_TYPE_COUNT = 32;			// 红包类型最大数值

typedef char HongBaoGreeting[128];
typedef long long LL_UniqueHongBaoID;


struct UniqueHongBaoID
{
	UniqueHongBaoID() :server_id(-1), hong_bao_sys_type(-1), hong_bao_id(0) {}
	UniqueHongBaoID(const UniqueHongBaoID & _o) :
		server_id(_o.server_id), hong_bao_sys_type(_o.hong_bao_sys_type), hong_bao_id(_o.hong_bao_id) {}
	UniqueHongBaoID(int _server_id, int _hong_bao_sys_type, int _hong_bao_id) :
		server_id(_server_id), hong_bao_sys_type(_hong_bao_sys_type), hong_bao_id(_hong_bao_id) {}
	UniqueHongBaoID(LL_UniqueHongBaoID _ll_unique_hong_bao_id)
	{
		short temp_server_id = _ll_unique_hong_bao_id >> (16 + 32);
		short temp_sys_type = (_ll_unique_hong_bao_id << 16) >> (16 + 32);
		int temp_hong_bao_id = (_ll_unique_hong_bao_id << 32) >> 32;

		server_id = temp_server_id;
		hong_bao_sys_type = temp_sys_type;
		hong_bao_id = temp_hong_bao_id;
	}

	bool operator<(const UniqueHongBaoID& right) const
	{
		if (server_id < right.server_id)
		{
			return true;
		}
		else if (hong_bao_sys_type < right.hong_bao_sys_type)
		{
			return true;
		}

		return hong_bao_id < right.hong_bao_id;
	}

	bool operator>(const UniqueHongBaoID& right) const
	{
		if (server_id > right.server_id)
		{
			return true;
		}
		else if (hong_bao_sys_type > right.hong_bao_sys_type)
		{
			return true;
		}

		return hong_bao_id > right.hong_bao_id;
	}

	bool operator==(const UniqueHongBaoID &right) const
	{
		return server_id == right.server_id && hong_bao_sys_type == right.hong_bao_sys_type  && hong_bao_id == right.hong_bao_id;
	}

	bool operator!=(const UniqueHongBaoID &right) const
	{
		return server_id != right.server_id || hong_bao_sys_type != right.hong_bao_sys_type || hong_bao_id != right.hong_bao_id;
	}

	short server_id;
	short hong_bao_sys_type;
	int hong_bao_id;
};

inline LL_UniqueHongBaoID UniqueHongBaoIDToLLUniqueHongBaoID(UniqueHongBaoID _unique_hong_bao_id)
{
	LL_UniqueHongBaoID temp_ll = 0;

	temp_ll = _unique_hong_bao_id.server_id;
	temp_ll = temp_ll << 16;

	temp_ll |= _unique_hong_bao_id.hong_bao_sys_type;
	temp_ll = temp_ll << 32;

	temp_ll |= _unique_hong_bao_id.hong_bao_id;

	return temp_ll;
}

inline UniqueHongBaoID LLUniqueHongBaoIDToUniqueHongBaoID(LL_UniqueHongBaoID _ll_unique_hong_bao_id)
{
	short temp_server_id = _ll_unique_hong_bao_id >> (16 + 32);
	short temp_sys_type = (_ll_unique_hong_bao_id << 16) >> (16 + 32);
	int temp_hong_bao_id = (_ll_unique_hong_bao_id << 32) >> 32;

	return UniqueHongBaoID(temp_server_id, temp_sys_type, temp_hong_bao_id);
}

enum HongBaoGuildType
{
	HONGBAO_TYPE_INVALID = 0,
	HONGBAO_GUILD_TYPE_FIRST_CHONG = 1,			// 首充红包  
	HONGBAO_GUILD_TYPE_CHONG_ZHI_1 = 2,			// 充值红包 - 数值配表  [旧]  
	HONGBAO_GUILD_TYPE_CHONG_ZHI_2 = 3,			// 充值红包 - 数值配表  [旧] 
	HONGBAO_GUILD_TYPE_CHONG_ZHI_3 = 4,			// 充值红包 - 数值配表  [旧] 
	HONGBAO_GUILD_TYPE_CHONG_ZHI_4 = 5,			// 充值红包 - 数值配表  [旧] 
	HONGBAO_GUILD_TYPE_ZHAN_LING = 6,			// 战令红包 
	HONGBAO_GUILD_TYPE_CHONG_ZHI_5 = 7,			// 充值红包 - 数值配表  [新] 
	HONGBAO_GUILD_TYPE_LEI_CHONG_1 = 8,			// 累充红包 - 数值配表
	HONGBAO_GUILD_TYPE_LEI_CHONG_2 = 9,			// 累充红包 - 数值配表
	HONGBAO_GUILD_TYPE_LEI_CHONG_3 = 10,		// 累充红包 - 数值配表
	HONGBAO_GUILD_TYPE_LEI_CHONG_4 = 11,		// 累充红包 - 数值配表
	HONGBAO_GUILD_TYPE_LEI_CHONG_5 = 12,		// 累充红包 - 数值配表
	HONGBAO_GUILD_TYPE_MAX,

	HONGBAO_TYPE_MAX = HONGBAO_GUILD_TYPE_MAX,
};

static const int HONGBAO_GUILD_TYPE_CHONG_ZHI_1_VAL = 1280;
static const int HONGBAO_GUILD_TYPE_CHONG_ZHI_2_VAL = 1980;
static const int HONGBAO_GUILD_TYPE_CHONG_ZHI_3_VAL = 3280;
static const int HONGBAO_GUILD_TYPE_CHONG_ZHI_4_VAL = 6480;

UNSTD_STATIC_CHECK(HONGBAO_GUILD_TYPE_MAX <= MAX_HONG_BAO_TYPE_COUNT)

//红包系统类型
enum HongBaoSysType
{
	HONGBAO_SYS_TYPE_GUILD = 0,				// 家族红包				infodata:会使用guild_id
	HONGBAO_SYS_TYPE_MAX,
};

static const int MAX_HONG_BAO_GET_MEMBER_COUNT = 50;				// 单个红包的最大人数
static const int MAX_ONCE_HONG_BAO_COUNT = 100;						// 一次性操作的红包数量

typedef char HongBaoInfoData[16];

class SyncHongBaoItem;

class HongBaoItem
{
public:
	union InfoData				// union可带有构造函数，但其成员不能有自定义的构造函数
	{
		InfoData()
		{
			this->Reset();
		}
		~InfoData() {}

		void GetData(HongBaoInfoData out_data) const
		{
			if (NULL == out_data) return;

			memcpy(out_data, data, sizeof(HongBaoInfoData));
		}

		void SetData(const HongBaoInfoData in_data)
		{
			if (NULL == in_data) return;

			memcpy(data, in_data, sizeof(HongBaoInfoData));
		}

		void Reset()
		{
			memset(data, 0, sizeof(data));
		}

		HongBaoInfoData data;
		long long param;
		int guild_id;
		int uid;
	};

	struct RewardInfo			// 红包领取信息
	{
		RewardInfo() { this->Reset(); }
		void Reset()
		{
			uid = 0;
			memset(name, 0, sizeof(name));
			val = 0;
			times = 0;
		}

		int uid;				// 领取人的UID
		GameName name;			// 领取人的名字
		int val;				// 领取到的金额
		unsigned int times;		// 领取的时间戳
	};

	struct HongBaoRewardInfo
	{
		HongBaoRewardInfo() { this->Reset(); }
		void Reset()
		{
			get_count = 0;
			total_count = 0;
			for (int i = 0; i < MAX_HONG_BAO_GET_MEMBER_COUNT; ++i)
			{
				get_info[i].Reset();
			}
		}

		int get_count;							// 领取红包的人员数量
		int total_count;						// 最大人数
		RewardInfo get_info[MAX_HONG_BAO_GET_MEMBER_COUNT];	// 领取红包的人员信息
	};

	HongBaoItem() { this->Reset(); }

	void Reset();

	HongBaoItem& operator= (const HongBaoItem & _o);
	HongBaoItem& operator= (const SyncHongBaoItem & _o);


	char change_state;						// 红包状态

	LL_UniqueHongBaoID ll_unique_hong_bao_id;	// 全服红包唯一ID  [主要用于合服标记]

	int hong_bao_id;						// 红包ID
	unsigned int invalid_timestamp;			// 红包失效时间

	int hongbao_type;						// 红包类型	[策划配置的类型]
	int hongbao_sys_type;					// 红包所属于的系统类型	[例如:世界,家族等等..]

	int send_uid;							// 发红包人的uid
	int send_avatar_type;					// 发红包人的角色模型
	int send_headshot_id;					// 发红包人的头像ID
	GameName send_name;						// 发红包人的名字

	HongBaoGreeting	greeting;				// 红包祝福语
	int used_val;							// 已使用的金额

	InfoData info_data;						// 根据sys的不同用处不同 , 也可能没用到,具体看sys_type

	HongBaoRewardInfo hongbao_reward_info;	// 红包领取信息

	// 下面的不写入数据库
	std::set<int> get_uid_set;				// 已经领取该红包的玩家uid索引
};

#pragma pack(push, 4)

// 用于跨服信息通信
class SyncHongBaoItem
{
public:
	SyncHongBaoItem() { this->Reset(); }

	void Reset();

	SyncHongBaoItem& operator= (const HongBaoItem & _o);

	char change_state;						// 红包状态

	LL_UniqueHongBaoID ll_unique_hong_bao_id;	// 全服红包唯一ID  [主要用于合服标记]

	int hong_bao_id;						// 红包ID
	unsigned int invalid_timestamp;			// 红包失效时间

	int hongbao_type;						// 红包类型	[策划配置的类型]
	int hongbao_sys_type;					// 红包所属于的系统类型	[例如:世界,家族等等..]

	int send_uid;							// 发红包人的uid
	int send_avatar_type;					// 发红包人的角色模型
	int send_headshot_id;					// 发红包人的头像ID
	GameName send_name;						// 发红包人的名字
	HongBaoGreeting	greeting;				// 红包祝福语

	int used_val;							// 已使用的金额

	HongBaoItem::InfoData info_data;					// 根据sys的不同用处不同 , 也可能没用到,具体看sys_type

	HongBaoItem::HongBaoRewardInfo hongbao_reward_info;	// 红包领取信息
};

// 传参用 - 发红包
struct GiveHongBaoUserInfo
{
	GiveHongBaoUserInfo() { this->Reset(); }

	void Reset();

	UniqueServerID from_usid;
	int hongbao_type;						// 红包类型	[策划配置的类型]
	int hongbao_sys_type;					// 红包所属于的系统类型	[例如:世界,家族等等..]
	int send_uid;							// 发红包人的uid
	int send_avatar_type;					// 发红包人的角色模型
	int send_headshot_id;					// 发红包人的头像ID
	GameName send_name;						// 发红包人的名字
	HongBaoGreeting	greeting;				// 红包祝福语
	HongBaoItem::InfoData info_data;		// 根据sys的不同用处不同 , 也可能没用到,具体看sys_type
};


class HongBaoParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	HongBaoParam() :count(0) {}

	int count;
	HongBaoItem hong_bao_list[MAX_ONCE_HONG_BAO_COUNT];
};


typedef char HongBaoInfoDataHex[sizeof(HongBaoInfoData) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(HongBaoInfoDataHex) < 128);

typedef char HongBaoRewardInfoHex[sizeof(HongBaoItem::HongBaoRewardInfo) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(HongBaoRewardInfoHex) < 8192);

#pragma pack(pop)

#endif	//__HONG_BAO_DEF_HPP__