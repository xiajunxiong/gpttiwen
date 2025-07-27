#include "servercommon/struct/structcommon.h"

#include "commonsavedef.h"

unsigned int GetCommonSaveDataLengthByType(int save_type)
{
	unsigned int length = 0;
	switch (save_type)
	{
		case COMMON_SAVE_DATA_TYPE_BRAVE_GROUND_TEAM_RECORD:
			{
				length = sizeof(CommonSaveBraveGroundTeamReceordData);
			}
			break;
		case COMMON_SAVE_DATA_TYPE_BRAVE_GROUND_ROLE:
			{
				length = sizeof(CommonSaveBraveGroundRoleData);
			}
			break;
		case COMMON_SAVE_DATA_TYPE_HUAN_JIE_TIAN_TI_RECORD:
			{
				length = sizeof(CommonSaveHuanJieTianTiReceordData);
			}
			break;
		case COMMON_SAVE_DATA_TYPE_PLATFORM_BATTLE_RECORD:
			{
				length = sizeof(CommonSavePlatformBattleReceordData);
			}
			break;
		case COMMON_SAVE_DATA_TYPE_BRAVE_GROUND_SAVE_DATA:
			{
				length = sizeof(CommonSaveBraveGroundData);
			}
			break;
		case COMMON_SAVE_DATA_TYPE_HUAN_JIE_SAVE_DATA:
			{
				length = sizeof(CommonSaveHuanJieData);
			}
			break;
		case COMMON_SAVE_DATA_TYPE_GUILD_FIGHT_DATA:
			{
				length = sizeof(CommonSaveGuildFightData);
			}
			break;
		case COMMON_SAVE_DATA_TYPE_GUILD_HONOR_DATA:
			{
				length = sizeof(CommonSaveGuildHonorData);
			}
			break;
		case COMMON_SAVE_DATA_TYPE_GUILD_HONOR_RECORD_OLD_DATA:
			{
				length = sizeof(CommonSaveGuildHonorRecordOldData);
			}
			break;
		case COMMON_SAVE_DATA_TYPE_PLATFORM_BATTLE_ACHIEVEMENT_DATA:
			{
				length = sizeof(CommonSavePlatformBattleAchievementData);
			}
			break;
		case COMMON_SAVE_DATA_TYPE_PLATFORM_BATTLE_OTHER_DATA:
			{
				length = sizeof(CommonSavePlatformBattleOtherData);
			}
			break;
		case COMMON_SAVE_DATA_TYPE_GIVE_GIFT:
			{
				length = sizeof(CommonSaveGiveGiftData);
			}
			break;
		case COMMON_SAVE_DATA_TYPE_RA_DELAY_SET_TIME_DATA:
			{
				length = sizeof(CommonSaveRADelaySetTimeData);
			}
			break;
		case COMMON_SAVE_DATA_TYPE_GUILD_HONOR_RECORD_DATA:
			{
				length = sizeof(CommonSaveGuildHonorRecordData);
			}
			break;
		case COMMON_SAVE_DATA_TYPE_JIE_YI_ZU:
			{
				length = sizeof(CommonSaveJieYiZu);
			}
			break;
		case COMMON_SAVE_DATA_TYPE_JIE_YI_ROLE_XUAN_YAN:
			{
				length = sizeof(CommonSaveJieYiRoleXuanYan);
			}
			break;
		case COMMON_SAVE_DATA_TYPE_JIE_YI_ZU_XUAN_YAN:
			{
				length = sizeof(CommonSaveJieYiZuXuanYan);
			}
			break;
		case COMMON_SAVE_DATA_TYPE_COW_REPORT_GOOD_NEWS_DATA:
			{
				length = sizeof(CommonSaveCowReportGoodNewsData);
			}
			break;
		case COMMON_SAVE_DATA_TYPE_TIGER_BRINGS_BLESSING_DATA:
			{
				length = sizeof(CommonSaveTigerBringsBlessingData);
			}
			break;
		case COMMON_SAVE_DATA_TYPE_QING_YUAN_DATA:
			{
				length = sizeof(CommonSaveQingYuan);
			}
			break;
		case COMMON_SAVE_DATA_TYPE_HONGMENG_TIANJI:
			{
				length = sizeof(CommonSaveWorldHongMengTianJie);
			}
			break;
		default:
			{
				length = 0;
			}
			break;
	}
	UNSTD_STATIC_CHECK(COMMON_SAVE_DATA_TYPE_MAX == 22);

	if (length > sizeof(CommonSaveData))
	{
		length = 0;
	}

	return length;
}

