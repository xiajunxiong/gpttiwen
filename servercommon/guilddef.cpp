#include "guilddef.hpp"

bool AllGuildParam::Serialize(TLVSerializer & outstream) const
{
	bool ret = outstream.Push(count);
	if (!ret) return false;

	for (int i = 0; i < count && i < MAX_ONCE_GUILD_COUNT; ++i)
	{
		ret = guild_list[i].base_param.Serialize(outstream);
		if (!ret) return false;

		ret = guild_list[i].member_param.Serialize(outstream);
		if (!ret) return false;

		ret = guild_list[i].gwt_param.Serialize(outstream);
		if (!ret) return false;

		ret = guild_list[i].guild_boss_param.Serialize(outstream);
		if (!ret) return false;
	}

	return true;
}

bool AllGuildParam::Unserialize(TLVUnserializer & instream)
{
	bool ret = instream.Pop(&count);
	if (!ret) return false;

	for (int i = 0; i < count && i < MAX_ONCE_GUILD_COUNT; ++i)
	{
		ret = guild_list[i].base_param.Unserialize(instream);
		if (!ret) return false;

		ret = guild_list[i].member_param.Unserialize(instream);
		if (!ret) return false;

		ret = guild_list[i].gwt_param.Unserialize(instream);
		if (!ret) return false;

		ret = guild_list[i].guild_boss_param.Unserialize(instream);
		if (!ret) return false;
	}

	return true;
}

bool GuildBaseParam::Serialize(TLVSerializer & outstream) const
{
	const char *guild_name_tmp = guild_name;
	const char *notice_tmp = guild_notice;
	const char *guild_banner_tmp = guild_banner;
	bool ret = outstream.Push(guild_id);
	ret = ret && outstream.Push(create_timestamp);
	ret = ret && outstream.Push(guild_name_tmp);
	ret = ret && outstream.Push(notice_tmp);
	ret = ret && outstream.Push(guild_banner_tmp);
	ret = ret && outstream.Push(guild_level);
	ret = ret && outstream.Push(guild_exp);
	ret = ret && outstream.Push(build_flag);
	ret = ret && outstream.Push(protect_timestamp);
	ret = ret && outstream.Push(guild_honor_title_id);
	ret = ret && outstream.Push(is_need_approval);
	ret = ret && outstream.Push(today_set_banner_flag);
	ret = ret && outstream.Push(reset_guild_eventhandler);
	ret = ret && outstream.Push(reset_guild_eventhandler_move_flag);
	ret = ret && outstream.Push(can_tan_he);
	ret = ret && outstream.Push(guild_fight_title_id);
	ret = ret && outstream.Push(tan_he_zu_zhang_start_time);
	ret = ret && outstream.Push(origin_plat_type);
	ret = ret && outstream.Push(origin_server_id);
	ret = ret && outstream.Push(guild_fight_rank);
	ret = ret && outstream.Push(guild_fight_buff_rank);
	ret = ret && outstream.Push(week_total_guild_active_val);
	ret = ret && outstream.Push(day_total_guild_active_val);
	ret = ret && outstream.Push(is_open_qifu);
	ret = ret && outstream.Push(is_send_will_overflow_mail);
	ret = ret && outstream.Push(is_send_overflow_mail);
	ret = ret && outstream.Push(reserve_ch2);
	ret = ret && outstream.Push(qifu_value);
	ret = ret && outstream.Push(single_building_finish_timestamp);
	ret = ret && outstream.Push(quicken_building_count);
	ret = ret && outstream.Push(can_speed_up);

	if (!ret) return false;

	for (int i = 0; i < GUILD_BUILD_COUNT; ++i)
	{
		bool ret = outstream.Push(build_value[i]);
		if (!ret) return false;
	}

	for (int i = 0; i < GUILD_SEVEN_DAY_ACTIVE; ++i)
	{
		bool ret = outstream.Push(seven_day_active[i]);
		if (!ret) return false;
	}

	{
		TLVSerializer reserve_ch_data;
		reserve_ch_data.Reset((char*)&reserve_ch, sizeof(reserve_ch));
		reserve_ch_data.MoveCurPos(sizeof(reserve_ch));

		ret = outstream.Push(reserve_ch_data);
		if (!ret) return false;
	}

	for (int i = 0; i < ARRAY_LENGTH(build_level); ++i)
	{
		bool ret = outstream.Push(build_level[i]);
		if (!ret) return false;
	}

	{
		TLVSerializer reserve_uint6_data;
		reserve_uint6_data.Reset((char*)&reserve_uint6, sizeof(reserve_uint6));
		reserve_uint6_data.MoveCurPos(sizeof(reserve_uint6));

		ret = outstream.Push(reserve_uint6_data);
		if (!ret) return false;
	}

	return true;
}

