#ifndef __GAMEWORLDSTR_H__
#define __GAMEWORLDSTR_H__

enum OPEN_RANDGIFT_BROADCAST_TYPE
{
	OPEN_RANDGIFT_BROADCAST_ID_CHOCO,					// 打开巧克力播报
	OPEN_RANDGIFT_BROADCAST_ID_SPRING_FESTIVAL_GIFT,	// 新春豪华礼包

	OPEN_RANDGIFT_BROADCAST_ID_MAX_NUM,
};

namespace gamestring
{
	extern char GAMESTRING_BUF[2048];

	extern char *g_leijichongzhi_subbject;
	extern char *g_leijichongzhi_content;
	extern char *g_leijichongzhi_day_content;

	extern char* g_collection_return;
	extern char* g_add_guild_gongxian;
	extern char* g_guild_build_donate;
	extern char* g_guild_dismiss_subject;
	extern char* g_guild_dismiss_content;
	extern char* g_guild_demotion_subject;
	extern char* g_guild_demotion_content;
	extern char* g_partner_talent_gift;

	extern char* g_world_boss_subject;
	extern char* g_world_boss_reward;
	extern char* g_world_boss_hurt_subject;
	extern char* g_world_boss_hurt_reward;
	extern char* g_world_boss_heal_subject;
	extern char* g_world_boss_heal_reward;
	extern char* g_world_boss_luck_subject;
	extern char* g_world_boss_luck_reward;
	extern char* g_world_boss_kill_subject;
	extern char* g_world_boss_kill_reward;
	extern char* g_world_boss_broadcast;
	extern char* g_world_boss_kill_broadcast;
	extern char* g_world_boss_not_kill_broadcast;
	extern char* g_world_boss_appear_broadcast;

	extern char* g_world_boss_2_subject;
	extern char* g_world_boss_2_reward;
	extern char* g_world_boss_2_hurt_subject;
	extern char* g_world_boss_2_hurt_reward;
	extern char* g_world_boss_2_heal_subject;
	extern char* g_world_boss_2_heal_reward;
	extern char* g_world_boss_2_luck_subject;
	extern char* g_world_boss_2_luck_reward;
	extern char* g_world_boss_2_kill_subject;
	extern char* g_world_boss_2_kill_reward;
	extern char* g_world_boss_2_broadcast;
	extern char* g_world_boss_2_kill_broadcast;
	extern char* g_world_boss_2_not_kill_broadcast;
	extern char* g_world_boss_2_appear_broadcast;

	extern char* g_world_boss_3_subject;
	extern char* g_world_boss_3_reward;
	extern char* g_world_boss_3_hurt_subject;
	extern char* g_world_boss_3_hurt_reward;
	extern char* g_world_boss_3_heal_subject;
	extern char* g_world_boss_3_heal_reward;
	extern char* g_world_boss_3_luck_subject;
	extern char* g_world_boss_3_luck_reward;
	extern char* g_world_boss_3_kill_subject;
	extern char* g_world_boss_3_kill_reward;
	extern char* g_world_boss_3_broadcast;
	extern char* g_world_boss_3_kill_broadcast;
	extern char* g_world_boss_3_not_kill_broadcast;
	extern char* g_world_boss_3_appear_broadcast;

	extern char* g_shi_tu_subject;
	extern char* g_shi_tu_reward;
	extern char* g_shitu_mile_subject;
	extern char* g_shitu_mile_master_reward;
	extern char* g_shitu_mile_apprentice_reward;
	extern char * g_shitu_break_time_notice;
	extern char * g_shitu_break_notice;

	extern char* g_knapsack_put_subject;
	extern char* g_knapsack_put_content;
	extern char* g_knapsack_full_mail_subject;
	extern char* g_knapsack_full_send_mail;
	extern char* g_pet_full_mail_subject;
	extern char* g_pet_full_send_mail;
	extern char* g_item_expire_notice_subject;
	extern char* g_item_expire_notice_content;
	extern char * g_item_expire_notice_subject_2;
	extern char * g_item_expire_notice_content_2;
	extern char * g_item_expire_notice_subject_3;
	extern char * g_item_expire_notice_content_3;
	extern char* g_temp_clear_notice_subject;
	extern char* g_temp_clear_notice_content;
	extern char* g_item_not_enough_notie_content;
	extern char* g_item_not_enough_notie_content2;
	extern char* g_team_member_item_not_enough_notice;

	extern char* g_system_item_return_subject;
	extern char* g_partner_item_system_return_content;

	extern char* g_trade_market_advertise;
	extern char* g_trade_market_sold_item_get_money;
	extern char* g_trade_market_focus_success;
	extern char* g_team_fight_reward_subject;
	extern char* g_team_fight_rank_reward;
	extern char* g_team_fight_box_reward;
	extern char* g_team_fight_box_reward_subject;
	extern char* g_team_fight_reissue_box_reward_subject;
	extern char* g_team_fight_reissue_first_box_reward_content;
	extern char* g_team_fight_reissue_three_box_reward_content;
	extern char* g_team_fight_join_reward_subject;
	extern char* g_team_fight_join_reward_content;

	extern char* g_role_rename_notice;
	extern char* g_role_rename_subject;
	extern char* g_role_rename_content;
	extern char* g_role_advance_content;
	extern char* g_maze_racer_amulet_drop;
	extern char* g_maze_racer_reward_subj;
	extern char* g_maze_racer_rank_reward;
	extern char* g_brave_ground_reward_subject;
	extern char* g_brave_ground_reward;
	extern char* g_guild_honor_reward_subject;
	extern char* g_guild_honor_reward_content;
	extern char* g_guild_honor_bye_reward_subject;
	extern char* g_guild_honor_bye_reward_content;
	extern char* g_guild_honor_battle_broadcast;
	extern char * g_guild_honor_battle_direct_win_broadcast;
	extern char* g_money_chongzhi_subject;
	extern char* g_money_chongzhi_content;
	extern char * g_guild_honor_settlement_list_notice;
	extern char * g_guild_honor_battle_start_round_notice;
	extern char * g_guild_honor_battle_guess_subject;
	extern char * g_guild_honor_battle_guess_content;
	extern char * g_guild_honor_battle_guess_champion_subject;
	extern char * g_guild_honor_battle_guess_champion_content;
	extern char * g_guild_honor_battle_rank_reward_subject;
	extern char * g_guild_honor_battle_rank_reward_content;
	extern char * g_guild_honor_battle_rank_unjoin_reward_content;
	extern char * g_guild_honor_battle_last_round_battle_notice;