bool IsPlatformBattleRankAchievementType(int type)
{
	switch (type)
	{
		case PLATFORM_BATTLE_ACHIEVEMENT_ANY_TOP_X:
		case PLATFORM_BATTLE_ACHIEVEMENT_BA_ZHU_TOP_X :
		case	PLATFORM_BATTLE_ACHIEVEMENT_QING_LONG_TOP_X :
		case	PLATFORM_BATTLE_ACHIEVEMENT_BAI_HU_TOP_X :
		case	PLATFORM_BATTLE_ACHIEVEMENT_ZHU_QUE_TOP_X:
		case PLATFORM_BATTLE_ACHIEVEMENT_XUAN_WU_TOP_X :
			{
				return true;
			}

		default:
			return false;
	}

	return false;
}



void CommonSaveBraveGroundRoleData::Reset()
{
	role_id = 0;
	reached_level = 0;
}

bool CommonSaveDataParam::Serialize(TLVSerializer & outstream) const
{
	bool ret = outstream.Push(count);
	if (!ret)
	{
		return false;
	}

	for (int i = 0; i < count && i < ARRAY_LENGTH(data_list); ++i)
	{
		const DBCommonSaveDataItem & rank_item = data_list[i];

		ret = outstream.Push(rank_item.change_state) && outstream.Push(rank_item.save_data_type) && outstream.Push(rank_item.inc_id) && outstream.Push(rank_item.param1)
			&& outstream.Push(rank_item.param2) && outstream.Push(rank_item.param3);
		if (!ret)
		{
			return false;
		}

		if (rank_item.change_state == structcommon::CS_DELETE)
		{
			continue;
		}

		unsigned int length = GetCommonSaveDataLengthByType(rank_item.save_data_type);
		if (length > 0)
		{
			TLVSerializer role_data;
			role_data.Reset((void *)&rank_item.data, sizeof(rank_item.data));
			role_data.MoveCurPos(length);

			ret = outstream.Push(role_data);

			if (!ret)
			{
				return false;
			}
		}
	}

	return true;
}

bool CommonSaveDataParam::Unserialize(TLVUnserializer & instream)
{
	bool ret = instream.Pop(&count);
	if (!ret) return false;

	for (int i = 0; i < count && i < ARRAY_LENGTH(data_list); ++i)
	{
		DBCommonSaveDataItem & item = data_list[i];
		ret = instream.Pop(&item.change_state) && instream.Pop(&item.save_data_type) && instream.Pop(&item.inc_id) && instream.Pop(&item.param1)
			&& instream.Pop(&item.param2) && instream.Pop(&item.param3);

		if (!ret)
		{
			return false;
		}

		if (item.change_state == structcommon::CS_DELETE)
		{
			continue;
		}

		unsigned int length = GetCommonSaveDataLengthByType(item.save_data_type);
		if (length > 0)
		{
			TLVUnserializer role_data;
			ret = instream.Pop(&role_data);

			if (role_data.Size() <= sizeof(item.data))
			{
				memcpy(&item.data, role_data.Ptr(), role_data.Size());
			}

			if (!ret)
			{
				return false;
			}
		}
	}

	return true;
}

