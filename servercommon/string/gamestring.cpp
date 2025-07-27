#include "common/config/xmlconfig.h"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/servercommon.h"

namespace gamestring
{
	char GAMESTRING_BUF[2048] = { 0 };

	char *g_leijichongzhi_subbject;
	char *g_leijichongzhi_content;
	char *g_leijichongzhi_day_content;

	char* g_collection_return;
	char* g_add_guild_gongxian;
	char* g_guild_build_donate;
	char* g_guild_dismiss_subject;
	char* g_guild_dismiss_content;
	char* g_guild_demotion_subject;
	char* g_guild_demotion_content;

	char* g_partner_talent_gift;

	char* g_world_boss_subject;
	char* g_world_boss_reward;
	char* g_world_boss_hurt_subject;
	char* g_world_boss_hurt_reward;
	char* g_world_boss_heal_subject;
	char* g_world_boss_heal_reward;
	char* g_world_boss_luck_subject;
	char* g_world_boss_luck_reward;
	char* g_world_boss_kill_subject;
	char* g_world_boss_kill_reward;
	char* g_world_boss_broadcast;
	char* g_world_boss_kill_broadcast;
	char* g_world_boss_not_kill_broadcast;
	char* g_world_boss_appear_broadcast;

	char* g_world_boss_2_subject;
	char* g_world_boss_2_reward;
	char* g_world_boss_2_hurt_subject;
	char* g_world_boss_2_hurt_reward;
	char* g_world_boss_2_heal_subject;
	char* g_world_boss_2_heal_reward;
	char* g_world_boss_2_luck_subject;
	char* g_world_boss_2_luck_reward;
	char* g_world_boss_2_kill_subject;
	char* g_world_boss_2_kill_reward;
	char* g_world_boss_2_broadcast;
	char* g_world_boss_2_kill_broadcast;
	char* g_world_boss_2_not_kill_broadcast;
	char* g_world_boss_2_appear_broadcast;

	char* g_world_boss_3_subject;
	char* g_world_boss_3_reward;
	char* g_world_boss_3_hurt_subject;
	char* g_world_boss_3_hurt_reward;
	char* g_world_boss_3_heal_subject;
	char* g_world_boss_3_heal_reward;
	char* g_world_boss_3_luck_subject;
	char* g_world_boss_3_luck_reward;
	char* g_world_boss_3_kill_subject;
	char* g_world_boss_3_kill_reward;
	char* g_world_boss_3_broadcast;
	char* g_world_boss_3_kill_broadcast;
	char* g_world_boss_3_not_kill_broadcast;
	char* g_world_boss_3_appear_broadcast;

	char* g_shi_tu_subject;
	char* g_shi_tu_reward;
	char* g_shitu_mile_subject;
	char* g_shitu_mile_master_reward;
	char* g_shitu_mile_apprentice_reward;
	char * g_shitu_break_time_notice;
	char * g_shitu_break_notice;

	char* g_knapsack_put_subject;
	char* g_knapsack_put_content;
	char* g_knapsack_full_mail_subject;
	char* g_knapsack_full_send_mail;
	char* g_pet_full_mail_subject;
	char* g_pet_full_send_mail;
	char* g_item_expire_notice_subject;
	char* g_item_expire_notice_content;
	char * g_item_expire_notice_subject_2;
	char * g_item_expire_notice_content_2;
	char * g_item_expire_notice_subject_3;
	char * g_item_expire_notice_content_3;
	char* g_temp_clear_notice_subject;
	char* g_temp_clear_notice_content;
	char* g_item_not_enough_notie_content;
	char* g_item_not_enough_notie_content2;
	char* g_team_member_item_not_enough_notice;

	char* g_system_item_return_subject;
	char* g_partner_item_system_return_content;

	char* g_trade_market_advertise;
	char* g_trade_market_sold_item_get_money;
	char* g_trade_market_focus_success;

	char* g_bounty_task_turn_table_reward_subject;
	char* g_bounty_task_turn_table_reward_content;
	char* g_bounty_task_turn_table_reward_notice;

	char* g_task_chain_turn_table_reward_subject;
	char* g_task_chain_turn_table_reward_content;

	char* g_team_fight_reward_subject;
	char* g_team_fight_rank_reward;
	char* g_team_fight_box_reward;
	char* g_team_fight_box_reward_subject;
	char* g_team_fight_reissue_box_reward_subject;
	char* g_team_fight_reissue_first_box_reward_content;
	char* g_team_fight_reissue_three_box_reward_content;
	char* g_team_fight_join_reward_subject;
	char* g_team_fight_join_reward_content;

	char* g_role_rename_notice;
	char* g_role_rename_subject;
	char* g_role_rename_content;
	char* g_role_advance_content;

	char* g_maze_racer_amulet_drop;
	char* g_maze_racer_reward_subj;
	char* g_maze_racer_rank_reward;

	char* g_brave_ground_reward_subject;
	char* g_brave_ground_reward;

	char* g_guild_honor_reward_subject;
	char* g_guild_honor_reward_content;
	char* g_guild_honor_bye_reward_subject;
	char* g_guild_honor_bye_reward_content;
	char *g_guild_honor_battle_broadcast;				//
	char *g_guild_honor_battle_direct_win_broadcast;
	char * g_guild_honor_settlement_list_notice;
	char * g_guild_honor_battle_start_round_notice;
	char * g_guild_honor_battle_guess_subject;
	char * g_guild_honor_battle_guess_content;
	char * g_guild_honor_battle_guess_champion_subject;
	char * g_guild_honor_battle_guess_champion_content;
	char * g_guild_honor_battle_rank_reward_subject;
	char * g_guild_honor_battle_rank_reward_content;
	char * g_guild_honor_battle_rank_unjoin_reward_content;
	char * g_guild_honor_battle_last_round_battle_notice;

	char * g_guild_building_finish_subject;
	char * g_guild_building_finish_leader_content;
	char * g_guild_building_finish_deputyleader_content;

	char * g_guild_exp_remind_subject;
	char * g_guild_exp_remind_will_overflow_content;
	char * g_guild_exp_remind_overflow_content;

	char* g_guild_fight_rank_reward_title;
	char* g_guild_fight_familys_rank_reward;
	char* g_guild_fight_survival_subject;
	char* g_guild_fight_survival_reward;
	char* g_guild_fight_participation_subject;
	char* g_guild_fight_participation_award;

	char* g_only_fight_rank_reward_subject;
	char* g_only_fight_rank_reward_content;
	char* g_only_fight_survival_subject;
	char* g_only_fight_survival_reward;
	char* g_only_fight_participation_subject;
	char* g_only_fight_participation_award;
	char* g_only_fight_notice;

	char* g_silk_roads_rank_reward_subject;
	char* g_silk_roads_rank_reward_content;
	char* g_silk_roads_settlement_reward_subject;
	char* g_silk_roads_settlement_reward_content;

	char* g_money_chongzhi_subject;
	char* g_money_chongzhi_content;
	char* g_colosseum_subject;
	char* g_colosseum_coin_reward;
	char* g_colosseum_rank_reward_subject;
	char* g_colosseum_rank_reward_content;
	char* g_colosseum_challenge_times_remain_reward_subject;
	char* g_colosseum_challenge_times_remain_reward_content;
	char* g_courage_challenge_reward_content;
	char* g_new_courage_challenge_reward_content;
	char* g_dream_notes_challenge_subject;
	char* g_dream_notes_challenge_content;
	char* g_dream_notes_challenge_reissue;
	char* g_dream_notes_challenge_use_item_add_times_notice;
	char* g_role_level_not_enough_not_challenge;
	char* g_colosseum_battle_reawrd_subject;
	char* g_colosseum_battle_reawrd_content;

	char* g_get_role_uid_float;
	char* g_refuse_qie_cuo_float;
	char* g_scene_unlock_notice;
	char* g_team_recruit;
	char* g_team_create;
	char* g_team_add_member;
	char* g_team_member_leave;
	char* g_team_become_leader;
	char* g_team_be_kicked;
	char* g_team_member_refuse_summon;
	char* g_team_member_level_not_enough;
	char* g_activity_state;
	char* g_activity_sign_up_subject;
	char* g_activity_sign_up_content;
	char* g_guild_week_task_settlement_reward_title;
	char* g_guild_week_task_issue_settlement_reward;
	char* g_guild_week_task_contribute_reward_title;
	char* g_guild_week_task_issue_contribute_reward;
	char* g_guild_week_task_settle_contribute_subject;
	char* g_guild_week_task_settle_contribute_content;
	char* g_guild_boss_chapter_reward_subject;
	char* g_guild_boss_chapter_baodi_reward;
	char* g_guild_boss_chapter_kill_reward_subject;
	char* g_guild_boss_chapter_kill_reward_content;
	char* g_guild_boss_chapter_notice_subject;
	char* g_guild_boss_chapter_notice_content;
	char* g_guild_boss_chapter_rank_subject;
	char* g_guild_boss_chapter_rank_content;
	char* g_guild_boss_chapter_notice;

	char* g_guild_gather_dream_level_not_enough;
	char* g_guild_gather_dream_publish;
	char* g_guild_gather_dream_daychange_reward_subject;
	char* g_guild_gather_dream_daychange_reward_content;
	char* g_guild_gather_Dream_leave_guild_reward_content;
	char* g_guild_gather_dream_reissue_reward_content;

	char* g_salary_reward_subject;
	char* g_salary_reward_content;
	char* g_salary_captain_salary;
	char* g_salary_chivalrous_salary;

	char* g_smart_mounts_system_level_not_engough;
	char* g_smart_mounts_system_skill_up_level_limit;
	char* g_smart_mounts_system_breach_faild_notice;

	char* g_role_guild_inborn_learn_skill_level_not_enough;
	char* g_role_guild_inborn_skill_points_not_enough;
	char* g_role_guild_inborn_reset_skill_subject;
	char* g_role_guild_inborn_reset_skill_content;

	char* g_role_huan_shou_open_egg_rare_broadcast_content;

	char* g_realm_break_offline_info_pool_notice;

	char *g_chat_open_level;

	char* g_platform_battle_reward_subject;
	char* g_platform_battle_reward_content;
	char * g_platform_battle_reward_content2;

	char* g_chief_election_no_winner_broadcast;
	char* g_chief_election_champion_broadcast;
	char* g_chief_election_reward_subject;
	char* g_chief_election_reward_content;
	char * g_chief_election_battle_reward_subject;
	char * g_chief_election_battle_reward_content;
	char * g_chief_election_sea_reward_subject;
	char * g_chief_election_sea_reward_content;
	char * g_chief_election_sea_tip1;		//由于对方不在场或者处于无法战斗中，恭喜晋级下一轮比赛
	char * g_chief_election_sea_tip2;		//由于您处于无法战斗中，此轮比赛失败
	char * g_chief_election_sea_tip3;		//由于无法再规定时间或回合里分出胜负，由角色+宠物评分高那方获胜
	char * g_chief_election_sea_tip4;		//由于战斗用时超过此轮比赛最大用时，无法晋级下一轮比赛

	char* g_chief_peak_battle_nobody_join_broadcast;
	char* g_chief_peak_battle_champion_broadcast;
	char* g_activity_chief_peak_battle_rank_reward_subject;
	char* g_activity_chief_peak_battle_rank_reward_content;
	char* g_activity_chief_peak_battle_bet_reward_subject;
	char* g_activity_chief_peak_battle_bet_reward_content;
	char * g_activity_chief_peak_battle_battle_reward_subject;
	char * g_activity_chief_peak_battle_battle_reward_content;

	char* g_put_prestige_notie;
	char* g_prestige_uplevel_notie;

	char* g_course_of_growth_uplevel_subject;
	char* g_course_of_growth_uplevel_content;
	char *g_chongzhi_reward_subject;
	char *g_chongzhi_reward_content;
	char *g_chongzhi_reward_coin_content;
	char *g_month_investment_subject;
	char *g_month_investment_content;
	char *g_first_recharge_subject;
	char *g_first_recharge_content;
	char *g_first_recharge_exclusive;

	char *g_month_week_card_reward_subject;
	char *g_buy_month_week_card_reward_content;
	char *g_buy_moth_week_card_item_content;

	char* g_mail_collection_content;
	char* g_mail_collection_main_content;

	char* g_mail_change_prof_subject;
	char* g_mail_change_prof_content;

	char * g_mail_bountytask_turntable;

	char* g_join_guild;
	char* g_agree_invite_but_full;
	char* g_refuse_invite;
	char* g_refuse_invite_notice;
	char* g_funopen_task_limit;
	char* g_trademark_buy_succ_content;
	char* g_task_adv_succ_content;
	char* g_task_reissue_item_subject;
	char* g_task_reissue_item_content;

	char* g_sold_succ_send_global_message;
	char* g_chairman_to_newbie_message;
	char* g_buff_effect_mp_message;
	char* g_buff_effect_hp_message;
	char* g_buff_effect_hpmp_message;
	char* g_knapsack_on_consume_message;
	char* g_knapsack_on_consume_message2;
	char* g_money_on_consume_money_message;
	char* g_buff_effect_attr_message;
	char* g_buff_bad_effect_attr_message;

	char* g_battle_server_disconnect;
	char* g_bigcross_battle_server_disconnect;
	char* g_fabao_level_limit_message;
	char* g_fabao_uplevel_limit_message;
	char* g_fabao_back_subject;
	char* g_fabao_back_content;
	char* g_fabao_back_subject2;
	char* g_fabao_back_content2;

	char* g_patrol_task_message;
	char* g_patrol_task_subject;
	/////////////////////////////////////// 系统回收 ///////////////////////////////////////////
	char *g_retrieve_mail_subject;
	char *g_retrieve_mail_content_refund_money;
	char *g_retrieve_mail_content_indebt_money;
	char *g_retrieve_mail_content_retrieve_debt;
	char *g_retrieve_mail_content_bug_money;
	char *g_retrieve_mail_content_refund_item;
	char *g_retrieve_mail_content_bug_item;
	char *g_retrieve_mail_content_exp;

	char *g_retrieve_mail_content_refund_other_money;
	char *g_retrieve_mail_content_indebt_other_money;
	char *g_retrieve_mail_content_retrieve_debt_other_money;
	char *g_retrieve_mail_content_bug_other_money;

	char* g_server_first_kill_subject;
	char* g_server_first_kill_content;
	char* g_server_first_kill_miyi;
	char* g_server_first_kill_boss;
	char* g_server_first_kill_challenge;
	char* g_server_first_kill_shan_hai;
	char* g_server_first_kill_big_dipper;
	char* g_server_first_kill_fallen_god;
	char* g_server_first_kill_hmtj_fb;

	char* g_wild_boss_refresh_notice;

	char* g_treasure_item_notice1;
	char* g_treasure_item_notice2;

	char* g_advancefb_task_notic;

	char* g_world_question_subject;
	char* g_world_question_content;

	char* g_seven_color_picture_subject;
	char* g_seven_color_picture_content;
	char* g_seven_color_picture_total_content;
	char* g_seven_color_picture_hearsay_content;

	char* g_animal_race_subject;
	char* g_animal_race_content;

	char* g_happy_up_hearsay_content;

	char * g_xun_bao_broadcast;
	char * g_xun_bao_partner_broadcast;

	char* g_face_score_active_succ;
	char* g_face_score_is_inactive;

	char* g_system_post_partner;
	char* g_system_post_equipment_max;
	char* g_system_post_pet_max;

	char * g_workshop_strone_level_notice;
	char * g_workshop_strone_puton_level_notice;
	char *g_synthesis_info_1;
	char *g_synthesis_info_2;
	char *g_synthesis_info_3;
	char *g_synthesis_info_4;
	char *g_synthesis_info_5;
	char *g_synthesis_info_6;
	char * g_compound_pet_equipment_broadcast;		//宠物护符新增传闻

	char * g_heritage_treasure_broadcast;

	char * g_tian_jiang_hao_li_hearsay;

	char * g_randactivity_expelmonster_notice;
	char * g_randactivity_fortune_maze_notice;

	char * g_randactivity_partner_exchange_notice;

	char * g_randactivity_collect_word_broadcast;

	char * g_randactivity_pet_exchange_notice;
	char * g_pet_compose;
	char * g_randactivity_adventure_gift_broadcast;

	char * g_randactivity_make_up_reward_subject;
	char * g_randactivity_make_up_reward_content;

	char *g_randactivity_lucky_turntable_content;
	char *g_rand_activity_tianming_chou_content;
	char *g_rand_activity_tian_ming_divination_reward_content;

	char* g_randactivity_quan_min_discount_subject;
	char* g_randactivity_quan_min_discount_message;
	
	char* g_randactivity_good_gift_turn_subject;
	char* g_randactivity_good_gift_turn_content;

	char* g_randactivity_help_others_subject;
	char* g_randactivity_help_others_content;

	char * g_randactivity_help_others_2_subject;
	char * g_randactivity_help_others_2_content;

	char * g_randactivity_shan_hai_question_subject;
	char * g_randactivity_shan_hai_question_sontent;

	char * g_randactivity_fate_entanglement_subject;
	char * g_randactivity_fate_entanglement_content;

	char * g_randactivity_yun_ze_territory_subject;
	char * g_randactivity_yun_ze_territory_content;

	char * g_randactivity_yunze_catch_elves_subject;
	char * g_randactivity_yunze_catch_elves_content;

	char * g_randactivity_yunze_hao_li_subject;
	char * g_randactivity_yunze_hao_li_content;

	char* g_randactivity_secret_treasure_subject;
	char* g_randactivity_secret_treasure_content;

	char* g_randactivity_yun_ze_slot_machine_subject;
	char* g_randactivity_yun_ze_slot_machine_content;

	char* g_randactivity_yun_ze_er_san_shi_subject;
	char* g_randactivity_yun_ze_er_san_shi_content;

	char* g_randactivity_yun_ze_blooming_record_subject;
	char* g_randactivity_yun_ze_blooming_record_content;

	char* g_randactivity_tiger_auspicious_snow_subject;
	char* g_randactivity_tiger_auspicious_snow_content;

	char* g_randactivity_tiger_find_spring_yesterday_content;

	char* g_trademarket_announce_notice;
	char* g_trademarket_onsale_notice;

	char *g_turntable_message_0;
	char *g_turntable_message_1;
	char *g_turntable_message_2;
	char *g_turntable_message_3;
	char *g_turntable_message_4;

	char* g_chat_group_subject;
	char* g_chat_group_dismiss_content;
	char* g_chat_group_tick_out_content;
	char* g_single_chat_is_limit_notice;

	char* g_ra_sports_subject;
	char* g_ra_sports_histroy_content;
	char* g_ra_sports_settlement_content;

	char * g_huan_jie_zhan_chang_rank_reward_subject;
	char * g_huan_jie_zhan_chang_rank_reward_content;
	char * g_huan_jie_zhan_chang_settlement_reward_subject;
	char * g_huan_jie_zhan_chang_settlement_reward_content;
	char * g_huan_jie_zhan_chang_ranking_list_reward_subject;
	char * g_huan_jie_zhan_chang_ranking_list_reward_content;
	char * g_huan_jie_zhan_chang_today_win_reward_subject;
	char * g_huan_jie_zhan_chang_today_win_reward_content;

	char * g_partner_crytals_remove;
	char * g_partner_crytals_done;
	char * g_partner_quality_pur_to_ora;
	char * g_partner_quality_ora_to_red;
	char * g_partner_quality_upgrade;
	char * partner_quality_revert_subject;
	char * partner_quality_revert_content;
	char * partner_skin_notice;
	char * partner_skin_notice_2;

	char * g_enter_miyi;
	char * g_enter_miyi_2;
	char * g_miyi_poker_title;
	char * g_miyi_poker_reward_content;

