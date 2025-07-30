#ifndef __KNAPSACK_H__
#define __KNAPSACK_H__

#include "item/itembase.h"
#include "item/itemgriddata.h"

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/basedef.h"
#include "servercommon/serverdef.h"
#include "money.h"
#include <vector>
#include <set>

class Role;

enum GET_REASON_TYPE
{
	GET_REASON_INVALID = 0,							// 无效
	GET_REASON_MONSTER_WAVE,						//魔窟
	GET_REASON_TREASURE_MAP_TASK,					//宝图任务完成获得物品
	GET_REASON_CRYSTAL_FB_BOSS_REWARD,				//水晶副本 boss 奖励 param1:经验
	GET_REASON_FANTASY_FB_COUNT_REWARD,				//四象幻境奖励
	GET_REASON_XUN_BAO,								//寻宝
	GET_REASON_TASK,								//任务 param1:task_id
	GET_REASON_SHI_TU_FB,							//师徒副本
	GET_REASON_RAND_GIFT,							//随机礼包
	GET_REASON_WILD_BOSS,							//野外boss
	GET_REASON_FAST_PASS_CRYSTAL_FB,				//扫荡水晶副本 param1:经验，param2：次数
	GET_REASON_SHAN_HAI_BOSS,						//山海降妖
	GET_REASON_ANECDOTE_TASK,						//奇闻任务
	GET_REASON_DREAM_NOTES_CHALLENGE_FINISH,		//梦渊笔录通关
	GET_REASON_DREAM_NOTES_CHALLENGE_FAST_PASS,		//梦渊笔录扫荡
	GET_REASON_TASK_CHAIN_REWARD,					//商人的烦恼
	GET_REASON_EQUIP_DECOMPOSE,						//装备分解
	GET_REASON_MIJIQIWEN,							//秘境奇闻
	GET_REASON_PARTNER_LIENCE,						//伙伴凭证
	GET_REASON_MATERIAL_FAST_PASS,					//玉虚境 param1: 1.通关 0.扫荡
	GET_REASON_ACTIVE_FA_BAO_GET_ITEM,				//已经激活法宝获得道具
	GET_REASON_ADD_GOD_PRINT_ITEM,					//获得神印道具
	GET_REASON_PET_KEY_LEVEL_UP,					//宠物一键升级
	GET_REASON_TIAN_TI_DAO_HEN,						//天地道痕
	GET_REASON_PRESTIGE_TASK,						//声望任务
	GET_REASON_HUNDRED_GHOST,						//百鬼夜行
	GET_REASON_ACTVITIY_REWARD,						//运营活动奖励 param1:活动ID
	GET_RAASON_ACTIVE_TURNTABLE,					//活跃转盘
	GET_REASON_TASK_CHAIN_COMPLETION,				//商人的烦恼-环终奖励
	GET_REASON_DU_JIE,								//渡劫
	GET_REASON_REALM_BREAK_FIGHT,					//境界战斗（心灵之境）
	GET_REASON_HORCRUX_TIANCHENG,					//魂器天成
	GET_REASON_ENDLESS_TOWER,						//无尽之塔
	GET_REASON_ZAI_ZHAN_YUN_ZE,						//再战云泽
	GET_REASON_YUNZE_ER_SAN_SHI,					//云泽二三事
	GET_REASON_JIXING_GAOZHAO,						//吉星高照
	GET_REASON_WAN_LING_FB,							//万灵话本
	GET_REASON_YUNZE_SLOT_MACHINE,					//纷享同花礼
	GET_REASON_BOUNTY_TASK_REWARD,					//赏金转盘结束总奖励展示
	GET_REASON_LIFE_SKILL_MAKE,						//生活职业制作道具 P1:生活技能/职业类型
	GET_REASON_MIRAGE_TERRITORY,					//琉璃蜃境
	GET_REASON_TREASURE_TASK,						//九州秘宝
	GET_REASON_TREASURE_TASK_TOTAL_ITEM,			//九州秘宝 所有物品统计
	GET_REASON_FALLEN_GOD_REWARD,					//殒神之地
	GET_REASON_WEDDING_SUB_ACT_XIN_XIN_XIANG_YIN,	//婚宴-小游戏 心心相印/枣生桂子  p_1:铜币
	GET_REASON_PARTNER_PACT,						//伙伴--元神契约 获得提示
	GET_REASON_SHI_TU_CHUAN_GONG,					//师徒传功 p_1:当前得分  p_2:本周最高得分
	GET_REASON_WHERE_IS_THE_FAIRY,					//仙子去哪儿
	GET_REASON_LANTERN_RIDDIE,						//山海灯会
	GET_REASON_MONEY_BOX,							//货币红包 p_1:货币类型 p_2:货币数量
	GET_REASON_LIANHUAN_TASK,						//连环支线任务奖励
	GET_REASON_RESERVE,								//保留
	GET_REASON_YUAN_QI_YAN_HUO_OPEN,				//元气烟火 打开烟火
	GET_REASON_YUAN_QI_FAN_PAI,						//元气翻牌 翻牌
	GET_REASON_BAO_ZI_LAI_LE,						//包子来了,p1:是否成功(1-成功,0-失败)
	GET_REASON_HONGMENG_TIANJIE_PASS,				//鸿蒙天界 p1:是否助战
	GET_REASON_ZAO_HUA_DISCOMPOSE,					//造化装备分解
	GET_REASON_DUAN_YANG_XIANG_NANG,				//端阳香囊
	GET_REASON_LAN_TANG_CHU_MU,						//兰汤初沐
	GET_REASON_KE_JU_EXAM,							//科举考试
	GET_REASON_TIAN_YI_YAO_SHI,						//天衣耀世
	GET_REASON_SMART_MOUNTS_ACTIVE_CONVERT,			//激活灵骑道具转换
	GET_REASON_YUE_XI_DENG_MI,						//月夕灯谜 
	GET_REASON_TEAM_CHALLENGE_REWARD,				//挑战任务奖励
	GET_REASON_HUAN_LING_DECOMPOSE,					//幻兽系统分解
};


