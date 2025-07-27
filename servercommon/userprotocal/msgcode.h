
#ifndef __PROTOCAL_H__
#define __PROTOCAL_H__

#include "servercommon/servercommon.h"

namespace Protocol
{
	enum _MsgType
	{
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		MT_ENTER_GS_ACK_SC = 1000,						// 请求进入场景服务器回复
		MT_CHANGE_GS_SC = 1001,
		MT_SERVER_BUSY_SC = 1002,						// 服务器太忙（承载用户数达到配置上限）
		MT_HEARTBEAT_SC = 1003,							// 心跳包返回
		MT_RESET_NAME_SC = 1004,						// 重置名字回包
		MT_NEW_MESSAGE_FROM_CUSTOM_SERVICE = 1005,		// 客服新通知
		MT_CMD_ROUTE_SC = 1006,		// 新增后台->服务端->客户端透传指令 转发

		MT_ENTER_GS_REQ_CS = 1050,						// 请求进入场景服务器
		MT_LOGOUT_CS = 1051,
		MT_TRANSPORT_REQ = 1052,
		MT_HEARTBEAT_CS = 1053,							// 心跳包请求
		MT_RESET_NAME_CS = 1054,						// 重置名字
		MT_SPECIAL_TRANSPORT_REQ_CS = 1055,				// 场景特别逻辑传送门
		MT_TREASURE_CHEST_REQ_CS = 1056,				// 宝箱操作
		MT_ROLE_SUBSCRIBE_REQ_CS = 1057,				// 订阅/取消订阅系统信息
	
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		MT_ENTER_SCENE_SC = 1100,
		MT_LEAVE_SCENE_SC = 1101,
		MT_VISIBLE_ENTER_ROLE_SC = 1102,				// Role对象进入可视区域
		MT_VISIBLE_LEAVE_SC = 1103,						// 对象退出可视区域
		MT_OBJ_MOVE_SC = 1104,							// 对象位置移动同步
		MT_RESET_POST_SC = 1105,						// 强设主角位置
		MT_CHANGE_SCENE_NOTICE_SC = 1106,				// 改变场景通知
		MT_ENTER_FB_CS = 1107,							// 请求进入副本
		MT_LEAVE_FB_CS = 1108,							// 请求离开副本
		MT_VISIBLE_ENTER_NPC_MONSTER_SC = 1109,			// NPC怪物进入视野通知
		MT_VISIBLE_ENTER_NPC_GATHERS_SC = 1110,			// NPC采集物进入视野通知
		MT_STOP_MOVE_NOTICE_SC = 1111,					// 通知客户端停止移动
		MT_APPEARACNE_CHANGE_SC = 1112,					// 形象改变
		MT_VISIBLE_ENTER_SPECIAL_TRANSPORT_SC = 1113,	// 特别逻辑传送门进入视野
		MT_VISIBLE_ENTER_TREASURE_CHEST_SC = 1114,		// 宝箱进入视野
		MT_VISIBLE_ENTER_NPC_FUNCNPC_SC = 1115,			// 功能性NPC进入视野通知
		MT_SCENE_OBJ_LIST_SC = 1116,					// 场景中obj位置列表
		MT_UPDATE_FOLLOW_LIST_SC = 1117,				// 更新跟随列表
		MT_ADD_FOLLOW_SC = 1118,						// 新增跟随
		MT_REMOVE_FOLLOW_SC = 1119,						// 删除跟随
		MT_JOIN_TEAM_NOTIFY_AREA_SC = 1120,				// 加入队伍通知附近的人
		MT_LEAVE_TEAM_NOTIFY_AREA_SC = 1121,			// 离开队伍通知附近的人
		MT_ROLE_BUFF_EFFECT_NOTIFY_AREA_SC = 1122,		// 玩家buff特效通知附件的人
		MT_SPECIAL_APPEARANCE_CHANGE_NOTICE_SC = 1123,	// 特殊形象改变通知  
		MT_ROLE_TALK_TO_NPC_CS = 1124,					// 与NPC对话
		MT_GET_CUR_POS_SC = 1125,						// 机器人客户端查询自己当前位置返回
		MT_WORLD_LEVEL_INFO_SC = 1126,					// 世界等级信息下发
		MT_DAY_REWARD_DAY_FOOD_INFO_SC = 1127,			// 午餐晚餐信息下发
		MT_VISIBLE_ENTER_XUN_YOU_CAR_SC = 1128,			// 巡游花车进入视野
		MT_VISIBLE_ENTER_PEAK_TOKEN_OBJ_SC = 1129,		// 天下至尊令 雕像进入视野
		MT_PET_STRENGTHEN_REWARD_INFO_SC = 1130,		// 宠物强化奖励信息下发
		MT_PET_STRENGTHEN_REWARD_REQ_CS = 1131,			// 宠物强化奖励请求
		MT_CLIENT_BEHAVIOR_REQ_CS = 1132,				// 客户端行为埋点请求

		MT_YAO_XING_SHI_LIAN_INFO_SC = 1133,			// 妖行试炼下发
		MT_YAO_XING_SHI_LIAN_REQ_CS = 1134,				// 妖行试炼请求

		MT_SAN_XIAN_GUI_DONG_REQ_CS = 1135,				// 三仙归洞请求
		MT_SAN_XIAN_GUI_DONG_INFO_SC = 1136,			// 三仙归洞下发
		MT_SAN_XIAN_GUI_DONG_PLAY_INFO_SC = 1137,		// 三仙归洞游戏下发
		MT_VISIBLE_ENTER_TU_ZI_KUAI_PAO_TU_ZI_OBJ_SC = 1138,		// 兔子快跑活动兔子进入视野

		MT_TAO_QUAN_NA_JI_REQ_CS = 1139,				// 套圈纳吉请求
		MT_TAO_QUAN_NA_JI_INFO_SC = 1140,				// 套圈纳吉下发
		MT_TAO_QUAN_NA_JI_PLAY_END_INFO_SC = 1141,		// 套圈纳吉结算下发

		MT_HUNDRED_GOD_BOSS_REQ_CS = 1145,				// 百神之师请求
		MT_HUNDRED_GOD_BOSS_INFO_SC = 1146,				// 百神之师信息下发

		MT_WILD_BOSS_OTHER_INFO_SC = 1147,				// 野外首领其它信息下发

		MT_CHARISMATIC_TANABATA_INFO_SC = 1148,			// 魅力七夕下发

		MT_OBJ_MOVE_CS = 1150,							// 移动
		MT_FLY_BY_SHOE_CS = 1151,						// 飞鞋
		MT_ESCAPE_FROM_CURRENT_POSITION = 1152,			// 客户端脱离卡死
		MT_AUTO_ESCAPE_FROM_CURRENT_POSITION = 1153,	// 客户端自动脱离卡死
		MT_GET_CUR_POS_CS = 1154,						// 机器人客户端查询自己当前位置
		MT_OBJ_SPEED_CHANGE_NOTICE = 1155,				// OBJ 速度改变通知

		//////////////////////////////////////////////////活动///////////////////////////////////////////////////////////////////
		MT_ACTIVITY_STATUS_SC = 1200,					// 活动状态返回
		MT_ACTIVITY_ENTER_REQ_CS = 1203,				// 请求进入活动
		MT_ACTIVITY_TEAM_FIGHT_MATCH_CS = 1204,			// 小队竞技-请求匹配
		MT_ACTIVITY_TEAM_FIGHT_NOTIC_SC = 1205,			// 小队竞技-匹配状态
		MT_ACTIVITY_TEAM_FIGHT_SCORE_SC = 1206,			// 小队竞技-积分
		MT_ACTIVITY_MAZE_RACER_HANDLE_REQ_CS = 1207,	// 迷宫竞速 - 操作请求
		MT_ACTIVITY_MAZE_RACER_AMULET_NUM_SC = 1208,	// 迷宫竞速 - 符咒数量
		MT_ACTIVITY_MAZE_RACER_RANK_INFOR_SC = 1209,	// 迷宫竞速 - 排行信息
		MT_ACTIVITY_TEAM_FIGHT_RANK_SC = 1210,			// 小队竞技-排行榜
		MT_ACTIVITY_TEAM_FIGHT_RECORD_SC = 1211,		// 小队竞技-记录
		MT_ACTIVITY_MAZE_RACER_HALL_INFOR_SC = 1212,	// 迷宫竞速 - 大厅信息
		MT_ACTIVITY_MAZE_RACER_COPY_INFOR_SC = 1213,	// 迷宫竞速 - 副本信息
		MT_ACTIVITY_MAZE_RACER_PASS_INFOR_SC = 1214,	// 迷宫竞速 - 通关信息
		MT_ACTIVITY_TEAM_FIGHT_MATCH_SUCC_NOTICE_SC = 1215,	// 小队竞技-倒数通知
		MT_ACTIVITY_MAZE_RACER_BUFF_INFOR_SC = 1216,	// 迷宫竞速 - 受咒信息
		MT_ACTIVITY_MAZE_RACER_ACT_INSERT_SC = 1217,	// 迷宫竞速 - 活动通关排名
		MT_CHONGZHI_INFO_SC = 1218,						// 返回充值相关信息
		MT_CHONGZHI_OPERATE_CS = 1219,					// 请求充值相关功能操作 
		MT_ACTIVITY_GUILD_FIGHT_HANDLE_REQ_CS = 1220,	// 家族大乱斗 - 操作请求
		MT_ACTIVITY_GUILD_FIGHT_RANK_INFOR_SC = 1221,	// 家族大乱斗 - 排行榜信息
		MT_ACTIVITY_GUILD_FIGHT_GUILD_INFO_SC = 1222,	// 家族大乱斗 - 我的家族信息
		MT_ACTIVITY_GUILD_ANSWER_CUR_QUESTION_SC = 1223,// 家族答题 - 提问信息
		MT_ACTIVITY_GUILD_ANSWER_ANSWER_QUEST_CS = 1224,// 家族答题 - 回答信息
		MT_ACTIVITY_GUILD_FIGHT_PARTIC_NUM_SC = 1225,	// 家族大乱斗 - 家族参与人数
		MT_ACTIVITY_GUILD_ANSWER_RESULT_INFOR_SC = 1226,// 家族答题 - 答题结果
		MT_ACTIVITY_GUILD_ANSWER_AWARD_SETTLE_SC = 1227,// 家族答题 - 结算信息
		MT_ACTIVITY_ONLY_FIGHT_HANDLE_REQ_CS = 1228,	// 以一敌百(个人版家族大乱斗) - 操作请求
		MT_ACTIVITY_ONLY_FIGHT_RANK_INFO_SC = 1229,		// 以一敌百(个人版家族大乱斗) - 排行榜信息
		MT_ACTIVITY_ONLY_FIGHT_MY_INFO_SC = 1230,		// 以一敌百(个人版家族大乱斗) - 我的信息
		MT_ACTIVITY_SIGN_UP_REQ_CS = 1231,				// 限时活动报名请求
		MT_ACTIVITY_SIGN_UP_INFO_SC = 1232,				// 限时活动报名信息下发
		MT_ACTIVITY_PARTNER_EXCHANGE_INFO_SC = 1233,	// 随机活动伙伴兑换下发
		MT_ACTIVITY_PET_EXCHANGE_INFO_SC = 1234,		// 随机活动宠物兑换下发
		MT_RA_TIAN_MING_DIVINATION_INFO_SC = 1235,					// 随机活动-天命卜卦活动信息
		MT_RA_TIANMING_DIVINATION_START_CHOU_RESULT_SC = 1236,		// 随机活动-天命卜卦卜卦结果
		MT_RA_TIANMING_DIVINATION_REWARD_POOL_SC = 1237,		// 随机活动-天命卜卦获奖池信息
		MT_RA_SPORTS_HISTROY_SC = 1239,					// 随机活动-新服竞技
		MT_ACTIVITY_MAZE_RACER_TRANSPORT_POS_SC = 1240,		// 迷宫竞速 - 传送门位置
		MT_ACTIVITY_ENTER_INFO_SC = 1241,				// 活动入场信息下发
		MT_ACTIVITY_GUILD_FIGHT_SPECIAL_INFO_SC = 1242,	//家族大乱斗特殊信息下发
		MT_ACTIVITY_ONLY_FIGHT_SPECIAL_INFO_SC = 1243,	//以一敌百特殊信息下发
		MT_RAND_ACTIVITY_MONEY_ADD_INFO_SC = 1244,		// 随机活动特殊货币增加
		MT_RAND_ACTIVITY_GIVE_SECRET_KEY_INFO_SC = 1245,// 随机活动赠送寻宝次数下发
		MT_RAND_ACTIVITY_CUMULATIVE_RECHARGE_SC = 1246,// 随机活动累计充值下发
		MT_ACTIVITY_ADVANCE_FETCH_REQ_CS = 1247,		// 限时活动提前领取奖励请求
		MT_ACTIVITY_ADVANCE_FETCH_INFO_SC = 1248,		// 限时活动提前领取奖励信息
		MT_RELOAD_ACTIVITY_NOTICE_SC = 1250,			// 热更活动配置重读通知
		MT_CMD_BUY_ITEM_CHECK_RET_SC = 1251,			// 
		MT_CMD_BUY_ITEM_CHECK_REQ_CS = 1252,			// 普通模块直购类型配置检测
		MT_ACTIVITY_MAZE_RACER_ROLE_INFO = 1253,		// 迷宫竞速 - 角色信息
		

		MT_RA_MAZE_SUMMER_MAP_INFO_SC = 1254,			// 随机活动-夏日迷宫地图下发
		MT_RA_MAZE_SUMMER_MAP_SINGLE_INFO_SC = 1255,	// 随机活动-夏日迷宫单格地图下发
		MT_RA_MAZE_SUMMER_SHOP_INFO_SC = 1256,			// 随机活动-夏日迷宫商店下发
		MT_RA_MAZE_SUMMER_QUESTION_INFO_SC = 1257,		// 随机活动-夏日迷宫题目下发
		MT_RA_MAZE_SUMMER_TASK_INFO_SC = 1258,			// 随机活动-夏日迷宫任务下发
		MT_RA_MAZE_SUMMER_STATUS_INFO_SC = 1259,		// 随机活动-夏日迷宫进行状态下发
		MT_RA_MAZE_SUMMER_FLAYINFO_FINISH_SC = 1260,	// 随机活动-夏日迷宫玩法结算下发
		MT_RA_MAZE_SUMMER_MY_POS_INFO_SC = 1261,		// 随机活动-夏日迷宫我的位置下发

		MT_RA_QIONG_LOU_YU_YAN_INFO_SC = 1262,			// 随机活动-琼楼玉宴下发
		MT_RA_QIONG_LOU_YU_YAN_ROLE_INFO_SC = 1263,		// 随机活动-琼楼玉宴个人信息下发

		MT_NOTICE_CLIENT_RESTART_SC = 1264,				// 通知客户端重启

		MT_ACTIVITY_SILK_ROADS_REQ_CS = 1270,			// 限时活动-丝绸之路请求
		MT_ACTIVITY_SILK_ROADS_SIMPLE_INFO_SC = 1271,	// 限时活动-丝绸之路简易信息下发
		MT_ACTIVITY_SILK_ROADS_ROLE_ALL_INFO_SC = 1272,	// 限时活动-丝绸之路角色所有信息
		MT_ACTIVITY_SILK_ROADS_ASSETS_INFO_SC = 1273,	// 限时活动-丝绸之路资产信息下发
		MT_ACTIVITY_SILK_ROADS_WAREHOUSE_CHANGE_SC = 1274,//限时活动-丝绸之路仓库变化
		MT_ACTIVITY_SILK_ROADS_ALL_RANK_INFO_SC = 1275,	// 限时活动-丝绸之路排行榜信息下发
		MT_ACTIVITY_SILK_ROADS_ALL_TRADING_LOG_SC = 1276,//限时活动-丝绸之路贸易消息列表下发
		MT_ACTIVITY_SILK_ROADS_SINGLE_TRADING_SC = 1277,// 限时活动-丝绸之路单条贸易消息下发

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		MT_SKILL_LIST_SC = 1300,						// 技能列表
		MT_SKILL_CHANGE_NOTICE = 1301,					// 技能信息变更通知
		MT_LIFE_SKILL_INFO_SC = 1302,					// 生活技能信息
		MT_COMMON_USE_SKILL_CHANGE_NOTICE = 1303,		// 常用技能变更通知
		MT_SKILL_EXP_CHANGE_NOTIFY_SC = 1304,			// 技能经验加值通知

		MT_NEW_LIFE_SKILL_REQ_CS = 1310,				// 生活技能请求
		MT_NEW_LIFE_SKILL_ALL_INFO_SC = 1311,			// 生活技能信息下发
		MT_NEW_LIFE_SKILL_SINGLE_INFO_SC = 1312,		// 生活技能单个信息下发
		MT_NEW_LIFE_SKILL_PROPS_ALL_INFO_SC = 1313,		// 生活食物类型信息下发
		MT_NEW_LIFE_SKILL_PROPS_SINGLE_INFO_SC = 1314,	// 生活食物类型单条下发

		MT_FISH_GROUND_REQ_CS = 1315,					// 钓鱼请求
		MT_FISHING_NOTIFY_SC = 1316,					// 钓鱼广播
		MT_STOP_FISHING_NOTIFY_SC = 1317,				// 结束钓鱼
		MT_LIFE_SKILL_CHANGE_CARD_SINGLE_INFO_SC = 1318,// 变身卡单条信息下发

		MT_NOTICE_CLIENT_RELOAD_SENSITIVE_WORDS_SC = 1320, //通知客户端重读敏感词列表 (与G4保持一致的协议号）
		
		MT_SKILL_OPERATE_REQ_CS = 1350,					// 技能操作请求
		MT_SKILL_POSITION_MODIFY_CS = 1351,				// 调整技能顺序

		MT_TASK_WAITING_LIST = 1352,					// 任务等待列表

		MT_SPECIAL_ANECDOTE_STAR_CS = 1354,					// 特殊奇闻任务开始
		MT_SPECIAL_ANECDOTE_TASK_LIST_SC = 1355,			// 特殊奇闻任务列表
		MT_SPECIAL_ANECDOTE_TASK_SINGLE_SC = 1356,			// 特殊奇闻任务 - 单个

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		MT_ROLE_INFO_ACK_SC = 1400,						// 请求角色基本信息返回
		MT_ROLE_MONEY_INFO_SC = 1401,					// 角色货币信息返回
		MT_ROLE_ATTRIBUTE_LIST_SC = 1402,				// 角色属性信息返回
		MT_ROLE_ATTRIBUTE_PLAN_INFO_SC = 1403,			// 角色属性加点信息返回
		MT_ROLE_EXP_CHANGE_NOTICE_SC = 1404,			// 角色经验改变通知
		MT_ROLE_LEVEL_CHANGE_NOTICE_SC = 1405,			// 角色等级改变通知
		MT_ROLE_PROFESSION_CHANGE_NOTICE_SC = 1406,		// 角色职业变更通知
		MT_ROLE_PROFESSION_EXTRA_INFO_SC = 1407,		// 角色职业额外信息返回
		MT_ROLE_SPEED_CHANGE_NOTICE_SC = 1408,			// 角色移速改变通知
		MT_ROLE_STATUS_CHANGE_NOTICE_SC = 1409,			// 角色状态改变通知
		MT_ROLE_AUTO_TALK_INFO_SC = 1410,				// 角色自动对话信息下发
		MT_ROLE_AUTHORITY_INFO_CHANGE_SC = 1411,		// 角色权限信息改变下发
		MT_ROLE_HISTORY_MAX_CAPABILITY_CHANGE_SC = 1412,// 角色历史最高评分变化下发


		MT_ALL_INFO_REQ_CS = 1450,						// 请求所有玩家的模块信息
		MT_ATTRIBUTE_PLAN_CREATE_CS = 1451,				// 请求角色创建新的角色加点方案
		MT_ATTRIBUTE_PLAN_CHANGE_NAME_CS = 1452,		// 请求对加点方案改名
		MT_ATTRIBUTE_PLAN_SWITCH_PLAN_CS = 1453,		// 请求切换加点方案
		MT_ATTRIBUTE_PLAN_ADD_POINT_CS = 1454,			// 请求加点
		MT_ATTRIBUTE_PLAN_CLEAR_POINT_CS = 1455,		// 请求洗点
		MT_ROLE_CAREER_ADVANCE_REQ_CS = 1456,			// 角色晋升请求
		MT_ROLE_CARRER_CHANGE_REQ_CS = 1457,			// 角色转职请求
		MT_ROLE_PROFESSION_EXTRA_INFO_REQ_CS = 1458,	// 角色职业额外信息请求
		MT_ROLE_CHANGE_COLOR_REQ_CS = 1459,				// 角色职业改变颜色请求

		MT_ROLE_SYSTEM_SET_REQ_CS = 1460,				// 玩家客户端设置请求
		MT_ROLE_SYSTEM_SET_INFO_SC = 1461,				// 玩家客户端设置信息
		MT_ROLE_SYSTEM_SET_CHAT_GROUP_CS = 1462,		// 玩家客户端设置群组请求
		MT_ROLE_SYSTEM_SET_CHAT_GROUP_INFO_SC = 1463,	// 玩家客户端设置群组信息

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		MT_KNAPSACK_INFO_ACK_SC = 1500,					// 请求背包信息返回
		MT_KNAPSACK_GRID_CHANGE_SC = 1501,				// 背包单个格子变化
		MT_USE_ITEM_SUC_SC = 1504,						// 使用物品成功
		MT_KNAPSACK_EXTEND_SC = 1505,					// 背包格子扩充结果
		MT_LACK_ITEM_SC = 1506,							// 缺少物品信息
		MT_ITEM_INTO_KNAPSACK_SPECIAL = 1507,			// 获得物品特殊通知
		MT_RAND_ACTIVITY_RESUILT_NOTICE_SC = 1508,		// 活动结果通知下发

		MT_KNAPSACK_OPERA_REQ_CS = 1550,				// 背包操作请求
		MT_USE_ITEM_CS = 1551,							// 使用物品
		MT_USE_OPTIONAL_GIFT_CS = 1552,					// 使用自选礼包
		MT_LACK_ITEM_NOTICE_FOR_TASK_COMMIT_SC = 1553,	// 缺少物品提交通知
		MT_COMMIT_TASK_ITEM_NOTICE_SC = 1554,			// 提交任务物品通知
		MT_USE_OPTIONAL_GIFT_2_CS = 1555,				// 使用自选礼包2
		MT_USE_GOLD_RECOVERY_CS = 1556,					// 使用元宝恢复
		MT_USE_RECOVERY_NOTICE_SC = 1557,				// 使用元宝恢复
		MT_KNAPSACK_ITEM_NOTICE_SC = 1558,				// 特殊获得道具飘字通知		
		MT_USE_SPECIAL_LOGIC_ITEM_CS = 1559,			// 使用需要特殊逻辑道具
		MT_ACTIVE_SPECIAL_LOGIC_ITEM_CS = 1560,			// 激活特殊逻辑道具
		MT_HEARSAY_INFO_CS = 1561,						// 请求下发传闻
		MT_BUY_CMD_GIFT_REQ_CS = 1562,					// 请求购买直购激活(礼包)
		MT_HONG_BAO_USE_RET_SC = 1563,					// 红包使用返回

		///////////////////////////////////////////////////////////////////// 装备 /////////////////////////////////////////////////////////////////////
		MT_EQUIPMENT_INFO_ACK_SC = 1600,				// 装备信息返回
		MT_EQUIPMENT_GRID_CHANGE_SC = 1601,				// 单个装备格子变化
		MT_ELEMENT_CRYSTAL_CHANGE_NOTCIE_SC = 1602,		// 元素水晶变更通知
		MT_EQUIP_VOC_LOCK_INFO_SC = 1603,				// 装备栏词条锁
		MT_EQUIPMENT_JEWERLY_UP_GREADE = 1604,			// 装备系统强化下发
		MT_EQUIP_JEWERLY_RESONACE_INFO_SC = 1605,		// 神装共鸣下发