	char * g_chat_group_invite_fail_content;
	char * g_chat_group_invite_succ_content;
	char * g_add_friend_succ_content;

	char * prestige_over;
	char * g_prestige_use_limit;
	char * person_rank_prof_capability_top_change_broadcast;

	char * g_math_team_fail_content;
	char * g_crytals_reward_content;
	char * g_crytals_reward2_content;

	char * g_wildboss_poker_title;
	char * g_wildboss_poker_reward_content;
	char * g_wildboss_poker_reward_notice;

	char * g_shanhaiboss_poker_title;
	char * g_shanhaiboss_poker_reward_content;
	char* g_shanhaiboss_broadcast;
	char * g_shanhaiboss_reward_notice;

	char * g_crystal_fb_poker_reward_subject;
	char * g_crystal_fb_poker_reward_content;
	char * g_crystal_fb_poker_reward_notice;

	char * g_wan_ling_fb_reward_notice;

	char * g_god_token_surplus_title;
	char * g_god_token_surplus_reward_content;

	char * g_god_token_level_reward_title;
	char * g_god_token_level_reward_content;

	char * g_chong_zhi_buy_item_reward_title;
	char * g_chong_zhi_buy_item_reward_content;

	char * g_pet_strengthen_broadcast;

	char * g_has_poker_get;
	char * g_guild_up_level;

	char * g_medal_up_grade;
	char * g_medal_lottery_broadcast;
	char * g_trademarket_give_item_to_user_offline_subject;
	char * g_trademarket_give_item_to_user_offline_content;

	char * g_anecdote_task_reward_by_mail_subject;
	char * g_anecdote_task_reward_by_mail_content;
	char * g_anecdote_task_reward_by_mail_knapsack_full;
	char * g_anecdote_big_event_reward_by_mail_subject;
	char * g_anecdote_big_event_reward_by_mail_content;


	char * g_retrieve_pet_notice;
	char * g_retrieve_pet_return_equip;

	char * g_stone_mail_back_subject;
	char * g_stone_mail_back_content;

	/////////////////////////// 翻牌 ///////////////////////////////////////////
	char *g_draw_reward_subject;
	char *g_draw_reward_content;

	char *g_randactivity_reward_subject;
	char *g_randactivity_reward_subject_1;
	char *g_randactivity_reward_content;
	char *g_randactivity_reward_end_content;
	char *g_randactivity_reward_buy_limit_content;
	char *g_randactivity_bag_full_subject;
	char *g_randactivity_bag_full_content;

	char *g_ra_server_competition_end_rank_content;
	char *g_ra_server_competition_end_broadcast;
	char *g_pet_add_exp_err_without_break_level;

	char *g_norex_item_use_item_invalid;
	char *g_norex_item_use_item_num_limit;

	char *g_life_skill_gather_notice;
	char *g_life_skill_gather_double_notice;
	char *g_life_skill_fish_notice;
	char *g_life_skill_fish_double_notice;
	char *g_life_skill_consume_gather_power;
	char *g_life_skill_level_up_notice;
	char *g_life_skill_change_surface_noitce;
	char *g_life_skill_surface_point_not_enough;

	char *g_buy_extra_times_notice;
	char *g_crystal_fb_use_energy_item;
	char *g_randactivity_fortune_maze_subject;
	char *g_randactivity_fortune_maze_content;

	char *g_kua_fu_ying_xiong_lu_rank_subject;
	char *g_kua_fu_ying_xiong_lu_rank_content;
	char *g_kua_fu_ying_xiong_lu_bag_full_subject;
	char *g_kua_fu_ying_xiong_lu_bag_full_content;
	char *g_kua_fu_ying_xiong_lu_rand_end_task_reward_subject;
	char *g_kua_fu_ying_xiong_lu_rand_end_task_reward_content;

	char* g_order_return_resource_subject;
	char* g_order_upshelve_return_resource;
	char* g_order_buy_return_resource;
	char* g_order_advertise_return_resource;

	char* g_new_version_auto_offshelve_subject;
	char* g_new_version_auto_offshelve_content;

	char * g_act_full_bag_title;
	char * g_act_full_bag_content;

	char * guild_tan_he_notice_title;
	char * guild_tan_he_notice_content;
	char * guild_tan_he_succ_title;
	char * guild_tan_he_succ_content;

	char * g_shen_shou_jiang_lin_broadcast_1;
	char * g_shen_shou_jiang_lin_broadcast_2;
	char * g_shen_shou_jiang_lin_content_1;
	char * g_shen_shou_jiang_lin_content_2;

	char * g_temp_chat_group_name;
	char * g_chat_group_msg_notice_join_temp;
	char * g_chat_group_msg_notice_create_temp;
	char * g_chat_group_msg_notice_become_official;
	char * g_teammate_chat_group_full;
	char * g_create_chat_group_level_limit_notice;
	char * g_create_chat_group_cd_limit_notice;
	char * g_auto_assign_attribute_points_notice;
	char * g_welcome_join_temp_group_notice;

	char * g_activity_trade_return_gold_subject;
	char * g_activity_trade_return_gold_content;

	char * g_god_equip_create_notice;
	char * g_cumulative_recharge_notice;
	char * g_gift_randitem_is_broadcast_notice;

	char * g_clear_mixed_element_crystal_subject;
	char * g_clear_mixed_element_crystal_content;

	char * g_brave_ground_sao_dang_subject;
	char * g_brave_ground_sao_dang_content;
	char * g_brave_ground_reset_subject;
	char * g_brave_ground_reset_content;
	char * g_brave_ground_reward_reissue_ver1_subject;
	char * g_brave_ground_reward_reissue_ver1_content;

	char * g_mi_yi_first_kill_subject;
	char * g_mi_yi_first_kill_content;

	char * g_chun_ri_deng_feng_broadcast;
	char * g_lao_dong_hao_li_broadcast;
	char * g_lao_dong_hao_li_end_broadcast;

	char * g_role_convert_buy_front_item_not_buy_notice;
	char * g_has_order;			//由于上一个订单未完成，请等待%d秒再试
	
	char * g_top_level_subject;
	char * g_top_level_content;
	
	char * g_ra_reward_borcast;
	char * g_ra_reward_borcast2;
	char * g_ling_pai_tian_ci_borcast;	
	char * g_xin_fa_wu_liang_borcast;
	char * g_fu_shi_hun_qi_borcast;
	char * g_yun_ze_er_san_shi_borcast;
	char * g_yun_ze_er_san_shi_marquee_borcast;

	char * g_chat_group_handover_leadership_succ;
	char * g_chat_group_leader_handovered_to_you;
	char * g_chat_group_leadership_handovered;
	char * g_ra_qiong_lou_yu_yan_subject;
	char * g_ra_qiong_lou_yu_yan_content;

	char * g_lun_hui_wang_chuan_subject;
	char * g_lun_hui_wang_chuan_content;

	char * g_energy_add_notice;

	char * g_cloud_arena_season_reward_subject;
	char * g_cloud_arena_season_reward_content;

	char * g_world_team_arena_team_member_level_not_enough;
	char * g_world_team_arena_team_member_cancel_sign_up;
	char * g_world_team_arena_team_member_logout;
	char * g_world_team_arena_team_member_leave_team;
	char * g_world_team_arena_create_team_succ_notice;
	char * g_world_team_arena_create_team_succ_borcast;
	char * g_world_team_arena_user_particip_subject;
	char * g_world_team_arena_user_particip_content;
	char * g_world_team_arena_team_rank_subject;
	char * g_world_team_arena_team_rank_content;
	char * g_world_team_arena_top_team_subjcet;
	char * g_world_team_arena_top_team_content;
	char * g_world_team_arena_team_standy_brocast;
	char * g_world_team_arena_team_can_enter_brocast;

	char * g_relic_looting_rank_reward_subject;
	char * g_relic_looting_rank_reward_content;

	char * g_lun_hui_wang_chuan_reward_subject;
	char * g_lun_hui_wang_chuan_reward_content;

	char * g_relic_looting_reward_subject;
	char * g_relic_looting_reward_content;
	
	char * g_heart_skill_displace_failed_notice;
	char * g_role_recommend_score_not_enough_notice;

	char * g_crystal_fb_xiyou_times_add_notice;

	char * g_god_equip_return_subject;
	char * g_god_equip_return_content;

	char * g_transfer_subject;
	char * g_transfer_content;
	
	
	// 家族活跃度
	char * g_guild_active_day_active_subject;
	char * g_guild_active_day_active_content;
	char * g_guild_active_week_active_rank_subject;
	char * g_guild_active_week_active_rank_content;
	char * g_guild_active_week_active_person_rank_subject;
	char * g_guild_active_week_active_person_rank_content;

	char * g_role_join_activity_reward_subject;
	char * g_role_join_activity_reward_content;

	char * g_ra_pai_mai_hang_bid_subject;
	char * g_ra_pai_mai_hang_bid_succ_content;
	char * g_ra_pai_mai_hang_bid_succ_content1;
	char * g_ra_pai_mai_hang_bid_failed_content;
	char * g_ra_pai_mai_hang_bid_failed_notice;
	char * g_ra_pai_mai_hang_bid_set_add_price_notice;


	char * g_chief_election_winner_login_broadcast;
	char * g_guild_honor_winner_login_broadcast;
	char * pet_refine_bian_yi_broadcast;		//宠物变异传闻
	char * g_pet_unlock_talent_group_str_level_not_enough;
	char * g_pet_talent_reset_return_consume_item_subject;
	char * g_pet_talent_reset_return_consume_item_content;

	char * g_return_item_subject;
	char * g_return_item_content;
	char * g_return_item_subject2;
	char * g_return_item_content2;
	char * g_return_item_subject3;
	char * g_return_item_content3;
	char * g_return_item_subject4;
	char * g_return_item_content4;
	char * g_return_item_subject5;
	char * g_return_item_content5;
	char * g_return_item_subject6;
	char * g_return_item_content6;
	char * g_return_item_subject7;
	char * g_return_item_content7;
	char * g_return_item_subject8;
	char * g_return_item_content8;
	char * g_return_item_subject9;
	char * g_return_item_content9;
	char * g_return_item_subject10;
	char * g_return_item_content10;
	char * g_return_item_subject11;
	char * g_return_item_content11;
	char * g_return_item_subject12;
	char * g_return_item_content12;
	char * g_return_item_subject13;
	char * g_return_item_content13;
	char * g_return_item_subject14;
	char * g_return_item_content14;
	char * g_return_item_subject15;
	char * g_return_item_content15;

	char * g_change_card_point_recovery_subject;
	char * g_change_card_point_recovery_content;

	char * g_nether_end_change_subject;
	char * g_nether_end_change_content;
	char * g_nether_end_change_silver_content;
	char * g_nether_end_change_gold_content;
	char * g_nether_end_change_final_content;

	char * g_pet_god_print_wish_broadcast;
	char * g_pet_god_print_wish_broadcast_2;

	char * g_hundred_ghost_poker_title;
	char * g_hundred_ghost_poker_reward_content;
	char * g_hundred_ghost_poker_reward_notice;
	char * g_hundred_ghost_broadcast;
	char * g_hundred_ghost_big_boss_broadcast;

	char * g_hongbao_notify_content;

	char * give_gift_back_item_subject;
	char * give_gift_back_item_content;	
	char * g_give_flower_notice;
	char * g_give_flower_back_kiss;

	char * g_god_equip_back_subject;
	char * g_god_equip_back_content;

	char * fang_fraud_send_mail_subject;
	char * fang_fraud_send_mail_content;

	char * g_ra_back_reward_send_mail_subject;
	char * g_ra_back_reward_send_mail_content;

	//决斗
	char * duel_begin_duel_battle;
	char * duel_begin_duel_battle_team;
	char * duel_begin_duel_two_battle;
	char * duel_begin_duel_two_battle_team;
	char * duel_red_name_login_cross;

	char * duel_lose_reduce_exp;
	char * duel_lose_reduce_exp_team;
	char * duel_lose_reduce_coin;
	char * duel_lose_reduce_coin_team;
	char * duel_lose_reduce_kill_value;
	char * duel_lose_reduce_kill_value_team;

	char * duel_type_one_attack_win;
	char * duel_type_one_attack_win_team;
	char * duel_type_one_defend_win;
	char * duel_type_one_defend_win_team;
	char * duel_type_one_defend_win_red;
	char * duel_type_one_defend_win_red_team;

	char * duel_type_two_attack_win;
	char * duel_type_two_attack_win_team;
	char * duel_type_two_defend_win;

	char * duel_type_three_attack_win;
	char * duel_type_three_defend_win;
	char * duel_type_three_defend_win_team;
	char * duel_type_three_defend_lose;
	char * duel_type_three_defend_lose_team;

	char * duel_win_add_coin;
	char * duel_win_add_coin_team;
	char * duel_end_duel_battle;
	char * duel_begining_duel_two;
	char * duel_lose_duel_two;

	//决斗

	char * g_capacity_target_reward_subject;
	char * g_capacity_target_reward_content;

	char * g_randactivity_lucky_bag_subject;
	char * g_randactivity_lucky_bag_content;

	char * g_god_equip_back_break_subject;
	char * g_god_equip_back_break_content;

	char * g_partner_tree_return_subject;
	char * g_partner_tree_return_content;
		
	char * g_randactivity_horcrux_tiancheng_broadcast_1;
	char * g_randactivity_horcrux_tiancheng_broadcast_2;

	char * g_randactivity_integral_draw_broadcast;
	char * g_god_equip_break_through_broadcast;

	char * g_ra_shan_hai_meet_subject;
	char * g_ra_shan_hai_meet_content;

	char * g_randactivity_zhaocaijinbao_broadcast;

	char * g_endless_tower_rank_reward_subject;
	char * g_endless_tower_rank_reward_content;

	char * g_role_skill_chagne_return_subject;
	char * g_role_skill_change_return_content;
	char * g_role_skill_chagne_return_subject_2;
	char * g_role_skill_change_return_content_2;

	char* g_jie_yi_add_member_subject;
	char* g_jie_yi_add_member_content;
	char* g_jie_yi_leave_subject;
	char* g_jie_yi_leave_content;
	char* g_jie_yi_kick_subject;
	char* g_jie_yi_kick_content;
	char* g_jie_yi_dismiss_subject;
	char* g_jie_yi_dismiss_content;
	char * g_jie_yi_mod_ver_0_subject;
	char * g_jie_yi_mod_ver_0_content;
	char * g_jie_yi_create_broadcast;
	char * g_jie_yi_tan_he_subject;
	char * g_jie_yi_tan_he_content;
	char * g_jie_yi_break_tan_he_content;

	char * g_cow_report_good_news_subject;
	char * g_cow_report_good_news_win_content;
	char * g_cow_report_good_news_lose_content;
	char * g_cow_report_good_news_notice;
	char * g_cow_report_good_news_remind;

	char * g_tiger_brings_blessing_subject;
	char * g_tiger_brings_blessing_win_content;
	char * g_tiger_brings_blessing_lose_content;
	char * g_tiger_brings_blessing_notice;
	char * g_tiger_brings_blessing_remind;

	char * g_randactivity_jixing_gaozhao_broadcast_1;
	char * g_randactivity_jixing_gaozhao_broadcast_2;
	char * g_randactivity_yinhu_xianshi_notice;
	char * g_randactivity_ciniu_yinxinghu_notice;

	char * g_reliclooting_open_box_notice;
	char * g_reliclooting_cancle_notice;
	char * g_reliclooting_catch_player_notice;

	char * g_flyup_succ_notice_1;
	char * g_flyup_succ_notice_2;

	char * g_randactivity_jingji_yueka_reissue_subject;
	char * g_randactivity_jingji_yueka_reissue_content;
	char * g_world_arena_standby_reminder;
	char * g_world_arena_force_start_battle_reminder;
	char * g_world_arena_battle_end_leave_scene;
	char * g_world_arena_season_rank_reward_subject;
	char * g_world_arena_season_rank_reward_content;
	char * g_world_arena_someone_leave_scene_wait_notice;
	char * g_world_arena_fight_result_subject;
	char * g_world_arena_not_show_up;
	char * g_world_arena_schedule_release_notice_subject;
	char * g_world_arena_schedule_release_notice_content;
	char * g_world_arena_win_box_reward_subject;
	char * g_world_arena_win_box_reward_content;
	char * g_world_arena_day_reward_subject;
	char * g_world_arena_day_reward_content;
	char * g_world_arena_signup_reward_subject;
	char * g_world_arena_signup_reward_content;
	char * g_world_arena_opponent_not_showup_yet;
	char * g_world_arena_opponent_showup;
	char * g_world_arena_start_battle_failed_draw;
	char * g_world_arena_start_battle_failed_win;
	char * g_world_arena_start_battle_failed_lose;
					     
	//情缘
	char * g_wedding_meal_notice;
	char * g_wedding_fb_bless_notice;
	char * g_wedding_invite_subject;
	char * g_wedding_invite_content;
	char * g_wedding_regist_subject;
	char * g_wedding_regist_content;
	char * g_wedding_xun_you_subject;
	char * g_wedding_xun_you_content;
	char * g_wedding_xun_you_notice;

	char * g_qing_yuan_create_broadcast;
	char * g_qing_yuan_dismiss_subject;
	char * g_qing_yuan_dismiss_content;
	char * g_qing_yuan_give_gift_subject;
	char * g_qing_yuan_give_gift_content;
	char * g_qing_yuan_duration_reward_subject;
	char * g_qing_yuan_duration_reward_content;

	char * g_relic_looting_rank_subject_compensate_1;
	char * g_relic_looting_rank_content_compensate_1;
	char * g_relic_looting_rank_subject_compensate_2;
	char * g_relic_looting_rank_content_compensate_2;
	char * g_relic_looting_rank_subject_compensate_3;
	char * g_relic_looting_rank_content_compensate_3;

	//天下至尊令
	char * g_peak_token_notice;
	char * g_peak_token_reward_notice;


	char * g_ra_yuan_qi_fan_pai_notice;
				     
	///////////////////////////////////////	 开随机礼包  /////////////////////////////////////////
	char *g_open_randgift[OPEN_RANDGIFT_BROADCAST_ID_MAX_NUM];

	char * g_treasure_task_notice;
	char * g_treasure_task_subject;
	char * g_treasure_task_content;

	char * g_ra_compensation_subject;
	char * g_ra_compensation_content;

	char * g_fake_announcement_content_1;
	char * g_fake_announcement_content_2;

	char * g_fallen_god_rank_reward_subject;
	char * g_fallen_god_rank_reward_content;

	char* g_mini_game_subject;
	char* g_mini_game_content;

	char* g_guild_battle_rank_subject;
	char* g_guild_battle_rank_content;

	char* g_guild_rich_rank_subject;
	char* g_guild_rich_rank_content;

	char* g_guild_kinder_rank_subject;
	char* g_guild_kinder_rank_content;

	char * g_get_pet_badge_broadcast;

	char* g_partner_strengthen_broadcast;

	char* g_qingyuan_duiduipeng_prom;
	char* g_qingyuan_duiduipeng_prom_1;
	char* g_qingyuan_duiduipeng_jifen;
	char* g_qingyuan_duiduipeng_jifen_1;
	char* g_qingyuan_duiduipeng_rank_subject;
	char* g_qingyuan_duiduipeng_rank_content;

	char* g_tian_gong_xun_bao_broadcast;
	char* g_tian_gong_xun_bao_subject;
	char* g_tian_gong_xun_bao_content;

	char* g_role_hundred_god_boss_time_over_subject;
	char* g_role_hundred_god_boss_time_over_content;
	char* g_role_hundred_god_boss_reward_reissue_content;

