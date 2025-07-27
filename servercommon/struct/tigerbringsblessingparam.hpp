#ifndef __TIGER_BRINGS_BLESSING_PARAM_HPP__
#define __TIGER_BRINGS_BLESSING_PARAM_HPP__

#include "common/tlvprotocol.h"
#include "servercommon/serverdef.h"
#include "servercommon/struct/structcommon.h"

static const int MAX_ONCE_TIGER_BRINGS_BLESSING_COUNT = 1000;				// 一次性操作最大数量
static const int MAX_TIGER_BRINGS_BLESSING_REWARD_USER_COUNT = 5;			// 最大得奖玩家数量

enum TIGER_BRINGS_BLESSING_STATUS
{
	TIGER_BRINGS_BLESSING_STATUS_INVALID = 0,
	TIGER_BRINGS_BLESSING_STATUS_BUY = 1,			// 购买期  (可购买
	TIGER_BRINGS_BLESSING_STATUS_REWARD = 2,			// 结算期  (结算奖励
	TIGER_BRINGS_BLESSING_STATUS_SHOW = 3,			// 展示期  (公布获奖
	TIGER_BRINGS_BLESSING_STATUS_FINISH = 4,			// 结束	   (整体结束
	TIGER_BRINGS_BLESSING_STATUS_MAX,

};

struct TigerBringsBlessingBuyInfo								//  购买的玩家的信息
{
	TigerBringsBlessingBuyInfo() {}
	void Reset()
	{
		change_state = structcommon::CS_NONE;

		role_id = 0;
		last_notice_timestamp = 0;
	}

	char change_state;									// 变更状态

	int role_id;										// 玩家ID
	unsigned int last_notice_timestamp;					// 开奖时需要通知玩家 , 如果玩家不在线则等上线时再通知
};

struct TigerBringsBlessingRewardUserInfo					//  获奖玩家信息
{
	void Reset()
	{
		role_id = 0;
		avatar_type = 0;
		headshot_id = 0;
		memset(role_name, 0, sizeof(role_name));
	}

	int role_id;										// 玩家ID
	short  avatar_type;									// 体型类型
	short  headshot_id;									// 头像ID
	GameName role_name;									// 玩家名称
};

#pragma pack(push, 4)

class TigerBringsBlessingBuyListParam
{
public:
	TigerBringsBlessingBuyListParam()
	{
		id_from = 0;
		count = 0;
		memset(user_list, 0, sizeof(user_list));
	}

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	long long id_from;
	int count;
	TigerBringsBlessingBuyInfo user_list[MAX_ONCE_TIGER_BRINGS_BLESSING_COUNT];
};

#pragma pack(pop)

#endif	