enum SEND_MAIL_TYPE
{
	SNED_MAIL_TYPE_DEFAULT = 0,								// 默认通知背包已满,改由邮件下发
	SEND_MAIL_TYPE_SEVEN_COLOR = 1,							// 七彩图鉴单个奖励
	SEND_MAIL_TYPE_SEVEN_COLOR_COUNT = 2,					// 七彩图鉴总奖励
	SEND_MAIL_TYPE_SIGN_UP = 3,								// 限时活动报名
	SEND_MAIL_TYPE_GUILD_BOSS_CHAPTER_KILL = 4,				// 家族试炼击杀奖励
	SEND_MAIL_TYPE_GUILD_BOSS_CHAPTER_NOTICE = 5,			// 家族试炼通知(已不用）
	SEND_MAIL_TYPE_TASK_REISSUE_ITEM = 6,					// 任务道具补发
	SEND_MAIL_TYPE_MAKE_UP_REWARD = 7,						// 运营活动-补齐奖励
	SEND_MAIL_TYPE_COURAGE_CHALLENGE = 8,					// 锢魔之塔背包满补发
	SEND_MAIL_TYPE_DREAM_NOTES_CHALLENGE = 9,				// 梦渊笔录背包满补发
	SEND_MAIL_TYPE_GOOD_GIFT_TURN = 10,						// 好礼转动背包满补发
	SEND_MAIL_TYPE_HELP_OTHERS = 11,						// 义胆助人背包满补发
	SEND_MAIL_TYPE_SECRET_TREASURE = 12,					// 特惠秘宝背包满补发
	SEND_MAIL_TYPE_LUN_HUI_WANG_CHUAN = 13,					// 轮回忘川背包满补发
	SEND_MAIL_TYPE_RELIC_LOOTING = 14,						// 遗迹夺宝背包满补发
	SEND_MAIL_TYPE_TOP_LEVEL = 15,							// 巅峰等级未领取补发
	SEND_MAIL_TYPE_FIRST_RECHARGE_EXCLUSIVE = 16,			// 首充专属背包满补发
	SEND_MAIL_TYPE_MONTH_CARD_BUY_CARD_OR_DAY_REWARD = 17,	// 月卡/周卡购买/每日奖励背包满补发
	SEND_MAIL_TYPE_MONTH_CARD_BUY_ITEM = 18,				// 购买月卡商城道具背包满补发
	SEND_MAIL_TYPE_ACTIVITY_REWARD = 19,					// 活动奖励,背包已满补发(通用)
	SEND_MAIL_TYPE_ACTIVITY_BUY_SHOP = 20,					// 购买活动,背包满补发(通用)
	SEND_MAIL_TYPE_FANG_FRAUD_REWARD = 21,					// 领取防诈骗奖励背包满补发
	SEND_MAIL_TYPE_BACK_REWARD = 22,						// 回归豪礼背包满补发
	SEND_MAIL_TYPE_SERVER_COMPETITION = 23,					// 全服比拼活动排名奖励
	SEND_MAIL_TYPE_ACTIVITY_END = 24,						// 活动奖励,活动奖励未及时领取补发 
	SEND_MAIL_TYPE_STONE_BACK = 25,							// 宝石镶嵌条件修改后，卸下低阶装备的未达到条件宝石
	SEND_MAIL_TYPE_HELP_OTHERS_2 = 26,						// 义胆助人2背包满补发
	SEND_MAIL_TYPE_SHAN_HAI_QUESTION = 27,					// 山海问答背包满补发
	SEND_MAIL_TYPE_SHAN_HAI_MEET = 28,						// 山海相逢背包满补发
	SEND_MAIL_TYPE_FATE_ENTANGLEMENT = 29,					// 山海缘起背包满补发
	SEND_MAIL_TYPE_YUN_ZE_TERRITORY = 30,					// 云泽画境背包满补发
	SEND_MAIL_TYPE_YUN_ZE_SLOT_MACHINE = 31,				// 纷享同花礼背包满补发
	SEND_MAIL_TYPE_YUN_ZE_ER_SAN_SHI = 32,					// 云泽二三事背包满补发
	SEND_MAIL_TYPE_YUN_ZE_BLOOMING_RECORD = 33,				// 云泽繁花录背包满补发
	SEND_MAIL_TYPE_YUNZE_CATCH_ELVES = 34,					// 精灵大抓捕背包满补发
	SEND_MAIL_TYPE_YUNZE_HAO_LI = 35,						// 云泽好礼背包满补发
	SEND_MAIL_TYPE_RA_XIAN_SHI_LEI_CHONG = 36,				// 随机活动限时累充背包满补发
	SEND_MAIL_TYPE_YUNZE_WISH = 37,							// 云泽祈愿背包满补发
	SEND_MAIL_TYPE_YUNZE_CUTE_PET = 38,						// 萌宠驾到背包满补发
	SEND_MAIL_TYPE_TREASURE_TASK = 39,						// 九州秘宝背包满补发
	SEND_MAIL_TYPE_TIGER_FISH_EVERY_YEAR = 40,				// 年年有鱼背包满补发
	SEND_MAIL_TYPE_TIGER_FIND_SPRING = 41,					// 寅虎探春背包满补发
	SEND_MAIL_TYPE_TIGER_TEASE_NEW_YEAR = 42,				// 寅虎逗新春背包满补发
	SEND_MAIL_TYPE_TIGER_LUCKY_MONEY = 43,					// 寅虎压岁背包满补发
	SEND_MAIL_TYPE_TIGER_AUSPICIOUS_SNOW = 44,				// 瑞雪丰年背包满补发
	SEND_MAIL_TYPE_TIGER_FIND_SPRING_YESTERDAY = 45,		// 寅虎探春前一天未领取奖励补发
	SEND_MAIL_TYPE_JINGJI_YUEKA_REISSUE_REWARD = 46,		// 竞技月卡奖励补发
	SEND_MAIL_TYPE_RONG_LIAN_YOU_LI = 47,					// 熔炼有礼奖励未领取补发
	SEND_MAIL_TYPE_WORLD_ARENA_WIN_BOX_REWARD = 48,			// 论剑九州胜场宝箱奖励补发
	SEND_MAIL_TYPE_WORLD_ARENA_SIGNUP_REWARD = 49,			// 论剑九州报名奖励补发
	SEND_MAIL_TYPE_WORLD_ARENA_DAILY_REWARD = 50,			// 论剑九州每日奖励补发
	SEND_MAIL_TYPE_PET_TALENT_RESET_RETURN = 51,			// 宠物天赋重置返还
	SEND_MAIL_TYPE_SAN_XIAN_GUI_DONG_REWARD = 52,			// 三仙归洞背包满补发
	SEND_MAIL_TYPE_TIAN_GONG_XUN_BAO_REWARD = 53,			// 天宫寻宝背包满补发
	SEND_MAIL_TYPE_HUNDRED_GOD_BOSS_TIME_OVER = 54,			// 百神之师超时补发
	SEND_MAIL_TYPE_HUNDRED_GOD_BOSS_REWARD_REISSUE = 55,	// 百神之师奖励补发
	SEND_MAIL_TYPE_TIGER_ACTIVITY = 56,						// 邮件名带活动号的背包满补发
	SEND_MAIL_TYPE_GOD_BEAST_ADVENT = 57,					// 异兽临世背包满补发
	SEND_MAIL_TYPE_GOD_BEAST_ADVENT_YESTERDAY = 58,			// 异兽临世补发前一天奖励
	SEND_MAIL_TYPE_BO_ZHONG_YOU_LI = 59,					// 播种有礼背包满补发
	SEND_MAIL_TYPE_YUAN_QI_FAN_PAI = 60,					// 元气翻牌背包满补发
	SEND_MAIL_TYPE_BAO_ZI_LAI_LE = 61,						// 包子来了背包满补发
	SEND_MAIL_TYPE_MIAO_BI_SHENG_HUA = 62,					// 妙笔生花背包满补发
	SEND_MAIL_TYPE_CHENG_FENG_PO_LANG = 63,					// 乘风破浪背包满补发
	SEND_MAIL_TYPE_PINJIA_YOULI = 64,						// 评价有礼背包满补发
	SEND_MAIL_TYPE_SALTY_SWEET_BATTLE = 65,					// 咸甜之争背包满补发
	SEND_MAIL_TYPE_DUAN_YANG_XIANG_NANG = 66,				// 端阳香囊背包满补发
	SEND_MAIL_TYPE_CANG_LONG_CI_FU = 67,					// 苍龙赐福背包满补发
	SEND_MAIL_TYPE_LAN_TANG_CHU_MU = 68,					// 兰汤初沐背包满补发
	SEND_MAIL_TYPE_ACTIVITY_REWARD_BAG_FULL = 69,			// 活动奖励,背包已满补发(通用)(跟19不同的文本)
	SEND_MAIL_TYPE_ACTIVITY_END_REWARD_REISSUE = 70,		// 活动奖励,活动奖励未及时领取补发(通用)(跟24不同的标题文本)
	SEND_MAIL_TYPE_ACTIVITY_BUY_BAG_FULL = 71,				// 活动购买,背包满补发
	SEND_MAIL_TYPE_COLOSSEUM_BATTLE_REWARD = 72,			// 竞技场战斗结束背包满补发
	SEND_MAIL_TYPE_KUA_FU_YING_XING_LU_BAG_FULL = 73,		// 跨服英雄录奖励,背包满补发
	SEND_MAIL_TYPE_KUA_FU_YING_XING_LU_RANK_END_TASK = 74,	// 跨服英雄录奖励,榜单结束,补发未领任务奖励
	SEND_MAIL_TYPE_NEW_COURAGE_CHALLENGE = 75,				// 弑神之塔背包满补发
	SEND_MAIL_TYPE_GUILD_INBORN_SKILL_RESET = 76,			// 家族灵脉技能重置返还

	SEND_MAIL_TYPE_MAX,
};

enum PUT_REASON_TYPE
{
	PUT_REASON_INVALID = 0,							//!< 0 无效

	PUT_REASON_NO_NOTICE,							//!< 1 不通知
	PUT_PET_CONVERSION_EXP,							//!< 2 宠物经验转化
	PUT_PET_REBIRTH,								//!< 3 宠物返生
	PUT_REASON_GM,									//!< 4 GM命令
	PUT_REASON_DEPOSIT,								//!< 5 存入仓库
	PUT_REASON_WITHDRAW_STORAGE,					//!< 6 取出仓库
	PUT_REASON_WITHDRAW_TEMPORARY,					//!< 7 取出临时背包
	PUT_REASON_WITHDRAW_ALL_TEMPORARY,				//!< 8 取出临时背包全部
	PUT_REASON_PRESTIGE_BUY,						//!< 9 声望商店
	PUT_REASON_COLLECTION_ONCONVERT,				//!< 10 藏品兑换
	PUT_REASON_GIFT_RAND_ITEM,						//!< 11 随机礼包(大)
	PUT_REASON_TASK_REWARD,							//!< 12 任务报酬
	PUT_REASON_ACHIEVEMENT_REWARD,					//!< 13 成就
	PUT_REASON_DAY_REWARD,							//!< 14 奖励界面
	PUT_REASON_WORKSHOP_COMPOUND,					//!< 15 工坊合成
	PUT_REASON_GUILD_EXPLORE,						//!< 16 家族探险
	PUT_REASON_GUILD_BOSS,							//!< 17 家族Boss
	PUT_REASON_MAIL_RECEIVE,						//!< 18 邮件
	PUT_REASON_SYSTEM_NOTICE_REWARD,				//!< 19 功能预告奖励
	PUT_REASON_SYSTEM_COURAGE_CHALLENGE,			//!< 20 勇气挑战
	PUT_REASON_RESERVE,								//!< 21 已废弃，用作保留项
	PUT_REASON_SYSTEM_MONSTER_WAVE,					//!< 22 魔窟
	PUT_REASON_GATHER_OBJECT,						//!< 23 采集
	PUT_REASON_GUILD_SHOP,							//!< 24 家族商店
	PUT_REASON_WILD_BOSS,							//!< 25 野外boss 
	PUT_REASON_MEDAL,								//!< 26 勋章
	PUT_REASON_MEDAL_PUT_ON,						//!< 27 勋章穿戴或脱下
	PUT_REASON_MI_YI,								//!< 28 密医
	PUT_REASON_SCHOOL_TASK,							//!< 29 门派任务
	PUT_REASON_DROP,								//!< 30 掉落
	PUT_REASON_ESCORT,								//!< 31 运镖
	PUT_REASON_TREASURE_MAP,						//!< 32 堆叠宝图/缉捕盗贼
	PUT_REASON_TRADE_MARKET_BUY,					//!< 33 交易所购买
	PUT_REASON_FIND_REWARD,							//!< 34 奖励找回
	PUT_REASON_TASK_CHAIN,							//!< 35 任务链
	PUT_REASON_FABAO_DECOMPOSE,						//!< 36 法宝分解
	PUT_REASON_CRYSTAL_FB,							//!< 37 水晶副本
	PUT_REASON_SHOP,								//!< 38 商店
	PUT_REASON_XUN_BAO,								//!< 39 寻宝
	PUT_REASON_EXCHANGE,							//!< 40 兑换
	PUT_REASON_BRAVE_GROUND,						//!< 41 勇闯地宫
	PUT_REASON_RA_TIME_LIMIT_STORE,					//!< 42 随机活动-限时商店
	PUT_REASON_MARRIAGE_TASK,						//!< 43 情缘日常任务
	PUT_REASON_GUILD_FIGHT,							//!< 44 家族大乱斗
	PUT_REASON_COLOSSEUM,							//!< 45 竞技场
	PUT_REASON_GUILD_ANSWER,						//!< 46 家族答题
	PUT_REASON_OPTIONAL_GIFT,						//!< 47 自选礼包
	PUT_REASON_CHALLENGE_FB,						//!< 48 挑战副本
	PUT_REASON_TRADE_MARKET_OTHER,					//!< 49 交易所（非购买）
	PUT_REASON_DEF_GIFT,							//!< 50 固定礼包
	PUT_REASON_RAND_GIFT,							//!< 51 随机礼包
	PUT_REASON_CHONGZHI_FANLI,						//!< 52 充值返利
	PUT_REASON_FUNC_GUIDE,							//!< 53 功能引导 / 新手事件
	PUT_REASON_GUIDE_FB,							//!< 54 指引副本
	PUT_REASON_MAZERACER,							//!< 55 迷宫竞速
	PUT_REASON_TREVIFOUNTION,						//!< 56 许愿池
	PUT_REASON_LOW_TREASURE_MAP,					//!< 57 低级宝图
	PUT_REASON_NPC_SHOP,							//!< 58 npc随机商店
	PUT_REASON_CONVERT_SHOP,						//!< 59 兑换商店
	PUT_REASON_GIFT_OPTIONAL_ITEM,					//!< 60 自选礼包(大)