		MT_ZAOHUA_EQUIP_OPERA_REQ_CS = 1640,			// 造化装备操作请求
		MT_ZAOHUA_BAG_LIST_SC = 1641,					// 造化背包list
		MT_ZAOHUA_BAG_SIGNLE_INFO_SC = 1642,			// 造化背包单个修改
		MT_ZAOHUA_WEAR_LIST_SC = 1643,					// 造化装备栏下发
		MT_ZAOHUA_WEAR_SIGNLE_SC = 1644,				// 造化装备栏单个下发
		MT_ZAOHUA_COMPOSE_RESULT_SC = 1645,				// 造化装备打造结果下发
		MT_ZAOHUA_WEAR_APPEARANCE_SC = 1646,			// 造化装备外观下发
		MT_ZAOHUA_DISCOMPOSE_CS = 1647,					// 造化装备列表分解
		MT_ZAOHUA_COMMON_INFO_SC = 1648,				// 造化装备通用下发

		MT_EQUIPMENT_OPERA_REQ_CS = 1650,				// 装备操作请求


		/////////////////////////////////////////////////////////////神印///////////////////////////////////////////////////////////////
		MT_PET_GOD_PRINT_SIGLE_SC = 1660,// 神印背包 - 单个修改
		MT_PET_GOD_PRINT_BAG_INFO_SC = 1661,// 神印背包		
		MT_PET_GOD_PRINT_EXP_INFO_SC = 1662,// 神印经验池 
		MT_PET_GOD_PRINT_WEAR_SIGLE_INFO_SC = 1663,	//神印穿戴方案变更
		MT_PET_GOD_PRINT_SET_WEAR_SIGLE_REQ_CS = 1664,	//设置神印穿戴方案
		MT_PET_GOD_PRINT_LOG_LIST_SC = 1665,	//神印祈神日志列表
		MT_PET_GOD_PRINT_LOG_DETAIL_INFO_SC = 1666,	//祈神日志详细下发
		MT_PET_GOD_PRINT_WISH_REWARD_SC = 1667,		//祈神奖励下发
		MT_PET_GOD_PRINT_USE_ITEM_UPLEVEL_CS = 1668,//神印使用神印升级

		///////////////////////////////////////////////////////////////////// 宠物/////////////////////////////////////////////////////////////////////
		MT_PET_FOLLOW_PET_INFO_SC = 1690,				// 宠物跟随信息
		MT_SEAL_PET_GET_ITEM_INFO_SC = 1691,				// 捕抓宠物得到什么物品
		MT_PET_ID_RECORD_LIST_SC = 1692,				// 宠物图鉴
		MT_ABANDON_PET_BY_LIST_CS = 1693,				// 批量遗弃宠物
		MT_MOVE_PET_GRID_NOTICE_SC = 1694,				// 移动宠物到仓库或背包通知
		MT_PET_SKIN_INFO_SC = 1695,						//  宠物皮肤
		MT_PET_CAL_ATTR_RET_SC = 1696,					//  
		MT_PET_SOUL_UPLEVEL_REQ_CS = 1697,				// 魂器升级
		MT_PET_BENEDICTION_ALL_INFO_SC = 1698,			// 宠物祝福列表信息下发
		MT_PET_BENEDICTION_SINGLE_INFO_SC = 1699,		// 单个宠物祝福信息下发

		///////////////////////////////////////////////////////////////////// 宠物2 /////////////////////////////////////////////////////////////////////
		MT_PET_OP_CS = 1700,							// 操作
		MT_PET_RENAME_CS = 1701,						// 改名
		MT_PET_ASSIGN_ATTRIBUTE_POINTS_CS = 1702,		// 分配属性点
		MT_SEND_PET_ALL_INFO_SC = 1703,					// 宠物所有的信息 只在登录时发一次
		MT_SEND_PET_SINGLE_INFO_SC = 1704,				// 单个宠物的信息
		MT_SEND_PET_OTHER_INFO_SC = 1705,				// 宠物其他信息
		MT_SEND_PET_DELETE_INFO_SC = 1706,				// 删除宠物
		MT_SEND_PET_ATTRS_INFO_SC = 1707,				// 宠物属性返回
		MT_SEND_PET_BREAKTHROUGHS_RET_SC = 1708,		// 宠物强化返回
		MT_SEND_PET_GET_NOTICE_SC = 1709,				// 获得宠物返回

		///////////////////////////////////////////////////////////////////// 伙伴 /////////////////////////////////////////////////////////////////////
		MT_PARTNER_HANDLE_REQUEST_CS = 1710,			// 伙伴系统操作请求
		MT_PARTNER_ALL_DATA_INFOR_SC = 1711,			// 所有伙伴列表信息
		MT_PARTNER_COMBATS_STATUS_SC = 1712,			// 伙伴战斗状态信息
		MT_PARTNER_ATTRIBUTE_LIST_SC = 1713,			// 伙伴属性列表信息
		MT_PARTNER_FETTERS_INFOMA_SC = 1714,			// 伙伴羁绊列表信息
		MT_PARTNER_ID_GRADE_STAGE_SC = 1715,			// 伙伴基础阶级信息
		MT_PARTNER_SKILL_SELECTOR_CS = 1716,			// 伙伴技能保存请求
		MT_PARTNER_SKILL_INFORMAT_SC = 1717,			// 伙伴当前技能信息
		MT_PARTNER_SYN_FIGHT_LIST_SC = 1718,			// 伙伴出战顺序列表
		MT_PARTNER_SYN_ALTER_POSI_CS = 1719,			// 伙伴出战列表调整

		///////////////////////////////////////////////////////////////////// 任务 /////////////////////////////////////////////////////////////////////
		MT_TASK_SINGLE_HANDLE_CS = 1720,				// 单个任务操作请求
		MT_TASK_INFORMAT_LIST_SC = 1721,				// 所有任务列表信息
		MT_TASK_DETELE_HANDLE_SC = 1722,				// 移除任务信息
		MT_TASK_TALK_WITH_NPC_CS = 1723,				// 与 NPC 交谈
		MT_TASK_GIVE_NPC_ITEM_CS = 1724,				// 给予 NPC 道具
		MT_TASK_COMPLETE_TASK_SC = 1725,				// 已完成的任务列表
		MT_TASK_CAN_ABANDONED_SC = 1726,				// 任务能否被放弃		0 - 不能； 1 - 可以
		MT_TASK_SINGLE_STATUS_SC = 1727,				// 单个任务当前状态   0 - 未进行； 1 - 已完成； 2 - 进行中
		MT_TASK_CHAIN_OP_CS		 = 1728,				// 任务链操作
		MT_TASK_CHAIN_INFO_SC	 = 1729,				// 任务链信息

		///////////////////////////////////////////////////////////////////// 工坊 /////////////////////////////////////////////////////////////////////
		MT_WORKSHOP_HANDLE_REQUEST_CS = 1730,			// 工坊系统操作请求
		MT_WORKSHOP_ALL_DATA_INFOR_SC = 1731,			// 工坊所有数据信息
		MT_WORKSHOP_INDIVID_RECIPE_SC = 1732,			// 单个配方激活信息
		MT_WORKSHOP_REFORGE_RESULT_SC = 1733,			// 灵饰重锻结果列表
		MT_WORKSHOP_GEM_UPGRA_MATE_SC = 1734,			// 宝石一键升级材料
		MT_WORKSHOP_EXC_SKILL_BOOK_CS = 1735,			// 兑换宠物技能书
		MT_WORKSHOP_COMPOSE_RESULT_SC = 1736,			// 工坊合成结果回复
		MT_WORKSHOP_EQUIP_VOC_CHANGE_CS = 1737,			// 装备更换词条
		MT_WORKSHOP_JEWELRY_TIMES_CS = 1738,			// 灵饰合成失败次数

		MT_PET_EQUIPMENT_REQ_CS = 1739,					// 宠物装备请求

		///////////////////////////////////////////////////////////////////// 灵饰 /////////////////////////////////////////////////////////////////////
		MT_JEWELRY_HANDLE_REQUEST_CS = 1740,			// 灵饰系统操作请求
		MT_JEWELRY_ALL_INFO_REPLY_SC = 1741,			// 灵饰所有信息回复
		MT_JEWELRY_SINGLE_CHANGES_SC = 1742,			// 灵饰单个改变通知

		/////////////////////////////////////////////////////////////////// 功能预告 ///////////////////////////////////////////////////////////////////
		MT_SYSTEM_NOTICE_GET_AWARD_CS = 1750,			// 功能预告领取奖励
		MT_SYSTEM_NOTICE_FLAG_INFO_SC = 1751,			// 功能预告领取信息

		/////////////////////////////////////////////////////////////////// 离线经验 ///////////////////////////////////////////////////////////////////
		MT_OFFLINE_EXP_ADD_INFO_SC = 1755,				// 离线经验累积信息
		MT_OFFLINE_EXP_RECEIVES_CS = 1756,				// 离线经验领取经验

		///////////////////////////////////////////////////////////////////// NPC /////////////////////////////////////////////////////////////////////
		MT_NPC_HANDLE_REQUEST_CS = 1760,				// NPC 模块操作请求
		MT_NPC_GATHER_TIMER_S_SC = 1761,				// NPC 模块采集时长
		MT_NPC_CANCEL_GATHERS_SC = 1762,				// NPC 模块取消采集
		MT_NPC_GATHER_NOTIFYS_SC = 1763,				// NPC 模块采集广播
		MT_NPC_GATHER_SUCCESS_SC = 1764,				// NPC 模块采集成功
		MT_NPC_INFO_NOTIFY_SC = 1765,					// NPC 模块信息发生改变
		MT_NPC_COMMIT_BACK_SC = 1766,					// NPC 模块交互信息返回
		MT_NPC_SHOP_INFO_SC = 1767,						// NPC 商店信息返回
		MT_NPC_SHOP_BUY_CS = 1768,						// NPC 购买请求

		/////////////////////////////////////////////////////////////////// 伙伴装备 ///////////////////////////////////////////////////////////////////
		MT_PARTNER_EQUIP_OPERATOR_CS = 1770,			// 伙伴装备 操作请求
		MT_PARTNER_EQUIP_SLOT_INF_SC = 1771,			// 伙伴装备 单个槽位信息

		/////////////////////////////////////////////////////////////////// 伙伴升品 ///////////////////////////////////////////////////////////////////
		MT_PARTNER_QUALITY_UP_SC = 1772,				// 伙伴升品信息
		MT_PARTNER_QUALITY_UP_INFO_CS = 1773,			// 伙伴升品请求
		
		/////////////////////////////////////////////////////////////////// 伙伴水晶 ///////////////////////////////////////////////////////////////////
		MT_PARTNER_USE_ELEMENT_CRYTAL_INFO_SC = 1774,			// 伙伴元素水晶装备
		MT_PARTNER_USE_ELEMENT_CRYTAL_TO_FIGHT_SC = 1775,		// 伙伴元素快速装备
		MT_PARTNER_ELEMENT_CRYTAL_REQ_CS = 1776,				// 伙伴元素水晶请求

		/////////////////////////////////////////////////////////////////// 伙伴皮肤 ///////////////////////////////////////////////////////////////////
		MT_PARTNER_SKIN_ACTIVE_SC = 1777,						// 伙伴皮肤激活总信息
		MT_PARTNER_SKIN_SINGLE_OPER_SC = 1778,					// 伙伴皮肤单个操作下发
		MT_PARTNER_SKIN_REQ_CS = 1779,							// 伙伴皮肤请求

		/////////////////////////////////////////////////////////////////// 成长之路 ///////////////////////////////////////////////////////////////////
		MT_COG_OPERATOR_REQUEST_CS = 1780,				// 成长之路 操作请求
		MT_COG_ALL_INFORMATIONS_SC = 1781,				// 成长之路 所有信息
		MT_COG_SINGLE_TASK_PROG_SC = 1782,				// 成长之路 单个任务 基本进度信息
		MT_COG_SINGLE_FLAG_STAT_SC = 1783,				// 成长之路 单个任务 奖励领取状态
		MT_COG_STAR_REWARD_STAT_SC = 1784,				// 成长之路 星级奖励 当前星级奖励状态
		MT_COG_SINGLE_SE_NOTIFY_SC = 1785,				// 成长之路 单个任务完成通知
		MT_ROLE_COURSE_ALL_INFO_SC = 1786,				// 成长之路 历程所有信息下发
		MT_ROLE_COURSE_SINGLE_INFO_SC = 1787,			// 成长之路 历程单条信息下发
		MT_ROLE_COURSE_NOTICE_INFO_SC = 1788,			// 成长之路 历程信息通知下发
		MT_SERVER_COURSE_ALL_INFO_SC = 1789,			// 成长之路 历程完成信息下发

		/////////////////////////////////////////////////////////////////// 颜值系统 ///////////////////////////////////////////////////////////////////
		MT_FSS_OPERATOR_REQUEST_CS = 1790,				// 颜值系统 - 操作请求
		MT_FSS_ALL_INFORMATIONS_SC = 1791,				// 颜值系统 - 所有信息
		MT_FSS_SINGLE_GRID_INFO_SC = 1792,				// 颜值系统 - 单个格子信息
		MT_FSS_SINGLE_RES_STATE_SC = 1793,				// 颜值系统 - 单个资源信息
		MT_FSS_LEVEL_EXPERIENCE_SC = 1794,				// 颜值系统 - 等级与经验值
		MT_FSS_TIME_SC			   = 1795,				// 颜值系统 - 时间戳信息
		MT_RES_TIME_ALL_SC		   = 1796,				// 颜值系统 - 激活的时间戳信息

		///////////////////////////////////////////////////////////////////// 声望 /////////////////////////////////////////////////////////////////////
		MT_PRESTIGE_ALL_VILLAGE_INFO_SC = 1800,			// 声望全部村子信息
		MT_PRESTIGE_ALL_SHOP_IINFO_SC = 1801,			// 声望全部商店信息
		MT_PRESTIGE_SINGLE_VILLAGE_INFO_SC = 1802,		// 声望单个村子的情报
		MT_PRESTIGE_SINGLE_SHOP_INFO_SC = 1803,			// 声望单个商店的情报
		MT_PRESTIGE_SHOPING_CS = 1804,					// 购买商品
		MT_PRESTIGE_DONATE_INFO_SC = 1805,				// 捐赠信息
		MT_PRESTIGE_DONATE_OP_CS = 1806,				// 捐赠操作
		MT_PRESTIGE_DONATE_NOTICE_SC = 1807,			// 捐赠通知
		MT_PRESTIGE_CHANGE_NOTICE_SC = 1808,			// 声望变动通知

		///////////////////////////////////////////////////////////////////// 引导 /////////////////////////////////////////////////////////////////////
		MT_FUNCGUIDE_OPERATOR_REQUEST_CS = 1830,		// 功能引导 - 操作请求
		MT_FUNCGUIDE_EVENT_FLAG_INFOR_SC = 1831,		// 功能引导 - 事件标记												
		MT_PET_DEAD_GUIDE_TRIGGER_CS = 1832,			// 宠物死亡召唤指引触发
		MT_PET_DEAD_GUIDE_INFO_SC = 1833,				// 宠物死亡召唤指引数据下发

		//////////////////////////////// 兑换商店(百货) ///////////////////////////////////////////
		MT_CONVERT_SHOP_BUY_REQ_CS = 1850,
		MT_CONVERT_SHOP_BUY_RECORD_LIST_SC = 1851,
		MT_CONVERT_SHOP_ADD_BUY_RECORD_SC = 1852,

		//////////////////////////////// Boss Chapter /////////////////////////////////////////////
		MT_BOSS_CHAPTER_INFO_SC = 1860,
		MT_BOSS_CHAPTER_REQ_CS = 1861,

		//////////////////////////////// 功能预告 /////////////////////////////////////
		MT_FUNC_PREVIEW_INFO_SC = 1870,
		MT_FUNC_PREVIEW_CLAIM_REWARD_CS = 1871,

		/////////////////////////////// 境界系统 //////////////////////////////////////
		MT_REALM_BREAK_REQ_CS = 1880,					// 请求境界系统相关操作
		MT_REALM_BREAK_ALL_INFO_SC = 1881,				// 境界系统全部信息下发
		//MT_REALM_BREAK_START_TUNA_SC = 1882,			// 境界系统开始吐纳下发
		MT_REALM_BREAK_ITEM_INFO_SC = 1883,				// 境界系统道具信息下发

		/////////////////////////////地图探索//////////////////////////////
		MT_MAP_EXPLORATION_REQ_CS = 1890,				// 地图探索请求
		MT_MAP_EXPLORATION_INFO_SC = 1891,				// 地图探索下发
		///////////////////////////////// 称号 /////////////////////////////////////////////////////////////////////
		MT_TITLE_ALL_INFO_SC = 1900,					// 称号全部信息
		MT_TITLE_SINGLE_INFO_SC = 1901,					// 称号单个信息
		MT_TITLE_OTHER_INFO_SC = 1902,					// 称号其他信息
		MT_TITLE_OP_CS = 1903,							// 称号操作
		MT_TITLE_CHANGE_CS= 1904,						// 场景称号更改
		MT_TITLE_NOTICE_SC = 1905,						// 称号信息通知

		///////////////////////////////////////////////////////////////////// 修炼 /////////////////////////////////////////////////////////////////////
		MT_TRAIN_ALL_INFO_SC = 1920,					// 修炼信息
		MT_TRAIN_REQ_CS = 1921,							// 修炼请求
		MT_ROLE_PET_TRAIN_SKILL_REQ_CS = 1922,			// 修炼 宠物属性技能
		MT_TRAIN_INFO_PROMOTED_SC = 1923,				// 修炼技能已提升信息下发
		MT_TRAIN_RED_DOT_FLAG_SC = 1924,				// 修炼技能红点标识下发

		/////////////////////////////////////////////////////////////////// 家族任务 ///////////////////////////////////////////////////////////////////
		MT_GUILD_WEEK_TASK_ALL_INFOR_CS = 1930,			// 家族任务 - 所有信息
		MT_GUILD_WEEK_TASK_RANK_LIST_CS = 1931,			// 家族任务 - 排行信息

		///////////////////////////////////////////////////////////////////// 藏品 /////////////////////////////////////////////////////////////////////
		MT_COLLECTION_ALL_INFO_SC = 1950,				// 藏品所有信息
		MT_COLLECTION_SINGLE_INFO_SC = 1951,			// 藏品单个信息
		MT_COLLECTION_OP_CS = 1952,						// 藏品操作
		MT_COLLECTION_INCLUDED_SC = 1953,				// 藏品收录
		MT_COLLECTION_INCLUDED_SIGNLE_SC = 1954,		// 藏品单独收录下发
		MT_COLLECTION_ATLAS_CARD_INFO_SC = 1955,		// 藏品图鉴卡下发
		MT_COLLECTION_ATLAS_CARD_SINGLE_SC = 1956,		// 藏品图鉴卡单独下发

		///////////////////////////////////////////////////////////////// 万事通的考验 /////////////////////////////////////////////////////////////////
		MT_WST_QUESTION_ALL_INFO_SC = 1960,
		MT_WST_QUESTION_OP_CS = 1961,
		MT_WST_QUESTION_OVER_INFO_SC = 1962,

		/////////////////////////////////////////////////////////////////// 奖励界面 ///////////////////////////////////////////////////////////////////
		MT_ONLINE_REWARD_INFO_SC = 1970,				// 在线奖励信息
		MT_SIGN_REWARD_INFO_SC = 1971,					// 签到奖励信息
		MT_LEVEL_REWARD_INFO_SC = 1972,					// 等级奖励信息
		MT_FETCH_REWARD_REQ_CS = 1973,					// 领取奖励
		MT_FIND_REWARD_REQ_CS = 1974,					// 奖励找回
		MT_FIND_REWARD_INFO_SC = 1975,					// 奖励找回信息
		MT_SCORE_REWARD_INFO_SC = 1976,					// 评分奖励信息
		MT_MONTH_INVESTMENT_INFO = 1977,				// 月度投资信息
		MT_REWARD_NOTIFY_REQUEST_CS = 1978,				// 奖励界面找回功能取消红点提示
		MT_REWARD_NOTIFY_INFO_SC = 1979,				// 奖励界面找回功能红点下发

		///////////////////////////////////////////////////////////////////// 幻化 /////////////////////////////////////////////////////////////////////
		MT_SURFACE_ALL_INFO_SC = 1980,					// 幻化全部信息
		MT_SURFACE_OP_CS = 1981,						// 幻化操作
		MT_SURFACE_SINGLE_INFO_SC = 1982,				// 幻化单条信息
		MT_SURFACE_WEAR_INFO_SC = 1983,					// 幻化穿戴信息
		MT_SURFACE_DECOMPOSE_CS = 1984,					// 时装分解(通用物品分解)

		MT_FASHION_COLLECTION_REQ_CS = 1985,			// 时装升级操作
		MT_FASHION_COLLECTION_INFO_SC = 1986,			// 时装升级全部信息

		// ------------- 奖励界面 ------------
		MT_DAY_REWARD_ONLINE_GIFT_INFO_SC = 1990,		// 奖励界面-在线礼包信息下发

		////////////////////////////////////////////////////////////////// 其他小系统 //////////////////////////////////////////////////////////////////
		MT_GM_COMMAND_SC = 2000,
		MT_EXP_CRYSTAL_OPEN_CS = 2001,					// 打卡水晶开关
		MT_EXP_CRYSTAL_INFO_SC = 2002,					// 打卡水晶信息
		MT_TREVI_FOUNTAIN_OPEN_CS = 2003,				// 许愿
		MT_TREVI_FOUNTAIN_INFO_SC = 2004,				// 许愿池信息
		MT_ENCOUNTER_MINE_MONSTER_FLAG_SC = 2005,		// 遭遇暗怪设置返回
		MT_SET_BATTLE_POSITION_SC = 2006,				// 设置主角站位返回
		MT_SET_BATTLE_AUTO_INFO_SC = 2007,				// 设置战斗自动信息返回
		MT_TASK_SET_TRACE_REQ_CS = 2008,				// 设置任务追踪
		MT_TASK_TRACE_INFORMA_SC = 2009,				// 任务追踪信息
		MT_REFER_ROLE_EQUIPMENT_CS = 2010,				// 人物装备属性查询
		MT_ROLE_QUERY_INFOR_SC = 2011,					// 人物装备属性下发
		MT_ROLE_SALARY_INFO_CS = 2012,					// 主角工资信息请求
		MT_ROLE_SALARY_INFO_SC = 2013,					// 主角工资信息下发
		MT_LOUD_SPEAKER_CS = 2014,						// 使用喇叭全服广播
		MT_SPEAKER_INFO_SC = 2015,						// 喇叭消息
		MT_TASK_CHAIN_MONSTER_SC = 2016,				// 任务链 - 刷出怪物通知
		MT_PATROL_CS = 2017,							// 巡逻设置请求
		MT_NEW_CHONG_ZHI_CONFIG_INFO_CS = 2018,			// 新充值表信息请求
		MT_TASK_PATH_FINDING_CS = 2019,					// 玩家任务寻路通知服务端
		MT_TASK_PATROL_CS = 2020,						// 巡逻任务下发
		MT_TASK_SKIP_BATTLE_CS = 2021,					// 任务跳过战斗
		MT_PARTNER_SINGLE_INFO_SC = 2022,				// 下发伙伴单条信息
		MT_PARTNER_GIFT_TREE_LIST_INFO_SC = 2023,		// 下发伙伴天赋树信息
		MT_PARTNER_SINGLE_GIFT_TREE_INFO_SC = 2024,		// 下发伙伴单条天赋树信息
		MT_ROLE_QUERY_SIMPLE_INFO_SC = 2025,			// 人物简易数据下发
		MT_TASK_GAME_PLAY_CS = 2026,					// 任务小游戏
		MT_SERVER_COURSE_SINGLE_INFO_SC = 2027,			// 成长之路 历程完成单条信息下发
		MT_PARTNER_SOAR_NOTICE_SC = 2028,				// 下发伙伴飞升相关通知
		MT_TASK_COMMIT_MONEY_CS = 2029,					// 提交货币