void CommonSaveData::Reset(int save_type)
{
	switch (save_type)
	{
		case COMMON_SAVE_DATA_TYPE_BRAVE_GROUND_TEAM_RECORD:
			{
				brave_ground_team_record_data.Reset();
			}
			break;
		case COMMON_SAVE_DATA_TYPE_BRAVE_GROUND_ROLE:
			{
				brave_ground_role_data.Reset();
			}
			break;
		case COMMON_SAVE_DATA_TYPE_HUAN_JIE_TIAN_TI_RECORD:
			{
				huan_jie_tian_ti_record_data.Reset();
			}
			break;
		case COMMON_SAVE_DATA_TYPE_PLATFORM_BATTLE_RECORD:
			{
				platform_battle_record_data.Reset();
			}
			break;
		case COMMON_SAVE_DATA_TYPE_BRAVE_GROUND_SAVE_DATA:
			{
				brave_ground_data.Reset();
			}
			break;
		case COMMON_SAVE_DATA_TYPE_HUAN_JIE_SAVE_DATA:
			{
				huan_jie_data.Reset();
			}
			break;
		case COMMON_SAVE_DATA_TYPE_GUILD_FIGHT_DATA:
			{
				guild_fight_data.Reset();
			}
			break;
		case COMMON_SAVE_DATA_TYPE_GUILD_HONOR_DATA:
			{
				guild_honor_data.Reset();
			}
			break;
		case COMMON_SAVE_DATA_TYPE_GUILD_HONOR_RECORD_OLD_DATA:
			{
				guild_honor_old_data.Reset();
			}
			break;
		case COMMON_SAVE_DATA_TYPE_PLATFORM_BATTLE_ACHIEVEMENT_DATA:
			{
				platform_battle_achievement_data.Reset();
			}
			break;
		case COMMON_SAVE_DATA_TYPE_PLATFORM_BATTLE_OTHER_DATA:
			{
				platform_battle_other_data.Reset();
			}
			break;
		case COMMON_SAVE_DATA_TYPE_GIVE_GIFT:
			{
				give_gift_data.Reset();
			}
			break;
		case COMMON_SAVE_DATA_TYPE_RA_DELAY_SET_TIME_DATA:
			{
				ra_delay_set_time_data.Reset();
			}
			break;
		case COMMON_SAVE_DATA_TYPE_GUILD_HONOR_RECORD_DATA:
			{
				guild_honor_record_data.Reset();
			}
			break;
		case COMMON_SAVE_DATA_TYPE_JIE_YI_ZU:
			{
				jie_yi_zu_data.Reset();
			}
			break;
		case COMMON_SAVE_DATA_TYPE_JIE_YI_ROLE_XUAN_YAN:
			{
				jie_yi_role_xuan_yan_data.Reset();
			}
			break;
		case COMMON_SAVE_DATA_TYPE_JIE_YI_ZU_XUAN_YAN:
			{
				jie_yi_zu_xuan_yan_data.Reset();
			}
			break;
		case COMMON_SAVE_DATA_TYPE_COW_REPORT_GOOD_NEWS_DATA:
			{
				cow_report_good_news_data.Reset();
			}
			break;
		case COMMON_SAVE_DATA_TYPE_TIGER_BRINGS_BLESSING_DATA:
			{
				tiger_brings_blessing_data.Reset();
			}
			break;
		case COMMON_SAVE_DATA_TYPE_QING_YUAN_DATA:
			{
				qing_yuan_data.Reset();
			}
			break;
		case COMMON_SAVE_DATA_TYPE_HONGMENG_TIANJI:
			{
				world_hongmeng_tianji_data.Reset();
			}
			break;
		default:
			{
				memset(this, 0, sizeof(*this));
			}
			break;
	}

	UNSTD_STATIC_CHECK(COMMON_SAVE_DATA_TYPE_MAX == 22);
}

void CommonSaveBraveGroundTeamReceordData::Reset()
{
	time = 0;
	reached_level = 0;
	record_key = 0;
	role_info_count = 0;
	for (int i = 0; i < ARRAY_LENGTH(role_info); i++)
	{
		role_info[i].role_id = 0;
		memset(role_info[i].role_name, 0, sizeof(role_info[i].role_name));
	}
}