	PUT_REASON_CATCH_PET,							//!< 61 捕获宠物
	PUT_REASON_PET_ABANDONMENT_SHARDS,				//!< 62 宠物放生获取的碎片

	PUT_REASON_DECOMPOSE_SURFACE,					//!< 63 分解时装 / 时装商店兑换重复转化 /伙伴皮肤/宠物皮肤
	PUT_REASON_RA_TURN_TABLE,						//!< 64 随机活动-开服转盘
	PUT_REASON_RA_DISCOUNT_STORE,					//!< 65 随机活动-折扣商店
	PUT_REASON_CATCH_PET_SHARD,						//!< 66 捕获宠物得到碎片奖励
	PUT_REASON_RA_MYSTERIOUS_TRIAL,					//!< 67 随机活动-神秘试炼
	PUT_REASON_GUILD_BOSS_CHAPTER_REWARD,			//!< 68 家族试炼章节奖励
	PUT_REASON_RA_CONSUME_CAROUSEL,					//!< 69 随机活动-消费转盘
	PUT_REASON_RA_FIRST_CHONGZHI_YIXIANER,			//!< 70 首充活动
	PUT_REASON_RA_LOGIN_SYSTEM_REWARD,				//!< 71 累计登陆活动
	PUT_REASON_BOSS_CHAPTER_BOSS_REWARD,			//!< 72 BOSS章节BOSS奖励
	PUT_REASON_BOSS_CHAPTER_CHAPTER_REWARD,			//!< 73 BOSS章节章节奖励
	PUT_REASON_BOUNTY_TASK_TURN_TABLE_REWARD,       //!< 74 赏金任务转盘奖励
	PUT_REASON_SERVER_FIRST_KILL,					//!< 75 全服首杀奖励
	PUT_REASON_TRANSACTION_TASK_REWARD,				//!< 76 购买任务 - 丝绸之路奖励(远洋贸易)
	PUT_REASON_TASKCHAIN_DROP_SCENE,				//!< 77 环任务暗雷掉落
	PUT_REASON_ACTIVE_SYSTEM_REWARD,				//!< 78 活跃系统奖励
	PUT_REASON_FANTASY_FB,							//!< 79	四象幻境副本
	PUT_REASON_WORLD_QUESTION_REWARD,				//!< 80 世界答题奖励
	PUT_REASON_RA_HERITAGE_TREASURE,				//!< 81 随机活动-传承瑰宝
	PUT_REASON_EXPEL_MONSTER_BUY_ITEM,				//!< 82 驱除妖兽活动购买物品
	PUT_REASON_ADVANCE_REWARD,						//!< 83 进阶副本
	PUT_REASON_SALE_SHOP_BUY,						//!< 84 特卖商店
	PUT_REASON_HAPPY_UP_EXCHANGE,					//!< 85 欣欣向荣
	PUT_REASON_RA_LUCKY_BAG,						//!< 86 随机活动-福袋
	PUT_REASON_RA_TIAN_JIANG_HAO_LI,				//!< 87 天降好礼
	PUT_REASON_EXPEL_MONSTER,						//!< 88 驱除妖兽
	PUT_REASON_MEDAL_EXCAHNG_MATERIAL,				//!< 89 勋章兑换材料
	PUT_REASON_JOY_SEEKS_FAVORS,					//!< 90 欢乐寻宠
	PUT_REASON_JIAN_DAN_QIN_XIN,					//!< 91 剑胆琴心
	PUT_REASON_ONLY_FIGHT,							//!< 92 以一敌百(个人版家族大乱斗)
	PUT_REASON_TEAM_FIGHT,							//!< 93 小队竞技
	PUT_REASON_LINKAGE_GIFT,						//!< 94 联动送礼
	PUT_REASON_RELATED_TO_THE_EVENT,				//!< 95 与活动相关(给予玩家活动道具)
	PUT_REASON_RA_PARTNER_EXCHANGE,					//!< 96 伙伴兑换
	PUT_REASON_RA_COLLECT_WORD_EXCHANGE,			//!< 97 集字活动兑换 / 每日礼包
	PUT_REASON_RA_DISCOUNT_CARNIVAL,				//!< 98 折扣狂欢
	PUT_REASON_RA_LUCKY_DISCOUNT,					//!< 99 幸运折扣
	PUT_REASON_RA_PET_EXCHANGE,						//!< 100 宠物兑换
	PUT_REASON_LUCKY_TURNTABLE,						//!< 101 幸运转盘
	PUT_REASON_RA_ADVENTURE_GIFT,					//!< 102 宠物兑换
	PUT_REASON_SURFACE_SHOP,						//!< 103 时装商店
	PUT_REASON_RA_QUAN_MIN_DISCOUNT,				//!< 104 全民团购
	PUT_REASON_RAND_GIFT_PET_EGG,					//!< 105 随机礼包宠物蛋
	PUT_REASON_OPTIONAL_GIFT_PET_EGG,				//!< 106 自选礼包宠物蛋
	PUT_REASON_DEF_GIFT_PET_EGG,					//!< 107 固定礼包宠物蛋
	PUT_REASON_RA_SPROTS_REWARD,					//!< 108 新服竞技
	PUT_REASON_PET_EQUIPMENT_TAKEOFF,				//!< 109 宠物装备脱下
	PUT_REASON_PARTNER_ELEMENT_TAKEOFF_DEFAULT,		//!< 110 元素水晶卸下(未损坏)
	PUT_REASON_DECOMPOSE_MEDAL,						//!< 111 分解勋章
	PUT_REASON_RA_TU_JIAN_TREASURE,					//!< 112 图鉴夺宝
	PUT_REASON_PARTNER_ELEMENT_TAKEOFF_BAD,			//!< 113 元素水晶卸下(损坏)
	PUT_REASON_RA_GOD_TOKEN,						//!< 114 封神战令
	PUT_REASON_FUNC_PREVIEW,						//!< 115 功能预告
	PUT_REASON_SEVEN_COLOR_PICTURE,					//!< 116 七彩图鉴
	PUT_REASON_RA_ELEMENT_CARDS,					//!< 117 元素翻牌
	PUT_REASON_SHI_TU_FB,							//!< 118 师徒副本
	PUT_REASON_RA_SIGN_IN_CONTINUITY,				//!< 119 连登好礼
	PUT_REASON_WORKSHOP_BATCH_COMPOUND,				//!< 120 工坊批量打造
	PUT_REASON_GUILD_GATHER_DREAM,					//!< 121 家族集愿屋
	PUT_REASON_MYSTERY_STORE,						//!< 122 神秘商店（命格商店）
	PUT_REASON_COMPUOUND_WORK_SHOP,					//!< 123 工坊兑换、合成
	PUT_REASON_CREATE_ROLE,							//!< 124 创角新手礼包
	PUT_REASON_SHAN_HAI_BOSS,						//!< 125 山海降妖
	PUT_REASON_RA_CONSUME_CAROUSEL_ALL,				//!< 126 随机活动-消费转盘转动全部
	PUT_REASON_DING_ZHI_BAO_XIANG,					//!< 127 定制宝箱
	PUT_REASON_ZHUAN_SHU_LI_BAO,					//!< 128 专属礼包
	PUT_REASON_LIU_LI_DING,							//!< 129 琉璃鼎
	PUT_REASON_XIAN_SHI_BAO_HE,						//!< 130 限时宝盒
	PUT_REASON_XIAN_SHI_BAO_DAI,					//!< 131 限时宝袋
	PUT_REASON_CRYSTAL_FB_FAST_PASS,				//!< 132 水晶副本扫荡
	PUT_REASON_ANECDOTE_TASK,						//!< 133 奇闻任务
	PUT_REASON_PARTNER_BACK_REWARD,					//!< 134 伙伴解锁返还
	PUT_REASON_RA_GIVE_PET_DRAW,					//!< 135 赠30连抽（送寻宠连抽）
	PUT_REASON_DRAW_MANAGER,						//!< 136 翻牌管理器
	PUT_REASON_RA_BONUS,							//!< 137 运营-额外奖励
	PUT_REASON_RA_CI_JIU_YING_XIN,					//!< 138 辞旧迎新奖励
	PUT_REASON_RA_NIU_XING_DA_YUN,					//!< 139 牛行大运奖励
	PUT_REASON_RA_SPRING_FESTIVAL_STORE,			//!< 140 年货节奖励
	PUT_REASON_RA_COW_EXCHANGE_STORE,				//!< 141 牛运兑换奖励
	PUT_REASON_RA_MONPOLY_GIFT,						//!< 142 牛运当头奖励
	PUT_REASON_RA_COW_LUCKY_EXCHANGE,				//!< 143 牛运无边奖励
	PUT_REASON_RA_LITTLE_YEAR_GIFT,					//!< 144 小年送礼
	PUT_REASON_RA_COW_STORE,						//!< 145 牛运商城
	PUT_REASON_BUG_REPAIR,							//!< 146 BUG修复
	PUT_REASON_BRAVE_GROUND_SAO_DANG,				//!< 147 勇闯地宫扫荡
	PUT_REASON_GIVE_SECRET_KEY,						//!< 148 赠送寻宝次数
	PUT_REASON_PET_LOOT,							//!< 149 宠物夺宝
	PUT_REASON_DREAM_NOTES_CHALLENGE,				//!< 150 梦渊笔录
	PUT_REASON_STAR_TREASURE_MAP,					//!< 151 星辰藏宝图
	PUT_REASON_HIGH_TREASURE_MAP,					//!< 152 高级藏宝图
	PUT_REASON_SHEN_SHOU_JIANG_LIN,					//!< 153 神兽降临
	PUT_REASON_SCHOOL_EXERCISE,						//!< 154 校场演练
	PUT_REASON_CHIEF_ELECTION,						//!< 155 首席竞选
	PUT_REASON_NOREXITEM_EQUIP,						//!< 156 未鉴定装备
	PUT_REASON_PET_BREAKTHROUGH,					//!< 157 宠物强化吃掉宠物
	PUT_REASON_GOOD_GIFT_TURN,						//!< 158 好礼转动
	PUT_REASON_HELP_OTHERS,							//!< 159 义胆助人
	PUT_REASON_SECRET_TREASURE,						//!< 160 特惠瑰宝
	PUT_REASON_CHUN_RI_DENG_FENG,					//!< 161 春日登峰
	PUT_REASON_MI_JING_QI_WEN_FB,					//!< 162 秘境奇闻副本
	PUT_REASON_ZU_HE_HE_LI,							//!< 163 组合贺礼
	PUT_REASON_LING_PAI_TIAN_CI,					//!< 164 令牌天赐
	PUT_REASON_FASHION_SHOP,						//!< 165 时装商店
	PUT_REASON_PARTNER_BIOGRAPHY,					//!< 166 伙伴传记
	PUT_REASON_NOREXITEM_ITEM,						//!< 167 主动消耗道具
	PUT_REASON_ACTIVE_SYSTEM_EXTRA_REWARD,			//!< 168 活跃系统额外奖励
	PUT_REASON_SMART_MOUNTS_SYSTEM,					//!< 169 灵骑系统
	PUT_REASON_REALM_BREAK,							//!< 170 境界系统突破
	PUT_REASON_PET_MONOPOLY,						//!< 171 灵宠秘境
	PUT_REASON_LUN_HUI_WANG_CHUAN,					//!< 172 轮回忘川
	PUT_REASON_MATERIAL,							//!< 173 玉虚境(材料副本)
	PUT_REASON_MATERIAL_ACHIEVEMENT_REWARD,			//!< 174 玉虚境成就奖励(材料副本)
	PUT_REASON_CUMULATIVE_RECHARGE,					//!< 175 累计充值(神装惊世)
	PUT_REASON_FA_BAO_DRAW_REWARD,					//!< 176 法宝抽奖
	PUT_REASON_CLOUD_ARENA_RANK_REWARD,				//!< 177 云端竞技段位奖励
	PUT_REASON_PLATFORM_ACHIEVEMENT_REWARD,			//!< 178 擂台战成就奖励
	PUT_REASON_CLOUD_ARENA_ACHIEVE_REWARD,			//!< 179 云端竞技成就奖励
	PUT_REASON_RELIC_LOOTING_REWARD,				//!< 180 遗迹夺宝成就奖励
	PUT_REASON_CRYSTAL_FB_RETURN_TICKET,			//!< 181 普通秘境返还门票
	PUT_REASON_MAP_EXPLORATION_REWARD,				//!< 182 地图探索奖励
	PUT_REASON_MARKET,								//!< 183 市场
	PUT_REASON_CHIVALROUS_ACHIEVE,					//!< 184 侠义值成就
	PUT_REASON_NOREX_CONVERT_ITEM,					//!< 185 主动道具使用
	PUT_REASON_TRANSFER_KNAPSACK_COLUMN,			//!< 186 转移背包物品到正确的栏
	PUT_REASON_MI_YI_FETCH_WEEK_KILL_REWARD,		//!< 187 密医领取周首杀奖励
	PUT_REASON_RESEARCH_TASK_REWARD,				//!< 188 三界除魔任务奖励
	PUT_REASON_ROLE_JOIN_ACTIVITY_REWARD,			//!< 189 角色参与限时活动奖励
	PUT_REASON_SHEN_SHOU_JIANG_LIN_BUY,				//!< 190 神兽降临直购
	PUT_REASON_GUILD_ACTIVE_DAY_GIFT,				//!< 191 家族日活跃度礼包
	PUT_REASON_ACHIEVEMENT_PROG_REWARD,				//!< 192 成就阶段奖励
	PUT_REASON_PET_SOUL_EQUIPMENT_TAKEOFF,			//!< 193 宠物魂器脱下
	PUT_REASON_TOP_LEVEL,							//!< 194 巅峰等级
	PUT_REASON_GOD_PRINT_WISH_REWARD,				//!< 195 神印祈神
	PUT_REASON_GOD_PRINT_EXCHANGE_REWARD,			//!< 196 神印兑换
	PUT_REASON_NETHER_WORLD_PURGATORY_REWARD,		//!< 197 幽冥炼狱
	PUT_REASON_ZHU_LING_DECOMPOSE,					//!< 198 铸灵分解
	PUT_REASON_PET_GOD_FB,							//!< 199 神印之地
	PUT_REASON_HEART_SKILL_COMPOSE,					//!< 200 心法合成
	PUT_REASON_HEART_SKILL_CONVERT,					//!< 201 心法转换
	PUT_REASON_ZHU_LING_UP_LEVEL,					//!< 202 铸灵升级
	PUT_REASON_ZHU_LING_PUT_ON,						//!< 203 铸灵穿戴替换下来
	PUT_REASON_ZHU_LING_TAKE_OFF,					//!< 204 铸灵脱下
	PUT_REASON_FIRST_RECHARGE_EXCLUSIVE,			//!< 205 首充专属
	PUT_REASON_MONTH_CARD_REWARD,					//!< 206 周卡/月卡
	PUT_REASON_BIG_DIPPER_REWARD,					//!< 207 北斗七星
	PUT_REASON_ONE_KEY_GRADE_MEDAL,					//!< 208 一键升华令牌
	PUT_REASON_TIAN_DI_DAO_HEN_ACTIVE,				//!< 209 天地道痕奖励
	PUT_REASON_TIAN_DI_DAO_HEN_FINAL,				//!< 210 天地道痕最终奖励
	PUT_REASON_TIAN_DI_DAO_HEN_DROP,				//!< 211 天地道痕最终奖励
	PUT_REASON_FETCH_GIVE_GIFT,						//!< 212 领取赠送礼物
	PUT_REASON_FETCH_RESOURCE_DOWNLOAD_REWARD,		//!< 213 获取资源下载奖励
	PUT_REASON_EXCHANGE_PET_SOUL_EQUIP,				//!< 214 魂器兑换
	PUT_REASON_PRESTIGE_TASK,						//!< 215 声望任务
	PUT_REASON_HUNDRED_GHOST,						//!< 216 百鬼夜行
	PUT_REASON_SUMMER_LI_LIAN,						//!< 217 初夏历练
	PUT_REASON_COLORFUL_LANTERN,					//!< 218 多彩花灯
	PUT_REASON_SUMMER_STORE,						//!< 219 夏日商店
	PUT_REASON_LUCKY_SIGN,							//!< 220 好运花签
	PUT_REASON_QIONG_LOU_YU_YAN,					//!< 221 琼楼玉宴
	PUT_REASON_SUPER_OFF_SHOP,						//!< 222 超级折扣商城
	PUT_REASON_YUAN_QI_TURNING,						//!< 223 元气翻牌
	PUT_REASON_COOL_BREEZE,							//!< 224 凉风送爽
	PUT_REASON_LOTUS_MI_BAO,						//!< 225 荷香觅宝
	PUT_REASON_MAZE_SUMMER,							//!< 226 夏日迷宫
	PUT_REASON_MI_BAO_FANG_DIAN,					//!< 227 觅宝坊店
	PUT_REASON_ZHEN_PIN_SHANG_PU,					//!< 228 珍品商铺
	PUT_REASON_FANG_FRUAD_REWARD,					//!< 229 防诈骗奖励
	PUT_REASON_PET_SOUL_EQUIP_CHANGE,				//!< 230 魂器变更
	PUT_REASON_ATTR_PELLET_RET,						//!< 231 属性丹返还
	PUT_REASON_ACTIVE_TURNTABLE,					//!< 232 活跃转盘
	PUT_REASON_PARTNER_PACT_GET,					//!< 233 伙伴--元神契约--获得
	PUT_REASON_PARTNER_PACT_PUT_ON,					//!< 234 伙伴--元神契约--装备
	PUT_REASON_PARTNER_PACT_UP_STAR,				//!< 235 伙伴--元神契约--升星
	PUT_REASON_RA_BACK_REWARD,						//!< 236 版本活动-回归豪礼
	PUT_REASON_DUEL,								//!< 237 决斗
	PUT_REASON_RA_HAPPY_LOOK,						//!< 238 连连乐翻天
	PUT_REASON_SKILL_BOOK_CONVERT,					//!< 239 超级技能书兑换 
	PUT_REASON_SERVER_COMPETITION,					//!< 240 全服比拼
	PUT_REASON_DU_JIE,								//!< 241 渡劫
	PUT_REASON_YIN_YANG_EYE,						//!< 242 阴阳眼
	PUT_REASON_REALM_BREAK_FIGHT,					//!< 243 境界系统战斗（心灵之境）
	PUT_REASON_CAPACITY_TARGET,						//!< 244 系统邀战
	PUT_REASON_HELP_OTHERS_2,						//!< 245 义胆助人2 (桃园一三事)
	PUT_REASON_ANECDOTE_BIG_EVENT,					//!< 246 奇闻大事件
	PUT_REASON_YUN_ZE_TERRITORY,					//!< 247 云泽画境
	PUT_REASON_RA_SIGN_HAPPY,						//!< 248 登录送好礼
	PUT_REASON_RA_WANLING_GONGYING,					//!< 249 万灵共盈
	PUT_REASON_RA_HORCRUX_TIANCHENG,				//!< 250 魂器天成
	PUT_REASON_RA_HORCRUX_TIANCHENG_DRAW,			//!< 251 魂器天成-抽奖
	PUT_REASON_RA_INTEGRAL_DRAW,					//!< 252 整点抽奖
	PUT_REASON_SHAN_HAI_QUESTION_REWARD,			//!< 253 山海问答奖励
	PUT_REASON_SHAN_HAI_JU_BAO_DRAW,				//!< 254 山海聚宝 抽奖
	PUT_REASON_SHAN_HAI_JU_BAO_EXTRA_REWARD,		//!< 255 山海聚宝 额外奖励
	PUT_REASON_SHAN_HAI_MET_REWARD,					//!< 256 山海初遇 额外奖励
	PUT_REASON_RA_SHAN_HAI_MEET,					//!< 257 山海相逢
	PUT_REASON_RA_FATE_ENTANGLEMENT,				//!< 258 山海缘起
	PUT_REASON_ENDLESS_TOWER_REWARD,				//!< 259 无尽之塔
	PUT_REASON_HUAN_JIE_TODAY_WIN_REWARD,			//!< 260 幻界战场胜场宝箱奖励
	PUT_REASON_RA_ZHAOCAIJINBAO_DRAW,				//!< 261 招财进宝 抽奖
	PUT_REASON_RA_ZHAOCAIJINBAO,					//!< 262 招财进宝
	PUT_REASON_RA_ZAI_ZHAN_YUN_ZE,					//!< 263 再战云泽
	PUT_REASON_YUN_ZE_SLOT_MACHINE,					//!< 264 纷享同花礼
	PUT_REASON_MO_ZU_ZAI_NA_LI,						//!< 265 魔族在哪里
	PUT_REASON_YUN_ZE_SLOT_MACHINE_BUY_KEY,			//!< 266 纷享同花礼购买钥匙
	PUT_REASON_CHAOZHI_FENXIANG,					//!< 267 超值纷享
	PUT_REASON_RA_COLORFUL_DOUBLE,					//!< 268 纷享加倍
	PUT_REASON_SHAN_HAI_ZHENG_FENG_REWARD,			//!< 269 山海争锋
	PUT_REASON_SHAN_HAI_MI_BAO_REWARD,				//!< 270 山海觅宝
	PUT_REASON_SHAN_HAI_BAO_XIANG,					//!< 271 山海宝箱
	PUT_REASON_SHAN_HAI_BAO_HE,						//!< 272 山海宝盒
	PUT_REASON_SHAN_HAI_BAO_DAI,					//!< 273 山海宝袋
	PUT_REASON_RA_YUN_ZE_ER_SAN_SHI,				//!< 274 云泽二三事转盘奖励下发
	PUT_REASON_SHANHAI_HUADENG,						//!< 275 山海花灯
	PUT_REASON_WAN_LING_FB,							//!< 276 万灵话本
	PUT_REASON_YUNZE_BLOOMING_RECORD,				//!< 277 云泽繁花录
	PUT_REASON_RA_JIXING_GAOZHAO,					//!< 278 吉星高照
	PUT_REASON_RA_JIXING_GAOZHAO_DRAW,				//!< 279 吉星高照-抽奖
	PUT_REASON_RA_YUNZE_ER_SAN_SHI_REWARD,			//!< 280 云泽二三事
	PUT_REASON_BOUNTY_FINISH_REWARD,				//!< 281 赏金任务完成奖励
	PUT_REASON_DIAN_HUA_SHI,						//!< 282 吸附石获取
	PUT_REASON_FISHING,								//!< 283 钓鱼
	PUT_REASON_NEW_LIFE_MAKE,						//!< 284 生活职业制作
	PUT_REASON_MIRAGE_TERRITORY,					//!< 285 琉璃蜃境
	PUT_REASON_RA_YUNZE_CATCH_ELVES,				//!< 286 精灵大抓捕
	PUT_REASON_RA_YUNZE_HAO_LI,						//!< 287 云泽好礼
	PUT_REASON_RA_YUNZE_HAO_LI_BUY_GOLD_HAMMER,		//!< 288 云泽好礼购买金锤
	PUT_REASON_RA_XIAN_SHI_LEI_CHONG,				//!< 289 限时累充
	PUT_REASON_RA_YUNZE_WISH,						//!< 290 云泽祈愿
	PUT_REASON_RA_WEEKEND_GIFT,						//!< 291 周末礼包
	PUT_REASON_RA_YUNZE_CUTE_PET,					//!< 292 萌宠驾到
	PUT_REASON_RA_CHAO_ZHI_GIFT,					//!< 293 超值献礼
	PUT_REASON_RA_FLY_UP_REWARD,					//!< 294 飞升任务奖励
	PUT_REASON_TREASURE_TASK,						//!< 295 九州秘宝
	PUT_REASON_RA_TIGER_AUSPICIOUS_SNOW,			//!< 296 瑞雪丰年
	PUT_REASON_RA_TIGER_FISH_EVERY_YEAR,			//!< 297 年年有鱼
	PUT_REASON_RA_YINHU_WENSHI,						//!< 298 寅虎问世
	PUT_REASON_RA_TIGER_FIND_SPRING,				//!< 299 寅虎探春
	PUT_REASON_RA_TIGER_TEASE_NEW_YEAR,				//!< 300 寅虎逗新春
	PUT_REASON_RA_CINIU_YINGXINHU,					//!< 301 辞牛迎新虎
	PUT_REASON_RA_TIGER_LUCKY_MONEY,				//!< 302 寅虎压岁
	PUT_REASON_RA_HU_XING_DA_YUN,					//!< 303 虎行大运
	PUT_REASON_RA_YIN_HU_BAI_SUI,					//!< 304 寅虎拜岁
	PUT_REASON_RA_AUSPICIOUS_SNOW_PICK_UP,			//!< 305 瑞雪丰年雪堆拾取
	PUT_REASON_RA_JINGJI_YUEKA_REWARD,				//!< 306 竞技月卡奖励
	PUT_REASON_WEDDING,								//!< 307 婚宴
	PUT_REASON_RONG_LIAN_YOU_LI,					//!< 308 熔炼有礼
	PUT_REASON_FALLEN_GOD_REWARD,					//!< 309 殒神之地奖励
	PUT_REASON_PET_FEI_SHENG_REWARDS,				//!< 310 宠物异闻录
	PUT_REASON_RA_QINGYUAN_SHIZHUANG,				//!< 311 情缘时装
	PUT_REASON_PET_BATCH_EXCHANGE,					//!< 312 宠物碎片批量合成分解得到物品
	PUT_REASON_MINI_GAME_STAGE_REWARD,				//!< 313 小游戏关卡奖励
	PUT_REASON_MINI_GAME_PHASE_REWARD,				//!< 314 小游戏阶段奖励
	PUT_REASON_QING_YUAN_GIFT_DAY_REWARD,			//!< 315 情缘礼物 领取每日奖励
	PUT_REASON_RA_QINGYUAN_LEICHONG,				//!< 316 情缘累充
	PUT_REASON_FALLEN_GOD_HELP_PASS_REWARD,			//!< 317 殒神之地助战奖励
	PUT_REASON_WORLD_ARENA_WIN_BOX,					//!< 318 论剑九州胜场宝箱
	PUT_REASON_GOD_PRINT_EXCHANGE_2,				//!< 319 神印兑换2
	PUT_REASON_QING_YUAN_MARRAY_REWARD,				//!< 320 情缘结婚奖励
	PUT_REASON_WORLD_ARENA_SIGNUP_REWARD,			//!< 321 论剑报名奖励
	PUT_REASON_WORLD_ARENA_DAILY_REWARD,			//!< 322 论剑每日奖励
	PUT_REASON_PET_BADGE_TAKE_OFF,					//!< 323 宠物徽章脱下
	PUT_REASON_PET_BADGE_COMPOSE,					//!< 324 宠物徽章合成
	PUT_REASON_PET_BADGE_DECOMPOSE,					//!< 325 宠物徽章分解
	PUT_REASON_WORLD_TEAM_ARENA,					//!< 326 英雄会武
	PUT_REASON_PEAK_TOKEN,							//!< 327 天下至尊令
	PUT_REASON_PET_STRENGTHEN_REWARD,				//!< 328 宠物强化奖励
	PUT_REASON_PARTNER_INTENSIFY,					//!< 329 伙伴强化
	PUT_REASON_QING_YUAN_SHENG_HUI,					//!< 330 情缘盛会
	PUT_REASON_YAO_XING_SHI_LIAN,					//!< 331 妖行试炼
	PUT_REASON_SHI_YU_MILE,							//!< 332 师徒里程
	PUT_REASON_SHI_YU_CHUAN_GONG,					//!< 333 师徒传功
	PUT_REASON_ACCESSORIES_BACK_STRENGTHEN,			//!< 334 配饰分解强化道具退还
	PUT_REASON_SAN_XIAN_GUI_DONG_REWARD,			//!< 335 三仙归洞奖励
	PUT_REASON_LANTERN_RIDDIE,						//!< 336 山海灯会最终奖励
	PUT_REASON_WHERE_IS_THE_FAIRY,					//!< 337 仙子在哪儿
	PUT_REASON_TIAN_GONG_XUN_BAO,					//!< 338 天宫寻宝
	PUT_REASON_QINGYUAN_DUIDUIPENG,					//!< 339 情缘对对碰 副本奖励
	PUT_REASON_LOVERS_MARKET,						//!< 340 情缘小铺
	PUT_REASON_ANSWER_LANTERN_RIDDIE,				//!< 341 山海灯会答题奖励
	PUT_REASON_CONCENTRIC_KNOT,						//!< 342 缘牵同心结
	PUT_REASON_TAO_QUAN_NA_JI,						//!< 343 套圈纳吉
	PUT_REASON_TU_ZI_SAI_PAO,						//!< 344 兔子赛跑
	PUT_REASON_GUESS_BET_REWARD,					//!< 345 竞猜奖励
	PUT_REASON_GUESS_BET_FAILD_RETURN,				//!< 346 竞猜失败返还
	PUT_REASON_UTA_WIN_BOX,							//!< 347 天下第一 胜场宝箱
	PUT_REASON_UTA_TODAY_JOIN_REWARD,				//!< 348 天下第一 每日参与奖励
	PUT_REASON_UTA_ACTIVE_REWARD,					//!< 349 天下第一 活跃值奖励
	PUT_REASON_ROLE_HUNDRED_GOD_BOSS,				//!< 350 百神之师
	PUT_REASON_XUN_BAO_GUIDE_PARTNER,				//!< 351 寻宝伙伴指引
	PUT_REASON_XUN_BAO_GUIDE_PARTNER_2,				//!< 352 寻宝伙伴指引2
	PUT_REASON_RA_QIYUAN_KONGMINGDENG,				//!< 353 祈愿孔明灯
	PUT_REASON_RA_YUAN_QI_SPEED,					//!< 354 元气竞速
	PUT_REASON_RA_YUAN_QI_HELP,						//!< 355 元气助人
	PUT_REASON_RA_XUNMENG_HUDIE,					//!< 356 寻梦蝴蝶
	PUT_REASON_RA_GOD_BEAST_ADVENT,					//!< 357 异兽临世
	PUT_REASON_RA_BO_ZHONG_YOU_LI,					//!< 358 播种有礼
	PUT_REASON_RA_YUAN_QI_FAN_PAI,					//!< 359 元气翻牌
	PUT_REASON_RA_YUAN_QI_FAN_PAI_REWARD,			//!< 360 元气翻牌翻牌奖励
	PUT_REASON_RA_WANLING_ZHULI,					//!< 361 万灵筑礼
	PUT_REASON_RA_BAO_ZI_LAI_LE,					//!< 362 包子来了
	PUT_REASON_RA_JIN_YI_TIAN_XIANG,				//!< 363 锦衣天香
	PUT_REASON_FETCH_PINJIA_REWARD,					//!< 364 获取评价有礼奖励
	PUT_REASON_RA_MIAO_BI_SHENG_HUA,				//!< 365 妙笔生花
	PUT_REASON_RA_YUANQI_JISHIBU,					//!< 366 元气记事簿
	PUT_REASON_RA_YUAN_QI_YAN_HUO,					//!< 367 元气烟火
	PUT_REASON_RA_YUAN_QI_YAN_HUO_OPEN_ITEM,		//!< 368 元气烟火开启烟火物品
	PUT_REASON_RA_YUAN_QI_FAN_PAI_DAY_FREE_REWARD,	//!< 369 元气翻牌每日奖励
	PUT_REASON_RA_CHENG_FENG_PO_LANG,				//!< 370 乘风破浪
	PUT_REASON_YUAN_HUN_ZHU_UP_LEVEL,				//!< 371 元魂珠升级
	PUT_REASON_YUAN_HUN_ZHU_PUT_ON,					//!< 372 元魂珠穿戴替换下来
	PUT_REASON_YUAN_HUN_ZHU_TAKE_OFF,				//!< 373 元魂珠脱下
	PUT_REASON_ZAO_HUA_DISCOMPOSE,					//!< 374 造化装备分解
	PUT_REASON_HONGMENG_TIANJIE,					//!< 375 鸿蒙天界掉落奖励
	PUT_REASON_HONGMENG_TIANJIE_BUY,				//!< 376 鸿蒙天界购买道具
	PUT_REASON_YUAN_HUN_ZHU_DECOMPOSE,				//!< 377 元魂珠分解
	PUT_REASON_YUAN_HUN_ZHU_COMPOSE,				//!< 378 元魂珠合成
	PUT_REASON_ZAO_HUA_COMPOSE,						//!< 379 造化装备合成
	PUT_REASON_RA_ZONG_XIANG_REWARD,				//!< 380 粽香满城
	PUT_REASON_RA_SALTY_SWEET_BATTLE,				//!< 381 咸甜之争
	PUT_REASON_RA_DUAN_YANG_XIANG_NANG,				//!< 382 端阳香囊
	PUT_REASON_RA_DUAN_YANG_ZI_XUAN,				//!< 383 端阳自选
	PUT_REASON_RA_CANG_LONG_CI_FU,					//!< 384 苍龙赐福
	PUT_REASON_RA_LUCKY_BLIND_BOX_REWARD,			//!< 385 幸运盲盒
	PUT_REASON_RA_LAN_TANG_CHU_MU,					//!< 386 兰汤初沐
	PUT_REASON_RA_SANBAI_CHOU,						//!< 387 三百抽皮肤
	PUT_REASON_RA_FU_LI,							//!< 388 端阳福礼
	PUT_REASON_RA_LUCKY_BLIND_EXTRA_REWARD,			//!< 389 幸运盲盒额外奖励
	PUT_REASON_RA_JIN_LONG_JU_BAO_DRAW,				//!< 390 金龙聚宝抽奖
	PUT_REASON_RA_JIN_LONG_JU_BAO_FETCH,			//!< 391 金龙聚宝领取
	PUT_REASON_RA_JIN_LONG_JU_BAO_BUY,				//!< 392 金龙聚宝购买
	PUT_REASON_SPECIAL_ACTIVE_REWARD,				//!< 393 特殊活跃奖励
	PUT_REASON_RA_FORTUNE_MAZE,						//!< 394 幸运迷宫	
	PUT_REASON_KE_JU_EXAM_ANSWER_REWARD,			//!< 395 科举考试答题奖励
	PUT_REASON_KE_JU_EXAM_LAST_REWARD,				//!< 396 科举考试最终奖励
	PUT_REASON_GOTO_SCHOOL,							//!< 397 我要上学奖励
	PUT_REASON_ZHOU_QI_SHOP,						//!< 398 周期商店
	PUT_REASON_CHONG_JI_FAN_LI,						//!< 399 宠技返利
	PUT_REASON_RA_SHEN_SHOU_EN_ZE,					//!< 400 神兽恩泽
	PUT_REASON_RA_SHEN_SHOU_EN_ZE_FETCH,			//!< 401 神兽恩泽领取自选奖励
	PUT_REASON_RA_SHEN_SHOU_EN_ZE_BUY,				//!< 402 神兽恩泽购买
	PUT_REASON_RA_XIN_FU_QIANG_HUA,					//!< 403 新服强化
	PUT_REASON_GOD_EQUIP_WORK_SHOP,					//!< 404 工坊神装产出
	PUT_REASON_RA_QI_XI_TE_HUI,						//!< 405 七夕特惠
	PUT_REASON_CHARISMAITC_TANABATA_REWARD,			//!< 406 魅力七夕奖励
	PUT_REASON_XIAN_SHI_BAO_HE_3,					//!< 407 限时宝盒3
	PUT_REASON_XIAN_SHI_BAO_HE_4,					//!< 408 限时宝盒4
	PUT_REASON_RA_PET_EXCHANGE_BUY_ITEM,			//!< 409 宠物兑换-商店购买
	PUT_REASON_HALLOWS_SKILL_CONVERT,				//!< 410 圣器技能书兑换
	PUT_REASON_SHENG_QI_TREASURE_MAP,				//!< 411 圣器藏宝图
	PUT_REASON_TAKE_OFF_FU_SHI,						//!< 412 脱下符石
	PUT_REASON_RA_SHEN_CI_HUA_FU_DRAW,				//!< 413 神赐华福-抽奖
	PUT_REASON_FU_YU_TAKE_OFF,						//!< 414 符玉卸下
	PUT_REASON_FU_YU_UPGRADE_SUCC,					//!< 415 符玉强化成功
	PUT_REASON_FU_YU_UPGRADE_FAIL,					//!< 416 符玉强化失败
	PUT_REASON_FU_YU_USE_GIFT,						//!< 417 符玉礼包使用
	PUT_REASON_ELEMENT_ELF_REWARD,					//!< 418 元素精灵奖励（彩蛋怪物奖励）
	PUT_REASON_COLOSSEUM_BATTLE_REWARD,				//!< 419 竞技场战斗结束
	PUT_REASON_XIN_FU_LING_CHONG,					//!< 420 新服灵宠
	PUT_REASON_RA_HUN_QI_FAN_LI,					//!< 421 魂器返利
	PUT_REASON_RA_SHEN_CI_HUA_FU_BUY,				//!< 422 神赐华福-商店购买
	PUT_REASON_RA_TIAN_YI_YAO_SHI,					//!< 423 天衣耀世
	PUT_REASON_KUA_FU_YING_XIONG_LU,				//!< 424 跨服英雄录
	PUT_REASON_SMART_MOUNTS_CONVERT_ITEM,			//!< 425 灵骑物品转换
	PUT_REASON_ONE_CLICK_FINISH,					//!< 426 一键完成活跃任务
	PUT_REASON_SMART_MOUNTS_ACTIVE_CONVERT,			//!< 427 激活灵骑道具转化
	PUT_REASON_DAY_REWARD_ONLINE_GIFT,				//!< 428 奖励界面-在线礼包
	PUT_REASON_STAR_CHART_RESET,					//!< 429 星图重置
	PUT_REASON_NEW_COURAGE_CHALLENGE,				//!< 430 弑神之塔
	PUT_REASON_FASHION_COLLECTION_DECOMPOSE,		//!< 431 外观升级系统道具分解
	PUT_REASON_SMART_MOUNTS_SKILL_COMPOSE,			//!< 432 灵技合成
	PUT_REASON_STAR_CHART_LEVEL_UP,					//!< 433 星图升级
	PUT_REASON_YUE_XI_LEI_CHONG,					//!< 434 月夕累充
	PUT_REASON_YUE_XI_DENG_MI,						//!< 435 月夕灯谜
	PUT_REASON_RA_ZHUI_YUE_SHANG_DIAN_BUY,			//!< 436 追月商店购买
	PUT_REASON_RA_ZHUI_YUE_SHANG_DIAN_DRAW,			//!< 437 追月商店抽奖
	PUT_REASON_FU_YU_SUBLIME,						//!< 438 符玉升华
	PUT_REASON_FU_YU_SWITCH,						//!< 439 符玉转化
	PUT_REASON_SPECIAL_ANECDOTE,					//!< 440 特殊奇闻任务
	PUT_REASON_TEAM_CHALLENGE,						//!< 441 组队挑战任务
	PUT_REASON_HUAN_SHOU_ACTIVE_CONVERT,			//!< 442 幻兽激活转化
	PUT_REASON_HUAN_SHOU_LIAN_DAN,					//!< 443 幻兽系统炼丹
	PUT_REASON_HUAN_SHOU_DECOMPOSE,					//!< 444 幻兽系统分解
	PUT_REASON_GUILD_TASK_GIVE,						//!< 445 家族任务捐赠
	PUT_REASON_GUILD_TASK_FINISH,					//!< 446 家族任务完成
	PUT_REASON_GUILD_TASK_FETCH,					//!< 447 家族任务领取征集
	PUT_REASON_PET_ABANDONMENT_GAI_MING,			//!< 448 改命相关道具返还
	PUT_REASON_GUILD_INBORN_RESET_RETURN,			//!< 449 家族灵脉重置技能返还
	PUT_REASON_REAL_NAME_REWARD,					//!< 450 实名认证奖励

