#include "worldstatusparam.hpp"

#include "servercommon/servercommon.h"
#include "servercommon/struct/structcommon.h"

bool WorldStatusParam::Serialize(TLVSerializer &outstream) const
{
	bool ret = outstream.Push(start_time) && 
		outstream.Push(world_time) && 
		outstream.Push(last_update_dayid);

	if (!ret)
	{
		return false;
	}

	{
		TLVSerializer common_data_data;
		common_data_data.Reset((char*)&common_data, sizeof(common_data));
		common_data_data.MoveCurPos(sizeof(common_data));

		ret = outstream.Push(common_data_data);
		if (!ret) return false;
	}
	{
		TLVSerializer honor_roll_data_data;
		honor_roll_data_data.Reset((char*)&honor_roll_data, sizeof(honor_roll_data));
		honor_roll_data_data.MoveCurPos(sizeof(honor_roll_data));

		ret = outstream.Push(honor_roll_data_data);
		if (!ret) return false;
	}

	{
		TLVSerializer wild_boss_data_data;
		wild_boss_data_data.Reset((char*)&wild_boss_data, sizeof(wild_boss_data));
		wild_boss_data_data.MoveCurPos(sizeof(wild_boss_data));

		ret = outstream.Push(wild_boss_data_data);
		if (!ret) return false;
	}

	{
		TLVSerializer courage_challenge_data_data;
		courage_challenge_data_data.Reset((char*)&courage_challenge_data, sizeof(courage_challenge_data));
		courage_challenge_data_data.MoveCurPos(sizeof(courage_challenge_data));

		ret = outstream.Push(courage_challenge_data_data);
		if (!ret) return false;
	}

	{
		TLVSerializer chief_election_data_data;
		chief_election_data_data.Reset((char*)&chief_election_data, sizeof(chief_election_data));
		chief_election_data_data.MoveCurPos(sizeof(chief_election_data));

		ret = outstream.Push(chief_election_data_data);
		if (!ret) return false;
	}

	{
		TLVSerializer chief_peak_battle_data_data;
		chief_peak_battle_data_data.Reset((char*)&chief_peak_battle_data, sizeof(chief_peak_battle_data));
		chief_peak_battle_data_data.MoveCurPos(sizeof(chief_peak_battle_data));

		ret = outstream.Push(chief_peak_battle_data_data);
		if (!ret) return false;
	}

	{
		TLVSerializer colosseum_manager_data_data;
		colosseum_manager_data_data.Reset((char *)&colosseum_manager_data, sizeof(colosseum_manager_data));
		colosseum_manager_data_data.MoveCurPos(sizeof(colosseum_manager_data));

		ret = outstream.Push(colosseum_manager_data_data);
		if (!ret) return false;
	}
	
	{
		ret = outstream.Push(pool_id_list_len);
		if (!ret) return false;
		
		TLVSerializer old_god_pool_data;
		old_god_pool_data.Reset((void*)pool_player_id_list, CHALLENGE_FIELD_POOL_PLAYER_LIST_LEN);
		old_god_pool_data.MoveCurPos(pool_id_list_len);
		
		ret = outstream.Push(old_god_pool_data);
		if (!ret) return false;
	}

	{
		TLVSerializer only_fight_data_data;
		only_fight_data_data.Reset((char *)&only_fight_data, sizeof(only_fight_data));
		only_fight_data_data.MoveCurPos(sizeof(only_fight_data));

		ret = outstream.Push(only_fight_data_data);
		if (!ret) return false;
	}

	{
		TLVSerializer shan_hai_boss_data_data;
		shan_hai_boss_data_data.Reset((char *)&shan_hai_boss_data, sizeof(shan_hai_boss_data));
		shan_hai_boss_data_data.MoveCurPos(sizeof(shan_hai_boss_data));

		ret = outstream.Push(shan_hai_boss_data_data);
		if (!ret) return false;
	}

	{
		TLVSerializer history_top_price_data_data;
		history_top_price_data_data.Reset((char *)&history_top_price_data, sizeof(history_top_price_data));
		history_top_price_data_data.MoveCurPos(sizeof(history_top_price_data));

		ret = outstream.Push(history_top_price_data_data);
		if (!ret) return false;
	}

	{
		TLVSerializer guild_fight_data_data;
		guild_fight_data_data.Reset((char *)&guild_fight_data, sizeof(guild_fight_data));
		guild_fight_data_data.MoveCurPos(sizeof(guild_fight_data));

		ret = outstream.Push(guild_fight_data_data);
		if (!ret) return false;
	}

	{
		TLVSerializer s_data;
		s_data.Reset((char *)&wedding_global_data, sizeof(wedding_global_data));
		s_data.MoveCurPos(sizeof(wedding_global_data));

		ret = outstream.Push(s_data);
		if (!ret) return false;
	}

	{
		TLVSerializer s_data;
		s_data.Reset((char *)&qingyuan_duiduipeng_data, sizeof(qingyuan_duiduipeng_data));
		s_data.MoveCurPos(sizeof(qingyuan_duiduipeng_data));

		ret = outstream.Push(s_data);
		if (!ret) return false;
	}

	{
		ret = outstream.Push(god_beast_advent_data);
		if (!ret) return false;
	}


	{
		TLVSerializer s_data;
		s_data.Reset((char *)&kua_fu_ying_xiong_lu_data, sizeof(kua_fu_ying_xiong_lu_data));
		s_data.MoveCurPos(sizeof(kua_fu_ying_xiong_lu_data));

		ret = outstream.Push(s_data);
		if (!ret) return false;
	}

	{
		TLVSerializer s_data;
		s_data.Reset((char *)&huan_shou_ann_data, sizeof(huan_shou_ann_data));
		s_data.MoveCurPos(sizeof(huan_shou_ann_data));

		ret = outstream.Push(s_data);
		if (!ret) return false;
	}

	return ret;
}