	///// 天下第一比武[组队] 竞猜  /////////////////////////////////////////
	char* g_uta_guess_reward_retroactiv_subject;
	char* g_uta_guess_reward_retroactiv_content;
	char* g_uta_guess_reward_bet_retroactiv_subject;
	char* g_uta_guess_reward_bet_retroactiv_content;
	char* g_uta_win_box_reward_subject;
	char* g_uta_win_box_reward_content;
	char* g_uta_win_box_reissue_content;
	char* g_uta_today_join_reward_subject;
	char* g_uta_today_join_reward_content;
	char* g_uta_rank_reward_subject;
	char* g_uta_qualification_rank_reward_content;
	char* g_uta_main_rank_reward_content;
	char* g_uta_advance_reward_subject;
	char* g_uta_advance_reward_content;
	char* g_uta_qualification_prepare_notice;
	char* g_uta_main_prepare_notice;
	char* g_uta_qualification_enter_notice;
	char* g_uta_main_enter_notice;
	char* g_fastforward_battle_continue;
	char* g_uta_server_reward_subject;
	char* g_uta_server_reward_content;

	char * g_randactivity_unfetch_reward_title;
	char * g_randactivity_unfetch_reward_content;

	char * g_randactivity_god_beast_advent_subject;
	char * g_randactivity_god_beast_advent_content;
	char * g_randactivity_god_beast_advent_receive_content;

	char* g_pinjia_youli_subject;
	char* g_pinjia_youli_content;

	char* g_hmtj_world_first_notice;
	char* g_hmtj_world_first_reward_subject;
	char* g_hmtj_world_first_reward_content;
	char* g_hmtj_role_first_reward_subject;
	char* g_hmtj_role_first_reward_content;
	char* g_hmtj_tean_not_times_can_enter_notice;
	char* g_hmtj_team_not_equip_can_enter_notice;
	char* g_hmtj_team_not_pass_can_enter_notice;
	char* g_hmtj_team_not_level_can_enter_notice;
	char* g_hmtj_reward_notice;

	char* g_holy_equip_fushi_takeoff_subject;
	char* g_holy_equip_fushi_takeoff_content;

	char* g_randactivity_duan_yang_xiang_nang_subject;
	char* g_randactivity_duan_yang_xiang_nang_content;

	char* g_randactivity_salty_sweet_battle_subject;
	char* g_randactivity_salty_sweet_battle_content;
	char* g_randactivity_salty_sweet_battle_lose_content;

	char* g_drop_broadcast;

	char * g_randactivity_get_item_broadcast;
	char * g_randactivity_get_item_broadcast_2;

	char * g_randactivity_get_item_broadcast_3;
	char * g_randactivity_zhui_yue_shang_dian_get_item_broadcast;

	char * g_meili_qixi_rank_reward_subject;
	char * g_meili_qixi_rank_reward_content;

	char * g_real_role_robot_reward_subject;
	char * g_real_role_robot_reward_content;

	char * g_role_guild_task_notice_subject;
	char * g_role_guild_task_notice_content;

#define INIT_GAME_STR(PATH, STR, FMT)\
		{\
			bool ret = false;\
			std::string s_tmp;\
			ret = config.get_item("/GameString/" PATH, &s_tmp);\
			if (!ret)\
			{\
				*error = PATH;\
				return false;\
			}\
			STR = new char[(int)s_tmp.size() + 1];\
			strcpy(STR, s_tmp.c_str());\
			ret = CheckFmt(STR, FMT);\
			if (!ret)\
			{\
				*error = PATH;\
				return false;\
			}\
		}

	bool CheckFmt(const char *str, const char *fmt)
	{
		const char *fmt_p = fmt;
		const char * str_p = str;
		while (*fmt_p)
		{
			while (*str_p)
			{
				if (*str_p == '%' && *(str_p + 1) == '%')
				{
					str_p += 2;
					continue;
				}

				if (*str_p++ != '%')
				{
					continue;
				}

				break;
			}

			if (*str_p != *fmt_p++)
			{
				return false;
			}
		}
		return true;
	}