bool GuildBaseParam::Unserialize(TLVUnserializer & instream)
{
	const char *guild_name_tmp = NULL;
	const char *notice_tmp = NULL;
	const char *guild_banner_tmp = NULL;
	bool ret = instream.Pop(&guild_id);
	ret = ret && instream.Pop(&create_timestamp);
	ret = ret && instream.Pop(&guild_name_tmp);
	ret = ret && instream.Pop(&notice_tmp);
	ret = ret && instream.Pop(&guild_banner_tmp);
	ret = ret && instream.Pop(&guild_level);
	ret = ret && instream.Pop(&guild_exp);
	ret = ret && instream.Pop(&build_flag);
	ret = ret && instream.Pop(&protect_timestamp);
	ret = ret && instream.Pop(&guild_honor_title_id);
	ret = ret && instream.Pop(&is_need_approval);
	ret = ret && instream.Pop(&today_set_banner_flag);
	ret = ret && instream.Pop(&reset_guild_eventhandler);
	ret = ret && instream.Pop(&reset_guild_eventhandler_move_flag);
	ret = ret && instream.Pop(&can_tan_he);
	ret = ret && instream.Pop(&guild_fight_title_id);
	ret = ret && instream.Pop(&tan_he_zu_zhang_start_time);
	ret = ret && instream.Pop(&origin_plat_type);
	ret = ret && instream.Pop(&origin_server_id);
	ret = ret && instream.Pop(&guild_fight_rank);
	ret = ret && instream.Pop(&guild_fight_buff_rank);
	ret = ret && instream.Pop(&week_total_guild_active_val);
	ret = ret && instream.Pop(&day_total_guild_active_val);
	ret = ret && instream.Pop(&is_open_qifu);
	ret = ret && instream.Pop(&is_send_will_overflow_mail);
	ret = ret && instream.Pop(&is_send_overflow_mail);
	ret = ret && instream.Pop(&reserve_ch2);
	ret = ret && instream.Pop(&qifu_value);
	ret = ret && instream.Pop(&single_building_finish_timestamp);
	ret = ret && instream.Pop(&quicken_building_count);
	ret = ret && instream.Pop(&can_speed_up);

	if (!ret) return false;
	STRNCPY(guild_name, guild_name_tmp, sizeof(guild_name));
	STRNCPY(guild_notice, notice_tmp, sizeof(guild_notice));
	STRNCPY(guild_banner, guild_banner_tmp, sizeof(guild_banner));

	for (int i = 0; i < GUILD_BUILD_COUNT; ++i)
	{
		bool ret = instream.Pop(&build_value[i]);
		if (!ret) return false;
	}

	for (int i = 0; i < GUILD_SEVEN_DAY_ACTIVE; ++i)
	{
		bool ret = instream.Pop(&seven_day_active[i]);
		if (!ret) return false;
	}

	{
		TLVUnserializer reserve_ch_data;

		ret = instream.Pop(&reserve_ch_data);
		if (!ret) return false;

		if (reserve_ch_data.Size() <= sizeof(reserve_ch))
		{
			memcpy(&reserve_ch, reserve_ch_data.Ptr(), reserve_ch_data.Size());
		}
	}

	for (int i = 0; i < ARRAY_LENGTH(build_level); ++i)
	{
		bool ret = instream.Pop(&build_level[i]);
		if (!ret) return false;
	}

	{
		TLVUnserializer reserve_uint6_data;

		ret = instream.Pop(&reserve_uint6_data);
		if (!ret) return false;

		if (reserve_uint6_data.Size() <= sizeof(reserve_uint6))
		{
			memcpy(&reserve_uint6, reserve_uint6_data.Ptr(), reserve_uint6_data.Size());
		}
	}

	return true;
}

