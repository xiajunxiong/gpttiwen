#include "roleinitparam.h"
#include "servercommon/servercommon.h"

bool RoleInitParam::Serialize(TLVSerializer &outstream) const
{
	bool ret = outstream.Pushf(
		"isbbbhhiiiiiii"
		"hiiiiiik"
		"kkkkki"
		"iikkiikiii"
		"i",
		role_id, role_name, authority_type, vip_level, color, avatar_type, headshot_id, top_level, profession, next_top_level_refresh_timestamp, level, scene_id, scene_x, scene_y,
		role_fly_flag, last_scene_id, last_scene_x, last_scene_y, online_time, today_online_time, lastday_online_time, create_time,
		coin_bind, immortal_coin, gold, history_recharge_gold, forbid_talk_time, forbid_talk_type,
		last_online_dayid, last_reset_module_timestamp, exp, max_exp, capability, max_capability, avatar_timestamp, plat_spid, weapon_id, account_user_num,
		mini_game_id
	);

	return ret;
}

bool RoleInitParam::Unserialize(TLVUnserializer &instream)
{
	const char *rolename_tmp = NULL;

	bool ret = instream.Popf(
		"isbbbhhiiiiiii"
		"hiiiiiik"
		"kkkkki"
		"iikkiikiii"
		"i",
		&role_id, &rolename_tmp, &authority_type, &vip_level, &color, &avatar_type, &headshot_id, &top_level, &profession, &next_top_level_refresh_timestamp, &level, &scene_id, &scene_x, &scene_y,
		&role_fly_flag, &last_scene_id, &last_scene_x, &last_scene_y, &online_time, &today_online_time, &lastday_online_time, &create_time,
		&coin_bind, &immortal_coin, &gold, &history_recharge_gold, &forbid_talk_time, &forbid_talk_type,
		&last_online_dayid, &last_reset_module_timestamp, &exp, &max_exp, &capability, &max_capability, &avatar_timestamp, &plat_spid, &weapon_id, &account_user_num,
		&mini_game_id);

	STRNCPY(role_name, rolename_tmp, sizeof(role_name));
	
	return ret;
}