	bool StringInit(const char * filename, std::string * error)
	{
		XMLConfig config; 
		if (!config.read(filename))
		{
			*error = "Read string.xml failed, check xml format or file existence";
			return false;
		}

		INIT_GAME_STR("GameWorld/RandActivity/ra_randactivity_unfetch_reward_title", g_randactivity_unfetch_reward_title, "d");
		INIT_GAME_STR("GameWorld/RandActivity/ra_randactivity_unfetch_reward_content", g_randactivity_unfetch_reward_content, "d");

		INIT_GAME_STR("GameWorld/GetItem/get_pet_badge_broadcast", g_get_pet_badge_broadcast, "dsd");

		INIT_GAME_STR("GameWorld/GuildRank/guild_battle_rank_subject", g_guild_battle_rank_subject, "");
		INIT_GAME_STR("GameWorld/GuildRank/guild_battle_rank_content", g_guild_battle_rank_content, "d");
		INIT_GAME_STR("GameWorld/GuildRank/guild_rich_rank_subject", g_guild_rich_rank_subject, "");
		INIT_GAME_STR("GameWorld/GuildRank/guild_rich_rank_content", g_guild_rich_rank_content, "d");
		INIT_GAME_STR("GameWorld/GuildRank/guild_kinder_rank_subject", g_guild_kinder_rank_subject, "");
		INIT_GAME_STR("GameWorld/GuildRank/guild_kinder_rank_content", g_guild_kinder_rank_content, "d");

		INIT_GAME_STR("GameWorld/relic_looting/relic_looting_rank_subject_compensate_1", g_relic_looting_rank_subject_compensate_1, "");
		INIT_GAME_STR("GameWorld/relic_looting/relic_looting_rank_content_compensate_1", g_relic_looting_rank_content_compensate_1, "");
		INIT_GAME_STR("GameWorld/relic_looting/relic_looting_rank_subject_compensate_2", g_relic_looting_rank_subject_compensate_2, "");
		INIT_GAME_STR("GameWorld/relic_looting/relic_looting_rank_content_compensate_2", g_relic_looting_rank_content_compensate_2, "");
		INIT_GAME_STR("GameWorld/relic_looting/relic_looting_rank_subject_compensate_3", g_relic_looting_rank_subject_compensate_3, "");
		INIT_GAME_STR("GameWorld/relic_looting/relic_looting_rank_content_compensate_3", g_relic_looting_rank_content_compensate_3, "");

		INIT_GAME_STR("GameWorld/flyup_succ_notice_1", g_flyup_succ_notice_1, "ds");
		INIT_GAME_STR("GameWorld/flyup_succ_notice_2", g_flyup_succ_notice_2, "ds");
		INIT_GAME_STR("GameWorld/reliclooting_cancle_notice", g_reliclooting_cancle_notice, "ds");
		INIT_GAME_STR("GameWorld/reliclooting_catch_player_notice", g_reliclooting_catch_player_notice, "d");
		INIT_GAME_STR("GameWorld/reliclooting_open_box_notice", g_reliclooting_open_box_notice, "d");
		INIT_GAME_STR("GameWorld/JieYi/g_jie_yi_add_member_subject", g_jie_yi_add_member_subject, "");
		INIT_GAME_STR("GameWorld/JieYi/g_jie_yi_add_member_content", g_jie_yi_add_member_content, "ds");
		INIT_GAME_STR("GameWorld/JieYi/g_jie_yi_leave_subject", g_jie_yi_leave_subject, "");
		INIT_GAME_STR("GameWorld/JieYi/g_jie_yi_leave_content", g_jie_yi_leave_content, "ds");
		INIT_GAME_STR("GameWorld/JieYi/g_jie_yi_kick_subject", g_jie_yi_kick_subject, "");
		INIT_GAME_STR("GameWorld/JieYi/g_jie_yi_kick_content", g_jie_yi_kick_content, "ds");
		INIT_GAME_STR("GameWorld/JieYi/g_jie_yi_dismiss_subject", g_jie_yi_dismiss_subject, "");
		INIT_GAME_STR("GameWorld/JieYi/g_jie_yi_dismiss_content", g_jie_yi_dismiss_content, "");
		INIT_GAME_STR("GameWorld/JieYi/g_jie_yi_mod_ver_0_subject", g_jie_yi_mod_ver_0_subject, "");
		INIT_GAME_STR("GameWorld/JieYi/g_jie_yi_mod_ver_0_content", g_jie_yi_mod_ver_0_content, "");
		INIT_GAME_STR("GameWorld/JieYi/g_jie_yi_create_broadcast", g_jie_yi_create_broadcast, "s");
		INIT_GAME_STR("GameWorld/JieYi/g_jie_yi_tan_he_subject", g_jie_yi_tan_he_subject, "");
		INIT_GAME_STR("GameWorld/JieYi/g_jie_yi_tan_he_content", g_jie_yi_tan_he_content, "ds");
		INIT_GAME_STR("GameWorld/JieYi/g_jie_yi_break_tan_he_content", g_jie_yi_break_tan_he_content, "");


		INIT_GAME_STR("GameWorld/endless_tower_rank_reward_subject", g_endless_tower_rank_reward_subject, "");
		INIT_GAME_STR("GameWorld/endless_tower_rank_reward_content", g_endless_tower_rank_reward_content, "d");
		INIT_GAME_STR("GameWorld/role_skill_change_return_subject", g_role_skill_chagne_return_subject, "");
		INIT_GAME_STR("GameWorld/role_skill_change_return_content", g_role_skill_change_return_content, "");
		INIT_GAME_STR("GameWorld/role_skill_change_return_subject_2", g_role_skill_chagne_return_subject_2, "");
		INIT_GAME_STR("GameWorld/role_skill_change_return_content_2", g_role_skill_change_return_content_2, "");
		INIT_GAME_STR("GameWorld/partner_tree_return_subject", g_partner_tree_return_subject, "");
		INIT_GAME_STR("GameWorld/partner_tree_return_content", g_partner_tree_return_content, "");
		INIT_GAME_STR("GameWorld/god_equip_back_break_subject", g_god_equip_back_break_subject, "");
		INIT_GAME_STR("GameWorld/god_equip_back_break_content", g_god_equip_back_break_content, "");
		INIT_GAME_STR("GameWorld/RandActivity/randactivity_lucky_bag_subject", g_randactivity_lucky_bag_subject, "");
		INIT_GAME_STR("GameWorld/RandActivity/randactivity_lucky_bag_content", g_randactivity_lucky_bag_content, "");
		INIT_GAME_STR("GameWorld/CapacityTarget/capacity_target_reward_subject", g_capacity_target_reward_subject, "");
		INIT_GAME_STR("GameWorld/CapacityTarget/capacity_target_reward_content", g_capacity_target_reward_content, "d");
		INIT_GAME_STR("GameWorld/GiveGift/give_gift_back_item_subject", give_gift_back_item_subject, "");
		INIT_GAME_STR("GameWorld/GiveGift/give_gift_back_item_content", give_gift_back_item_content, "");
		INIT_GAME_STR("GameWorld/GiveGift/give_flower_notice", g_give_flower_notice, "sds"); 
		INIT_GAME_STR("GameWorld/GiveGift/give_flower_back_kiss", g_give_flower_back_kiss, "s");

		INIT_GAME_STR("GameWorld/ChiefElection/chief_election_winner_role_login", g_chief_election_winner_login_broadcast, "ds");
		INIT_GAME_STR("GameWorld/ActGuildHonor/guild_honor_winner_login", g_guild_honor_winner_login_broadcast, "dss");
	
		INIT_GAME_STR("GameWorld/RandActivity/ra_reward_borcast", g_ra_reward_borcast, "dsddd");
		INIT_GAME_STR("GameWorld/RandActivity/ra_reward_borcast2", g_ra_reward_borcast2, "dsddd");
		INIT_GAME_STR("GameWorld/RandActivity/ling_pai_tian_ci_borcast", g_ling_pai_tian_ci_borcast, "dsddd");
		INIT_GAME_STR("GameWorld/RandActivity/xin_fa_wu_liang_borcast", g_xin_fa_wu_liang_borcast, "dsddd");
		INIT_GAME_STR("GameWorld/RandActivity/fu_shi_hun_qi_borcast", g_fu_shi_hun_qi_borcast, "dsddd");
		INIT_GAME_STR("GameWorld/RandActivity/xin_fa_wu_liang_borcast", g_xin_fa_wu_liang_borcast, "dsdd");
		INIT_GAME_STR("GameWorld/RandActivity/yun_ze_er_san_shi_borcast", g_yun_ze_er_san_shi_borcast,"dsddd");
		INIT_GAME_STR("GameWorld/RandActivity/yun_ze_er_san_shi_marquee_borcast", g_yun_ze_er_san_shi_marquee_borcast, "dsdd");
		INIT_GAME_STR("GameWorld/HasOrder", g_has_order, "d");
		INIT_GAME_STR("GameWorld/role_convert_buy_front_item_not_buy", g_role_convert_buy_front_item_not_buy_notice, "s");
		INIT_GAME_STR("GameWorld/top_level_subject", g_top_level_subject, "");
		INIT_GAME_STR("GameWorld/top_level_content", g_top_level_content, "");
		INIT_GAME_STR("GameWorld/MiYi/mi_yi_first_kill_subject", g_mi_yi_first_kill_subject, "");
		INIT_GAME_STR("GameWorld/MiYi/mi_yi_first_kill_content", g_mi_yi_first_kill_content, "d");
		INIT_GAME_STR("GameWorld/ActBraveGround/SaoDangSubject", g_brave_ground_sao_dang_subject, "");
		INIT_GAME_STR("GameWorld/ActBraveGround/SaoDangContent", g_brave_ground_sao_dang_content, "");
		INIT_GAME_STR("GameWorld/ActBraveGround/ResetSubject", g_brave_ground_reset_subject, "");
		INIT_GAME_STR("GameWorld/ActBraveGround/ResetContent", g_brave_ground_reset_content, "");
		INIT_GAME_STR("GameWorld/ActBraveGround/brave_ground_reward_reissue_ver1_subject", g_brave_ground_reward_reissue_ver1_subject, "");
		INIT_GAME_STR("GameWorld/ActBraveGround/brave_ground_reward_reissue_ver1_content", g_brave_ground_reward_reissue_ver1_content, "d");

		INIT_GAME_STR("GameWorld/RandActivity/randactivity_lao_dong_hao_li_end", g_lao_dong_hao_li_end_broadcast, "");
		INIT_GAME_STR("GameWorld/ClearMixedElementCrystalSubject", g_clear_mixed_element_crystal_subject, "");
		INIT_GAME_STR("GameWorld/ClearMixedElementCrystalContent", g_clear_mixed_element_crystal_content, "");
		INIT_GAME_STR("GameWorld/RandActivity/randactivity_lao_dong_hao_li", g_lao_dong_hao_li_broadcast, "");
		INIT_GAME_STR("GameWorld/RandActivity/chun_ri_deng_feng_broadcast", g_chun_ri_deng_feng_broadcast, "dsd");
		INIT_GAME_STR("GameWorld/pet/g_pet_add_exp_err_without_break_level", g_pet_add_exp_err_without_break_level, "s");
		INIT_GAME_STR("GameWorld/norex_item/norex_item_use_item_invalid", g_norex_item_use_item_invalid, "d");
		INIT_GAME_STR("GameWorld/norex_item/norex_item_use_item_num_limit", g_norex_item_use_item_num_limit, "d");

		INIT_GAME_STR("GameWorld/life_skill/gather_notice", g_life_skill_gather_notice, "d");
		INIT_GAME_STR("GameWorld/life_skill/gather_double_notice", g_life_skill_gather_double_notice, "dd");
		INIT_GAME_STR("GameWorld/life_skill/fish_notice", g_life_skill_fish_notice, "d");
		INIT_GAME_STR("GameWorld/life_skill/fish_double_notice", g_life_skill_fish_double_notice, "d");
		INIT_GAME_STR("GameWorld/life_skill/consume_gather_power", g_life_skill_consume_gather_power, "d");
		INIT_GAME_STR("GameWorld/life_skill/life_skill_level_up", g_life_skill_level_up_notice, "ddd");
		INIT_GAME_STR("GameWorld/life_skill/change_surface_noitce", g_life_skill_change_surface_noitce, "d");
		INIT_GAME_STR("GameWorld/life_skill/surface_point_not_enough", g_life_skill_surface_point_not_enough, "d");

		INIT_GAME_STR("GameWorld/kua_fu_ying_xiong_lu/rank_reward_subject", g_kua_fu_ying_xiong_lu_rank_subject, "");
		INIT_GAME_STR("GameWorld/kua_fu_ying_xiong_lu/rank_reward_content", g_kua_fu_ying_xiong_lu_rank_content, "dd");
		INIT_GAME_STR("GameWorld/kua_fu_ying_xiong_lu/bag_full_reward_subject", g_kua_fu_ying_xiong_lu_bag_full_subject, "");
		INIT_GAME_STR("GameWorld/kua_fu_ying_xiong_lu/bag_full_reward_content", g_kua_fu_ying_xiong_lu_bag_full_content, "");
		INIT_GAME_STR("GameWorld/kua_fu_ying_xiong_lu/rand_end_task_reward_subject", g_kua_fu_ying_xiong_lu_rand_end_task_reward_subject, "");
		INIT_GAME_STR("GameWorld/kua_fu_ying_xiong_lu/rand_end_task_reward_content", g_kua_fu_ying_xiong_lu_rand_end_task_reward_content, "d");

		INIT_GAME_STR("GameWorld/crystal_fb/crystal_fb_use_energy_item", g_crystal_fb_use_energy_item, "d");
		INIT_GAME_STR("GameWorld/RandActivity/randactivity_fortune_maze_subject", g_randactivity_fortune_maze_subject, "");
		INIT_GAME_STR("GameWorld/RandActivity/randactivity_fortune_maze_content", g_randactivity_fortune_maze_content, "sddd");
		INIT_GAME_STR("GameWorld/RandActivity/randactivity_reward_limit_subject", g_randactivity_reward_subject, "");
		INIT_GAME_STR("GameWorld/RandActivity/randactivity_reward_limit_subject_1", g_randactivity_reward_subject_1, "d");
		INIT_GAME_STR("GameWorld/RandActivity/randactivity_reward_limit_content", g_randactivity_reward_content, "d");
		INIT_GAME_STR("GameWorld/RandActivity/randactivity_reward_end_content", g_randactivity_reward_end_content, "d");
		INIT_GAME_STR("GameWorld/RandActivity/randactivity_reward_buy_limit_content", g_randactivity_reward_buy_limit_content, "d");
		INIT_GAME_STR("GameWorld/RandActivity/randactivity_bag_full_subject", g_randactivity_bag_full_subject, "d");
		INIT_GAME_STR("GameWorld/RandActivity/randactivity_bag_full_content", g_randactivity_bag_full_content, "d");


		INIT_GAME_STR("GameWorld/RandActivity/ra_server_competition_end_rank_content", g_ra_server_competition_end_rank_content, "dd");
		INIT_GAME_STR("GameWorld/RandActivity/ra_server_competition_end_broadcast", g_ra_server_competition_end_broadcast, "sdd");

		INIT_GAME_STR("GameWorld/Medal/medal_up_grade", g_medal_up_grade, "sdddddd");
		INIT_GAME_STR("GameWorld/Medal/medal_lottery", g_medal_lottery_broadcast, "dsdd");
		INIT_GAME_STR("Global/Guild/GuildUpLevel", g_guild_up_level, "sd");
		INIT_GAME_STR("Global/Guild/GuildUpLevel", g_guild_up_level, "sd");
		INIT_GAME_STR("GameWorld/Collection/return", g_collection_return, "");
		INIT_GAME_STR("Global/Guild/addgongxian", g_add_guild_gongxian, "d");
		INIT_GAME_STR("Global/Guild/guildbuilddonate", g_guild_build_donate, "");
		INIT_GAME_STR("Global/Guild/GuildDismissSubject", g_guild_dismiss_subject, "");
		INIT_GAME_STR("Global/Guild/GuildDismissContent", g_guild_dismiss_content, "s");
		INIT_GAME_STR("Global/Guild/GuildDemotionSubject", g_guild_demotion_subject, "");
		INIT_GAME_STR("Global/Guild/GuildDemotionContent", g_guild_demotion_content, "sd");

		INIT_GAME_STR("Global/Guild/Guild_Building_Finish_Subject", g_guild_building_finish_subject, "");
		INIT_GAME_STR("Global/Guild/Guild_Building_Finish_Leader_Content", g_guild_building_finish_leader_content, "sd");
		INIT_GAME_STR("Global/Guild/Guild_Building_Finish_DeputyLeader_Content", g_guild_building_finish_deputyleader_content, "sd");

		INIT_GAME_STR("Global/Guild/Guild_Exp_Remind_Subject", g_guild_exp_remind_subject, "");
		INIT_GAME_STR("Global/Guild/Guild_Exp_Remind_Will_Overflow_Content", g_guild_exp_remind_will_overflow_content, "s");
		INIT_GAME_STR("Global/Guild/Guild_Exp_Remind_Overflow_Content", g_guild_exp_remind_overflow_content, "s");

		INIT_GAME_STR("Global/Guild/WeekTaskSettleSubject", g_guild_week_task_settlement_reward_title, "");
		INIT_GAME_STR("Global/Guild/WeekTaskSettleReward", g_guild_week_task_issue_settlement_reward, "d");
		INIT_GAME_STR("Global/Guild/WeekTaskContriSubject", g_guild_week_task_contribute_reward_title, "");
		INIT_GAME_STR("Global/Guild/WeekTaskContriReward", g_guild_week_task_issue_contribute_reward, "dd");
		INIT_GAME_STR("Global/Guild/WeekTaskSettleContriSubject", g_guild_week_task_settle_contribute_subject, "");
		INIT_GAME_STR("Global/Guild/WeekTaskSettleContriReward", g_guild_week_task_settle_contribute_content, "dd");
		INIT_GAME_STR("Global/Guild/guild_boss_chapter_reward_subject", g_guild_boss_chapter_reward_subject, "");
		INIT_GAME_STR("Global/Guild/guild_boss_chapter_baodi_reward", g_guild_boss_chapter_baodi_reward, "s");
		INIT_GAME_STR("Global/Guild/guild_boss_chapter_kill_reward_subject", g_guild_boss_chapter_kill_reward_subject, "");
		INIT_GAME_STR("Global/Guild/guild_boss_chapter_kill_reward_content", g_guild_boss_chapter_kill_reward_content, "s");
		INIT_GAME_STR("Global/Guild/guild_boss_chapter_notice_subject", g_guild_boss_chapter_notice_subject, "");
		INIT_GAME_STR("Global/Guild/guild_boss_chapter_notice_content", g_guild_boss_chapter_notice_content, "s");
		INIT_GAME_STR("Global/Guild/guild_boss_chapter_rank_subject", g_guild_boss_chapter_rank_subject, "");
		INIT_GAME_STR("Global/Guild/guild_boss_chapter_rank_content", g_guild_boss_chapter_rank_content, "d");
		INIT_GAME_STR("Global/Guild/guild_gather_dream_level_not_enough", g_guild_gather_dream_level_not_enough, "d");
		INIT_GAME_STR("Global/Guild/guild_gather_dream_publish", g_guild_gather_dream_publish, "d");
		INIT_GAME_STR("Global/Guild/guild_gather_dream_daychange_reward_subject", g_guild_gather_dream_daychange_reward_subject, "");
		INIT_GAME_STR("Global/Guild/guild_gather_dream_daychange_reward_content", g_guild_gather_dream_daychange_reward_content, "");
		INIT_GAME_STR("Global/Guild/guild_gather_dream_leave_guild_reward_content", g_guild_gather_Dream_leave_guild_reward_content, "");
		INIT_GAME_STR("Global/Guild/guild_gather_dream_reissue_reward_content", g_guild_gather_dream_reissue_reward_content, "");
		INIT_GAME_STR("Global/Guild/guild_boss_chapter_notice", g_guild_boss_chapter_notice, "s");
		INIT_GAME_STR("Global/team/recruit", g_team_recruit, "dddddd");
		INIT_GAME_STR("Global/team/create", g_team_create, "dddddd");
		INIT_GAME_STR("Global/team/jointeam", g_team_add_member, "s");
		INIT_GAME_STR("Global/team/leaveteam", g_team_member_leave, "s");
		INIT_GAME_STR("Global/team/becomeleader", g_team_become_leader, "s");
		INIT_GAME_STR("Global/team/bekicked", g_team_be_kicked, "");
		INIT_GAME_STR("Global/team/refusesummon", g_team_member_refuse_summon, "s");
		INIT_GAME_STR("Global/team/level_not_enough", g_team_member_level_not_enough, "sd");

		INIT_GAME_STR("GameWorld/Partner/TalentGift", g_partner_talent_gift, "s");

		INIT_GAME_STR("GameWorld/WorldBoss/result_mail_subject", g_world_boss_subject, "");
		INIT_GAME_STR("GameWorld/WorldBoss/result_mail_context", g_world_boss_reward, "d");
		INIT_GAME_STR("GameWorld/WorldBoss/hurt_subject", g_world_boss_hurt_subject, "");
		INIT_GAME_STR("GameWorld/WorldBoss/hurt_reward", g_world_boss_hurt_reward, "d");
		INIT_GAME_STR("GameWorld/WorldBoss/heal_subject", g_world_boss_heal_subject, "");
		INIT_GAME_STR("GameWorld/WorldBoss/heal_reward", g_world_boss_heal_reward, "d");
		INIT_GAME_STR("GameWorld/WorldBoss/luck_subject", g_world_boss_luck_subject, "");
		INIT_GAME_STR("GameWorld/WorldBoss/luck_reward", g_world_boss_luck_reward, "d");
		INIT_GAME_STR("GameWorld/WorldBoss/kill_subject", g_world_boss_kill_subject, "");
		INIT_GAME_STR("GameWorld/WorldBoss/kill_reward", g_world_boss_kill_reward, "");
		INIT_GAME_STR("GameWorld/WorldBoss/ready_broadcast", g_world_boss_broadcast, "ddd");
		INIT_GAME_STR("GameWorld/WorldBoss/kill_broadcast", g_world_boss_kill_broadcast, "d");
		INIT_GAME_STR("GameWorld/WorldBoss/not_kill_broadcast", g_world_boss_not_kill_broadcast, "d");
		INIT_GAME_STR("GameWorld/WorldBoss/appear_broadcast", g_world_boss_appear_broadcast, "dd");

		INIT_GAME_STR("GameWorld/WorldBoss2/result_mail_subject", g_world_boss_2_subject, "");
		INIT_GAME_STR("GameWorld/WorldBoss2/result_mail_context", g_world_boss_2_reward, "d");
		INIT_GAME_STR("GameWorld/WorldBoss2/hurt_subject", g_world_boss_2_hurt_subject, "");
		INIT_GAME_STR("GameWorld/WorldBoss2/hurt_reward", g_world_boss_2_hurt_reward, "d");
		INIT_GAME_STR("GameWorld/WorldBoss2/heal_subject", g_world_boss_2_heal_subject, "");
		INIT_GAME_STR("GameWorld/WorldBoss2/heal_reward", g_world_boss_2_heal_reward, "d");
		INIT_GAME_STR("GameWorld/WorldBoss2/luck_subject", g_world_boss_2_luck_subject, "");
		INIT_GAME_STR("GameWorld/WorldBoss2/luck_reward", g_world_boss_2_luck_reward, "d");
		INIT_GAME_STR("GameWorld/WorldBoss2/kill_subject", g_world_boss_2_kill_subject, "");
		INIT_GAME_STR("GameWorld/WorldBoss2/kill_reward", g_world_boss_2_kill_reward, "");
		INIT_GAME_STR("GameWorld/WorldBoss2/ready_broadcast", g_world_boss_2_broadcast, "ddd");
		INIT_GAME_STR("GameWorld/WorldBoss2/kill_broadcast", g_world_boss_2_kill_broadcast, "d");
		INIT_GAME_STR("GameWorld/WorldBoss2/not_kill_broadcast", g_world_boss_2_not_kill_broadcast, "d");
		INIT_GAME_STR("GameWorld/WorldBoss2/appear_broadcast", g_world_boss_2_appear_broadcast, "dd");

		INIT_GAME_STR("GameWorld/WorldBoss3/result_mail_subject", g_world_boss_3_subject, "d");
		INIT_GAME_STR("GameWorld/WorldBoss3/result_mail_context", g_world_boss_3_reward, "d");
		INIT_GAME_STR("GameWorld/WorldBoss3/hurt_subject", g_world_boss_3_hurt_subject, "d");
		INIT_GAME_STR("GameWorld/WorldBoss3/hurt_reward", g_world_boss_3_hurt_reward, "dd");
		INIT_GAME_STR("GameWorld/WorldBoss3/heal_subject", g_world_boss_3_heal_subject, "d");
		INIT_GAME_STR("GameWorld/WorldBoss3/heal_reward", g_world_boss_3_heal_reward, "dd");
		INIT_GAME_STR("GameWorld/WorldBoss3/luck_subject", g_world_boss_3_luck_subject, "d");
		INIT_GAME_STR("GameWorld/WorldBoss3/luck_reward", g_world_boss_3_luck_reward, "dd");
		INIT_GAME_STR("GameWorld/WorldBoss3/kill_subject", g_world_boss_3_kill_subject, "d");
		INIT_GAME_STR("GameWorld/WorldBoss3/kill_reward", g_world_boss_3_kill_reward, "d");
		INIT_GAME_STR("GameWorld/WorldBoss3/ready_broadcast", g_world_boss_3_broadcast, "ddd");
		INIT_GAME_STR("GameWorld/WorldBoss3/kill_broadcast", g_world_boss_3_kill_broadcast, "d");
		INIT_GAME_STR("GameWorld/WorldBoss3/not_kill_broadcast", g_world_boss_3_not_kill_broadcast, "d");
		INIT_GAME_STR("GameWorld/WorldBoss3/appear_broadcast", g_world_boss_3_appear_broadcast, "dd");

		INIT_GAME_STR("GameWorld/shitu/shitu_subject", g_shi_tu_subject, "");
		INIT_GAME_STR("GameWorld/shitu/shitu_reward", g_shi_tu_reward, "dsd");

		INIT_GAME_STR("GameWorld/shitu/shitu_mile_subject", g_shitu_mile_subject, "");
		INIT_GAME_STR("GameWorld/shitu/shitu_mile_master_reward", g_shitu_mile_master_reward, "s");
		INIT_GAME_STR("GameWorld/shitu/shitu_mile_apprentice_reward", g_shitu_mile_apprentice_reward, "s");
		INIT_GAME_STR("GameWorld/shitu/shitu_break_time_notice", g_shitu_break_time_notice, "dsdsu");
		INIT_GAME_STR("GameWorld/shitu/shitu_break_notice", g_shitu_break_notice, "dsds");

		INIT_GAME_STR("Global/TradeMarket/KnapsackFullSendMailSubject", g_knapsack_full_mail_subject, "");
		INIT_GAME_STR("Global/TradeMarket/KnapsackFullSendMail", g_knapsack_full_send_mail, "");
		INIT_GAME_STR("Global/TradeMarket/PetKnapsackFullSendMailSubject", g_pet_full_mail_subject, "");
		INIT_GAME_STR("Global/TradeMarket/PetKnapsackFullSendMail", g_pet_full_send_mail, "");
		INIT_GAME_STR("Global/TradeMarket/Advertise", g_trade_market_advertise, "");
		INIT_GAME_STR("Global/TradeMarket/SoldItemGetMoney", g_trade_market_sold_item_get_money, "dd");
		INIT_GAME_STR("Global/TradeMarket/FocusSuccess", g_trade_market_focus_success, "");
		INIT_GAME_STR("Global/TradeMarket/AnnounceNotice", g_trademarket_announce_notice, "");
		INIT_GAME_STR("Global/TradeMarket/OnSaleNotice", g_trademarket_onsale_notice, "");

		INIT_GAME_STR("GameWorld/ActTeamFightRewardSubject", g_team_fight_reward_subject, "");
		INIT_GAME_STR("GameWorld/ActTeamFightBoxRewardSubject", g_team_fight_box_reward_subject, "");
		INIT_GAME_STR("GameWorld/ActTeamFightRankReward", g_team_fight_rank_reward, "d");
		INIT_GAME_STR("GameWorld/ActTeamFightBoxReward", g_team_fight_box_reward, "s");
		INIT_GAME_STR("GameWorld/ActTeamFightReissueBoxRewardSubject", g_team_fight_reissue_box_reward_subject, "");
		INIT_GAME_STR("GameWorld/ActTeamFightReissueFirstBoxRewardCon", g_team_fight_reissue_first_box_reward_content, "");
		INIT_GAME_STR("GameWorld/ActTeamFightReissueThreeBoxRewardCon", g_team_fight_reissue_three_box_reward_content, "");
		INIT_GAME_STR("GameWorld/ActTeamFightJoinRewardSubject", g_team_fight_join_reward_subject, "");
		INIT_GAME_STR("GameWorld/ActTeamFightJoinRewardContent", g_team_fight_join_reward_content, "");


		INIT_GAME_STR("GameWorld/RoleResetName/role_rename_notice", g_role_rename_notice, "sdsd");
		INIT_GAME_STR("GameWorld/RoleResetName/role_rename_subject", g_role_rename_subject, "");
		INIT_GAME_STR("GameWorld/RoleResetName/role_rename_content", g_role_rename_content, "ss");
		INIT_GAME_STR("GameWorld/RoleResetName/role_advance_content", g_role_advance_content, "ss");

		INIT_GAME_STR("GameWorld/ActMazeRacer/AmuletDrop", g_maze_racer_amulet_drop, "d");
		INIT_GAME_STR("GameWorld/ActMazeRacer/RankListSubject", g_maze_racer_reward_subj, "");
		INIT_GAME_STR("GameWorld/ActMazeRacer/RankListReward", g_maze_racer_rank_reward, "d");

		INIT_GAME_STR("GameWorld/ActBraveGround/RewardSubject", g_brave_ground_reward_subject, "");
		INIT_GAME_STR("GameWorld/ActBraveGround/RankReward", g_brave_ground_reward, "d");

		INIT_GAME_STR("GameWorld/ActGuildHonor/RewardSubject", g_guild_honor_reward_subject, "");
		INIT_GAME_STR("GameWorld/ActGuildHonor/Reward", g_guild_honor_reward_content, "d");
		INIT_GAME_STR("GameWorld/ActGuildHonor/ByeSubject", g_guild_honor_bye_reward_subject, "");
		INIT_GAME_STR("GameWorld/ActGuildHonor/ByeReward", g_guild_honor_bye_reward_content, "");
		INIT_GAME_STR("GameWorld/ActGuildHonor/BattleBroadcast", g_guild_honor_battle_broadcast, "");
		INIT_GAME_STR("GameWorld/ActGuildHonor/DirectWin", g_guild_honor_battle_direct_win_broadcast, "");
		INIT_GAME_STR("GameWorld/ActGuildHonor/settlement_list_notice", g_guild_honor_settlement_list_notice, "");
		INIT_GAME_STR("GameWorld/ActGuildHonor/start_round_notice", g_guild_honor_battle_start_round_notice, "dd");
		INIT_GAME_STR("GameWorld/ActGuildHonor/guess_subject", g_guild_honor_battle_guess_subject, "");
		INIT_GAME_STR("GameWorld/ActGuildHonor/guess_content", g_guild_honor_battle_guess_content, "ddsds");
		INIT_GAME_STR("GameWorld/ActGuildHonor/guess_champion_subject", g_guild_honor_battle_guess_champion_subject, "");
		INIT_GAME_STR("GameWorld/ActGuildHonor/guess_champion_content", g_guild_honor_battle_guess_champion_content, "ds");
		INIT_GAME_STR("GameWorld/ActGuildHonor/rank_reward_subject", g_guild_honor_battle_rank_reward_subject, ""); 
		INIT_GAME_STR("GameWorld/ActGuildHonor/rank_reward_content", g_guild_honor_battle_rank_reward_content, "d");
		INIT_GAME_STR("GameWorld/ActGuildHonor/rank_unjoin_reward_content", g_guild_honor_battle_rank_unjoin_reward_content, "d");
		INIT_GAME_STR("GameWorld/ActGuildHonor/last_round_battle", g_guild_honor_battle_last_round_battle_notice, "dd");

		INIT_GAME_STR("GameWorld/ActGuildFight/FamilyRankRewardSubject", g_guild_fight_rank_reward_title, "");
		INIT_GAME_STR("GameWorld/ActGuildFight/FamilyRankReward", g_guild_fight_familys_rank_reward, "d");
		INIT_GAME_STR("GameWorld/ActGuildFight/SurvivalSubject", g_guild_fight_survival_subject, "");
		INIT_GAME_STR("GameWorld/ActGuildFight/SurvivalReward", g_guild_fight_survival_reward, "d");
		INIT_GAME_STR("GameWorld/ActGuildFight/ParticipationSubject", g_guild_fight_participation_subject, "");
		INIT_GAME_STR("GameWorld/ActGuildFight/ParticipationAward", g_guild_fight_participation_award, "");

		INIT_GAME_STR("GameWorld/ActOnlyFight/RoleRankRewardSubject", g_only_fight_rank_reward_subject, "");
		INIT_GAME_STR("GameWorld/ActOnlyFight/RoleRankRewardContent", g_only_fight_rank_reward_content, "d");
		INIT_GAME_STR("GameWorld/ActOnlyFight/RoleSurvivalSubject", g_only_fight_survival_subject, "");
		INIT_GAME_STR("GameWorld/ActOnlyFight/RoleSurvivalReward", g_only_fight_survival_reward, "d");
		INIT_GAME_STR("GameWorld/ActOnlyFight/RoleParticipationSubject", g_only_fight_participation_subject, "");
		INIT_GAME_STR("GameWorld/ActOnlyFight/RoleParticipationAward", g_only_fight_participation_award, "");
		INIT_GAME_STR("GameWorld/ActOnlyFight/OnlyFightNotice", g_only_fight_notice, "");

		INIT_GAME_STR("GameWorld/ActSilkRoads/RoleRankRewardSubject", g_silk_roads_rank_reward_subject, "");
		INIT_GAME_STR("GameWorld/ActSilkRoads/RoleRankRewardContent", g_silk_roads_rank_reward_content, "dfdd");
		INIT_GAME_STR("GameWorld/ActSilkRoads/RoleSettlementRewardSubject", g_silk_roads_settlement_reward_subject, "");
		INIT_GAME_STR("GameWorld/ActSilkRoads/RoleSettlementRewardContent", g_silk_roads_settlement_reward_content, "dfdd");

		INIT_GAME_STR("Global/Mail/Money/chongzhi_subject", g_money_chongzhi_subject, "");
		INIT_GAME_STR("Global/Mail/Money/chongzhi_content", g_money_chongzhi_content, "d");

		INIT_GAME_STR("GameWorld/Colosseum/ColosseumSubj", g_colosseum_subject, "");
		INIT_GAME_STR("GameWorld/Colosseum/ColosseumCoin", g_colosseum_coin_reward, "");
		INIT_GAME_STR("GameWorld/Colosseum/chenge_times_remain_subject", g_colosseum_challenge_times_remain_reward_subject, "");
		INIT_GAME_STR("GameWorld/Colosseum/chenge_times_remain_content", g_colosseum_challenge_times_remain_reward_content, "d");
		INIT_GAME_STR("GameWorld/Colosseum/rank_reward_subject", g_colosseum_rank_reward_subject, "");
		INIT_GAME_STR("GameWorld/Colosseum/rankr_eward_content", g_colosseum_rank_reward_content, "d");
		INIT_GAME_STR("GameWorld/Colosseum/colosseum_battle_reward_subj", g_colosseum_battle_reawrd_subject, "");
		INIT_GAME_STR("GameWorld/Colosseum/colosseum_battle_reward_cont", g_colosseum_battle_reawrd_content, "");

		INIT_GAME_STR("GameWorld/courage_challenge/courage_challenge_content", g_courage_challenge_reward_content, "");
		INIT_GAME_STR("GameWorld/new_courage_challenge/new_courage_challenge_content", g_new_courage_challenge_reward_content, "");
		INIT_GAME_STR("GameWorld/dream_notes_challenge/dream_notes_challenge_subeject", g_dream_notes_challenge_subject, "");
		INIT_GAME_STR("GameWorld/dream_notes_challenge/dream_notes_challenge_content", g_dream_notes_challenge_content, "");
		INIT_GAME_STR("GameWorld/dream_notes_challenge/dream_notes_challenge_reissue", g_dream_notes_challenge_reissue, "");
		INIT_GAME_STR("GameWorld/dream_notes_challenge/use_item_add_times_notice", g_dream_notes_challenge_use_item_add_times_notice, "d");
		INIT_GAME_STR("GameWorld/dream_notes_challenge/role_level_not_enough_not_challenge", g_role_level_not_enough_not_challenge, "d");

		INIT_GAME_STR("Global/GmOrder/GetRoleUID", g_get_role_uid_float, "d");

		INIT_GAME_STR("Global/NormalNotice/RefuseQieCuo", g_refuse_qie_cuo_float, "s");
		INIT_GAME_STR("Global/NormalNotice/scene_unlock", g_scene_unlock_notice, "d");
		INIT_GAME_STR("Global/activity/activitystate", g_activity_state, "dd");
		INIT_GAME_STR("Global/activity/activity_sign_up_subject", g_activity_sign_up_subject, "");
		INIT_GAME_STR("Global/activity/activity_sign_up_content", g_activity_sign_up_content, "d");

		INIT_GAME_STR("Global/Mail/Item/invalid_subject", g_item_expire_notice_subject, "");
		INIT_GAME_STR("Global/Mail/Item/invalid_content", g_item_expire_notice_content, "ddd");
		INIT_GAME_STR("Global/Mail/Item/invalid_subject_2", g_item_expire_notice_subject_2, "");
		INIT_GAME_STR("Global/Mail/Item/invalid_content_2", g_item_expire_notice_content_2, "s");
		INIT_GAME_STR("Global/Mail/Item/invalid_subject_3", g_item_expire_notice_subject_3, "");
		INIT_GAME_STR("Global/Mail/Item/invalid_content_3", g_item_expire_notice_content_3, "sdd");
		INIT_GAME_STR("Global/Mail/Item/temp_clear_subject", g_temp_clear_notice_subject, "");
		INIT_GAME_STR("Global/Mail/Item/temp_clear_content", g_temp_clear_notice_content, "");
		INIT_GAME_STR("Global/Mail/Item/item_not_enough_notie", g_item_not_enough_notie_content, "d");
		INIT_GAME_STR("Global/Mail/Item/item_not_enough_notie2", g_item_not_enough_notie_content2, "d");
		INIT_GAME_STR("Global/Mail/Item/team_member_item_not_enough_notice", g_team_member_item_not_enough_notice, "sd");
		INIT_GAME_STR("Global/Mail/Item/system_item_return_subject", g_system_item_return_subject, "");
		INIT_GAME_STR("Global/Mail/Item/partner_item_system_return_content", g_partner_item_system_return_content, "");

		INIT_GAME_STR("GameWorld/Salary/reward_subject", g_salary_reward_subject, "");
		INIT_GAME_STR("GameWorld/Salary/reward_content", g_salary_reward_content, "");
		INIT_GAME_STR("GameWorld/Salary/captain_salary", g_salary_captain_salary, "d");
		INIT_GAME_STR("GameWorld/Salary/chivalrous_salary", g_salary_chivalrous_salary, "d");

		INIT_GAME_STR("GameWorld/SmartMountsSystem/role_level_not_enough", g_smart_mounts_system_level_not_engough, "d");
		INIT_GAME_STR("GameWorld/SmartMountsSystem/skill_up_level_limit", g_smart_mounts_system_skill_up_level_limit, "d");
		INIT_GAME_STR("GameWorld/SmartMountsSystem/breach_faild_notice", g_smart_mounts_system_breach_faild_notice, "dd");

		INIT_GAME_STR("GameWorld/RoleGuildInborn/role_learn_skill_level_not_enough", g_role_guild_inborn_learn_skill_level_not_enough, "d");
		INIT_GAME_STR("GameWorld/RoleGuildInborn/learn_skill_skill_points_not_enough", g_role_guild_inborn_skill_points_not_enough, "d");
		INIT_GAME_STR("GameWorld/RoleGuildInborn/role_reset_skill_subject", g_role_guild_inborn_reset_skill_subject, "");
		INIT_GAME_STR("GameWorld/RoleGuildInborn/role_reset_skill_content", g_role_guild_inborn_reset_skill_content, "");

		INIT_GAME_STR("GameWorld/RoleHuanShou/open_egg_rare_broadcast_content", g_role_huan_shou_open_egg_rare_broadcast_content, "dsddd");

		INIT_GAME_STR("GameWorld/RealmBreak/offline_info_pool_notice", g_realm_break_offline_info_pool_notice, "d");

		INIT_GAME_STR("Global/chat/chat_open_level", g_chat_open_level, "dd");

		INIT_GAME_STR("GameWorld/PlatformBattle/reward_subject", g_platform_battle_reward_subject, "");
		INIT_GAME_STR("GameWorld/PlatformBattle/reward_content", g_platform_battle_reward_content, "s");
		INIT_GAME_STR("GameWorld/PlatformBattle/reward_content2", g_platform_battle_reward_content2, "dd");

		INIT_GAME_STR("GameWorld/ChiefElection/nobody_join", g_chief_election_no_winner_broadcast, "d");
		INIT_GAME_STR("GameWorld/ChiefElection/champion", g_chief_election_champion_broadcast, "ddsd");
		INIT_GAME_STR("GameWorld/ChiefElection/reward_subject", g_chief_election_reward_subject, "");
		INIT_GAME_STR("GameWorld/ChiefElection/reward_content", g_chief_election_reward_content, "d");
		INIT_GAME_STR("GameWorld/ChiefElection/battle_reward_subject", g_chief_election_battle_reward_subject, "");
		INIT_GAME_STR("GameWorld/ChiefElection/battle_reward_content", g_chief_election_battle_reward_content, "d");
		INIT_GAME_STR("GameWorld/ChiefElection/sea_reward_subject", g_chief_election_sea_reward_subject, "");
		INIT_GAME_STR("GameWorld/ChiefElection/sea_reward_content", g_chief_election_sea_reward_content, "");
		INIT_GAME_STR("GameWorld/ChiefElection/tip1", g_chief_election_sea_tip1, "");
		INIT_GAME_STR("GameWorld/ChiefElection/tip2", g_chief_election_sea_tip2, "");
		INIT_GAME_STR("GameWorld/ChiefElection/tip3", g_chief_election_sea_tip3, "");
		INIT_GAME_STR("GameWorld/ChiefElection/tip4", g_chief_election_sea_tip4, "");

		INIT_GAME_STR("GameWorld/ChiefPeakBattle/nobody_join", g_chief_peak_battle_nobody_join_broadcast, "");
		INIT_GAME_STR("GameWorld/ChiefPeakBattle/champion", g_chief_peak_battle_champion_broadcast, "ds");
		INIT_GAME_STR("GameWorld/ChiefPeakBattle/RewardSubject", g_activity_chief_peak_battle_rank_reward_subject, "");
		INIT_GAME_STR("GameWorld/ChiefPeakBattle/RankReward", g_activity_chief_peak_battle_rank_reward_content, "d");
		INIT_GAME_STR("GameWorld/ChiefPeakBattle/BetRewardSubject", g_activity_chief_peak_battle_bet_reward_subject, "");
		INIT_GAME_STR("GameWorld/ChiefPeakBattle/BetReward", g_activity_chief_peak_battle_bet_reward_content, "");
		INIT_GAME_STR("GameWorld/ChiefPeakBattle/battle_reward_subject", g_activity_chief_peak_battle_battle_reward_subject, "");
		INIT_GAME_STR("GameWorld/ChiefPeakBattle/battle_reward_content", g_activity_chief_peak_battle_battle_reward_content, "");

		INIT_GAME_STR("Global/Mail/Knapsack/put_subject", g_knapsack_put_subject, "");
		INIT_GAME_STR("Global/Mail/Knapsack/put_content", g_knapsack_put_content, "");
		INIT_GAME_STR("Global/Mail/Knapsack/put_prestige", g_put_prestige_notie, "dd");
		INIT_GAME_STR("Global/Mail/Knapsack/prestige_uplevel", g_prestige_uplevel_notie, "dd");
		INIT_GAME_STR("Global/Mail/Knapsack/transfer_subject", g_transfer_subject, "");
		INIT_GAME_STR("Global/Mail/Knapsack/transfer_content", g_transfer_content, "");

		INIT_GAME_STR("GameWorld/CourseOfGrowth/uplevel_subject", g_course_of_growth_uplevel_subject, "");
		INIT_GAME_STR("GameWorld/CourseOfGrowth/uplevel_content", g_course_of_growth_uplevel_content, "dd");
		INIT_GAME_STR("GameWorld/Chongzhi/chongzhi_reward_subject", g_chongzhi_reward_subject, "");
		INIT_GAME_STR("GameWorld/Chongzhi/chongzhi_reward_content", g_chongzhi_reward_content, "dd");
		INIT_GAME_STR("GameWorld/Chongzhi/chongzhi_reward_coin_content", g_chongzhi_reward_coin_content, "dd");
		INIT_GAME_STR("GameWorld/Chongzhi/month_investment_subject", g_month_investment_subject, "");
		INIT_GAME_STR("GameWorld/Chongzhi/month_investment_content", g_month_investment_content, "");
		INIT_GAME_STR("GameWorld/Chongzhi/first_recharge_subject", g_first_recharge_subject, "");
		INIT_GAME_STR("GameWorld/Chongzhi/first_recharge_content", g_first_recharge_content, "d");
		INIT_GAME_STR("GameWorld/Chongzhi/first_recharge_exclusive", g_first_recharge_exclusive, "");

		INIT_GAME_STR("GameWorld/MonthCard/month_week_card_reward_subject", g_month_week_card_reward_subject, "");
		INIT_GAME_STR("GameWorld/MonthCard/buy_month_week_card_reward_content", g_buy_month_week_card_reward_content, "d");
		INIT_GAME_STR("GameWorld/MonthCard/buy_month_week_card_item_content", g_buy_moth_week_card_item_content, "");

		INIT_GAME_STR("Global/Mail/Mail/collection", g_mail_collection_content, "d");
		INIT_GAME_STR("Global/Mail/Mail/collection_main", g_mail_collection_main_content, "");
		INIT_GAME_STR("Global/Mail/Mail/changeprofession_subject", g_mail_change_prof_subject, "");
		INIT_GAME_STR("Global/Mail/Mail/changeprofession_content", g_mail_change_prof_content, "s");

		INIT_GAME_STR("Global/Mail/Mail/bountytask_turntable", g_mail_bountytask_turntable, "");

		INIT_GAME_STR("Global/Guild/joinguild", g_join_guild, "s");
		INIT_GAME_STR("Global/Guild/agree_invite_but_full", g_agree_invite_but_full, "");
		INIT_GAME_STR("Global/Guild/refuse_invite", g_refuse_invite, "");
		INIT_GAME_STR("Global/Guild/refuse_invite_notice", g_refuse_invite_notice, "s");
		INIT_GAME_STR("Global/Guild/chairman_to_newbie_message", g_chairman_to_newbie_message, "s");

		INIT_GAME_STR("GameWorld/FunOpen/task_limit", g_funopen_task_limit, "dd");
		INIT_GAME_STR("Global/TradeMarket/trademark_buy_succ", g_trademark_buy_succ_content, "ddd");
		INIT_GAME_STR("Global/TradeMarket/SoldItemSendGlobal", g_sold_succ_send_global_message, "");

		INIT_GAME_STR("GameWorld/Task/Task", g_task_adv_succ_content, "d");
		INIT_GAME_STR("GameWorld/Task/Task_Reissue_Item_Subject", g_task_reissue_item_subject, "");
		INIT_GAME_STR("GameWorld/Task/Task_Reissue_Item_Content", g_task_reissue_item_content, "");
		INIT_GAME_STR("GameWorld/Buff_Effect/Buff_Effect_mp", g_buff_effect_mp_message, "s");
		INIT_GAME_STR("GameWorld/Buff_Effect/Buff_Effect_hp", g_buff_effect_hp_message, "s");
		INIT_GAME_STR("GameWorld/Buff_Effect/Buff_Effect_hpmp", g_buff_effect_hpmp_message, "s");


		INIT_GAME_STR("Global/Mail/Knapsack/on_consume_item", g_knapsack_on_consume_message, "dd");
		INIT_GAME_STR("Global/Mail/Knapsack/on_consume_item2", g_knapsack_on_consume_message2, "dd");
		INIT_GAME_STR("Global/Mail/Money/on_consume_money", g_money_on_consume_money_message, "dd");

		INIT_GAME_STR("GameWorld/Buff_Effect/Buff_Effect_attr", g_buff_effect_attr_message, "d");
		INIT_GAME_STR("GameWorld/Buff_Effect/Buff_bad_Effect_attr", g_buff_bad_effect_attr_message, "d");
		INIT_GAME_STR("Global/Battle/BattleServerDisconnect", g_battle_server_disconnect, "");
		INIT_GAME_STR("Global/Battle/BigCrossBattleServerDisconnect", g_bigcross_battle_server_disconnect, "");
		INIT_GAME_STR("GameWorld/Fabao/level_limit", g_fabao_level_limit_message, "d");
		INIT_GAME_STR("GameWorld/Fabao/uplevel_limit", g_fabao_uplevel_limit_message, "");
		INIT_GAME_STR("GameWorld/Fabao/back_subject", g_fabao_back_subject, "");
		INIT_GAME_STR("GameWorld/Fabao/back_content", g_fabao_back_content, "");
		INIT_GAME_STR("GameWorld/Fabao/back_subject2", g_fabao_back_subject2, "");
		INIT_GAME_STR("GameWorld/Fabao/back_content2", g_fabao_back_content2, "");

		INIT_GAME_STR("Global/Mail/Mail/patrol_task", g_patrol_task_message, "");
		INIT_GAME_STR("Global/Mail/Mail/patrol_subject", g_patrol_task_subject, "");

		INIT_GAME_STR("GameWorld/Retrieve/RetrieveSubject", g_retrieve_mail_subject, "");
		INIT_GAME_STR("GameWorld/Retrieve/RefundMoney", g_retrieve_mail_content_refund_money, "sss");
		INIT_GAME_STR("GameWorld/Retrieve/InDebtMoney", g_retrieve_mail_content_indebt_money, "sss");
		INIT_GAME_STR("GameWorld/Retrieve/RetrieveDebt", g_retrieve_mail_content_retrieve_debt, "sss");
		INIT_GAME_STR("GameWorld/Retrieve/BugMoney", g_retrieve_mail_content_bug_money, "sss");
		INIT_GAME_STR("GameWorld/Retrieve/RefundItem", g_retrieve_mail_content_refund_item, "dd");
		INIT_GAME_STR("GameWorld/Retrieve/BugItem", g_retrieve_mail_content_bug_item, "dd");
		INIT_GAME_STR("GameWorld/Retrieve/RetrieveExp", g_retrieve_mail_content_exp, "s");
		INIT_GAME_STR("GameWorld/Retrieve/RetrievePet", g_retrieve_pet_notice, "ds");
		INIT_GAME_STR("GameWorld/Retrieve/ReturnPetEquipment", g_retrieve_pet_return_equip, "");

		INIT_GAME_STR("GameWorld/Retrieve/RefundOtherMoney", g_retrieve_mail_content_refund_other_money, "sd");
		INIT_GAME_STR("GameWorld/Retrieve/InDebtOtherMoney", g_retrieve_mail_content_indebt_other_money, "sd");
		INIT_GAME_STR("GameWorld/Retrieve/RetrieveDebtOtherMoney", g_retrieve_mail_content_retrieve_debt_other_money, "sd");
		INIT_GAME_STR("GameWorld/Retrieve/BugOtherMoney", g_retrieve_mail_content_bug_other_money, "sd");

		INIT_GAME_STR("Global/Server_First_Kill/first_kill_subject", g_server_first_kill_subject, "");
		INIT_GAME_STR("Global/Server_First_Kill/first_kill_content", g_server_first_kill_content, "s");
		INIT_GAME_STR("Global/Server_First_Kill/first_kill_miyi", g_server_first_kill_miyi, "ssd");
		INIT_GAME_STR("Global/Server_First_Kill/first_kill_boss", g_server_first_kill_boss, "ssd");
		INIT_GAME_STR("Global/Server_First_Kill/first_kill_challenge", g_server_first_kill_challenge, "ssd");
		INIT_GAME_STR("Global/Server_First_Kill/first_kill_shan_hai", g_server_first_kill_shan_hai, "ssd");
		INIT_GAME_STR("Global/Server_First_Kill/first_kill_big_dipper", g_server_first_kill_big_dipper, "ssd");
		INIT_GAME_STR("Global/Server_First_Kill/first_kill_fallen_god", g_server_first_kill_fallen_god, "ssd");
		INIT_GAME_STR("Global/Server_First_Kill/first_kill_hmtj_fb", g_server_first_kill_hmtj_fb, "ssd");

		INIT_GAME_STR("Global/WildBoss/refresh_wild_boss_notice", g_wild_boss_refresh_notice, "dd");

		INIT_GAME_STR("Global/TreasureItem/TreasureItemNotice1", g_treasure_item_notice1, "sdd");
		INIT_GAME_STR("Global/TreasureItem/TreasureItemNotice2", g_treasure_item_notice2, "sddd");

		INIT_GAME_STR("GameWorld/AdvanceFB/Notic", g_advancefb_task_notic, "d");

		INIT_GAME_STR("GameWorld/BountyTask/TurnTableRewardSubject", g_bounty_task_turn_table_reward_subject, "");
		INIT_GAME_STR("GameWorld/BountyTask/TurnTableRewardContent", g_bounty_task_turn_table_reward_content, "");
		INIT_GAME_STR("GameWorld/BountyTask/TurnTableRewardNotice", g_bounty_task_turn_table_reward_notice, "sd");

		INIT_GAME_STR("GameWorld/TaskChain/RewardSubject", g_task_chain_turn_table_reward_subject, "");
		INIT_GAME_STR("GameWorld/TaskChain/RewardContent", g_task_chain_turn_table_reward_content, "d");

		INIT_GAME_STR("GameWorld/WorldQuestion/Subject", g_world_question_subject, "");
		INIT_GAME_STR("GameWorld/WorldQuestion/Content", g_world_question_content, "");

		INIT_GAME_STR("GameWorld/RandActivity/SevenColorPicture/seven_color_picture_subject", g_seven_color_picture_subject, "d");
		INIT_GAME_STR("GameWorld/RandActivity/SevenColorPicture/seven_color_picture_content", g_seven_color_picture_content, "dd");
		INIT_GAME_STR("GameWorld/RandActivity/SevenColorPicture/seven_color_picture_total_content", g_seven_color_picture_total_content, "d");
		INIT_GAME_STR("GameWorld/RandActivity/SevenColorPicture/seven_color_picture_hearsay_content", g_seven_color_picture_hearsay_content, "sddd");

		INIT_GAME_STR("GameWorld/RandActivity/AnimalRace/animal_race_subject", g_animal_race_subject, "");
		INIT_GAME_STR("GameWorld/RandActivity/AnimalRace/animal_race_content", g_animal_race_content, "ds");

		INIT_GAME_STR("GameWorld/RandActivity/HappyUp/happy_up_hearsy_content", g_happy_up_hearsay_content, "sddd");

		INIT_GAME_STR("GameWorld/XunBao/broadcast", g_xun_bao_broadcast, "dsd");

		INIT_GAME_STR("GameWorld/PetCompose/pet_compose", g_pet_compose, "sdd");

		INIT_GAME_STR("GameWorld/face_scroe/active_succ", g_face_score_active_succ, "d");
		INIT_GAME_STR("GameWorld/face_scroe/res_is_inactive", g_face_score_is_inactive, "d");

		INIT_GAME_STR("GameWorld/system_post/partner_wake_post", g_system_post_partner, "sddddd");
		INIT_GAME_STR("GameWorld/system_post/equipment_max_post", g_system_post_equipment_max, "sddd");
		INIT_GAME_STR("GameWorld/system_post/pet_max_post", g_system_post_pet_max, "sdddd");

		INIT_GAME_STR("GameWorld/workshop/strone_level_limit", g_workshop_strone_level_notice, "");
		INIT_GAME_STR("GameWorld/workshop/strone_puton_level", g_workshop_strone_puton_level_notice, "");
		INIT_GAME_STR("GameWorld/workshop/synthesis_info_1", g_synthesis_info_1, "s");
		INIT_GAME_STR("GameWorld/workshop/synthesis_info_2", g_synthesis_info_2, "");
		INIT_GAME_STR("GameWorld/workshop/synthesis_info_3", g_synthesis_info_3, "");
		INIT_GAME_STR("GameWorld/workshop/synthesis_info_4", g_synthesis_info_4, "");
		INIT_GAME_STR("GameWorld/workshop/synthesis_info_5", g_synthesis_info_5, "dd");
		INIT_GAME_STR("GameWorld/workshop/synthesis_info_6", g_synthesis_info_6, "dd");
		INIT_GAME_STR("GameWorld/workshop/compound_pet_equipment_broadcast", g_compound_pet_equipment_broadcast, "dsd");
		

		INIT_GAME_STR("GameWorld/RandActivity/TianJiangHaoLi/tian_jiang_hao_li_hearsay", g_tian_jiang_hao_li_hearsay, "sdd");

		INIT_GAME_STR("GameWorld/RandActivity/HeritageTreasure/broadcast", g_heritage_treasure_broadcast, "sddd");
		INIT_GAME_STR("GameWorld/RandActivity/expel_monster", g_randactivity_expelmonster_notice, "sdd");
		INIT_GAME_STR("GameWorld/RandActivity/FortuneMaze", g_randactivity_fortune_maze_notice, "dd");

		INIT_GAME_STR("GameWorld/RandActivity/partner_exchange", g_randactivity_partner_exchange_notice, "sdd");
		INIT_GAME_STR("GameWorld/RandActivity/CollectWord/broadcast", g_randactivity_collect_word_broadcast, "sdd");
		INIT_GAME_STR("GameWorld/RandActivity/pet_exchange", g_randactivity_pet_exchange_notice, "sddd");

		INIT_GAME_STR("GameWorld/RandActivity/AdventureGift/broadcast", g_randactivity_adventure_gift_broadcast, "dsd");

		INIT_GAME_STR("GameWorld/RandActivity/make_up_reward_subject", g_randactivity_make_up_reward_subject, "d");
		INIT_GAME_STR("GameWorld/RandActivity/make_up_reward_content", g_randactivity_make_up_reward_content, "");

		INIT_GAME_STR("GameWorld/RandActivity/lucky_turntable_content", g_randactivity_lucky_turntable_content, "sdd");
		INIT_GAME_STR("GameWorld/RandActivity/tianming_chou_content", g_rand_activity_tianming_chou_content, "sddd");
		INIT_GAME_STR("GameWorld/RandActivity/tian_ming_divination_reward_content", g_rand_activity_tian_ming_divination_reward_content, "sddd");


		INIT_GAME_STR("GameWorld/RandActivity/QuanMinDiscount/quan_min_discount_subject", g_randactivity_quan_min_discount_subject, "");
		INIT_GAME_STR("GameWorld/RandActivity/QuanMinDiscount/quan_min_discount_message", g_randactivity_quan_min_discount_message, "dsd");

		INIT_GAME_STR("GameWorld/RandActivity/good_gift_turn_subject", g_randactivity_good_gift_turn_subject, "");
		INIT_GAME_STR("GameWorld/RandActivity/good_gift_turn_content", g_randactivity_good_gift_turn_content, "");

		INIT_GAME_STR("GameWorld/RandActivity/help_others_subject", g_randactivity_help_others_subject, "");
		INIT_GAME_STR("GameWorld/RandActivity/help_others_content", g_randactivity_help_others_content, "");

		INIT_GAME_STR("GameWorld/RandActivity/help_others_2_subject", g_randactivity_help_others_2_subject, "");
		INIT_GAME_STR("GameWorld/RandActivity/help_others_2_content", g_randactivity_help_others_2_content, "");

		INIT_GAME_STR("GameWorld/RandActivity/shan_hai_question_subject", g_randactivity_shan_hai_question_subject, "");
		INIT_GAME_STR("GameWorld/RandActivity/shan_hai_question_content", g_randactivity_shan_hai_question_sontent, "");

		INIT_GAME_STR("GameWorld/RandActivity/secret_treausre_subject", g_randactivity_secret_treasure_subject, "");
		INIT_GAME_STR("GameWorld/RandActivity/secret_treasure_content", g_randactivity_secret_treasure_content, "");

		INIT_GAME_STR("GameWorld/RandActivity/yun_ze_torritory_subject", g_randactivity_yun_ze_territory_subject,"d");
		INIT_GAME_STR("GameWorld/RandActivity/yun_ze_torritory_content", g_randactivity_yun_ze_territory_content,"d");

		INIT_GAME_STR("GameWorld/RandActivity/yun_ze_slot_machine_subject", g_randactivity_yun_ze_slot_machine_subject, "d");
		INIT_GAME_STR("GameWorld/RandActivity/yun_ze_slot_machine_content", g_randactivity_yun_ze_slot_machine_content, "d");

		INIT_GAME_STR("GameWorld/RandActivity/yun_ze_er_san_shi_subject", g_randactivity_yun_ze_er_san_shi_subject, "d");
		INIT_GAME_STR("GameWorld/RandActivity/yun_ze_er_san_shi_content", g_randactivity_yun_ze_er_san_shi_content, "d");

		INIT_GAME_STR("GameWorld/RandActivity/yun_ze_blooming_record_subject", g_randactivity_yun_ze_blooming_record_subject, "d");
		INIT_GAME_STR("GameWorld/RandActivity/yun_ze_blooming_record_content", g_randactivity_yun_ze_blooming_record_content, "d");

		INIT_GAME_STR("GameWorld/RandActivity/tiger_auspicious_snow_subject", g_randactivity_tiger_auspicious_snow_subject, "d");
		INIT_GAME_STR("GameWorld/RandActivity/tiger_auspicious_snow_content", g_randactivity_tiger_auspicious_snow_content, "d");

		INIT_GAME_STR("GameWorld/RandActivity/tiger_find_spring_yesterday_content", g_randactivity_tiger_find_spring_yesterday_content, "d");

		INIT_GAME_STR("Global/chat/group_subject", g_chat_group_subject, "");
		INIT_GAME_STR("Global/chat/group_dismiss", g_chat_group_dismiss_content, "s");
		INIT_GAME_STR("Global/chat/tick_out_group", g_chat_group_tick_out_content, "s");
		INIT_GAME_STR("Global/chat/single_chat_is_limit", g_single_chat_is_limit_notice, "d");
		INIT_GAME_STR("Global/chat/ChatGroupInviteFail", g_chat_group_invite_fail_content, "s");
		INIT_GAME_STR("Global/chat/ChatGroupInviteSucc", g_chat_group_invite_succ_content, "s");
		INIT_GAME_STR("Global/chat/add_friend_succ", g_add_friend_succ_content, "s");

		INIT_GAME_STR("GameWorld/Colosseum/ra_colosseum_subject", g_ra_sports_subject, "");
		INIT_GAME_STR("GameWorld/Colosseum/ra_colosseum_histroy_content", g_ra_sports_histroy_content, "d");
		INIT_GAME_STR("GameWorld/Colosseum/ra_colosseum_settlement_content", g_ra_sports_settlement_content, "dd");

		INIT_GAME_STR("GameWorld/ActHuanJieZhanChang/reward_subject", g_huan_jie_zhan_chang_rank_reward_subject, "");
		INIT_GAME_STR("GameWorld/ActHuanJieZhanChang/reward_content", g_huan_jie_zhan_chang_rank_reward_content, "d");
		INIT_GAME_STR("GameWorld/ActHuanJieZhanChang/settlement_reward_subject", g_huan_jie_zhan_chang_settlement_reward_subject, "");
		INIT_GAME_STR("GameWorld/ActHuanJieZhanChang/settlement_reward_content", g_huan_jie_zhan_chang_settlement_reward_content, "dd");
		INIT_GAME_STR("GameWorld/ActHuanJieZhanChang/ranking_list_reward_subject", g_huan_jie_zhan_chang_ranking_list_reward_subject, "");
		INIT_GAME_STR("GameWorld/ActHuanJieZhanChang/ranking_list_reward_content", g_huan_jie_zhan_chang_ranking_list_reward_content, "dd");
		INIT_GAME_STR("GameWorld/ActHuanJieZhanChang/today_win_reward_subject", g_huan_jie_zhan_chang_today_win_reward_subject, "");
		INIT_GAME_STR("GameWorld/ActHuanJieZhanChang/today_win_reward_content", g_huan_jie_zhan_chang_today_win_reward_content, "");

		INIT_GAME_STR("GameWorld/Partner/crytals_remove", g_partner_crytals_remove, "");
		INIT_GAME_STR("GameWorld/Partner/crytals_usetime_done", g_partner_crytals_done, "dd");
		INIT_GAME_STR("GameWorld/Partner/quality_pur_to_ora", g_partner_quality_pur_to_ora, "d");
		INIT_GAME_STR("GameWorld/Partner/quality_ora_to_red", g_partner_quality_ora_to_red, "d");
		INIT_GAME_STR("GameWorld/Partner/quality_upgrade", g_partner_quality_upgrade, "d");
		INIT_GAME_STR("GameWorld/Partner/partner_quality_revert_subject", partner_quality_revert_subject, "");
		INIT_GAME_STR("GameWorld/Partner/partner_quality_revert_content", partner_quality_revert_content, "");
		INIT_GAME_STR("GameWorld/Partner/partner_skin_notice", partner_skin_notice, "");
		INIT_GAME_STR("GameWorld/Partner/partner_skin_notice_2", partner_skin_notice_2, "");

		INIT_GAME_STR("GameWorld/Enter_FB/EnterMiYi", g_enter_miyi, "d");
		INIT_GAME_STR("GameWorld/Enter_FB/EnterMiYi_2", g_enter_miyi_2, "sd");
		INIT_GAME_STR("Global/Mail/Item/miyi_poker_title", g_miyi_poker_title, "");
		INIT_GAME_STR("Global/Mail/Item/miyi_poker_reward_content", g_miyi_poker_reward_content, "");

		INIT_GAME_STR("GameWorld/PrestigeNotice/over", prestige_over, "d");
		INIT_GAME_STR("GameWorld/PrestigeNotice/use_limit", g_prestige_use_limit, "dd");
		INIT_GAME_STR("GameWorld/person_rank/person_rank_prof_capability_top_change_broadcast", person_rank_prof_capability_top_change_broadcast, "dsdsdd");

		INIT_GAME_STR("Global/team/math_team_fail", g_math_team_fail_content, "dsdsdsds");
		INIT_GAME_STR("Global/team/crytals_reward", g_crytals_reward_content, "sd");
		INIT_GAME_STR("Global/team/crytals_reward2", g_crytals_reward2_content, "d");

		INIT_GAME_STR("Global/Mail/Item/wildboss_poker_title", g_wildboss_poker_title, "");
		INIT_GAME_STR("Global/Mail/Item/wildboss_poker_reward_content", g_wildboss_poker_reward_content, "");
		INIT_GAME_STR("Global/Mail/Item/wildboss_poker_reward_notice", g_wildboss_poker_reward_notice, "sd");

		INIT_GAME_STR("Global/Mail/Item/shanhaiboss_poker_title", g_shanhaiboss_poker_title, "");
		INIT_GAME_STR("Global/Mail/Item/shanhaiboss_poker_reward_content", g_shanhaiboss_poker_reward_content, "");

		INIT_GAME_STR("Global/Mail/Item/crystal_fb_poker_reward_subject", g_crystal_fb_poker_reward_subject, "");
		INIT_GAME_STR("Global/Mail/Item/crystal_fb_poker_reward_content", g_crystal_fb_poker_reward_content, "");
		INIT_GAME_STR("Global/Mail/Item/crystal_fb_poker_reward_notice", g_crystal_fb_poker_reward_notice, "");

		INIT_GAME_STR("Global/Mail/Item/wan_ling_fb_reward_notice", g_wan_ling_fb_reward_notice, "sd");


		INIT_GAME_STR("Global/Mail/Item/god_token_surplus_title", g_god_token_surplus_title, "");
		INIT_GAME_STR("Global/Mail/Item/god_token_surplus_reward_content", g_god_token_surplus_reward_content, "");

		INIT_GAME_STR("Global/Mail/Item/god_token_level_reward_title", g_god_token_level_reward_title, "u");
		INIT_GAME_STR("Global/Mail/Item/god_token_level_reward_content", g_god_token_level_reward_content, "u");

		INIT_GAME_STR("Global/Mail/Item/chong_zhi_buy_item_reward_title", g_chong_zhi_buy_item_reward_title, "");
		INIT_GAME_STR("Global/Mail/Item/chong_zhi_buy_item_reward_content", g_chong_zhi_buy_item_reward_content, "d");

		INIT_GAME_STR("Global/Mail/Item/act_full_bag_title", g_act_full_bag_title, "d");
		INIT_GAME_STR("Global/Mail/Item/act_full_bag_content", g_act_full_bag_content, "d");

		INIT_GAME_STR("GameWorld/Pet/pet_strengthen_broadcast", g_pet_strengthen_broadcast, "dsddd");
		INIT_GAME_STR("GameWorld/Pet/pet_refine_bian_yi_broadcast", pet_refine_bian_yi_broadcast, "dsdd");
		INIT_GAME_STR("GameWorld/Pet/pet_unlock_talent_group_str_level_not_enough", g_pet_unlock_talent_group_str_level_not_enough, "d");
		INIT_GAME_STR("GameWorld/Pet/pet_talent_reset_return_consume_item_subject", g_pet_talent_reset_return_consume_item_subject, "");
		INIT_GAME_STR("GameWorld/Pet/pet_talent_reset_return_consume_item_content", g_pet_talent_reset_return_consume_item_content, "d");

		INIT_GAME_STR("Global/Mail/Item/has_poker_get", g_has_poker_get, "");

		INIT_GAME_STR("GameWorld/AnecdoteTask/RewardByMailSubject", g_anecdote_task_reward_by_mail_subject, "");
		INIT_GAME_STR("GameWorld/AnecdoteTask/RewardByMailContent", g_anecdote_task_reward_by_mail_content, "d");
		INIT_GAME_STR("GameWorld/AnecdoteTask/RewardByMailKnapsackFull", g_anecdote_task_reward_by_mail_knapsack_full, "d");
		INIT_GAME_STR("GameWorld/AnecdoteTask/BigEventSubject", g_anecdote_big_event_reward_by_mail_subject, "");
		INIT_GAME_STR("GameWorld/AnecdoteTask/BigEventContent", g_anecdote_big_event_reward_by_mail_content, "");

		INIT_GAME_STR("GameWorld/DrawMananger/draw_reward_subject", g_draw_reward_subject, "");
		INIT_GAME_STR("GameWorld/DrawMananger/draw_reward_content", g_draw_reward_content, "");

		INIT_GAME_STR("Global/TradeMarket/GiveItemToUserOfflineSubject", g_trademarket_give_item_to_user_offline_subject, "");
		INIT_GAME_STR("Global/TradeMarket/GiveItemToUserOfflineContent", g_trademarket_give_item_to_user_offline_content, "");

		INIT_GAME_STR("Global/TradeMarket/OrderReturnResourceSubject", g_order_return_resource_subject, "");
		INIT_GAME_STR("Global/TradeMarket/OrderUpshelveReturn", g_order_upshelve_return_resource, "");
		INIT_GAME_STR("Global/TradeMarket/OrderBuyReturn", g_order_buy_return_resource, "");
		INIT_GAME_STR("Global/TradeMarket/OrderAdvertiseReturn", g_order_advertise_return_resource, "");
		INIT_GAME_STR("Global/TradeMarket/NewVersionAutoOffshelveSubject", g_new_version_auto_offshelve_subject, "");
		INIT_GAME_STR("Global/TradeMarket/NewVersionAutoOffshelveContent", g_new_version_auto_offshelve_content, "");

		INIT_GAME_STR("Global/Guild/GuildTanHeNoticeTitle", guild_tan_he_notice_title, "");
		INIT_GAME_STR("Global/Guild/GuildTanHeNoticeContent", guild_tan_he_notice_content, "");
		INIT_GAME_STR("Global/Guild/GuildTanHeSuccTitle", guild_tan_he_succ_title, "");
		INIT_GAME_STR("Global/Guild/GuildTanHeSuccContent", guild_tan_he_succ_content, "ds");

		INIT_GAME_STR("GameWorld/RandActivity/ShenShouJiangLin/shen_shou_jiang_lin_broadcast_1", g_shen_shou_jiang_lin_broadcast_1, "dsd");
		INIT_GAME_STR("GameWorld/RandActivity/ShenShouJiangLin/shen_shou_jiang_lin_broadcast_2", g_shen_shou_jiang_lin_broadcast_2, "dsd");
		INIT_GAME_STR("GameWorld/RandActivity/ShenShouJiangLin/shen_shou_jiang_lin_content_1", g_shen_shou_jiang_lin_content_1, "ds");
		INIT_GAME_STR("GameWorld/RandActivity/ShenShouJiangLin/shen_shou_jiang_lin_content_2", g_shen_shou_jiang_lin_content_2, "d");

		INIT_GAME_STR("GameWorld/ShanHai/shanhaiboss_broadcast", g_shanhaiboss_broadcast, "s");
		INIT_GAME_STR("GameWorld/ShanHai/shanhaiboss_reward_notice", g_shanhaiboss_reward_notice, "sd");

		INIT_GAME_STR("Global/chat/temp_chat_group_name", g_temp_chat_group_name, "");
		INIT_GAME_STR("Global/chat/chat_group_notice_join_temp", g_chat_group_msg_notice_join_temp, "s");
		INIT_GAME_STR("Global/chat/chat_group_notice_create_temp", g_chat_group_msg_notice_create_temp, "");
		INIT_GAME_STR("Global/chat/chat_group_notice_become_official", g_chat_group_msg_notice_become_official, "ss");
		INIT_GAME_STR("Global/chat/teammate_chat_group_full", g_teammate_chat_group_full, "s");
		INIT_GAME_STR("Global/chat/create_group_level_limit_notice", g_create_chat_group_level_limit_notice, "d");
		INIT_GAME_STR("Global/chat/create_group_cd_limit_notice", g_create_chat_group_cd_limit_notice, "u");
		INIT_GAME_STR("Global/chat/welcome_join_temp_group", g_welcome_join_temp_group_notice, "s");

		INIT_GAME_STR("GameWorld/Pet/AutoAssignAttributePointsNotice", g_auto_assign_attribute_points_notice, "s");

		INIT_GAME_STR("Global/activity/activity_trade_return_gold_subject", g_activity_trade_return_gold_subject, "");
		INIT_GAME_STR("Global/activity/activity_trade_return_gold_content", g_activity_trade_return_gold_content, "ddd");

		INIT_GAME_STR("Global/Mail/Item/god_equip_create", g_god_equip_create_notice, "sddd");
		INIT_GAME_STR("GameWorld/RandActivity/randactivity_cumulative_recharge_notice", g_cumulative_recharge_notice, "sddd");
		INIT_GAME_STR("Global/Mail/Item/gift_randitem_is_broadcast", g_gift_randitem_is_broadcast_notice, "sdd");

		INIT_GAME_STR("Global/chat/chat_group_handover_leadership_succ", g_chat_group_handover_leadership_succ, "s");
		INIT_GAME_STR("Global/chat/chat_group_leader_handovered_to_you", g_chat_group_leader_handovered_to_you, "ss");
		INIT_GAME_STR("Global/chat/chat_group_leadership_handovered", g_chat_group_leadership_handovered, "ss");

		INIT_GAME_STR("GameWorld/lun_hui_wang_chuan_subject", g_lun_hui_wang_chuan_subject, "");
		INIT_GAME_STR("GameWorld/lun_hui_wang_chuan_content", g_lun_hui_wang_chuan_content, "");
		
		INIT_GAME_STR("GameWorld/buy_extra_times_notice", g_buy_extra_times_notice, "d");
		INIT_GAME_STR("GameWorld/crystal_fb/crystal_energy_add", g_energy_add_notice, "d");

		INIT_GAME_STR("Global/CloudArena/season_reward_subject", g_cloud_arena_season_reward_subject, "");
		INIT_GAME_STR("Global/CloudArena/season_reward_content", g_cloud_arena_season_reward_content, "ddd");

		INIT_GAME_STR("Global/world_team_arena/team_member_level_not_enough", g_world_team_arena_team_member_level_not_enough, "d");
		INIT_GAME_STR("Global/world_team_arena/team_member_cancel_sign_up", g_world_team_arena_team_member_cancel_sign_up, "s");
		INIT_GAME_STR("Global/world_team_arena/team_member_logout", g_world_team_arena_team_member_logout, "s");
		INIT_GAME_STR("Global/world_team_arena/team_member_leave_team", g_world_team_arena_team_member_leave_team, "s");
		INIT_GAME_STR("Global/world_team_arena/create_team_succ_notice", g_world_team_arena_create_team_succ_notice, "s");
		INIT_GAME_STR("Global/world_team_arena/create_team_succ_borcast", g_world_team_arena_create_team_succ_borcast, "ssssss");
		INIT_GAME_STR("Global/world_team_arena/user_particip_subject", g_world_team_arena_user_particip_subject, "");
		INIT_GAME_STR("Global/world_team_arena/user_particip_content", g_world_team_arena_user_particip_content, "");
		INIT_GAME_STR("Global/world_team_arena/team_rank_subjcet", g_world_team_arena_team_rank_subject, "");
		INIT_GAME_STR("Global/world_team_arena/team_rank_content", g_world_team_arena_team_rank_content, "sd");
		INIT_GAME_STR("Global/world_team_arena/top_team_subject", g_world_team_arena_top_team_subjcet, "");
		INIT_GAME_STR("Global/world_team_arena/top_team_content", g_world_team_arena_top_team_content, "sd");
		INIT_GAME_STR("Global/world_team_arena/team_standy_brocast", g_world_team_arena_team_standy_brocast, "");
		INIT_GAME_STR("Global/world_team_arena/team_can_enter_brocast", g_world_team_arena_team_can_enter_brocast, "");

		INIT_GAME_STR("GameWorld/relic_looting/relic_looting_rank_subject", g_relic_looting_rank_reward_subject, "");
		INIT_GAME_STR("GameWorld/relic_looting/relic_looting_rank_content", g_relic_looting_rank_reward_content, "d");

		INIT_GAME_STR("GameWorld/lun_hui_wang_chuan_reward_subject", g_lun_hui_wang_chuan_reward_subject, "");
		INIT_GAME_STR("GameWorld/lun_hui_wang_chuan_reward_content", g_lun_hui_wang_chuan_reward_content, "");

		INIT_GAME_STR("GameWorld/relic_looting_reward_subject", g_relic_looting_reward_subject, "");
		INIT_GAME_STR("GameWorld/relic_looting_reward_content", g_relic_looting_reward_content, "");
		
		INIT_GAME_STR("GameWorld/heart_skill_displace_failed_notice", g_heart_skill_displace_failed_notice, "d");
		INIT_GAME_STR("GameWorld/role_recommend_score_not_enough_notice", g_role_recommend_score_not_enough_notice, "d");

		INIT_GAME_STR("GameWorld/crystal_fb_xiyou_times_add_notice", g_crystal_fb_xiyou_times_add_notice, "d");

		INIT_GAME_STR("GameWorld/god_equip_return_subject", g_god_equip_return_subject, "");
		INIT_GAME_STR("GameWorld/god_equip_return_content", g_god_equip_return_content, "d");
		
		// 家族活跃
		INIT_GAME_STR("GameWorld/GuildActive/GuildActiveDayActive_subject", g_guild_active_day_active_subject, "");
		INIT_GAME_STR("GameWorld/GuildActive/GuildActiveDayActive_content", g_guild_active_day_active_content, "d");
		INIT_GAME_STR("GameWorld/GuildActive/GuildActiveWeekActiveRank_subject", g_guild_active_week_active_rank_subject, "");
		INIT_GAME_STR("GameWorld/GuildActive/GuildActiveWeekActiveRank_content", g_guild_active_week_active_rank_content, "d");
		INIT_GAME_STR("GameWorld/GuildActive/GuildActiveWeekActivePersonRank_subject", g_guild_active_week_active_person_rank_subject, "");
		INIT_GAME_STR("GameWorld/GuildActive/GuildActiveWeekActivePersonRank_content", g_guild_active_week_active_person_rank_content, "d");

		INIT_GAME_STR("GameWorld/RoleJoinActivityReward/RoleJoinActivityReward_subject", g_role_join_activity_reward_subject, "d");
		INIT_GAME_STR("GameWorld/RoleJoinActivityReward/RoleJoinActivityReward_content", g_role_join_activity_reward_content, "d");

		INIT_GAME_STR("GameWorld/RAPaiMaiHang/bid_subject", g_ra_pai_mai_hang_bid_subject, "");
		INIT_GAME_STR("GameWorld/RAPaiMaiHang/bid_succ_content", g_ra_pai_mai_hang_bid_succ_content, "d");
		INIT_GAME_STR("GameWorld/RAPaiMaiHang/bid_succ_content1", g_ra_pai_mai_hang_bid_succ_content1, "dd");
		INIT_GAME_STR("GameWorld/RAPaiMaiHang/bid_failed_content", g_ra_pai_mai_hang_bid_failed_content, "d");
		INIT_GAME_STR("GameWorld/RAPaiMaiHang/bid_failed_notice", g_ra_pai_mai_hang_bid_failed_notice, "ddu");
		INIT_GAME_STR("GameWorld/RAPaiMaiHang/bid_set_add_price_notice", g_ra_pai_mai_hang_bid_set_add_price_notice, "dd");

		INIT_GAME_STR("GameWorld/return_item_subject", g_return_item_subject, "");
		INIT_GAME_STR("GameWorld/return_item_content", g_return_item_content, "dd");
		INIT_GAME_STR("GameWorld/return_item_subject2", g_return_item_subject2, "");
		INIT_GAME_STR("GameWorld/return_item_content2", g_return_item_content2, "dd");
		INIT_GAME_STR("GameWorld/return_item_subject3", g_return_item_subject3, "");
		INIT_GAME_STR("GameWorld/return_item_content3", g_return_item_content3, "dd");
		INIT_GAME_STR("GameWorld/return_item_subject4", g_return_item_subject4, "");
		INIT_GAME_STR("GameWorld/return_item_content4", g_return_item_content4, "");
		INIT_GAME_STR("GameWorld/return_item_subject5", g_return_item_subject5, "");
		INIT_GAME_STR("GameWorld/return_item_content5", g_return_item_content5, "");
		INIT_GAME_STR("GameWorld/return_item_subject6", g_return_item_subject6, "");
		INIT_GAME_STR("GameWorld/return_item_content6", g_return_item_content6, "");
		INIT_GAME_STR("GameWorld/return_item_subject7", g_return_item_subject7, "");
		INIT_GAME_STR("GameWorld/return_item_content7", g_return_item_content7, "dd");
		INIT_GAME_STR("GameWorld/return_item_subject8", g_return_item_subject8, "");
		INIT_GAME_STR("GameWorld/return_item_content8", g_return_item_content8, "");
		INIT_GAME_STR("GameWorld/return_item_subject9", g_return_item_subject9, "");
		INIT_GAME_STR("GameWorld/return_item_content9", g_return_item_content9, "");
		INIT_GAME_STR("GameWorld/return_item_subject10", g_return_item_subject10, "");
		INIT_GAME_STR("GameWorld/return_item_content10", g_return_item_content10, "");
		INIT_GAME_STR("GameWorld/return_item_subject11", g_return_item_subject11, "");
		INIT_GAME_STR("GameWorld/return_item_content11", g_return_item_content11, "");
		INIT_GAME_STR("GameWorld/return_item_subject12", g_return_item_subject12, "");
		INIT_GAME_STR("GameWorld/return_item_content12", g_return_item_content12, "");
		INIT_GAME_STR("GameWorld/return_item_subject13", g_return_item_subject13, "");
		INIT_GAME_STR("GameWorld/return_item_content13", g_return_item_content13, "");
		INIT_GAME_STR("GameWorld/return_item_subject14", g_return_item_subject14, "");
		INIT_GAME_STR("GameWorld/return_item_content14", g_return_item_content14, "");
		INIT_GAME_STR("GameWorld/return_item_subject15", g_return_item_subject15, "");
		INIT_GAME_STR("GameWorld/return_item_content15", g_return_item_content15, "");

		INIT_GAME_STR("GameWorld/change_card_point_recovery_subject", g_change_card_point_recovery_subject, "");
		INIT_GAME_STR("GameWorld/change_card_point_recovery_content", g_change_card_point_recovery_content, "ddd");

		INIT_GAME_STR("GameWorld/nether_end_change_subject", g_nether_end_change_subject, "");
		INIT_GAME_STR("GameWorld/nether_end_change_content", g_nether_end_change_content, "");
		INIT_GAME_STR("GameWorld/nether_end_change_silver_content", g_nether_end_change_silver_content, "d");
		INIT_GAME_STR("GameWorld/nether_end_change_gold_content", g_nether_end_change_gold_content, "d");
		INIT_GAME_STR("GameWorld/nether_end_change_final_content", g_nether_end_change_final_content, "");

		INIT_GAME_STR("GameWorld/pet_god_print_wish", g_pet_god_print_wish_broadcast, "dsd");
		INIT_GAME_STR("GameWorld/pet_god_print_wish_2", g_pet_god_print_wish_broadcast_2, "dsd");


		INIT_GAME_STR("Global/Mail/Item/hundred_ghost_poker_title", g_hundred_ghost_poker_title, "");
		INIT_GAME_STR("Global/Mail/Item/hundred_ghost_poker_reward_content", g_hundred_ghost_poker_reward_content, "");
		INIT_GAME_STR("Global/Mail/Item/hundred_ghost_reward_notice", g_hundred_ghost_poker_reward_notice, "");
		INIT_GAME_STR("GameWorld/HundredGhost/hundred_ghost_broadcast", g_hundred_ghost_broadcast, "");
		INIT_GAME_STR("GameWorld/HundredGhost/hundred_ghost_big_boss_broadcast", g_hundred_ghost_big_boss_broadcast, "");

		INIT_GAME_STR("GameWorld/HongBao/HongBao_notify_content", g_hongbao_notify_content, "ddds");

		INIT_GAME_STR("GameWorld/RandActivity/randactivity_qiong_lou_yu_yan_subject", g_ra_qiong_lou_yu_yan_subject, "d");
		INIT_GAME_STR("GameWorld/RandActivity/randactivity_qiong_lou_yu_yan_content", g_ra_qiong_lou_yu_yan_content, "");

		INIT_GAME_STR("Global/Mail/Item/god_equip_subject", g_god_equip_return_subject, "");
		INIT_GAME_STR("Global/Mail/Item/god_equip_content", g_god_equip_return_content, "");

		INIT_GAME_STR("GameWorld/FangFraud/FangFraudSendMailSubject", fang_fraud_send_mail_subject, "");
		INIT_GAME_STR("GameWorld/FangFraud/FangFraudSendMail", fang_fraud_send_mail_content, "");

		INIT_GAME_STR("GameWorld/RandActivity/back_reward_send_mail_subject", g_ra_back_reward_send_mail_subject, "");
		INIT_GAME_STR("GameWorld/RandActivity/back_reward_send_mail_content", g_ra_back_reward_send_mail_content, "");

		INIT_GAME_STR("GameWorld/Duel/duel_begin_duel_battle", duel_begin_duel_battle, "sdsddd");
		INIT_GAME_STR("GameWorld/Duel/duel_begin_duel_battle_team", duel_begin_duel_battle_team, "sdsddd");
		INIT_GAME_STR("GameWorld/Duel/duel_begin_duel_two_battle", duel_begin_duel_two_battle, "ss");
		INIT_GAME_STR("GameWorld/Duel/duel_begin_duel_two_battle_team", duel_begin_duel_two_battle_team, "ss");
		INIT_GAME_STR("GameWorld/Duel/duel_red_name_login_cross", duel_red_name_login_cross, "sddd");
		INIT_GAME_STR("GameWorld/Duel/duel_lose_reduce_exp", duel_lose_reduce_exp, "sd");
		INIT_GAME_STR("GameWorld/Duel/duel_lose_reduce_exp_team", duel_lose_reduce_exp_team, "sd");
		INIT_GAME_STR("GameWorld/Duel/duel_lose_reduce_coin", duel_lose_reduce_coin, "sd");
		INIT_GAME_STR("GameWorld/Duel/duel_lose_reduce_coin_team", duel_lose_reduce_coin_team, "sd");
		INIT_GAME_STR("GameWorld/Duel/duel_lose_reduce_kill_value", duel_lose_reduce_kill_value, "sd");
		INIT_GAME_STR("GameWorld/Duel/duel_lose_reduce_kill_value_team", duel_lose_reduce_kill_value_team, "sd");
		INIT_GAME_STR("GameWorld/Duel/duel_type_one_attack_win", duel_type_one_attack_win, "sd");
		INIT_GAME_STR("GameWorld/Duel/duel_type_one_attack_win_team", duel_type_one_attack_win_team, "sd");
		INIT_GAME_STR("GameWorld/Duel/duel_type_one_defend_win", duel_type_one_defend_win, "s");
		INIT_GAME_STR("GameWorld/Duel/duel_type_one_defend_win_team", duel_type_one_defend_win_team, "s");
		INIT_GAME_STR("GameWorld/Duel/duel_type_one_defend_win_red", duel_type_one_defend_win_red, "s");
		INIT_GAME_STR("GameWorld/Duel/duel_type_one_defend_win_red_team", duel_type_one_defend_win_red_team, "s");
		INIT_GAME_STR("GameWorld/Duel/duel_type_two_attack_win", duel_type_two_attack_win, "s");
		INIT_GAME_STR("GameWorld/Duel/duel_type_two_attack_win_team", duel_type_two_attack_win_team, "s");
		INIT_GAME_STR("GameWorld/Duel/duel_type_two_defend_win", duel_type_two_defend_win, "");
		INIT_GAME_STR("GameWorld/Duel/duel_type_three_attack_win", duel_type_three_attack_win, "d");
		INIT_GAME_STR("GameWorld/Duel/duel_type_three_defend_win", duel_type_three_defend_win, "s");
		INIT_GAME_STR("GameWorld/Duel/duel_type_three_defend_win_team", duel_type_three_defend_win_team, "s");
		INIT_GAME_STR("GameWorld/Duel/duel_type_three_defend_lose", duel_type_three_defend_lose, "s");
		INIT_GAME_STR("GameWorld/Duel/duel_type_three_defend_lose_team", duel_type_three_defend_lose_team, "s");
		INIT_GAME_STR("GameWorld/Duel/duel_win_add_coin", duel_win_add_coin, "sd");
		INIT_GAME_STR("GameWorld/Duel/duel_win_add_coin_team", duel_win_add_coin_team, "sd");
		INIT_GAME_STR("GameWorld/Duel/duel_end_duel_battle", duel_end_duel_battle, "ssssd");
		INIT_GAME_STR("GameWorld/Duel/duel_begining_duel_two", duel_begining_duel_two, "ssddd");
		INIT_GAME_STR("GameWorld/Duel/duel_lose_duel_two", duel_lose_duel_two, "sss");

		INIT_GAME_STR("GameWorld/stone_back/stone_mail_back_subject", g_stone_mail_back_subject,"");
		INIT_GAME_STR("GameWorld/stone_back/stone_mail_back_content", g_stone_mail_back_content,"");

		INIT_GAME_STR("GameWorld/RandActivity/horcrux_tiancheng_broadcast_1", g_randactivity_horcrux_tiancheng_broadcast_1, "dsd");
		INIT_GAME_STR("GameWorld/RandActivity/horcrux_tiancheng_broadcast_2", g_randactivity_horcrux_tiancheng_broadcast_2, "dsd");

		INIT_GAME_STR("GameWorld/RandActivity/integral_draw_broadcast", g_randactivity_integral_draw_broadcast, "ds");
		INIT_GAME_STR("GameWorld/god_equip_break_through_broadcast", g_god_equip_break_through_broadcast, "sdddd");
		
		INIT_GAME_STR("GameWorld/RandActivity/ra_shan_hai_meet_subject", g_ra_shan_hai_meet_subject, "");
		INIT_GAME_STR("GameWorld/RandActivity/ra_shan_hai_meet_content", g_ra_shan_hai_meet_content, "");

		INIT_GAME_STR("GameWorld/RandActivity/ra_fate_entanglement_subject", g_randactivity_fate_entanglement_subject, "");
		INIT_GAME_STR("GameWorld/RandActivity/ra_fate_entanglement_content", g_randactivity_fate_entanglement_content, "");

		INIT_GAME_STR("GameWorld/RandActivity/yunze_catch_elves_subject", g_randactivity_yunze_catch_elves_subject, "d");
		INIT_GAME_STR("GameWorld/RandActivity/yunze_catch_elves_content", g_randactivity_yunze_catch_elves_content, "d");

		INIT_GAME_STR("GameWorld/RandActivity/yunze_hao_li_subject", g_randactivity_yunze_hao_li_subject, "d");
		INIT_GAME_STR("GameWorld/RandActivity/yunze_hao_li_content", g_randactivity_yunze_hao_li_content, "d");

		INIT_GAME_STR("GameWorld/RandActivity/ra_zhaocaijinbao_broadcast", g_randactivity_zhaocaijinbao_broadcast, "dsd");

		INIT_GAME_STR("GameWorld/CowReportGoodNews/subject", g_cow_report_good_news_subject, "");
		INIT_GAME_STR("GameWorld/CowReportGoodNews/wincontent", g_cow_report_good_news_win_content, "sssss");
		INIT_GAME_STR("GameWorld/CowReportGoodNews/losecontent", g_cow_report_good_news_lose_content, "sssss");
		INIT_GAME_STR("GameWorld/CowReportGoodNews/notice", g_cow_report_good_news_notice, "");
		INIT_GAME_STR("GameWorld/CowReportGoodNews/remind", g_cow_report_good_news_remind, "");

		INIT_GAME_STR("GameWorld/TigerBringsBlessing/subject", g_tiger_brings_blessing_subject, "");
		INIT_GAME_STR("GameWorld/TigerBringsBlessing/wincontent", g_tiger_brings_blessing_win_content, "sssss");
		INIT_GAME_STR("GameWorld/TigerBringsBlessing/losecontent", g_tiger_brings_blessing_lose_content, "sssss");
		INIT_GAME_STR("GameWorld/TigerBringsBlessing/notice", g_tiger_brings_blessing_notice, "");
		INIT_GAME_STR("GameWorld/TigerBringsBlessing/remind", g_tiger_brings_blessing_remind, "");

		INIT_GAME_STR("GameWorld/RandActivity/jixing_gaozhao_broadcast_1", g_randactivity_jixing_gaozhao_broadcast_1, "dsd");
		INIT_GAME_STR("GameWorld/RandActivity/jixing_gaozhao_broadcast_2", g_randactivity_jixing_gaozhao_broadcast_2, "dsdd");

		INIT_GAME_STR("GameWorld/RandActivity/yinhu_xianshi_notice", g_randactivity_yinhu_xianshi_notice, "dsd");
		INIT_GAME_STR("GameWorld/RandActivity/ciniu_yinxinghu_notice", g_randactivity_ciniu_yinxinghu_notice, "dsd");
		

		//////////////////////////////////////////////////	 开随机礼包  ////////////////////////////////////////////
		UNSTD_STATIC_CHECK(2 == OPEN_RANDGIFT_BROADCAST_ID_MAX_NUM);
		INIT_GAME_STR("GameWorld/OpenRandGift/randgift_1", g_open_randgift[0], "dsds");
		INIT_GAME_STR("GameWorld/OpenRandGift/randgift_2", g_open_randgift[1], "dsds");

		INIT_GAME_STR("GameWorld/RandActivity/leijichongzhi_subbject", g_leijichongzhi_subbject, "");
		INIT_GAME_STR("GameWorld/RandActivity/leijichongzhi_content", g_leijichongzhi_content, "");
		INIT_GAME_STR("GameWorld/RandActivity/leijichongzhi_day_content", g_leijichongzhi_day_content, "");

		INIT_GAME_STR("GameWorld/treasure_task/treasure_task_notice", g_treasure_task_notice, "ds");
		INIT_GAME_STR("GameWorld/treasure_task/treasure_task_subject", g_treasure_task_subject, "");
		INIT_GAME_STR("GameWorld/treasure_task/treasure_task_content", g_treasure_task_content, "");

		INIT_GAME_STR("GameWorld/RandActivity/CompensationSubject", g_ra_compensation_subject, "");
		INIT_GAME_STR("GameWorld/RandActivity/CompensationContent", g_ra_compensation_content, "");

		INIT_GAME_STR("GameWorld/RandActivity/jingji_yueka_reissue_subject", g_randactivity_jingji_yueka_reissue_subject, "d");
		INIT_GAME_STR("GameWorld/RandActivity/jingji_yueka_reissue_content", g_randactivity_jingji_yueka_reissue_content, "d");

		INIT_GAME_STR("GameWorld/QingYuan/wedding_meal_notice", g_wedding_meal_notice, "dd");
		INIT_GAME_STR("GameWorld/QingYuan/wedding_fb_bless_notice", g_wedding_fb_bless_notice, "sd");
		INIT_GAME_STR("GameWorld/QingYuan/wedding_invite_subject", g_wedding_invite_subject, "");
		INIT_GAME_STR("GameWorld/QingYuan/wedding_invite_content", g_wedding_invite_content, "ssdddddd");
		INIT_GAME_STR("GameWorld/QingYuan/wedding_regist_subject", g_wedding_regist_subject, "");
		INIT_GAME_STR("GameWorld/QingYuan/wedding_regist_content", g_wedding_regist_content, "ddddd");
		INIT_GAME_STR("GameWorld/QingYuan/wedding_xun_you_subject", g_wedding_xun_you_subject, "");
		INIT_GAME_STR("GameWorld/QingYuan/wedding_xun_you_content", g_wedding_xun_you_content, "ddddd");
		INIT_GAME_STR("GameWorld/QingYuan/wedding_xun_you_notice", g_wedding_xun_you_notice, "ss");
		
			
		INIT_GAME_STR("GameWorld/QingYuan/qing_yuan_create_broadcast", g_qing_yuan_create_broadcast, "dsds");
		INIT_GAME_STR("GameWorld/QingYuan/qing_yuan_dismiss_subject", g_qing_yuan_dismiss_subject, "");
		INIT_GAME_STR("GameWorld/QingYuan/qing_yuan_dismiss_content", g_qing_yuan_dismiss_content, "dddd");
		INIT_GAME_STR("GameWorld/QingYuan/qing_yuan_give_gift_subject", g_qing_yuan_give_gift_subject, "");
		INIT_GAME_STR("GameWorld/QingYuan/qing_yuan_give_gift_content", g_qing_yuan_give_gift_content, "dsd");
		INIT_GAME_STR("GameWorld/QingYuan/qing_yuan_duration_reward_subject", g_qing_yuan_duration_reward_subject, "");
		INIT_GAME_STR("GameWorld/QingYuan/qing_yuan_duration_reward_content", g_qing_yuan_duration_reward_content, "d");

		INIT_GAME_STR("GameWorld/fake_announcement_content_1", g_fake_announcement_content_1, "sdssds");
		INIT_GAME_STR("GameWorld/fake_announcement_content_2", g_fake_announcement_content_2, "ssds");

		INIT_GAME_STR("GameWorld/fallen_god_rank_reward_subject", g_fallen_god_rank_reward_subject, "");
		INIT_GAME_STR("GameWorld/fallen_god_rank_reward_content", g_fallen_god_rank_reward_content, "d");

		INIT_GAME_STR("GameWorld/MiniGame/subject", g_mini_game_subject, "s");
		INIT_GAME_STR("GameWorld/MiniGame/content", g_mini_game_content, "s");

		INIT_GAME_STR("Global/WorldArena/standby_reminder", g_world_arena_standby_reminder, "");
		INIT_GAME_STR("Global/WorldArena/force_start_battle_reminder", g_world_arena_force_start_battle_reminder, "");
		INIT_GAME_STR("Global/WorldArena/battle_end_leave_scene", g_world_arena_battle_end_leave_scene, "");
		INIT_GAME_STR("Global/WorldArena/season_rank_reward_subject", g_world_arena_season_rank_reward_subject, "d");
		INIT_GAME_STR("Global/WorldArena/season_rank_reward_content", g_world_arena_season_rank_reward_content, "dd");
		INIT_GAME_STR("Global/WorldArena/someone_leave_scene_wait_notice", g_world_arena_someone_leave_scene_wait_notice, "");
		INIT_GAME_STR("Global/WorldArena/fight_result_subject", g_world_arena_fight_result_subject, "");
		INIT_GAME_STR("Global/WorldArena/not_showup_notice", g_world_arena_not_show_up, "dd");
		INIT_GAME_STR("Global/WorldArena/schedule_release_notice_subject", g_world_arena_schedule_release_notice_subject, "");
		INIT_GAME_STR("Global/WorldArena/schedule_release_notice_content", g_world_arena_schedule_release_notice_content, "");
		INIT_GAME_STR("Global/WorldArena/win_box_reward_subject", g_world_arena_win_box_reward_subject, "");
		INIT_GAME_STR("Global/WorldArena/win_box_reward_content", g_world_arena_win_box_reward_content, "");
		INIT_GAME_STR("Global/WorldArena/day_reward_subject", g_world_arena_day_reward_subject, "");
		INIT_GAME_STR("Global/WorldArena/day_reward_content", g_world_arena_day_reward_content, "");
		INIT_GAME_STR("Global/WorldArena/signup_reward_subject", g_world_arena_signup_reward_subject, "");
		INIT_GAME_STR("Global/WorldArena/signup_reward_content", g_world_arena_signup_reward_content, "");
		INIT_GAME_STR("Global/WorldArena/opponent_not_showup_yet", g_world_arena_opponent_not_showup_yet, "");
		INIT_GAME_STR("Global/WorldArena/opponent_showup", g_world_arena_opponent_showup, "s");
		INIT_GAME_STR("Global/WorldArena/start_battle_fail_draw", g_world_arena_start_battle_failed_draw, "");
		INIT_GAME_STR("Global/WorldArena/start_battle_fail_win", g_world_arena_start_battle_failed_win, "");
		INIT_GAME_STR("Global/WorldArena/start_battle_fail_lose", g_world_arena_start_battle_failed_lose, "");
		INIT_GAME_STR("GameWorld/PeakToken/peak_token_noticee", g_peak_token_notice, "sddddds");
		INIT_GAME_STR("GameWorld/PeakToken/peak_token_reward_noticee", g_peak_token_reward_notice, "sd");
		INIT_GAME_STR("GameWorld/Partner/strengthen_broadcast", g_partner_strengthen_broadcast, "dsddd");
		
		INIT_GAME_STR("GameWorld/QingYuanDuiDuiPeng/prom", g_qingyuan_duiduipeng_prom, "d");
		INIT_GAME_STR("GameWorld/QingYuanDuiDuiPeng/prom_1", g_qingyuan_duiduipeng_prom_1, "dsd");
		INIT_GAME_STR("GameWorld/QingYuanDuiDuiPeng/jifen", g_qingyuan_duiduipeng_jifen, "d");
		INIT_GAME_STR("GameWorld/QingYuanDuiDuiPeng/jifen_1", g_qingyuan_duiduipeng_jifen_1, "dd");
		INIT_GAME_STR("GameWorld/QingYuanDuiDuiPeng/rank_subject", g_qingyuan_duiduipeng_rank_subject, "");
		INIT_GAME_STR("GameWorld/QingYuanDuiDuiPeng/rank_content", g_qingyuan_duiduipeng_rank_content, "d");

		INIT_GAME_STR("GameWorld/TianGongXunBao/broadcast", g_tian_gong_xun_bao_broadcast, "dsd");
		INIT_GAME_STR("GameWorld/TianGongXunBao/subject", g_tian_gong_xun_bao_subject, "");
		INIT_GAME_STR("GameWorld/TianGongXunBao/content", g_tian_gong_xun_bao_content, "");

		INIT_GAME_STR("GameWorld/HundredGodBoss/role_hundred_god_boss_time_over_subject", g_role_hundred_god_boss_time_over_subject, "");
		INIT_GAME_STR("GameWorld/HundredGodBoss/role_hundred_god_boss_time_over_content", g_role_hundred_god_boss_time_over_content, "");
		INIT_GAME_STR("GameWorld/HundredGodBoss/role_hundred_god_boss_reward_reissue_content", g_role_hundred_god_boss_reward_reissue_content, "");

		INIT_GAME_STR("Global/UTAGuess/retroactiv_subject", g_uta_guess_reward_retroactiv_subject, "");
		INIT_GAME_STR("Global/UTAGuess/retroactiv_content", g_uta_guess_reward_retroactiv_content, "");
		INIT_GAME_STR("Global/UTAGuess/bet_retroactiv_subject", g_uta_guess_reward_bet_retroactiv_subject, "");
		INIT_GAME_STR("Global/UTAGuess/bet_retroactiv_content", g_uta_guess_reward_bet_retroactiv_content, "");

		INIT_GAME_STR("Global/UniverseTeamArena/WinBoxRewardSubject", g_uta_win_box_reward_subject, "");
		INIT_GAME_STR("Global/UniverseTeamArena/WinBoxRewardContent", g_uta_win_box_reward_content, "");
		INIT_GAME_STR("Global/UniverseTeamArena/WinBoxRewardReissueContent", g_uta_win_box_reissue_content, "");
		INIT_GAME_STR("Global/UniverseTeamArena/TodayJoinRewardSubject", g_uta_today_join_reward_subject, "");
		INIT_GAME_STR("Global/UniverseTeamArena/TodayJoinRewardContent", g_uta_today_join_reward_content, "");

		INIT_GAME_STR("Global/UniverseTeamArena/RankRewardSubject", g_uta_rank_reward_subject, "");
		INIT_GAME_STR("Global/UniverseTeamArena/QualificationRankRewardContent", g_uta_qualification_rank_reward_content, "sd");
		INIT_GAME_STR("Global/UniverseTeamArena/MainRankRewardContent", g_uta_main_rank_reward_content, "sd");

		INIT_GAME_STR("Global/UniverseTeamArena/AdvanceRewardSubject", g_uta_advance_reward_subject, "");
		INIT_GAME_STR("Global/UniverseTeamArena/AdvanceRewardContent", g_uta_advance_reward_content, "ss");

		INIT_GAME_STR("Global/UniverseTeamArena/PrepareQualificationNotice", g_uta_qualification_prepare_notice, "d");
		INIT_GAME_STR("Global/UniverseTeamArena/PrepareMainNotice", g_uta_main_prepare_notice, "d");
		INIT_GAME_STR("Global/UniverseTeamArena/EnterQualificationNotice", g_uta_qualification_enter_notice, "d");
		INIT_GAME_STR("Global/UniverseTeamArena/EnterMainNotice", g_uta_main_enter_notice, "d");

		INIT_GAME_STR("Battle/FastForward/ContinueBattle", g_fastforward_battle_continue, "d");
		INIT_GAME_STR("Global/UniverseTeamArena/ServerRewardSubject", g_uta_server_reward_subject, "");
		INIT_GAME_STR("Global/UniverseTeamArena/ServerRewardContent", g_uta_server_reward_content, "");
		INIT_GAME_STR("GameWorld/RandActivity/yuan_qi_fan_pai_notice", g_ra_yuan_qi_fan_pai_notice, "dsd");

		INIT_GAME_STR("GameWorld/RandActivity/god_beast_advent_subject", g_randactivity_god_beast_advent_subject, "");
		INIT_GAME_STR("GameWorld/RandActivity/god_beast_advent_content", g_randactivity_god_beast_advent_content, "");
		INIT_GAME_STR("GameWorld/RandActivity/god_beast_advent_receive_content", g_randactivity_god_beast_advent_receive_content, "");

		INIT_GAME_STR("GameWorld/PinJiaYouLi/pinjia_youli_subject", g_pinjia_youli_subject, "");
		INIT_GAME_STR("GameWorld/PinJiaYouLi/pinjia_youli_content", g_pinjia_youli_content, "");

		INIT_GAME_STR("GameWorld/HongMengTianJie/world_first_notice", g_hmtj_world_first_notice, "sd");
		INIT_GAME_STR("GameWorld/HongMengTianJie/world_first_reward_subject", g_hmtj_world_first_reward_subject, "");
		INIT_GAME_STR("GameWorld/HongMengTianJie/world_first_reward_content", g_hmtj_world_first_reward_content, "d");
		INIT_GAME_STR("GameWorld/HongMengTianJie/role_first_reward_subject", g_hmtj_role_first_reward_subject, "");
		INIT_GAME_STR("GameWorld/HongMengTianJie/role_first_reward_content", g_hmtj_role_first_reward_content, "");
		INIT_GAME_STR("GameWorld/HongMengTianJie/tean_not_times_can_enter_notice", g_hmtj_tean_not_times_can_enter_notice, "ds");
		INIT_GAME_STR("GameWorld/HongMengTianJie/team_not_equip_can_enter_notice", g_hmtj_team_not_equip_can_enter_notice, "ds");
		INIT_GAME_STR("GameWorld/HongMengTianJie/team_not_pass_can_enter_notice", g_hmtj_team_not_pass_can_enter_notice, "ds");
		INIT_GAME_STR("GameWorld/HongMengTianJie/team_not_level_can_enter_notice", g_hmtj_team_not_level_can_enter_notice, "ds");
		INIT_GAME_STR("GameWorld/HongMengTianJie/reward_notice", g_hmtj_reward_notice, "dsd");

		INIT_GAME_STR("GameWorld/HolyEquip/fushi_takeoff_subject", g_holy_equip_fushi_takeoff_subject, "");
		INIT_GAME_STR("GameWorld/HolyEquip/fushi_takeoff_content", g_holy_equip_fushi_takeoff_content, "");

		INIT_GAME_STR("GameWorld/RandActivity/duan_yang_xiang_nang_subject", g_randactivity_duan_yang_xiang_nang_subject, "");
		INIT_GAME_STR("GameWorld/RandActivity/duan_yang_xiang_nang_content", g_randactivity_duan_yang_xiang_nang_content, "");

		INIT_GAME_STR("GameWorld/RandActivity/salty_sweet_battle_subject", g_randactivity_salty_sweet_battle_subject, "");
		INIT_GAME_STR("GameWorld/RandActivity/salty_sweet_battle_content", g_randactivity_salty_sweet_battle_content, "");
		INIT_GAME_STR("GameWorld/RandActivity/salty_sweet_battle_lose_content", g_randactivity_salty_sweet_battle_lose_content, "");

		INIT_GAME_STR("GameWorld/Drop/drop_broadcast", g_drop_broadcast, "dsd");
		INIT_GAME_STR("GameWorld/RandActivity/get_item_broadcast", g_randactivity_get_item_broadcast, "dsddd");
		INIT_GAME_STR("GameWorld/RandActivity/get_item_broadcast_2", g_randactivity_get_item_broadcast_2, "dsddd");
		INIT_GAME_STR("GameWorld/RandActivity/get_item_broadcast_3", g_randactivity_get_item_broadcast_3, "dsddd");
		INIT_GAME_STR("GameWorld/RandActivity/randactivity_zhui_yue_shang_dian_get_item_broadcast", g_randactivity_zhui_yue_shang_dian_get_item_broadcast, "dsd");

		INIT_GAME_STR("GameWorld/RandActivity/meili_qixi_subject", g_meili_qixi_rank_reward_subject, "");
		INIT_GAME_STR("GameWorld/RandActivity/meili_qixi_content", g_meili_qixi_rank_reward_content, "d");
		
		INIT_GAME_STR("GameWorld/Robot/real_role_robot_reward_subject", g_real_role_robot_reward_subject, "");
		INIT_GAME_STR("GameWorld/Robot/real_role_robot_reward_content", g_real_role_robot_reward_content, "");

		INIT_GAME_STR("Global/Guild/g_role_guild_task_notice_subject", g_role_guild_task_notice_subject, "");
		INIT_GAME_STR("Global/Guild/g_role_guild_task_notice_content", g_role_guild_task_notice_content, "");
		
		return true;
	}
}