bool GuildMemberParam::Serialize(TLVSerializer & outstream) const
{
	bool ret = outstream.Push(count);
	if (!ret) return false;

	for (int i = 0; i < count && i < GUILD_MAX_MEMBER_COUNT; ++i)
	{
		ret = outstream.Push(member_list[i].change_state) && outstream.Push(member_list[i].index);
		if (!ret) return false;

		if (member_list[i].change_state == structcommon::CS_DELETE) continue;

		const char* name = member_list[i].name;

		ret = outstream.Push(member_list[i].uid)
			&& outstream.Push(member_list[i].level)
			&& outstream.Push(member_list[i].m_profession)
			&& outstream.Push(name)
			&& outstream.Push(member_list[i].post)
			&& outstream.Push(member_list[i].gongxian)
			&& outstream.Push(member_list[i].total_gongxian)
			&& outstream.Push(member_list[i].join_time)
			&& outstream.Push(member_list[i].last_login_time);
		if (!ret) return false;

		TLVSerializer appearance_data;
		appearance_data.Reset((void*)&member_list[i].appearance, sizeof(member_list[i].appearance));
		appearance_data.MoveCurPos(sizeof(member_list[i].appearance));
		ret = outstream.Push(appearance_data);

		TLVSerializer member_data;
		member_data.Reset((void *)&member_list[i].member_data, sizeof(member_list[i].member_data));
		member_data.MoveCurPos(sizeof(member_list[i].member_data));
		ret = outstream.Push(member_data);

		if (!ret)
		{
			return false;
		}
	}

	return true;
}

bool GuildMemberParam::Unserialize(TLVUnserializer & instream)
{
	bool ret = instream.Pop(&count);
	if (!ret) return false;

	for (int i = 0; i < count && i < GUILD_MAX_MEMBER_COUNT; ++i)
	{
		ret = instream.Pop(&member_list[i].change_state) && instream.Pop(&member_list[i].index);
		if (!ret) return false;

		if (member_list[i].change_state == structcommon::CS_DELETE) continue;

		const char* name = NULL;

		ret = instream.Pop(&member_list[i].uid)
			&& instream.Pop(&member_list[i].level)
			&& instream.Pop(&member_list[i].m_profession)
			&& instream.Pop(&name)
			&& instream.Pop(&member_list[i].post)
			&& instream.Pop(&member_list[i].gongxian)
			&& instream.Pop(&member_list[i].total_gongxian)
			&& instream.Pop(&member_list[i].join_time)
			&& instream.Pop(&member_list[i].last_login_time);

		if (!ret) return false;

		F_STRNCPY(member_list[i].name, name, sizeof(GameName));

		TLVUnserializer appearance_data;
		ret = instream.Pop(&appearance_data);
		if (!ret)
		{
			return false;
		}

		if (appearance_data.Size() <= sizeof(member_list[i].appearance))
		{
			memcpy(&member_list[i].appearance, appearance_data.Ptr(), appearance_data.Size());
		}

		TLVUnserializer member_data;
		ret = instream.Pop(&member_data);
		if (!ret)
		{
			return false;
		}

		if (member_data.Size() <= sizeof(member_list[i].member_data))
		{
			memcpy(&member_list[i].member_data, member_data.Ptr(), member_data.Size());
		}
	}

	return true;
}

bool GuildWeekTaskParam::Serialize(TLVSerializer & outstream) const
{
	bool ret = outstream.Push(next_refresh_task_timestamp) &&
		outstream.Push(next_awarding_timestamp) &&
		outstream.Push(cur_finish_times) &&
		outstream.Push(cur_gwt_task_id) &&
		outstream.Push(cur_gwt_task_prog);

	return ret;
}