bool WorldStatusParam::Unserialize(TLVUnserializer &instream)
{
	bool ret = instream.Pop(&start_time) &&
		instream.Pop(&world_time) && 
		instream.Pop(&last_update_dayid);

	if (!ret)
	{
		return false;
	}

	TLVUnserializer common_data_data;
	{
		ret = instream.Pop(&common_data_data);
		if (!ret) return false;

		if (common_data_data.Size() <= sizeof(common_data))
		{
			memcpy(&common_data, common_data_data.Ptr(), common_data_data.Size());
		}
	}

	TLVUnserializer honor_roll_data_data;
	{
		ret = instream.Pop(&honor_roll_data_data);
		if (!ret) return false;

		if (honor_roll_data_data.Size() <= sizeof(honor_roll_data))
		{
			memcpy(&honor_roll_data, honor_roll_data_data.Ptr(), honor_roll_data_data.Size());
		}
	}

	{
		TLVUnserializer wild_boss_data_data;
		ret = instream.Pop(&wild_boss_data_data);
		if (!ret) return false;

		if (wild_boss_data_data.Size() <= sizeof(wild_boss_data))
		{
			memcpy(&wild_boss_data, wild_boss_data_data.Ptr(), wild_boss_data_data.Size());
		}
	}

	{
		TLVUnserializer courage_challenge_data_data;
		ret = instream.Pop(&courage_challenge_data_data);
		if (!ret) return false;

		if (courage_challenge_data_data.Size() <= sizeof(courage_challenge_data))
		{
			memcpy(&courage_challenge_data, courage_challenge_data_data.Ptr(), courage_challenge_data_data.Size());
		}
	}

	{
		TLVUnserializer chief_election_data_data;
		ret = instream.Pop(&chief_election_data_data);
		if (!ret) return false;
		
		if (chief_election_data_data.Size() <= sizeof(chief_election_data))
		{
			memcpy(&chief_election_data, chief_election_data_data.Ptr(), chief_election_data_data.Size());
		}
	}

	{
		TLVUnserializer chief_peak_battle_data_data;
		ret = instream.Pop(&chief_peak_battle_data_data);
		if (!ret) return false;

		if (chief_peak_battle_data_data.Size() <= sizeof(chief_peak_battle_data))
		{
			memcpy(&chief_peak_battle_data, chief_peak_battle_data_data.Ptr(), chief_peak_battle_data_data.Size());
		}
	}

	{
		TLVUnserializer colosseum_manager_data_data;
		ret = instream.Pop(&colosseum_manager_data_data);
		if (!ret) return false;

		if (colosseum_manager_data_data.Size() <= sizeof(colosseum_manager_data))
		{
			memcpy(&colosseum_manager_data, colosseum_manager_data_data.Ptr(), colosseum_manager_data_data.Size());
		}
	}

	{
		ret = instream.Pop(&pool_id_list_len);
		if (!ret) return false;
		
		TLVUnserializer old_god_pool_data;
		ret = instream.Pop(&old_god_pool_data);
		if (!ret) return false;
		
		if (old_god_pool_data.Size() <= sizeof(pool_player_id_list))
		{
			memcpy(pool_player_id_list, old_god_pool_data.Ptr(), old_god_pool_data.Size());
		}
	}

	{
		TLVUnserializer only_fight_data_data;
		ret = instream.Pop(&only_fight_data_data);
		if (!ret) return false;

		if (only_fight_data_data.Size() <= sizeof(only_fight_data))
		{
			memcpy(&only_fight_data, only_fight_data_data.Ptr(), only_fight_data_data.Size());
		}
	}

	{
		TLVUnserializer shan_hai_boss_data_data;
		ret = instream.Pop(&shan_hai_boss_data_data);
		if (!ret) return false;

		if (shan_hai_boss_data_data.Size() <= sizeof(shan_hai_boss_data))
		{
			memcpy(&shan_hai_boss_data, shan_hai_boss_data_data.Ptr(), shan_hai_boss_data_data.Size());
		}
	}

	{
		TLVUnserializer history_top_price_data_data;
		ret = instream.Pop(&history_top_price_data_data);
		if (!ret) return false;

		if (history_top_price_data_data.Size() <= sizeof(history_top_price_data))
		{
			memcpy(&history_top_price_data, history_top_price_data_data.Ptr(), history_top_price_data_data.Size());
		}
	}

	{
		TLVUnserializer guild_fight_data_data;
		ret = instream.Pop(&guild_fight_data_data);
		if (!ret) return false;

		if (guild_fight_data_data.Size() <= sizeof(guild_fight_data))
		{
			memcpy(&guild_fight_data, guild_fight_data_data.Ptr(), guild_fight_data_data.Size());
		}
	}

	{
		TLVUnserializer uns_data;
		ret = instream.Pop(&uns_data);
		if (!ret) return false;

		if (uns_data.Size() <= sizeof(wedding_global_data))
		{
			memcpy(&wedding_global_data, uns_data.Ptr(), uns_data.Size());
		}
	}

	{
		TLVUnserializer uns_data;
		ret = instream.Pop(&uns_data);
		if (!ret) return false;

		if (uns_data.Size() <= sizeof(qingyuan_duiduipeng_data))
		{
			memcpy(&qingyuan_duiduipeng_data, uns_data.Ptr(), uns_data.Size());
		}
	}

	{
		ret = instream.Pop(&god_beast_advent_data);
		if (!ret) return false;
	}

	{
		TLVUnserializer uns_data;
		ret = instream.Pop(&uns_data);
		if (!ret) return false;

		if (uns_data.Size() <= sizeof(kua_fu_ying_xiong_lu_data))
		{
			memcpy(&kua_fu_ying_xiong_lu_data, uns_data.Ptr(), uns_data.Size());
		}
	}

	{
		TLVUnserializer uns_data;
		ret = instream.Pop(&uns_data);
		if (!ret) return false;

		if (uns_data.Size() <= sizeof(huan_shou_ann_data))
		{
			memcpy(&huan_shou_ann_data, uns_data.Ptr(), uns_data.Size());
		}
	}

	return ret;
}