	extern char * g_guild_building_finish_subject;
	extern char * g_guild_building_finish_leader_content;
	extern char * g_guild_building_finish_deputyleader_content;

	extern char * g_guild_exp_remind_subject;
	extern char * g_guild_exp_remind_will_overflow_content;
	extern char * g_guild_exp_remind_overflow_content;

	extern char* g_guild_fight_rank_reward_title;
	extern char* g_guild_fight_familys_rank_reward;
	extern char* g_guild_fight_survival_subject;
	extern char* g_guild_fight_survival_reward;
	extern char* g_guild_fight_participation_subject;
	extern char* g_guild_fight_participation_award;

	extern char* g_only_fight_rank_reward_subject;
	extern char* g_only_fight_rank_reward_content;
	extern char* g_only_fight_survival_subject;
	extern char* g_only_fight_survival_reward;
	extern char* g_only_fight_participation_subject;
	extern char* g_only_fight_participation_award;
	extern char* g_only_fight_notice;

	extern char* g_silk_roads_rank_reward_subject;
	extern char* g_silk_roads_rank_reward_content;
	extern char* g_silk_roads_settlement_reward_subject;
	extern char* g_silk_roads_settlement_reward_content;

	extern char* g_colosseum_subject;
	extern char* g_colosseum_coin_reward;
	extern char* g_colosseum_rank_reward_subject;
	extern char* g_colosseum_rank_reward_content;
	extern char* g_colosseum_challenge_times_remain_reward_subject;
	extern char* g_colosseum_challenge_times_remain_reward_content;
	extern char* g_courage_challenge_reward_content;
	extern char* g_new_courage_challenge_reward_content;
	extern char* g_dream_notes_challenge_subject;
	extern char* g_dream_notes_challenge_content;
	extern char* g_dream_notes_challenge_reissue;
	extern char* g_dream_notes_challenge_use_item_add_times_notice;
	extern char* g_role_level_not_enough_not_challenge;
	extern char* g_get_role_uid_float;
	extern char* g_refuse_qie_cuo_float;
	extern char* g_scene_unlock_notice;
	extern char* g_team_recruit;
	extern char* g_activity_state;
	extern char* g_activity_sign_up_subject;
	extern char* g_activity_sign_up_content;
	extern char* g_team_create;
	extern char* g_team_add_member;
	extern char* g_team_member_leave;
	extern char* g_team_become_leader;
	extern char* g_team_be_kicked;
	extern char* g_team_member_refuse_summon;
	extern char* g_team_member_level_not_enough;
	extern char* g_guild_week_task_settlement_reward_title;
	extern char* g_guild_week_task_issue_settlement_reward;
	extern char* g_guild_week_task_contribute_reward_title;
	extern char* g_guild_week_task_issue_contribute_reward;
	extern char* g_guild_week_task_settle_contribute_subject;
	extern char* g_guild_week_task_settle_contribute_content;
	extern char* g_guild_boss_chapter_notice_subject;
	extern char* g_guild_boss_chapter_notice_content;
	extern char* g_guild_boss_chapter_rank_subject;
	extern char* g_guild_boss_chapter_rank_content;

	extern char* g_guild_gather_dream_level_not_enough;
	extern char* g_guild_gather_dream_publish;
	extern char* g_guild_gather_dream_daychange_reward_subject;
	extern char* g_guild_gather_dream_daychange_reward_content;
	extern char* g_guild_gather_Dream_leave_guild_reward_content;
	extern char* g_guild_gather_dream_reissue_reward_content;

	extern char* g_guild_boss_chapter_reward_subject;
	extern char* g_guild_boss_chapter_baodi_reward;
	extern char* g_guild_boss_chapter_kill_reward_subject;
	extern char* g_guild_boss_chapter_kill_reward_content;
	extern char* g_guild_boss_chapter_notice;

	extern char* g_salary_reward_subject;
	extern char* g_salary_reward_content;
	extern char* g_salary_captain_salary;
	extern char* g_salary_chivalrous_salary;

	extern char* g_smart_mounts_system_level_not_engough;
	extern char* g_smart_mounts_system_skill_up_level_limit;
	extern char* g_smart_mounts_system_breach_faild_notice;

	extern char* g_role_guild_inborn_learn_skill_level_not_enough;
	extern char* g_role_guild_inborn_skill_points_not_enough;
	extern char* g_role_guild_inborn_reset_skill_subject;
	extern char* g_role_guild_inborn_reset_skill_content;

	extern char* g_role_huan_shou_open_egg_rare_broadcast_content;

	extern char* g_realm_break_offline_info_pool_notice;

	extern char* g_chat_open_level;

	extern char* g_platform_battle_reward_subject;
	extern char* g_platform_battle_reward_content;
	extern char * g_platform_battle_reward_content2;

	extern char* g_chief_election_no_winner_broadcast;
	extern char* g_chief_election_champion_broadcast;
	extern char* g_chief_election_reward_subject;
	extern char* g_chief_election_reward_content;
	extern char * g_chief_election_battle_reward_subject;
	extern char * g_chief_election_battle_reward_content;
	extern char * g_chief_election_sea_reward_subject;
	extern char * g_chief_election_sea_reward_content;
	extern char * g_chief_election_sea_tip1;		//由于对方不在场或者处于无法战斗中，恭喜晋级下一轮比赛
	extern char * g_chief_election_sea_tip2;		//由于您处于无法战斗中，此轮比赛失败
	extern char * g_chief_election_sea_tip3;		//由于无法再规定时间或回合里分出胜负，由角色+宠物评分高那方获胜
	extern char * g_chief_election_sea_tip4;		//由于战斗用时超过此轮比赛最大用时，无法晋级下一轮比赛

