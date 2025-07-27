#ifndef __OFFLINE_BATTLE_DATA_DEF_HPP__
#define __OFFLINE_BATTLE_DATA_DEF_HPP__

#include "common/tlvprotocol.h"
#include <vector>
#include "servercommon/struct/battle/battle_def.hpp"

static const int OFFLINE_USER_BATTLE_DATA_ONCE_OPER_COUNT = 64;

struct OfflineUserBattleDataParam
{
	int uid;
	RoleBattleData rbd;
	int today_offline_reward_times;
};

class OfflineUserBattleDataListForInit
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	long long next_id_from;
	std::vector<OfflineUserBattleDataParam> paramlist;
};

class OfflineUserBattleDataListForSave
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	std::vector<OfflineUserBattleDataParam> paramlist;
};

#endif