void HuanJieZhanChangRecordRoleItem::Reset()
{
	role_id = 0;
	role_level = 0;
	rank_score = 0;
	memset(role_name, 0, sizeof(role_name));
	avatar_type = 0;
	prof = 0;
	headshot_id = 0;
	is_leader = 0;
	is_ding_ding = 0;
	reserved_ch = 0;
	reserved_sh = 0;

	shanghai = 0;
	zhi_liao = 0;
	chengshou_shanghai = 0;
}

void CommonSaveHuanJieTianTiReceordData::Reset()
{
	for (int i = 0; i < HUAN_JIE_ZHAN_CHANGTEAM_ROLE_INFO_ITEM_MAX_NUM; ++i)
	{
		role_info_1[i].Reset();
		role_info_2[i].Reset();
	}

	time = 0;
	fight_round = 0;
	fight_result = 0;
	add_score = 0;
	add_huan_jie_coin = 0;
	is_ding_ji = 0;
	is_protect = 0;
}

bool CommonSaveHuanJieTianTiReceordData::IsEmpty() const
{
	for (int i = 0; i < ARRAY_LENGTH(role_info_1); ++i)
	{
		if (role_info_1[i].role_id != 0)
		{
			return false;
		}
	}

	for (int i = 0; i < ARRAY_LENGTH(role_info_2); ++i)
	{
		if (role_info_2[i].role_id != 0)
		{
			return false;
		}
	}

	return true;
}

void CommonSavePlatformBattleRoleRecord::Reset()
{
	role_info_1.Reset();
	role_info_2.Reset();

	time = 0;
	is_win = 0;
	platform_num = 0;
	curr_rank = 0;		//当前排名
	last_rank = 0;		//上次排名
}

void CommonSavePlatformBattleRecordRoleInfo::Reset()
{
	role_id = 0;
	role_level = 0;
	memset(role_name, 0, sizeof(role_name));
	avatar_type = 0;
	prof = 0;
	headshot_id = 0;
	reserve_sh = 0;
}

void CommonSavePlatformBattleLastRankItem::Reset()
{
	platform_num = 0;
	rank = 0;
}

void CommonSavePlatformBattleReceordData::Reset()
{
	role_id = 0;
	is_first_times = 1;
	lose_record_flag = 0;
	record_index = 0;

	ArrayCallReset(records);
	ArrayCallReset(last_rank_info);
}

void CommonSavePlatformBattleAchievementItem::Reset()
{
	num = 0;
	fetch_flag = 0;
}

void CommonSavePlatformBattleAchievementData::Reset()
{
	for (int i = 0; i < ARRAY_LENGTH(achievement_info); ++i)
	{
		achievement_info[i].Reset();
	}
}

bool CommonSavePlatformBattleAchievementData::SetTypeProcess(int type, int num)
{
	if (type < 0 || type >= ARRAY_LENGTH(achievement_info))
	{
		return false;
	}

	if (achievement_info[type].num >= num)
	{
		return false;
	}

	achievement_info[type].num = num;

	return true;
}

bool CommonSavePlatformBattleAchievementData::SetTypeRankProcess(int type, int rank)
{
	if (type < 0 || type >= ARRAY_LENGTH(achievement_info))
	{
		return false;
	}

	if (rank <= 0)
	{
		return false;
	}

	if (achievement_info[type].num > 0 && achievement_info[type].num <= rank)
	{
		return false;
	}

	achievement_info[type].num = rank;

	return true;
}

void CommonSavePlatformBattleAchievementData::AddTypeProcess(int type, int num)
{
	if (type < 0 || type >= ARRAY_LENGTH(achievement_info))
	{
		return;
	}

	achievement_info[type].num += num;
}

int CommonSavePlatformBattleAchievementData::GetTypeProcessNum(int type)
{
	if (type < 0 || type >= ARRAY_LENGTH(achievement_info))
	{
		return 0;
	}

	return achievement_info[type].num;
}