		MT_SET_ENCOUNTER_MINE_MONSTER_FLAG_CS = 2030,	// 设置遭遇暗怪
		MT_SET_BATTLE_POSITION_CS = 2031,				// 设置主角站位
		MT_SET_BATTLE_AUTO_CS = 2032,					// 设置战斗自动
		MT_SET_BATTLE_AUTO_MOVE_CS = 2033,				// 设置战斗自动行为
		MT_CHANGE_APPEARANCE_CS = 2034,					// 改变形象
		MT_CHANGE_HEADSHOT_CS = 2035,					// 角色改变头像请求
		MT_ATTR_LIST_REASON_SC = 2036,					// 属性来源列表下发
		MT_SIGNLE_ATTR_REASON_REQ_CS = 2037,			// 请求属性来源列表下发

		MT_TREASURE_CHEST_REWARD_LIST_SC = 2039,		// 通知玩家宝箱奖励
		MT_NOTICE_NEARBY_OPEN_TREASURE_CHEST_SC = 2040,	// 通知附近宝箱被打开
		MT_NOTICE_CFG_UPDATE_SC = 2041,					// 公告配置更新通知
		MT_NEW_CHONG_ZHI_CONFIG_INFO_SC = 2042,			// 新充值表下发信息
		MT_NEW_CHONG_ZHI_CONFIG_RELOAD_NOTICE_SC = 2043,// 新充值表重读

		MT_PARTNER_SWITCH_FIGHT_POS_CS = 2044,			// 伙伴互换出战位置
		MT_ROLE_SALARY_DAY_SC = 2045,					// 主角每天的侠义值与工资
		MT_ROLE_LOGIN_SYSTEM_INFO_SC = 2046,			// 累计登陆下发
		MT_ROLE_LOGIN_SYSTEM_REQ_CS = 2047,				// 累计登陆请求
		MT_ROLE_SPECIAL_NOTICE_SC = 2048,				// 特殊信息系统栏通知

		MT_PARTNER_SKILL_ACTIVATE_CS = 2049,			// 伙伴技能启用、禁用

		// 加在这中间

		MT_GM_COMMAND_CS = 2050,		

		MT_HEARSAY_INFO_SC = 2059,						// 传闻提示信息下发
		MT_BABY_INFO_SC = 2060,							// 子女信息
		MT_BABY_REQ_CS = 2061,							// 子女请求操作
		MT_BABY_RENAME_CS = 2062,						// 子女重命名
		MT_BABY_PLAY_ROUTE_SC = 2063,					// 询问开始小游戏
		MT_BABY_START_PLAY_SC = 2064,					// 小游戏开始
		MT_BABY_ACTIVE_CS = 2065,						// 激活子女
		MT_ROLE_HISTORY_CHIVALROUS = 2066,				// 历史侠义值下发
		MT_ROLE_HISTORY_CHIVALROUS_REWARD_FETCH = 2067,	// 历史侠义值奖励领取请求

		MT_FRIEND_REQ_CS = 2070,						// 好友操作请求
		MT_FRIEND_INFO_SC = 2071,						// 好友信息		
		MT_FRIEND_LIST_CHANGE_SC = 2072,				// 好友 列表变化通知
		MT_ADD_FRIEND_ROUTE_SC = 2073,					// 好友 添加转发
		MT_ASK_ADD_FRIEND_LIST_SC = 2074,				// 添加好友请求列表
		MT_BLACK_LIST_CHANGE_SC = 2075,					// 黑名单 列表变化通知
		MT_BLACK_INFO_SC = 2076,						// 好友信息	
		MT_FRIEND_EDIT_GRUOP_CS = 2077,					// 创建分组
		MT_FRIEND_MOVE_GRUOP_CS = 2078,					// 移动好友到哪个分组
		MT_FRIEND_RENAME_GRUOP_CS = 2079,				// 重命名分组
		MT_FRIEND_GRUOP_INFO_SC = 2080,					// 好友分组信息
		MT_RECENTLY_TEAM_INFO_SC = 2081,				// 最近组队信息
		MT_ONE_RECENTLY_TEAM_INFO_SC = 2082,			// 最近组队信息改变
		MT_RECENTLY_CHAT_CHANGE_SC = 2083,				// 最近联系人改变
		MT_RECENTLY_CHAT_INFO_SC = 2084,				// 最近联系人信息
		MT_REPLY_ROLE_IS_ONLINE_SC = 2085,				// 回复玩家是否在线

		MT_RECOMMEND_FRIEND_LIST_REQ_CS = 2090,			// 请求推荐列表
		MT_RECOMMEND_FRIEND_LIST_SC = 2091,				// 返回推荐列表
		MT_ADD_RECOMMEND_FRIEND_CS = 2092,				// 请求添加好友推荐
		MT_RECOMMEND_FRIEND_ONE_SC = 2093,				// 返回一个好友推荐
		MT_REFRESH_RECOMMEND_FRIEND_CS = 2094,			// 请求推荐列表换一批 
		MT_FUZZY_SEARCH_ROLE_INFO_CS = 2095,			// 请求模糊查询所有角色
		MT_FUZZY_SEARCH_ROLE_INFO_SC = 2096,			// 模糊查询所有角色信息
		MT_GIVE_FLOWER_NOTICE = 2097,					// 送花通知

		MT_SVIP_CLIENT_CUSTOM_DATA_SC = 2098,			// SVIP客户端自定义数据下发
		MT_SVIP_CLIENT_CUSTOM_DATA_CS = 2099,			// SVIP客户端自定义数据请求

		//灵骑系统
		MT_SMART_MOUNTS_SYSTEM_REQ_CS = 2100,			// 灵骑系统请求
		MT_SMART_MOUNTS_SYSTEM_ALL_INFO_SC = 2101,		// 灵骑系统信息下发
		MT_SMART_MOUNTS_SYSTEM_SINGLE_INFO_SC = 2102,	// 灵骑系统单个信息下发
		MT_SMART_MOUNTS_SYSTEM_ATTR_LIST_SC = 2103,		// 灵骑基础属性列表下发
		MT_SMART_MOUNTS_SYSTEM_DOMINANCE_INFO_SC = 2104,// 灵骑统御单个信息下发
		MT_SMART_MOUNTS_SYSTEM_SHOW_INFO_SC = 2105,		// 灵骑弹窗界面展示信息下发
		MT_SMART_MOUNTS_SYSTEM_REFINE_SKILL_SC = 2106,	// 灵骑洗练技能结果下发
		MT_SMART_MOUNTS_SYSTEM_OTHER_INFO_SC = 2107,	// 灵骑其它信息下发
		MT_SMART_MOUNTS_SYSTEM_RELATED_INFO_SC = 2108,	// 灵骑相关信息下发
		MT_SMART_MOUNTS_SYSTEM_SKILL_REQ_CS = 2109,		// 灵骑系统技能请求

		MT_SHITU_OPERA_MAKE_RELATION_REQ_CS = 2110,		// 师徒系统   拜师或收徒 请求
		MT_SHITU_OPERA_MAKE_RELATION_AKC_CS = 2111,		// 师徒系统   回复拜师或收徒 请求
		MT_SHITU_OPERA_BREAK_RELATION_REQ_CS = 2112,	// 师徒系统   毕业或出师 请求
		MT_SHITU_OPERA_MAKE_RELATION_ROUTE_SC = 2113,	// 师徒系统   拜师或收徒 转发
		MT_SHITU_OPERA_RELATION_DATA_SC = 2114,			// 师徒系统   师徒信息
		MT_SHITU_OPERA_BREAK_RELATION_ROUTE_SC = 2115,	// 师徒系统   毕业或出师 转发
		MT_SHITU_OPERA_BREAK_RELATION_AKC_CS = 2116,	// 师徒系统   回复 毕业或出师 
		MT_SHITU_OPERA_REQ_CS = 2117,					// 师徒系统   请求操作
		MT_SHITU_FB_SCENE_INFO_SC = 2118,				// 师徒副本信息
		MT_SHITU_COMMON_INFO_SC = 2119,				// 师徒通用信息
		MT_SHITU_INVITATION_NOTICE_SC = 2120,		//师徒副本组队邀请
		MT_SHITU_FINISH_NOTICE_SC = 2121,		//师徒副本结束通知
		MT_SHITU_BATTLE_RESULT_SC = 2122,		//师徒副本战斗结果通知
		
		MT_SHITU_PREFERENCES_INFO_SC = 2124,		//师徒系统 偏好设置信息
		MT_SHITU_SEEKING_REQ_CS = 2125,					// 师徒系统 宣言
		MT_SHITU_SEEKING_INFO_SC = 2126,				// 师徒系统 信息

		MT_SERVER_FIRST_KILL_REQ_CS = 2127,			// 全服首杀  信息请求
		MT_SERVER_FIRST_KILL_AKC_SC = 2128,			// 全服首杀  信息下发

		//--------------------------聊天--------------------------------------
		MT_CHANNEL_CHAT_CS = 2130,						// 请求频道聊天
		MT_SINGLE_CHAT_CS = 2131,						// 请求私聊
		MT_CHANNEL_CHAT_SC = 2132,						// 频道聊天返回
		MT_SINGLE_CHAT_SC = 2133,						// 私聊返回
		MT_SINGLE_CHAT_USET_NOT_EXIST_SC = 2134,		// 私聊 提示用户不在线
		MT_CHAT_ITEM_INFO_REQ_CS = 2135,				// 聊天中物品信息请求
		MT_CHAT_ITEM_INFO_RET_SC = 2136,				// 聊天中物品详细信息返回
		MT_REALTIME_COMMENT_CS = 2137,					// 弹幕发送请求
		MT_REALTIME_COMMENT_SC = 2138,					// 弹幕回包
		MT_REPLY_WHETHER_FORBIDEN_SC = 2139,			// 返回玩家是否被禁言

		MT_COURAGE_CHALLENGE_REQ_CS = 2140,				// 勇气挑战 操作
		MT_COURAGE_CHALLENGE_ALL_INFO_SC = 2141,			// 勇气挑战 所有信息
		MT_COURAGE_CHALLENGE_FIRST_KILL_INFO_SC = 2142,	// 勇气挑战 首杀信息
		MT_COURAGE_CHALLENGE_SINGLE_INFO_SC = 2143,			// 勇气挑战 单条信息
		MT_COURAGE_CHALLENGE_SCORE_SINGLE_INFO_SC = 2144,	// 勇气挑战 评分单条信息

		MT_MONSTER_WAVE_INFO_SC = 2145,					// 魔窟信息
		MT_MONSTER_WAVE_FINISH_INFO_SC = 2146,			// 魔窟完成信息

		MT_CHAT_RECORD_LIST_SC = 2147,					// 最近聊天记录
		MT_CHAT_RECORD_SET_CS = 2148,					// 最近聊天记录设置
		MT_CHAT_SIMPLE_RECORD_LIST_SC = 2149,			// 快捷发言记录
		MT_CHAT_SIMPLE_RECORD_SET_CS = 2150,			// 快捷发言记录设置
		MT_DELETE_MUTE_PLAYER_CHAT_RECORD_SC = 2151,	// 删除涉及某玩家的聊天记录
		MT_COURAGE_CHALLENGE_PET_HELPER_NOTICE_SC = 2152,	// 勇气挑战（锢魔之塔）宠物助战通知
		MT_USER_CHAT_RECORD_CS = 2153,					// 查询用户的聊天记录

		MT_MEDAL_REQ_CS = 2160,							// 勋章请求
		MT_MEDAL_INFO_SC = 2161,						// 勋章信息
		MT_MEDAL_FB_SCENE_INFO_SC = 2162,				// 勋章副本信息
		MT_MEDAL_FB_KILL_BOSS_SC = 2163,				// 勋章副本击杀boss
		MT_MEDAL_DECOMPOSE_NOTICE_SC = 2164,			// 勋章分解通知
		MT_MEDAL_TASK_PLAY_VIDEO_CS = 2165,				// 勋章任务播放动画
		MT_MEDAL_LOTTREY_INFO_SC = 2166,				// 勋章抽奖下发
		MT_MEDAL_TAKE_OFF_SC = 2167,					// 勋章脱下
		MT_MEDAL_UPGRADE_NOTICE = 2168,					// 勋章进阶
		MT_MEDAL_SPECIAL_EXCHANGE = 2169,				// 天赐勋章置换通知
		MT_MEDAL_ONE_KEY_GRADE_CS = 2170,				// 令牌一键升华
		MT_MEDAL_UP_GRADE_SPECIAL = 2171,				// 天赐令牌升华

		MT_AUTO_CATCH_PET_CS = 2175,					// 自动抓宠设置请求
		MT_AUTO_CATCH_PET_SC = 2176,					// 自动抓宠设置信息

		MT_ROLE_GIFT_GATHER_DREAM_LIST_SC = 2177,		// 玩家今天集愿赠予信息
		MT_ROLE_CAN_PUBLISH_GATHER_DREAM_LIST_SC = 2178,// 玩家可发布的集愿信息列表
		MT_ROLE_GAHTER_DREAM_RECORD_INFO_LIST_SC = 2179,// 玩家集愿记录列表下发

		/////////////////////////////////////////////////////////////////// 门派任务 ///////////////////////////////////////////////////////////////////
		MT_SCHOOL_TASK_ALL_INFO_SC = 2180,
		MT_SCHOOL_TASK_SINGLE_INFO_SC = 2181,
		MT_SCHOOL_TASK_REQ_CS = 2182,
		MT_BOUNTY_TASK_TURN_TABLE_FETCH_REWARD_CS = 2183,
		MT_BOUNTY_TASK_TURN_TABLE_INFO_SC = 2184,
		MT_BOUNTY_TASK_INFO_SC = 2185,

		////////////////////////////////////////////////////////////////// 真中の研究 //////////////////////////////////////////////////////////////////
		MT_RESEARCH_TASK_OPERATOR_CS = 2190,			// 真中研究操作请求
		MT_RESEARCH_TASK_ALL_INFO_SC = 2191,			// 真中研究所有信息
		MT_RESEARCH_TASK_AUTO_START_CS = 2192,			// 真中研究自动开始

		/////////////////////////////////////////////////////////////////// 世界BOSS （异族boss）///////////////////////////////////////////////////////////////////
		MT_WROLD_BOSS_REQ_CS = 2200,					// 世界BOSS 操作请求
		MT_WROLD_BOSS_INFO_SC = 2201,					// 世界BOSS 信息
		MT_WROLD_BOSS_RESULT_SC = 2202,					// 世界BOSS 活动结束奖励结算信息
		MT_WROLD_BOSS_POS_INFO_SC = 2203,				// 世界BOSS 位置信息
		MT_WROLD_BOSS_BATTLE_INFO_SC = 2204,				// 世界BOSS 每次战斗结束一些数据信息
		MT_WROLD_BOSS_ROLE_INFO_SC = 2205,				// 世界BOSS 玩家信息
		MT_WROLD_BOSS_RANK_INFO_SC = 2206,				// 世界BOSS 排行榜信息

		//-------------------------------- 宝图 -----------------------------------------
		MT_TREASURE_MAP_REQ_CS = 2210,					// 宝图请求
		MT_TREASURE_MAP_INFO_SC = 2211,					// 宝图信息
		MT_TREASURE_MAP_ITEM_INFO_SC = 2212,			// 藏宝图信息
		MT_TREASURE_MAP_ITEM_OPEN_RESULT_SC = 2213,		// 藏宝图打开获得物品信息
		MT_TREASURE_MAP_BATCH_USED_RESULT_SC = 2214,	// 高级藏宝图批量使用获得物品信息

		//-------------------------------- 运镖 -----------------------------------------
		MT_ESCORT_ALL_INFO_SC = 2220,					// 运镖信息
		MT_ESCORT_REQ_CS = 2221,						// 运镖操作
		MT_ESCORT_RESULT_NOTIC_SC = 2222,				// 运镖结果通知
		MT_ESCORT_STATUS_CHANGE_NOTICE_AREA_SC = 2223,	// 运镖状态改变通知

		////////////////////////////////////// 地图解锁 //////////////////////////////////////
		MT_MAP_UNLOCK_ENTIRE_ACTIVE_FLAG_LISTS_SC = 2230,	// 地图解锁 所有解锁标记列表信息
		MT_MAP_UNLOCK_FIRST_ENTER_SCENE_NOTICE_SC = 2231,	// 地图解锁 首次进入解锁场景通知
		MT_MAP_UNLOCK_REMOVE_UNLOCK_ANIMATIONS_CS = 2232,	// 地图解锁 移除地图首次解锁动画
		MT_MAP_UNLOCK_FIRST_ENTER_FB_TYPE_CS = 2233,		// 地图解锁 是否第一次进入副本类型

		////////////////////////////////////// 挑战副本 //////////////////////////////////////
		MT_CHALLENGE_FB_ROLE_INFOR = 2240,					// 挑战副本 - 个人信息
		MT_CHALLENGE_FB_AWARD_LIST = 2241,					// 挑战副本 - 副本奖励

		////////////////////////////////////// 神秘试炼 /////////////////////////////////////
		MT_MYSTERIOUS_TRIAL_CS = 2250,						// 神秘试炼 - 请求(七天活动)
		MT_MYSTERIOUS_TRIAL_INFO_SC = 2251,					// 神秘试炼 - 信息下发
		MT_MYSTERIOUS_TRIAL_SINGLE_INFO_SC = 2252,			// 神秘试炼 - 单个信息下发
		MT_MYSTERIOUS_TRIAL_FINALLY_REWARD_SC = 2253,		// 神秘试炼 - 最终奖励信息下发

		//////////////////////////////////// 购买任务 - 丝绸之路 ////////////////////////////
		MT_TRANSACTION_TASK_CS = 2255,						// 购买任务 - 丝绸之路请求
		MT_TRANSACTION_TASK_INFO_SC = 2256,					// 购买任务 - 丝绸之路信息下发

		////////////////////////////////////// 活跃系统 ////////////////////////////////////
		MT_ACTIVE_SYSTEM_CS = 2257,							// 活跃系统 - 信息请求
		MT_ACTIVE_SYSTEM_INFO_SC = 2258,					// 活跃系统 - 信息下发

		////////////////////////////////////// 一键完成 ////////////////////////////////////
		MT_ONE_KEY_FINISH_SC = 2259,

		////////////////////////////////////// 翻牌请求 ////////////////////////////////////
		MT_POKER_REWARD_CS = 2260,

		MT_ACTIVE_SYSTEM_SINGLE_INFO_SC = 2264,				// 活跃系统 - 单个任务下发
		MT_ACTIVE_SYSTEM_EXTRA_REWARD_FETCH_CS = 2265,		// 活跃系统 - 领取奖励
		MT_ACTIVE_SYSTEM_SPECIAL_ACTIVE_INFO_SC = 2266,		// 特殊活跃信息下发

		///////////////////////////////////// 心法 /////////////////////////////////////
		MT_HEART_SKILL_REQ_CS = 2270,						// 心法请求
		MT_HEART_SKILL_ALL_INFO_SC = 2271,					// 心法所有信息下发	
		MT_HEART_SKILL_SINGLE_INFO_SC = 2272,				// 心法单个信息下发
		MT_HEART_SKILL_OTHER_INFO_SC = 2273,				// 心法其它信息下发
		MT_HEART_SKILL_DISPLACE_REQ_CS = 2274,				// 心法置换合成请求

		///////////////////////////////////// 月卡周卡 ////////////////////////////////////
		MT_MONTH_CARD_REQ_CS = 2280,						// 月卡周卡请求
		MT_MONTH_CARD_INFO_SC = 2281,						// 月卡周卡信息下发

		////////////////////////////////// 声望任务 ///////////////////////////////////////
		MT_PRESTIGE_TASK_INFO_SC = 2290,					// 声望任务信息下发
		MT_PRESTIGE_TASK_SELECT_VILLAGE_CS = 2291,			// 选择村庄
		MT_PRESTIGE_TASK_FINAL_NOTICE = 2292,				// 声望任务最后通知
		MT_PRESTIGE_TASK_REQ_CS = 2293,						// 声望任务请求

		////////////////////////////////// 交易所 ///////////////////////////////////////////
		MT_TRADE_MARKET_CS_BEGIN = 2300,

		MT_ADD_ITEM_ON_SHELVE_CS = 2301,					// 上架
		MT_REMOVE_ITEM_ON_SHELVE_CS = 2302,					// 下架
		MT_CASH_OUT_CS = 2303,								// 提现
		MT_TRADE_MARKET_USER_SHELVE_DATA_CS = 2304,			// 个人交易所数据请求
		MT_TRADE_MARKET_SEARCH_BY_ITEM_ID_CS = 2305,		// 按物品ID搜索交易所商品请求
		MT_TRADE_MARKET_SEARCH_BY_ITEM_SUB_TYPE_CS = 2306,	// 按物品类型搜索交易所商品请求
		MT_TRADE_MARKET_BUY_ITEM_CS = 2307,					// 购买
		MT_RECENT_TRANSACTION_RECORDS_CS = 2308,			// 查询最近交易记录
		MT_TRADE_MARKET_GET_FOCUS_LIST_CS = 2309,			// 获取关注列表
		MT_TRADE_MARKET_ADD_FOCUS_CS = 2310,				// 添加关注
		MT_TRADE_MARKET_REMOVE_FOCUS_CS = 2311,				// 取消关注
		MT_TRADE_MARKET_GET_FOCUS_ITEM_INFO_CS = 2312,		// 获取关注物品的最新数据
		MT_TRADE_MARKET_ADVERTISE_CS = 2313,				// 发广告
		MT_TRADE_MARKET_SEARCH_ADVERTISE_CS = 2314,			// 查找广告的商品
		MT_TRADE_MARKET_GET_SELLING_LIST_CS = 2315,			// 查询某物品正在售卖的列表
		MT_TRADE_MARKET_ROLE_GET_TRADE_RECORD_CS = 2316,	// 获取玩家交易记录请求（购买、出售）
		MT_TRADE_MARKET_REDO_ADD_ITEM_ON_SHELVE_CS = 2317,	// 重新上架
		MT_TRADE_MARKET_SEARCH_PET_FOR_QUICK_BUY_CS = 2318,	// 宠物快捷购买搜索
		MT_TRADE_MARKET_GET_MERCHANDISE_NUM_CS = 2319,		// 请求商品数量
		MT_TRADE_MARKET_SEARCH_MEDAL_BY_SPE_TYPE_AND_COLOR_CS = 2320,	// 按颜色和特效类型查找令牌
		MT_TRADE_MARKET_QUERY_ITEM_RECOMMEND_PRICE = 2321,	// 请求推荐价格

		MT_TRADE_MARKET_CS_END = 2350,