	extern char* g_chief_peak_battle_nobody_join_broadcast;
	extern char* g_chief_peak_battle_champion_broadcast;
	extern char* g_activity_chief_peak_battle_rank_reward_subject;
	extern char* g_activity_chief_peak_battle_rank_reward_content;
	extern char* g_activity_chief_peak_battle_bet_reward_subject;
	extern char* g_activity_chief_peak_battle_bet_reward_content;
	extern char * g_activity_chief_peak_battle_battle_reward_subject;
	extern char * g_activity_chief_peak_battle_battle_reward_content;

	extern char* g_put_prestige_notie;
	extern char* g_prestige_uplevel_notie;
	extern char* g_transfer_subject;
	extern char* g_transfer_content;

	extern char* g_course_of_growth_uplevel_subject;
	extern char* g_course_of_growth_uplevel_content;

	extern char *g_chongzhi_reward_subject;
	extern char *g_chongzhi_reward_content;
	extern char *g_chongzhi_reward_coin_content;
	extern char *g_month_investment_subject;
	extern char *g_month_investment_content;
	extern char *g_first_recharge_subject;
	extern char *g_first_recharge_content;
	extern char *g_first_recharge_exclusive;

	extern char *g_month_week_card_reward_subject;
	extern char *g_buy_month_week_card_reward_content;
	extern char *g_buy_moth_week_card_item_content;

	extern char* g_mail_collection_content;
	extern char* g_mail_collection_main_content;

	extern char* g_mail_change_prof_subject;
	extern char* g_mail_change_prof_content;

	extern char * g_mail_bountytask_turntable;

	extern char* g_join_guild;
	extern char* g_agree_invite_but_full;
	extern char* g_refuse_invite;
	extern char* g_refuse_invite_notice;

	extern char* g_funopen_task_limit;
	extern char* g_trademark_buy_succ_content;
	extern char* g_task_adv_succ_content;
	extern char* g_task_reissue_item_subject;
	extern char* g_task_reissue_item_content;

	extern char* g_sold_succ_send_global_message;
	extern char* g_chairman_to_newbie_message;
	extern char* g_buff_effect_mp_message;
	extern char* g_buff_effect_hp_message;
	extern char* g_buff_effect_hpmp_message;

	extern char* g_knapsack_on_consume_message;
	extern char* g_knapsack_on_consume_message2;
	extern char* g_money_on_consume_money_message;

	extern char* g_buff_effect_attr_message;
	extern char* g_buff_bad_effect_attr_message;
	extern char* g_battle_server_disconnect;

	extern char* g_fabao_level_limit_message;
	extern char* g_fabao_uplevel_limit_message;
	extern char* g_fabao_back_subject;
	extern char* g_fabao_back_content;
	extern char* g_fabao_back_subject2;
	extern char* g_fabao_back_content2;

	extern char* g_patrol_task_message;
	extern char* g_patrol_task_subject;
	/////////////////////////////////////// 系统回收 ///////////////////////////////////////////
	extern char *g_retrieve_mail_subject;
	extern char *g_retrieve_mail_content_refund_money;
	extern char *g_retrieve_mail_content_indebt_money;
	extern char *g_retrieve_mail_content_retrieve_debt;
	extern char *g_retrieve_mail_content_bug_money;
	extern char *g_retrieve_mail_content_refund_item;
	extern char *g_retrieve_mail_content_bug_item;
	extern char *g_retrieve_mail_content_exp;

	extern char *g_retrieve_mail_content_refund_other_money;
	extern char *g_retrieve_mail_content_indebt_other_money;
	extern char *g_retrieve_mail_content_retrieve_debt_other_money;
	extern char *g_retrieve_mail_content_bug_other_money;

	extern char* g_server_first_kill_subject;
	extern char* g_server_first_kill_content;
	extern char* g_server_first_kill_miyi;
	extern char* g_server_first_kill_boss;
	extern char* g_server_first_kill_challenge;
	extern char* g_server_first_kill_shan_hai;
	extern char* g_server_first_kill_big_dipper;
	extern char* g_server_first_kill_fallen_god;
	extern char* g_server_first_kill_hmtj_fb;

	extern char* g_wild_boss_refresh_notice;

	extern char* g_treasure_item_notice1;
	extern char* g_treasure_item_notice2;

	extern char* g_advancefb_task_notic;

	extern char *g_bounty_task_turn_table_reward_subject;
	extern char *g_bounty_task_turn_table_reward_content;
	extern char* g_bounty_task_turn_table_reward_notice;

	extern char *g_task_chain_turn_table_reward_subject;
	extern char *g_task_chain_turn_table_reward_content;

	extern char* g_world_question_subject;
	extern char* g_world_question_content;

	extern char* g_seven_color_picture_subject;
	extern char* g_seven_color_picture_content;
	extern char* g_seven_color_picture_total_content;
	extern char* g_seven_color_picture_hearsay_content;

	extern char* g_animal_race_subject;
	extern char* g_animal_race_content;

	extern char* g_happy_up_hearsay_content;

	extern char * g_xun_bao_broadcast;
	extern char * g_xun_bao_partner_broadcast;

	extern char* g_face_score_active_succ;
	extern char* g_face_score_is_inactive;

	extern char* g_system_post_partner;
	extern char* g_system_post_equipment_max;
	extern char* g_system_post_pet_max;

