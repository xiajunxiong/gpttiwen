#ifndef __LOGDEF_H__
#define __LOGDEF_H__

#include "utility/logagent.h"
#include "ilogmodule.h"

enum LOG_TYPE
{
	LOG_TYPE_INVALID = 0,
	LOG_TYPE_GOLD_ADD = 1,						// 增加元宝
	LOG_TYPE_GOLD_USE = 2,						// 使用元宝
	LOG_TYPE_IMMORTAL_COIN_ADD = 3,				// 增加仙币
	LOG_TYPE_IMMORTAL_COIN_USE = 4,				// 使用仙币
	LOG_TYPE_COIN_BIND_ADD = 5,					// 增加绑定铜钱
	LOG_TYPE_COIN_BIND_USE = 6,					// 使用绑定铜钱

	LOG_TYPE_ITEM_PUT = 7,						// 获得物品
	LOG_TYPE_ITEM_USE = 8,						// 使用物品
	LOG_TYPE_ITEM_DISCARD = 9,					// 丢弃物品
	LOG_TYPE_ITEM_CONSUME = 10,					// 消耗物品

	LOG_TYPE_TRADE_MARTKET_ADD = 11,			// 上架物品
	LOG_TYPE_TRADE_MARTKET_DEL = 12,			// 下架物品
	LOG_TYPE_TRADE_MARTKET_BUY = 13,			// 交易成功
	LOG_TYPE_TRADE_MARTKET_EXPIRED = 14,		// 物品过期

	LOG_TYPE_MAIL_ITEM_IN = 15,					// 邮件获得物品
	LOG_TYPE_MAIL_ITEM_OUT = 16,				// 邮件发送物品

	LOG_TYPE_FORBID_TALK = 17,					// 禁言
	LOG_TYPE_FORBID_ROLE = 18,					// 禁止用户登录
	
	LOG_TYPE_ROLE_PRESTIGE = 20,				// 角色声望

	LOG_TYPE_ONLINE_ROLE_NUM = 49,				// 在线人数
	LOG_TYPE_XIANHUN = 50,						// 仙魂
	LOG_TYPE_YUANLI = 51,						// 元力
	LOG_TYPE_HONOUR = 52,						// 荣誉
	LOG_TYPE_REGISTER = 53,						// 注册
	LOG_TYPE_LOGIN = 54,						// 登录
	LOG_TYPE_LOGOUT = 55,						// 登出
	LOG_TYPE_UPGRADE_LEVEL = 56,				// 升级
	LOG_TYPE_CHANGE_AUTHORITY_TYPE = 57,		// 设置权限

	LOG_TYPE_ACCEPT_TASK = 58,					// 接受任务
	LOG_TYPE_COMMIT_TASK = 59,					// 提交任务
	LOG_TYPE_GIVEUP_TASK = 60,					// 放弃任务
	LOG_TYPE_COMPLETE_TASK = 61,				// 完成任务

	LOG_TYPE_PAY = 64,							// 充值
	LOG_TYPE_PLAT_ONLINE = 65,					// 平台在线(按角色平台名前缀分组统计)

	LOG_TYPE_MAIL_SEND_GOLD = 70,				// 邮件发元宝 （g6不允许）
	LOG_TYPE_MAIL_RECEIVE_GOLD = 71,			// 邮件获得元宝

	LOG_TYPE_RAND_ACTIVITY_CHONGZHI_RANK = 79,	// 充值排行
	LOG_TYPE_RAND_ACTIVITY_CONSUME_GOLD_RANK = 80, // 消费排行

	LOG_TYPE_TRADE_MARKET_DEAL = 90,			// 交易行成交记录

	
	LOG_TYPE_RETRIEVE_GOLD_UNBIND = 111,		// 回收元宝
	LOG_TYPE_RETRIEVE_GOLD_BIND = 112,			// 回收绑元
	LOG_TYPE_RETRIEVE_COIN_BIND = 113,			// 回收绑铜
	LOG_TYPE_RETRIEVE_VIP_EXP = 114,			// 回收vip经验
	LOG_TYPE_RETRIEVE_ITEM = 115,				// 回收物品
	LOG_TYPE_CAPABILITY_CHANGE = 116,			// 战力变化
	LOG_TYPE_RETRIEVE_OTHER_CURRENCY = 117,		// 回收其他类型货币
	LOG_TYPE_OTHER_MONEY_USE = 118,				// 使用其他货币

	LOG_TYPE_CAPABILITY_CHANGE_LOGIN = 124,		// 登录时与离线时的战力发生变化

