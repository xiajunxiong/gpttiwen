#include "gamelog.h"
#include "servercommon/servercommon.h"

namespace gamelog
{
	// static const char DEFAULT_LOG_FILE[]		= "gamelog/gamelog.txt";

	static const char DEFAULT_LOG_FILE[]		= "1";			// ��׼���

	LogAgent g_log_rank;							// ���а�
	LogAgent g_log_world;
	LogAgent g_log_scenemanager;
	LogAgent g_log_scene;
	LogAgent g_log_messagehandler;

	LogAgent g_log_msgmoniter;						// �ڴ�ؼ��
	LogAgent g_log_login;							// ��¼�ǳ���־
	LogAgent g_log_debug;							// ������־
	LogAgent g_log_save_error;						// RMI�洢����
	LogAgent g_log_serious_error;					// ���ش��� 
	LogAgent g_log_moneygold;						// Ԫ����־
	LogAgent g_log_moneycoin;						// ͭǮ��־
	LogAgent g_log_moneyimmortalcoin;				// �ɱ���־
	LogAgent g_log_money_allcoin;					// ͭ�����ɱҺ���ʱ����־
	LogAgent g_log_money_other_currency;			// �������������־
	LogAgent g_log_quick;							// ������־����ˮ��
	LogAgent g_log_knapsack;						// ��ɫ�����ֿ���־
	LogAgent g_log_monitor;							// �����־
	LogAgent g_log_role_num;						// ����������־
	LogAgent g_log_gm;								// gm����
	LogAgent g_log_pet;								// ����
	LogAgent g_log_collection;						// ��Ʒ
	LogAgent g_log_battle;							// ս����־
	LogAgent g_log_title;							// �ƺ�
	LogAgent g_log_achievement;						// �ɾ�
	LogAgent g_log_task;							// ����
	LogAgent g_log_task_stats;						// ����ͳ����־
	LogAgent g_log_workshop;						// ����
	LogAgent g_log_surface;							// �û�
	LogAgent g_log_guild;							// ����
	LogAgent g_log_role_data;						// ��ɫ����
	LogAgent g_log_mail;							// ����
	LogAgent g_log_rolemail;						// ��ɫ����
	LogAgent g_log_rand_activity;					// ����
	LogAgent g_log_cmd;								// �ⲿϵͳ������־
	LogAgent g_log_forbid;							// ���Է����־
	LogAgent g_log_drop;							// ����
	LogAgent g_log_trade_market;					// ������
	LogAgent g_log_team_fight;						// С�Ӿ���
	LogAgent g_log_treasure_hunt;					// Ѱ��
	LogAgent g_log_colosseum;						// ������
	LogAgent g_log_exchange;						// �һ�
	LogAgent g_log_guild_fight;						// ������Ҷ�
	LogAgent g_log_maze_racer;						// �Թ�����
	LogAgent g_log_activity;						// �
	LogAgent g_log_chongzhi;						// ��ֵ������־
	LogAgent g_log_roledrop;						
	LogAgent g_log_robot;							// ������
	LogAgent g_log_npc;								// npc��־
	LogAgent g_log_convert_shop;					// �һ��̵�
	LogAgent g_log_role_exp;						// ������־
	LogAgent g_log_offline_exp;						// ���߾�����־
	LogAgent g_log_boss_chapter;					// Boss�½���־
	LogAgent g_log_first_kill;						// ȫ����ɱ
	LogAgent g_log_bounty_task;						// �ͽ�������־
	LogAgent g_log_social;							// �罻��־
	LogAgent g_log_rolecross;						// �����־
	LogAgent g_log_capability;						// ���ս������־
	LogAgent g_log_video;
	LogAgent g_log_animal_race;						// ���޾�����־
	LogAgent g_log_only_fight;						// ��һ�а�(���˰������Ҷ�)
	LogAgent g_log_qm_discount;						// ȫ���Ź�
	LogAgent g_log_elapse;							// ִ��Ч����־
	LogAgent g_log_personrank_snapshot;				// �������а�������־
	LogAgent g_log_pet_rank_snapshot;				// �������а�������־
	LogAgent g_log_item_track;						// ��Ʒ׷����־
	LogAgent g_log_medal;							// ѫ����־
	LogAgent g_Log_courage_challenge_rank;			// ��ħ֮������
	LogAgent g_Log_chief_election;					// ��ϯ��ѡ
	LogAgent g_Log_chief_peak;						// ��ϯ�۷�ս
	LogAgent g_log_partner_qualityup;				// ������
	LogAgent g_log_huan_jie_zhan_chang;				// �ý�ս��
	LogAgent g_log_trade_credit;					// ��������
	LogAgent g_log_auto_catch_pet;					// �Զ�ץ��
	LogAgent g_log_gather_dream_record;				// ���弯Ը��¼
	LogAgent g_log_role_activity;					// ���˻��־
	LogAgent g_log_role_cmd;						// role_cmd����־
	LogAgent g_log_platform_battle;					// ��̨ս
	LogAgent g_log_brave_ground;					// �´��ع�
	LogAgent g_log_chat_system;						// ����ϵͳ
	LogAgent g_log_xun_bao;							// Ѱ��
	LogAgent g_log_rmi_back_manager;				// rmiback ����
	LogAgent g_log_shan_hai_boss;					// ɽ������
	LogAgent g_log_anecdote_task;					// ��������
	LogAgent g_log_train;							// ��������
	LogAgent g_log_partner;							// ���
	LogAgent g_log_school_exercise;					// У������
	LogAgent g_log_shen_shou_jiang_lin;				// ���޽���
	LogAgent g_log_prestige;						// ����
	LogAgent g_log_levelcomplement;					// �ȼ�����
	LogAgent g_Log_treasure_map;					// �ر�ͼ
	LogAgent g_log_chun_ri_deng_feng;				// ���յǷ�
	LogAgent g_log_zu_he_he_li;						// ��Ϻ���
	LogAgent g_log_fashion_shop;					// ʱװ�̵�
	LogAgent g_log_ling_pai_tian_ci;				// �������
	LogAgent g_log_norex_use_gift_item;				// ʹ��ֱ��������
	LogAgent g_log_active_system;					// ��Ծϵͳ
	LogAgent g_log_energy;							// �ؾ�����
	LogAgent g_Log_smart_mounts_system;				// ����ϵͳ
	LogAgent g_Log_realm_break;						// ����ͻ��
	LogAgent g_log_pet_monopoly;					// ����ؾ�
	LogAgent g_log_role_material;					// ���龳(���ϸ���)
	LogAgent g_log_cumulative_recharge;				// �ۼƳ�ֵ(��װ����)
	LogAgent g_log_cloud_arena;						// �ƶ˾���
	LogAgent g_log_relic_looting_reward;			// �ż��ᱦ
	LogAgent g_log_guild_honor_battle;				// ��������ս
	LogAgent g_log_dream_notes_challenge;			// ��Ԩ��¼
	LogAgent g_log_login_comm_monitor;				// login��gameworld��Э���monitor
	LogAgent g_log_battle_comm_monitor;				// battle��gameworld��Э���monitor
	LogAgent g_log_cross_comm_monitor;				// cross��gameworld��Э���monitor
	LogAgent g_log_market;							// �г�	
	LogAgent g_log_role_heart_skill;				// �ķ�
	LogAgent g_log_role_top_level;					// �۷�ȼ�
	LogAgent g_log_salary;							// ����ֵ�Ͷӳ�����
	LogAgent g_log_one_yuan_gou;					// һԪ��
	LogAgent g_log_ling_yu_ji_jin;					// �������
	LogAgent g_log_function_stats;					// ����ͳ����־
	LogAgent g_log_lucky_bag;						// ��������
	LogAgent g_log_month_investment;				// �¶�Ͷ��
	LogAgent g_log_cai_shen_ju_bao;					// �������۱�
	LogAgent g_log_ding_zhi_bao_xiang;				// ������Ʊ���
	LogAgent g_log_ding_zhi_bao_xiang_2;				// ������Ʊ���2
	LogAgent g_log_ding_zhi_bao_xiang_3;				// ������Ʊ���3
	LogAgent g_log_zhuan_shu_li_bao;				// ���ר�����
	LogAgent g_log_xian_shi_bao_he;					// �����ʱ����
	LogAgent g_log_xian_shi_bao_he_2;					// �����ʱ����2
	LogAgent g_log_xian_shi_bao_he_3;					// �����ʱ����3(�ļ�����)
	LogAgent g_log_xian_shi_bao_he_4;				// �����ʱ����4(�ļ�����)
	LogAgent g_log_xian_shi_bao_dai;				// �����ʱ����
	LogAgent g_log_xian_shi_bao_dai_2;				// �����ʱ����2
	LogAgent g_log_fa_bao;							// ����
	LogAgent g_lun_hui_wang_chuan;					// �ֻ�����
	LogAgent g_log_guild_rank_snapshot;				// �������а�������־
	LogAgent g_log_role_guild_inborn;				// ��������
	LogAgent g_log_role_common_save;				// ��ɫͨ�ñ�����־
	LogAgent g_log_fb_other;						// ����-����
	LogAgent g_log_hong_bao;						// ���
	LogAgent g_log_zhu_ling;						// ����
	LogAgent g_log_first_recharge;					// �׳�ͨ����־
	LogAgent g_log_month_week_card;					// �¿��ܿ���־
	LogAgent g_log_prestige_task;					// ����������־
	LogAgent g_log_give_gift;						// ��������
	LogAgent g_log_burning_summer;					// ���ڻ-��ȼ����
	LogAgent g_log_cool_summer;						// ���ڻ-��������
	LogAgent g_log_national_day;					// ����
	LogAgent g_log_zhen_pin_shang_pu;				// ��Ʒ����
	LogAgent g_log_ra_pai_mai_hang;					// ����-������
	LogAgent g_log_role_attr_pellet;				// ���Ե�
	LogAgent g_log_duel;							// ����
	LogAgent g_log_role_unique_check;				// ����׷��Role�ĳ�Ա����m_role_module_mgrΪ�յ�BUG
	LogAgent g_log_server_competition;				// ��Ӫ�-ȫ����ƴ
	LogAgent g_log_tian_ming;						// ����
	LogAgent g_log_shan_hai_meet;					// ɽ�����
	LogAgent g_log_jie_yi;							// ����
	LogAgent g_log_cow_report_good_news;			// ��ţ��ϲ
	LogAgent g_log_advance_career;					// ְҵ����
	LogAgent g_log_life_skill;						// �����
	LogAgent g_log_xiu_wei;							// ��Ϊ
	LogAgent g_log_activity_silk_roads;				// ��ʱ�-˿��֮·
	LogAgent g_log_treasure_task;					// �����ر�
	LogAgent g_log_compensation;					// ����
	LogAgent g_log_tiger_brings_blessing;			// �����ɸ�
	LogAgent g_log_rand_item_param;					// ���߾�ֵ��־
	LogAgent g_log_role_fly_system;					// ��ɫ����
	LogAgent g_log_score_task;						// ��ɫ����������
	LogAgent g_log_ra_jingji_yueka;					// ��ʱ�-�����¿�
	LogAgent g_log_qing_yuan;						// ��Ե
	LogAgent g_log_world_arena;						// �۽�����
	LogAgent g_log_other;							// ����С����
	LogAgent g_log_world_team_arena;				// Ӣ�ۻ���
	LogAgent g_log_common_save;						// ͨ�ñ���log
	LogAgent g_log_accessories_ment;				// ����
	LogAgent g_log_cycleactivity;					// ���ڻ
	LogAgent g_zhou_qi;								// ����
	LogAgent g_tu_zi_kuai_pao;						// ���ӿ���
	LogAgent g_log_uta;								// ���µ�һ����
	LogAgent g_log_role_hundred_god_boss;			// ����֮ʦ
	LogAgent g_jin_yi_tian_xiang;					// ������������
	LogAgent g_yuan_qi_yan_huo;					// ����Ԫ���̻�
	LogAgent g_log_yuan_hun_zhu;					// Ԫ����	
	LogAgent g_zao_hua_equip;						// �컯װ��
	LogAgent g_hongmeng_tianjie;					// �������
	LogAgent g_sheng_qi_system;						// ʥ��ϵͳ
	LogAgent g_log_zao_hua;						// �컯
	LogAgent g_log_notice;
	LogAgent g_duan_yang_zi_xuan;					// ����������ѡ
	LogAgent g_lucky_blind_box;					// ��������ä��
	LogAgent g_duan_yang_fu_li;					// ������������
	LogAgent g_log_jin_long_ju_bao;				// ���������۱�
	LogAgent g_log_shi_tu;				// ʦͽ
	LogAgent g_log_ra_chong_ji_fan_li;			// ����-�輼����
	LogAgent g_log_ra_shen_shou_en_ze;			// �������޶���
	LogAgent g_log_qi_xi_te_hui;				// ������Ϧ�ػ�
	LogAgent g_log_fu_yu;						// ����
	LogAgent g_log_offline_battle_data;			// ����ս������ϵͳ
	LogAgent g_log_ra_shen_ci_hua_fu;			// ������ͻ���
	LogAgent g_log_tian_yi_yao_shi;				// ��������ҫ��
	LogAgent g_log_kua_fu_ying_xiong_lu;		// ���Ӣ��¼
	LogAgent g_log_xing_tu_system;				// ��ͼϵͳ
	LogAgent g_log_new_courage_challenge;		// ߱��֮��
	LogAgent g_log_zhui_yue_shang_dian;			// ����׷���̵�
	LogAgent g_log_role_huan_shou;				// ����ϵͳ
	LogAgent g_log_role_guild_task;	//��������

	bool g_open_console_log = false; 

	void LogInit(ILogModule *log, const std::string &logdir, bool open_console_log, int server_index)
	{
		g_open_console_log = open_console_log;

		g_log_world.SetLogModule(log);
		g_log_world.SetCatagory("World", DEFAULT_LOG_FILE);

		g_log_scenemanager.SetLogModule(log);
		g_log_scenemanager.SetCatagory("SceneManager", DEFAULT_LOG_FILE);

		char gamelog_dir[256] = { 0 };
		SNPRINTF(gamelog_dir, sizeof(gamelog_dir), "gamelog_%d", server_index);
		std::string real_logdir = logdir + std::string(gamelog_dir);

		g_log_relic_looting_reward.SetLogModule(log);
		g_log_relic_looting_reward.SetCatagory("RelicLooting", (real_logdir + "/relic_looting/relic_looting.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_scene.SetLogModule(log);
		g_log_scene.SetCatagory("scene", (real_logdir + "/scene/scene.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_rank.SetLogModule(log);
		g_log_rank.SetCatagory("rank", (real_logdir + "/rank/rank.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_msgmoniter.SetLogModule(log);
		g_log_msgmoniter.SetCatagory("MsgMoniter", (real_logdir + "/mem_moniter/mem_moniter.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_messagehandler.SetLogModule(log);
		g_log_messagehandler.SetCatagory("MessageHandler", (real_logdir + "/message/messagehandler.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_login.SetLogModule(log);
		g_log_login.SetCatagory("Login", (real_logdir + "/login/login.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_debug.SetLogModule(log);
		g_log_debug.SetCatagory("Debug", (real_logdir + "/debug/debug.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_save_error.SetLogModule(log);
		g_log_save_error.SetCatagory("SaveError", (real_logdir + "/save_error/save_error.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_serious_error.SetLogModule(log);
		g_log_serious_error.SetCatagory("SeriousError", (real_logdir + "/seriouserror/seriouserror.txt").c_str());

		g_log_moneygold.SetLogModule(log);
		g_log_moneygold.SetCatagory("MoneyGold", (real_logdir + "/money_gold/money_gold.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_moneycoin.SetLogModule(log);
		g_log_moneycoin.SetCatagory("MoneyCoin", (real_logdir + "/money_coin/money_coin.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_moneyimmortalcoin.SetLogModule(log);
		g_log_moneyimmortalcoin.SetCatagory("MoneyImmortalCoin", (real_logdir + "/money_immortal_coin/money_immortal_coin.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_money_allcoin.SetLogModule(log);
		g_log_money_allcoin.SetCatagory("MoneyAllCoin", (real_logdir + "/money_allcoin/money_allcoin.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_money_other_currency.SetLogModule(log);
		g_log_money_other_currency.SetCatagory("MoneyOtherCoin", (real_logdir + "/money/money_other_currency/money_other_currency.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_quick.SetLogModule(log);
		g_log_quick.SetCatagory("quick", (real_logdir + "/quick/quick.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_knapsack.SetLogModule(log);
		g_log_knapsack.SetCatagory("Knapsack", (real_logdir + "/knapsack/knapsack.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_monitor.SetLogModule(log);
		g_log_monitor.SetCatagory("Monitor", (real_logdir + "/monitor/monitor.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_role_num.SetLogModule(log);
		g_log_role_num.SetCatagory("RoleNum", (real_logdir + "/rolenum/rolenum.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_pet.SetLogModule(log);
		g_log_pet.SetCatagory("Pet", (real_logdir + "/pet/pet.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_collection.SetLogModule(log);
		g_log_collection.SetCatagory("Collection", (real_logdir + "/collection/collection.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_gm.SetLogModule(log);
		g_log_gm.SetCatagory("GM", (real_logdir + "/gm/gm.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_battle.SetLogModule(log);
		g_log_battle.SetCatagory("Battle", (real_logdir + "/battle/battle.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_achievement.SetLogModule(log);
		g_log_achievement.SetCatagory("Achievement", (real_logdir + "/achievement/achievement.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_title.SetLogModule(log);
		g_log_title.SetCatagory("Title", (real_logdir + "/title/title.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_task.SetLogModule(log);
		g_log_task.SetCatagory("Task", (real_logdir + "/task/task.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_task_stats.SetLogModule(log);
		g_log_task_stats.SetCatagory("TaskState", (real_logdir + "/taskstats/taskstats.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_workshop.SetLogModule(log);
		g_log_workshop.SetCatagory("Workshop", (real_logdir + "/workshop/workshop.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_surface.SetLogModule(log);
		g_log_surface.SetCatagory("Surface", (real_logdir + "/surface/surface.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_guild.SetLogModule(log);
		g_log_guild.SetCatagory("Guild", (real_logdir + "/guild/guild.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_role_data.SetLogModule(log);
		g_log_role_data.SetCatagory("RoleData", (real_logdir + "/role_data/role_data.txt").c_str(), ILogModule::NFIS_HOUR);

		g_log_mail.SetLogModule(log);
		g_log_mail.SetCatagory("mail", (real_logdir + "/mail/mail.txt").c_str(), ILogModule::NFIS_HOUR);

		g_log_rolemail.SetLogModule(log);
		g_log_rolemail.SetCatagory("rolemail", (real_logdir + "/rolemail/rolemail.txt").c_str(), ILogModule::NFIS_HOUR);

		g_log_rand_activity.SetLogModule(log);
		g_log_rand_activity.SetCatagory("RandActivity", (real_logdir + "/randactivity/randactivity.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_cmd.SetLogModule(log);
		g_log_cmd.SetCatagory("Cmd", (real_logdir + "/cmd/cmd.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_forbid.SetLogModule(log);
		g_log_forbid.SetCatagory("Forbid", (real_logdir + "/forbid/forbid.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_drop.SetLogModule(log);
		g_log_drop.SetCatagory("Drop", (real_logdir + "/drop/drop.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_trade_market.SetLogModule(log);
		g_log_trade_market.SetCatagory("TradeMarket", (real_logdir + "/trademarket/trademarket.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_team_fight.SetLogModule(log);
		g_log_team_fight.SetCatagory("TeamFight", (real_logdir + "/teamfight/teamfight.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_treasure_hunt.SetLogModule(log);
		g_log_treasure_hunt.SetCatagory("TreasureHunt", (real_logdir + "/treasurehunt/treasurehunt.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_colosseum.SetLogModule(log);
		g_log_colosseum.SetCatagory("Colosseum", (real_logdir + "/colosseum/colosseum.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_exchange.SetLogModule(log);
		g_log_exchange.SetCatagory("Exchange", (real_logdir + "/exchange/exchange.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_guild_fight.SetLogModule(log);
		g_log_guild_fight.SetCatagory("ActGuildFight", (real_logdir + "/guildfight/guildfight.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_maze_racer.SetLogModule(log);
		g_log_maze_racer.SetCatagory("ActMazeRacer", (real_logdir + "/mazeracer/mazeracer.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_activity.SetLogModule(log);
		g_log_activity.SetCatagory("Activity", (real_logdir + "/activity/activity.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_chongzhi.SetLogModule(log);
		g_log_chongzhi.SetCatagory("Chongzhi", (real_logdir + "/chongzhi/chongzhi.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_roledrop.SetLogModule(log);
		g_log_roledrop.SetCatagory("RoleDrop", (real_logdir + "/roledrop/roledrop.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_robot.SetLogModule(log);
		g_log_robot.SetCatagory("Robot", (real_logdir + "/robot/robot.txt").c_str(), ILogModule::NFIS_HOUR);

		g_log_npc.SetLogModule(log);
		g_log_npc.SetCatagory("npc", (real_logdir + "/npc/npc.txt").c_str(), ILogModule::NFIS_HOUR);

		g_log_convert_shop.SetLogModule(log);
		g_log_convert_shop.SetCatagory("ConvertShop", (real_logdir + "/convert_shop/convert_shop.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_role_exp.SetLogModule(log);
		g_log_role_exp.SetCatagory("RoleExp", (real_logdir + "/role_exp/role_exp.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_offline_exp.SetLogModule(log);
		g_log_offline_exp.SetCatagory("OfflineExp", (real_logdir + "/offline_exp/offline_exp.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_boss_chapter.SetLogModule(log);
		g_log_boss_chapter.SetCatagory("BossChapter", (real_logdir + "/boss_chapter/boss_chapter.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_first_kill.SetLogModule(log);
		g_log_first_kill.SetCatagory("FirstSkill", (real_logdir + "/first_kill/first_kill.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_bounty_task.SetLogModule(log);
		g_log_bounty_task.SetCatagory("BountyTask", (real_logdir + "/bountytask/bountytask.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_social.SetLogModule(log);
		g_log_social.SetCatagory("Social", (real_logdir + "/social/social.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_rolecross.SetLogModule(log);
		g_log_rolecross.SetCatagory("RoleCross", (real_logdir + "/rolecross/rolecross.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_capability.SetLogModule(log);
		g_log_capability.SetCatagory("RoleCapability", (real_logdir + "/rolecapability/rolecapability.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_video.SetLogModule(log);
		g_log_video.SetCatagory("BattleVideo", (real_logdir + "/battlevideo/battlevideo.txt").c_str(), ILogModule::NFIS_DAY);
		g_log_animal_race.SetLogModule(log);
		g_log_animal_race.SetCatagory("AnimalRace", (real_logdir + "/animalrace/animalrace.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_only_fight.SetLogModule(log);
		g_log_only_fight.SetCatagory("Actonlyfight", (real_logdir + "/onlyfight/onlyfight.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_qm_discount.SetLogModule(log);
		g_log_qm_discount.SetCatagory("QuanMinDiscount", (real_logdir + "/qmdicount/qmdicount.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_elapse.SetLogModule(log);
		g_log_elapse.SetCatagory("Elapse", (real_logdir + "/elapse/elapse.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_personrank_snapshot.SetLogModule(log);
		g_log_personrank_snapshot.SetCatagory("PersonRankSnapshot", (real_logdir + "/personranksnapshot/personranksnapshot.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_pet_rank_snapshot.SetLogModule(log);
		g_log_pet_rank_snapshot.SetCatagory("PetRankSnapshot", (real_logdir + "/petranksnapshot/petranksnapshot.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_item_track.SetLogModule(log);
		g_log_item_track.SetCatagory("ItemTrack", (real_logdir + "/item_track/item_track.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_medal.SetLogModule(log);
		g_log_medal.SetCatagory("Medal", (real_logdir + "/medal/medal.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_Log_courage_challenge_rank.SetLogModule(log);
		g_Log_courage_challenge_rank.SetCatagory("CourageChallengeRank", (real_logdir + "/courage_challenge_rank/courage_challenge_rank.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_Log_chief_election.SetLogModule(log);
		g_Log_chief_election.SetCatagory("ChiefElection", (real_logdir + "/chiefelection/chiefelection.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_Log_chief_peak.SetLogModule(log);
		g_Log_chief_peak.SetCatagory("ChiefPeak", (real_logdir + "/chiefpeak/chiefpeak.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_partner_qualityup.SetLogModule(log);
		g_log_partner_qualityup.SetCatagory("ParnterQuality", (real_logdir + "/partner_qualityup/partner_qualityup.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_huan_jie_zhan_chang.SetLogModule(log);
		g_log_huan_jie_zhan_chang.SetCatagory("HuanJieZhanChang", (real_logdir + "/huanjiezhanchang/huanjiezhanchang.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_trade_credit.SetLogModule(log);
		g_log_trade_credit.SetCatagory("TradeCredit", (real_logdir + "/trade_credit/trade_credit.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_auto_catch_pet.SetLogModule(log);
		g_log_auto_catch_pet.SetCatagory("AutoCatchPet", (real_logdir + "/auto_catch_pet/auto_catch_pet.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_gather_dream_record.SetLogModule(log);
		g_log_gather_dream_record.SetCatagory("GatherDreamRecord", (real_logdir + "/gather_dream_record/gather_dream_record.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_role_activity.SetLogModule(log);
		g_log_role_activity.SetCatagory("RoleActivity", (real_logdir + "/RoleActivity/RoleActivity.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_role_cmd.SetLogModule(log);
		g_log_role_cmd.SetCatagory("RoleCmd", (real_logdir + "/role_cmd/role_cmd.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_platform_battle.SetLogModule(log);
		g_log_platform_battle.SetCatagory("PlatformBattle", (real_logdir + "/PlatformBattle/platform_battle.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_brave_ground.SetLogModule(log);
		g_log_brave_ground.SetCatagory("BraveGround", (real_logdir + "/BraveGround/brave_ground.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_chat_system.SetLogModule(log);
		g_log_chat_system.SetCatagory("ChatSystem", (real_logdir + "/ChatSystem/chat_system.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_xun_bao.SetLogModule(log);
		g_log_xun_bao.SetCatagory("XunBao", (real_logdir + "/XunBao/xun_bao.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_rmi_back_manager.SetLogModule(log);
		g_log_rmi_back_manager.SetCatagory("RMIBackManager", (real_logdir + "/RMIBackManager/RMIBackManager.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_shan_hai_boss.SetLogModule(log);
		g_log_shan_hai_boss.SetCatagory("ShanHaiBoss", (real_logdir + "/ShanHaiBoss/shan_hai_boss.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_anecdote_task.SetLogModule(log);
		g_log_anecdote_task.SetCatagory("AnecdoteTask", (real_logdir + "/AnecdoteTask/anecdote_task.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_platform_battle.SetLogModule(log);
		g_log_platform_battle.SetCatagory("PlatformBattle", (real_logdir + "/PlatformBattle/platform_battle.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_train.SetLogModule(log);
		g_log_train.SetCatagory("Train", (real_logdir + "/train/train.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_partner.SetLogModule(log);
		g_log_partner.SetCatagory("Partner", (real_logdir + "/partner/partner.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_school_exercise.SetLogModule(log);
		g_log_school_exercise.SetCatagory("SchoolExercise", (real_logdir + "/school_exercise/school_exercise.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_shen_shou_jiang_lin.SetLogModule(log);
		g_log_shen_shou_jiang_lin.SetCatagory("ShenShouJiangLin", (real_logdir + "/shen_shou_jiang_lin/shen_shou_jiang_lin.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_prestige.SetLogModule(log);
		g_log_prestige.SetCatagory("Prestige", (real_logdir + "/prestige/prestige.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_levelcomplement.SetLogModule(log);
		g_log_levelcomplement.SetCatagory("levelcomplement", (real_logdir + "/levelcomplement/levelcomplement.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_Log_treasure_map.SetLogModule(log);
		g_Log_treasure_map.SetCatagory("TreasureMap", (real_logdir + "/treasuremap/treasuremap.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_chun_ri_deng_feng.SetLogModule(log);
		g_log_chun_ri_deng_feng.SetCatagory("ChunRiDengFeng", (real_logdir + "/chunridengfeng/chunridengfeng.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_zu_he_he_li.SetLogModule(log);
		g_log_zu_he_he_li.SetCatagory("ZuHeHeLi", (real_logdir + "/zuheheli/zuheheli.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_fashion_shop.SetLogModule(log);
		g_log_fashion_shop.SetCatagory("FashionShop", (real_logdir + "/fashionshop/fashionshop.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_ling_pai_tian_ci.SetLogModule(log);
		g_log_ling_pai_tian_ci.SetCatagory("LingPaiTianCi", (real_logdir + "/LingPaiTianCi/lingpaitianci.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_norex_use_gift_item.SetLogModule(log);
		g_log_norex_use_gift_item.SetCatagory("UseGiftItem", (real_logdir + "/usegiftitem/usegiftitem.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_active_system.SetLogModule(log);
		g_log_active_system.SetCatagory("ActiveSystem", (real_logdir + "/active_system/active_system.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_energy.SetLogModule(log);
		g_log_energy.SetCatagory("Energy", (real_logdir + "/energy/energy.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_Log_smart_mounts_system.SetLogModule(log);
		g_Log_smart_mounts_system.SetCatagory("SmartMountsSystem", (real_logdir + "/smartmountssystem/smartmountssystem.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_Log_realm_break.SetLogModule(log);
		g_Log_realm_break.SetCatagory("RealmBreak", (real_logdir + "/realmbreak/realmbreak.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_pet_monopoly.SetLogModule(log);
		g_log_pet_monopoly.SetCatagory("PetMonopoly", (real_logdir + "/petmonopoly/petmonopoly.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_role_material.SetLogModule(log);
		g_log_role_material.SetCatagory("RoleMaterial", (real_logdir + "/rolematerial/rolematerial.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_cumulative_recharge.SetLogModule(log);
		g_log_cumulative_recharge.SetCatagory("Cumulative_recharge", (real_logdir + "/Cumulative_recharge/Cumulative_recharge.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_cloud_arena.SetLogModule(log);
		g_log_cloud_arena.SetCatagory("CloudArena", (real_logdir + "/cloudarena/cloudarena.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_guild_honor_battle.SetLogModule(log);
		g_log_guild_honor_battle.SetCatagory("guildhonorbattle", (real_logdir + "/guildhonorbattle/guildhonorbattle.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_dream_notes_challenge.SetLogModule(log);
		g_log_dream_notes_challenge.SetCatagory("DreamNotesChallenge", (real_logdir + "/dreamnoteschallenge/dreamnoteschallenge.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_login_comm_monitor.SetLogModule(log);
		g_log_login_comm_monitor.SetCatagory("LoginCommMonitor", (real_logdir + "/internal_comm_monitor/login_comm_monitor.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_battle_comm_monitor.SetLogModule(log);
		g_log_battle_comm_monitor.SetCatagory("BattleCommMonitor", (real_logdir + "/internal_comm_monitor/battle_comm_monitor.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_cross_comm_monitor.SetLogModule(log);
		g_log_cross_comm_monitor.SetCatagory("CrossCommMonitor", (real_logdir + "/internal_comm_monitor/cross_comm_monitor.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_market.SetLogModule(log);
		g_log_market.SetCatagory("Market", (real_logdir + "/market/market.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_salary.SetLogModule(log);
		g_log_salary.SetCatagory("Salary", (real_logdir + "/salary/salary.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_one_yuan_gou.SetLogModule(log);
		g_log_one_yuan_gou.SetCatagory("RaOneYuanGou", (real_logdir + "/raoneyuangou/raoneyuangou.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_function_stats.SetLogModule(log);
		g_log_function_stats.SetCatagory("FunctionStats", (real_logdir + "/functionstats/functionstats.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_ling_yu_ji_jin.SetLogModule(log);
		g_log_ling_yu_ji_jin.SetCatagory("RaLingYuJiJin", (real_logdir + "/ralingyujijin/ralingyujijin.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_lucky_bag.SetLogModule(log);
		g_log_lucky_bag.SetCatagory("RaLuckyBag", (real_logdir + "/luckybag/raluckybag.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_month_investment.SetLogModule(log);
		g_log_month_investment.SetCatagory("month_investment", (real_logdir + "/month_investment/month_investment.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_cai_shen_ju_bao.SetLogModule(log);
		g_log_cai_shen_ju_bao.SetCatagory("RaCaiShenJuBao", (real_logdir + "/caishenjubao/racaishenjubao.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_ding_zhi_bao_xiang.SetLogModule(log);
		g_log_ding_zhi_bao_xiang.SetCatagory("RaDingZhiBaoXiang", (real_logdir + "/dingzhibaoxiang/radingzhibaoxiang.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_ding_zhi_bao_xiang_2.SetLogModule(log);
		g_log_ding_zhi_bao_xiang_2.SetCatagory("RaDingZhiBaoXiang2", (real_logdir + "/dingzhibaoxiang2/radingzhibaoxiang2.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_ding_zhi_bao_xiang_3.SetLogModule(log);
		g_log_ding_zhi_bao_xiang_3.SetCatagory("RaDingZhiBaoXiang3", (real_logdir + "/dingzhibaoxiang3/radingzhibaoxiang3.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_zhuan_shu_li_bao.SetLogModule(log);
		g_log_zhuan_shu_li_bao.SetCatagory("RaZhuanShuLiBao", (real_logdir + "/zhuanshulibao/razhuanshulibao.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_xian_shi_bao_he.SetLogModule(log);
		g_log_xian_shi_bao_he.SetCatagory("RaXianShiBaoHe", (real_logdir + "/xianshibaohe/raxianshibaohe.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_xian_shi_bao_he_2.SetLogModule(log);
		g_log_xian_shi_bao_he_2.SetCatagory("RaXianShiBaoHe2", (real_logdir + "/xianshibaohe2/raxianshibaohe2.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_xian_shi_bao_he_3.SetLogModule(log);
		g_log_xian_shi_bao_he_3.SetCatagory("RaXianShiBaoHe3", (real_logdir + "/xianshibaohe3/raxianshibaohe3.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_xian_shi_bao_he_4.SetLogModule(log);
		g_log_xian_shi_bao_he_4.SetCatagory("RaXianShiBaoHe4", (real_logdir + "/xianshibaohe4/raxianshibaohe4.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_xian_shi_bao_dai.SetLogModule(log);
		g_log_xian_shi_bao_dai.SetCatagory("RaXianShiBaoDai", (real_logdir + "/xianshibaodai/raxianshibaodai.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_xian_shi_bao_dai_2.SetLogModule(log);
		g_log_xian_shi_bao_dai_2.SetCatagory("RaXianShiBaoDai2", (real_logdir + "/xianshibaodai2/raxianshibaodai2.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_fa_bao.SetLogModule(log);
		g_log_fa_bao.SetCatagory("fabao", (real_logdir + "/fabao/fabao.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_lun_hui_wang_chuan.SetLogModule(log);
		g_lun_hui_wang_chuan.SetCatagory("LunHuiWangChuan", (real_logdir + "/lunhuiwangchuan/lunhuiwangchuan.txt").c_str(), ILogModule::NFIS_HALF_DAY);


		g_log_guild_rank_snapshot.SetLogModule(log);
		g_log_guild_rank_snapshot.SetCatagory("GuildRankSnapshot", (real_logdir + "/GuildRankSnapshot/GuildRankSnapshot.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_role_guild_inborn.SetLogModule(log);
		g_log_role_guild_inborn.SetCatagory("RoleGuildInborn", (real_logdir + "/RoleGuildInborn/RoleGuildInborn.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_role_common_save.SetLogModule(log);
		g_log_role_common_save.SetCatagory("RoleCommonSave", (real_logdir + "/rolecommonsave/rolecommonsave.txt").c_str(), ILogModule::NFIS_HALF_HOUR);


		g_log_role_heart_skill.SetLogModule(log);
		g_log_role_heart_skill.SetCatagory("RoleHeartSkill", (real_logdir + "/roleheartskill/roleheartskill.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_role_top_level.SetLogModule(log);
		g_log_role_top_level.SetCatagory("role_top_level", (real_logdir + "/roletoplevel/roletoplevel.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_fb_other.SetLogModule(log);
		g_log_fb_other.SetCatagory("fb_other", (real_logdir + "/fb_other/fb_other.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_hong_bao.SetLogModule(log);
		g_log_hong_bao.SetCatagory("hongbao", (real_logdir + "/hongbao/hongbao.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_zhu_ling.SetLogModule(log);
		g_log_zhu_ling.SetCatagory("zhuling", (real_logdir + "/zhuling/zhuling.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_first_recharge.SetLogModule(log);
		g_log_first_recharge.SetCatagory("FirstRecharge", (real_logdir + "/firstrecharge/firstrecharge.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_month_week_card.SetLogModule(log);
		g_log_month_week_card.SetCatagory("MonthWeekCard", (real_logdir + "/monthweekcard/monthweekcard.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_prestige_task.SetLogModule(log);
		g_log_prestige_task.SetCatagory("PrestigeTask", (real_logdir + "/prestigetask/prestigetask.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_give_gift.SetLogModule(log);
		g_log_give_gift.SetCatagory("GiveGift", (real_logdir + "/givegift/givegift.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_burning_summer.SetLogModule(log);
		g_log_burning_summer.SetCatagory("BurningSummer", (real_logdir + "/BurningSummer/burningsummer.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_cool_summer.SetLogModule(log);
		g_log_cool_summer.SetCatagory("CoolSummer", (real_logdir + "/CoolSummer/coolsummer.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_national_day.SetLogModule(log);
		g_log_national_day.SetCatagory("NationalDay", (real_logdir + "/NationalDay/nationalday.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_zhen_pin_shang_pu.SetLogModule(log);
		g_log_zhen_pin_shang_pu.SetCatagory("ZhenPinShangPu", (real_logdir + "/zhenpinshangpu/zhenpinshangpu.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_cow_report_good_news.SetLogModule(log);
		g_log_cow_report_good_news.SetCatagory("CowReportGoodNews", (real_logdir + "/CowReportGoodNews/CowReportGoodNews.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_tiger_brings_blessing.SetLogModule(log);
		g_log_tiger_brings_blessing.SetCatagory("TigerBringsBlessing", (real_logdir + "/TigerBringsBlessing/TigerBringsBlessing.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_ra_pai_mai_hang.SetLogModule(log);
		g_log_ra_pai_mai_hang.SetCatagory("RAPaiMaiHang", (real_logdir + "/rapaimaihang/rapaimaihang.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_role_attr_pellet.SetLogModule(log);
		g_log_role_attr_pellet.SetCatagory("RoleAttrPellet", (real_logdir + "/roleattrpellet/roleattrpellet.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_duel.SetLogModule(log);
		g_log_duel.SetCatagory("Duel", (real_logdir + "/duel/duel.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_role_unique_check.SetLogModule(log);
		g_log_role_unique_check.SetCatagory("RoleUniqueCheck", (real_logdir + "/roleuniquecheck/roleuniquecheck.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_server_competition.SetLogModule(log);
		g_log_server_competition.SetCatagory("ServerCompetition", (real_logdir + "/servercompetition/servercompetition.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_tian_ming.SetLogModule(log);
		g_log_tian_ming.SetCatagory("TianMing", (real_logdir + "/TianMing/TianMing.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_shan_hai_meet.SetLogModule(log);
		g_log_shan_hai_meet.SetCatagory("RAShanHaiMeet", (real_logdir + "/shanhaimeet/shanhaimeet.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_jie_yi.SetLogModule(log);
		g_log_jie_yi.SetCatagory("JieYi", (real_logdir + "/jieyi/jieyi.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_advance_career.SetLogModule(log);
		g_log_advance_career.SetCatagory("AdvanceCareer", (real_logdir + "/AdvanceCareer/AdvanceCareer.txt").c_str(), ILogModule::NFIS_DAY);
		g_log_life_skill.SetLogModule(log);
		g_log_life_skill.SetCatagory("LifeSkill", (real_logdir + "/lifeskill/lifeskill.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_xiu_wei.SetLogModule(log);
		g_log_xiu_wei.SetCatagory("XiuWei", (real_logdir + "/XiuWei/XiuWei.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_activity_silk_roads.SetLogModule(log);
		g_log_activity_silk_roads.SetCatagory("silk_roads", (real_logdir + "/silk_roads/silk_roads.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_treasure_task.SetLogModule(log);
		g_log_treasure_task.SetCatagory("treasure_task", (real_logdir + "/treasure_task/treasure_task.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_compensation.SetLogModule(log);
		g_log_compensation.SetCatagory("compensation", (real_logdir + "/compensation/compensation.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_rand_item_param.SetLogModule(log);
		g_log_rand_item_param.SetCatagory("rand_item_param", (real_logdir + "/rand_item_param/rand_item_param.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

		g_log_role_fly_system.SetLogModule(log);
		g_log_role_fly_system.SetCatagory("roleflysystem", (real_logdir + "/roleflysystem/roleflysystem.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_score_task.SetLogModule(log);
		g_log_score_task.SetCatagory("ScoreTask", (real_logdir + "/scoretask/scoretask.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_ra_jingji_yueka.SetLogModule(log);
		g_log_ra_jingji_yueka.SetCatagory("JingJiYueKa", (real_logdir + "/jingjiyueka/jingjiyueka.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_qing_yuan.SetLogModule(log);
		g_log_qing_yuan.SetCatagory("QingYuan", (real_logdir + "/qingyuan/qingyuan.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_world_arena.SetLogModule(log);
		g_log_world_arena.SetCatagory("WorldArena", (real_logdir + "/world_arena/world_arena.txt").c_str(), ILogModule::NFIS_HALF_DAY);

		g_log_other.SetLogModule(log);
		g_log_other.SetCatagory("Other", (real_logdir + "/other/other.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_world_team_arena.SetLogModule(log);
		g_log_world_team_arena.SetCatagory("WorldTeamArena", (real_logdir + "/world_team_arena/world_team_arena.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_common_save.SetLogModule(log);
		g_log_common_save.SetCatagory("CommonSave", (real_logdir + "/CommonSave/CommonSave.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_accessories_ment.SetLogModule(log);
		g_log_accessories_ment.SetCatagory("accessories", (real_logdir + "/accessories/accessories.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_cycleactivity.SetLogModule(log);
		g_log_cycleactivity.SetCatagory("cycleactivity", (real_logdir + "/cycleactivity/cycleactivity.txt").c_str(), ILogModule::NFIS_DAY);

		g_zhou_qi.SetLogModule(log);
		g_zhou_qi.SetCatagory("zhouqi", (real_logdir + "/zhouqi/zhouqi.txt").c_str(), ILogModule::NFIS_DAY);

		g_tu_zi_kuai_pao.SetLogModule(log);
		g_tu_zi_kuai_pao.SetCatagory("tuzikuaipao", (real_logdir + "/tuzikuaipao/tuzikuaipao.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_uta.SetLogModule(log);
		g_log_uta.SetCatagory("UTA", (real_logdir + "/UTA/UTA.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_role_hundred_god_boss.SetLogModule(log);
		g_log_role_hundred_god_boss.SetCatagory("rolehundredgodboss", (real_logdir + "/rolehundredgodboss/rolehundredgodboss.txt").c_str(), ILogModule::NFIS_DAY);


		g_jin_yi_tian_xiang.SetLogModule(log);
		g_jin_yi_tian_xiang.SetCatagory("jinyitianxiang", (real_logdir + "/jinyitianxiang/jinyitianxiang.txt").c_str(), ILogModule::NFIS_DAY);

		g_yuan_qi_yan_huo.SetLogModule(log);
		g_yuan_qi_yan_huo.SetCatagory("yuanqiyanhuo", (real_logdir + "/yuanqiyanhuo/yuanqiyanhuo.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_yuan_hun_zhu.SetLogModule(log);
		g_log_yuan_hun_zhu.SetCatagory("yuanhunzhu", (real_logdir + "/yuanhunzhu/yuanhunzhu.txt").c_str(), ILogModule::NFIS_DAY);

		g_zao_hua_equip.SetLogModule(log);
		g_zao_hua_equip.SetCatagory("zao_hua", (real_logdir + "/zao_hua/zaohua.txt").c_str(), ILogModule::NFIS_HOUR);

		g_hongmeng_tianjie.SetLogModule(log);
		g_hongmeng_tianjie.SetCatagory("hongmeng_tianjie", (real_logdir + "/hongmeng_tianjie/hongmeng_tianjie.txt").c_str(), ILogModule::NFIS_DAY);

		g_sheng_qi_system.SetLogModule(log);
		g_sheng_qi_system.SetCatagory("shengqi", (real_logdir + "/shengqi/shengqi.txt").c_str(), ILogModule::NFIS_HOUR);

		g_log_zao_hua.SetLogModule(log);
		g_log_zao_hua.SetCatagory("zaohua", (real_logdir + "/zaohua/zaohua.txt").c_str(), ILogModule::NFIS_HOUR);		

		g_log_notice.SetLogModule(log);
		g_log_notice.SetCatagory("zaohua", (real_logdir + "/notice/notice.txt").c_str(), ILogModule::NFIS_HOUR);

		g_duan_yang_zi_xuan.SetLogModule(log);
		g_duan_yang_zi_xuan.SetCatagory("duanyangzixuan", (real_logdir + "/duanyangzixuan/duanyangzixuan.txt").c_str(), ILogModule::NFIS_HOUR);

		g_lucky_blind_box.SetLogModule(log);
		g_lucky_blind_box.SetCatagory("luckyblindbox", (real_logdir + "/luckyblindbox/luckyblindbox.txt").c_str(), ILogModule::NFIS_DAY);

		g_duan_yang_fu_li.SetLogModule(log);
		g_duan_yang_fu_li.SetCatagory("duanyangfuli", (real_logdir + "/duanyangfuli/duanyangfuli.txt").c_str(), ILogModule::NFIS_HOUR);

		g_log_jin_long_ju_bao.SetLogModule(log);
		g_log_jin_long_ju_bao.SetCatagory("jinlongjubao", (real_logdir + "/jinlong_jubao/jinlong_jubao.txt").c_str(), ILogModule::NFIS_HOUR);

		g_log_shi_tu.SetLogModule(log);
		g_log_shi_tu.SetCatagory("shitu", (real_logdir + "/shitu/shitu.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_ra_chong_ji_fan_li.SetLogModule(log);
		g_log_ra_chong_ji_fan_li.SetCatagory("chongjifanli", (real_logdir + "/chongjifanli/chongjifanli.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_ra_shen_shou_en_ze.SetLogModule(log);
		g_log_ra_shen_shou_en_ze.SetCatagory("shenshouenze", (real_logdir + "/shenshouenze/shenshouenze.txt").c_str(), ILogModule::NFIS_DAY);
		
		g_log_qi_xi_te_hui.SetLogModule(log);
		g_log_qi_xi_te_hui.SetCatagory("qixitehui", (real_logdir + "/qixitehui/qixitehui.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_fu_yu.SetLogModule(log);
		g_log_fu_yu.SetCatagory("fuyu", (real_logdir + "/fuyu/fuyu.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_offline_battle_data.SetLogModule(log);
		g_log_offline_battle_data.SetCatagory("offlinebattledata", (real_logdir + "/offline_battle_data/offline_battle_data.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_ra_shen_ci_hua_fu.SetLogModule(log);
		g_log_ra_shen_ci_hua_fu.SetCatagory("shencihuafu", (real_logdir + "/shencihuafu/shencihuafu.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_tian_yi_yao_shi.SetLogModule(log);
		g_log_tian_yi_yao_shi.SetCatagory("tianyiyaoshi", (real_logdir + "/tianyiyaoshi/tianyiyaoshi.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_kua_fu_ying_xiong_lu.SetLogModule(log);
		g_log_kua_fu_ying_xiong_lu.SetCatagory("kuafuyingxionglu", (real_logdir + "/kuafuyingxionglu/kuafuyingxionglu.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_xing_tu_system.SetLogModule(log);
		g_log_xing_tu_system.SetCatagory("xingtu", (real_logdir + "/xingtu/xingtu.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_new_courage_challenge.SetLogModule(log);
		g_log_new_courage_challenge.SetCatagory("newcouragechallenge", (real_logdir + "/newcouragechallenge/newcouragechallenge.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_zhui_yue_shang_dian.SetLogModule(log);
		g_log_zhui_yue_shang_dian.SetCatagory("kuafuyingxionglu", (real_logdir + "/zhuiyueshangdian/zhuiyueshangdian.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_role_huan_shou.SetLogModule(log);
		g_log_role_huan_shou.SetCatagory("rolehuanshou", (real_logdir + "/rolehuanshou/rolehuanshou.txt").c_str(), ILogModule::NFIS_DAY);

		g_log_role_guild_task.SetLogModule(log);
		g_log_role_guild_task.SetCatagory("roleguildtask", (real_logdir + "/roleguildtask/roleguildtask.txt").c_str(), ILogModule::NFIS_DAY);
	}
}