	extern char * g_workshop_strone_level_notice;
	extern char * g_workshop_strone_puton_level_notice;
	extern char *g_synthesis_info_1;
	extern char *g_synthesis_info_2;
	extern char *g_synthesis_info_3;
	extern char *g_synthesis_info_4;
	extern char *g_synthesis_info_5;
	extern char *g_synthesis_info_6;
	extern char * g_compound_pet_equipment_broadcast;		//宠物护符新增传闻

	extern char * g_heritage_treasure_broadcast;

	extern char * g_tian_jiang_hao_li_hearsay;

	extern char * g_randactivity_expelmonster_notice;

	extern char * g_randactivity_partner_exchange_notice;

	extern char * g_randactivity_collect_word_broadcast;

	extern char * g_randactivity_pet_exchange_notice;
	extern char * g_pet_compose;
	extern char * g_randactivity_adventure_gift_broadcast;

	extern char * g_randactivity_make_up_reward_subject;
	extern char * g_randactivity_make_up_reward_content;

	extern char * g_randactivity_lucky_turntable_content;
	extern char *g_rand_activity_tianming_chou_content;
	extern char *g_rand_activity_tian_ming_divination_reward_content;

	extern char * g_randactivity_quan_min_discount_subject;
	extern char * g_randactivity_quan_min_discount_message;

	extern char* g_randactivity_good_gift_turn_subject;
	extern char* g_randactivity_good_gift_turn_content;

	extern char* g_randactivity_help_others_subject;
	extern char* g_randactivity_help_others_content;

	extern char * g_randactivity_help_others_2_subject;
	extern char * g_randactivity_help_others_2_content;

	extern char * g_randactivity_shan_hai_question_subject;
	extern char * g_randactivity_shan_hai_question_sontent;

	extern char* g_randactivity_secret_treasure_subject;
	extern char* g_randactivity_secret_treasure_content;

	extern char *g_trademarket_announce_notice;
	extern char *g_trademarket_onsale_notice;

	extern char* g_chat_group_subject;
	extern char* g_chat_group_dismiss_content;
	extern char* g_chat_group_tick_out_content;
	extern char* g_single_chat_is_limit_notice;

	extern char* g_ra_sports_subject;
	extern char* g_ra_sports_histroy_content;
	extern char* g_ra_sports_settlement_content;

	extern char * g_huan_jie_zhan_chang_rank_reward_subject;
	extern char * g_huan_jie_zhan_chang_rank_reward_content;
	extern char * g_huan_jie_zhan_chang_settlement_reward_subject;
	extern char * g_huan_jie_zhan_chang_settlement_reward_content;
	extern char * g_huan_jie_zhan_chang_ranking_list_reward_subject;
	extern char * g_huan_jie_zhan_chang_ranking_list_reward_content;
	extern char * g_huan_jie_zhan_chang_today_win_reward_subject;
	extern char * g_huan_jie_zhan_chang_today_win_reward_content;

	extern char * g_partner_crytals_remove;
	extern char * g_partner_crytals_done;
	extern char * g_partner_quality_pur_to_ora;
	extern char * g_partner_quality_ora_to_red;
	extern char * g_partner_quality_upgrade;
	extern char * partner_quality_revert_subject;
	extern char * partner_quality_revert_content;
	extern char * partner_skin_notice;
	extern char * partner_skin_notice_2;

	extern char * g_enter_miyi;
	extern char * g_enter_miyi_2;
	extern char * g_miyi_poker_title;
	extern char * g_miyi_poker_reward_content;

	extern char * g_chat_group_invite_fail_content;
	extern char * g_chat_group_invite_succ_content;
	extern char * g_add_friend_succ_content;

	extern char * prestige_over;
	extern char * g_prestige_use_limit;
	extern char * person_rank_prof_capability_top_change_broadcast;

	extern char * g_math_team_fail_content;
	extern char * g_crytals_reward_content;
	extern char * g_crytals_reward2_content;

	extern char * g_wildboss_poker_title;
	extern char * g_wildboss_poker_reward_content;
	extern char * g_wildboss_poker_reward_notice;

	extern char * g_shanhaiboss_poker_title;
	extern char * g_shanhaiboss_poker_reward_content;
	extern char * g_shanhaiboss_broadcast;
	extern char * g_shanhaiboss_reward_notice;

	extern char * g_crystal_fb_poker_reward_subject;
	extern char * g_crystal_fb_poker_reward_content;
	extern char * g_crystal_fb_poker_reward_notice;

	extern char * g_wan_ling_fb_reward_notice;

	extern char * g_god_token_surplus_title;
	extern char * g_god_token_surplus_reward_content;

	extern char * g_god_token_level_reward_title;
	extern char * g_god_token_level_reward_content;

	extern char * g_chong_zhi_buy_item_reward_title;
	extern char * g_chong_zhi_buy_item_reward_content;

	extern char * g_pet_strengthen_broadcast;

	extern char * g_has_poker_get;

	extern char * g_guild_up_level;

	extern char * g_medal_up_grade;
	extern char * g_medal_lottery_broadcast;

	extern char * g_anecdote_task_reward_by_mail_subject;
	extern char * g_anecdote_task_reward_by_mail_content;
	extern char * g_anecdote_task_reward_by_mail_knapsack_full;
	extern char * g_anecdote_big_event_reward_by_mail_subject;
	extern char * g_anecdote_big_event_reward_by_mail_content;

	extern char * g_retrieve_pet_notice;
	extern char * g_retrieve_pet_return_equip;

	extern char *g_draw_reward_subject;
	extern char *g_draw_reward_content;

	extern char *g_randactivity_reward_subject;
	extern char *g_randactivity_reward_subject_1;
	extern char *g_randactivity_reward_content;
	extern char *g_randactivity_reward_end_content;
	extern char *g_randactivity_reward_buy_limit_content;
	extern char *g_randactivity_bag_full_subject;
	extern char *g_randactivity_bag_full_content;