	LOG_TYPE_ROLE_LV1_MINE_MONSTER = 150,		// 玩家遭遇1级怪
	LOG_TYPE_SYSTEM_LV1_MONSTER = 151,			// 系统统计1级怪
	LOG_TYPE_ROLE_EXP = 152,					// 角色经验
	LOG_TYPE_ACTIVE_POINT = 153,				// 活跃度
	LOG_TYPE_WORK_SHOP_EQUIPMENT = 154,			// 工坊 - 装备
	LOG_TYPE_WORK_SHOP_JEWELRY = 155,			// 工坊 - 灵饰
	LOG_TYPE_PARTNER_CHANGE_ADD = 156,			// 伙伴新增
	LOG_TYPE_PARTNER_CHANGE_ATTR = 157,			// 伙伴进阶
	LOG_TYPE_COLLECTION = 158,					// 图鉴
	LOG_TYPE_FABAO_ACTIVE = 159,				// 激活法宝
	LOG_TYPE_FABAO_UP = 160,					// 法宝进阶
	LOG_TYPE_MIYI = 161,						// 试炼之地
	LOG_TYPE_COURAGECHALLENGE = 162,			// 锢魔之塔
	LOG_TYPE_PET_ADD = 163,						// 宠物新增
	LOG_TYPE_PET_ABANDONMENT = 164,				// 宠物放生
	LOG_TYPE_PET_BREAKTHROUGHS = 165,			// 宠物强化
	LOG_TYPE_OTHER_MONEY = 166,					// 特殊货币
	LOG_TYPE_ADD_TITLE = 167,					// 获得称号
	LOG_TYPE_ACTIVITY_QUICK = 168,				// 周常活动参与(后台统计总人数)
	LOG_TYPE_PET_REMOVE = 169,					// 宠物删除
	LOG_TYPE_ROLE_FIRST_KILL_MONSTER_GROUP = 170,//首通怪物组 
	LOG_TYPE_FUNCTION_STATUS = 171,				// 功能参与
	LOG_TYPE_HUANHUA_ACTIVE = 172,				// 主角幻化激活
	LOG_TYPE_PET_SKIN_ACTIVE = 173,				// 宠物皮肤激活
	LOG_TYPE_PARTNER_SKIN_ACTIVE = 174,			// 伙伴皮肤激活
	LOG_TYPE_FACESCORE_ACTIVE = 175,			// 颜值系统激活
	LOG_TYPE_RAND_ACTIVITY = 176,				// 随机活动
	LOG_TYPE_PARTNER_PACT = 177,				// 伙伴元神
	LOG_TYPE_PET_GOD_PRINT = 178,				// 宠物神印
	LOG_TYPE_EQUIP_ZHULING = 179,				// 装备铸灵
	LOG_TYPE_GEM_INLAY = 180,					// 宝石镶嵌
	LOG_TYPE_HUAN_JIE_ROLE = 181,				// 幻界角色
	LOG_TYPE_HUAN_JIE_PET = 182,				// 幻界宠物
	LOG_TYPE_MARKET = 183,						// 市场
	LOG_TYPE_ACTIVE_STAT = 184,					// 功能活跃统计
	LOG_TYPE_RAND_ACTIVITY_PARTICIPATE = 185,	// 随机活动玩家参与
	LOG_TYPE_RESET_NAME = 186,					// 玩家改名
	LOG_TYPE_PARTNER_AWAKENING = 187,			// 伙伴觉醒
	LOG_TYPE_UNIVERSAL_ACTIVE_CARD = 188,		// 万能激活卡使用
	LOG_TYPE_CLIENT_BEHAVIOR = 189,				// 客户端行为埋点记录
	LOG_TYPE_ENTER_UTA_BATTLE = 190,			// 参与天下第一战斗
	LOG_TYPE_GOD_PRINT_DISCOMPOSE = 191,		// 神印分解
	LOG_TYPE_TOP_LEVEL = 192,					// 巅峰等级
	LOG_TYPE_UTA_QUALIFICATION = 193,			// 天下第一资格赛
	LOG_TYPE_UTA_FIGHT_RESULT = 194,			// 天下第一比赛结果
	LOG_TYPE_UTA_USER_FIGHT_RESULT = 195,		// 天下第一玩家战斗结果
	LOG_TYPE_UTA_PET_FIGHT_RESULT = 196,		// 天下第一宠物战斗结果
	LOG_TYPE_UTA_TEAM_MEMBER = 197,				// 天下第一队伍成员信息
	LOG_TYPE_ZAO_HUA_ADD_ITEM = 198,			// 造化装备获得
	LOG_TYPE_YUAN_HUN_ZHU = 199,				// 元魂珠
	LOG_TYPE_HOLY_EQUIP = 200,					// 圣器获得
	LOG_TYPE_FU_SHI = 201,						// 符石获得
	LOG_TYPE_SHENG_QI_LEVEL_UP = 202,			// 圣器升级
	LOG_TYPE_SHENG_QI_STATE_CHANGE = 203,		// 圣器状态改变
	LOG_TYPE_FU_YU = 204,						// 符玉
	LOG_TYPE_GUILD_HONOR_RESULT = 205,			// 家族荣誉战结果