	PUT_REASON_MAX,									// 最大原因
};

enum KNAPSACK_CONSUME_TYPE							// 消耗物品的类型
{
	KNAPSACK_CONSUME_TYPE_CANNOT_BUY = 0,			// 不允许自动购买
	KNAPSACK_CONSUME_TYPE_GOLD_BUY,					// 允许非绑元宝自动购买

	KNAPSACK_CONSUME_TYPE_MAX
};

struct SortStruct
{
	void Set(long long _prior_id, bool _is_bind, int _src_index)
	{
		prior_id = _prior_id;
		is_bind = _is_bind;
		src_index = _src_index;
	}

	static long long GetSupplyPriorId(short _item_type, short _item_sub_type, short _item_color, ItemID _item_id)
	{
		// 补给品 按item_id排先后

		return _item_id;
	}

	static long long GetPriorId(short _item_level, short _item_type, short _item_sub_type, short _item_color, short _equip_type, ItemID _item_id, short _param);
	static long long GetPriorId_1(long long base_prior, short _item_level, short _item_type, short _item_sub_type, short _item_color, short _equip_type, ItemID _item_id, short _param);
	static long long GetPriorId_2(long long base_prior, short _item_level, short _item_type, short _item_sub_type, short _item_color, short _equip_type, ItemID _item_id, short _param);

