#ifndef _PLATFORM_BATTLE_PARAM_HPP__
#define _PLATFORM_BATTLE_PARAM_HPP__

#include "common/tlvprotocol.h"
#include "servercommon/platformbattledef.hpp"

class PlatformBattleParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	struct PlatformBattleSaveEntry
	{
		PlatformBattleSaveEntry()
		{
			this->Reset();
		}

		void Reset()
		{
			change_state = 0;
			rank_item.Reset();
		}

		long long inc_id;
		char change_state;
		PlatformBattleRankEntry rank_item;
	};

	int count;
	PlatformBattleSaveEntry data_list[PLATFORM_BATTLE_RANK_ITEM_MAX];
};

#endif