bool GuildWeekTaskParam::Unserialize(TLVUnserializer & instream)
{
	bool ret = instream.Pop(&next_refresh_task_timestamp) &&
		instream.Pop(&next_awarding_timestamp) &&
		instream.Pop(&cur_finish_times) &&
		instream.Pop(&cur_gwt_task_id) &&
		instream.Pop(&cur_gwt_task_prog);

	return ret;
}

bool GuildBossParam::Serialize(TLVSerializer & outstream) const
{
	GuildBossParam temp = *this;

	TLVSerializer guild_boss_data;
	guild_boss_data.Reset(&temp, sizeof(temp));
	guild_boss_data.MoveCurPos(sizeof(temp));

	int ret = outstream.Push(guild_boss_data);
	
	if (!ret)
	{
		return false;
	}

	return true;
}

bool GuildBossParam::Unserialize(TLVUnserializer & instream)
{
	TLVUnserializer guild_boss_data;
	int ret = instream.Pop(&guild_boss_data);
	if (!ret)
	{
		return false;
	}

	if (guild_boss_data.Size() <= sizeof(*this))
	{
		memcpy(this, guild_boss_data.Ptr(), guild_boss_data.Size());
	}

	return true;
}

bool GuildEventhandlerCountParam::Serialize(TLVSerializer & outstream) const
{
	bool ret = true;

	ret = outstream.Push(count);
	if (!ret) return ret;

	TLVSerializer eventhandler_data_data;
	eventhandler_data_data.Reset((char*)&guild_eventhandler_list, sizeof(guild_eventhandler_list));
	eventhandler_data_data.MoveCurPos(sizeof(guild_eventhandler_list));

	ret = outstream.Push(eventhandler_data_data);
	if (!ret) return ret;

	return ret;
}

bool GuildEventhandlerCountParam::Unserialize(TLVUnserializer & instream)
{
	bool ret = true;

	ret = instream.Pop(&count);
	if (!ret) return ret;

	TLVUnserializer eventhandler_data_data;

	ret = instream.Pop(&eventhandler_data_data);
	if (!ret) return ret;

	if (eventhandler_data_data.Size() <= sizeof(guild_eventhandler_list))
	{
		memcpy(&guild_eventhandler_list, eventhandler_data_data.Ptr(), eventhandler_data_data.Size());
	}

	return ret;
}


bool AllGuildGathersDreamParam::Serialize(TLVSerializer & outstream) const
{
	bool ret = outstream.Push(count);
	if (!ret) return false;

	for (int i = 0; i < count && i < MAX_ONCE_GUILD_COUNT; ++i)
	{
		ret = guild_gather_dream_list[i].Serialize(outstream);

		if (!ret) return false;
	}

	return true;
}

bool AllGuildGathersDreamParam::Unserialize(TLVUnserializer & instream)
{
	bool ret = instream.Pop(&count);
	if (!ret) return false;

	for (int i = 0; i < count && i < MAX_ONCE_GUILD_COUNT; ++i)
	{
		ret = guild_gather_dream_list[i].Unserialize(instream);

		if (!ret) return false;
	}

	return true;
}

bool GuildGatherDreamParam::Serialize(TLVSerializer & outstream) const
{
	bool ret = true;

	ret = outstream.Push(count);
	if (!ret) return ret;

	ret = outstream.Push(record_index);
	if (!ret) return ret;

	TLVSerializer guild_gather_dream_data;
	guild_gather_dream_data.Reset((char*)&data_list, sizeof(data_list));
	guild_gather_dream_data.MoveCurPos(sizeof(data_list));

	ret = outstream.Push(guild_gather_dream_data);
	if (!ret) return ret;

	return ret;
}

bool GuildGatherDreamParam::Unserialize(TLVUnserializer & instream)
{
	bool ret = true;

	ret = instream.Pop(&count);
	if (!ret) return ret;

	ret = instream.Pop(&record_index);
	if (!ret) return ret;

	TLVUnserializer guild_gather_dream_data;

	ret = instream.Pop(&guild_gather_dream_data);
	if (!ret) return ret;

	if (guild_gather_dream_data.Size() <= sizeof(data_list))
	{
		memcpy(&data_list, guild_gather_dream_data.Ptr(), guild_gather_dream_data.Size());
	}

	return ret;
}