	bool is_bind;
	int src_index;

	long long prior_id;
};
int SortCompare(const void *arg1, const void *arg2);

class Knapsack
{
public:
	enum ITEM_CHANGE_NOTICE_REASON
	{
		ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE = 0,	//!< 0 数据改变
		ITEM_CHANGE_NOTICE_REASON_PUT,				//!< 1 从外部获得
		ITEM_CHANGE_NOTICE_REASON_CONSUME,			//!< 2 从内部消耗
		ITEM_CHANGE_NOTICE_REASON_MOVE,				//!< 3 从内部转移
	};

public:
	Knapsack();
	~Knapsack();

	void* operator new(size_t c);
	void  operator delete(void* m);

	void Init(RoleModuleManager* module_mgr, int param_num, const ItemListParam::ItemListData *param_list[], const KnapsackOtherParam& other_param);
	void GetOtherInitParam(KnapsackOtherParam* other_param);
	void Update(time_t now_second);

	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnLevelUp(int new_level);
	void OnLogin();

	void EquipGemMounting(int stone_type, int equip_grid, int stone_grid);	// 宝石镶嵌
	void EquipGemDismount(int stone_type, int equip_grid, int stone_slot);	// 宝石拆卸
	void EquipGemUpgrades(int stone_type, int equip_grid, int stone_slot);	// 宝石升级
	void ConfirmGemUpgrade();												// 确认升级宝石

