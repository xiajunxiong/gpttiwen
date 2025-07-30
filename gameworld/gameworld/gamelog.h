#ifndef __GAMELOG_H__
#define __GAMELOG_H__

#include "ilogmodule.h"
#include "servercommon/servercommon.h"
#include "servercommon/logdef.h"
#include "world.h"

class ILogModule;

namespace gamelog
{
	extern LogAgent g_log_rank;				// 排行榜
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
	extern LogAgent g_log_forbid;			// 禁言封号日志
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
	extern LogAgent g_log_npc;			// npc日志
	extern LogAgent g_log_robot;
	extern LogAgent g_log_convert_shop;
	extern LogAgent g_log_role_exp;
	extern LogAgent g_log_offline_exp;
	extern LogAgent g_log_boss_chapter;
	extern LogAgent g_log_first_kill;
	extern LogAgent g_log_bounty_task;
	extern LogAgent g_log_social;						// 社交日志 好友 仇人
	extern LogAgent g_log_rolecross;
	extern LogAgent g_log_capability;					// 玩家战力日志
	extern LogAgent g_log_video;
	extern LogAgent g_log_animal_race;					// 神兽径赛日志
	extern LogAgent g_log_only_fight;					// 以一敌百(个人版家族大乱斗)
	extern LogAgent g_log_qm_discount;					// 全民团购
	extern LogAgent g_log_elapse;
	extern LogAgent g_log_personrank_snapshot;			// 个人排行榜整点日志
	extern LogAgent g_log_pet_rank_snapshot;			// 宠物排行榜整点日志
	extern LogAgent g_log_item_track;					// 物品追踪日志
	extern LogAgent g_log_medal;						// 勋章日志
	extern LogAgent g_Log_courage_challenge_rank;		// 锢魔之塔排行
	extern LogAgent g_Log_chief_election;				// 首席竞选
	extern LogAgent g_Log_chief_peak;					// 首席巅峰战
	extern LogAgent g_log_partner_qualityup;			// 伙伴觉醒
	extern LogAgent g_log_huan_jie_zhan_chang;			// 幻界战场
	extern LogAgent g_log_trade_credit;
	extern LogAgent g_log_auto_catch_pet;
	extern LogAgent g_log_gather_dream_record;
	extern LogAgent g_log_role_activity;				// 个人活动日志
	extern LogAgent g_log_role_cmd;
	extern LogAgent g_log_platform_battle;
	extern LogAgent g_log_brave_ground;
	extern LogAgent g_log_chat_system;
	extern LogAgent g_log_xun_bao;
	extern LogAgent g_log_rmi_back_manager;				// rmiback 管理
	extern LogAgent g_log_shan_hai_boss;
	extern LogAgent g_log_anecdote_task;
	extern LogAgent g_log_platform_battle;
	extern LogAgent g_log_train;
	extern LogAgent g_log_partner;				// 伙伴
	extern LogAgent g_log_school_exercise;
	extern LogAgent g_log_shen_shou_jiang_lin;				// 神兽降临
	extern LogAgent g_log_prestige;
	extern LogAgent g_log_levelcomplement;
	extern LogAgent g_Log_treasure_map;
	extern LogAgent g_log_chun_ri_deng_feng;
	extern LogAgent g_log_zu_he_he_li;					// 组合贺礼
	extern LogAgent g_log_fashion_shop;					
	extern LogAgent g_log_ling_pai_tian_ci;
	extern LogAgent g_log_norex_use_gift_item;				// 使用直购礼包激活卡
	extern LogAgent g_log_active_system;
	extern LogAgent g_log_energy;
	extern LogAgent g_Log_smart_mounts_system;				// 灵骑系统
	extern LogAgent g_Log_realm_break;						// 境界系统
	extern LogAgent g_log_pet_monopoly;
	extern LogAgent g_log_role_material;					// 玉虚境(材料副本)
	extern LogAgent g_log_cumulative_recharge;				// 累计充值(神装惊世)
	extern LogAgent g_log_cloud_arena;						// 云端竞技
	extern LogAgent g_log_relic_looting_reward;				// 遗迹夺宝
	extern LogAgent g_log_guild_honor_battle;				// 家族荣誉战
	extern LogAgent g_log_dream_notes_challenge;			// 梦渊笔录
	extern LogAgent g_log_login_comm_monitor;				// login向gameworld发协议的monitor
	extern LogAgent g_log_battle_comm_monitor;				// battle向gameworld发协议的monitor
	extern LogAgent g_log_cross_comm_monitor;				// cross向gameworld发协议的monitor
	extern LogAgent g_log_market;							// 市场
	extern LogAgent g_log_salary;							// 侠义值和队长工资
	extern LogAgent g_log_one_yuan_gou;						// 一元购
	extern LogAgent g_log_role_heart_skill;					// 心法
	extern LogAgent g_log_role_top_level;					// 巅峰等级
	extern LogAgent g_log_ling_yu_ji_jin;					// 灵玉基金
	extern LogAgent g_log_function_stats;					// 功能统计日志
	extern LogAgent g_log_lucky_bag;						// 随机活动福袋
	extern LogAgent g_log_month_investment;					// 月度投资
	extern LogAgent g_log_cai_shen_ju_bao;					// 随机财神聚宝
	extern LogAgent g_log_ding_zhi_bao_xiang;				// 随机定制宝箱
	extern LogAgent g_log_ding_zhi_bao_xiang_2;				// 随机定制宝箱2
	extern LogAgent g_log_ding_zhi_bao_xiang_3;				// 随机定制宝箱3
	extern LogAgent g_log_zhuan_shu_li_bao;					// 随机专属礼包
	extern LogAgent g_log_xian_shi_bao_he;					// 随机限时宝盒
	extern LogAgent g_log_xian_shi_bao_he_2;					// 随机限时宝盒2
	extern LogAgent g_log_xian_shi_bao_he_3;					// 随机限时宝盒3(四季花牌)
	extern LogAgent g_log_xian_shi_bao_he_4;				// 随机限时宝盒4(四季宝盒)
	extern LogAgent g_log_xian_shi_bao_dai;					// 随机限时宝袋
	extern LogAgent g_log_xian_shi_bao_dai_2;					// 随机限时宝袋2
	extern LogAgent g_log_fa_bao;							// 法宝
	extern LogAgent g_lun_hui_wang_chuan;					// 轮回忘川
	extern LogAgent g_log_guild_rank_snapshot;				// 公会排行榜整点日志
	extern LogAgent g_log_role_guild_inborn;				// 家族灵脉
	extern LogAgent g_log_role_common_save;					// 角色通用保存日志
	extern LogAgent g_log_first_recharge;					// 首充通用日志
	extern LogAgent g_log_fb_other;							// 副本-其他
	extern LogAgent g_log_hong_bao;							// 红包
	extern LogAgent g_log_zhu_ling;							//铸灵
	extern LogAgent g_log_month_week_card;					// 月卡周卡日志
	extern LogAgent g_log_prestige_task;
	extern LogAgent g_log_give_gift;
	extern LogAgent g_log_burning_summer;					// 暑期活动-情燃季夏
	extern LogAgent g_log_cool_summer;						// 暑期活动-沁凉夏日
	extern LogAgent g_log_national_day;						// 国庆活动
	extern LogAgent g_log_zhen_pin_shang_pu;						// 珍品商铺
	extern LogAgent g_log_cow_report_good_news;				// 福牛报喜
	extern LogAgent g_log_zhu_ling;							// 铸灵
	extern LogAgent g_log_ra_pai_mai_hang;					// 随机活动-拍卖行
	extern LogAgent g_log_role_attr_pellet;					// 属性丹
	extern LogAgent g_log_duel;								// 决斗
	extern LogAgent g_log_role_unique_check;
	extern LogAgent g_log_server_competition;				// 运营活动-全服比拼
	extern LogAgent g_log_tian_ming;						// 天命
	extern LogAgent g_log_shan_hai_meet;					// 山海相逢
	extern LogAgent g_log_jie_yi;							// 结义
	extern LogAgent g_log_advance_career;					// 职业晋级
	extern LogAgent g_log_tiger_brings_blessing;			// 寅虎纳福
	extern LogAgent g_log_life_skill;						// 生活技能
	extern LogAgent g_log_xiu_wei;							// 修为
	extern LogAgent g_log_activity_silk_roads;				// 限时活动-丝绸之路
	extern LogAgent g_log_treasure_task;					// 九州秘宝
	extern LogAgent g_log_compensation;						// 补偿
	extern LogAgent g_log_rand_item_param;					// 道具随机净值日志
	extern LogAgent g_log_role_fly_system;					// 角色飞升
	extern LogAgent g_log_score_task;						// 角色评分任务达成
	extern LogAgent g_log_ra_jingji_yueka;					// 限时活动-竞技月卡
	extern LogAgent g_log_world_arena;						// 论剑九州
	extern LogAgent g_log_other;							// 其他小功能
	extern LogAgent g_log_qing_yuan;						// 情缘
	extern LogAgent g_log_uta;	
	