	extern char *g_ra_server_competition_end_rank_content;
	extern char *g_ra_server_competition_end_broadcast;
	extern char * g_pet_add_exp_err_without_break_level;

	extern char *g_norex_item_use_item_invalid;
	extern char *g_norex_item_use_item_num_limit;

	extern char *g_life_skill_gather_notice;
	extern char *g_life_skill_gather_double_notice;
	extern char *g_life_skill_fish_notice;
	extern char *g_life_skill_fish_double_notice;
	extern char *g_life_skill_consume_gather_power;
	extern char *g_life_skill_level_up_notice;
	extern char *g_life_skill_change_surface_noitce;
	extern char *g_life_skill_surface_point_not_enough;

	extern char *g_buy_extra_times_notice;
	extern char *g_crystal_fb_use_energy_item;

	extern char *g_kua_fu_ying_xiong_lu_rank_subject;
	extern char *g_kua_fu_ying_xiong_lu_rank_content;
	extern char *g_kua_fu_ying_xiong_lu_bag_full_subject;
	extern char *g_kua_fu_ying_xiong_lu_bag_full_content;
	extern char *g_kua_fu_ying_xiong_lu_rand_end_task_reward_subject;
	extern char *g_kua_fu_ying_xiong_lu_rand_end_task_reward_content;

	extern char* g_trademarket_give_item_to_user_offline_subject;
	extern char* g_trademarket_give_item_to_user_offline_content;

	extern char* g_order_return_resource_subject;
	extern char* g_order_upshelve_return_resource;
	extern char* g_order_buy_return_resource;
	extern char* g_order_advertise_return_resource;
	extern char* g_new_version_auto_offshelve_subject;
	extern char* g_new_version_auto_offshelve_content;

	extern char * g_act_full_bag_title;
	extern char * g_act_full_bag_content;
	extern char * g_temp_chat_group_name;
	extern char * g_chat_group_msg_notice_join_temp;
	extern char * g_chat_group_msg_notice_create_temp;
	extern char * g_chat_group_msg_notice_become_official;
	extern char * g_teammate_chat_group_full;
	extern char * g_create_chat_group_level_limit_notice;
	extern char * g_create_chat_group_cd_limit_notice;
	extern char * g_welcome_join_temp_group_notice;

	extern char * guild_tan_he_notice_title;
	extern char * guild_tan_he_notice_content;
	extern char * guild_tan_he_succ_title;
	extern char * guild_tan_he_succ_content;
	
	extern char * g_shen_shou_jiang_lin_broadcast_1;
	extern char * g_shen_shou_jiang_lin_broadcast_2;
	extern char * g_shen_shou_jiang_lin_content_1;
	extern char * g_shen_shou_jiang_lin_content_2;
	extern char * g_auto_assign_attribute_points_notice;

	extern char * g_activity_trade_return_gold_subject;
	extern char * g_activity_trade_return_gold_content;

	extern char * g_god_equip_create_notice;
	extern char * g_cumulative_recharge_notice;
	extern char * g_chun_ri_deng_feng_broadcast;
	extern char * g_gift_randitem_is_broadcast_notice;

	extern char * g_clear_mixed_element_crystal_subject;
	extern char * g_clear_mixed_element_crystal_content;

	extern char * g_brave_ground_sao_dang_subject;
	extern char * g_brave_ground_sao_dang_content;
	extern char * g_brave_ground_reset_subject;
	extern char * g_brave_ground_reset_content;
	extern char * g_brave_ground_reward_reissue_ver1_subject;
	extern char * g_brave_ground_reward_reissue_ver1_content;

	extern char * g_mi_yi_first_kill_subject;
	extern char * g_mi_yi_first_kill_content;

	extern char *g_randactivity_fortune_maze_subject;
	extern char *g_randactivity_fortune_maze_content;

	extern char *g_lao_dong_hao_li_broadcast;
	extern char *g_lao_dong_hao_li_end_broadcast;

	extern char * g_role_convert_buy_front_item_not_buy_notice;
	extern char * g_has_order;			//由于上一个订单未完成，请等待%d秒再试

	extern char * g_top_level_subject;
	extern char * g_top_level_content;

	extern char * g_ra_reward_borcast;
	extern char * g_ra_reward_borcast2;
	extern char * g_ling_pai_tian_ci_borcast;
	extern char * g_xin_fa_wu_liang_borcast;
	extern char * g_fu_shi_hun_qi_borcast;
	extern char * g_yun_ze_er_san_shi_borcast;
	extern char * g_yun_ze_er_san_shi_marquee_borcast;

	extern char* g_chat_group_handover_leadership_succ;
	extern char* g_chat_group_leader_handovered_to_you;
	extern char* g_chat_group_leadership_handovered;

	extern char* g_lun_hui_wang_chuan_subject;
	extern char* g_lun_hui_wang_chuan_content;

	extern char* g_energy_add_notice;

	extern char* g_cloud_arena_season_reward_subject;
	extern char* g_cloud_arena_season_reward_content;

	extern char* g_world_team_arena_team_member_level_not_enough;
	extern char* g_world_team_arena_team_member_cancel_sign_up;
	extern char* g_world_team_arena_team_member_logout;
	extern char* g_world_team_arena_team_member_leave_team;
	extern char* g_world_team_arena_create_team_succ_notice;
	extern char* g_world_team_arena_create_team_succ_borcast;
	extern char* g_world_team_arena_user_particip_subject;
	extern char* g_world_team_arena_user_particip_content;
	extern char* g_world_team_arena_team_rank_subject;
	extern char* g_world_team_arena_team_rank_content;
	extern char* g_world_team_arena_top_team_subjcet;
	extern char* g_world_team_arena_top_team_content;
	extern char* g_world_team_arena_team_standy_brocast;
	extern char* g_world_team_arena_team_can_enter_brocast;

	extern char * g_relic_looting_rank_reward_subject;
	extern char * g_relic_looting_rank_reward_content;