CommonSavePlatformBattleAchievementItem * CommonSavePlatformBattleAchievementData::GetCommonSavePlatformBattleAchievementItem(int type)
{
	if (type < 0 || type >= ARRAY_LENGTH(achievement_info))
	{
		return 0;
	}

	return &achievement_info[type];
}


void CommonSaveBraveGroundData::Reset()
{
	memset(top_role_id, 0, sizeof(top_role_id));
}

void CommonSaveHuanJieData::Reset()
{
	for (int i = 0; i < ARRAY_LENGTH(role_id); ++i)
	{
		role_id[i] = 0;
	}
}

void CommonSaveGuildHonorRecordOldDataItem::Reset()
{
	number = 0;
	match_round = 1;
	match_round_2 = 0;
	guild_fight_rank = 0;

	guild_id = 0;
	server_id = 0;
	memset(guild_name, 0, sizeof(guild_name));
	memset(banner, 0, sizeof(banner));
	memset(patriarch_name, 0, sizeof(patriarch_name));

	champion_guess_num = 0;
	for (int i = 0; i < ARRAY_LENGTH(guess_num); ++i)
	{
		guess_num[i] = 0;
	}

	wday_flag = 0;
	for (int i = 0; i < ARRAY_LENGTH(score_records); ++i)
	{
		score_records[i] = 0;
	}
}

void CommonSaveGuildHonorRecordOldData::Reset()
{
	for (int i = 0; i < ARRAY_LENGTH(record_item); ++i)
	{
		record_item[i].Reset();
	}
}

void CommonSaveGuildFightRecord::Reset()
{
	time = 0;
	score = 0;
}

void CommonSaveGuildFightData::Reset()
{
	guild_id = 0;
	server_id = 0;
	memset(guild_name, 0, sizeof(guild_name));
	memset(banner, 0, sizeof(banner));
	memset(patriarch_name, 0, sizeof(patriarch_name));

	join_guild_fight_day = 0;
	for (int i = 0; i < ARRAY_LENGTH(records); ++i)
	{
		records[i].Reset();
	}

}

void CommonSaveGuildHonorData::Reset()
{
	guild_honor_process = GUILD_HONOR_BATTLE_PROCESS_WEEK_START;
	settlement_list_time = 0;
}


void CommonSaveGuildHonorRecordDataItem::Reset()
{
	start_first_round =1;
	win_rank = 1;
	finish_rank = 0;
	number = 0;
	match_round = 1;
	guild_fight_rank = 0;

	guild_id = 0;
	server_id = 0;
	memset(guild_name, 0, sizeof(guild_name));
	memset(banner, 0, sizeof(banner));
	memset(patriarch_name, 0, sizeof(patriarch_name));

	champion_guess_num = 0;
	for (int i = 0; i < ARRAY_LENGTH(guess_num); ++i)
	{
		guess_num[i] = 0;
	}

	wday_flag = 0;
	for (int i = 0; i < ARRAY_LENGTH(score_records); ++i)
	{
		score_records[i] = 0;
	}
}


void CommonSaveGuildHonorRecordData::Reset()
{
	for (int i = 0; i < ARRAY_LENGTH(record_item); ++i)
	{
		record_item[i].Reset();
	}
}

void CommonSavePlatformBattleOtherData::Reset()
{
	achievement_next_reset_time = 0;
	reset_version = 0;
}

void CommonSaveGiveGiftItemData::Reset()
{
	item_id = 0;
	num = 0;
	invaild_time = 0;
}

void CommonSaveGiveGiftData::Reset()
{
	uuid=0;
	from_role_id=0;
	to_role_id=0;
	fetch_time=0;
	time=0;
	memset(str, 0, sizeof(str));
	item.Reset();
	time_group_id = 0;
}

