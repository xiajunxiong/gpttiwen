#ifndef __GAMELOG_H__
#define __GAMELOG_H__

#include "ilogmodule.h"
#include "servercommon/servercommon.h"
#include "servercommon/logdef.h"
#include "world.h"

class ILogModule;

namespace gamelog
{
	extern LogAgent g_log_rank;				// ���а�
	extern LogAgent g_log_world;
	extern LogAgent g_log_scenemanager;
	extern LogAgent g_log_scene;
	extern LogAgent g_log_messagehandler;

	extern LogAgent g_log_msgmoniter;
	extern LogAgent g_log_login;
	extern LogAgent g_log_debug;
	extern LogAgent g_log_save_error;
	extern LogAgent g_log_serious_error;
	
	extern LogAgent g_log_moneygold;
	extern LogAgent g_log_moneycoin;
	extern LogAgent g_log_moneyimmortalcoin;
	extern LogAgent g_log_money_other_currency;
	extern LogAgent g_log_knapsack;
	extern LogAgent g_log_monitor;
	extern LogAgent g_log_role_num;
	extern LogAgent g_log_pet;
	extern LogAgent g_log_title;
	extern LogAgent g_log_achievement;
	extern LogAgent g_log_quick;
	extern LogAgent g_log_gm;
	extern LogAgent g_log_collection;
	extern LogAgent g_log_battle;
	extern LogAgent g_log_task;
	extern LogAgent g_log_task_stats;
	extern LogAgent g_log_money_allcoin;
	extern LogAgent g_log_workshop;
	extern LogAgent g_log_surface;
	extern LogAgent g_log_guild;
	extern LogAgent g_log_role_data;
	extern LogAgent g_log_mail;
	extern LogAgent g_log_rolemail;
	extern LogAgent g_log_rand_activity;						
	extern LogAgent g_log_cmd;
	extern LogAgent g_log_drop;
	extern LogAgent g_log_forbid;			// ���Է����־
	extern LogAgent g_log_trade_market;
	extern LogAgent g_log_team_fight;
	extern LogAgent g_log_treasure_hunt;
	extern LogAgent g_log_colosseum;
	extern LogAgent g_log_exchange;
	extern LogAgent g_log_guild_fight;
	extern LogAgent g_log_maze_racer;
	extern LogAgent g_log_activity;
	extern LogAgent g_log_chongzhi;
	extern LogAgent g_log_roledrop;
	extern LogAgent g_log_npc;			// npc��־
	extern LogAgent g_log_robot;
	extern LogAgent g_log_convert_shop;
	extern LogAgent g_log_role_exp;
	extern LogAgent g_log_offline_exp;
	extern LogAgent g_log_boss_chapter;
	extern LogAgent g_log_first_kill;
	extern LogAgent g_log_bounty_task;
	extern LogAgent g_log_social;						// �罻��־ ���� ����
	extern LogAgent g_log_rolecross;
	extern LogAgent g_log_capability;					// ���ս����־
	extern LogAgent g_log_video;
	extern LogAgent g_log_animal_race;					// ���޾�����־
	extern LogAgent g_log_only_fight;					// ��һ�а�(���˰������Ҷ�)
	extern LogAgent g_log_qm_discount;					// ȫ���Ź�
	extern LogAgent g_log_elapse;
	extern LogAgent g_log_personrank_snapshot;			// �������а�������־
	extern LogAgent g_log_pet_rank_snapshot;			// �������а�������־
	extern LogAgent g_log_item_track;					// ��Ʒ׷����־
	extern LogAgent g_log_medal;						// ѫ����־
	extern LogAgent g_Log_courage_challenge_rank;		// ��ħ֮������
	extern LogAgent g_Log_chief_election;				// ��ϯ��ѡ
	extern LogAgent g_Log_chief_peak;					// ��ϯ�۷�ս
	extern LogAgent g_log_partner_qualityup;			// ������
	extern LogAgent g_log_huan_jie_zhan_chang;			// �ý�ս��
	extern LogAgent g_log_trade_credit;
	extern LogAgent g_log_auto_catch_pet;
	extern LogAgent g_log_gather_dream_record;
	extern LogAgent g_log_role_activity;				// ���˻��־
	extern LogAgent g_log_role_cmd;
	extern LogAgent g_log_platform_battle;
	extern LogAgent g_log_brave_ground;
	extern LogAgent g_log_chat_system;
	extern LogAgent g_log_xun_bao;
	extern LogAgent g_log_rmi_back_manager;				// rmiback ����
	extern LogAgent g_log_shan_hai_boss;
	extern LogAgent g_log_anecdote_task;
	extern LogAgent g_log_platform_battle;
	extern LogAgent g_log_train;
	extern LogAgent g_log_partner;				// ���
	extern LogAgent g_log_school_exercise;
	extern LogAgent g_log_shen_shou_jiang_lin;				// ���޽���
	extern LogAgent g_log_prestige;
	extern LogAgent g_log_levelcomplement;
	extern LogAgent g_Log_treasure_map;
	extern LogAgent g_log_chun_ri_deng_feng;
	extern LogAgent g_log_zu_he_he_li;					// ��Ϻ���
	extern LogAgent g_log_fashion_shop;					
	extern LogAgent g_log_ling_pai_tian_ci;
	extern LogAgent g_log_norex_use_gift_item;				// ʹ��ֱ��������
	extern LogAgent g_log_active_system;
	extern LogAgent g_log_energy;
	extern LogAgent g_Log_smart_mounts_system;				// ����ϵͳ
	extern LogAgent g_Log_realm_break;						// ����ϵͳ
	extern LogAgent g_log_pet_monopoly;
	extern LogAgent g_log_role_material;					// ���龳(���ϸ���)
	extern LogAgent g_log_cumulative_recharge;				// �ۼƳ�ֵ(��װ����)
	extern LogAgent g_log_cloud_arena;						// �ƶ˾���
	extern LogAgent g_log_relic_looting_reward;				// �ż��ᱦ
	extern LogAgent g_log_guild_honor_battle;				// ��������ս
	extern LogAgent g_log_dream_notes_challenge;			// ��Ԩ��¼
	extern LogAgent g_log_login_comm_monitor;				// login��gameworld��Э���monitor
	extern LogAgent g_log_battle_comm_monitor;				// battle��gameworld��Э���monitor
	extern LogAgent g_log_cross_comm_monitor;				// cross��gameworld��Э���monitor
	extern LogAgent g_log_market;							// �г�
	extern LogAgent g_log_salary;							// ����ֵ�Ͷӳ�����
	extern LogAgent g_log_one_yuan_gou;						// һԪ��
	extern LogAgent g_log_role_heart_skill;					// �ķ�
	extern LogAgent g_log_role_top_level;					// �۷�ȼ�
	extern LogAgent g_log_ling_yu_ji_jin;					// �������
	extern LogAgent g_log_function_stats;					// ����ͳ����־
	extern LogAgent g_log_lucky_bag;						// ��������
	extern LogAgent g_log_month_investment;					// �¶�Ͷ��
	extern LogAgent g_log_cai_shen_ju_bao;					// �������۱�
	extern LogAgent g_log_ding_zhi_bao_xiang;				// ������Ʊ���
	extern LogAgent g_log_ding_zhi_bao_xiang_2;				// ������Ʊ���2
	extern LogAgent g_log_ding_zhi_bao_xiang_3;				// ������Ʊ���3
	extern LogAgent g_log_zhuan_shu_li_bao;					// ���ר�����
	extern LogAgent g_log_xian_shi_bao_he;					// �����ʱ����
	extern LogAgent g_log_xian_shi_bao_he_2;					// �����ʱ����2
	extern LogAgent g_log_xian_shi_bao_he_3;					// �����ʱ����3(�ļ�����)
	extern LogAgent g_log_xian_shi_bao_he_4;				// �����ʱ����4(�ļ�����)
	extern LogAgent g_log_xian_shi_bao_dai;					// �����ʱ����
	extern LogAgent g_log_xian_shi_bao_dai_2;					// �����ʱ����2
	extern LogAgent g_log_fa_bao;							// ����
	extern LogAgent g_lun_hui_wang_chuan;					// �ֻ�����
	extern LogAgent g_log_guild_rank_snapshot;				// �������а�������־
	extern LogAgent g_log_role_guild_inborn;				// ��������
	extern LogAgent g_log_role_common_save;					// ��ɫͨ�ñ�����־
	extern LogAgent g_log_first_recharge;					// �׳�ͨ����־
	extern LogAgent g_log_fb_other;							// ����-����
	extern LogAgent g_log_hong_bao;							// ���
	extern LogAgent g_log_zhu_ling;							//����
	extern LogAgent g_log_month_week_card;					// �¿��ܿ���־
	extern LogAgent g_log_prestige_task;
	extern LogAgent g_log_give_gift;
	extern LogAgent g_log_burning_summer;					// ���ڻ-��ȼ����
	extern LogAgent g_log_cool_summer;						// ���ڻ-��������
	extern LogAgent g_log_national_day;						// ����
	extern LogAgent g_log_zhen_pin_shang_pu;						// ��Ʒ����
	extern LogAgent g_log_cow_report_good_news;				// ��ţ��ϲ
	extern LogAgent g_log_zhu_ling;							// ����
	extern LogAgent g_log_ra_pai_mai_hang;					// ����-������
	extern LogAgent g_log_role_attr_pellet;					// ���Ե�
	extern LogAgent g_log_duel;								// ����
	extern LogAgent g_log_role_unique_check;
	extern LogAgent g_log_server_competition;				// ��Ӫ�-ȫ����ƴ
	extern LogAgent g_log_tian_ming;						// ����
	extern LogAgent g_log_shan_hai_meet;					// ɽ�����
	extern LogAgent g_log_jie_yi;							// ����
	extern LogAgent g_log_advance_career;					// ְҵ����
	extern LogAgent g_log_tiger_brings_blessing;			// �����ɸ�
	extern LogAgent g_log_life_skill;						// �����
	extern LogAgent g_log_xiu_wei;							// ��Ϊ
	extern LogAgent g_log_activity_silk_roads;				// ��ʱ�-˿��֮·
	extern LogAgent g_log_treasure_task;					// �����ر�
	extern LogAgent g_log_compensation;						// ����
	extern LogAgent g_log_rand_item_param;					// ���������ֵ��־
	extern LogAgent g_log_role_fly_system;					// ��ɫ����
	extern LogAgent g_log_score_task;						// ��ɫ����������
	extern LogAgent g_log_ra_jingji_yueka;					// ��ʱ�-�����¿�
	extern LogAgent g_log_world_arena;						// �۽�����
	extern LogAgent g_log_other;							// ����С����
	extern LogAgent g_log_qing_yuan;						// ��Ե
	extern LogAgent g_log_uta;	
	