		MT_TRADE_MARKET_SC_BEGIN = MT_TRADE_MARKET_CS_END,
		MT_TRADE_MARKET_USER_SHELVE_DATA_SC = 2351,			// 个人交易所数据返回
		MT_TRADE_MARKET_SEARCH_RESULT_SC = 2352,			// 搜索数据返回
		MT_RECENT_TRANSACTION_RECORDS_SC = 2353,			// 返回最近交易记录
		MT_TRADE_MARKET_GET_FOCUS_LIST_SC = 2354,			// 返回关注列表
		MT_TRADE_MARKET_GET_FOCUS_ITEM_INFO_SC = 2355,		// 返回关注物品的最新数据
		MT_TRADE_MARKET_GET_SELLING_LIST_SC = 2356,			// 查询某物品正在售卖的列表返回
		MT_TRADE_MARKET_ROLE_GET_TRADE_RECORD_SC = 2357,	// 获取玩家交易记录返回（购买、出售）
		MT_TRADE_MARKET_SEARCH_PET_FOR_QUICK_BUY_SC = 2358,	// 宠物快捷购买搜索返回
		MT_TRADE_MARKET_REFRESH_NOTICE = 2359, 				// 刷新通知
		MT_TRADE_MARKET_GET_MERCHANDISE_NUM_SC = 2360,		// 商品数量下发
		MT_TRADE_MARKET_CREDIT_INFO_SC = 2361,				// 交易信用信息下发
		MT_TRADE_MARKET_QUERY_ITEM_RECOMMEND_PRICE_RESP = 2362,	// 请求物品推荐价格返回 

		MT_TRADE_MARKET_SC_END = 2400,

		//--------------------------法宝--------------------------------------
		MT_FABAO_SEND_INFO_SC = 2450,				// 下发法宝信息
		MT_FABAO_REQ_CS = 2451,						// 请求法宝操作
		MT_FABAO_SEND_SIGNL_INFO_SC = 2452,			// 下发单个法宝修改信息
		MT_FABAO_SEND_EQUIP_BAR_INFO_SC = 2453,		// 下发法宝装备栏修改信息
		MT_FABAO_DRAW_INFO_SC = 2454,		// 法宝抽奖信息
		MT_FABAO_DRAW_REWARD_INFO_SC = 2455,		// 法宝抽奖奖励信息

		//--------------------------水晶副本--------------------------------------
		MT_CRYSTAL_FB_ALL_NPC_SC = 2459,			// 水晶副本所有NPC信息
		MT_CRYSTAL_FB_INFO_SC = 2460,				// 水晶副本信息
		
		MT_CRYSTAL_FB_REQ_CS = 2462,				// 水晶副本请求
		MT_CRYSTAL_FB_USE_ITEM_INFO_SC = 2463,		// 水晶副本使用物品信息
		MT_CRYSTAL_FB_OBJ_INFO_SC = 2464,			// 水晶副本使各个对象信息

		//--------------------------进阶副本---------------------------------------
		MT_ADVANCE_FB_INFO_SC = 2465,				//进阶副本各个对象信息
		MT_ADVANCE_FB_TASK_INFO_SC = 2466,			//进阶副本任务信息

		//--------------------------四象幻境(副本)--------------------------------------
		MT_FANTASY_FB_SCENE_INFO_SC = 2467,			//四象幻境副本场景信息
		MT_FANTASY_FB_INFO_SC = 2468,				//四象幻境副本信息
		MT_FANTASY_FB_REWARD_INFO_SC = 2469,		//四象幻境副本暂存奖励信息(临时背包)

		//--------------------------野外boss--------------------------------------
		MT_WILD_BOSS_NOTICE_SC = 2470,				// 野外boss反馈

		MT_ADVANCE_FB_DROP_BACK_SC = 2471,			//进阶副本特殊临时背包

		MT_WILD_BOSS_INFO_SC = 2472,				// 野外boss下发
		MT_WILD_BOSS_REQ_CS = 2473,					// 野外boss请求
		MT_WILD_BOSS_POSI_INFO_SC = 2474,			// 野外boss位置
		MT_WILD_BOSS_POKER_SC = 2475,				// 野外boss翻牌下发
		MT_WILD_BOSS_POKER_CS = 2476,				// 野外boss翻牌请求
		MT_WILD_BOSS_SINGLE_SC = 2477,				// 野外boss单个下发

		MT_CRYSTAL_FB_FAST_PASS_CS = 2478,			// 水晶副本请求扫荡
		MT_CRYSTAL_FB_ENERGY_INFO_SC = 2479,		// 水晶副本体力信息

		//--------------------------buff--------------------------------------
		MT_BUFF_INFO_SC = 2480,
		//------------------------------------ shop ------------------------------------
		MT_SHOP_ALL_INFO_SC = 2490,
		MT_SHOP_SINGLE_INFO_SC = 2491,
		MT_SHOP_BUY_REQ_CS = 2492,
		MT_SHOP_FAST_BUY_CS = 2493,					// 快速购买
		MT_SHOP_GOLD_SHOP_BUY_CS = 2494,			// 元宝商城
		MT_SHOP_GOLD_SHOP_ALL_INFO_SC = 2495,		// 元宝商城
		MT_SHOP_GOLD_SHOP_SINGLE_INFO_SC = 2496,	// 元宝商城
		MT_EXCHANGE_REQ_CS = 2497,			// 兑换
		MT_EXCHANGE_ALL_INFO_SC = 2498,		// 兑换
		MT_EXCHANGE_SINGLE_INFO_SC = 2499,	// 兑换

		//--------------------------treasure hunt--------------------------------------
		MT_XUN_BAO_REQ_CS = 2500,			// 寻宝
		MT_XUN_BAO_RET_SC = 2501,			// 寻宝结果
		MT_XUN_BAO_OPEN_INFO_SC = 2502,			// 寻宝开启信息
		MT_XUN_BAO_DRAW_TIMES_INFO_SC = 2503,			// 寻宝抽奖次数信息
		MT_XUN_BAO_EXCHANGE_CS = 2504,			// 寻宝兑换
		MT_XUN_BAO_BELSS_CS = 2505,				//寻宝祈福请求
		MT_XUN_BAO_BELSS_INFO_SC = 2506,		//寻宝祈福信息下发

		// 轮回忘川
		MT_LUN_HUI_WANG_CHUAN_REQ_CS = 2507,	//轮回忘川请求
		MT_LUN_HUI_WANG_CHUAN_INFO_SC = 2508,	//轮回忘川下发

		//--------------------------采集副本--------------------------------------
		MT_GATHER_FB_INFO_SC = 2510,				// 采集副本信息
		MT_GATHER_FB_GATHER_LIST_SC = 2513,			// 采集所得列表信息
		
		//--------------------------勇闯地宫--------------------------------------
		MT_BRAVE_GROUND_REQ_CS = 2520,				// 勇闯地宫请求
		MT_BRAVE_GROUND_RANK_SC = 2521,				// 勇闯地宫排行榜信息
		MT_BRAVE_GROUND_SCENE_SC = 2522,			// 勇闯地宫场景信息
		MT_BRAVE_GROUND_REWARD_INFO_SC = 2523,		// 勇闯地宫层奖励信息
		MT_BRAVE_GROUND_INFO_SC = 2524,				// 勇闯地宫信息
		MT_BRAVE_GROUND_ROUTE_SC = 2525,			// 勇闯地宫转发
		MT_BRAVE_GROUND_NOTIFY_SC =2526,			// 勇闯地宫通知
		MT_BRAVE_GROUND_USE_ITEM_INFO_SC = 2527,			// 勇闯地宫使用物品信息
		MT_BRAVE_GROUND_TEAM_INFO_SC = 2528,			// 勇闯地宫队伍玩家信息
		MT_BRAVE_GROUND_ROLE_INFO_SC = 2529,			// 勇闯地宫玩家信息
		MT_BRAVE_GROUND_COMMON_INFO_SC = 2530,			// 勇闯地宫活动本身一些信息

		//------------------------------------ shop 2 ------------------------------------
		MT_CANCEL_NOTIFY_REQUEST_CS = 2540,			// 取消今日红点提示请求
		MT_PURCHASE_NOTIFY_INFOR_SC = 2541,			// 今日红点提示下发

		//------------------------------------ 市场 --------------------------------------
		MT_MARKET_REQ_CS = 2545,						// 市场操作请求
		MT_MARKET_ALL_INFO_SC = 2546,					// 市场所有信息下发
		MT_MARKET_SINGLE_INFO_SC = 2547,				// 市场单条信息下发

		////////////////////////////////////////////////////////////////// 结婚  //////////////////////////////////////////////////////////////////
		MT_MARRY_REQ_CS = 2551,							// 结婚请求
		MT_MARRY_REQ_ROUTE_SC = 2552,					// 求婚信息转发给对方
		MT_MARRY_REQ_INFO_SC = 2553,					// 结婚信息
		MT_DIVORCE_REQ_ROUTE_SC = 2554,					// 离婚请求转发
		MT_DIVORCE_PROPOSE_NOTICE_SC = 2555,			// 通知求婚者请求已发送到对方
		MT_MARRIAGE_SEEKING_REQ_CS = 2556,				// 查询征婚宣言详细信息请求
		MT_MARRIAGE_SEEKING_INFO_SC = 2557,				// 征婚宣言结果信息
		MT_MARRIAGE_TASK_ROUTE_SC = 2558,				// 情缘日常任务请求转发
		MT_MARRIAGE_TASK_QUESTION_INFO_SC = 2559,		// 情缘日常答题信息
		MT_MARRIAGE_TASK_QUESTION_RESULT_SC = 2560,		// 情缘日常任务答题结果
		MT_MARRIAGE_QUERY_OTHER_INFO_RET = 2561,		// 查询别人结婚信息返回
		MT_MARRIAGE_DUET_PUZZLE_INFO = 2562,			// 双人拼图信息
		MT_MARRIAGE_DUET_TAST_LOGOUT_NOTICE = 2563,		// 双人游戏离线通知

		//--------------------------竞技场--------------------------------------
		MT_COLOSSEUM_REQ_CS = 2600,					//请求
		MT_COLOSSEUM_ROLE_INFO_SC = 2601,			//个人信息
		MT_COLOSSEUM_TOP_INFO_SC = 2602,			//前4信息
		MT_COLOSSEUM_TARGET_INFO_SC = 2603,			//对手信息
		MT_COLOSSEUM_RANK_INFO_SC = 2604,			//排行榜信息
		MT_COLOSSEUM_REPORT_INFO_SC = 2605,			//战报信息
		MT_COLOSSEUM_RANK_REWARD_SC = 2606,			//每日奖励领取信息
		MT_COLOSSEUM_PVPS_RESULT_SC = 2607,			//战斗结果
		MT_COLOSSEUM_OPEN_STATUS_SC = 2608,			//活动状态变更通知
		MT_COLOSSEUM_INPOOL_LIST_SC = 2609,			//入库玩家排名
		MT_COLOSSEUM_NEW_REPORT_SC = 2610,			//新战报通知
		MT_COLOSSEUM_NEW_DAY_REWARD_SC = 2611,		//新每日奖励通知
		MT_COLOSSEUM_NEW_REPORT_DETAIL_SC = 2612,	//新战报详情
		MT_COLOSSEUM_CLIENT_REFRESH_TARGET_NOTICE_SC = 2613, //通知客户端请求对手信息
		MT_COLOSSEUM_ACHIEVEMENT_INFO_SC = 2614,	//竞技场成就信息下发
		MT_COLOSSEUM_ACHIEVEMENT_REQ_CS = 2615,		//竞技场成就请求
		MT_COLOSSEUM_DETAL_INFO_SC = 2616,			//竞技场详细信息
		MT_COLOSSEUM_HISTORY_RANK_CHANGE_NOTICE = 2617, //竞技场历史排名变化通知
		MT_COLOSSEUM_SWEEP_RESULT_INFO = 2618,		// 竞技场扫荡成功/失败通知
		MT_COLOSSEUM_BATTLE_REWARD_NOTICE = 2619,	// 竞技场挑战奖励组通知

		//---------------------秘境奇闻
		MT_RA_MI_JING_QI_WEN_OBJ_INFO_SC = 2620,	// 秘境奇闻obj信息

		//---------------------幽冥炼狱
		MT_NETHER_WORLD_PURGATORY_INFO_SC = 2630,	//信息下发
		MT_NETHER_WORLD_PURGATORY_REQ_CS = 2631,	//请求操作
		MT_NETHER_WORLD_PURGATORY_SIGLE_INFO_SC = 2632,//单个信息下发
		MT_NETHER_WORLD_PURGATORY_MI_LING_INFO_SC = 2633,//密令信息下发
		MT_NETHER_WORLD_PURGATORY_SIGLE_CAPHTER_SC = 2634,//单章节下发

		//--------------------------首席竞选--------------------------------------
		MT_CHIEF_ELECTION_REQ_CS = 2650,			// 首席竞选 请求
		MT_CHIEF_ELECTION_NOTICE_SC = 2651,			// 首席竞选 通知
		MT_CHIEF_ELECTION_MATCH_INFO_SC = 2652,		// 首席竞选 比赛信息
		MT_CHIEF_ELECTION_ROLE_INFO_SC = 2653,		// 首席竞选 角色信息
		MT_CHIEF_ELECTION_WIN_ROLE_INFO_SC = 2654,	// 首席竞选 胜出信息
		MT_CHIEF_ELECTION_JOIN_TOP_INFO_SC = 2655,	// 首席竞选 报名人信息
		MT_CHIEF_ELECTION_FIGHTER_FINISH_NOTICE_SC = 2656,	// 首席竞选 其他人战斗完成
		MT_CHIEF_ELECTION_CHAMPION_SC = 2657,	// 首席竞选 冠军通知
		MT_CHIEF_ELECTION_SEA_ROLE_INFO_SC = 2658,	// 
		MT_CHIEF_ELECTION_SEA_SCENE_INFO_SC = 2659,	// 
		MT_CHIEF_ELECTION_STATE_INFO_SC = 2660,	// 
		MT_CHIEF_ELECTION_RANK_SC = 2661,	// 

		//--------------------------首席巅峰战--------------------------------------
		MT_CHIEF_PEAK_BATTLE_REQ_CS = 2700,			// 首席巅峰战 请求
		MT_CHIEF_PEAK_BATTLE_NOTICE_SC = 2701,		// 首席巅峰战 通知
		MT_CHIEF_PEAK_BATTLE_MATCH_INFO_SC = 2702,	// 首席巅峰战 比赛信息
		MT_CHIEF_PEAK_BATTLE_ROLE_INFO_SC = 2703,	// 首席巅峰战 角色信息
		MT_CHIEF_PEAK_BATTLE_WINNER_INFO_SC = 2704,	// 首席巅峰战 胜者信息
		MT_CHIEF_PEAK_BATTLE_FIGHTER_FINISH_NOTICE_SC = 2705,	// 首席巅峰战 其他人战斗完成
		MT_CHIEF_PEAK_BATTLE_CHAMPION_SC = 2706,	// 首席巅峰战 冠军通知

		//--------------------------家族荣耀战--------------------------------------
		MT_GUILD_HONOR_REQ_CS = 2750,				// 家族荣耀战 请求
		MT_GUILD_HONOR_ROLE_INFO_SC = 2751,			// 家族荣耀战 角色信息
		MT_GUILD_HONOR_ROLE_CHANGE_SC = 2752,		// 家族荣耀战 角色改变区域信息
		MT_GUILD_HONOR_ADD_ROLE_SC = 2754,		// 家族荣耀战 进入场景
		MT_GUILDHONOR_ZONE_BATTLE_RESULT_INFO_SC = 2755,		
		MT_GUILD_HONOR_ALL_ZONE_BATTLE_RESULT_INFO_SC = 2756,
		MT_GUILD_HONOR_ALL_ZONE_ROLE_NUM_INFO_SC = 2757,
		MT_GUILD_HONOR_REQ_2_CS = 2758,				// 家族荣耀战 请求2
		MT_GUILD_HONOR_PROCESSINFO_SC = 2759,
		MT_GUILD_HONOR_SCORE_RANK_SC = 2760,
		MT_GUILD_HONOR_SCORE_RANK_INFO_SC = 2761,		
		MT_GUILD_HONOR_MATCH_INFO_SC = 2762,
		MT_GUILD_HONOR_GUESS_INFO_SC = 2763,
		MT_GUILD_HONOR_MATCH_RECORD_SC = 2764,
		MT_GUILD_HONOR_TOP_INFO_SC = 2765,
		MT_GUILD_HONOR_ROLE_BATTLE_STATE_SC = 2766,

		//--------------------------擂台站--------------------------------------
		MT_PLATFORM_BATTLE_REQ_CS = 2780,			// 擂台站 请求
		MT_PLATFORM_BATTLE_ROLE_INFO_SC = 2781,	// 擂台站 角色上榜信息
		MT_PLATFORM_BATTLE_RANK_INFO_SC = 2782,	// 擂台站 擂台玩家信息
		MT_PLATFORM_BATTLE_RANK_FIRST_INFO_REQ_CS = 2783,	// 擂台站 擂台玩家信息
		MT_PLATFORM_BATTLE_RANK_FIRST_INFO_SC = 2784,	// 擂台站 擂台玩家信息
		MT_PLATFORM_BATTLE_SWITCH_PLATFORM_CS = 2785,// 切换擂台
		MT_PLATFORM_BATTLE_RECORD_CS = 2786,	// 擂台战报
		MT_PLATFORM_BATTLE_RECORD_NOTICE_CS = 2787,	
		MT_PLATFORM_BATTLE_REQ_2_CS = 2788,
		MT_PLATFORM_BATTLE_ACHIEVEMENT_INFO_SC = 2789,
		MT_PLATFORM_BATTLE_SETTLE_NOTICE_SC = 2790,
		MT_PLATFORM_BATTLE_LAST_RANK_INFO_SC= 2791,

		//--------------------------跨服------------------------------------------
		MT_CROSS_START_REQ_CS = 2800,				// 开始跨服请求
		MT_CROSS_START_ACK_SC = 2801,				// 开始跨服返回
		MT_BACK_TO_ORIGIN_SERVER_CS = 2802,			// 返回本服请求
		MT_BACK_TO_ORIGIN_SERVER_SC = 2803,			// 返回本服返回

		MT_ROLE_INFO_NOTICE_REQ_CS = 2850,
		MT_ROLE_INFO_NOTICE_SC = 2851,		
		
		/////////////////////////// 巅峰等级 //////////////////////////////////////	
		MT_ROLE_TOP_LEVEL_REQ_CS = 2860,		// 巅峰等级请求
		MT_ROLE_TOP_LEVEL_ALL_INFO_SC = 2861,	// 巅峰等级所有信息下发
		MT_ROLE_TOP_LEVEL_NOTICE_SC = 2862,		// 巅峰等级通知
		MT_ROLE_TOP_LEVEL_AOI_NOTICE_SC = 2863,	// 巅峰等级通知周边玩家

		//--------------------------引导副本--------------------------------------
		MT_GUIDE_FB_INFO_SC = 2900,				// 引导副本信息

		//--------------------------玉虚境(材料副本)------------------------------
		MT_MATERIAL_REQ_CS = 2910,				// 玉虚境操作请求
		MT_MATERIAL_SCENE_INFO_SC = 2911,		// 玉虚境场景信息
		MT_MATERIAL_ALL_INFO_SC = 2912,			// 玉虚境角色所有信息
		MT_MATERIAL_SINGLE_INFO_SC = 2913,		// 玉虚境单个关卡信息
		MT_MATERIAL_OTHER_INFO_SC = 2914,		// 玉虚境其它信息下发

		//
		MT_SALARY_BATTLE_STAT_INFO_SC = 2920,	// 战斗前统计本场战斗侠义值信息

		//--------------------------布阵系统--------------------------------------
		MT_FORMATION_SIMPLE_INFO_CS = 2950,		// 布阵系统简易信息请求
		MT_FORMATION_SIMPLE_INFO_SC = 2951,		// 布阵系统简易信息返回
		MT_FORMATION_DETAIL_INFO_CS = 2952,		// 布阵系统详细信息请求
		MT_FORMATION_DETAIL_INFO_SC = 2953,		// 布阵系统详细信息返回
		MT_FORMATION_RENAME_CS = 2954,			// 布阵系统重命名请求
		MT_FORMATION_RENAME_SC = 2955,			// 布阵系统重命名返回
		MT_FORMATION_SAVE_CS = 2956,			// 布阵系统保存改动请求
		MT_FORMATION_DELETE_CS = 2957,			// 布阵系统删除请求

		MT_PET_HELPER_FORMATION_SET_CS = 2970,	// 宠物助战阵容设置请求
		MT_PET_HELPER_FORMATION_INFO_SC = 2971,	// 宠物助战阵容信息返回
		MT_PET_HELPER_FORMATION_QUERY_CS = 2972,// 宠物助战阵容查询

		//--------------------------随机活动--------------------------------------
		MT_RAND_ACTIVITY_OP_CS = 3000,							// 随机活动操作请求
		MT_RAND_ACTIVITY_TIME_LIMIT_STORE_INFO = 3001,			// 随机活动-限时商店
		MT_RAND_ACTIVITY_TURN_TABLE_INFO = 3003,				// 随机活动-开服转盘
		MT_RAND_ACTIVITY_TURN_TABLE_REWARD = 3004,				// 随机活动-开服转盘-奖励
		MT_RAND_ACTIVITY_DISCOUNT_STORE_INFO = 3005,			// 随机活动-折扣商店
		MT_RAND_ACTIVITY_CONSUME_CAROUSEL_INFO = 3008,			// 随机活动-消费转盘
		MT_RAND_ACTIVITY_CONSUME_CAROUSEL_REWARD = 3009,		// 随机活动-消费转盘-奖励
		MT_RAND_ACTIVITY_CONSUME_CAROUSEL_SHOW = 3010,			// 随机活动-消费转盘-珍稀显示
		MT_FIRST_CHONGZHI_ACTIVITY_YIXIANER = 3011,				// 首充活动-医仙儿
		MT_FIRST_CHONGZHI_ACTIVITY_REQ_CS = 3012,				// 首充活动请求
		MT_RAND_ACTIVITY_HERITAGE_TREASURE_INFO_CS = 3013,		// 传承瑰宝信息
		MT_RAND_ACTIVITY_SEVEN_COLOR_PICTURE = 3014,			// 七彩图鉴信息下发
		MT_RAND_ACTIVITY_EXPEL_MONSTER_INFO_SC = 3015,			// 随机获得驱除妖兽下发
		MT_RAND_ACTIVITY_SALE_SHOP_INFO_SC = 3016,				// 特卖商店信息下发
		MT_RAND_ACTIVITY_HAPPY_UP_INFO_SC = 3017,				// 欣欣向荣信息下发
		MT_RAND_ACTIVITY_ANIMAL_RACE_INFO_SC = 3018,			// 神兽径赛场景信息下发
		MT_RAND_ACTIVITY_ANIMAL_RACE_ROLE_SC = 3019,			// 神兽径赛玩家信息下发
		MT_RAND_ACTIVITY_LUCKY_BAG_INFO_SC = 3020,				// 福袋信息
		MT_RAND_ACTIVITY_SEVEN_COLOR_REQ_CS = 3021,				// 七彩图鉴客户端数据请求
		MT_RAND_ACTIVITY_SEVEN_COLOR_TYPE_CS = 3022,			// 七彩图鉴发送单个花灵信息
		MT_RAND_ACTIVITY_TIAN_JIANG_HAO_LI_INFO_CS = 3023,		// 天降好礼信息
		MT_RAND_ACTIVITY_JOY_SEEKS_FAVORS_SC = 3024,			// 欢乐寻宠匹配
		MT_RAND_ACTIVITY_JOY_SEEKS_FAVORS_MATH_SUCC_SC = 3025,	// 欢乐寻宠倒计时
		MT_RAND_ACTIVITY_JOY_SEEKS_FAVORS_MATH_REQ_CS = 3026,	// 欢乐寻宠请求开始战斗
		MT_RAND_ACTIVITY_JIAN_DAN_QIN_XIN_INFO_SC = 3027,		// 剑胆琴心信息下发
		MT_RAND_ACTIVITY_LINKAGE_GIFT_INFO_SC = 3028,			// 联动送礼信息下发
		MT_RAND_ACTIVITY_COLLECT_WORD_INFO_SC = 3029,			// 集字活动信息
		MT_RAND_ACTIVITY_DISCOUNT_CARNIVAL_INFO_SC = 3030,		// 折扣狂欢信息
		MT_RAND_ACTIVITY_LUCKY_DISCOUNT_INFO_SC = 3031,			// 幸运折扣信息
		MT_RAND_ACTIVITY_LUCKY_TURNTABLE_INFO_SC = 3032,		// 幸运转盘信息下发
		MT_RAND_ACTIVITY_LUCKY_TURNTABLE_REWARD_SC = 3033,		// 幸运转盘 - 奖励
		MT_RAND_ACTIVITY_ADVENTURE_GIFT_INFO_SC = 3035,			// 奇遇礼包信息
		MT_RAND_ACTIVITY_QUAN_MIN_DISCOUNT_INFO_SC = 3036,		// 全民团购信息
		MT_RAND_ACTIVITY_ANIMAL_RACE_ANIMATION_INFO_SC = 3037,	// 神兽径赛动画信息下发
		MT_RAND_ACTIVITY_TU_JIAN_TREASURE_INFO_SC = 3038,		// 图鉴夺宝信息
		MT_RAND_ACTIVITY_GOD_TOKEN_INFO_SC = 3039,				// 封神战令信息
		MT_RAND_ACTIVITY_ELEMENT_CARDS_INFO_SC = 3040,			// 元素翻牌信息
		MT_RAND_ACTIVITY_SIGN_IN_CONTINUITY_INFO_SC = 3041,		// 连登好礼信息
		MT_RAND_ACTIVITY_GIVE_PET_DRAW_INFO_SC = 3042,			// 赠30连抽（送寻宠连抽）信息
		MT_RAND_ACTIVITY_MAKE_UP_REWARD_REQ_CS = 3045,			// 运营活动-奖励补齐请求
		MT_RAND_ACTIVITY_BONUS_OP_CS = 3050,					// 随机活动额外奖励通用请求
		MT_RAND_ACTIVITY_BONUS_INFO_SC = 3051,					// 随机活动额外奖励信息下发