	extern char * g_lun_hui_wang_chuan_reward_subject;
	extern char * g_lun_hui_wang_chuan_reward_content;

	extern char * g_relic_looting_reward_subject;
	extern char * g_relic_looting_reward_content;

	extern char * g_heart_skill_displace_failed_notice;
	extern char * g_role_recommend_score_not_enough_notice;

	extern char * g_crystal_fb_xiyou_times_add_notice;

	extern char * g_god_equip_return_subject;
	extern char * g_god_equip_return_content;

	extern char * g_role_join_activity_reward_subject;
	extern char * g_role_join_activity_reward_content;

	extern char * g_ra_pai_mai_hang_bid_subject;
	extern char * g_ra_pai_mai_hang_bid_succ_content;
	extern char * g_ra_pai_mai_hang_bid_succ_content1;
	extern char * g_ra_pai_mai_hang_bid_failed_content;
	extern char * g_ra_pai_mai_hang_bid_failed_notice;
	extern char * g_ra_pai_mai_hang_bid_set_add_price_notice;

	extern char * g_chief_election_winner_login_broadcast;
	extern char * g_guild_honor_winner_login_broadcast;


	extern char * g_guild_active_day_active_subject;
	extern char * g_guild_active_day_active_content;
	extern char * g_guild_active_week_active_rank_subject;
	extern char * g_guild_active_week_active_rank_content;
	extern char * g_guild_active_week_active_person_rank_subject;
	extern char * g_guild_active_week_active_person_rank_content;
	extern char * pet_refine_bian_yi_broadcast;
	extern char * g_pet_unlock_talent_group_str_level_not_enough;
	extern char * g_pet_talent_reset_return_consume_item_subject;
	extern char * g_pet_talent_reset_return_consume_item_content;

	extern char * g_return_item_subject;
	extern char * g_return_item_content;
	extern char * g_return_item_subject2;
	extern char * g_return_item_content2;
	extern char * g_return_item_subject3;
	extern char * g_return_item_content3;
	extern char * g_return_item_subject4;
	extern char * g_return_item_content4;
	extern char * g_return_item_subject5;
	extern char * g_return_item_content5;
	extern char * g_return_item_subject6;
	extern char * g_return_item_content6;
	extern char * g_return_item_subject7;
	extern char * g_return_item_content7;
	extern char * g_return_item_subject8;
	extern char * g_return_item_content8;
	extern char * g_return_item_subject9;
	extern char * g_return_item_content9;
	extern char * g_return_item_subject10;
	extern char * g_return_item_content10;
	extern char * g_return_item_subject11;
	extern char * g_return_item_content11;
	extern char * g_return_item_subject12;
	extern char * g_return_item_content12;
	extern char * g_return_item_subject13;
	extern char * g_return_item_content13;
	extern char * g_return_item_subject14;
	extern char * g_return_item_content14;
	extern char * g_return_item_subject15;
	extern char * g_return_item_content15;

	extern char * g_change_card_point_recovery_subject;
	extern char * g_change_card_point_recovery_content;

	extern char * g_hundred_ghost_poker_title;
	extern char * g_hundred_ghost_poker_reward_content;
	extern char * g_hundred_ghost_poker_reward_notice;
	extern char * g_hundred_ghost_broadcast;
	extern char * g_hundred_ghost_big_boss_broadcast;

	extern char * g_nether_end_change_subject;
	extern char * g_nether_end_change_content;
	extern char * g_nether_end_change_silver_content;
	extern char * g_nether_end_change_gold_content;
	extern char * g_nether_end_change_final_content;

	extern char * g_pet_god_print_wish_broadcast;
	extern char * g_pet_god_print_wish_broadcast_2;

	extern char * g_randactivity_fortune_maze_notice;

	extern char * g_hongbao_notify_content;

	extern char * give_gift_back_item_subject;
	extern char * give_gift_back_item_content;	
	extern char * g_give_flower_notice;
	extern char * g_give_flower_back_kiss;

	extern char * g_ra_qiong_lou_yu_yan_subject;
	extern char * g_ra_qiong_lou_yu_yan_content;

	extern char * g_god_equip_return_subject;
	extern char * g_god_equip_return_content;

	extern char * fang_fraud_send_mail_subject;
	extern char * fang_fraud_send_mail_content;

	extern char * g_ra_back_reward_send_mail_subject;
	extern char * g_ra_back_reward_send_mail_content;

	extern char * duel_begin_duel_battle;
	extern char * duel_begin_duel_battle_team;
	extern char * duel_begin_duel_two_battle;
	extern char * duel_begin_duel_two_battle_team;

	extern char * duel_red_name_login_cross;
	extern char * duel_lose_reduce_exp;
	extern char * duel_lose_reduce_exp_team;
	extern char * duel_lose_reduce_coin;
	extern char * duel_lose_reduce_coin_team;
	extern char * duel_lose_reduce_kill_value;
	extern char * duel_lose_reduce_kill_value_team;

	extern char * duel_type_one_attack_win;
	extern char * duel_type_one_attack_win_team;
	extern char * duel_type_one_defend_win;
	extern char * duel_type_one_defend_win_team;
	extern char * duel_type_one_defend_win_red;
	extern char * duel_type_one_defend_win_red_team;

	extern char * duel_type_two_attack_win;
	extern char * duel_type_two_attack_win_team;
	extern char * duel_type_two_defend_win;

	extern char * duel_type_three_attack_win;
	extern char * duel_type_three_defend_win;
	extern char * duel_type_three_defend_win_team;
	extern char * duel_type_three_defend_lose;
	extern char * duel_type_three_defend_lose_team;

	extern char * duel_win_add_coin;
	extern char * duel_win_add_coin_team;
	extern char * duel_end_duel_battle;
	extern char * duel_begining_duel_two;
	extern char * duel_lose_duel_two;

	extern char * g_capacity_target_reward_subject;
	extern char * g_capacity_target_reward_content;
	extern char * g_randactivity_lucky_bag_subject;
	extern char * g_randactivity_lucky_bag_content;