	extern LogAgent g_log_life_skill;						// �����
	extern LogAgent g_log_xiu_wei;							// ��Ϊ
	extern LogAgent g_log_activity_silk_roads;				// ��ʱ�-˿��֮·
	extern LogAgent g_log_treasure_task;					// �����ر�
	extern LogAgent g_log_world_team_arena;					// Ӣ�ۻ���
	extern LogAgent g_log_common_save;						// ͨ�ñ�����־
	extern LogAgent g_log_accessories_ment;					// ����
	extern LogAgent g_log_cycleactivity;					// ���ڻ
	extern LogAgent g_zhou_qi;								// ����
	extern LogAgent g_tu_zi_kuai_pao;						// ���ӿ���
	extern LogAgent g_tu_zi_kuai_pao;						// ���ӿ���
	extern LogAgent g_log_role_hundred_god_boss;			// ����֮ʦ
	extern LogAgent g_jin_yi_tian_xiang;					// ������������
	extern LogAgent g_yuan_qi_yan_huo;						// ����Ԫ���̻�
	extern LogAgent g_zao_hua_equip;	
	extern LogAgent g_log_yuan_hun_zhu;						// Ԫ����
	extern LogAgent g_hongmeng_tianjie;						// �������
	extern LogAgent g_sheng_qi_system;						// ʥ��ϵͳ
	extern LogAgent g_log_zao_hua;							// �컯
	extern LogAgent g_log_notice;							// notice��ʾ��־
	extern LogAgent g_lucky_blind_box;					// ��������ä��
	extern LogAgent g_duan_yang_zi_xuan;					// ����������ѡ
	extern LogAgent g_duan_yang_fu_li;					// ������������
	extern LogAgent g_log_jin_long_ju_bao;				// ���������۱�
	extern LogAgent g_log_shi_tu;						// ʦͽ
	extern LogAgent g_log_ra_chong_ji_fan_li;			// ����-�輼����
	extern LogAgent g_log_ra_shen_shou_en_ze;			// ����-���޶���
	extern LogAgent g_log_qi_xi_te_hui;					// ������Ϧ�ػ�
	extern LogAgent g_log_fu_yu;						// ����
	extern LogAgent g_log_offline_battle_data;			// ����ս������ϵͳ
	extern LogAgent g_log_ra_shen_ci_hua_fu;			// ������ͻ���
	extern LogAgent g_log_tian_yi_yao_shi;				// ��������ҫ��
	extern LogAgent g_log_kua_fu_ying_xiong_lu;			// ���Ӣ��¼
	extern LogAgent g_log_xing_tu_system;				// ��ͼϵͳ
	extern LogAgent g_log_new_courage_challenge;		// ߱��֮��
	extern LogAgent g_log_zhui_yue_shang_dian;
	extern LogAgent g_log_role_huan_shou;				// ����ϵͳ
	extern LogAgent g_log_role_guild_task;		// 