		MT_RAND_ACTIVITY_CI_JIU_YING_XIN_INFO_SC = 3052,		// 随机活动-辞旧迎新下发
		MT_RAND_ACTIVITY_NIU_XING_DA_YUN_INFO_SC = 3053,		// 随机活动-牛行大运下发
		MT_RAND_ACTIVITY_SPRING_FESTIVAL_STORE_INFO_SC = 3054,	// 随机活动-年货节下发
		MT_RAND_ACTIVITY_MONPOLY_GIFT_INFO_SC = 3055,			// 随机活动-牛运当头下发
		MT_RAND_ACTIVITY_MONPOLY_GIFT_EXCHANGE_INFO_SC = 3056,	// 随机活动-牛运当头兑换下发
		MT_RAND_ACTIVITY_MONPOLY_GIFT_QUESTION_INFO_SC = 3057,	// 随机活动-牛运当头答题下发
		MT_RAND_ACTIVITY_COW_EXCHANGE_INFO_SC = 3058,			// 随机活动-牛运兑换下发
		MT_RAND_ACTIVITY_COW_STORE_INFO_SC = 3059,				// 随机活动-牛运商城下发
		MT_RAND_ACTIVITY_COW_LUCKY_EXCHANGE = 3060,				// 随机活动-牛运无边
		MT_RAND_ACTIVITY_LITTLE_YEAR_GIFT = 3061,				// 随机活动-小年送礼
		MT_RAND_ACTIVITY_PET_LOOT = 3062,						// 随机活动-宠物夺宝
		MT_RAND_ACTIVITY_TRADEMARKET_RETURN_GOLD_INFO_SC = 3063,// 随机活动-交易行返还
		MT_RAND_ACTIVITY_TRADEMARKET_RETURN_GOLD_HOT_ITEM_LIST = 3064, // 随机活动-交易行返还-热卖列表
		MT_RAND_ACTIVITY_HAPPY_LOOK_INFO_SC = 3065,				// 随机活动-连连乐翻天
		MT_RAND_ACTIVITY_SIGN_HAPPY_INFO_SC = 3066,				// 随机活动-登录送好礼
		MT_RAND_ACTIVITY_WAN_LING_GONG_YING_INFO_SC = 3067,		// 随机活动-万灵共盈

		MT_RAND_ACTIVITY_GOOD_GIFT_TURN_INFO_SC = 3070,			// 随机活动-好礼转动信息下发
		MT_RAND_ACTIVITY_GOOD_GIFT_TURN_REWARD_SC = 3071,		// 随机活动-好礼转动信息下发
		MT_RAND_ACTIVITY_HELP_OTHERS_INFO_SC = 3072,			// 随机活动-义胆助人信息下发
		MT_RAND_ACTIVITY_SECRET_TREASURE_INFO = 3073,			// 随机活动-特惠秘宝信息下发
		MT_RAND_ACTIVITY_LAO_DONG_HAO_LI_INFO_SC = 3074,		// 随机活动-劳动好礼信息下发
		MT_RAND_ACTIVITY_SUMMER_LI_LIAN_ALL_INFO_SC = 3075,		// 随机活动-初夏历练信息下发
		MT_RAND_ACTIVITY_COLORFUL_LANTERN_ALL_INFO_SC = 3076,	// 随机活动-多彩花灯信息下发
		MT_RAND_ACTIVITY_COLORFUL_LANTERN_GAME_INFO_SC = 3077,	// 随机活动-多彩花灯游戏信息下发
		MT_RAND_ACTIVITY_COLORFUL_LANTERN_LIGHT_INFO_SC = 3078,	// 随机活动-多彩花灯花灯信息下发
		MT_RAND_ACTIVITY_SUMMER_STORE_INFO_SC = 3079,			// 随机活动-夏日商店信息下发
		MT_RAND_ACTIVITY_LUCKY_SIGN_INFO_SC = 3080,				// 随机活动-好运花签信息下发
		MT_RAND_ACTIVITY_SUPER_OFF_SHOP_SC = 3081,				// 随机活动-超级折扣商城信息下发
		MT_RAND_ACTIVITY_SUMMER_STORE_SINGLE_INFO_SC = 3082,	// 随机活动-夏日商店单个信息下发
		MT_RAND_ACTIVITY_SUPER_OFF_SHOP_SINGLE_INFO_SC = 3083,	// 随机活动-超级折扣商城单个信息下发
		MT_RAND_ACTIVITY_YUAN_QI_TURNING_INFO_SC = 3084,		// 随机活动-元气翻牌信息下发
		MT_RAND_ACTIVITY_COOL_BREEZE_INFO_SC = 3085,			// 随机活动-凉风送爽信息下发
		MT_RAND_ACTIVITY_COOL_BREEZE_SINGLE_INFO_SC = 3086,		// 随机活动-凉风送爽单个信息下发
		MT_RAND_ACTIVITY_LOTUS_MI_BAO_INFO_SC = 3087,			// 随机活动-荷香觅宝信息下发
		MT_RAND_ACTIVITY_MI_BAO_FANG_DIAN_INFO_SC = 3088,		// 随机活动-觅宝坊店信息下发
		MT_RAND_ACTIVITY_MI_BAO_FANG_DIAN_SINGLE_INFO_SC = 3089,// 随机活动-觅宝坊店单个信息下发
		MT_RAND_ACTIVITY_DUI_HUAN_LING_YU_CS = 3090,			// 随机活动-兑换灵玉
		MT_RAND_ACTIVITY_HELP_OTHERS_INFO_2_SC = 3091,			// 随机活动-义胆助人2  （桃园一三事）信息下发
		MT_RAND_ACTIVITY_SHAN_HAI_QUESTION = 3092,				// 随机活动-山海问答信息下发

		MT_RAND_ACTIVITY_HORCRUX_TIANCHENG_ROLE_INFO = 3093,	// 随机活动-魂器天成 角色信息
		MT_RAND_ACTIVITY_HORCRUX_TIANCHENG_POOL_INFO = 3094,	// 随机活动-魂器天成 奖池信息
		MT_RAND_ACTIVITY_HORCRUX_TIANCHENG_DRAW = 3095,			// 随机活动-魂器天成 抽奖返回
		MT_RAND_ACTIVITY_INTEGRAL_DRAW_ROLE_INFO = 3096,		// 随机活动-整点抽奖 角色信息
		MT_RAND_ACTIVITY_INTEGRAL_DRAW_GAME_INFO = 3097,		// 随机活动-整点抽奖 游戏信息
		MT_RAND_ACTIVITY_INTEGRAL_DRAW_RESULT = 3098,			// 随机活动-整点抽奖 抽奖返回

		MT_RAND_ACTIVITY_SHAN_HAI_QUESTION_STATUS = 3099,		// 随机活动-山海问答答题状态信息下发

		MT_RAND_ACTIVITY_PAI_MAI_HANG_NOTICE_INFO_SC = 3100,	// 随机活动-拍卖行通知信息下发
		MT_RAND_ACTIVITY_PAI_MAI_HANG_SINGLE_NOTICE_SC = 3101,	// 随机活动-拍卖行单个通知信息下发
		MT_RAND_ACTIVITY_PAI_MAI_HANG_USER_INFO_SC = 3102,		// 随机活动-拍卖行角色信息下发
		MT_RAND_ACTIVITY_PAI_MAI_HANG_USER_SINGLE_INFO_SC = 3103,// 随机活动-拍卖行角色单个信息下发
		MT_RAND_ACTIVITY_PAI_MAI_HANG_USER_RECORD_INFO_SC = 3104,//	随机活动-拍卖行角色竞拍记录下发
		MT_RAND_ACTIVITY_BACK_REWARD_INFO_SC = 3105,			// 随机活动-回归豪礼信息下发
		MT_RAND_ACTIVITY_BACK_REWARD_REQ_CS = 3106,				// 随机活动-回归豪礼请求

		MT_RAND_ACTIVITY_SERVER_COMPETITION_ROLE_ALL_INFO = 3107,// 随机活动-全服比拼角色所有信息下发
		MT_RAND_ACTIVITY_SERVER_COMPETITION_SINGLE_INFO = 3108,	 // 随机活动-全服比拼角色单条信息下发
		MT_RAND_ACTIVITY_SERVER_COMPETITION_ACTIVITY_INFO = 3109,// 随机活动-全服比拼活动信息下发

		MT_RAND_ACTIVITY_SHAN_HAI_MEET_INFO = 3110,				 // 随机活动-山海相逢信息下发
		MT_RAND_ACTIVITY_FATE_ENTANGLEMENT_INFO_SC = 3111,			 // 随机活动-山海缘起信息下发

		MT_RAND_ACTIVITY_ZHAOCAIJINBAO_ROLE_INFO_SC = 3112,		// 随机活动-招财进宝 角色信息
		MT_RAND_ACTIVITY_ZHAOCAIJINBAO_DRAW_RESULT_SC = 3113,	// 随机活动-招财进宝 抽奖返回

		MT_RAND_ACTIVITY_YUN_ZE_TERRITORY_INFO_SC = 3114,		 // 随机活动-云泽画境信息下发
		MT_RAND_ACTIVITY_YUN_ZE_BLOOMING_RECORD_INFO_SC = 3115,	 // 随机活动-云泽繁花录信息下发
		MT_RAND_ACTIVITY_MO_ZU_ZAI_NA_LI_SC = 3116,				 // 随机活动-魔族在哪里信息下发
		MT_RAND_ACTIVITY_CHAOZHI_FENXIANG = 3117,				// 随机活动-超值纷享
		MT_RAND_ACTIVITY_YUN_ZE_SLOT_MACHINE_INFO_SC = 3118,	 // 随机活动-纷享同花礼信息下发
		MT_RAND_ACTIVITY_YUN_ZE_SLOT_MACHINE_RESULT_INFO_SC = 3119,	// 随机活动-纷享同花礼摇奖结果信息下发
		MT_RAND_ACTIVITY_YUN_ZE_ER_SAN_SHI_INFO_SC = 3120,		 // 随机活动-云泽二三事信息下发
		MT_RAND_ACTIVITY_SHAN_HAI_ZHENG_FENG_ALL_INFO_SC = 3121, // 随机活动-山海争锋信息下发
		MT_RAND_ACTIVITY_SHAN_HAI_MI_BAO_INFO_SC = 3122,		 // 随机互动-山海觅宝
		MT_RAND_ACTIVITY_SHANHAI_HUADENG_ALL_INFO_SC = 3123,	// 随机活动-山海花灯信息下发
		MT_RAND_ACTIVITY_SHANHAI_HUADENG_GAME_INFO_SC = 3124,	// 随机活动-山海花灯游戏信息下发
		MT_RAND_ACTIVITY_SHANHAI_HUADENG_LIGHT_INFO_SC = 3125,	// 随机活动-山海花灯花灯信息下发
		MT_RAND_ACTIVITY_YUN_ZE_ER_SAN_SHI_TURNTABLE_INFO_SC = 3126,	// 随机活动-云泽二三事转盘信息下发
		
		MT_RAND_ACTIVITY_JIXING_GAOZHAO_ROLE_INFO_SC = 3127,	// 随机活动-吉星高照 角色信息
		MT_RAND_ACTIVITY_JIXING_GAOZHAO_POOL_INFO_SC = 3128,	// 随机活动-吉星高照 奖池信息
		MT_RAND_ACTIVITY_JIXING_GAOZHAO_DRAW_SC = 3129,			// 随机活动-吉星高照 抽奖返回
		MT_RAND_ACTIVITY_JIXING_GAOZHAO_RECORD_LIST_INFO_SC = 3130,// 随机活动-吉星高照 记录列表
		MT_RAND_ACTIVITY_JIXING_GAOZHAO_RECORD_INFO_SC = 3131,	// 随机活动-吉星高照 添加记录

		MT_RAND_ACTIVITY_YUN_ZE_WISH_INFO_SC = 3132,			// 随机活动-云泽祈愿  信息下发
		MT_RAND_ACTIVITY_YUN_ZE_HAO_LI_INFO_SC = 3133,			// 随机活动-云泽好礼  信息下发
		MT_RAND_ACTIVITY_YUN_ZE_CUTE_PET_INFO_SC = 3134,		// 随机活动-萌宠驾到  信息下发
		MT_RAND_ACTIVITY_YUN_ZE_CATCH_ELVES_GET_INFO_SC = 3135,	// 随机活动-精灵大抓捕 抓捕信息下发
		MT_RAND_ACTIVITY_YUN_ZE_CUTE_PET_CHALLENGE_INFO_SC = 3136,	//随机活动-萌宠驾到挑战信息下发

		MT_RAND_ACTIVITY_SERVER_COMPETITION_OTHER_INFO_SC = 3137,// 随机活动-全服比拼其它信息下发

		MT_RAND_ACTIVITY_TIGER_FISH_EVERY_YEAR_INFO_SC = 3138,	// 随机活动-年年有鱼 信息下发
		MT_RAND_ACTIVITY_YINHU_WENSHI_INFO_SC = 3139,			// 随机活动-寅虎问世
		MT_RAND_ACTIVITY_TIGER_FIND_SPRING_INFO_SC = 3140,		// 随机活动-寅虎探春 信息下发
		MT_RAND_ACTIVITY_TIGER_TEASE_NEWYEAR_INFO_SC = 3141,	// 随机活动-寅虎逗新春
		MT_RAND_ACTIVITY_CINIU_YINGXINHU_INFO_SC = 3142,		// 随机活动-辞牛迎新虎
		MT_RAND_ACTIVITY_YIN_HU_BAI_SUI_INFO_SC = 3143,			// 随机活动-寅虎拜岁
		MT_RAND_ACTIVITY_HU_XING_DA_YUN_INFO_SC = 3144,			// 随机活动-虎行大运
		MT_RAND_ACTIVITY_HU_XING_DA_YUN_USER_LIST_SC = 3145,	// 随机活动-虎行大运 被邀请人列表
		MT_RAND_ACTIVITY_TIGER_LUCKY_MONEY_INFO_SC = 3146,		// 随机活动-寅虎压岁
		MT_RAND_ACTIVITY_TIGERAUSPICIOUS_SNOW_INFO_SC = 3147,	// 随机活动-瑞雪丰年 信息下发
		
		MT_RAND_ACTIVITY_YUAN_QI_SPEED_INFO_SC = 3148,			// 随机活动-元气竞速
		MT_RAND_ACTIVITY_YUAN_QI_SPEED_PLAY_INFO_SC = 3149,		// 随机活动-元气竞速游戏状态

		MT_RAND_ACTIVITY_JINGJI_YUEKA_ALL_INFO_SC = 3150,		// 随机活动-竞技月卡所有信息下发
		MT_RAND_ACTIVITY_JINGJI_YUEKA_NOTICE_SC = 3151,			// 随机活动-竞技月卡相关通知  
		MT_RAND_ACTIVITY_JINGJI_YUEKA_REFRESH_NOTICE_SC = 3152,	// 随机活动-竞技月卡任务刷新下发   
		MT_RAND_ACTIVITY_JINGJI_YUEKA_SINGLE_TYPE_INFO_SC = 3153,//随机活动-竞技月卡单条条件信息下发   
		MT_RAND_ACTIVITY_JINGJI_YUEKA_TASK_LIST_INFO_SC = 3154, //随机活动-竞技月卡任务列表信息下发     

		MT_RAND_ACTIVITY_RONG_LIAN_YOU_LI_INFO_SC = 3155,		// 随机活动-熔炼有礼所有信息下发
		MT_RAND_ACTIVITY_QIYUAN_KONGMINGDENG_SC = 3156,			// 随机活动-祈愿空明灯
		MT_RAND_ACTIVITY_XUNMENG_HUDIE_INFO_SC = 3157,			// 随机活动-寻梦蝴蝶
		MT_RAND_ACTIVITY_YUAN_QI_HELP_INFO_SC = 3158,			// 随机活动-元气助人下发
		MT_RAND_ACTIVITY_BO_ZHONG_YOU_LI_INFO_SC = 3159,		// 随机活动-播种有礼
		MT_RAND_ACTIVITY_GOD_BEAST_ADVENT_INFO_SC = 3160,		// 随机活动-异兽临世
		MT_RAND_ACTIVITY_YUAN_QI_FAN_PAI_INFO_SC = 3161,		// 随机活动-元气翻牌
		MT_RAND_ACTIVITY_GOD_BEAST_ADVENT_OPEN_INFO_SC = 3162,	// 随机活动-异兽临世开启状态信息
		MT_RAND_ACTIVITY_GOD_BEAST_ADVENT_BUY_INFO_SC = 3163,	// 随机活动-异兽临世全服限购道具数量信息
		MT_RAND_ACTIVITY_WANLING_ZHULI_INFO_SC = 3164,			// 随机活动-万灵筑礼
		MT_RAND_ACTIVITY_BAO_ZI_LAI_LE_INFO_SC = 3165,			// 随机活动-包子来了
		MT_RAND_ACTIVITY_YUAN_QI_JISHIBU_INFO_SC = 3166,		// 随机活动-元气记事簿
		MT_RAND_ACTIVITY_CHENG_FENG_PO_LANG_OPEN_INFO_SC = 3167,// 随机活动-乘风破浪开启信息
		MT_RAND_ACTIVITY_CHENG_FENG_PO_LANG_INFO_SC = 3168,		// 随机活动-乘风破浪
		MT_RAND_ACTIVITY_GOD_BEAST_ADVENT_NOTIFY_INFO_SC = 3169,// 随机活动-异兽临世好感度变动通知
		MT_RAND_ACTIVITY_MIAO_BI_SHENG_HUA_INFO_SC = 3170,		//  随机活动-妙笔生花
		MT_RAND_ACTIVITY_ZHAOCAIJINBAO_RECORD_LIST_INFO_SC = 3171,	// 随机活动-招财进宝 抽奖记录信息下发

		MT_RAND_ACTIVITY_ZONG_XIANG_MAN_CHENG_INFO = 3180,		//随机活动-粽香满城
		MT_RAND_ACTIVITY_SALTY_SWEET_BATTLE_INFO_SC = 3181,		// 随机活动-咸甜之争
		MT_RAND_ACTIVITY_SALTY_SWEET_BATTLE_ROLE_INFO_SC = 3182,// 随机活动-咸甜之争个人下发
		MT_RAND_ACTIVITY_DUAN_YANG_XIANG_NANG_INFO = 3183,		// 随机活动-端阳香囊
		MT_RAND_ACTIVITY_CANG_LONG_CI_FU_INFO = 3184,			// 随机活动-苍龙赐福
		MT_RAND_ACTIVITY_LUCKY_BLIND_BOX_INFO_SC = 3185,		// 随机活动-幸运盲盒
		MT_RAND_ACTIVITY_LAN_TANG_CHU_MU_INFO = 3186,			// 随机活动-兰汤初沐
	
		MT_RAND_ACTIVITY_CHONG_JI_FAN_LI_INFO_SC = 3187,		// 随机活动-宠技返利信息下发
		MT_RAND_ACTIVITY_XIN_FU_QIANG_HUA_INFO_SC = 3188,		// 随机活动-新服强化
		
		MT_RAND_ACTIVITY_QI_XI_TE_HUI_INFO = 3189,				// 随机活动-七夕特惠信息
		MT_RAND_ACTIVITY_QI_XI_TE_HUI_SINGLE_INFO = 3190,		// 随机活动-七夕特惠单条信息
		MT_RAND_ACTIVITY_QI_XI_TE_HUI_OTHER_INFO = 3191,		// 随机活动-七夕特惠其它信息
		MT_RAND_ACTIVITY_PET_EXCHANGE_SIGNLE_INFO_SC = 3192,	// 随机活动-宠物兑换单条信息下发

		/////////////////////////////////////////////////////////////////// 世界BOSS2 （巨龙boss）///////////////////////////////////////////////////////////////////
		MT_WROLD_BOSS_2_RANK_INFO_SC = 3198,				// 世界BOSS2 排行榜信息
		MT_WROLD_BOSS_2_ROLE_INFO_SC = 3199,				// 世界BOSS2 玩家信息
		MT_WROLD_BOSS_2_REQ_CS = 3200,					// 世界BOSS2 操作请求
		MT_WROLD_BOSS_2_INFO_SC = 3201,					// 世界BOSS2 信息
		MT_WROLD_BOSS_2_RESULT_SC = 3202,					// 世界BOSS2 活动结束奖励结算信息
		MT_WROLD_BOSS_2_POS_INFO_SC = 3203,				// 世界BOSS2 位置信息
		MT_WROLD_BOSS_2_BATTLE_INFO_SC = 3204,				// 世界BOSS2 每次战斗结束一些数据信息		

		/////////////////////////////////////////////////世界答题//////////////////////////////////////////
		MT_WORLD_QUESTION_INFO_SC = 3205,				//世界答题下发
		MT_WORLD_QUESTION_REQ_CS = 3206,				//世界答题请求

		//////////////////////////////世界BOSS3(天神降临)///////////////////////////////////////
		MT_WORLD_BOSS_3_REQ_CS = 3210,					// 世界BOSS3 操作请求
		MT_WORLD_BOSS_3_INFO_SC = 3211,					// 世界BOSS3 信息
		MT_WORLD_BOSS_3_RESULT_SC = 3212,				// 世界BOSS3 活动结束奖励结算信息
		MT_WORLD_BOSS_3_POS_INFO_SC = 3213,				// 世界BOSS3 位置信息
		MT_WORLD_BOSS_3_BATTLE_INFO_SC = 3214,			// 世界BOSS3 每次战斗结束一些数据信息
		MT_WORLD_BOSS_3_ROLE_INFO_SC = 3215,			// 世界BOSS3 玩家信息
		MT_WORLD_BOSS_3_RANK_INFO_SC = 3216,			// 世界BOSS3 排行榜信息
	