	extern char * g_stone_mail_back_subject;
	extern char * g_stone_mail_back_content;

	extern char * g_god_equip_back_break_subject;
	extern char * g_god_equip_back_break_content;

	extern char * g_partner_tree_return_subject;
	extern char * g_partner_tree_return_content;

	extern char * g_randactivity_horcrux_tiancheng_broadcast_1;
	extern char * g_randactivity_horcrux_tiancheng_broadcast_2;

	extern char * g_randactivity_integral_draw_broadcast;
	extern char * g_god_equip_break_through_broadcast;

	extern char * g_ra_shan_hai_meet_subject;
	extern char * g_ra_shan_hai_meet_content;

	extern char * g_randactivity_fate_entanglement_subject;
	extern char * g_randactivity_fate_entanglement_content;

	extern char * g_randactivity_yunze_catch_elves_subject;
	extern char * g_randactivity_yunze_catch_elves_content;

	extern char * g_randactivity_yunze_hao_li_subject;
	extern char * g_randactivity_yunze_hao_li_content;

	extern char * g_randactivity_zhaocaijinbao_broadcast;

	extern char * g_endless_tower_rank_reward_subject;
	extern char * g_endless_tower_rank_reward_content;

	extern char * g_role_skill_chagne_return_subject;
	extern char * g_role_skill_change_return_content;
	extern char * g_role_skill_chagne_return_subject_2;
	extern char * g_role_skill_change_return_content_2;

	extern char* g_jie_yi_add_member_subject;
	extern char* g_jie_yi_add_member_content;
	extern char* g_jie_yi_leave_subject;
	extern char* g_jie_yi_leave_content;
	extern char* g_jie_yi_kick_subject;
	extern char* g_jie_yi_kick_content;
	extern char* g_jie_yi_dismiss_subject;
	extern char* g_jie_yi_dismiss_content;
	extern  char * g_jie_yi_mod_ver_0_subject;
	extern  char * g_jie_yi_mod_ver_0_content;
	extern  char * g_jie_yi_create_broadcast;
	extern char * g_jie_yi_tan_he_subject;
	extern char * g_jie_yi_tan_he_content;	
	extern char * g_jie_yi_break_tan_he_content;

	extern char * g_randactivity_yun_ze_territory_subject;
	extern char * g_randactivity_yun_ze_territory_content;

	extern char* g_randactivity_yun_ze_slot_machine_subject;
	extern char* g_randactivity_yun_ze_slot_machine_content;

	extern char* g_randactivity_yun_ze_er_san_shi_subject;
	extern char* g_randactivity_yun_ze_er_san_shi_content;

	extern char* g_randactivity_yun_ze_blooming_record_subject;
	extern char* g_randactivity_yun_ze_blooming_record_content;

	extern char* g_randactivity_tiger_auspicious_snow_subject;
	extern char* g_randactivity_tiger_auspicious_snow_content;

	extern char* g_randactivity_tiger_find_spring_yesterday_content;

	extern char * g_cow_report_good_news_subject;
	extern char * g_cow_report_good_news_win_content;
	extern char * g_cow_report_good_news_lose_content;
	extern char * g_cow_report_good_news_notice;
	extern char * g_cow_report_good_news_remind;

	extern char * g_tiger_brings_blessing_subject;
	extern char * g_tiger_brings_blessing_win_content;
	extern char * g_tiger_brings_blessing_lose_content;
	extern char * g_tiger_brings_blessing_notice;
	extern char * g_tiger_brings_blessing_remind;

	extern char * g_randactivity_jixing_gaozhao_broadcast_1;
	extern char * g_randactivity_jixing_gaozhao_broadcast_2;
	extern char * g_randactivity_yinhu_xianshi_notice;
	extern char * g_randactivity_ciniu_yinxinghu_notice;

	extern char * g_reliclooting_open_box_notice;
	extern char * g_reliclooting_cancle_notice;
	extern char * g_reliclooting_catch_player_notice;

	extern char * g_flyup_succ_notice_1;
	extern char * g_flyup_succ_notice_2;

	extern char *g_open_randgift[OPEN_RANDGIFT_BROADCAST_ID_MAX_NUM];

	extern char * g_treasure_task_notice;
	extern char * g_treasure_task_subject;
	extern char * g_treasure_task_content;

	extern char * g_ra_compensation_subject;
	extern char * g_ra_compensation_content;

	extern char * g_randactivity_jingji_yueka_reissue_subject;
	extern char * g_randactivity_jingji_yueka_reissue_content;

	extern char * g_wedding_meal_notice;
	extern char * g_wedding_fb_bless_notice;
	extern char * g_wedding_invite_subject;
	extern char * g_wedding_invite_content;
	extern char * g_wedding_regist_subject;
	extern char * g_wedding_regist_content;
	extern char * g_wedding_xun_you_subject;
	extern char * g_wedding_xun_you_content;
	extern char * g_wedding_xun_you_notice;

	extern char * g_qing_yuan_create_broadcast;
	extern char * g_qing_yuan_dismiss_subject;
	extern char * g_qing_yuan_dismiss_content;
	extern char * g_qing_yuan_give_gift_subject;
	extern char * g_qing_yuan_give_gift_content;
	extern char * g_qing_yuan_duration_reward_subject;
	extern char * g_qing_yuan_duration_reward_content;