	extern bool g_open_console_log;

	void LogInit(ILogModule *log, const std::string &logdir, bool open_console_log, int server_index);
}

#define CONSOLE_DEBUG(format, ...) \
	if (gamelog::g_open_console_log) printf("File: "__FILE__", Line: %05d: "format"\n", __LINE__, ##__VA_ARGS__); fflush(stdout);

#define ROLE_LOG_QUICK6(t, r, n3, n4, z3, z4) \
	World::GetInstWorld()->LOG_QUICK((r)->GetUserId().db_index, gamelog::g_log_quick, (t), (r)->GetUID(), (r)->GetName(), (r)->GetPlatName(), (n3), (n4), (z3), (z4), 0, 0, 0, 0, 0);

#define ROLE_LOG_QUICK10(t, r, n3, n4, z3, z4, n5, n6, n7, n8, n9) \
	World::GetInstWorld()->LOG_QUICK((r)->GetUserId().db_index, gamelog::g_log_quick, (t), (r)->GetUID(), (r)->GetName(), (r)->GetPlatName(), (n3), (n4), (z3), (z4), (n5), (n6), (n7), (n8), (n9));

#define ROLE_LOG_QUICK14(sid, t, u1, z1, z2, n3, n4, z3, z4) \
	World::GetInstWorld()->LOG_QUICK(sid, gamelog::g_log_quick, (t), (u1), (z1), (z2), (n3), (n4), (z3), (z4), 0, 0, 0, 0, 0);

#define ROLE_LOG_QUICK16(sid, t, u1, z1, z2, n3, n4, z3, z4, n5, n6, n7, n8, n9) \
	World::GetInstWorld()->LOG_QUICK(sid, gamelog::g_log_quick, (t), (u1), (z1), (z2), (n3), (n4), (z3), (z4), (n5), (n6), (n7), (n8), (n9));

#endif