	//装备升星
	void EquipUpStar(int seq, int equip_type, int equip_grid, int cost_grid, int cost_grid_2);
	void CalcGemOneKeyUpgradeNeed(std::map<ItemID, int> & owned_list);
	void SendGemOneKeyUpgradeNeed();

	//特殊星耀，神装功能
	void UpEquipGrade(int equip_type, int equip_grid, int role_type = ROLE_TYPE_BENZUN);
	void BreakThroughGarde(int equip_type, int equip_grid, int role_type = ROLE_TYPE_BENZUN);

	void DianHuaEquip(int equip_grid, int dianhuashi_coloumn, int dianhuashi_grid);
	void SaveDianHuaEquip(int equip_grid);
	void XiFuEquip(int equip_grid);
	void FuMoEquip(int equip_grid, int *new_spid = NULL, int *new_level = NULL);
	void ComposeDianHuaShi(int main_stone_idx, int cost_stone_1, int cost_stone_2);

	bool Use(short column, short index, short num, short param1, short param2, short param3, int item_param_type = ITEM_PARAM_TYPE_INVAILD, void * param = NULL);
	bool Discard(short column, short index, short discard_num);
	bool InOrder(short column, short index_start, short index_end, bool ignore_time_interval = false);
	bool Deposit(short column, short index, short storage_page); // 存入物品到仓库
	bool Withdraw(short column, short index, ARG_OUT short* out_column, bool notice_on_error = false); // 从仓库(临时背包)取出物品
	bool WithdrawAll(short column);
	bool Extend(short column, short add_grid_num); // 扩充指定背包栏
	bool DecomposeSurface(short column, short index, short discard_num);   //时装分解
	bool SendMail(const ItemConfigData * item_list, int item_list_num, int type = SNED_MAIL_TYPE_DEFAULT, bool is_notice = false, int param1 = 0, const char * param2 = NULL, int param3 = 0);	//背包满改由邮件下发
	bool SendMail(const std::vector<ItemConfigData> & item_list, int type = SNED_MAIL_TYPE_DEFAULT, bool is_notice = false, int param1 = 0, const char * param2 = NULL, int param3 = 0);	//背包满改由邮件下发

