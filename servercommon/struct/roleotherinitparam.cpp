#include "roleotherinitparam.hpp"

#define SERIALIZE_OTHER_PARAM(SERIALIZER, PARAM)					\
{																	\
	SERIALIZER.Reset((void*)&PARAM, sizeof(PARAM));					\
	SERIALIZER.MoveCurPos(sizeof(PARAM));							\
}																	

#define DESERIALIZE_OTHER_PARAM(DESERIALIZER, PARAM)				\
{																	\
	if (DESERIALIZER.Size() <= sizeof(PARAM))						\
	{																\
		memcpy(&PARAM, DESERIALIZER.Ptr(), DESERIALIZER.Size());	\
	}																\
	else                                                            \
	{                                                               \
		printf("DESERIALIZE %s.Size() > sizeof(%s) error\n", #DESERIALIZER, #PARAM); \
		return false;                                               \
	}                                                               \
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool RoleOtherInitParam::Serialize(TLVSerializer &outstream) const
{
	TLVSerializer monitor_data;
	monitor_data.Reset((void*)&monitor_param, sizeof(monitor_param));
	monitor_data.MoveCurPos(sizeof(monitor_param));

	TLVSerializer common_data;
	SERIALIZE_OTHER_PARAM(common_data, common_data_param);

	TLVSerializer vip_data;
	SERIALIZE_OTHER_PARAM(vip_data, vip_param);

	TLVSerializer attribute_plan_data;
	SERIALIZE_OTHER_PARAM(attribute_plan_data, attribute_plan_param);

	TLVSerializer money_other_data;
	SERIALIZE_OTHER_PARAM(money_other_data, money_other_param);

	TLVSerializer knapsack_other_data;
	SERIALIZE_OTHER_PARAM(knapsack_other_data, knapsack_other_param);

	TLVSerializer prestige_data;
	SERIALIZE_OTHER_PARAM(prestige_data, prestige_param);

	TLVSerializer collection_data;
	SERIALIZE_OTHER_PARAM(collection_data, collection_param);

	TLVSerializer partner_data;
	SERIALIZE_OTHER_PARAM(partner_data, partner_param);

	TLVSerializer title_data;
	SERIALIZE_OTHER_PARAM(title_data, title_param);

	TLVSerializer train_data;
	SERIALIZE_OTHER_PARAM(train_data, train_param);

	TLVSerializer task_recorder_data;
	task_recorder_data.Reset((void*)taskrecord_list, MAX_TASK_RECORD_LEN);
	task_recorder_data.MoveCurPos(taskrecrod_len);

	TLVSerializer task_manager_data;
	SERIALIZE_OTHER_PARAM(task_manager_data, task_param);

	TLVSerializer skill_other_data;
	SERIALIZE_OTHER_PARAM(skill_other_data, skill_other_param);

	TLVSerializer equip_other_data;
	SERIALIZE_OTHER_PARAM(equip_other_data, equip_pther_param);

	TLVSerializer workshop_data;
	SERIALIZE_OTHER_PARAM(workshop_data, workshop_param);

	TLVSerializer surface_data;
	SERIALIZE_OTHER_PARAM(surface_data, surface_param);

	TLVSerializer role_baby_data;
	SERIALIZE_OTHER_PARAM(role_baby_data, role_baby_param);

	TLVSerializer marriage_data;
	SERIALIZE_OTHER_PARAM(marriage_data, marry_param);

	TLVSerializer friend_common_data;
	SERIALIZE_OTHER_PARAM(friend_common_data, friend_common_param);

	TLVSerializer guild_common_data;
	SERIALIZE_OTHER_PARAM(guild_common_data, guild_common_param);

	TLVSerializer role_system_set_data;
	SERIALIZE_OTHER_PARAM(role_system_set_data, role_system_set_param);

	TLVSerializer shi_tu_data;
	SERIALIZE_OTHER_PARAM(shi_tu_data, shi_tu_param);

	TLVSerializer courage_challenge_data;
	SERIALIZE_OTHER_PARAM(courage_challenge_data, courage_challenge_param);

	TLVSerializer monster_wave_data;
	SERIALIZE_OTHER_PARAM(monster_wave_data, monster_wave_param);

	TLVSerializer mi_yi_data;
	SERIALIZE_OTHER_PARAM(mi_yi_data, mi_yi_param);

	TLVSerializer school_task_data;
	SERIALIZE_OTHER_PARAM(school_task_data, school_task_param);

	TLVSerializer role_activity_common_data;
	SERIALIZE_OTHER_PARAM(role_activity_common_data, roleactivity_common_param);

	TLVSerializer treasure_map_data;
	SERIALIZE_OTHER_PARAM(treasure_map_data, treasure_map_param);

	TLVSerializer task_chain_data;
	SERIALIZE_OTHER_PARAM(task_chain_data, task_chain_param);

	TLVSerializer fabao_data;
	SERIALIZE_OTHER_PARAM(fabao_data, fabao_param);

	TLVSerializer research_task_data;
	SERIALIZE_OTHER_PARAM(research_task_data, research_task_param);

	TLVSerializer medal_data;
	SERIALIZE_OTHER_PARAM(medal_data, medal_param);

	TLVSerializer role_crystal_fb_data;
	SERIALIZE_OTHER_PARAM(role_crystal_fb_data, role_crystal_fb);

	TLVSerializer buff_data;
	SERIALIZE_OTHER_PARAM(buff_data, buff_param);

	TLVSerializer shop_data;
	SERIALIZE_OTHER_PARAM(shop_data, shop_param);

	TLVSerializer task_trace_data;
	SERIALIZE_OTHER_PARAM(task_trace_data, task_trace_param);

	TLVSerializer map_unlock_data;
	SERIALIZE_OTHER_PARAM(map_unlock_data, map_unlock_param);

	TLVSerializer gather_fb_data;
	SERIALIZE_OTHER_PARAM(gather_fb_data, role_gather_fb_param);

	TLVSerializer exchange_data;
	SERIALIZE_OTHER_PARAM(exchange_data, exchange_param);

	TLVSerializer rolecross_data;
	SERIALIZE_OTHER_PARAM(rolecross_data, rolecross_param);

	TLVSerializer course_of_growth_data;
	SERIALIZE_OTHER_PARAM(course_of_growth_data, course_of_growth_param);

	TLVSerializer salary_data;
	SERIALIZE_OTHER_PARAM(salary_data, salary_param);

	TLVSerializer cog_record_data;
	cog_record_data.Reset((void*)course_of_growth_list, MAX_COG_RECORD_LEN);
	cog_record_data.MoveCurPos(course_of_growth_len);

	TLVSerializer challenge_fb_data;
	SERIALIZE_OTHER_PARAM(challenge_fb_data, challenge_fb_param);

	TLVSerializer face_score_data;
	SERIALIZE_OTHER_PARAM(face_score_data, face_score_param);

	TLVSerializer face_score_record_data;
	face_score_record_data.Reset((void*)face_score_record_list, MAX_FACE_RECORD_LEN);
	face_score_record_data.MoveCurPos(face_score_record_len);

	TLVSerializer appearance_data;
	SERIALIZE_OTHER_PARAM(appearance_data, appearance_param);

	TLVSerializer func_guide_data;
	SERIALIZE_OTHER_PARAM(func_guide_data, func_guide_param);

	TLVSerializer convert_shop_data;
	SERIALIZE_OTHER_PARAM(convert_shop_data, convert_shop_param);

	TLVSerializer special_appearance_data;
	SERIALIZE_OTHER_PARAM(special_appearance_data, special_appearance_param);

	TLVSerializer role_drop_data;
	SERIALIZE_OTHER_PARAM(role_drop_data, role_drop_param);

	TLVSerializer monster_group_drop_data;
	SERIALIZE_OTHER_PARAM(monster_group_drop_data, monster_group_drop_param);

	TLVSerializer role_login_system_data;
	SERIALIZE_OTHER_PARAM(role_login_system_data, role_login_system_param);

	TLVSerializer boss_chapter_data;
	SERIALIZE_OTHER_PARAM(boss_chapter_data, boss_chapter_param);

	TLVSerializer role_mysterious_trial_data;
	SERIALIZE_OTHER_PARAM(role_mysterious_trial_data, role_mysterious_trial_param);

	TLVSerializer bounty_task_data;
	SERIALIZE_OTHER_PARAM(bounty_task_data, bounty_task_param);

	TLVSerializer role_transaction_task_data;
	SERIALIZE_OTHER_PARAM(role_transaction_task_data, role_transaction_task_param);

	TLVSerializer pet_common_data;
	SERIALIZE_OTHER_PARAM(pet_common_data, pet_common_param);

	TLVSerializer role_active_system_data;
	SERIALIZE_OTHER_PARAM(role_active_system_data, role_active_system_param);

	TLVSerializer role_chosed_data;
	SERIALIZE_OTHER_PARAM(role_chosed_data, role_chosed_param);

	TLVSerializer role_huashen_data;
	SERIALIZE_OTHER_PARAM(role_huashen_data, role_huashen_param);

	TLVSerializer func_preview_data;
	SERIALIZE_OTHER_PARAM(func_preview_data, func_preview_param);

	TLVSerializer auto_catch_pet_data;
	SERIALIZE_OTHER_PARAM(auto_catch_pet_data, auto_catch_pet_param);

	TLVSerializer trade_credit_data;
	SERIALIZE_OTHER_PARAM(trade_credit_data, trade_credit_param);

	TLVSerializer pet_skin_data;
	SERIALIZE_OTHER_PARAM(pet_skin_data, pet_skin_param);

	TLVSerializer face_score_time_data;
	SERIALIZE_OTHER_PARAM(face_score_time_data, face_score_time_param);

	TLVSerializer formation_data;
	SERIALIZE_OTHER_PARAM(formation_data, formation_param);

	TLVSerializer gather_dream_data;
	SERIALIZE_OTHER_PARAM(gather_dream_data, gather_dream_param);

	TLVSerializer role_huan_jie_data;
	SERIALIZE_OTHER_PARAM(role_huan_jie_data, role_huan_jie_param);

	TLVSerializer anecdote_task_data;
	SERIALIZE_OTHER_PARAM(anecdote_task_data, anecdote_task_param);

	TLVSerializer anecdote_task_common_data;
	SERIALIZE_OTHER_PARAM(anecdote_task_common_data, anecdote_task_common_param);

	TLVSerializer task_wenxianglou_data;
	SERIALIZE_OTHER_PARAM(task_wenxianglou_data, task_wenxianglou_param);

	TLVSerializer dream_notes_challenge_data;
	SERIALIZE_OTHER_PARAM(dream_notes_challenge_data, dream_notes_challenge_param);

	TLVSerializer school_exercise_data;
	SERIALIZE_OTHER_PARAM(school_exercise_data, school_exercise_param);

	TLVSerializer pet_auto_move_data;
	SERIALIZE_OTHER_PARAM(pet_auto_move_data, pet_auto_move_param);

	TLVSerializer smart_mounts_system_data;
	SERIALIZE_OTHER_PARAM(smart_mounts_system_data, smart_mounts_system_param);

	TLVSerializer realm_break_data;
	SERIALIZE_OTHER_PARAM(realm_break_data, realm_break_param);

	TLVSerializer pet_monopoly_data;
	SERIALIZE_OTHER_PARAM(pet_monopoly_data, old_pet_monopoly_param);

	TLVSerializer lun_hui_wang_chuan_data;
	SERIALIZE_OTHER_PARAM(lun_hui_wang_chuan_data, lun_hui_wang_chuan_param);

	TLVSerializer role_material_data;
	SERIALIZE_OTHER_PARAM(role_material_data, role_material_param);

	TLVSerializer fabao_draw_data;
	SERIALIZE_OTHER_PARAM(fabao_draw_data, fabao_draw_param);

	TLVSerializer map_exploration_data;
	SERIALIZE_OTHER_PARAM(map_exploration_data, map_exploration_param);

	TLVSerializer role_join_activity_reward_data;
	SERIALIZE_OTHER_PARAM(role_join_activity_reward_data, role_join_activity_reward_param);
	
	TLVSerializer custom_instruction_data;
	SERIALIZE_OTHER_PARAM(custom_instruction_data, custom_instruction_param);

	TLVSerializer role_guild_inborn_data;
	SERIALIZE_OTHER_PARAM(role_guild_inborn_data, role_guild_inborn_param);

	TLVSerializer role_new_partner_data;
	SERIALIZE_OTHER_PARAM(role_new_partner_data, new_partner_param);

	TLVSerializer heart_skill_data;
	SERIALIZE_OTHER_PARAM(heart_skill_data, m_heart_skill_param);

	TLVSerializer pet_helper_formation_data;
	SERIALIZE_OTHER_PARAM(pet_helper_formation_data, pet_helper_formation_param);

	TLVSerializer god_print_bag_data;
	SERIALIZE_OTHER_PARAM(god_print_bag_data, god_print_bag_param);

	TLVSerializer god_print_log_data;
	SERIALIZE_OTHER_PARAM(god_print_log_data, god_print_log_param);

	TLVSerializer nether_world_gatory_data;
	SERIALIZE_OTHER_PARAM(nether_world_gatory_data, nether_world_gatory_param);

	TLVSerializer pet_god_fb_data;
	SERIALIZE_OTHER_PARAM(pet_god_fb_data, pet_god_fb_param);

	TLVSerializer week_base_data;
	SERIALIZE_OTHER_PARAM(week_base_data, week_base_param);

	TLVSerializer week_task_data;
	SERIALIZE_OTHER_PARAM(week_task_data, week_task_param);

	TLVSerializer role_tian_di_dao_hen_data;
	SERIALIZE_OTHER_PARAM(role_tian_di_dao_hen_data, tian_di_dao_hen_param);

	TLVSerializer prestige_task_data;
	SERIALIZE_OTHER_PARAM(prestige_task_data, prestige_task_param);

	TLVSerializer partner_gift_tree_data;
	SERIALIZE_OTHER_PARAM(partner_gift_tree_data, partner_gift_tree_param);

	TLVSerializer big_dipper_data;
	SERIALIZE_OTHER_PARAM(big_dipper_data, big_dipper_param);

	TLVSerializer role_attr_pellet_data;
	SERIALIZE_OTHER_PARAM(role_attr_pellet_data, role_attr_pellet_param);

	TLVSerializer active_turntable_data;
	SERIALIZE_OTHER_PARAM(active_turntable_data, active_turntable_param);

	TLVSerializer partner_pact_data;
	SERIALIZE_OTHER_PARAM(partner_pact_data, partner_pact_param);

	TLVSerializer duel_data;
	SERIALIZE_OTHER_PARAM(duel_data, duel_param);

	TLVSerializer tian_ming_data;
	SERIALIZE_OTHER_PARAM(tian_ming_data, tian_ming_param);

	TLVSerializer du_jie_data;
	SERIALIZE_OTHER_PARAM(du_jie_data, du_jie_param);

	TLVSerializer yin_yang_eye_data;
	SERIALIZE_OTHER_PARAM(yin_yang_eye_data, yin_yang_eye_param);

	TLVSerializer big_dipper_help_data;
	SERIALIZE_OTHER_PARAM(big_dipper_help_data, big_dipper_help_param);

	TLVSerializer role_course_data;
	SERIALIZE_OTHER_PARAM(role_course_data, role_course_param);

	TLVSerializer advance_career_data;
	SERIALIZE_OTHER_PARAM(advance_career_data, advance_career_param);

	TLVSerializer wan_ling_fb_data;
	SERIALIZE_OTHER_PARAM(wan_ling_fb_data, wan_ling_fb_param);

	TLVSerializer role_new_life_skill_data;
	SERIALIZE_OTHER_PARAM(role_new_life_skill_data, role_new_life_skill_param);

	TLVSerializer role_fly_up_data;
	SERIALIZE_OTHER_PARAM(role_fly_up_data, role_fly_up_param);

	TLVSerializer role_mirage_territory_data;
	SERIALIZE_OTHER_PARAM(role_mirage_territory_data, role_mirage_territory_param);

	TLVSerializer xiu_wei_data;
	SERIALIZE_OTHER_PARAM(xiu_wei_data, xiu_wei_param);

	TLVSerializer treasure_task_data;
	SERIALIZE_OTHER_PARAM(treasure_task_data, treasure_task_param);

	TLVSerializer role_pet_info_data;
	SERIALIZE_OTHER_PARAM(role_pet_info_data, role_pet_info_param);

	TLVSerializer fallen_god_data;
	SERIALIZE_OTHER_PARAM(fallen_god_data, fallen_god_param);

	TLVSerializer role_partner_info_data;
	SERIALIZE_OTHER_PARAM(role_partner_info_data, role_partner_info_param);

	TLVSerializer fallen_god_help_data;
	SERIALIZE_OTHER_PARAM(fallen_god_help_data, fallen_god_help_param);

	TLVSerializer mini_game_data;
	SERIALIZE_OTHER_PARAM(mini_game_data, mini_game_param);

	TLVSerializer role_mirage_territory_info_data;
	SERIALIZE_OTHER_PARAM(role_mirage_territory_info_data, role_mirage_territory_info_param);

	TLVSerializer role_peak_token_data;
	SERIALIZE_OTHER_PARAM(role_peak_token_data, role_peak_token_param);

	TLVSerializer role_income_data;
	SERIALIZE_OTHER_PARAM(role_income_data, role_income_param);

	TLVSerializer new_level_complement_data;
	SERIALIZE_OTHER_PARAM(new_level_complement_data, new_level_complement_param);

	TLVSerializer new_pet_monopoly_data;
	SERIALIZE_OTHER_PARAM(new_pet_monopoly_data, new_pet_monopoly_param);

	TLVSerializer yao_xing_shi_lian_data;
	SERIALIZE_OTHER_PARAM(yao_xing_shi_lian_data, yao_xing_shi_lian_param);

	TLVSerializer lantern_riddie_data;
	SERIALIZE_OTHER_PARAM(lantern_riddie_data, lantern_riddie_param);

	TLVSerializer lovers_market_data;
	SERIALIZE_OTHER_PARAM(lovers_market_data, lovers_market_param);

	TLVSerializer tao_quan_na_ji_data;
	SERIALIZE_OTHER_PARAM(tao_quan_na_ji_data, tao_quan_na_ji_param);

	TLVSerializer concentric_knot_data;
	SERIALIZE_OTHER_PARAM(concentric_knot_data, concentric_knot_param);
	
	TLVSerializer role_hongmeng_tianjie_data;
	SERIALIZE_OTHER_PARAM(role_hongmeng_tianjie_data, role_hongmeng_tianjie_param);

	TLVSerializer zao_hua_refine_data;
	SERIALIZE_OTHER_PARAM(zao_hua_refine_data, zao_hua_refine_param);

	TLVSerializer zao_hua_long_zhu_data;
	SERIALIZE_OTHER_PARAM(zao_hua_long_zhu_data, zao_hua_long_zhu_param);

	TLVSerializer role_holy_equip_data;
	SERIALIZE_OTHER_PARAM(role_holy_equip_data, role_holy_equip_common_param);

	TLVSerializer item_drop_cooldown_data;
	SERIALIZE_OTHER_PARAM(item_drop_cooldown_data, item_cool_down_drop_param);

	TLVSerializer role_ke_ju_exam_data;
	SERIALIZE_OTHER_PARAM(role_ke_ju_exam_data, role_ke_ju_exam_param);

	TLVSerializer pet_strengthen_buff_data;
	SERIALIZE_OTHER_PARAM(pet_strengthen_buff_data, pet_strengthen_buff_param);

	TLVSerializer element_elf_data;
	SERIALIZE_OTHER_PARAM(element_elf_data, element_elf_param);

	TLVSerializer fu_yu_common_data;
	SERIALIZE_OTHER_PARAM(fu_yu_common_data, fu_yu_common_param);

	TLVSerializer role_kua_fu_ying_xiong_lu_data;
	SERIALIZE_OTHER_PARAM(role_kua_fu_ying_xiong_lu_data, role_kua_fu_ying_xiong_lu_param);

	TLVSerializer role_day_reward_data;
	SERIALIZE_OTHER_PARAM(role_day_reward_data, role_day_reward_param);

	TLVSerializer star_chart_data;
	SERIALIZE_OTHER_PARAM(star_chart_data, star_chart_param);


	TLVSerializer new_courage_challenge_data;
	SERIALIZE_OTHER_PARAM(new_courage_challenge_data, new_courage_challenge_param);

	TLVSerializer fashion_collection_data;
	SERIALIZE_OTHER_PARAM(fashion_collection_data, fashion_collection_param);

	TLVSerializer new_god_print_log_data;
	SERIALIZE_OTHER_PARAM(new_god_print_log_data, new_god_print_log_param);

	TLVSerializer role_guild_task_data;
	SERIALIZE_OTHER_PARAM(role_guild_task_data, role_guild_task_param);

	TLVSerializer special_anecdote_data;
	SERIALIZE_OTHER_PARAM(special_anecdote_data, special_anecdote_param);

	TLVSerializer role_huan_shou_data;
	SERIALIZE_OTHER_PARAM(role_huan_shou_data, role_huan_shou_param);

	bool ret = outstream.Push(monitor_data) &&
		outstream.Push(common_data) &&
		outstream.Push(vip_data) &&
		outstream.Push(attribute_plan_data) &&
		outstream.Push(money_other_data) &&
		outstream.Push(knapsack_other_data) &&
		outstream.Push(prestige_data) &&
		outstream.Push(collection_data) &&
		outstream.Push(partner_data) &&
		outstream.Push(title_data) &&
		outstream.Push(train_data) &&
		outstream.Push(task_recorder_data) &&
		outstream.Push(task_manager_data) &&
		outstream.Push(skill_other_data) &&
		outstream.Push(equip_other_data) &&
		outstream.Push(workshop_data) &&
		outstream.Push(surface_data) && 
		outstream.Push(role_baby_data) &&
		outstream.Push(marriage_data) &&
		outstream.Push(friend_common_data) &&
		outstream.Push(guild_common_data) &&
		outstream.Push(role_system_set_data) &&
		outstream.Push(shi_tu_data) &&
		outstream.Push(courage_challenge_data) &&
		outstream.Push(monster_wave_data) && 
		outstream.Push(mi_yi_data) &&
		outstream.Push(school_task_data) &&
		outstream.Push(role_activity_common_data) &&
		outstream.Push(treasure_map_data) &&
		outstream.Push(task_chain_data) &&
		outstream.Push(fabao_data) &&
		outstream.Push(research_task_data) &&
		outstream.Push(medal_data) &&
		outstream.Push(role_crystal_fb_data) &&
		outstream.Push(buff_data) &&
		outstream.Push(shop_data) &&
		outstream.Push(task_trace_data) &&
		outstream.Push(map_unlock_data) &&
		outstream.Push(gather_fb_data) &&
		outstream.Push(exchange_data) &&
		outstream.Push(rolecross_data) &&
		outstream.Push(course_of_growth_data) &&
		outstream.Push(salary_data) &&
		outstream.Push(cog_record_data) &&
		outstream.Push(challenge_fb_data) &&
		outstream.Push(face_score_data) &&
		outstream.Push(face_score_record_data) &&
		outstream.Push(appearance_data) &&
		outstream.Push(func_guide_data) &&
		outstream.Push(convert_shop_data) &&
		outstream.Push(special_appearance_data) &&
		outstream.Push(role_drop_data) &&
		outstream.Push(monster_group_drop_data) &&
		outstream.Push(role_login_system_data) &&
		outstream.Push(boss_chapter_data) &&
		outstream.Push(role_mysterious_trial_data) &&
		outstream.Push(bounty_task_data) &&
		outstream.Push(role_transaction_task_data) &&
		outstream.Push(pet_common_data) &&
		outstream.Push(role_active_system_data) &&
		outstream.Push(role_chosed_data) && 
		outstream.Push(role_huashen_data) &&
		outstream.Push(func_preview_data) &&
		outstream.Push(auto_catch_pet_data) &&
		outstream.Push(trade_credit_data) &&
		outstream.Push(pet_skin_data) &&
		outstream.Push(face_score_time_data) &&
		outstream.Push(formation_data) &&
		outstream.Push(gather_dream_data) &&
		outstream.Push(role_huan_jie_data) &&
		outstream.Push(anecdote_task_data) &&
		outstream.Push(anecdote_task_common_data) &&
		outstream.Push(task_wenxianglou_data) &&
		outstream.Push(dream_notes_challenge_data) &&
		outstream.Push(school_exercise_data) &&
		outstream.Push(pet_auto_move_data) &&
		outstream.Push(smart_mounts_system_data) &&
		outstream.Push(realm_break_data) &&
		outstream.Push(pet_monopoly_data) &&
		outstream.Push(lun_hui_wang_chuan_data) &&
		outstream.Push(role_material_data) &&
		outstream.Push(fabao_draw_data) &&
		outstream.Push(map_exploration_data) &&
		outstream.Push(role_join_activity_reward_data) &&
		outstream.Push(custom_instruction_data) &&
		outstream.Push(role_guild_inborn_data) &&
		outstream.Push(role_new_partner_data) &&
		outstream.Push(heart_skill_data) &&
		outstream.Push(pet_helper_formation_data) &&
		outstream.Push(god_print_bag_data) &&
		outstream.Push(god_print_log_data) &&
		outstream.Push(nether_world_gatory_data) &&
		outstream.Push(pet_god_fb_data) &&
		outstream.Push(week_base_data) &&
		outstream.Push(week_task_data) &&
		outstream.Push(role_tian_di_dao_hen_data) &&
		outstream.Push(prestige_task_data) &&
		outstream.Push(partner_gift_tree_data) &&
		outstream.Push(big_dipper_data) &&
		outstream.Push(role_attr_pellet_data) &&
		outstream.Push(active_turntable_data) &&
		outstream.Push(partner_pact_data) &&
		outstream.Push(duel_data) &&
		outstream.Push(tian_ming_data) &&
		outstream.Push(du_jie_data) &&
		outstream.Push(yin_yang_eye_data) &&
		outstream.Push(big_dipper_help_data) &&
		outstream.Push(role_course_data) &&
		outstream.Push(advance_career_data) &&
		outstream.Push(wan_ling_fb_data) &&
		outstream.Push(role_new_life_skill_data) &&
		outstream.Push(role_fly_up_data) &&
		outstream.Push(role_mirage_territory_data) &&
		outstream.Push(xiu_wei_data) &&
		outstream.Push(treasure_task_data) && 
		outstream.Push(role_pet_info_data) &&
		outstream.Push(role_partner_info_data) &&
		outstream.Push(fallen_god_data) &&
		outstream.Push(fallen_god_help_data) &&
		outstream.Push(mini_game_data) &&
		outstream.Push(role_mirage_territory_info_data) &&
		outstream.Push(role_peak_token_data) &&
		outstream.Push(role_income_data) &&
		outstream.Push(new_level_complement_data) &&
		outstream.Push(new_pet_monopoly_data) &&
		outstream.Push(yao_xing_shi_lian_data) &&
		outstream.Push(lantern_riddie_data) &&
		outstream.Push(lovers_market_data) &&
		outstream.Push(tao_quan_na_ji_data) &&
		outstream.Push(concentric_knot_data) &&
		outstream.Push(role_hongmeng_tianjie_data) &&
		outstream.Push(role_holy_equip_data)
		&& outstream.Push(zao_hua_refine_data)
		&& outstream.Push(zao_hua_long_zhu_data)
		&& outstream.Push(item_drop_cooldown_data)
		&& outstream.Push(role_ke_ju_exam_data)
		&& outstream.Push(pet_strengthen_buff_data)
		&& outstream.Push(element_elf_data)
		&&	outstream.Push(fu_yu_common_data)
		&& outstream.Push(role_kua_fu_ying_xiong_lu_data)
		&& outstream.Push(role_day_reward_data)
		&& outstream.Push(star_chart_data)
		&& outstream.Push(new_courage_challenge_data)
		&& outstream.Push(fashion_collection_data)
		&& outstream.Push(new_god_print_log_data)
		&& outstream.Push(role_guild_task_data)
		&& outstream.Push(special_anecdote_data)
		&& outstream.Push(role_huan_shou_data)
	;

	if (!ret)
	{
		return false;
	}

	return true;
}

bool RoleOtherInitParam::Unserialize(TLVUnserializer &instream)
{
	TLVUnserializer monitor_data;
	TLVUnserializer common_data;
	TLVUnserializer vip_data;
	TLVUnserializer attribute_plan_data;
	TLVUnserializer money_other_data;
	TLVUnserializer knapsack_other_data;
	TLVUnserializer prestige_data;
	TLVUnserializer collection_data;
	TLVUnserializer partner_data;
	TLVUnserializer title_data;
	TLVUnserializer train_data;
	TLVUnserializer task_recorder_data;
	TLVUnserializer task_manager_data;
	TLVUnserializer skill_other_data;
	TLVUnserializer equip_other_data;
	TLVUnserializer workshop_data;
	TLVUnserializer surface_data;
	TLVUnserializer role_baby_data;
	TLVUnserializer marriage_data;
	TLVUnserializer friend_common_data;
	TLVUnserializer guild_common_data;
	TLVUnserializer role_system_set_data;
	TLVUnserializer shi_tu_data;
	TLVUnserializer courage_challenge_data;
	TLVUnserializer monster_wave_data;
	TLVUnserializer mi_yi_data;
	TLVUnserializer school_task_data;
	TLVUnserializer role_activity_common_data;
	TLVUnserializer treasure_map_data;
	TLVUnserializer task_chain_data;
	TLVUnserializer fabao_data;
	TLVUnserializer research_task_data;
	TLVUnserializer medal_data;
	TLVUnserializer role_crystal_fb_data;
	TLVUnserializer buff_data;
	TLVUnserializer shop_data;
	TLVUnserializer task_trace_data;
	TLVUnserializer map_unlock_data;
	TLVUnserializer gather_fb_data;
	TLVUnserializer exchange_data;
	TLVUnserializer rolecross_data;
	TLVUnserializer course_of_growth_data;
	TLVUnserializer salary_data;
	TLVUnserializer cog_record_data;
	TLVUnserializer challenge_fb_data;
	TLVUnserializer face_score_data;
	TLVUnserializer face_score_record_data;
	TLVUnserializer appearance_data;
	TLVUnserializer func_guide_data;
	TLVUnserializer convert_shop_data;
	TLVUnserializer special_appearance_data;
	TLVUnserializer role_drop_data;
	TLVUnserializer monster_group_drop_data;
	TLVUnserializer role_login_system_data;
	TLVUnserializer boss_chapter_data;
	TLVUnserializer role_mysterious_trial_data;
	TLVUnserializer bounty_task_data;
	TLVUnserializer role_transaction_task_data;
	TLVUnserializer pet_common_data;
	TLVUnserializer role_active_system_data;	
	TLVUnserializer role_chosed_data;
	TLVUnserializer role_huashen_data;
	TLVUnserializer func_preview_data;
	TLVUnserializer auto_catch_pet_data;
	TLVUnserializer trade_credit_data;
	TLVUnserializer pet_skin_data;
	TLVUnserializer face_score_time_data;
	TLVUnserializer formation_data;
	TLVUnserializer gather_dream_data;
	TLVUnserializer role_huan_jie_data;
	TLVUnserializer anecdote_task_data;
	TLVUnserializer anecdote_task_common_data;
	TLVUnserializer task_wenxianglou_data;
	TLVUnserializer dream_notes_challenge_data;
	TLVUnserializer school_exercise_data;
	TLVUnserializer pet_auto_move_data;
	TLVUnserializer smart_mounts_system_data;
	TLVUnserializer realm_break_data;
	TLVUnserializer pet_monopoly_data;
	TLVUnserializer lun_hui_wang_chuan_data;
	TLVUnserializer role_material_data;
	TLVUnserializer fabao_draw_data;
	TLVUnserializer map_exploration_data;
	TLVUnserializer role_join_activity_reward_data;
	TLVUnserializer custom_instruction_data;
	TLVUnserializer role_guild_inborn_data;
	TLVUnserializer role_new_partner_data;
	TLVUnserializer heart_skill_data;
	TLVUnserializer pet_helper_formation_data;
	TLVUnserializer god_print_bag_data;
	TLVUnserializer god_print_log_data;
	TLVUnserializer nether_world_gatory_data;
	TLVUnserializer pet_god_fb_data;
	TLVUnserializer week_base_data;
	TLVUnserializer week_task_data;
	TLVUnserializer role_tian_di_dao_hen;
	TLVUnserializer prestige_task_data;
	TLVUnserializer partner_gift_tree_data;
	TLVUnserializer big_dipper_data;
	TLVUnserializer role_attr_pellet_data;
	TLVUnserializer active_turntable_data;
	TLVUnserializer partner_pact_data;
	TLVUnserializer duel_data;
	TLVUnserializer tian_ming_data;
	TLVUnserializer du_jie_data;
	TLVUnserializer yin_yang_eye_data;
	TLVUnserializer big_dipper_help_data;
	TLVUnserializer role_course_data;
	TLVUnserializer advance_career_data;
	TLVUnserializer wan_ling_fb_data;

	TLVUnserializer role_new_life_skill_data;
	TLVUnserializer role_fly_up_data;
	TLVUnserializer role_mirage_territory_data;
	TLVUnserializer xiu_wei_data;
	TLVUnserializer treasure_task_data;
	TLVUnserializer role_pet_info_data;
	TLVUnserializer role_partner_info_data;

	TLVUnserializer fallen_god_data;
	TLVUnserializer fallen_god_help_data;

	TLVUnserializer mini_game_data;
	TLVUnserializer role_mirage_territory_info_data;
	TLVUnserializer role_peak_token_data;
	TLVUnserializer role_income_data;
	TLVUnserializer new_level_complement_data;
	TLVUnserializer new_pet_monopoly_data;
	TLVUnserializer yao_xing_shi_lian_data;
	TLVUnserializer lantern_riddie_data;
	TLVUnserializer lovers_market_data;
	TLVUnserializer tao_quan_na_ji_data;
	TLVUnserializer concentric_knot_data;
	TLVUnserializer role_hongmeng_tianjie_data;
	TLVUnserializer zao_hua_refine_data;
	TLVUnserializer zao_hua_long_zhu_data;
	TLVUnserializer role_holy_equip_data;
	TLVUnserializer item_drop_cooldown_data;
	TLVUnserializer role_ke_ju_exam_data;
	TLVUnserializer pet_strengthen_buff_data;
	TLVUnserializer element_elf_data;
	TLVUnserializer fu_yu_common_data;
	TLVUnserializer role_kua_fu_ying_xiong_lu_data;
	TLVUnserializer role_day_reward_data;
	TLVUnserializer star_chart_data;
	TLVUnserializer new_courage_challenge_data;
	TLVUnserializer fashion_collection_data;
	TLVUnserializer new_god_print_log_data;
	TLVUnserializer role_guild_task_data;
	TLVUnserializer special_anecdote_data;
	TLVUnserializer role_huan_shou_data;

	bool ret = instream.Pop(&monitor_data) &&
		instream.Pop(&common_data) &&
		instream.Pop(&vip_data) &&
		instream.Pop(&attribute_plan_data) &&
		instream.Pop(&money_other_data) &&
		instream.Pop(&knapsack_other_data) &&
		instream.Pop(&prestige_data) &&
		instream.Pop(&collection_data) &&
		instream.Pop(&partner_data) &&
		instream.Pop(&title_data) &&
		instream.Pop(&train_data) &&
		instream.Pop(&task_recorder_data) &&
		instream.Pop(&task_manager_data) &&
		instream.Pop(&skill_other_data) &&
		instream.Pop(&equip_other_data) &&
		instream.Pop(&workshop_data) &&
		instream.Pop(&surface_data) &&
		instream.Pop(&role_baby_data) &&
		instream.Pop(&marriage_data) &&
		instream.Pop(&friend_common_data) &&
		instream.Pop(&guild_common_data) &&
		instream.Pop(&role_system_set_data) &&
		instream.Pop(&shi_tu_data) &&
		instream.Pop(&courage_challenge_data) &&
		instream.Pop(&monster_wave_data) &&
		instream.Pop(&mi_yi_data) &&
		instream.Pop(&school_task_data) &&
		instream.Pop(&role_activity_common_data) &&
		instream.Pop(&treasure_map_data) &&
		instream.Pop(&task_chain_data) &&
		instream.Pop(&fabao_data) &&
		instream.Pop(&research_task_data) &&
		instream.Pop(&medal_data) &&
		instream.Pop(&role_crystal_fb_data) &&
		instream.Pop(&buff_data) &&
		instream.Pop(&shop_data) &&
		instream.Pop(&task_trace_data) &&
		instream.Pop(&map_unlock_data) &&
		instream.Pop(&gather_fb_data) &&
		instream.Pop(&exchange_data) &&
		instream.Pop(&rolecross_data) &&
		instream.Pop(&course_of_growth_data) &&
		instream.Pop(&salary_data) &&
		instream.Pop(&cog_record_data) &&
		instream.Pop(&challenge_fb_data) &&
		instream.Pop(&face_score_data) &&
		instream.Pop(&face_score_record_data) &&
		instream.Pop(&appearance_data) &&
		instream.Pop(&func_guide_data) &&
		instream.Pop(&convert_shop_data) &&
		instream.Pop(&special_appearance_data) &&
		instream.Pop(&role_drop_data) &&
		instream.Pop(&monster_group_drop_data) &&
		instream.Pop(&role_login_system_data) &&
		instream.Pop(&boss_chapter_data) &&
		instream.Pop(&role_mysterious_trial_data) &&
		instream.Pop(&bounty_task_data) &&
		instream.Pop(&role_transaction_task_data) &&
		instream.Pop(&pet_common_data) &&
		instream.Pop(&role_active_system_data) &&
		instream.Pop(&role_chosed_data) &&
		instream.Pop(&role_huashen_data) &&
		instream.Pop(&func_preview_data) &&
		instream.Pop(&auto_catch_pet_data) &&
		instream.Pop(&trade_credit_data) &&
		instream.Pop(&pet_skin_data) &&
		instream.Pop(&face_score_time_data) &&
		instream.Pop(&formation_data) &&
		instream.Pop(&gather_dream_data) &&
		instream.Pop(&role_huan_jie_data) &&
		instream.Pop(&anecdote_task_data) &&
		instream.Pop(&anecdote_task_common_data) &&
		instream.Pop(&task_wenxianglou_data) &&
		instream.Pop(&dream_notes_challenge_data) &&
		instream.Pop(&school_exercise_data) &&
		instream.Pop(&pet_auto_move_data) &&
		instream.Pop(&smart_mounts_system_data) &&
		instream.Pop(&realm_break_data) &&
		instream.Pop(&pet_monopoly_data) &&
		instream.Pop(&lun_hui_wang_chuan_data) &&
		instream.Pop(&role_material_data) &&
		instream.Pop(&fabao_draw_data) &&
		instream.Pop(&map_exploration_data) &&
		instream.Pop(&role_join_activity_reward_data) &&
		instream.Pop(&custom_instruction_data) &&
		instream.Pop(&role_guild_inborn_data) &&
		instream.Pop(&role_new_partner_data) &&
		instream.Pop(&heart_skill_data) &&
		instream.Pop(&pet_helper_formation_data) &&
		instream.Pop(&god_print_bag_data) &&
		instream.Pop(&god_print_log_data) &&
		instream.Pop(&nether_world_gatory_data) &&
		instream.Pop(&pet_god_fb_data) &&
		instream.Pop(&week_base_data) &&
		instream.Pop(&week_task_data) &&
		instream.Pop(&role_tian_di_dao_hen) &&
		instream.Pop(&prestige_task_data) &&
		instream.Pop(&partner_gift_tree_data) &&
		instream.Pop(&big_dipper_data) &&
		instream.Pop(&role_attr_pellet_data) &&
		instream.Pop(&active_turntable_data) &&
		instream.Pop(&partner_pact_data) &&
		instream.Pop(&duel_data) &&
		instream.Pop(&tian_ming_data) &&
		instream.Pop(&du_jie_data) &&
		instream.Pop(&yin_yang_eye_data) &&
		instream.Pop(&big_dipper_help_data) &&
		instream.Pop(&role_course_data) &&
		instream.Pop(&advance_career_data) &&
		instream.Pop(&wan_ling_fb_data) &&
		instream.Pop(&role_new_life_skill_data) &&
		instream.Pop(&role_fly_up_data) &&
		instream.Pop(&role_mirage_territory_data) &&
		instream.Pop(&xiu_wei_data) &&
		instream.Pop(&treasure_task_data) &&
		instream.Pop(&role_pet_info_data) &&
		instream.Pop(&role_partner_info_data) &&
		instream.Pop(&fallen_god_data) &&
		instream.Pop(&fallen_god_help_data) &&
		instream.Pop(&mini_game_data) &&
		instream.Pop(&role_mirage_territory_info_data) &&
		instream.Pop(&role_peak_token_data) &&
		instream.Pop(&role_income_data) &&
		instream.Pop(&new_level_complement_data) &&
		instream.Pop(&new_pet_monopoly_data) &&
		instream.Pop(&yao_xing_shi_lian_data) &&
		instream.Pop(&lantern_riddie_data) &&
		instream.Pop(&lovers_market_data) &&
		instream.Pop(&tao_quan_na_ji_data) &&
		instream.Pop(&concentric_knot_data) &&
		instream.Pop(&role_hongmeng_tianjie_data) &&
		instream.Pop(&role_holy_equip_data)
		&&instream.Pop(&zao_hua_refine_data)
		&&instream.Pop(&zao_hua_long_zhu_data)
		&&instream.Pop(&item_drop_cooldown_data)
		&&instream.Pop(&role_ke_ju_exam_data)
		&&instream.Pop(&pet_strengthen_buff_data)
		&& instream.Pop(&element_elf_data)
		&&instream.Pop(&fu_yu_common_data)
		&& instream.Pop(&role_kua_fu_ying_xiong_lu_data)
		&& instream.Pop(&role_day_reward_data)
		&&instream.Pop(&star_chart_data)
		&&instream.Pop(&new_courage_challenge_data)
		&&instream.Pop(&fashion_collection_data)
		&&instream.Pop(&new_god_print_log_data)
		&&instream.Pop(&role_guild_task_data)
		&&instream.Pop(&special_anecdote_data)
		&&instream.Pop(&role_huan_shou_data)
		;
	
	if (!ret)
	{
		return false;
	}

	if (monitor_data.Size() <= sizeof(monitor_param))
	{
		memcpy(&monitor_param, monitor_data.Ptr(), monitor_data.Size());
	}

	DESERIALIZE_OTHER_PARAM(common_data, common_data_param);
	DESERIALIZE_OTHER_PARAM(vip_data, vip_param);
	DESERIALIZE_OTHER_PARAM(attribute_plan_data, attribute_plan_param);
	DESERIALIZE_OTHER_PARAM(money_other_data, money_other_param);
	DESERIALIZE_OTHER_PARAM(knapsack_other_data, knapsack_other_param);
	DESERIALIZE_OTHER_PARAM(prestige_data, prestige_param);
	DESERIALIZE_OTHER_PARAM(collection_data, collection_param);
	DESERIALIZE_OTHER_PARAM(partner_data, partner_param);
	DESERIALIZE_OTHER_PARAM(title_data, title_param);
	DESERIALIZE_OTHER_PARAM(train_data, train_param);

	taskrecrod_len = 0;
	if (task_recorder_data.Size() <= sizeof(taskrecord_list))
	{
		memcpy(taskrecord_list, task_recorder_data.Ptr(), task_recorder_data.Size());
		taskrecrod_len = task_recorder_data.Size();
	}

	DESERIALIZE_OTHER_PARAM(task_manager_data, task_param);
	DESERIALIZE_OTHER_PARAM(skill_other_data, skill_other_param);
	DESERIALIZE_OTHER_PARAM(equip_other_data, equip_pther_param);
	DESERIALIZE_OTHER_PARAM(workshop_data, workshop_param);
	DESERIALIZE_OTHER_PARAM(surface_data, surface_param);
	DESERIALIZE_OTHER_PARAM(role_baby_data, role_baby_param);
	DESERIALIZE_OTHER_PARAM(marriage_data, marry_param);
	DESERIALIZE_OTHER_PARAM(friend_common_data, friend_common_param);
	DESERIALIZE_OTHER_PARAM(guild_common_data, guild_common_param);
	DESERIALIZE_OTHER_PARAM(role_system_set_data, role_system_set_param);
	DESERIALIZE_OTHER_PARAM(shi_tu_data, shi_tu_param);
	DESERIALIZE_OTHER_PARAM(courage_challenge_data, courage_challenge_param);
	DESERIALIZE_OTHER_PARAM(monster_wave_data, monster_wave_param);
	DESERIALIZE_OTHER_PARAM(mi_yi_data, mi_yi_param);
	DESERIALIZE_OTHER_PARAM(school_task_data, school_task_param);
	DESERIALIZE_OTHER_PARAM(role_activity_common_data, roleactivity_common_param);
	DESERIALIZE_OTHER_PARAM(treasure_map_data, treasure_map_param);
	DESERIALIZE_OTHER_PARAM(task_chain_data, task_chain_param);
	DESERIALIZE_OTHER_PARAM(fabao_data, fabao_param);
	DESERIALIZE_OTHER_PARAM(research_task_data, research_task_param);
	DESERIALIZE_OTHER_PARAM(medal_data, medal_param);
	DESERIALIZE_OTHER_PARAM(role_crystal_fb_data, role_crystal_fb);
	DESERIALIZE_OTHER_PARAM(buff_data, buff_param);
	DESERIALIZE_OTHER_PARAM(shop_data, shop_param);
	DESERIALIZE_OTHER_PARAM(task_trace_data, task_trace_param);
	DESERIALIZE_OTHER_PARAM(map_unlock_data, map_unlock_param);
	DESERIALIZE_OTHER_PARAM(gather_fb_data, role_gather_fb_param);
	DESERIALIZE_OTHER_PARAM(exchange_data, exchange_param);
	DESERIALIZE_OTHER_PARAM(rolecross_data, rolecross_param);
	DESERIALIZE_OTHER_PARAM(course_of_growth_data, course_of_growth_param);
	DESERIALIZE_OTHER_PARAM(salary_data, salary_param);
	DESERIALIZE_OTHER_PARAM(fabao_draw_data, fabao_draw_param);
	DESERIALIZE_OTHER_PARAM(map_exploration_data, map_exploration_param);
	course_of_growth_len = 0;
	if (cog_record_data.Size() <= sizeof(course_of_growth_list))
	{
		memcpy(course_of_growth_list, cog_record_data.Ptr(), cog_record_data.Size());
		course_of_growth_len = cog_record_data.Size();
	}

	DESERIALIZE_OTHER_PARAM(challenge_fb_data, challenge_fb_param);
	DESERIALIZE_OTHER_PARAM(face_score_data, face_score_param);

	face_score_record_len = 0;
	if (face_score_record_data.Size() <= sizeof(face_score_record_list))
	{
		memcpy(face_score_record_list, face_score_record_data.Ptr(), face_score_record_data.Size());
		face_score_record_len = face_score_record_data.Size();
	}

	DESERIALIZE_OTHER_PARAM(appearance_data, appearance_param);
	DESERIALIZE_OTHER_PARAM(func_guide_data, func_guide_param);
	DESERIALIZE_OTHER_PARAM(convert_shop_data, convert_shop_param);
	DESERIALIZE_OTHER_PARAM(special_appearance_data, special_appearance_param);
	DESERIALIZE_OTHER_PARAM(role_drop_data, role_drop_param);
	DESERIALIZE_OTHER_PARAM(monster_group_drop_data, monster_group_drop_param);
	DESERIALIZE_OTHER_PARAM(role_login_system_data, role_login_system_param);
	DESERIALIZE_OTHER_PARAM(boss_chapter_data, boss_chapter_param);
	DESERIALIZE_OTHER_PARAM(role_mysterious_trial_data, role_mysterious_trial_param);
	DESERIALIZE_OTHER_PARAM(bounty_task_data, bounty_task_param);
	DESERIALIZE_OTHER_PARAM(role_transaction_task_data, role_transaction_task_param);
	DESERIALIZE_OTHER_PARAM(pet_common_data, pet_common_param);
	DESERIALIZE_OTHER_PARAM(role_active_system_data, role_active_system_param);
	DESERIALIZE_OTHER_PARAM(role_chosed_data, role_chosed_param);
	DESERIALIZE_OTHER_PARAM(role_huashen_data, role_huashen_param);
	DESERIALIZE_OTHER_PARAM(func_preview_data, func_preview_param);
	DESERIALIZE_OTHER_PARAM(auto_catch_pet_data, auto_catch_pet_param);
	DESERIALIZE_OTHER_PARAM(trade_credit_data, trade_credit_param);
	DESERIALIZE_OTHER_PARAM(pet_skin_data, pet_skin_param);
	DESERIALIZE_OTHER_PARAM(face_score_time_data, face_score_time_param);
	DESERIALIZE_OTHER_PARAM(formation_data, formation_param);
	DESERIALIZE_OTHER_PARAM(gather_dream_data, gather_dream_param);
	DESERIALIZE_OTHER_PARAM(role_huan_jie_data, role_huan_jie_param);
	DESERIALIZE_OTHER_PARAM(anecdote_task_data, anecdote_task_param);
	DESERIALIZE_OTHER_PARAM(anecdote_task_common_data, anecdote_task_common_param);
	DESERIALIZE_OTHER_PARAM(task_wenxianglou_data, task_wenxianglou_param);
	DESERIALIZE_OTHER_PARAM(dream_notes_challenge_data, dream_notes_challenge_param);
	DESERIALIZE_OTHER_PARAM(school_exercise_data, school_exercise_param);
	DESERIALIZE_OTHER_PARAM(pet_auto_move_data, pet_auto_move_param);
	DESERIALIZE_OTHER_PARAM(smart_mounts_system_data, smart_mounts_system_param);
	DESERIALIZE_OTHER_PARAM(realm_break_data, realm_break_param);
	DESERIALIZE_OTHER_PARAM(pet_monopoly_data, old_pet_monopoly_param);
	DESERIALIZE_OTHER_PARAM(lun_hui_wang_chuan_data, lun_hui_wang_chuan_param);
	DESERIALIZE_OTHER_PARAM(role_material_data, role_material_param);
	DESERIALIZE_OTHER_PARAM(role_join_activity_reward_data, role_join_activity_reward_param);
	DESERIALIZE_OTHER_PARAM(custom_instruction_data, custom_instruction_param);
	DESERIALIZE_OTHER_PARAM(role_guild_inborn_data, role_guild_inborn_param);
	DESERIALIZE_OTHER_PARAM(role_new_partner_data, new_partner_param);
	DESERIALIZE_OTHER_PARAM(heart_skill_data, m_heart_skill_param);
	DESERIALIZE_OTHER_PARAM(pet_helper_formation_data, pet_helper_formation_param);
	DESERIALIZE_OTHER_PARAM(god_print_bag_data, god_print_bag_param);
	DESERIALIZE_OTHER_PARAM(god_print_log_data, god_print_log_param);
	DESERIALIZE_OTHER_PARAM(nether_world_gatory_data, nether_world_gatory_param);
	DESERIALIZE_OTHER_PARAM(pet_god_fb_data, pet_god_fb_param);


	DESERIALIZE_OTHER_PARAM(week_base_data, week_base_param);
	DESERIALIZE_OTHER_PARAM(week_task_data, week_task_param);
	DESERIALIZE_OTHER_PARAM(role_tian_di_dao_hen, tian_di_dao_hen_param);
	DESERIALIZE_OTHER_PARAM(prestige_task_data, prestige_task_param);
	DESERIALIZE_OTHER_PARAM(partner_gift_tree_data, partner_gift_tree_param);
	DESERIALIZE_OTHER_PARAM(big_dipper_data, big_dipper_param);
	DESERIALIZE_OTHER_PARAM(role_attr_pellet_data, role_attr_pellet_param);
	DESERIALIZE_OTHER_PARAM(active_turntable_data, active_turntable_param);
	DESERIALIZE_OTHER_PARAM(partner_pact_data, partner_pact_param);
	DESERIALIZE_OTHER_PARAM(duel_data, duel_param);
	DESERIALIZE_OTHER_PARAM(tian_ming_data, tian_ming_param);
	DESERIALIZE_OTHER_PARAM(du_jie_data, du_jie_param);
	DESERIALIZE_OTHER_PARAM(yin_yang_eye_data, yin_yang_eye_param);
	DESERIALIZE_OTHER_PARAM(big_dipper_help_data, big_dipper_help_param);
	DESERIALIZE_OTHER_PARAM(role_course_data, role_course_param);
	DESERIALIZE_OTHER_PARAM(advance_career_data, advance_career_param);
	DESERIALIZE_OTHER_PARAM(wan_ling_fb_data, wan_ling_fb_param);
	DESERIALIZE_OTHER_PARAM(role_new_life_skill_data, role_new_life_skill_param);
	DESERIALIZE_OTHER_PARAM(role_fly_up_data, role_fly_up_param);
	DESERIALIZE_OTHER_PARAM(role_mirage_territory_data, role_mirage_territory_param);
	DESERIALIZE_OTHER_PARAM(xiu_wei_data, xiu_wei_param);
	DESERIALIZE_OTHER_PARAM(treasure_task_data, treasure_task_param);
	DESERIALIZE_OTHER_PARAM(role_pet_info_data, role_pet_info_param);
	DESERIALIZE_OTHER_PARAM(role_partner_info_data, role_partner_info_param);
	DESERIALIZE_OTHER_PARAM(fallen_god_data, fallen_god_param);
	DESERIALIZE_OTHER_PARAM(fallen_god_help_data, fallen_god_help_param);
	DESERIALIZE_OTHER_PARAM(mini_game_data, mini_game_param);
	DESERIALIZE_OTHER_PARAM(role_mirage_territory_info_data, role_mirage_territory_info_param);
	DESERIALIZE_OTHER_PARAM(role_peak_token_data, role_peak_token_param);
	DESERIALIZE_OTHER_PARAM(role_income_data, role_income_param);
	DESERIALIZE_OTHER_PARAM(new_level_complement_data, new_level_complement_param);
	DESERIALIZE_OTHER_PARAM(new_pet_monopoly_data, new_pet_monopoly_param);
	DESERIALIZE_OTHER_PARAM(yao_xing_shi_lian_data, yao_xing_shi_lian_param);
	DESERIALIZE_OTHER_PARAM(lantern_riddie_data,lantern_riddie_param);
	DESERIALIZE_OTHER_PARAM(lovers_market_data, lovers_market_param);
	DESERIALIZE_OTHER_PARAM(concentric_knot_data, concentric_knot_param);
	DESERIALIZE_OTHER_PARAM(tao_quan_na_ji_data, tao_quan_na_ji_param);
	DESERIALIZE_OTHER_PARAM(role_hongmeng_tianjie_data, role_hongmeng_tianjie_param);
	DESERIALIZE_OTHER_PARAM(zao_hua_refine_data, zao_hua_refine_param);
	DESERIALIZE_OTHER_PARAM(zao_hua_long_zhu_data, zao_hua_long_zhu_param);
	DESERIALIZE_OTHER_PARAM(role_holy_equip_data, role_holy_equip_common_param);
	DESERIALIZE_OTHER_PARAM(item_drop_cooldown_data, item_cool_down_drop_param);
	DESERIALIZE_OTHER_PARAM(role_ke_ju_exam_data, role_ke_ju_exam_param);
	DESERIALIZE_OTHER_PARAM(pet_strengthen_buff_data, pet_strengthen_buff_param);
	DESERIALIZE_OTHER_PARAM(element_elf_data, element_elf_param);
	DESERIALIZE_OTHER_PARAM(fu_yu_common_data, fu_yu_common_param);
	DESERIALIZE_OTHER_PARAM(role_kua_fu_ying_xiong_lu_data, role_kua_fu_ying_xiong_lu_param);
	DESERIALIZE_OTHER_PARAM(role_day_reward_data, role_day_reward_param);
	DESERIALIZE_OTHER_PARAM(star_chart_data, star_chart_param);
	DESERIALIZE_OTHER_PARAM(new_courage_challenge_data, new_courage_challenge_param);
	DESERIALIZE_OTHER_PARAM(fashion_collection_data, fashion_collection_param);
	DESERIALIZE_OTHER_PARAM(new_god_print_log_data, new_god_print_log_param);
	DESERIALIZE_OTHER_PARAM(role_guild_task_data, role_guild_task_param);
	DESERIALIZE_OTHER_PARAM(special_anecdote_data, special_anecdote_param);
	DESERIALIZE_OTHER_PARAM(role_huan_shou_data, role_huan_shou_param);

	return true;
}