		//////////////////////////////// 匹配 ///////////////////////////
		MT_SCENE_MATCH_START_SC = 3230,
		MT_SCENE_MATCH_INFO_UPDATE_SC = 3231,
		MT_SCENE_MATCH_STOP_SC = 3232,
		MT_SCENE_MATCH_MATCH_ENEMY_SC = 3233,

		MT_TASK_CHOSED_REQ_CS = 3234,
		MT_TASK_CHOSED_INFO_SC = 3235,

		MT_TASK_CHAIN_TURN_TABLE_INFO_SC = 3236,			// 任务链转盘信息下发

		/////////////////////////////// 聊天群组协议 //////////////////////////////////
		MT_CHAT_GROUP_START_SC = 3240,
		MT_CHAT_GROUP_ROLE_GROUP_INFO_SC = 3241,			// 所有群组的缩略信息
		MT_CHAT_GROUP_ALL_INFO_SC = 3242,					// 单个群组的详细信息
		MT_CHAT_GROUP_ROLE_INVITE_ENTER_SC = 3243,			// 发送邀请群组信息
		MT_CHAT_GROUP_ONE_GROUP_INFO_SC = 3244,				// 单个群组缩略信息

		MT_CHAT_GROUP_START_CS = 3260,
		MT_CHAT_GROUP_ROLE_INVITE_ENTER_CS = 3261,			// 请求邀请加入群组
		MT_CHAT_GROUP_ROLE_ENTER_GROUP_CS = 3262,			// 接受邀请后加入群组 (拒绝直接忽略信息
		MT_CHAT_GROUP_CREATE_GROUP_CS = 3263,				// 创建群组
		MT_CHAT_GROUP_KICK_MEMBER_CS = 3264,				// 移除玩家出群聊
		MT_CHAT_GROUP_DISSMISS_CS = 3265,					// 解散群聊
		MT_CHAT_GROUP_LEAVE_CS = 3266,						// 离开群聊
		MT_CHAT_GROUP_GET_GROUP_LIST_CS = 3267,				// 获取群聊列表
		MT_CHAT_GROUP_GET_GROUP_INFO_CS = 3268,				// 获取群聊详细信息，以及成员信息
		MT_CHAT_GROUP_CHANGE_NOTICE_CS = 3269,				// 修改群聊宣言

		MT_TEMP_CHAT_GROUP_SIMPLE_INFO_SC = 3270,			// 临时群组信息
		MT_TEMP_CHAT_GROUP_LEAVE_SC = 3271,					// 离开临时群组
		MT_CHAT_GROUP_MSG_NOTICE_SC = 3272,					// 群组通知信息显示
		MT_TEMP_CHAT_GROUP_BECOME_OFFICIAL_CS = 3273,		// 临时群组转正
		MT_TEMP_CHAT_GROUP_BECOME_OFFICIAL_NOTICE_SC = 3274,// 临时群组转正成功通知
		MT_CHAT_GROUP_HANDOVER_TO_OTHERS_CS = 3275,			// 群组移交群主
		
		//-----------------------幻界战场----------------------------
		MT_HUAN_JIE_ZHAN_CHANG_REQ_CS = 3290,				//幻界战场请求
		MT_HUAN_JIE_ZHAN_CHANG_TEAM_INFO_SC = 3291,				//幻界战场队伍信息
		MT_HUAN_JIE_ZHAN_CHANG_ERR_RETURN_SC = 3292,				
		MT_HUAN_JIE_ZHAN_CHANG_MATCH_STATE_SC = 3301,		//幻界战场匹配状态
		MT_HUAN_JIE_ZHAN_CHANG_RESULT_SC = 3302,			//幻界战场战斗结果
		MT_HUAN_JIE_ZHAN_CHANG_OPPONENT_INFO_SC = 3303,		//幻界战场对手信息
		MT_HUAN_JIE_ZHAN_CHANG_ROLE_INFO_SC = 3304,		//幻界战场信息
		MT_HUAN_JIE_ZHAN_CHANG_RANK_LIST_INFO_SC = 3305,		//幻界战场排行榜
		MT_HUAN_JIE_ZHAN_CHANG_ROLE_RANK_LIST_REQ_CS = 3306,		//
		MT_HUAN_JIE_ZHAN_CHANG_ROLE_RANK_LIST_SC = 3307,		//
		MT_HUAN_JIE_ZHAN_CHANG_TIAN_TI_RECORD_SC = 3308,		//
		MT_HUAN_JIE_ZHAN_CHANG_PROF_WIN_LOSE_INFO_SC = 3309,		//
			
		//////////////////////////////////幻界战场 - 化身模块///////////////////////////////////////////
		MT_HUASHEN_ROLE_INFO_SC = 3310,						// 化身基本信息下发
		MT_HUASHEN_ROLE_ATTR_LIST_SC = 3311,				// 化身属性列表下发
		MT_HUASHEN_MEDAL_REQ_CS = 3312,						// 化身勋章请求
		MT_HUASHEN_MEDAL_INFO_SC = 3313,					// 化身勋章穿戴信息下发
		MT_HUASHEN_EQUIPMENT_OPERA_REQ_CS = 3314,			// 化身装备操作请求
		MT_HUASHEN_EQUIPMENT_INFO_SC = 3315,				// 化身装备信息下发
		MT_HUASHEN_EQUIPMENT_GRID_CHANGE_SC = 3316,			// 化身装备改变通知
		MT_HUASHEN_JEWELRY_HANDLE_REQ_CS = 3317,			// 化身灵饰操作请求
		MT_HUASHEN_JEWELRY_ALL_FORM_SC = 3318,				// 化身灵饰装备信息下发
		MT_HUASHEN_JEWELRY_INFO_CHANGE_SC = 3319,			// 化身灵饰改变通知
		MT_HUASHEN_ATTR_PLAN_POINT_CS = 3320,				// 化身潜能操作请求
		MT_HUASHEN_ROLE_ATTR_PLAN_SC = 3321,				// 化身加点信息下发
		MT_HUASHEN_PET_REQ_CS = 3322,						// 化身宠物操作请求
		MT_HUASHEN_PET_ASSIGN_ATTR_POINT_CS = 3323,			// 化身请求分配宠物属性点
		MT_HUASHEN_SEND_PET_ALL_INFO_SC = 3324,				// 化身所有宠物信息下发
		MT_HUASHEN_PET_SINGLE_INFO_SC = 3325,				// 化身单个宠物信息下发
		MT_HAUSHEN_PET_OTHER_INFO_SC = 3326,				// 化身宠物其他信息下发
		MT_HUASHEN_PET_ATTRLIST_INFO_SC = 3327,				// 化身单个宠物属性列表下发
		MT_HUASHEN_INFO_REQ_CS = 3328,						// 化身模块信息请求下发
		MT_HUASHEN_KNAPSACK_ALL_INFO_SC = 3329,				// 化身背包所有信息下发
		MT_HUASHEN_KNAPSACK_SINGLE_ITEM_CHANEGE_SC = 3330,	// 化身单个物品改变下发
		MT_HUASHEN_SET_BATTLE_POSITION_CS = 3331,			// 化身请求设置主角站位
		MT_HUASHEN_BATTLE_POSITION_SC = 3332,				// 化身站位信息下发
		MT_HUASHEN_SEASON_INFO_SC = 3333,					// 化身赛季信息下发
		MT_HUASHEN_CHANGE_CRYSTAL_NOTICE_SC = 3334,			// 化身元素水晶信息变更下发
		MT_HUASHEN_ATTR_PLAN_LIST_INFO_SC = 3335,			// 化身方案加点信息下发
		MT_HUASHEN_ATTR_PLAN_CHANGE_NAME_CS = 3336,			// 化身方案改名请求
		MT_HUASHEN_MEDAL_TAKE_OFF_SC = 3337,				// 化身勋章脱下后该勋章位置下发

		//////////////////////////////////神秘(命格)商店///////////////////////////////////
		MT_MYSTERY_STORE_OP_CS = 3350,						// 神秘(命格)商店操作请求
		MT_MYSTERY_STORE_INFO_SC = 3351,					// 神秘(命格)商店信息

		/////////////////////////////////家族灵脉////////////////////////////////////////
		MT_GUILD_INBORN_REQ_CS = 3360,						// 家族灵脉操作请求
		MT_GUILD_INBORN_ALL_INFO_SC = 3361,					// 家族灵脉所有信息下发
		MT_GUILD_INBORN_SINGLE_INFO_SC = 3362,				// 家族灵脉单个信息下发
		MT_GUILD_INBORN_SINGLE_TYPE_INFO_SC = 3363,			// 家族灵脉类型信息下发

		//////////////////////////////////山海降妖///////////////////////////////////
		MT_SHAN_HAI_BOSS_OP_CS = 3370,						// 山海降妖操作请求
		MT_SHAN_HAI_BOSS_NPC_INFO_SC = 3371,				// 山海降妖NPC信息
		MT_SHAN_HAI_BOSS_ROLE_INFO_SC = 3372,				// 山海降妖玩家信息
		//MT_SHAN_HAI_BOSS_POKER_INFO_SC = 3373,				// 山海降妖翻牌信息
		MT_SHAN_HAI_BOSS_SINGLE_INFO_SC = 3374,				// 山海降妖单个信息

		MT_RAND_ACTIVITY_CAI_SHEN_JU_BAO_INFO_SC = 3380,				// 财神聚宝信息
		MT_RAND_ACTIVITY_DING_ZHI_BAO_XIANG_INFO_SC = 3390,				// 定制宝箱
		MT_RAND_ACTIVITY_ZHUAN_SHU_LI_BAO_INFO_SC = 3400,				// 专属礼包
		MT_RAND_ACTIVITY_ZHUAN_SHU_LI_BAO_DAY_INFO_SC = 3401,			// 专属礼包-每日礼包信息下发
		MT_RAND_ACTIVITY_JING_YAN_BIAO_SHENG_INFO_SC = 3410,			// 经验飙升信息
		MT_RAND_ACTIVITY_XIAN_SHI_BAO_HE_SC = 3420,						// 限时宝盒
		MT_RAND_ACTIVITY_XIAN_SHI_BAO_DAI_SC = 3430,					// 限时宝袋
		MT_RAND_ACTIVITY_SHAN_HAI_BAO_XIANG_INFO_SC = 3431,				// 山海宝箱 信息
		MT_RAND_ACTIVITY_SHAN_HAI_BAO_HE_INFO_SC = 3432,				// 山海宝盒 信息
		MT_RAND_ACTIVITY_SHAN_HAI_BAO_DAI_INFO_SC = 3433,				// 山海宝袋 信息

		MT_WEN_XIANG_LOU_FB_REQ_CS = 3474,					// 温香楼副本操作请求
		MT_WEN_XIANG_LOU_NPC_INFO_SC = 3475,				// 温香楼副本操作请求
		MT_WEN_XIANG_LOU_SCENE_INFO_SC = 3476,				// 温香楼副本操作请求

		MT_RAND_ACTIVITY_DING_ZHI_BAO_XIANG_2_INFO_SC = 3477,				// 定制宝箱2
		MT_RAND_ACTIVITY_XIAN_SHI_BAO_HE_2_SC = 3478,				// 限时宝盒2
		MT_RAND_ACTIVITY_XIAN_SHI_BAO_DAI_2_SC = 3479,				// 限时宝袋2
	
		//////////////////////////////////奇闻任务////////////////////////////////////
		MT_ANECDOTE_TASK_INFO_SC = 3490,					// 单个奇闻任务信息改变通知
		MT_ANECDOTE_TASK_INFO_LIST_SC = 3491,				// 奇闻任务信息列表
		MT_ANECDOTE_TASK_BATTLE_FAIL_SC = 3492,				// 奇闻战斗失败通知
		MT_ANECDOTE_TASK_COMPLETE_TASK_NOTICE_SC = 3493,	// 完成一个任务通知
		MT_ANECDOTE_TASK_OPERATE_REQ_CS = 3494,				// 奇闻任务请求
		MT_ANECDOTE_TASK_SPECIAL_INFO_SC = 3495,			// 奇闻任务其他特殊下发
		MT_ANECDOTE_TASK_SPECIAL_SET_CS = 3496,				// 奇闻任务其他特殊设置
	
		MT_NORMAL_TASK_FINISH_ANEDOTE_GAME_CS = 3497,		// 普通任务完成奇闻小游戏

		MT_RESEARCH_TASK_ROUND_INFO_SC = 3498,				// 三界除魔结算下发

		MT_ANECDOTE_BIG_EVENT_INFO_SC = 3499,				// 奇闻大事件

		//////////////////////////////////新的任务框架////////////////////////////////
		MT_NEW_TASK_ACTION_REQ_CS = 3500,						// 新的任务行为请求
		MT_NEW_TASK_DU_CHUANG_ZEI_YING_SCENE_INFO_SC = 3501,	// 独闯贼营场景信息下发
		MT_NEW_TASK_DU_CHUANG_ZEI_YING_NOTICE_SC = 3502,		// 独闯贼营通知寻路

		MT_NEW_TASK_CATCH_FOX_REQ_CS = 3503,					// 抓拿狐妖
		MT_NEW_TASK_GIVE_NPC_ITEM_CS = 3504,					// 新的任务提交物品给NPC请求
		MT_NEW_TASK_PARTNER_SOAR_INFO_SC = 3505,				// 伙伴飞升任务相关信息

		MT_NEW_TASK_ACTION_END = 3550,								// 预留50条协议

		//////////////////////////////////翻牌管理器////////////////////////////////
		MT_DRAW_REQ_CS = 3560,								// 翻牌请求
		
		MT_ROLE_DRAW_INFO_SC = 3561,						// 角色翻牌选择信息
		MT_DRAW_POKER_INFO_SC = 3565,						// 奖励翻牌信息


		////////////////////////////////等级助力////////////////////////////////////////

		//旧的等级助力屏蔽
		MT_ROLE_LEVEL_COMPLEMENT_SC = 3562,					//等级助力下发
		MT_ROLE_LEVEL_COMPLEMENT_CS = 3563,					//等级助力请求

		//---新等级助力
		MT_ROLE_NEW_LEVEL_COMPLEMENT_REQ_CS = 3566,			//新等级助力请求
		MT_ROLE_NEW_LEVEL_COMPLEMENT_INFO_SC = 3567,		//新等级助力角色信息下发
		MT_NEW_LEVEL_COMPLEMENT_SPEICAL_INFO_SC = 3568,		//新等级助力特殊信息下发

		/////////////////////////////////梦渊笔录//////////////////////////////////////
		MT_DREAM_NOTES_CHALLENGE_CS = 3570,					// 梦渊笔录请求
		MT_DREAM_NOTES_CHALLENGE_ROLE_INFO_SC = 3571,		// 梦渊笔录角色信息
		MT_DREAM_NOTES_CHALLENGE_SCENE_INFO_SC = 3572,		// 梦渊笔录场景信息
		MT_DREAM_NOTES_CHALLENGE_NOTICE_SC = 3573,			// 梦渊笔录通知

		/////////////////////////////////神兽降临//////////////////////////////////////
		MT_SHEN_SHOU_JIANG_LIN_INFO_SC = 3580,		// 神兽降临信息
		MT_SHEN_SHOU_JIANG_LIN_DRWA_RESULT_SC = 3581,		// 神兽降临抽奖结果
		MT_SHEN_SHOU_JIANG_LIN_BUY_INFO_SC = 3582,		// 神兽降临购买信息

		/////////////////////////////////校场演练//////////////////////////////////////
		MT_SCHOOL_EXERCISE_INFO_SC = 3590,					// 校场演练信息
		MT_SCHOOL_EXERCISE_CHALLENGE_CS = 3591,				// 校场挑战请求
		MT_SCHOOL_EXERCISE_CHALLENGE_SUCC_SC = 3592,		// 校场挑战成功通知
		MT_SCHOOL_EXERCISE_FIRST_OPEN_UI = 3593,			// 首次打开校场UI

		MT_WORKSHOP_EQUIP_DECOMPOSE_CS = 3600,				// 装备分解

		////////////////////////////////////春日登峰//////////////////////////////////////////
		MT_RA_CHUN_RI_DENG_FENG_INFO_SC = 3611,						// 春日登峰

		MT_MI_JING_QI_WEN_SHOP_INFO_SC = 3612,						// 秘境奇闻商店
		MT_MI_JING_QI_WEN_QUESTION_INFO_SC = 3614,					// 秘境奇闻答题信息
		MT_MI_JING_QI_WEN_QUESTION_REQ_CS = 3615,					// 秘境奇闻回答题目
		MT_RA_FORTUNE_MAZE_BOX_NUM_SC = 3616,						// 幸运迷宫宝箱数量
		MT_RAND_ACTIVITY_FORTUNE_MAZE_SC = 3617,					// 随机活动 - 幸运迷宫
		MT_RAND_ACTIVITY_FORTUNE_MAZE_PIN_TU_MAP_SC = 3618,			// 随机活动 - 幸运迷宫地图
		MT_MI_JING_QI_WEN_BUY_ITEM_REQ_CS = 3619,					// 秘境奇闻购买道具

		MT_CLOSE_WINDOS_RET_SC = 3620,								// 关闭窗口下发
		MT_CLOSE_WINDOS_REQ_CS = 3621,								// 关闭窗口请求

		//////////////////////////////////遗迹夺宝副本
		MT_RELIC_LOOTING_TIME_SC = 3622,							// 遗迹夺宝时间内容下发
		MT_RELIC_LOOTING_SKILL_SC = 3623,							// 技能信息下发
		MT_RELIC_LOOTING_MATH_CS = 3624,							// 遗迹夺宝请求匹配
		MT_RELIC_LOOTING_TEAM_SC = 3625,							// 遗迹夺宝队伍信息
		MT_RELIC_LOOTING_PLAYER_REQ_CS = 3626,						// 遗迹夺宝玩家操作
		MT_RELIC_LOOTING_OBJ_INFO_SC = 3627,						// 遗迹夺宝obj
		MT_RELIC_LOOTING_MEMBER_POS_SC = 3628,						// 遗迹夺宝队友位置
		MT_RELIC_LOOTING_MEMBER_TS_SC = 3629,						// 遗迹夺宝传送队友

		////////////////////////////////////组合贺礼//////////////////////////////////////////
		MT_RA_ZU_HE_HE_LI_INFO_SC = 3630,						// 组合贺礼


		//////////////////////////遗迹夺宝副本
		MT_RELIC_LOOTING_MEMBER_MARK_SC = 3631,						// 标记下发
		MT_RELIC_LOOTING_FB_FINISH_SC = 3632,						// 副本完成下发
		MT_RELIC_LOOTING_RANK_LIST_SC = 3633,						// 排行榜下发
		MT_RELIC_LOOTING_ACHIEVEMENT_SC = 3634,						// 成就下发
		MT_RELIC_LOOTING_SPECIAL_EFFECT_SC = 3635,					// 玩家状态下发

		//----------------
		MT_WOLRD_STATUS_GUILD_FIGHT_TOP_INFO_SC=3650,

		MT_WOLRD_STATUS_END = 3659,

		////////////////////////////// 灵宠秘境 ////////////////////////////////
		MT_PET_MONOPOLY_INFO_SC = 3660,						// 灵宠秘境信息下发
		MT_PET_MONOPOLY_OPERATE_CS = 3661,					// 灵宠秘境操作请求
		MT_PET_MONOPOLY_EXPECT_PET_SET_SC = 3662,			// 灵宠秘境祈福返回
		MT_PET_MONOPOLY_ROLL_DICE_RESULT_SC = 3663,			// 灵宠秘境投骰子结果返回
		MT_PET_MONOPOLY_CUR_GRID_CHANGE_NOTICE_SC = 3664,	// 灵宠秘境当前格点内容变更通知
		MT_PET_MONOPOLY_REWARD_RECORD_SC = 3665,			// 灵宠秘境查看奖励
		MT_PET_MONOPOLY_ENTER_NEXT_LEVEL_SC = 3666,			// 灵宠秘境进入下一层通知
		MT_PET_MONOPOLY_TURN_TABLE_SC = 3667,				// 灵宠秘境转盘信息下发
		MT_PET_MONOPOLY_TURN_TABLE_RESULT_SC = 3668,		// 灵宠秘境转盘结果下发
		MT_PET_MONOPOLY_SPECIAL_ITEM = 3669,				// 灵宠秘境特殊道具数量信息下发
		MT_PET_MONOPOLY_SPECIAL_ITEM_USE = 3670,			// 灵宠秘境心愿卡替换格子位置

		////////////////////////////// 云端竞技 ////////////////////////////////
		MT_CLOUD_ARENA_OPERATE_CS = 3675,					// 云端竞技操作请求
		MT_CLOUD_ARENA_SELF_INFO_SC = 3676,					// 云端竞技自身信息返回
		MT_CLOUD_ARENA_TARGET_INFO_SC = 3677,				// 云端竞技目标信息返回
		MT_CLOUD_ARENA_NEW_FIGHT_RECORD_NOTICE_SC = 3678,	// 云端竞技新战报通知
		MT_CLOUD_ARENA_FIGHT_RECORD_LIST_SC = 3679,			// 云端竞技战报列表返回
		MT_CLOUD_ARENA_RANK_LIST_SC = 3680,					// 云端竞技排行榜返回
		MT_CLOUD_ARENA_SEASON_INITIAL_SCORE_NOTICE = 3681,	// 云端竞技通知初始积分
		MT_CLOUD_ARENA_NOTICE_DAN_CHANGE = 3682,			// 云端竞技段位变更通知
		MT_CLOUD_ARENA_ACHIEVEMENT_INFO = 3683,				// 云端竞技成就信息返回
		MT_CLOUD_ARENA_ACHIEVEMENT_UPDATE_NOTICE = 3684,	// 云端竞技成就改变通知
		MT_CLOUD_ARENA_ROLE_SEASON_HIGHEST_DATA_SC = 3685,	// 云端竞技玩家赛季最高数据下发
		MT_CLOUD_ARENA_REAL_TIME_INFO = 3686,				// 云端竞技实时积分排名返回
		MT_CLOUD_ARENA_REAL_TIME_INFO_REQ = 3687,			// 云端竞技实时积分排名请求
		MT_CLOUD_ARENA_SET_FORMATION_REQ = 3688,			// 云端竞技阵容设置
		MT_CLOUD_ARENA_FORMATION_INFO_SC = 3689,			// 云端竞技阵容信息下发

		////////////////////////////// 移动辅助 ////////////////////////////////
		MT_MOVE_ASSIST_REQ_SC = 3690,						// 移动协助请求
		MT_MOVE_ASSIST_RET_CS = 3691,						// 移动协助返回

		// 角色参与活动奖励  保留 3692  ~ 3694
		MT_ROLE_JOIN_ACTIVITY_REWARD_REQ_CS = 3692,			// 角色参与活动奖励 客户端请求
		MT_ROLE_JOIN_ACTIVITY_REWARD_INFO_SC = 3693,		// 角色参与活动奖励 信息
		MT_ROLE_JOIN_ACTIVITY_ONE_CLICK_FINISH_INFO_SC = 3694,		// 一键完成活动

		////////////////////////////////////令牌天赐//////////////////////////////////////////
		MT_RA_LING_PAI_TIAN_CI_INFO_SC = 3810,						// 令牌天赐

		////////////////////////////////////无量心法//////////////////////////////////////////
		MT_RA_LING_PAI_TIAN_CI_2_INFO_SC = 3811,						// 无量心法(令牌天赐2)

		///////////////////////////////////赋世魂器//////////////////////////////////////////
		MT_RA_LING_PAI_TIAN_CI_3_INFO_SC = 3812,						// 赋世魂器