void CommonSaveCowReportGoodNewsData::Reset()
{
	curren_status = COW_REPORT_GOOD_NEWS_STATUS_INVALID;
	buy_start_times = 0;
	buy_end_times = 0;
	show_start_times = 0;
	show_end_times = 0;

	for (int i = 0; i < ARRAY_LENGTH(reward_user_info); ++i)
	{
		reward_user_info[i].Reset();
	}
}

void CommonSaveTigerBringsBlessingData::Reset()
{
	curren_status = TIGER_BRINGS_BLESSING_STATUS_INVALID;
	buy_start_times = 0;
	buy_end_times = 0;
	show_start_times = 0;
	show_end_times = 0;

	for (int i = 0; i < ARRAY_LENGTH(reward_user_info); ++i)
	{
		reward_user_info[i].Reset();
	}
}

std::string CommonSaveGiveGiftDataToString(const CommonSaveGiveGiftData & other)
{
	std::string ret = STRING_SPRINTF("uuid:%lld from_role_id:%d to_role_id:%d fetch_time:%d time:%d [item_id:%d item_num:%d invaild_time:%ud] str:%s",
					 other.uuid, other.from_role_id, other.to_role_id, other.fetch_time, other.time, other.item.item_id, other.item.num, other.item.invaild_time, other.str);

	return ret;
}

void CommonSaveJieYiZuRoleInfoItem::Reset()
{
	role_id = 0;
	memset(role_name, 0, sizeof(role_name));
	level = 0;
	avatar_type = 0;
	prof = 0;
	is_zu_zhang = 0;
	has_fisrt_member_name = 0;
	headshot_id = 0;
	join_time = 0;
	leave_end_time = 0;		// 申请离开时间, ②点击割袍断义，弹出确认弹窗，若玩家点击确认则进入24小时冷静期
	last_login_time = 0;
	memset(member_name, 0, sizeof(member_name));
	is_vote = 0;
	tan_he_number_of_votes = 0;
	memset(reserved_arr, 0, sizeof(reserved_arr));
}

void CommonSaveJieYiZu::Reset()
{
	jie_yi_zu_id=0;
	create_time=0;
	jie_yi_name.Reset();
	memset(jie_yi_text, 0, sizeof(jie_yi_text));

	for (int i = 0; i < ARRAY_LENGTH(role_info); ++i)
	{
		role_info[i].Reset();
	}

	tan_he_vote_end_time = 0;
}

void CommonSaveJieYiRoleXuanYan::Reset()
{
	role_id = 0;
	time = 0;
	memset(xuan_yan_text, 0, sizeof(xuan_yan_text));
}

void CommonSaveJieYiZuXuanYan::Reset()
{
	jie_yi_zu_id = 0;
	time = 0;
	memset(xuan_yan_text, 0, sizeof(xuan_yan_text));
}

void CommonSaveQingYuanRoleInfoItem::Reset()
{
	role_id = 0;
	ArrayFillZero(role_name);
	level = 0;	
	avatar_type=0;
	prof=0;
	headshot_id=0;
	leave_end_time=0;		
	last_login_time=0;

	gift_type = 0;
	fetch_gift_flag = 0;
	gift_type_end_time = 0;
	skill_no_effect_flag = 0;

	ArrayFillZero(reserved_arr);
}

void CommonSaveQingYuan::Reset()
{
	qing_yuan_id = 0;
	has_hun_yan = 0;
	create_time = 0;
	leave_end_time = 0;		//到时间就解除	
	qing_yuan_duration_reward_already_fetch_day = 0;
	ArrayCallReset(role_info);
	common_data.Reset();
}

void CommonSaveQingYuanCommonData::Reset()
{
	ra_ra_shizhuang_join_timestamp = 0;
	ra_qingyuan_shizhuang_buy_mark = 0;
	ra_qingyuan_leichong_begin_timestamp = 0;
	ra_qingyuan_leichong_end_timestamp = 0;
	ra_qingyuan_leichong_chongzhi_gold = 0;
}

void CommonSaveWorldHongMengTianJie::Reset()
{
	memset(first_pass, 0, sizeof(first_pass));
}