	LOG_TYPE_MAX,
};


// 日志ID=184 功能活动统计 枚举定义
enum LOG_ACTIVE_TYPE
{
	LOG_ACTIVE_TYPE_INVALID = 0,
	LOG_ACTIVE_TYPE_RESEARCH_TASK = 1,			//!< 1 三界除魔
	LOG_ACTIVE_TYPE_CLOUD_ARENA = 2,			//!< 2 穹顶之争
	LOG_ACTIVE_TYPE_CHALLENGE_FB = 3,			//!< 3 挑战副本
	LOG_ACTIVE_TYPE_CHALLENGE_TASK = 4,			//!< 4 挑战任务
	LOG_ACTIVE_TYPE_ESCORT = 5,					//!< 5 运镖
	LOG_ACTIVE_TYPE_PLATFORM_BATTLE = 6,		//!< 6 擂台
	LOG_ACTIVE_TYPE_MAZE_RACE = 7,				//!< 7 迷宫竞速
	LOG_ACTIVE_TYPE_MIYI = 8,					//!< 8 远征魔冢
	LOG_ACTIVE_TYPE_TASK_CHAIN = 9,				//!< 9 商人的烦恼
	LOG_ACTIVE_TYPE_TEAM_FIGHT = 10,			//!< 10 竞技对抗赛
	LOG_ACTIVE_TYPE_WORLD_BOSS = 11,			//!< 11 魔将入侵
	LOG_ACTIVE_TYPE_GUILD_FIGHT = 12,			//!< 12 家族大乱斗
	LOG_ACTIVE_TYPE_BRAVE_GROUND = 13,			//!< 13 勇闯地宫
	LOG_ACTIVE_TYPE_CHIEF_ELECT = 14,			//!< 14 首席精选
	LOG_ACTIVE_TYPE_BOUNTY_TASK = 15,			//!< 15 门派任务
	LOG_ACTIVE_TYPE_COURAGE_CHALLENGE = 16,		//!< 16 锢魔之塔
	LOG_ACTIVE_TYPE_WORLD_BOSS_2 = 17,			//!< 17 讨伐巨龙
	LOG_ACTIVE_TYPE_ONLY_FIGHT = 18,			//!< 18 以一敌百
	LOG_ACTIVE_TYPE_GUILD_QUESTION = 19,		//!< 19 家族答题
	LOG_ACTIVE_TYPE_FIELD_BOSS = 20,			//!< 20 野外BOSS
	LOG_ACTIVE_TYPE_GUILD_HONOR = 21,			//!< 21 家族荣誉战
	LOG_ACTIVE_TYPE_HUANJIE_BATTLE = 22,		//!< 22 幻界战场
	LOG_ACTIVE_TYPE_DREAM_NOTE = 23,			//!< 23 梦渊笔录
	LOG_ACTIVE_TYPE_SCHOOL_EXERCISE = 24,		//!< 24 校场演武
	LOG_ACTIVE_TYPE_LUN_HUI_WANG_CHUAN = 25,	//!< 25 轮回忘川
	LOG_ACTIVE_TYPE_SHAN_HAI_BOSS = 26,			//!< 26 山海降妖
	LOG_ACTIVE_TYPE_PET_MONOPLY = 27,			//!< 27 灵兽岛
	LOG_ACTIVE_TYPE_MATERIAL_FB = 28,			//!< 28 玉虚境
	LOG_ACTIVE_TYPE_CRYSTAL_FB_COMMON = 29,		//!< 29 云梦泽（水晶副本）
	LOG_ACTIVE_TYPE_CRYSTAL_FB_RARE = 30,		//!< 30 幻境试炼-稀有副本（水晶副本稀有）
	LOG_ACTIVE_TYPE_WORLD_PURGATORY = 31,		//!< 31 幽冥炼狱
	LOG_ACTIVE_TYPE_ENDLESS_TOWER = 32,			//!< 32 无尽之塔
	LOG_ACTIVE_TYPE_BIG_DIPPER = 33,			//!< 33 北斗七星
	LOG_ACTIVE_TYPE_TIAN_DI_DAO_HEN = 34,		//!< 34 天地道痕
	LOG_ACTIVE_TYPE_JIEYI = 35,					//!< 35 结义
	LOG_ACTIVE_TYPE_LUNCH_DINNER = 36,			//!< 36 午餐晚餐
	LOG_ACTIVE_TYPE_CRYSTAL_FB_ELITE = 37,		//!< 37 上古遗迹（水晶副本精英）
	LOG_ACTIVE_TYPE_HUARONGDAO = 38,			//!< 38 奇闻-华容道
	LOG_ACTIVE_TYPE_TREASURE_BOX_UNLOCK = 39,	//!< 39 奇闻-宝箱解密
	LOG_ACTIVE_TYPE_FIND_FOOD = 40,				//!< 40 奇闻-寻味美食
	LOG_ACTIVE_TYPE_POEMS_AND_SONGS = 41,		//!< 41 奇闻-诗词歌赋
	LOG_ACTIVE_TYPE_REAL_FAKE_PAINTING = 42,	//!< 42 奇闻-真假画作
	LOG_ACTIVE_TYPE_PET_WISH_GOD_PRINT = 43,	//!< 43 宠物祈神
	LOG_ACTIVE_TYPE_WORLD_BOSS_3 = 44,			//!< 44 世界BOSS(天神降临)