		//
		MT_RA_FASHION_SHOP_INFO_SC = 3820,						// 时装商店

		//
		MT_MI_YI_INFO_SC = 3830,						// 密医阴谋信息
		MT_MIYI_POKER_SC = 3831,						// 密医阴谋翻牌信息
		MT_MIYI_POKER_REQ_CS = 3832,					// 密医阴谋翻牌请求
		MT_MIYI_POKER_INFO_SC = 3833,					// 密医阴谋信息下发
		MT_MIYI_SCENE_INFO_SC = 3834,					// 密医阴谋场景信息下发
		MT_MIYI_KILL_PASS_INFO_SC = 3835,				// 通知客户端此次通关是不是首通信息
		MT_MIYI_REQ_CS = 3836,				// 密医请求
	
		//铸灵
		MT_ZHU_LING_REQ_CS = 3850,						
		MT_ZHU_LING_CHANG_INFO_SC = 3851,						
		MT_ZHU_LING_DECOMPOSE_INFO_SC = 3852,					
		MT_ZHU_LING_DECOMPOSE_REQ_CS = 3853,
		MT_ZHU_LING_UP_LEVEL_REQ_CS = 3854,
		MT_ZHU_LING_PUT_ON_SUCC_NOTICE_SC = 3855,

		MT_SET_AUTO_ADD_POINT_FLAG_CS = 3880,  //设置自动加点
		
		//////////////////////////////////////////////////////////////////////////////
		MT_RA_ONE_YUAN_GOU_INFO_SC = 3890,						// 一元购

		///////////////////////////////// 灵玉基金 //////////////////////////////////////
		MT_LING_YU_JI_JIN_INFO_SC = 3900,		// 灵玉基金信息
		
		
		// 帮派活跃度礼包及排行   保留  3910 ~ 3916 
		MT_GUILD_ACTIVE_PERSON_RANK_CS = 3910,					//  获取 帮派活跃度 个人排行信息 
		MT_GUILD_ACTIVE_INFO_SC = 3911,							//  帮派活跃度信息[帮派活跃值]
		MT_GUILD_ACTIVE_INFO_MEMBER_SC = 3912,					//  帮派活跃度信息[个人领取奖励的信息]
		MT_GUILD_ACTIVE_PERSON_RANK_SC= 3913,					//  帮派活跃度 个人排行信息 
		MT_GUILD_ACTIVE_GET_GIFT_CS = 3914,						//  帮派活跃度  领取礼包


		///////////////////////////////// 自定义指令 /////////////////////////////////////
		MT_CUSTOM_INSTRUCTION_INFO_SC = 3920,					//  自定义指令下发
		MT_CUSTOM_INSTRUCTION_ADD_CS = 3921,					//  新增自定义指令
		MT_CUSTOM_INSTRUCTION_REMOVE_CS = 3922,					//  去除自定义指令
		MT_CUSTOM_INSTRUCTION_BATCH_REMOVE_CS = 3923,			//  批量清除自定义指令

		///////////////////////////////// 周常 //////////////////////////////////////
		MT_WEEK_ACTIVE_DEGREE_INFO_SC = 3925,	//周常 - 玩家信息通知

		///////////////////////////////// 神印之地 //////////////////////////////////////
		MT_PETY_GOD_FB_REQ_CS = 3930,				// 神印之地 用户操作

		MT_PETY_GOD_FB_ROLE_INFO_SC = 3931,			// 神印之地 角色信息
		MT_PETY_GOD_FB_SCENE_INFO_SC = 3932,		// 神印之地 副本场景信息

		///////////////////////////////// 渡劫 //////////////////////////////////////
		MT_DU_JIE_REQ_CS = 3935,					// 渡劫 用户操作

		MT_DU_JIE_ROLE_INFO = 3936,					// 渡劫 角色信息
		MT_DU_JIE_FB_INFO = 3937,					// 渡劫 副本信息

		//////////////////////////////////百鬼夜行///////////////////////////////////
		MT_HUNDRED_GHOST_INFO_SC = 3941,					// 百鬼夜行NPC信息
		MT_HUNDRED_GHOST_POKER_INFO_SC = 3942,				// 百鬼夜行翻牌信息

		//////////////////////////////北斗七星//////////////
		MT_BIG_DIPPER_REQ_CS = 3950,			//请求
		MT_BIG_DIPPER_INFO_SC = 3951,			//下发
		MT_BIG_DIPPER_MY_INFO_SC = 3952,		//我的通关信息
		
		//////////////////////////////殒神之地
		MT_FALLEN_GOD_INFO_SC = 3953,			//殒神之地信息
		MT_FALLEN_GOD_REQ_CS = 3954,			//殒神之地请求

		///////////////////////////////// 资源下载 //////////////////////////////////////
		MT_FETCH_RESOURCE_DOWNLOAD_REWARD_CS = 3955,			// 获取资源下载奖励
		MT_FETCH_RESOURCE_DOWNLOAD_REWARD_SC = 3956,			// 资源下载奖励标记变更

		///////////////////////////////// 防诈骗奖励 //////////////////////////////////////
		MT_FANG_FRADU_REWARD_REQ_CS = 3957,			// 防诈骗奖励回答
		MT_FANG_FRADU_REWARD_INFO_SC = 3958,			// 防诈骗奖励标记变更

		//////////////////////////////// 属性丹 //////////////////////////////////////////
		MT_ROLE_ATTR_PELLET_REQ_CS = 3960,			// 属性丹操作请求
		MT_ROLE_ATTR_PELLET_ALL_INFO_SC = 3961,		// 属性丹所有信息下发
		MT_ROLE_ATTR_PELLET_SINGLE_INFO_SC = 3962,	// 属性丹单个信息下发

		///////////////////////////////// 活跃转盘 //////////////////////////////////////
		MT_ACTIVE_TURNTABLE_REQ_CS = 3965,			// 活跃转盘请求
		MT_ACTIVE_TURNTABLE_INFO_SC = 3966,			// 活跃转盘任务信息
		MT_ACTIVE_TURNTABLE_REWARD_SC = 3967,		// 活跃转盘抽奖返回

		///////////////////////////////// 评分有礼 //////////////////////////////////////
		MT_PINGJIA_REWARD_REQ_CS = 3968,			// 评价有礼请求
		MT_PINGJIA_REWARD_INFO_SC = 3969,			// 评价有礼信息
		//
		MT_CAPACITY_TARGET_REQ_CS = 3975,		// 系统邀战请求
		MT_CAPACITY_TARGET_INFO_SC = 3976,		// 系统邀战信息

		///////////////////////////////// 红包 //////////////////////////////////////
		MT_HONG_BAO_CLIENT_REQ_CS = 4000,			// 客户端请求
		MT_HONG_BAO_SEND_LIST_SC = 4001,			// 红包 - 可发送列表信息
		MT_HONG_BAO_GET_LIST_SC = 4002,				// 红包 - 可领取列表信息
		MT_HONG_BAO_BASE_INFO_SC = 4003,			// 红包 - 详细信息
		MT_HONG_BAO_SEND_LIST_CHANGE_SC = 4004,		// 红包 - 可发送列表信息变更
		MT_HONG_BAO_GET_LIST_CHANGE_SC = 4005,		// 红包 - 可领取列表信息变更
		MT_HONG_BAO_BASE_INFO_CHANGE_SC = 4006,		// 红包 - 基础信息发生变更 [策划要求红包的领取人员名单要事实刷新] 

		///////////////////////////////// 伙伴--元神契约 //////////////////////////////////////
		MT_PARTNER_PACT_CHANGE_LIST_SC = 4010,		// 伙伴--元神契约 仓库列表变更
		MT_PARTNER_PACT_BASE_ATTR_SC = 4011,		// 伙伴--元神契约 契约用 属性通知
		MT_PARTNER_PACT_PACTINFO_SC = 4012,			// 伙伴--元神契约 契约改变

		//////////////////////////////// 天命 4015 ~ 4018  ///////////////////////////////////////
		MT_TIAN_MING_INFO_SC = 4015,				// 天命 信息

		///////////////////////////////// 天地道痕 //////////////////////////////////////
		MT_TIAN_DI_DAO_HEN_REQ_CS = 4040,		//
		MT_TIAN_DI_DAO_HEN_ROLE_INFO_SC = 4041,		//
		MT_TIAN_DI_DAO_HEN_OPEN_INFO_SC = 4042,		//


		MT_GIVE_GIFT_REQ_CS = 4050,
		MT_GIVE_GIFT_ROLE_INFO_SC = 4051,
		MT_GIVE_GIFT_INFO_SC = 4052,

		MT_RAND_ACTIVITY_ZHEN_PIN_SHANG_PU_INFO_SC = 4080,			// 珍品商铺信息
		MT_RAND_ACTIVITY_ZHEN_PIN_SHANG_PU_OPEN_DAY_SC = 4081,			// 珍品商铺信息

		///////////////////////////////// 福牛报喜 [4090 ~ 4093] //////////////////////////////////////
		MT_COW_REPORT_GOOD_NEWS_CLIENT_REQ_CS = 4090,				// 客户端操作请求
		MT_COW_REPORT_GOOD_NEWS_REWARD_LIST_INFO_SC = 4091,			// 中奖人员名单
		MT_COW_REPORT_GOOD_NEWS_INFO_SC = 4092,						// 福牛报喜的系统信息
		MT_COW_REPORT_GOOD_NEWS_ROLE_BUY_INFO = 4093,				// 玩家的购买信息

		///////////////////////////////// 寅虎纳福(福牛报喜复刻) [4094 ~ 4097] ////////////////////////
		MT_TIGER_BRINGS_BLESSING_CLIENT_REQ_CS = 4094,				// 客户端操作请求
		MT_TIGER_BRINGS_BLESSING_REWARD_LIST_INFO_SC = 4095,		// 中奖人员名单
		MT_TIGER_BRINGS_BLESSING_INFO_SC = 4096,					// 寅虎纳福的系统信息
		MT_TIGER_BRINGS_BLESSING_ROLE_BUY_INFO = 4097,				// 玩家的购买信息


		///////////////////////////////// 决斗 //////////////////////////////////////
		MT_DUEL_BATTLE_INFO_SC = 4100,				// 决斗战斗信息
		MT_DUEL_KILL_VALUE_AOI_NOTICE_SC = 4101,	// 杀戮值称号通知周边玩家

		///////////////////////////////// 阴阳眼 //////////////////////////////////////
		MT_YIN_YANG_EYE_OP_REQ_CS = 4110,			// 阴阳眼操作请求
		MT_YIN_YANG_EYE_INFO_SC = 4111,				// 阴阳眼信息

		////////////////////////////////  无尽之塔////////////////////////////////////
		MT_ENDLESS_TOWER_INFO_SC = 4120,				// 无尽之塔下发
		MT_ENDLESS_TOWER_REQ_CS = 4121,					// 无尽之塔请求

		/////////////////////////////  飞升
		MT_FLY_UP_SYSTEM_INFO_SC = 4130,				// 飞升下发
		MT_FLY_UP_SYSTEM_REQ_CS = 4131,				// 飞升请求
		

		///////////////////////////		琉璃蜃境
		MT_MIRAGE_TERRITORY_REQ_CS = 4132,			// 请求
		MT_MIRAGE_TERRITORY_INFO_SC = 4133,			// 下发
		MT_MIRAGE_TERRITORY_SET_PARTNER_REQ_CS = 4134,//布阵伙伴
		MT_MIRAGE_TERRITORY_PARTNER_INFO_SC = 4135,//布阵伙伴下发

		MT_FLY_UP_SYSTEM_ADV_INFO_SC = 4136,		// 飞升强化下发
		MT_FLY_UP_SKILL_SIGLE_INFO_SC = 4137,		// 飞升技能信息下发

		///////////////////////////////// 小游戏 ////////////////////////////////////
		MT_MINI_GAME_INFO_SC = 4145,				// 小游戏信息下发
		MT_MINI_GAME_FINISH_STAGE_CS = 4146,		// 小游戏完成请求
		MT_MINI_GAME_CLAIM_PHASE_REWARD_CS = 4147,	// 小游戏获取阶段奖励请求
		MT_MINI_GAME_START_BATTLE = 4148,			// 小游戏进入战斗
		MT_MINI_GAME_SET_MINI_GAME_ID = 4149,		// 由客户端告知小游戏id

		///////////////////////////////// 配饰 //////////////////////////////////////
		MT_ACCESSORIES_REQ_CS = 4150,				// 配饰请求
		MT_ACCESSORIES_INFO_SC = 4151,				// 配饰信息下发
		MT_ACCESSORIES_SIGNLE_SC = 4152,			// 配饰单个信息变化
		MT_ACCESSORIES_VOC_COMPOSE_CS = 4153,		// 配饰继承词条
		MT_ACCESSORIES_SPECAIL_EQUIP_SC = 4154,		// 配饰-仙饰下发

		///////////////////////////////// 职业晋阶 - 子阶 4155 - 4158 ///////////////////////////
		MT_ADVANCE_CAREER_SUB_INFO_SC = 4156,		// 职业晋阶子阶信息


		//秘境奇闻2
		MT_RA_MI_JING_QI_WEN_2_OBJ_INFO_SC = 4170,						// 秘境奇闻2商店
		MT_MI_JING_QI_WEN_2_SHOP_INFO_SC = 4171,						// 秘境奇闻2商店
		MT_MI_JING_QI_WEN_2_QUESTION_INFO_SC = 4172,					// 秘境奇闻2答题信息
		MT_MI_JING_QI_WEN_2_QUESTION_REQ_CS = 4173,						// 秘境奇闻2回答题目
		MT_MI_JING_QI_WEN_2_BUY_ITEM_REQ_CS = 4174,						// 秘境奇闻2购买道具


		//////随机活动 山海聚宝
		MT_SHAN_HAI_JU_BAO_ROLE_INFO_SC = 4177,			//角色信息
		MT_SHAN_HAI_JU_BAO_DRAW_RET_SC = 4178,			//抽奖返回


		///////////////////////////////// 好友亲密度 [4180 ~ 4183 ] //////////////////////////////////////
		MT_FRIEND_INTIMACY_LIST_INFO_SC = 4180,							// 好友亲密度信息列表
		MT_FRIEND_INTIMACY_INFO_CHANGE_SC = 4181,						// 好友亲密度信息变更

		//////随机活动 山海初遇
		MT_SHAN_HAI_MET_ROLE_INFO_SC = 4185,			//角色信息
		MT_SHAN_HAI_MET_TASK_PARAM_SC = 4186,			//任务信息
		MT_SHAN_HAI_MET_TASK_CHANGE_PARAM_SC = 4187,	//任务改变

		//////随机活动 再战云泽
		MT_ZAI_ZHAN_YUN_ZE_INFO_SC = 4190,			//角色信息

		//////随机活动 纷享加倍
		MT_COLORFUL_DOUBLE_ROLE_INFO_SC = 4195,			//角色信息
		MT_COLORFUL_DOUBLE_TASK_PARAM_SC = 4196,		//任务信息
		MT_COLORFUL_DOUBLE_TASK_CHANGE_PARAM_SC = 4197,	//任务改变

		//结义
		MT_JIE_YI_REQ_CS = 4200,			
		MT_JIE_YI_CREATE_CS = 4201,			
		MT_JIE_YI_CREATE_NAME_CS = 4202,
		MT_JIE_YI_CREATE_INFO_SC = 4203,
		MT_JIE_YI_CREATE_NAME_UPDATE_SC = 4204,
		MT_JIE_YI_CREATE_CONFIRM_SC = 4205,
		MT_JIE_YI_CREATE_CANCEL_SC = 4206,
		MT_JIE_YI_ZU_INFO_SC = 4207,
		MT_JIE_YI_ZU_MEMBER_NAME_CS = 4208,
		MT_JIE_YI_ZU_TEXT_CS = 4209,
		MT_JIE_YI_ZU_NAME_CS = 4210,
		MT_JIE_YI_ZU_ROUTE_SC = 4211,
		MT_JIE_YI_ZU_ROLE_XUAN_YAN_LIST_SC = 4212,
		MT_JIE_YI_ZU_XUAN_YAN_LIST_SC = 4213,
		MT_JIE_YI_ZU_XUAN_YAN_REQ_CS = 4214,
		MT_JIE_YI_ZU_XUAN_YAN_REQ_LIST_CS = 4215,
		MT_JIE_YI_ZU_KICK_NOTICE_SC = 4216,
		MT_JIE_YI_ZU_ROLE_ONLINE_STATUS_SC = 4217,

		//////万灵话本
		MT_WAN_LING_FB_REQ_CS = 4230,			//用户操作
		MT_WAN_LING_FB_ROLE_INFO_SC = 4231,		//角色信息
		MT_WAN_LING_FB_INFO_SC = 4232,			//副本信息

		//////情缘-婚宴
		MT_WEDDING_FB_REQ_CS = 4240,			//玩家操作
		MT_WEDDING_FB_SCENE_INFO_SC = 4241,		//副本信息
		MT_WEDDING_FB_ROLE_SC = 4242,			//副本角色信息
		MT_WEDDING_FB_SYNC_SC = 4243,			//副本信息同步
		MT_WEDDING_SHOW_INFO_SC = 4244,			//婚宴--展示信息(邀请函)

		MT_WEDDING_REGIST_LIST_SC = 4245,		//婚宴--预约列表
		MT_WEDDING_INVITE_LIST_SC = 4246,		//婚宴--邀请列表
		MT_WEDDING_APPLY_LIST_SC = 4247,		//婚宴--申请列表
		MT_WEDDING_CHANGE_SC = 4248,			//婚宴--通知

		MT_WEDDING_BASE_INFO_SC = 4249,			//婚宴--基础信息
		MT_WEDDING_STATUS_SYNC_SC = 4250,		//婚宴--开启广播

		MT_WEDDING_FB_SUB_ACT_XUN_QING_MI_AI_SC = 4251,	//婚宴--小游戏 寻情觅爱 角色信息
		MT_WEDDING_FB_SUB_ACT_XIN_XIN_XIANG_YIN_SC = 4252,	//婚宴--小游戏 心心相印 角色信息
		MT_WEDDING_FB_SUB_ACT_BAO_WEI_XIU_QIU_SC = 4253,	//婚宴--小游戏 保卫绣球 角色信息
		MT_WEDDING_FB_SUB_ACT_ZAO_SHENG_GUI_ZI_SC = 4254,	//婚宴--小游戏 枣生桂子 角色信息



		// 情缘
		MT_QING_YUAN_REQ_CS = 4260,			//用户操作
		MT_QING_YUAN_INVITE_ROUTE_SC = 4261,
		MT_QING_YUAN_INFO_SC = 4262,
		MT_QING_YUAN_LEAVE_ROUTE_SC = 4263,
		MT_QING_YUAN_INVITE_RET_SC = 4264,
		MT_QING_YUAN_LEAVE_NOTICE_SC = 4265,
		MT_QING_YUAN_CREATE_NOTICE_SC = 4266,

		//宠物飞升
		MT_PET_FEI_SHENG_TASK_INFO_SC = 4300,

		MT_PET_FEI_SHENG_END = 4330,

		// 限时累充 4331 ~4333
		MT_XIAN_SHI_LEI_CHONG_INFO_SC = 4331,			// 限时累充信息

		//周末礼包
		MT_WEEKEND_GIFT_INFO = 4335,			//角色信息

		//超值献礼
		MT_CHAO_ZHI_GIFT_INFO = 4340,			//角色信息

		// 修为 4345 ~4348
		MT_XIU_WEI_REQ_CS = 4345,					// 修为 客户端请求
		MT_XIU_WEI_INFO_SC = 4346,					// 修为信息
		MT_XIU_WEI_INFO_CHANGE_SC = 4347,			// 修为信息变更
		MT_XIU_WEI_DAN_YAO_LIMIT_INFO_SC = 4348,	// 修为丹药信息

		//九州秘宝
		MT_TREASURE_TASK_REQ_CS = 4349,			// 九州秘宝请求
		MT_TREASURE_TASK_INFO_SC = 4350,		// 九州秘宝角色信息	

		// 论剑九州
		MT_WORLD_ARENA_INFO_SC = 4400,					//!< 赛季信息
		MT_WORLD_ARENA_SIGNUP_INFO_SC = 4401,			//!< 主角报名信息
		MT_WORLD_ARENA_OPERATE_CS = 4402,				//!< 玩家操作
		MT_WORLD_ARENA_SCHEDULE_INFO_SC = 4403,			//!< 赛程信息
		MT_WORLD_ARENA_USER_BASE_INFO_SC = 4404,		//!< 玩家基本信息
		MT_WORLD_ARENA_GROUP_RANK_LIST_SC = 4405,		//!< 组内排行榜下发
		MT_WORLD_ARENA_COUNT_DOWN_SC_TO_FIGHT = 4406,	//!< 倒计时开始战斗 
		MT_WORLD_ARENA_START_ENTER_SCENE_NOTICE = 4407,	//!< 开始进场通知
		MT_WORLD_ARENA_SHOWUP_INFO = 4408,				//!< 到场信息下发

			//宠物异闻录
		MT_PET_FEI_SHENG_REWARD_INFO_SC = 4420,

		//情缘时装
		MT_RA_QING_YUAN_SHI_ZHUANG_INFO_SC = 4425,	// 随机活动-情缘时装 角色信息
		//情缘累充
		MT_RA_QING_YUAN_LEI_CHONG_INFO_SC = 4426,	// 随机活动-情缘时装 角色信息

		MT_PET_BATCH_COMPOSE_CS = 4430,	// 宠物批量合成
		MT_PET_BATCH_COMPOSE_RET_SC = 4431,	// 宠物批量合成
		MT_PET_BATCH_COMPOSE_AND_DECOMPOSE_CS = 4432,
		MT_PET_DECOMPOSE_DECOMPOSE_CS = 4433,

		MT_TASK_SKIP_CS = 4434,//通用任务跳过

		//巡游
		MT_XUN_YOU_CAR_STATUS_INFO_SYNC = 4450,	//花车 状态信息同步

		//天下至尊令
		MT_PEAK_TOKEN_ROLE_REQ_CS = 4455,			//玩家操作


		MT_PEAK_TOKEN_ROLE_JOIN_STATUS_SC = 4457,	//角色参与状态通知
		MT_PEAK_TOKEN_REWARD_INFO_SC = 4458,		//奖励通知
		MT_PEAK_TOKEN_ROLE_INFO_SC = 4459,			//玩家信息

		//收益统计
		MT_INCOME_ROLE_REQ_CS = 4460,			//玩家操作
		MT_INCOME_ROLE_INFO_SC = 4461,			//玩家信息

		//伙伴强化
		MT_PARTNER_INTENSIFY_CS = 4465,			//伙伴强化
		MT_PARTNER_INTENSIFY_INFO_SC = 4466,	//伙伴强化成功返回

		//师徒系统 优化 师徒传功 
		MT_SHI_TU_CHAUN_GONG_FB_REQ = 4470,			//师徒传功 角色操作

		MT_SHI_TU_CHAUN_GONG_GAME_INFO = 4471,		//师徒传功 游戏信息通知
		MT_SHI_TU_CHAUN_GONG_SCENE_INFO = 4472,		//师徒传功 场景信息
		MT_SHI_TU_SYNC_INFO = 4473,					//师徒传功 场景信息