	extern LogAgent g_log_life_skill;						// 生活技能
	extern LogAgent g_log_xiu_wei;							// 修为
	extern LogAgent g_log_activity_silk_roads;				// 限时活动-丝绸之路
	extern LogAgent g_log_treasure_task;					// 九州秘宝
	extern LogAgent g_log_world_team_arena;					// 英雄会武
	extern LogAgent g_log_common_save;						// 通用保存日志
	extern LogAgent g_log_accessories_ment;					// 配饰
	extern LogAgent g_log_cycleactivity;					// 周期活动
	extern LogAgent g_zhou_qi;								// 周期
	extern LogAgent g_tu_zi_kuai_pao;						// 兔子快跑
	extern LogAgent g_tu_zi_kuai_pao;						// 兔子快跑
	extern LogAgent g_log_role_hundred_god_boss;			// 百神之师
	extern LogAgent g_jin_yi_tian_xiang;					// 随机活动锦衣天香
	extern LogAgent g_yuan_qi_yan_huo;						// 随机活动元气烟火
	extern LogAgent g_zao_hua_equip;	
	extern LogAgent g_log_yuan_hun_zhu;						// 元魂珠
	extern LogAgent g_hongmeng_tianjie;						// 鸿蒙天界
	extern LogAgent g_sheng_qi_system;						// 圣器系统
	extern LogAgent g_log_zao_hua;							// 造化
	extern LogAgent g_log_notice;							// notice提示日志
	extern LogAgent g_lucky_blind_box;					// 随机活动幸运盲盒
	extern LogAgent g_duan_yang_zi_xuan;					// 随机活动端阳自选
	extern LogAgent g_duan_yang_fu_li;					// 随机活动端阳福礼
	extern LogAgent g_log_jin_long_ju_bao;				// 随机活动金龙聚宝
	extern LogAgent g_log_shi_tu;						// 师徒
	extern LogAgent g_log_ra_chong_ji_fan_li;			// 随机活动-宠技返利
	extern LogAgent g_log_ra_shen_shou_en_ze;			// 随机活动-神兽恩泽
	extern LogAgent g_log_qi_xi_te_hui;					// 随机活动七夕特惠
	extern LogAgent g_log_fu_yu;						// 符玉
	extern LogAgent g_log_offline_battle_data;			// 离线战斗数据系统
	extern LogAgent g_log_ra_shen_ci_hua_fu;			// 随机活动神赐华福
	extern LogAgent g_log_tian_yi_yao_shi;				// 随机活动天衣耀世
	extern LogAgent g_log_kua_fu_ying_xiong_lu;			// 跨服英雄录
	extern LogAgent g_log_xing_tu_system;				// 星图系统
	extern LogAgent g_log_new_courage_challenge;		// 弑神之塔
	extern LogAgent g_log_zhui_yue_shang_dian;
	extern LogAgent g_log_role_huan_shou;				// 幻兽系统
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