	bool Put(const ItemConfigData &item_configdata, int reason_type, int* out_column = NULL, int *out_index = NULL, UInt16 drop_monster_id = 0, int gold_price = 0, bool is_notice = true);
	bool Put(const ItemDataWrapper &item_wrapper, int reason_type, int* out_column = NULL, int *out_index = NULL, UInt16 drop_monster_id = 0, bool is_notice = true);
	bool Put2(int item_id, int num, int reason_type, bool is_bind = true, int * out_column = NULL, int * out_index = NULL, UInt16 drop_monster_id = 0, int gold_price = 0, bool is_notice = true);
	bool PutList(short put_list_num, const ItemConfigData *item_put_list, int reason_type, bool is_noitce = true);		// 有净值物品不可用此接口
	bool PutList3(short put_list_num, const ItemConfigData * item_put_list, int reason_type, std::vector< std::pair<int, int> > * out_column_index_list = NULL, bool is_noitce = true);		// 有净值物品不可用此接口
	bool PutList2(short list_num, const ItemConfigData * item_configdata_list, int reason_type, int get_reson = GET_REASON_INVALID, int param1 = 0);		// 有净值物品不可用此接口	
	bool PutList(short list_num, const ItemDataWrapper *item_wrapper_list, int reason_type, int *out_column_list = NULL, int *out_index_list = NULL);
	bool PutVec(const std::vector<ItemConfigData> & item_configdata_list, int reason_type);
	bool PutVec(const std::vector<ItemDataWrapper>& item_wrapper_list, int reason_type);
	bool MergeBeforePutList(short list_num, const ItemConfigData *item_configdata_list, ARG_OUT short* out_list_num, ARG_OUT ItemConfigData* out_item_list, ARG_IN short out_item_max_num);
	void MergeItemList(std::vector<ItemConfigData> item_configdata_list, std::vector<ItemConfigData> & out_item_list);
	//bool PutOrMail(const ItemConfigData &item_configdata, int reason_type);
	//bool PutListOrMail(const ItemConfigData item_configdata_list[MAX_ATTACHMENT_ITEM_NUM], int reason_type);

	// PutList前用这个检查	
	bool CheckForPutList(short list_num, const ItemConfigData *item_configdata_list, bool need_check_temperary = true);
	bool CheckForPutList2(const std::vector<ItemConfigData> & item_configdata_list, bool need_check_temperary = true);
	bool CheckForPutList(short list_num, const ItemDataWrapper *item_wrapper_list, bool need_check_temperary = true);
	bool CheckForPutListWithoutTemporary(short list_num, const ItemConfigData * item_configdata_list);
	bool CheckForPutListWithoutTemporaryVec(const std::vector<ItemConfigData> & item_configdata_list);
	bool CheckEmptyGridNoLessThan(int column, int check_num, int * out_need_num = NULL, bool is_notice_no_space = false);		// 检查当前背包空位是否不少于这个值