		// 英雄会武
		MT_WORLD_TEAM_ARENA_REQ_CS = 4600,							// 英雄会武请求
		MT_WORLD_TEAM_ARENA_ALL_SIGN_UP_TEAM_SC = 4601,				// 所有报名战队信息下发
		MT_WORLD_TEAM_ARENA_SIGN_UP_REQ_CS = 4602,					// 英雄会武报名请求
		MT_WORLD_TEAM_ARENA_PRE_TEAM_SIGN_UP_INFO_SC = 4603,		// 英雄会武预备报名信息下发
		MT_WORLD_TEAM_ARENA_NOTICE_INFO_SC = 4604,					// 英雄会武通知信息下发
		MT_WORLD_TEAM_ARENA_TEAM_SIGN_UP_DETAILED_INFO_SC = 4605,	// 英雄会武战队详细信息下发
		MT_WORLD_TEAM_ARENA_ACTIVITY_INFO_SC = 4606,				// 英雄会武活动信息下发
		MT_WORLD_TEAM_ARENA_SCENE_OP_REQ_CS = 4607,					// 英雄会武场景操作请求
		MT_WORLD_TEAM_ARENA_ALL_RANK_INFO_SC = 4608,				// 英雄会武排行榜信息
		MT_WORLD_TEAM_ARENA_MY_RANK_INFO_SC = 4609,					// 英雄会武自身战队排行榜信息
		MT_WORLD_TEAM_ARENA_SPECIAL_INFO_SC = 4610,					// 英雄会武特殊信息下发
		MT_WORLD_TEAM_ARENA_TEAM_SPECIAL_INFO_SC = 4611,			// 英雄会武队伍特殊信息下发
		MT_WORLD_TEAM_ARENA_FIGHT_INFO_SC = 4612,					// 英雄会武发起战斗信息下发
		MT_WORLD_TEAM_ARENA_FINALLY_RANK_REQ_CS = 4613,				// 英雄会武最终榜单请求
		MT_WORLD_TEAM_ARENA_FINALLY_RANK_INFO_SC = 4614,			// 英雄会武最终榜单信息下发
		MT_WORLD_TEAM_ARENA_OTHER_INFO_SC = 4615,					// 英雄会武其它信息下发

		//情缘盛会
		MT_QING_YUAN_SHENG_HUI_REQ_CS = 4650,		//情缘盛会 请求
		MT_QING_YUAN_SHENG_SCENE_INFO_SC = 4651,	//情缘盛会 场景信息
		MT_QING_YUAN_SHENG_ROLE_INFO_SC = 4652,		//情缘盛会 角色信息
		MT_QING_YUAN_SHENG_HUI_QUESTION_RESULT_SC= 4653,		
		MT_QING_YUAN_SHENG_HUI_QUESTION_ROUTE_SC = 4654,		
		MT_QING_YUAN_SHENG_HUI_QUESTION_INFO_SC = 4655,
		MT_QING_YUAN_SHENG_HUI_OPPONENT_INFO_SC = 4656,				
		MT_QING_YUAN_SHENG_HUI_MATCH_OP_ROUTE_SC = 4657,
		MT_QING_YUAN_SHENG_HUI_INFO_SC = 4658,
		MT_QING_YUAN_SHENG_HUI_QI_YUAN_NOTICE_SC = 4659,

		MT_QING_YUAN_SHENG_HUI_REQ_2_CS = 4680,		//情缘盛会 请求2
		
		///////////////////////////////山海明月耀九州///////////////////////////////
		//情缘对对碰
		MT_QINGYUAN_DUIDUIPENG_REQ_CS = 4660,						// 情缘对对碰 请求信息
		MT_QINGYUAN_DUIDUIPENG_START_SC = 4661,						// 情缘对对碰 副本开始通知
		MT_QINGYUAN_DUIDUIPENG_ROLE_FORM_SC = 4662,					// 情缘对对碰 玩家幻化信息
		MT_QINGYUAN_DUIDUIPENG_FB_INFO_SC = 4663,					// 情缘对对碰 副本积分信息
		MT_QINGYUAN_DUIDUIPENG_FINISH_SC = 4664,					// 情缘对对碰 副本结算通知
		MT_QINGYUAN_DUIDUIPENG_RANK_INFO_SC = 4665,					// 情缘对对碰 排行榜信息
		MT_QINGYUAN_DUIDUIPENG_DAY_TIMES_SC = 4666,					// 情缘对对碰 每日次数
	
		// 山海灯会		
		MT_LANTERN_RIDDIE_REQ_CS = 4670,						//山海灯会 客户端请求
		MT_LANTERN_RIDDIE_NPC_INFO_SC = 4671,					//山海灯会 NPC生成
		MT_LANTERN_RIDDIE_TIMEOUT_INFO_SC = 4672,				//山海灯会 答题超时时间戳
		MT_LANTERN_RIDDIE_ANSWER_RESULT_INFO_SC = 4673,			//山海灯会 答题结果信息
		MT_LANTERN_RIDDIE_BASE_INFO_SC = 4674,					//山海灯会 登录时下发
		MT_LANTERN_RIDDIE_PRAISE_INFO_SC = 4675,				//山海灯会 点赞和道具信息下发
		MT_LANTERN_RIDDIE_NOTIFY_INFO_SC = 4676,				//山海灯会 漂字
		MT_LANTERN_RIDDIE_USE_CARD_INFO_SC = 4677,				//山海灯会 使用道具
		MT_LANTERN_RIDDIE_LAST_RESULT_INFO_SC = 4678,			//山海灯会 最终结算

		// 情缘小铺
		MT_LOVERS_MARKET_REQ_CS = 4685,							// 情缘小铺 请求信息
		MT_LOVERS_MARKET_INFO_SC = 4686,						// 情缘小铺 商品信息

		// 缘牵同心结
		MT_CONCENTRIC_KNOT_REQ_CS = 4690,						// 缘牵同心结 请求信息
		MT_CONCENTRIC_KNOT_AGREE_SC = 4691,						// 缘牵同心结 准备通知
		MT_CONCENTRIC_KNOT_START_SC = 4692,						// 缘牵同心结 开始游戏返回
		MT_CONCENTRIC_KNOT_OPEN_CARD_SC = 4693,					// 缘牵同心结 翻牌通知
		MT_CONCENTRIC_KNOT_END_GAME_SC = 4694,					// 缘牵同心结 结束返回
		ME_CONCENTRIC_KNOT_INFO_SC = 4695,						// 缘牵同心结 角色信息
		//周期
		MT_ZHOU_QI_INFO_SC = 4700,					// 周期信息

		//仙子去哪儿
		MT_CA_WHERE_IS_THE_FAIRY_OPERA_REQ_CS = 4730,		// 仙子去哪儿操作请求
		MT_CA_ROLE_WHERE_IS_THE_FAIRY_INFO_SC = 4731,		// 仙子去哪儿玩家信息下发
		MT_CA_WHERE_IS_THE_FAIRY_INFO_SC = 4732,			// 仙子去哪儿仙子信息下发

		//天宫寻宝
		MT_CA_TIAN_GONG_XUN_BAO_OP_REQ_CS = 4735,			//天宫寻宝操作请求
		MT_CA_TIAN_GONG_XUN_BAO_INFO_SC = 4736,				//天宫寻宝信息下发

		MT_WORLD_STATUS_HIDDEN_INFO_SC = 4750,			//世界状态隐藏服信息

		MT_TU_ZI_KUAI_PAO_REQ_CS = 4770,				//兔子快跑请求
		MT_TU_ZI_KUAI_PAO_SCENE_INFO_SC = 4771,				//兔子快跑场景信息
		MT_TU_ZI_KUAI_PAO_INFO_SC = 4772,				//兔子快跑场景信息
		MT_TU_ZI_KUAI_PAO_REST_SC = 4773,				//兔子快跑场景信息
		MT_TU_ZI_KUAI_PAO_ENCOURAGE_RESULT_SC = 4774,			//兔子快跑场景信息
		MT_TU_ZI_KUAI_PAO_RUN_SC = 4775,			//
		MT_TU_ZI_KUAI_PAO_ROLE_INFO_SC = 4776,			//
		MT_TU_ZI_KUAI_PAO_FINISH_NOTICE_SC = 4777,			//
		MT_TU_ZI_KUAI_PAO_POS_INFO_SC = 4778,			//

		MT_JIN_YI_TIAN_XIANG_SC = 4790,			//锦衣天香信息
		MT_YUAN_QI_YAN_HUO_INFO_SC = 4800,			//元气烟火信息

		// 随机活动-三百抽皮肤
		MT_RA_SANBAI_CHOU_INFO_SC = 4805,			// 随机活动-三百抽皮肤 角色信息
		MT_RA_SANBAI_CHOU_DRAW_SC = 4806,			// 随机活动-三百抽皮肤 抽奖返回

		// 圣器
		MT_HOLY_EQUIP_REQ_CS = 4830,					// 圣器 客户端请求
		MT_HOLY_EQUIP_LEVEL_UP_REQ_CS = 4831,			// 圣器 升级请求
		MT_HOLY_EQUIP_HOLY_BAG_ALL_INFO_SC = 4832,		// 圣器 圣器背包全部信息下发
		MT_HOLY_EQUIP_FUSHI_BAG_ALL_INFO_SC = 4833,		// 圣器 符石背包全部信息下发
		MT_HOLY_EQUIP_HOLY_BAG_SIGNLE_INFO_SC = 4834,	// 圣器 圣器背包单个信息下发
		MT_HOLY_EQUIP_LEVEL_UP_SONCUME_INFO_SC = 4835,	// 圣器 圣器圣器/符石合成消耗列表下发
		MT_HOLY_EQUIP_FIRST_GAIN_INFO_SC = 4836,		// 圣器 第一次获得时特殊指引
		MT_HOLY_EQUIP_FOLLOW_INDEX_INFO_SC = 4837,		// 圣器 跟随信息

		// 天下第一比武[组队] 冠军记录手册 4962 ~ 4967
		MT_GET_UTA_CHAMPION_RECORD_CS = 4962 ,		// 客户端获取 冠军记录手册[组队]
		MT_GET_UTA_CHAMPION_RECORD_ACK_SC = 4963,	// 发送 冠军记录手册[组队] 信息


		// 天下第一比武[组队] 竞猜 4968 ~ 4973
		MT_UTA_GUESS_CLIENT_REQ_CS = 4968,			// UTA竞猜 客户端操作
		MT_UTA_GUESS_MATCH_INFO_SC = 4969,			// UTA竞猜 某一个比赛的竞猜信息
		MT_UTA_GUESS_BET_RECORD_SC = 4970,			// UTA竞猜 玩家参与下注的列表
		MT_UTA_GUESS_HISTORY_EARNING_SC = 4971,		// UTA竞猜 累计收益下发
		MT_UTA_GUESS_HAS_BET_REWARD_SC = 4972,		// UTA竞猜 有可领取奖励通知

		////////////////////////////// 天下第一武道大会 ///////////////////////////////
		MT_UTA_SC_BEGIN = 5000,
		MT_UTA_BASE_INFO_SC = 5001,
		MT_UTA_TEAM_LIST_SC = 5002,
		MT_UTA_MY_TEAM_QUALIFICATION_SCHEDULES_SC = 5003,
		MT_UTA_TEAM_DETAIL_INFO_SC = 5004,	
		MT_UTA_MY_TEAM_BASE_INFO_SC = 5005,
		MT_UTA_TEAM_SCORE_RANK_LIST_SC = 5006,
		MT_UTA_MAIN_SCHEDULES_SC = 5007,
		MT_UTA_FINAL_RANK_LIST_SC = 5008,
		MT_UTA_OVERVIEW_SCHEDULES_SC = 5009,
		MT_UTA_QUALIFICATION_SCHEDULES_SC = 5010,
		MT_UTA_SCENE_INFO_SC = 5011,
		MT_UTA_BATTLE_VS_INFO = 5012,
		MT_UTA_PREPARE_ENTER_SCENE_SC = 5013,
		MT_UTA_BATTLE_RESULT_SHOW_SC = 5014,

		MT_UTA_CS_BEGIN = 5050,
		MT_UTA_TEAM_LIST_CS = 5051,		// 请求参赛队伍
		MT_UTA_TEAM_DETAIL_CS = 5052,	// 请求某个队伍详细信息
		MT_UTA_MY_TEAM_QUALIFICATION_SCHEDULES_CS = 5053, // 请求我的队伍资格赛赛程	
		MT_UTA_FETCH_WIN_BOX_CS = 5054,	// 请求领取胜场宝箱
		MT_UTA_TEAM_QUALIFICATION_RANK_LIST_CS = 5055,	// 请求资格赛排行榜
		MT_UTA_MAIN_SCHEDULES_CS = 5056,// 请求正赛赛程
		MT_UTA_FINAL_RANK_LIST_CS = 5057,	//请求最终榜单
		MT_UTA_QUALIFICATION_SCHEDULES_CS = 5058,	//请求资格赛赛程
		MT_UTA_MAIN_SCHEDULES_OVERVIEW_CS = 5059,	//请求正赛赛程总览
		MT_UTA_ENTER_SCENE_CS = 5060,	// 请求进入场景
		MT_UTA_WATCH_VIDEO_CS = 5061,	// 查看战报录像
		MT_UTA_START_OB_CS = 5062,		// 开始观战

		//鸿蒙天界
		MT_HONGMENG_TIANJIE_REQ_CS = 5100,				// 鸿蒙天界 请求
		MT_HONGMENG_TIANJIE_SCENE_INFO_SC = 5101,		// 鸿蒙天界 场景信息
		MT_HONGMENG_TIANJIE_ROLE_INFO_SC = 5102,		// 鸿蒙天界 个人信息
		MT_HONGMENG_TIANJIE_WORLD_INFO_SC = 5103,		// 鸿蒙天界 全服BOSS 信息
		MT_HONGMENG_TIANJIE_TALK_INFO_SC = 5104,		// 鸿蒙天界 场景对话信息转发
		MT_HONGMENG_TIANJIE_REWARD_INFO_SC = 5105,		// 鸿蒙天界 奖励记录
		MT_HONGMENG_TIANJIE_PASS_TEAM_SC = 5106,		// 鸿蒙天界 首通队伍信息

		//造化装备
		MT_ZAO_HUA_EQUIPMENT_REQ_CS = 5120,				// 造化装备 请求
		MT_ZAO_HUA_EQUIPMENT_REFINE_SC = 5121,				// 造化装备精炼信息
		MT_ZAO_HUA_YUAN_HUN_ZHU_REQ_CS = 5122,				// 造化装备元魂珠请求
		MT_ZAO_HUA_YUAN_HUN_ZHU_UP_LEVEL_CS = 5123,			// 造化装备元魂珠升级
		MT_ZAO_HUA_LONG_ZHU_INFO_SC = 5124,			// 造化装备珑铸信息
		MT_ZAO_HUA_LONG_ZHU_SLOT_INFO_SC = 5125,			// 造化装备珑铸孔信息
	

		MT_DUAN_YANG_ZI_XUAN_INFO_SC = 5130,		// 端阳自选
		MT_DUAN_YANG_FU_LI_INFO_SC = 5131,			// 端阳福礼

		MT_JIN_LONG_JU_BAO_INFO_SC = 5135,		// 金龙聚宝信息
		MT_JIN_LONG_JU_BAO_DRAW_RET_SC = 5136,		// 金龙聚宝抽奖返回

		MT_SERVER_COMPETITION_ALL_RANK_INFO_SC = 5137,		// 新服比拼全部排行榜信息

		// 科举考试
		MT_KE_JU_EXAM_REQ_CS = 5140,						//科举考试 客户端请求
		MT_KE_JU_EXAM_NPC_INFO_SC = 5141,					//科举考试 NPC生成
		MT_KE_JU_EXAM_TIMEOUT_INFO_SC = 5142,				//科举考试 答题超时时间戳
		MT_KE_JU_EXAM_ANSWER_RESULT_INFO_SC = 5143,			//科举考试 答题结果信息
		MT_KE_JU_EXAM_BASE_INFO_SC = 5144,					//科举考试 登录时下发
		MT_KE_JU_EXAM_PRAISE_INFO_SC = 5145,				//科举考试 点赞和道具信息下发
		MT_KE_JU_EXAM_NOTIFY_INFO_SC = 5146,				//科举考试 漂字
		MT_KE_JU_EXAM_USE_CARD_INFO_SC = 5147,				//科举考试 使用道具
		MT_KE_JU_EXAM_LAST_RESULT_INFO_SC = 5148,			//科举考试 最终结算

		//我要上学
		MT_GOTO_SCHOOL_REQ_CS = 5150,				// 我要上学请求
		MT_GOTO_SCHOOL_INFO_SC = 5151,				// 我要上学请求信息

		MT_ZHOU_QI_SHOP_REQ_CS = 5160,				// 周期商店请求
		MT_ZHOU_QI_SHOP_INFO_SC = 5161,				// 周期商店信息

		
		MT_PARTNER_PET_RANK_REQ_CS = 5162,		// 伙伴/宠物前十排行榜请求
		MT_PET_TOP_RANK_INFO_SC = 5163,			// 宠物前十信息下发
		MT_PARTNER_TOP_RANK_INFO_SC = 5164,		// 伙伴前十信息下发

		MT_PET_STRENGTHEN_BUFF_INFO = 5170,				// 宠物共鸣信息

		MT_SHEN_SHOU_EN_ZE_INFO_SC = 5180,				// 神兽恩泽信息
		MT_SHEN_SHOU_EN_ZE_DRAW_RESULT_SC = 5181,		// 神兽恩泽抽奖返回
		MT_SHEN_SHOU_EN_ZE_RECORD_INFO_SC = 5182,		// 神兽恩泽抽奖记录
	
		MT_ELEMENT_ELF_REWARDS_SC = 5185,				// 元素精灵奖励通知

		// 符玉
		MT_FU_YU_SINGLE_INFO_SC = 5186,			// 已穿戴符玉单条信息下发
		MT_FU_YU_ALL_INFO_SC = 5187,			// 已穿戴符玉全部信息下发
		MT_FU_YU_OPERATE_REQ_CS = 5188,			// 符玉请求

		MT_RAND_ACTIVITY_XIAN_SHI_BAO_HE_3_SC = 5190,				// 限时宝盒3
		MT_SHI_TU_OP_RET_SC = 5191,									// 师徒操作通知
		MT_RAND_ACTIVITY_XIAN_SHI_BAO_HE_4_SC = 5200,				// 限时宝盒4
		MT_RAND_ACTIVITY_DING_ZHI_BAO_XIANG_3_INFO_SC = 5210,		// 定制宝箱3

		MT_RAND_ACTIVITY_XIN_FU_LING_CHONG_INFO = 5220,				// 新服灵宠信息下发
																			
		// 随机活动-天衣耀世
		MT_RAND_ACTIVITY_TIAN_YI_YAO_SHI_INFO_SC = 5221,
		MT_RAND_ACTIVITY_HUN_QI_FAN_LI_INFO_SC = 5222,		// 随机活动-魂器返利	

		MT_RAND_ACTIVITY_SHEN_CI_HUA_FU_INFO_SC = 5225,				// 神赐华福信息
		MT_RAND_ACTIVITY_SHEN_CI_HUA_FU_DRAW_RET_SC = 5226,			// 神赐华福信息
		MT_PET_LIAN_YAO_COMMON_INFO_SC = 5230,					
		MT_PET_ADD_EXP_NOTICE_SC = 5231,			

		//跨服英雄录
		MT_KUA_FU_YING_XIONG_LU_REQ_CS = 5310,						// 跨服英雄录请求
		MT_KUA_FU_YING_XIONG_LU_ACTIVITY_INFO_SC = 5311,			// 跨服英雄录活动信息下发
		MT_KUA_FU_YING_XIONG_LU_ROLE_ALL_INFO_SC = 5312,			// 跨服英雄录角色所有信息下发
		MT_KUA_FU_YING_XIONG_LU_ROLE_SINGLE_RANK_INFO_SC = 5313,	// 跨服英雄录角色单个榜单信息下发
		MT_KUA_FU_YING_XIONG_LU_ROLE_SINGLE_TASK_INFO_SC = 5314,	// 跨服英雄录角色单个任务信息下发
		MT_KUA_FU_YING_XIONG_LU_ROLE_RANK_VALUE_SC = 5315,			// 跨服英雄录角色榜单数值列表信息下发

		// 星图系统
		MT_STAR_CHART_REQUEST_CS = 5320,					// 星图系统请求
		MT_STAR_CHART_INFO_SC = 5321,						// 星图系统信息下发
		MT_STAR_CHART_INSTALL_SKILL_NOTICE_SC = 5322,		// 星图装备技能通知

		//弑神之塔
		MT_NEW_COURAGE_CHALLENGE_REQ_CS = 5330,					// 弑神之塔请求
		MT_NEW_COURAGE_CHALLENGE_LEVEL_LIST_INFO_SC = 5331,		// 弑神之塔关卡列表信息
		MT_NEW_COURAGE_CHALLENGE_LEVEL_SIGNLE_INFO_SC = 5332,	// 弑神之塔关卡单个信息
		MT_NEW_COURAGE_CHALLENGE_REWARD_INFO_SC = 5333,			// 弑神之塔奖励信息


		// 月夕灯谜
		MT_RA_YUE_XI_DENG_MI_REQ_CS = 5340,					// 随机活动-月夕灯谜 请求
		MT_RA_YUE_XI_DENG_MI_INFO_SC = 5341,				// 随机活动-月夕灯谜 活动信息下发
		// 月夕累充
		MT_RA_YUE_XI_LEI_CHONG_INFO_SC = 5342,				// 随机活动-月夕累充 活动信息下发
		// 月夕灯谜补充
		MT_RA_YUE_XI_DENG_MI_ANSWER_RESULT_SC = 5343,		// 随机活动-月夕灯谜 答题结果下发
		
		//随机活动 - 单笔返利
		MT_RA_DAN_BI_FAN_LI_INFO_SC = 5345,					// 单笔返利信息下发
		MT_RA_DAN_BI_FAN_LI_SINGLE_INFO_SC = 5346,			// 单笔返利单条信息下发

		MT_RAND_ACTIVITY_ZHUI_YUE_SHANG_DIAN_INFO_SC = 5350,				// 追月商店
		MT_RAND_ACTIVITY_ZHUI_YUE_SHANG_DIAN_DRAW_RET_SC = 5351,			// 追月商店
		MT_RAND_ACTIVITY_ZHUI_YUE_SHANG_DRAW_RECORD_SC = 5352,			// 追月商店

		MT_GUA_JI_EXP_INFO_SC = 5355,				// 每日挂机经验

		//幻兽系统
		MT_HUAN_SHOU_REQ_CS = 5360,					//幻兽请求
		MT_HUAN_SHOU_ALL_INFO_SC = 5361,			//所有幻兽信息下发
		MT_HUAN_SHOU_SINGLE_INFO_SC = 5362,			//单个幻兽信息下发
		MT_HUAN_SHOU_NOTICE_INFO_SC = 5363,			//幻兽信息通知下发
		MT_HUAN_SHOU_DECOMPOSE_REQ_CS = 5364,		//幻兽灵魄分解请求
		MT_HUAN_SHOU_OTHER_INFO_SC = 5365,			//幻兽其它信息下发
		MT_HUAN_SHOU_ANNOUNCEMENT_INFO_SC = 5366,	//幻兽公告信息下发

		MT_HUAN_SHOU_BAG_ALL_INFO_SC = 5370,		//幻兽背包所有信息下发
		MT_HUAN_SHOU_BAG_SINGLE_INFO_SC = 5371,		//幻兽背包单条信息下发


		//
		MT_ROLE_GUILD_TASK_REQ_CS = 5400,			// 家族任务请求
		MT_ROLE_GUILD_TASK_INFO_SC = 5401,			// 家族任务
		MT_ROLE_GUILD_CHANGE_SC = 5402,			// 家族任务

		// 实名认证奖励
		MT_REAL_NAME_REWARD_RECEIVE_CS = 5410,		// 实名认证奖励领取请求
		MT_REAL_NAME_REWARD_RECEIVE_INFO_SC = 5411,	// 实名认证奖励领取信息下发
	};
}

#endif