	LOG_ACTIVE_TYPE_MAX
};

enum LOG_GET_ITEM_WAY
{
	LOG_GET_ITEM_WAY_XUNBAO = 0,
	LOG_GET_ITEM_WAY_SHOP,
	LOG_GET_ITEM_WAY_COMPOSE,
	LOG_GET_ITEM_WAY_DUIHUAN,

	LOG_GET_ITEM_WAY_MAX,
};

enum LOG_MONEY_TYPE
{
	LOG_MONEY_TYPE_GOLD = 0,
	LOG_MONEY_TYPE_GOLD_BIND,
	LOG_MONEY_TYPE_COIN,
	LOG_MONEY_TYPE_HONOR,
	LOG_MONEY_TYPE_SHENGWANG,
	LOG_MONEY_TYPE_CROSS_HONOR,

	LOG_MONEY_TYPE_MAX,
};

enum LOG_TYPE_FUNCTION_STATUS_TYPE
{
	LOG_TYPE_FUNCTION_STATUS_TYPE_QIFU_COIN = 0,
	LOG_TYPE_FUNCTION_STATUS_TYPE_QIFU_YUANLI,
	LOG_TYPE_FUNCTION_STATUS_TYPE_QIFU_XIANHUN,
	LOG_TYPE_FUNCTION_STATUS_TYPE_ACCEPT_HUSONG,
	LOG_TYPE_FUNCTION_STATUS_TYPE_ACCEPT_DAILY,
	LOG_TYPE_FUNCTION_STATUS_TYPE_ACCEPT_GUILDTASK,
	LOG_TYPE_FUNCTION_STATUS_TYPE_XUNBAO,
	LOG_TYPE_FUNCTION_STATUS_TYPE_ACCEPT_LOOPTASK,

	LOG_TYPE_FUNCTION_STATUS_TYPE_ACTIVITY = 10000,

	LOG_TYPE_FUNCTION_STATUS_TYPE_FB = 20000,
};

inline void LOG_QUICK8(LogAgent &log_agent, long long n1, long long n2, const char *z1, const char *z2, long long n3, long long n4, const char *z3, const char *z4)
{
	log_agent.printf(LL_INFO, "%lld\t%lld\t%s\t%s\t%lld\t%lld\t%s\t%s", 
		(long long)n1, 
		(long long)n2,
		(NULL == z1 ? "-" : z1),
		(NULL == z2 ? "-" : z2),
		(long long)n3,
		(long long)n4,
		(NULL == z3 ? "-" : z3),
		(NULL == z4 ? "-" : z4)
		);
}

inline void LOG_QUICK12(LogAgent &log_agent, long long n1, long long n2, const char *z1, const char *z2, long long n3, long long n4, const char *z3, const char *z4, 
	long long n5, long long n6, long long n7, long long n8, long long n9)
{
	log_agent.printf(LL_INFO, "%lld\t%lld\t%s\t%s\t%lld\t%lld\t%s\t%s\t%lld\t%lld\t%lld\t%lld\t%lld", 
		(long long)n1, 
		(long long)n2,
		(NULL == z1 ? "-" : z1),
		(NULL == z2 ? "-" : z2),
		(long long)n3,
		(long long)n4,
		(NULL == z3 ? "-" : z3),
		(NULL == z4 ? "-" : z4),
		(long long)n5,
		(long long)n6,
		(long long)n7,
		(long long)n8,
		(long long)n9
		);
}

#endif