	// Put前用这个函数检查
	bool CheckForPut(ItemID item_id, int num, bool need_check_temporary = true);	// 检查指定物品跟数量能否够位置放进去背包（根据堆叠数计算占用格子数）
	bool CheckForPut2(const ItemConfigData & item, bool need_notice = true, bool need_check_temporary = true);	// 检查指定物品跟数量能否够位置放进去背包（根据堆叠数计算占用格子数）
	bool CheckForPutItemDataWrapper(const ItemDataWrapper & item, bool need_notice = true, bool need_check_temporary = true);	// 检查指定物品跟数量能否够位置放进去背包（根据堆叠数计算占用格子数）

	bool CheckEmptyGridNoLessThanInTemporary(int check_num);		// 检查当前临时背包空位是否不少于这个值
	bool HaveSpaceInTemporary();
	bool CheckEmptyGridNoLessThanForItem(ItemID item_id, int num);

	// 检查的同时会执行Put，不适用于Check和Put要分开的场合
	//bool CheckNumAndPut(const ItemConfigData &item_configdata, int reason_type, bool check_pile_limit = true); // 参数的物品数量可以超过最大堆叠数，可以看作Put的优化版本

	int  Count(ItemID item_id, bool is_check_temporary = false, bool is_check_storage = false);
	bool HasItem(ItemID item_id, int num, bool is_check_temporary = true);
	bool HasItemByMap(const std::map<int, int> & check_item_map, bool is_check_temporary = true);

	bool ConsumeItemByMap(const std::map<int, int> & check_item_map, const char * reason);
	bool ConsumeItem(ItemID item_id, int num, const char * reason, int consume_type = KNAPSACK_CONSUME_TYPE_CANNOT_BUY, bool notify_lack = false);
	bool ConsumeItemByIndex(int column, int index, int num, const char * reason, bool is_real_consume = true, bool need_audit_log = true);
	bool ConsumeItemByIndexList(int list_num, const short* column_list, const short *knapsack_index_list, const short *consume_num_list, const char *reason, bool is_real_consume = true, bool need_audit_log = true);		// 允许index重复
																																																							//消耗道具类型所在的背包栏+仓库+临时背包
	bool ConsumeKnapsackItem(ItemID item_id, int consume_num, const char *reason, int consume_type = KNAPSACK_CONSUME_TYPE_CANNOT_BUY, bool notify_lack = false);

	void SendKnapsackInfoToRole(short reason = 0);

	void OnCheckNorexConvertItem(ItemConfigData & item_data);		//主动消耗道具转换
	bool OnRoleAttrPelletConvert(ItemConfigData item_data, std::vector<ItemConfigData> & convert_item_list);	//属性丹转换

	bool GetItemGridData(int column, int index, ItemGridData *item_data);
	ItemGridData * GetItemGridData2(int column, int index);
	const ItemGridData* GetItemGridData(int column, int index) const;
	bool GetItemGridWrapData(int column, int index, ItemDataWrapper* out_wrapper);
	void SetItemDirty(int column, int index, bool to_notice_client, short reason_type, short put_reason = 0);
	void RefreshValidGridIndexEnd(int column, int change_index);											// 修正非空格子最终序号

	void GridSetBind(int column, int index, bool need_set_dirty);
	void GridClearInvalidTime(int column, int index, bool need_set_dirty);

	void GetItemList(int *count, ItemListParam::ItemListData *save_list);
	void GetChangeItemList(int *count, ItemListParam::ItemListData *save_list);
	void ClearDirtyMark();

	bool GetListOfItem(ItemID item_id, short max_num,
		short *bind_column_list, short *bind_index_list, short *bind_num_list, short *bind_count,
		short *nobind_column_list, short *nobind_index_list, short *nobind_num_list, short *nobind_count);

	int GetGridIndex(ItemID item_id);
	bool GetGridColumnIndex(ItemID item_id, int & out_column, int & out_index);

	void NotifyGetItemList(short list_num, const ItemConfigData *item_configdata_list, int get_reson = 0, int param1 = 0, int param2 = 0);
	void NotifyGetItemListVec(const std::vector< ItemConfigData> & list, int get_reson = 0, int param1 = 0, int param2 = 0);
	void NotifyGetItemList3(int add_gold, int add_coin_bind, int add_exp, short list_num, const ItemConfigData *item_configdata_list, int get_reson = 0, int param1 = 0);

	static bool GetRealIndexColumn(int index, int *out_real_column, int *out_real_index);
	static int GetIndexByRealIndexColumn(int real_column, int real_index);
	void ClearGrid(int column, int index);
	void ClearAll();																			// 清空背包，仅供GM命令使用

																								//清理道具列表
	void ClearItemList(int column, std::vector<ItemID>* item_list, const char *reason);//清除某类背包里面的物品列表

	bool UseExpCrystal();

	void OnChangeProf();

	void OnRoleLogout();
	void SendExpCrystalInfo();
	void OnRoleModifyExpCrystal();
	void RetrieveItem(ItemID item_id, short num, int reason);
	void SendGetItemNotice(int notice_type, ItemID item_id, short num, int param1 = 0);

	int GetKnapsackVaildNum(int column);

	//化身操作
	void SynItemData(int column, ItemGridData * item_data);			//同步化身道具数据 化身->本尊
	void SynHuaShenItemData(std::set<long long> * item_set = NULL, bool is_element_crystal = false);	//赛季开始,同步本尊背包道具信息  本尊->化身
	void SynItemBindInfo(int column, long long item_unique_id, bool is_bind);	//同步化身道具绑定情况  化身->本尊

	int ReclaimItem(ItemID item_id);			//回收物品
	bool ReturnGodEquip();//神装突破修复
	bool ReturnGodEquip2();//神装突破修复

	void CheckItemReturn();
	void CheckGodEquipErrUplevel();
	void CheckGodEquipChangeLevel();
	void FixGodEquipBaseVoc();

	void SendErrorNotice();
	void GodEquipReBack();
	void GmCheckItemInWrongColumn();

	//宠物魂器
	//检测魂器仓库的收藏是否激活
	void CheckPetSoulCollection(bool need_sync = false);

	void ChangeNewSuit();
	void ChangeNewVocEatra();

public:
	// 堆叠道具工具函数
	static void AddRewards(std::vector<ItemConfigData>& rewards, ItemConfigData item);
	// 转化为日志字符串
	static std::string RewardsLog(const std::vector<ItemConfigData>& rewards);

private:
	bool CheckZhouQiItemExpired(ItemGridData * item_grid,const ItemBase * item_base);

	enum ITEM_CHANGE_TYPE
	{
		ITEM_CHANGE_TYPE_SNEAKY_IN = -4,	// 偷偷的放入 不需要通知玩家获得 当脱下装备和宝石镶嵌惩罚时使用这个
		ITEM_CHANGE_TYPE_CHANGE = -3,		// 发生改变
		ITEM_CHANGE_TYPE_OUT = -2,			// 从背包进入外部
		ITEM_CHANGE_TYPE_IN = -1,			// 从外部进入背包
											// 0以上表示是从背包/仓库的其他格子里移动过来/去 值表示原来的下标
	};

	void SendknapsackGridInfoToRole(int column, int index, short reason_type, short put_reason = 0);

	void CertainPut(int column, const ItemDataWrapper & item_info, int reason_type, int * out_column, int * out_index, UInt16 drop_monster_id = 0, bool is_notice = true);
	void CertainPut(const ItemDataWrapper &item_info, int reason_type, int* out_column, int *out_index, UInt16 drop_monster_id = 0);
	bool HaveSpace(int column, bool notice_on_error = false) { return CheckEmptyGridNoLessThan(column, 1, NULL, notice_on_error); }

	bool PutHelper(const ItemDataWrapper &item_wrapper, int reason_type, int* out_column = NULL, int *out_index = NULL, UInt16 drop_monster_id = 0, bool is_notice = true);
	bool CheckVirtualBagHaveGrids(int bags_type, int grid_num);
	bool PutItemIntoCorrespondBag(const ItemDataWrapper &item_wrapper, int reason_type);

	bool CertainMoveHelper(int from_column, int from_index, int to_column, int offset_index = 0, int range = 0); // 该函数不检查to_column栏位的空间，留给调用者自行判断
	void SendNoSpaceNotice(int column);
	void AddNoSpaceErrorNotice(int column);
	void AddErrorNotice(int error_num);
	void ClearErrorNotice();
	void CheckItemInWrongColumn();
	void CheckColumnInitialGridNum();
	void CheckEquipZhuLing();
	bool PutExpenseItemAutoUse(const ItemBase* itembase, int num, bool is_bind, unsigned int invalid_time, bool is_notice);

	RoleModuleManager* m_module_mgr;
	bool need_confirm;
	GemUpgradeInfo m_gem_upgrade_param;
	bool m_old_state[ItemNamespace::ITEM_COLUMN_TYPE_MAX][ItemNamespace::ITEM_COLUMN_GRID_NUM_MAX];
	bool m_dirty_mark[ItemNamespace::ITEM_COLUMN_TYPE_MAX][ItemNamespace::ITEM_COLUMN_GRID_NUM_MAX];
	bool m_knapsack_is_change;

	short m_valid_grid_index_end_list[ItemNamespace::ITEM_COLUMN_TYPE_MAX];	// 当前背包中非空格子的再下一个格子序号
	short m_curr_valid_grid_num_list[ItemNamespace::ITEM_COLUMN_TYPE_MAX];	// 当前背包所有栏的有效格数
	short m_extend_column_times_list[ItemNamespace::ITEM_COLUMN_TYPE_MAX];	// 扩展背包栏的次数

	ItemGridData* m_grid_list[ItemNamespace::ITEM_COLUMN_TYPE_MAX][ItemNamespace::ITEM_COLUMN_GRID_NUM_MAX]; // 背包仓库数据

	UInt32 m_knapsack_last_inorder_time;									// 上次背包整理时间
	UInt32 m_storage_last_inorder_time;										// 上次仓库整理时间
	UInt32 m_last_inorder_merge_time;										// 上次整理并合并时间

	char m_init_flag;
	char m_novice_gift_flag;

	unsigned int m_clear_temporary_column_time;

	std::vector<int> m_error_notice;
};

#endif