	extern char * g_world_arena_standby_reminder;
	extern char * g_world_arena_force_start_battle_reminder;
	extern char * g_world_arena_battle_end_leave_scene;
	extern char * g_world_arena_season_rank_reward_subject;
	extern char * g_world_arena_season_rank_reward_content;
	extern char * g_world_arena_someone_leave_scene_wait_notice;
	extern char * g_world_arena_fight_result_subject;
	extern char * g_world_arena_not_show_up;
	extern char * g_world_arena_schedule_release_notice_subject;
	extern char * g_world_arena_schedule_release_notice_content;
	extern char * g_world_arena_win_box_reward_subject;
	extern char * g_world_arena_win_box_reward_content;
	extern char * g_world_arena_day_reward_subject;
	extern char * g_world_arena_day_reward_content;
	extern char * g_world_arena_signup_reward_subject;
	extern char * g_world_arena_signup_reward_content;
	extern char * g_world_arena_opponent_not_showup_yet;
	extern char * g_world_arena_opponent_showup;
	extern char * g_world_arena_start_battle_failed_draw;
	extern char * g_world_arena_start_battle_failed_win;
	extern char * g_world_arena_start_battle_failed_lose;

	extern char * g_fake_announcement_content_1;
	extern char * g_fake_announcement_content_2;

	extern char * g_fallen_god_rank_reward_subject;
	extern char * g_fallen_god_rank_reward_content;

	extern char* g_mini_game_subject;
	extern char* g_mini_game_content;

	extern char * g_relic_looting_rank_subject_compensate_1;
	extern char * g_relic_looting_rank_content_compensate_1;
	extern char * g_relic_looting_rank_subject_compensate_2;
	extern char * g_relic_looting_rank_content_compensate_2;
	extern char * g_relic_looting_rank_subject_compensate_3;
	extern char * g_relic_looting_rank_content_compensate_3;

	extern char * g_peak_token_notice;
	extern char * g_peak_token_reward_notice;

	extern char* g_guild_battle_rank_subject;
	extern char* g_guild_battle_rank_content;
	extern char* g_guild_rich_rank_subject;
	extern char* g_guild_rich_rank_content;
	extern char* g_guild_kinder_rank_subject;
	extern char* g_guild_kinder_rank_content;
	extern char* g_get_pet_badge_broadcast;

	extern char* g_partner_strengthen_broadcast;

	extern char* g_qingyuan_duiduipeng_prom;
	extern char* g_qingyuan_duiduipeng_prom_1;
	extern char* g_qingyuan_duiduipeng_jifen;
	extern char* g_qingyuan_duiduipeng_jifen_1;
	extern char* g_qingyuan_duiduipeng_rank_subject;
	extern char* g_qingyuan_duiduipeng_rank_content;

	extern char* g_tian_gong_xun_bao_broadcast;
	extern char* g_tian_gong_xun_bao_subject;
	extern char* g_tian_gong_xun_bao_content;

	extern char* g_role_hundred_god_boss_time_over_subject;
	extern char* g_role_hundred_god_boss_time_over_content;
	extern char* g_role_hundred_god_boss_reward_reissue_content;

	///// 天下第一比武[组队] 竞猜  /////////////////////////////////////////
	extern char* g_uta_guess_reward_retroactiv_subject;
	extern char* g_uta_guess_reward_retroactiv_content;
	extern char* g_uta_guess_reward_bet_retroactiv_subject;
	extern char* g_uta_guess_reward_bet_retroactiv_content;
	extern char* g_uta_win_box_reward_subject;
	extern char* g_uta_win_box_reward_content;
	extern char* g_uta_win_box_reissue_content;
	extern char* g_uta_today_join_reward_subject;
	extern char* g_uta_today_join_reward_content;
	extern char* g_uta_qualification_prepare_notice;
	extern char* g_uta_main_prepare_notice;
	extern char* g_uta_qualification_enter_notice;
	extern char* g_uta_main_enter_notice;
	extern char* g_uta_server_reward_subject;
	extern char* g_uta_server_reward_content;

	extern char * g_ra_yuan_qi_fan_pai_notice;

	extern char * g_randactivity_unfetch_reward_title;
	extern char * g_randactivity_unfetch_reward_content;

	extern char * g_randactivity_god_beast_advent_subject;
	extern char * g_randactivity_god_beast_advent_content;
	extern char * g_randactivity_god_beast_advent_receive_content;

	extern char* g_pinjia_youli_subject;
	extern char* g_pinjia_youli_content;

	extern char* g_hmtj_world_first_notice;
	extern char* g_hmtj_world_first_reward_subject;
	extern char* g_hmtj_world_first_reward_content;
	extern char* g_hmtj_role_first_reward_subject;
	extern char* g_hmtj_role_first_reward_content;
	extern char* g_hmtj_tean_not_times_can_enter_notice;
	extern char* g_hmtj_team_not_equip_can_enter_notice;
	extern char* g_hmtj_team_not_pass_can_enter_notice;
	extern char* g_hmtj_team_not_level_can_enter_notice;
	extern char* g_hmtj_reward_notice;

	extern char* g_holy_equip_fushi_takeoff_subject;
	extern char* g_holy_equip_fushi_takeoff_content;

	extern char* g_randactivity_duan_yang_xiang_nang_subject;
	extern char* g_randactivity_duan_yang_xiang_nang_content;

	extern char* g_randactivity_salty_sweet_battle_subject;
	extern char* g_randactivity_salty_sweet_battle_content;
	extern char* g_randactivity_salty_sweet_battle_lose_content;
	extern char* g_drop_broadcast;
	extern char * g_randactivity_get_item_broadcast_3;

	extern char * g_randactivity_get_item_broadcast;
	extern char * g_randactivity_get_item_broadcast_2;
	extern char * g_randactivity_zhui_yue_shang_dian_get_item_broadcast;

	extern char * g_meili_qixi_rank_reward_subject;
	extern char * g_meili_qixi_rank_reward_content;

	extern char * g_real_role_robot_reward_subject;
	extern char * g_real_role_robot_reward_content;

	extern char* g_colosseum_battle_reawrd_subject;
	extern char* g_colosseum_battle_reawrd_content;

	extern char * g_role_guild_task_notice_subject;
	extern char * g_role_guild_task_notice_content;

	bool StringInit(const char *filename, std::string *error);
}

#endif // __GAMEWORLDSTR_H__

