#ifndef __ACTIVITY_DEF_HPP__
#define __ACTIVITY_DEF_HPP__

#include <time.h>

#include "servercommon/servercommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "gameworld/gameworld/gamedef.h"
#include "servercommon/serverdef.h"
#include "servercommon/silkroadsdef.hpp"

static const int MAX_ONCE_ACTIVITY_ROLE_DATA_COUNT = 100;					// 一次操作角色数据最大数量
static const int INT_TYPE_BIT = 32;
static const int CLIENT_ACTIVITY_TYPE_BEGIN = 10000;
static const int MAX_ACTIVITY_TYPE_NUM = 64;


enum BUY_TIME_TYPE
{
	BUY_TIME_TYPE_LUCKY_BAG = 1,						//购买福袋
	BUY_TIME_TYPE_MYSTERIOUS_TRIAL = 2,					//购买神秘试炼每日折扣 
	BUY_TIME_TYPE_MONTH_PURCHASE = 3,					//购买月度投资
	BUY_TIME_TYPE_GOD_TOKEN = 4,						//购买封神战令
	BUY_TIME_TYPE_CAI_SHEN_JU_BAO = 5,					//购买财神聚宝		param1 配置seq
	BUY_TIME_TYPE_DING_ZHI_BAO_XIANG = 6,				//购买定制宝箱	 param1 配置seq，param2 选择的宝箱按位来
	BUY_TIME_TYPE_ZHUAN_SHU_LI_BAO = 7,					//购买专属礼包  param1 活动协议下发的type
	BUY_TIME_TYPE_XIAN_SHI_BAO_HE = 8,					//购买限时宝盒  param1 配置seq
	BUY_TIME_TYPE_XIAN_SHI_BAO_DAI = 9,					//购买限时宝袋  param1 配置seq
	BUY_TIME_TYPE_COW_EXCHANGE = 10,					//购买牛运兑换  param1 索引
	BUY_TIME_TYPE_SPRING_FAESTIVAL_STORE = 11,			//购买年货节  param1 索引
	BUY_TIME_TYPE_COW_STORE = 12,						// 购买牛运商场 param1 索引
	BUY_TIME_TYPE_SECRET_TREASURE = 13,					// 购买特惠瑰宝 param1 索引
	BUY_TIME_TYPE_ZU_HE_HE_LI = 14,						// 组合贺礼 param1 能一次买多个 ，按位(bit),配置的type
	BUY_TIME_TYPE_ZU_HE_HE_LI_2 = 15,					// 组合贺礼 param1 (1251协议 int1) ，param2 (1251协议 int2)
	BUY_TIME_TYPE_FASHION_SHOP = 16,					// 时装商店 param1 (1251协议 int1) ， param2 (1251协议 int2)
	BUY_TIME_TYPE_CUMULATIVE_RECHARGE = 17,				// 累计充值(神器惊世) param1 (1251协议 int1)
	BUY_TIME_TYPE_ONE_YUAN_GOU = 18,					//一元购 param1 (1251协议 int1) ， param2 (1251协议 int2)
	BUY_TIME_TYPE_LING_YU_JI_JIN = 19,					//灵玉基金 param1 (1251协议 int1) ， param2 (1251协议 int2)
	BUY_TIME_TYPE_SHEN_SHOU_JIANG_LIN = 20,				//神兽降临直购 param1 (1251协议 int1) ， param2 (1251协议 int2)
	BUY_TIME_TYPE_LUCKY_BAG_NEW = 21,					//购买福袋	param1 (1251协议 int1) ， param2 (1251协议 int2)
	BUY_TIME_TYPE_CAI_SHEN_JU_BAO_NEW = 22,				//购买财神聚宝		param1 (1251协议 int1) ， param2 (1251协议 int2)
	BUY_TIME_TYPE_DING_ZHI_BAO_XIANG_NEW = 23,			//购买定制宝箱	 param1 (1251协议 int1) ， param2 (1251协议 int2)
	BUY_TIME_TYPE_ZHUAN_SHU_LI_BAO_NEW = 24,			//购买专属礼包  param1 (1251协议 int1) ， param2 (1251协议 int2)
	BUY_TIME_TYPE_XIAN_SHI_BAO_HE_NEW = 25,				//购买限时宝盒  param1 (1251协议 int1) ， param2 (1251协议 int2)
	BUY_TIME_TYPE_XIAN_SHI_BAO_DAI_NEW = 26,			//购买限时宝袋  pparam1 (1251协议 int1) ， param2 (1251协议 int2)
	BUY_TIME_TYPE_MYSTERIOUS_TRIAL_NEW = 27,			//购买神秘试炼每日折扣 param1 (1251协议 int1) ，param2 (1251协议 int2)
	BUY_TIME_TYPE_MONTH_PURCHASE_NEW = 28,				//购买月度投资 param1 (1251协议 int1) ，param2 (1251协议 int2)
	BUY_TIME_TYPE_ZHUAN_SHU_LI_BAO_DAY_GIFT = 29,		//购买专属礼包-每日礼包 param1 (1251协议 int1) ，param2 (1251协议 int2)
	BUY_TIME_TYPE_FIRST_RECHARGE_EXCLUSIVE = 30,	//购买首充专属-成长助力 param1 (1251协议 int1) ，param2 (1251协议 int2)
	BUY_TIME_TYPE_MONTH_CARD = 31,					//购买周卡/月卡/共用商城 param1 (1251协议 int1) ，param2 (1251协议 int2)
	BUY_TIME_TYPE_SUMMER_STORE = 32,					// 夏日商店 param1 (1251协议 int1) ，param2 (1251协议 int2)
	BUY_TIME_TYPE_SUPER_OFF_SHOP = 33,					// 超级折扣商城 param1 (1251协议 int1) ，param2 (1251协议 int2)
	BUY_TIME_TYPE_ZHEN_PIN_SHANG_PU = 34,				// 珍品商铺 param1 (1251协议 int1) ，param2 (1251协议 int2)
	BUY_TIME_TYPE_LUCKY_BAG_BATCH = 35,				// 福袋购买 批量N天 param1 (1251协议 int1) ，param2 (1251协议 int2)
	BUY_TIME_TYPE_WAN_LING_GONG_YING = 36,				// 购买万灵共盈活动领取奖励资格
	BUY_TIME_TYPE_HORCRUX_TIANCHENG = 37,				//购买魂器天成礼包 param1: shop_index
	BUY_TIME_TYPE_SHAN_HAI_MET = 38,					// 购买山海初遇活动双倍领取奖励资格
	BUY_TIME_TYPE_SHAN_HAI_MEET = 39,					//购买激活山海相逢
	BUY_TIME_TYPE_ZHAOCAIJINBAO = 40,					//购买招财进宝礼包
	BUY_TIME_TYPE_YUN_ZE_SLOT_MACHINE = 41,				// 购买纷享同花礼钥匙
	BUY_TYME_TYPE_CHAOZHI_FENXIANG = 42,				// 超值纷享
	BUY_TIME_TYPE_COLORFUL_DOUBLE = 43,				// 纷享加倍		p_1:激活类型 p_2:box_seq
	BUY_TIME_TYPE_SHAN_HAI_BAO_XIANG = 44,			// 购买山海宝箱	 param1 (1251协议 int1) ， param2 (1251协议 int2)
	BUY_TIME_TYPE_SHAN_HAI_BAO_HE = 45,				// 购买山海宝盒	 param1 (1251协议 int1) ， param2 (1251协议 int2)
	BUY_TIME_TYPE_SHAN_HAI_BAO_DAI = 46,			// 购买山海宝袋	 param1 (1251协议 int1) ， param2 (1251协议 int2)
	BUY_TIME_TYPE_DING_ZHI_BAO_XIANG_2 = 47,		//购买定制宝箱2	 param1 (1251协议 int1) ， param2 (1251协议 int2)
	BUY_TIME_TYPE_XIAN_SHI_BAO_DAI_2 = 48,			//购买限时宝袋2	 param1 (1251协议 int1) ， param2 (1251协议 int2)
	BUY_TIME_TYPE_XIAN_SHI_BAO_HE_2 = 49,			//购买限时宝盒2	 param1 (1251协议 int1) ， param2 (1251协议 int2)
	BUY_TIME_TYPE_JIXING_GAOZHAO = 50,				// 购买吉星高照 param1: shop_index
	BUY_TIME_TYPE_WEEKEND_GIFT = 51,				// 周末礼包 param1: gift_index
	BUY_TIME_TYPE_CHAO_ZHI_GIFT = 52,				// 超值献礼
	BUY_TIME_TYPE_JINGJI_YUEKA = 53,				// 购买竞技月卡  param1 (1251协议 int1) ， param2 (1251协议 int2)
	BUY_TIME_TYPE_YINHU_WENSHI = 54,					// 购买 寅虎问世
	BUY_TIME_TYPE_YIN_HU_BAI_SUI = 55,					// 购买 寅虎拜岁
	BUY_TIME_TYPE_QING_YUAN_SHIZHUANG = 56,			// 购买情缘时装 param1:buy_index
	BUY_TIME_TYPE_YAO_XING_SHI_LIAN = 57,			// 购买妖行试炼 param1:seq param2:param1
	BUY_TIME_TYPE_GOD_BEAST_ADVENT = 58,				// 购买 异兽临世
	BUY_TIME_TYPE_WAN_LING_ZHU_LI = 59,				// 购买万灵筑礼活动领取奖励资格
	BUY_TIME_TYPE_JIN_YI_TIAN_XIANG = 60,				// 购买 锦衣天香
	BUY_TIME_TYPE_YUAN_QI_YAN_HUO = 61,			//购买元气烟火	 param1 (1251协议 int1) ， param2 (1251协议 int2)
	BUY_TIME_TYPE_YUAN_QI_JISHIBU = 62,				// 购买元气记事簿领取奖励资格
	BUY_TIME_TYPE_DUAN_YANG_ZI_XUAN = 63,			// 购买端阳自选	 param1 (1251协议 int1) ， param2 (1251协议 int2)
	BUY_TIME_TYPE_LUCKY_BLIND_BOX = 64,				// 购买幸运盲盒	 param1 (1251协议 int1) ， param2 (1251协议 int2)
	BUY_TIME_TYPE_DUAN_YANG_FU_LI = 65,				// 购买端阳福礼	 param1 (1251协议 int1) ， param2 (1251协议 int2)
	BUY_TIME_TYPE_JIN_LONG_JU_BAO = 66,				// 购买金龙聚宝	 param1 (1251协议 int1) ， param2 (1251协议 int2)
	BUY_TIME_TYPE_SHEN_SHOU_EN_ZE = 67,				// 购买神兽恩泽	 param1 (1251协议 int1) ， param2 (1251协议 int2)
	BUY_TIME_TYPE_XIAN_SHI_BAO_HE_3 = 68,			// 购买限时宝盒3 (四季花牌)  param1 (1251协议 int1) ， param2 (1251协议 int2)
	BUY_TIME_TYPE_XIAN_SHI_BAO_HE_4 = 69,			// 购买限时宝盒4 (四季宝盒)  param1 (1251协议 int1) ， param2 (1251协议 int2)
	BUY_TIME_TYPE_PET_EXCHANGE_SHOP = 70,			// 宠物兑换-商店	 param1 (1251协议 int1) ， param2 (1251协议 int2)
	BUY_TIME_TYPE_QI_XI_TE_HUI = 71,				// 购买七夕特惠	 param1 (1251协议 int1) ， param2 (1251协议 int2)	
	BUY_TIME_TYPE_DING_ZHI_BAO_XIANG_3 = 72,		// 购买定制宝箱3 (四季宝箱)  param1 (1251协议 int1) ， param2 (1251协议 int2)
	BUY_TIME_TYPE_XIN_FU_LING_CHONG = 73,			// 购买新服灵宠	 param1 (1251协议 int1) ， param2 (1251协议 int2)	
	BUY_TIME_TYPE_SHEN_CI_HUA_FU = 74,				// 购买神赐华福   param1 (1251协议 int1) ， param2 (1251协议 int2)
	BUY_TIME_TYPE_TIAN_YI_YAO_SHI = 75,				// 购买天衣耀世 param1 (1251协议 int1) ， param2 (1251协议 int2)
	BUY_TIME_TYPE_NEW_COURAGE_CHALLENGE = 76,		// 购买弑神之塔战令
	BUY_TIME_TYPE_ZHUI_YUE_SHANG_DIAN = 77,			// 购买追月商店 param1 (1251协议 int1) ， param2 (1251协议 int2)
	BUY_TIME_TYPE_DAN_BI_FAN_LI = 78,				// 购买单笔返利 param1 (1251协议 int1) ， param2 (1251协议 int2)
	BUY_TIME_TYPE_MAX
	
};

enum BUY_GIFT_ITEM_RET_TYPE
{
	BUY_GIFT_ITEM_RET_TYPE_1 = 1,		//参数错误
	BUY_GIFT_ITEM_RET_TYPE_2 = 2,		//次数不足
};

enum BUY_GIFT_ITEM_TYPE
{
	BUY_GIFT_ITEM_TYPE_INVALID = 0,

	BUY_GIFT_ITEM_TYPE_LUCKY_BAG = 1,							//!< 购买福袋 param2 - seq
	BUY_GIFT_ITEM_TYPE_MYSTERIOUS_TRIAL = 2,					//!< 购买神秘试炼每日折扣 param3 - day
	BUY_GIFT_ITEM_TYPE_GOD_TOKEN = 3,							//!< 购买封神战令 
	BUY_GIFT_ITEM_TYPE_CAI_SHEN_JU_BAO = 4,						//!< 购买财神聚宝 param2 - seq
	BUY_GIFT_ITEM_TYPE_DING_ZHI_BAO_XIANG = 5,					//!< 购买定制宝箱 param2 - seq
	BUY_GIFT_ITEM_TYPE_BUY_TIME_TYPE_ZHUAN_SHU_LI_BAO = 6,		//!< 购买专属礼包 param2 - type(无法自适应)
	BUY_GIFT_ITEM_TYPE_XIAN_SHI_BAO_HE = 7,						//!< 购买限时宝盒 param2 - seq
	BUY_GIFT_ITEM_TYPE_XIAN_SHI_BAO_DAI = 8,					//!< 购买限时宝袋 param2 - seq
	BUY_GIFT_ITEM_TYPE_ZU_HE_HE_LI = 9,							//!< 购买组合贺礼 param2 - seq1 param3 - seq2	(param2必须要有值)
	BUY_GIFT_ITEM_TYPE_SPRING_FAESTIVAL_STORE = 10,				//!< 购买年货节   param2 - seq
	BUY_GIFT_ITEM_TYPE_COW_STORE = 11,							//!< 购买牛运商城 param2 - seq
	BUY_GIFT_ITEM_TYPE_FASHION_SHOP = 12,						//!< 购买时装商店 param2 - seq
	BUY_GIFT_ITEM_TYPE_LING_YU_JI_JIN = 13,						//!< 购买灵玉基金 param2 - phase
	BUY_GIFT_ITEM_TYPE_CUMULATIVE_RECHARG = 14,					//!< 购买累计充值(神装惊世) 
	BUY_GIFT_ITEM_TYPE_MONTH_CARD = 15,							//!< 购买月卡/周卡 param2 - 根据P3 param3 - type(0:p2填card_type 1:p2填商品seq)
	BUY_GIFT_ITEM_TYPE_SHAN_HAI_BAO_XIANG = 16,					//!< 购买山海宝箱 param2 - seq
	BUY_GIFT_ITEM_TYPE_SHAN_HAI_BAO_DAI = 17,					//!< 购买山海宝袋 param2 - seq	
	BUY_GIFT_ITEM_TYPE_SHAN_HAI_BAO_HE = 18,					//!< 购买山海宝盒 param2 - seq
	BUY_GIFT_ITEM_TYPE_SHAN_HAI_MET = 19,						//!< 山海初遇 
	BUY_GIFT_ITEM_TYPE_SHAN_HAI_MEET = 20,						//!< 山海相逢
	BUY_GIFT_ITEM_TYPE_ZHAO_CAI_JIN_BAO = 21,					//!< 招财进宝 param2:seq
	BUY_GIFT_ITEM_TYPE_SHEN_SHOU_JIANG_LIN = 22,				//!< 神兽降临 param2:seq
	BUY_GIFT_ITEM_TYPE_WAN_LING_GONG_YING = 23,					//!< 万灵共盈
	BUY_GIFT_ITEM_TYPE_HORCRUX_TIANCHENG = 24,					//!< 魂器天成 param2:shop_index
	BUY_GIFT_ITEM_TYPE_YUN_ZE_SLOT_MACHINE = 25,				//!< 纷享同花礼
	BUY_GIFT_ITEM_TYPE_CHAOZHI_FENXIANG = 26,					//!< 超值纷享
	BUY_GIFT_ITEM_TYPE_DING_ZHI_BAO_XIANG_2 = 27,				//!< 云泽宝箱(定制宝箱2) param2:seq
	BUY_GIFT_ITEM_TYPE_XIAN_SHI_BAO_HE_2 = 28,					//!< 云泽宝盒(限时宝盒2) param2:seq
	BUY_GIFT_ITEM_TYPE_XIAN_SHI_BAO_DAI_2 = 29,					//!< 云泽宝袋(限时宝袋2) param2:seq
	BUY_GIFT_ITEM_TYPE_JIXIANG_GAOZHAO = 30,					//!< 吉星高照 param2:shop_index
	BUY_GIFT_ITEM_TYPE_CHAO_ZHI_GIFT = 31,						//!< 超值献礼
	BUY_GIFT_ITEM_TYPE_YINHU_WENSHI = 32,						//!< 寅虎问世
	BUY_GIFT_ITEM_TYPE_YIN_HU_BAI_SUI = 33,						//!< 寅虎拜岁  
	BUY_GIFT_ITEM_TYPE_ZHUAN_SHU_LI_BAO_DAY_GIFT = 34,			//!< 专属礼包-每日礼包 param2 - phase param3:type
	BUY_GIFT_ITEM_TYPE_COLORFUL_DOUBLE = 35,					//!< 缤纷加倍 P2 box_seq
	BUY_GIFT_ITEM_TYPE_YAO_XING_SHI_LIAN = 36,					//!< 妖行试炼  和 1252 协议p1 p2 相同
	BUY_GIFT_ITEM_TYPE_WANLING_ZHULI = 37,						//!< 万灵筑礼
	BUY_GIFT_ITEM_TYPE_JIN_YI_TIAN_XIANG = 38,					//!< 购买 锦衣天香
	BUY_GIFT_ITEM_TYPE_YUAN_QI_YAN_HUO = 39,					//!< 购买元气烟火	 
	BUY_GIFT_ITEM_TYPE_YUAN_QI_JISHIBU = 40,					//!< 购买元气记事簿领取奖励资格
	BUY_GIFT_ITEM_TYPE_LUCKY_BAG_BATCH = 41,					//!< 福袋购买 批量N天 param2:seq
	BUY_GIFT_ITEM_TYPE_CHONGZHI = 42,							//!< 充值 param2:chongzhi_num
	BUY_GIFT_ITEM_TYPE_FIRST_RECHARGE_EXCLUSIVE = 43,			//!< 首充专属-成长助力 param2:first_recharge_cmd_type
	BUY_GIFT_ITEM_TYPE_LUCKY_BLIND_BOX = 44,					//!< 幸运盲盒 P2:seq
	BUY_GIFT_ITEM_TYPE_DUAN_YANG_FU_LI = 45,					//!< 端阳福礼 P2:seq P3:is_buy_all
	BUY_GIFT_ITEM_TYPE_DUAN_YANG_ZI_XUAN = 46,					//!< 端阳自选
	BUY_GIFT_ITEM_TYPE_JIN_LONG_JU_BAO = 47,					//!< 金龙聚宝 P2:seq
	BUY_GIFT_ITEM_TYPE_SHEN_SHOU_EN_ZE = 48,					//!< 购买神兽恩泽 P2:seq
	BUY_GIFT_ITEM_TYPE_XIAN_SHI_BAO_HE_3 = 49,					//!< 限时宝盒3 (四季花牌) param2:seq
	BUY_GIFT_ITEM_TYPE_XIAN_SHI_BAO_HE_4 = 50,					//!< 限时宝盒4 (四季宝盒) param2:seq
	BUY_GIFT_ITEM_TYPE_PET_EXCHANGE_SHOP = 51,					//!< 宠物兑换-商店 param2:seq
	BUY_GIFT_ITEM_TYPE_QI_XI_TE_HUI = 52,						//!< 七夕特惠 p2:seq
	BUY_GIFT_ITEM_TYPE_DING_ZHI_BAO_XIANG_3 = 53,				//!< 定制宝箱3 (四季宝箱) param2 - seq
	BUY_GIFT_ITEM_TYPE_XIN_FU_LING_CHONG = 54,					//!< 新服灵宠
	BUY_GIFT_ITEM_TYPE_QING_YUAN_SHIZHUANG = 55,				//!< 购买情缘时装 param2:buy_index
	BUY_GIFT_ITEM_TYPE_ZHEN_PIN_SHANG_PU = 56,					//!< 珍品商铺 param2:seq param3:select_flag
	BUY_GIFT_ITEM_TYPE_TIAN_YI_YAO_SHI = 57,					//!< 天衣耀世 param2 - seq
	BUY_GIFT_ITEM_TYPE_SHEN_CI_HUA_FU = 58,						//!< 购买神赐华福 param2 - seq
	BUY_GIFT_ITEM_TYPE_NEW_COURAGE_CHALLENGE = 59,				//!< 弑神之塔战令
	BUY_GIFT_ITEM_TYPE_DAN_BI_FAN_LI = 60,						//!< 单笔返利 param2:pool_seq param3:seq
	BUY_GIFT_ITEM_TYPE_ZHUI_YUE_SHANG_DIAN = 61,				//!< 追月商店 param2 - seq

	BUY_GIFT_ITEM_TYPE_MAX
};


//直购加了,这里也顺便加下/顺便通知客户端也加下
UNSTD_STATIC_CHECK(BUY_TIME_TYPE_MAX == 79);
UNSTD_STATIC_CHECK(BUY_GIFT_ITEM_TYPE_MAX == 62);	

enum ACTIVITY_TYPE
{
	ACTIVITY_TYPE_INVALID = 0,
	ACTIVITY_TYPE_WORLD_BOSS = 1,				// 讨伐世界BOSS
	ACTIVITY_TYPE_TEAM_FIGHT = 2,				// 小队竞技
	ACTIVITY_TYPE_MAZE_RACER = 3,				// 迷宫竞速
	ACTIVITY_TYPE_BRAVE_GROUND = 4,				// 勇闯地宫
	ACTIVITY_TYPE_GUILD_FIGHT = 5,				// 家族大乱斗
	ACTIVITY_TYPE_CHIEF_ELECTION = 6,			// 首席竞选
	ACTIVITY_TYPE_CHIEF_PEAK_BATTLE = 7,		// 首席巅峰战
	ACTIVITY_TYPE_GUILD_HONOR_BATTLE = 8,		// 家族荣耀战
	ACTIVITY_TYPE_GUILD_ANSWER = 9,				// 家族答题
	ACTIVITY_TYPE_WORLD_BOSS_2 = 10,			// 讨伐世界BOSS2 (巨龙boss)
	ACTIVITY_TYPE_ONLY_FIGHT = 11,				// 以一敌百(个人版家族大乱斗)
	ACTIVITY_TYPE_HUAN_JIE_ZHAN_CHANG = 12,		// 幻界战场
	ACTIVITY_TYPE_RELIC_LOOTING = 13,			// 遗迹夺宝
	ACTIVITY_TYPE_HUNDRED_GHOST = 14,			// 百鬼夜行
	ACTIVITY_TYPE_SILK_ROADS = 15,				// 丝绸之路
	ACTIVITY_TYPE_WORLD_TEAM_ARENA = 16,		// 英雄会武
	ACTIVITY_TYPE_QING_YUAN_SHENG_HUI = 17,		// 情缘盛会
	ACTIVITY_TYPE_WORLD_BOSS_3 = 18,			// 讨伐世界BOSS3 (天神降临)

	ACTIVITY_TYPE_MAX,
};
UNSTD_STATIC_CHECK(ACTIVITY_TYPE_MAX < MAX_ACTIVITY_TYPE_NUM);
UNSTD_STATIC_CHECK(ACTIVITY_TYPE_MAX < CLIENT_ACTIVITY_TYPE_BEGIN);

enum SPECIAL_ACTIVITY_TYPE
{
	SPECIAL_ACTIVITY_TYPE_INVALID = 1024,

	SPECIAL_ACTIVITY_TYPE_OPENGAME,										// 开服活动
	SPECIAL_ACTIVITY_TYPE_CLOSEDBETA,									// 封测活动
	SPECIAL_ACTIVITY_TYPE_COMMON_ACTIVITY,								// 通用活动
	SPECIAL_ACTIVITY_TYPE_COMBINE_SERVER,								// 合服活动

	SPECIAL_ACTIVITY_TYPE_RESVERED,
};
UNSTD_STATIC_CHECK(SPECIAL_ACTIVITY_TYPE_RESVERED < CLIENT_ACTIVITY_TYPE_BEGIN);

static const int SPECIAL_ACTIVITY_TYPE_MAX = SPECIAL_ACTIVITY_TYPE_RESVERED - SPECIAL_ACTIVITY_TYPE_INVALID;

// 随机活动
enum RAND_ACTIVITY_TYPE
{
	RAND_ACTIVITY_TYPE_BEGIN = 2048,

	RAND_ACTIVITY_TYPE_TIME_LIMIT_STORE = 2049,			//限时商店
	RAND_ACTIVITY_TYPE_TRUN_TABLE = 2051,				//开服活动-转盘活动
	RAND_ACTIVITY_TYPE_DISCOUNT_STORE = 2052,			//开服活动-折扣商店
	RAND_ACTIVITY_TYPE_SKY_DROP_LUXURY_GIFT = 2054,		//天降豪礼
	RAND_ACTIVITY_TYPE_CONSUME_CAROUSEL = 2055,			//消费转盘
	RAND_ACTIVITY_TYPE_EXPEL_MONSTER = 2056,			//驱除妖兽
	RAND_ACTIVITY_TYPE_HERITAGE_TREASURE = 2057,		//传承瑰宝
	RAND_ACTIVITY_TYPE_SEVEN_COLOR_PICTURE = 2058,		//七彩图鉴
	RAND_ACTIVITY_TYPE_SALE_SHOP = 2059,				//特卖商店
	RAND_ACTIVITY_TYPE_HAPPY_UP = 2060,					//欣欣向荣/印记兑换
	RAND_ACTIVITY_TYPE_ANIMAL_RACE = 2061,				//神兽径赛
	RAND_ACTIVITY_TYPE_LUCKY_BAG = 2062,				// 福袋
	RAND_ACTIVITY_TYPE_REDUCE_TAX_RATE = 2063,			//税率优惠
	RAND_ACTIVITY_TYPE_TIAN_JIANG_HAO_LI = 2064,		//天降好礼
	RAND_ACTIVITY_TYPE_SEAL_PROMOTE = 2065,				//封印提升
	RAND_ACTIVITY_TYPE_DROP_DOUBLE = 2066,				//掉落翻倍
	RAND_ACTIVITY_TYPE_JOYSEEKSFAVORS = 2067,			//欢乐寻宠
	RAND_ACTIVITY_TYPE_COLLECT_WORD = 2068,				// 集字活动
	RAND_ACTIVITY_TYPE_DISCOUNT_CARNIVAL = 2069,		//折扣狂欢
	RAND_ACTIVITY_TYPE_PARTNER_EXCHANGE = 2070,			//伙伴兑换
	RAND_ACTIVITY_TYPE_LUCKY_DISCOUNT = 2071,			//幸运折扣
	RAND_ACTIVITY_TYPE_PET_EXCHANGE = 2072,				//宠物兑换
	RAND_ACTIVITY_TYPE_JIAN_DAN_QIN_XIN = 2073,			//剑胆琴心（幻境奇遇）
	RAND_ACTIVITY_TYPE_ADVENTURE_GIFT = 2074,			//奇遇礼包
	RAND_ACTIVITY_TYPE_LINKAGE_GIFT = 2075,				//联动送礼
	RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE = 2076,			//幸运转盘
	RAND_ACTIVITY_TYPE_TIAN_MING_DIVINATION = 2077,		//天命卜卦
	RAND_ACTIVITY_TYPE_QUAN_MIN_DISCOUNT = 2078,		//全民团购
	RAND_ACTIVITY_TYPE_SPORTS = 2079,					//竞技场排名奖励
	RAND_ACTIVITY_TYPE_TU_JIAN_TREASURE = 2080,			//图鉴夺宝
	RAND_ACTIVITY_TYPE_GOD_TOKEN = 2081,				//封神战令
	RAND_ACTIVITY_TYPE_ELEMENT_CARDS = 2082,			//元素翻牌
	RAND_ACTIVITY_TYPE_SIGN_IN_CONTINUITY = 2083,		//连登好礼
	RAND_ACTIVITY_TYPE_CAI_SHEN_JU_BAO = 2084,			// 财神聚宝
	RAND_ACTIVITY_TYPE_DING_ZHI_BAO_XIANG = 2085,		// 定制宝箱
	RAND_ACTIVITY_TYPE_ZHUAN_SHU_LI_BAO = 2086,			// 专属礼包
	RAND_ACTIVITY_TYPE_JING_YAN_BIAO_SHENG = 2087,		// 经验飙升
	RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE = 2088,			// 限时宝盒
	RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_DAI = 2089,			// 限时宝袋
	RAND_ACTIVITY_TYPE_GIVE_PET_DRAW = 2090,			// 赠30连抽（送寻宠连抽）
	RAND_ACTIVITY_TYPE_CI_JIU_YING_XIN = 2091,			// 辞旧迎新
	RAND_ACTIVITY_TYPE_NIU_XING_DA_YUN = 2092,			// 牛行大运
	RAND_ACTIVITY_TYPE_SPRING_FESTIVAL_STORE = 2093,	// 年货节
	RAND_ACTIVITY_TYPE_MONPOLY_GIFT = 2094,				// 牛运当头
	RAND_ACTIVITY_TYPE_COW_EXCHANGE = 2095,				// 牛运兑换
	RAND_ACTIVITY_TYPE_COW_STORE = 2096,				// 牛运商城
	RAND_ACTIVITY_TYPE_COW_LUCKY_EXCHANGE = 2097,		// 牛运无边
	RAND_ACTIVITY_TYPE_LITTLE_YEAR_GIFT = 2098,			// 小年送礼
	RAND_ACTIVITY_TYPE_GIVE_SECRET_KEY = 2099,			// 赠寻宝次数
	RAND_ACTIVITY_TYPE_CUMULATIVE_RECHARGE = 2100,		// 累计充值
	RAND_ACTIVITY_TYPE_PET_LOOT = 2101,					// 宠物夺宝
	RAND_ACTIVITY_TYPE_TRADEMARKET_GOLD_RETURN = 2102,	// 交易所返还
	RAND_ACTIVITY_TYPE_SHEN_SHOU_JIANG_LIN= 2103,		// 神兽降临
	RAND_ACTIVITY_TYPE_GOOD_GIFT_TURN = 2104,			// 好礼转动
	RAND_ACTIVITY_TYPE_SPECIAL_SECRET_TREASURE = 2105,	// 特惠秘宝
	RAND_ACTIVITY_TYPE_HELP_OTHERS = 2106,				// 义胆助人
	RAND_ACTIVITY_TYPE_FORTUNE_MAZE = 2107,				// 幸运迷宫
	RAND_ACTIVITY_TYPE_CHUN_RI_DENG_FENG = 2108,		// 春日登峰
	RAND_ACTIVITY_TYPE_ANECDOTE = 2109,					// 秘境奇闻
	RAND_ACTIVITY_TYPE_LAO_DONG_HAO_LI = 2110,			// 劳动好礼
	RAND_ACTIVITY_TYPE_ZU_HE_HE_LI = 2111,				// 组合贺礼
	RAND_ACTIVITY_TYPE_LING_PAI_TIAN_CI = 2112,			// 令牌天赐
	RAND_ACTIVITY_TYPE_FASHION_SHOP = 2113,				// 时装商店
	RAND_ACTIVITY_TYPE_ONE_YUAN_GOU = 2114,				// 一元购
	RAND_ACTIVITY_TYPE_LING_YU_JI_JIN = 2115,			// 灵玉基金
	RAND_ACTIVITY_TYPE_LING_PAI_TIAN_CI_2 = 2116,		// 心法无量(令牌天赐2)
	RAND_ACTIVITY_TYPE_LUCKY_SIGN = 2117,				// 好运花签
	RAND_ACTIVITY_TYPE_QIONG_LOU_YU_YAN = 2118,			// 琼楼玉宴
	RAND_ACTIVITY_TYPE_SUPER_OFF_SHOP = 2119,			// 超级折扣商城
	RAND_ACTIVITY_TYPE_YUAN_QI_TURNING = 2120,			// 元气翻牌
	RAND_ACTIVITY_TYPE_COOL_BREEZE = 2121,				// 凉风送爽
	RAND_ACTIVITY_TYPE_LOTUS_MI_BAO = 2122,				// 荷香觅宝
	RAND_ACTIVITY_TYPE_MAZE_SUMMER = 2123,				// 夏日迷宫
	RAND_ACTIVITY_TYPE_SUMMER_LI_LIAN = 2124,			// 初夏历练
	RAND_ACTIVITY_TYPE_COLORFUL_LANTERN = 2125,			// 多彩花灯
	RAND_ACTIVITY_TYPE_MI_BAO_FANG_DIAN = 2126,			// 觅宝坊店
	RAND_ACTIVITY_TYPE_SUMMER_STORE = 2127,				// 夏日商店
	RAND_ACTIVITY_TYPE_ZHEN_PIN_SHANG_PU = 2128,		// 珍品商铺
	RAND_ACTIVITY_TYPE_PAI_MAI_HANG = 2129,				// 拍卖行
	RAND_ACTIVITY_TYPE_LING_PAI_TIAN_CI_3 = 2130,		// 赋世魂器(令牌天赐3)
	RAND_ACTIVITY_TYPE_DUI_HUAN_LING_YU = 2131,			// 兑换灵玉
	RAND_ACTIVITY_TYPE_BACK_REWARD = 2132,				// 回归豪礼
	RAND_ACTIVITY_TYPE_SERVER_COMPETITION = 2133,		// 全服比拼 
	RAND_ACTIVITY_TYPE_HAPPY_LOOK = 2134,				// 连连乐翻天
	RAND_ACTIVITY_TYPE_SIGN_HAPPY = 2135,				// 登录送好礼
	RAND_ACTIVITY_TYPE_WAN_LING_GONG_YING = 2136,		// 万灵共盈
	RAND_ACTIVITY_TYPE_HELP_OTHERS_2 = 2137,			// 桃园一三事
	RAND_ACTIVITY_TYPE_ANECDOTE_2 = 2138,				// 秘境奇闻2 （万灵奇闻）
	RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG = 2139,		// 魂器天成
	RAND_ACTIVITY_TYPE_INTEGRAL_DRAW = 2140,			// 整点抽奖 （山海风月）
	RAND_ACTIVITY_TYPE_SHAN_HAI_MET = 2141,				// 山海初遇
	RAND_ACTIVITY_TYPE_SHANHAI_QUESTION = 2142,			// 山海问答
	RAND_ACTIVITY_TYPE_SHAN_HAI_JU_BAO = 2143,			// 山海聚宝
	RAND_ACTIVITY_TYPE_SHAN_HAI_MEET = 2144,			// 山海相逢
	RAND_ACTIVITY_TYPE_FATE_ENTANGLEMENT = 2145,		// 山海缘起
	RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO = 2146,			// 招财进宝
	RAND_ACTIVITY_TYPE_YUN_ZE_TERRITORY = 2147,			// 云泽画境
	RAND_ACTIVITY_TYPE_YUN_ZE_BLOOMING_RECORD = 2148,	// 云泽繁花录
	RAND_ACTIVITY_TYPE_YUN_ZE_SLOT_MACHINE = 2149,		// 纷享同花礼
	RAND_ACTIVITY_TYPE_ZAI_ZHAN_YUN_ZE = 2150,			// 再战云泽
	RAND_ACTIVITY_TYPE_MO_ZU_ZAI_NA_LI = 2151,			// 魔族在哪里
	RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG = 2152,			// 超值纷享
	RAND_ACTIVITY_TYPE_YUN_ZE_ER_SAN_SHI =2153,			// 云泽二三事
	RAND_ACTIVITY_TYPE_COLORFUL_DOUBLE = 2154,			// 纷享加倍
	RAND_ACTIVITY_TYPE_SHAN_HAI_ZHENG_FENG = 2155,		// 山海争锋
	RAND_ACTIVITY_TYPE_SHAN_HAI_MI_BAO = 2156,			// 山海觅宝
	RAND_ACTIVITY_TYPE_SHAN_HAI_BAO_XIANG = 2157,		// 山海宝箱
 	RAND_ACTIVITY_TYPE_SHAN_HAI_BAO_HE = 2158,			// 山海宝盒
 	RAND_ACTIVITY_TYPE_SHAN_HAI_BAO_DAI = 2159,			// 山海宝袋
	RAND_ACTIVITY_TYPE_SHANHAI_HUADENG = 2160,			// 山海花灯
	RAND_ACTIVITY_TYPE_DING_ZHI_BAO_XIANG_2 = 2161,		// 定制宝箱2 (复制2085) 云泽宝箱
	RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE_2 = 2162,		// 限时宝盒2 (复制2088) 云泽宝盒
	RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_DAI_2 = 2163,		// 限时宝袋2 (复制2089) 云泽宝袋
	RAND_ACTIVITY_TYPE_JIXING_GAOZHAO = 2164,			// 吉星高照
	RAND_ACTIVITY_TYPE_YUN_ZE_WISH = 2165,				// 云泽祈愿
	RAND_ACTIVITY_TYPE_YUN_ZE_HAO_LI = 2166,			// 云泽好礼
	RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET = 2167,			// 萌宠驾到
	RAND_ACTIVITY_TYPE_YUN_ZE_CATCH_ELVES = 2168,		// 精灵大抓捕
	RAND_ACTIVITY_TYPE_XIAN_SHI_LEI_CHONG = 2169,		// 限时累充
	RAND_ACTIVITY_TYPE_WEEKEND_GIFT = 2170,				// 周末礼包
	RAND_ACTIVITY_TYPE_CHAO_ZHI_GIFT = 2171,			// 超值献礼
	RAND_ACTIVITY_TYPE_JING_JI_YUE_KA = 2172,			// 竞技月卡
 	RAND_ACTIVITY_TYPE_TIGER_FISH_EVERY_YEAR = 2173,	// 年年有鱼
 	RAND_ACTIVITY_TYPE_TIGER_FIND_SPRING = 2174,		// 寅虎探春
 	RAND_ACTIVITY_TYPE_TIGER_TEASE_NEWYEAR = 2175,		// 寅虎利是(寅虎逗新春)
 	RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY = 2176,		// 寅虎压岁
	RAND_ACTIVITY_TYPE_YINHU_WENSHI = 2177,				// 寅虎问世
	RAND_ACTIVITY_TYPE_CINIU_YINGXINHU = 2178,			// 辞牛迎新虎
	RAND_ACTIVITY_TYPE_HU_XING_DA_YUN = 2179,			// 虎行大运
	RAND_ACTIVITY_TYPE_YIN_HU_BAI_SUI = 2180,			// 寅虎拜岁
	RAND_ACTIVITY_TYPE_TIGER_AUSPICIOUS_SNOW = 2181,	// 瑞雪丰年
	RAND_ACTIVITY_TYPE_RONG_LIAN_YOU_LI = 2182,			// 熔炼有礼
	RAND_ACTIVITY_TYPE_QING_YUAN_SHI_ZHUANG = 2183,		// 情缘时装
	RAND_ACTIVITY_TYPE_QING_YUAN_LEI_CHONG = 2184,		// 情缘累充
	RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT = 2185,			// 异兽临世
	RAND_ACTIVITY_TYPE_QIYUAN_KONGMINGDENG = 2186,		// 祈愿孔明灯
	RAND_ACTIVITY_TYPE_YUAN_QI_SPEED = 2187,			// 元气竞速
	RAND_ACTIVITY_TYPE_YUAN_QI_HELP = 2188,				// 元气助人	
	RAND_ACTIVITY_TYPE_XUNMENG_HUDIE = 2189,			// 寻梦蝴蝶
	RAND_ACTIVITY_TYPE_BO_ZHONG_YOU_LI = 2190,			// 播种有礼
	RAND_ACTIVITY_TYPE_YUNA_QI_FAN_PAI = 2191,			// 元气翻牌
	RAND_ACTIVITY_TYPE_BAO_ZI_LAI_LE = 2192,			// 包子来了
	RAND_ACTIVITY_TYPE_WANLING_ZHULI = 2193,			// 万灵筑礼
	RAND_ACTIVITY_TYPE_JIN_YI_TIAN_XIANG = 2194,		// 锦衣天香
	RAND_ACTIVITY_TYPE_MIAO_BI_SHENG_HUA = 2195,		// 妙笔生花
	RAND_ACTIVITY_TYPE_YUAN_QI_YAN_HUO = 2196,			// 元气烟火
	RAND_ACTIVITY_TYPE_YUAN_QI_JI_SHI_BU = 2197,		// 元气记事簿
	RAND_ACTIVITY_TYPE_CHENG_FENG_PO_LANG = 2198,		// 乘风破浪
	RAND_ACTIVITY_TYPE_ZONG_XIANG_MAN_CHENG = 2199,		// 粽香满城
	RAND_ACTIVITY_TYPE_SALTY_SWEET_BATTLE = 2200,		// 咸甜之争
	RAND_ACTIVITY_TYPE_DUAN_YANG_XIANG_NANG = 2201,		// 端阳香囊
	RAND_ACTIVITY_TYPE_DUAN_YANG_ZI_XUAN = 2202,		// 端阳自选
	RAND_ACTIVITY_TYPE_CANG_LONG_CI_FU = 2203,			// 苍龙赐福
	RAND_ACTIVITY_TYPE_LUCKY_BLIND_BOX = 2204,			// 幸运盲盒
	RAND_ACTIVITY_TYPE_LAN_TANG_CHU_MU = 2205,			// 兰汤初沐
	RAND_ACTIVITY_TYPE_300_CHOU = 2206,					// 三百抽皮肤
	RAND_ACTIVITY_TYPE_DUAN_YANG_FU_LI = 2207,			// 端阳福礼
	RAND_ACTIVITY_TYPE_JIN_LONG_JU_BAO = 2208,			// 金龙聚宝
	RAND_ACTIVITY_TYPE_CHONG_JI_FAN_LI = 2209,			// 宠技返利
	RAND_ACTIVITY_TYPE_SHEN_SHOU_EN_ZE = 2210,			// 神兽恩泽
	RAND_ACTIVITY_TYPE_XIN_FU_QIANG_HUA = 2211,			// 新服强化
	RAND_ACTIVITY_TYPE_QI_XI_TE_HUI = 2212,				// 七夕特惠
	RAND_ACTIVITY_TYPE_CHARISMATIC_TANABATA = 2213,		// 魅力七夕
	RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE_3 = 2214,		// 限时宝盒3 (四季花牌)
	RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE_4 = 2215,		// 限时宝盒4 (四季宝盒)
	RAND_ACTIVITY_TYPE_DING_ZHI_BAO_XIANG_3 = 2216,		// 定制宝箱3 (四季宝箱) 复制2085 
	RAND_ACTIVITY_TYPE_XIN_FU_LING_CHONG = 2217,		// 新服灵宠
	RAND_ACTIVITY_TYPE_SHEN_CI_HUA_FU = 2218,			// 神赐华福 
	RAND_ACTIVITY_TYPE_TIAN_YI_YAO_SHI = 2219,			// 天衣耀世
	RAND_ACTIVITY_TYPE_HUN_QI_FAN_LI = 2220,			// 魂器返利 
	RAND_ACTIVITY_TYPE_YUE_XI_DENG_MI = 2221,			// 月夕灯谜
	RAND_ACTIVITY_TYPE_YUE_XI_LEI_CHONG = 2222,			// 月夕累充 
	RAND_ACTIVITY_TYPE_DAN_BI_FAN_LI = 2223,			// 单笔返利
	RAND_ACTIVITY_TYPE_ZHUI_YUE_SHANG_DIAN = 2224,		// 追月商店
	RAND_ACTIVITY_TYPE_ZENG_100_CHOU = 2225,			// 赠100抽

	RAND_ACTIVITY_TYPE_END,
};
UNSTD_STATIC_CHECK(RAND_ACTIVITY_TYPE_END < CLIENT_ACTIVITY_TYPE_BEGIN);

static const int RAND_ACTIVITY_TYPE_MAX = RAND_ACTIVITY_TYPE_END - RAND_ACTIVITY_TYPE_BEGIN; 

// 随机活动开启类型
enum RAND_ACTIVITY_OPEN_TYPE
{
	RAND_ACTIVITY_OPEN_TYPE_BEGIN = 0,

	RAND_ACTIVITY_OPEN_TYPE_NORMAL = RAND_ACTIVITY_OPEN_TYPE_BEGIN,		// 普通随机活动

	RAND_ACTIVITY_OPEN_TYPE_END,
};

enum ACTIVITY_STATUS
{
	ACTIVITY_STATUS_CLOSE = 0,											// 活动关闭状态
	ACTIVITY_STATUS_STANDY,												// 活动准备状态
	ACTIVITY_STATUS_OPEN,												// 活动开始中 
	ACTIVITY_STATUS_MAX,
};

enum RAND_ACTIVITY_MAKE_UP_TYPE
{
	RAND_ACTIVITY_MAKE_UP_TYPE_COIN = 0,			//使用铜币补齐
	RAND_ACTIVITY_MAKE_UP_TYPE_GOLD = 1,			//使用元宝补齐

	RAND_ACTIVITY_MAKE_UP_TYPE_MAX
};

struct ActivityStatus
{
	short activity_type;
	short status;
	unsigned int activity_begin_time;
	unsigned int activity_end_time;
};

struct SyncActivityStatus
{
	SyncActivityStatus()
	{
		activity_status = 0;
		next_status_switch_time = 0;
	}

	int activity_status;
	unsigned int next_status_switch_time;
};

struct SpecialActivityCfg
{
	SpecialActivityCfg() : type(SPECIAL_ACTIVITY_TYPE_INVALID), is_open(false), begin_time(0), end_time(0) {}

	bool Invalid() { return SPECIAL_ACTIVITY_TYPE_INVALID == type; }

	int type;
	bool is_open;

	unsigned int begin_time;
	unsigned int end_time;
};

struct RAMoneyInfo
{
	RAMoneyInfo() : money_type(0), money_count(0) {}
	RAMoneyInfo(short _money_type, int _money_count) : money_type(_money_type), money_count(_money_count) {}

	short money_type;
	int money_count;
};

struct RandActivityCfg
{
	RandActivityCfg() : type(0), is_open(false), duration_day(0) {}

	bool Invalid() { return 0 == type; }

	int type;
	bool is_open;

	int duration_day;
};

//月卡周卡
static const int MAX_MONTH_CARD_SHOP_ITEM_NUM = 30;

//首充
enum FRIST_RECHARGE_TYPE
{
	FRIST_RECHARGE_TYPE_YIXIANER = 1,//医仙儿

	FRIST_RECHARGE_TYPE_MAX,
};
static const int MAX_FIRST_RECHARGE_DAY_NUM = 3;			//奖励天数,领取时默认第一天
static const int MAX_FIRST_RECHARGE_TYPE_NUM = BIT_COUNT_OF_BYTE;		//修改注意对齐
UNSTD_STATIC_CHECK(MAX_FIRST_RECHARGE_DAY_NUM <= BIT_COUNT_OF_BYTE);
UNSTD_STATIC_CHECK(MAX_FIRST_RECHARGE_TYPE_NUM >= FRIST_RECHARGE_TYPE_MAX);

static const int MAX_FIRST_RECHARGE_EXCLUSIVE_NUM = 32;		//首充专属索引最大值
enum FIRST_RECHARGE_CMD_TYPE
{
	FIRST_RECHARGE_CMD_TYPE_GROW = 0,			//首充专属-成长助力

	FIRST_RECHARGE_CMD_TYPE_MAX
};

enum FIRST_RECHARGE_EXCLUSIVE_TYPE
{
	FIRST_RECHARGE_EXCLUSIVE_TYPE_INVAILD = 0,
	FIRST_RECHARGE_EXCLUSIVE_TYPE_ZHAO_CAI_XIAN_ZI = 1,		// 招财仙子
	FIRST_RECHARGE_EXCLUSIVE_TYPE_CHENG_ZHANG_ZHU_LI = 2,	// 成长助力

	FIRST_RECHARGE_EXCLUSIVE_TYPE_MAX
};

inline int GetFirstRechargeCmdTypeByBuyType(int buy_type)
{
	int first_charge_cmd_type = -1;
	switch (buy_type)
	{
		case BUY_TIME_TYPE_FIRST_RECHARGE_EXCLUSIVE:
		{
			first_charge_cmd_type = FIRST_RECHARGE_CMD_TYPE_GROW;
		}
	default:
		break;
	}

	return first_charge_cmd_type;
}

//////////////////////////////////////////////// 活动 相关定义 ////////////////////////////////////////////////
struct ActivityRoleInform
{
	ActivityRoleInform() : role_uid(0), level(0), prof(0), avatar(0), headshot_id(0)
	{
		memset(role_name, 0, sizeof(GameName));
		memset(guild_name, 0, sizeof(GuildName));
	}

	int role_uid;			//!< 角色的UID
	UInt16 level;			//!< 角色等级
	UInt16 prof;			//!< 角色职业
	UInt16 avatar;			//!< 角色模型
	UInt16 headshot_id;		//!< 头像ID
	GameName role_name;		//!< 角色名称
	GuildName guild_name;	//!< 家族名称
};

//--------------------------------------迷宫竞速 ----------------------------------------------------
static const int MAZE_RACER_MIN_LAYER = 1;

enum MAZE_RACER_AMULET_EFFECT_TYPE		// 迷宫竞速 - 符咒 - 效果类型
{
	MAZE_AMULET_EFFECT_SET_BATTLE_SPEED = 0,		// 设置战斗速率
	MAZE_AMULET_EFFECT_SET_WAR_INTERVAL = 1,		// 设置遇怪间隔
	MAZE_AMULET_EFFECT_SET_MOVES_PERMIT = 2,		// 设置移动许可
	MAZE_AMULET_EFFECT_SET_ENCNT_PERMIT = 3,		// 设置遇怪许可

	MAZE_AMULET_EFFECT_TYPE_MAX
};

enum MAZE_RACER_AMULET_TARGET_TYPE
{
	MAZE_AMULET_TARGET_ENTIRE_TEAM = 0,			// 整个队伍
	MAZE_AMULET_TARGET_ANOTHER_TEAM = 1,		// 随机一个其它队伍

	MAZE_AMULET_TARGET_TYPE_MAX
};
struct MazeRacerTeamInfo			// 迷宫竞速 - 队伍信息
{
	MazeRacerTeamInfo(int _pass_id = 0, int _mem_count = 0,int _team_index=0) : pass_id(_pass_id), mem_count(_mem_count)
	{
		team_index = _team_index;
	}

	int team_index;
	int pass_id;		// 通关编号
	int mem_count;		// 通关时的队员数量
	ActivityRoleInform pass_member[MAX_TEAM_MEMBER_NUM];	// 通关时全队员的信息(忽略离线玩家)
};
//----------------------------------------------------------------------------------------------------------


static const int ACTIVITY_RANK_DEFAULT_NUM_MAX = 100;								// 随机活动默认的排行榜长度
static const int ACTIVITY_RANK_SHORTER_NUM_MAX = 50;								// 随机活动较短的排行榜长度
static const int ACTIVITY_RANK_LIST_COUNT_10 = 10;
static const int ACTIVITY_AWARD_NUM_MAX = 10;										// 随机活动道具列表长度

//世界boss
static const int ACTIVITY_WORLD_BOSS_MAX_RANK_ROLE_NUM = 20;

//世界boss2(巨龙boss)
static const int ACTIVITY_WORLD_BOSS_2_MAX_RANK_ROLE_NUM = 20;

//世界boss3(天神降临)
static const int ACTIVITY_WORLD_BOSS_3_MAX_RANK_ROLE_NUM = 20;

//迷宫竞速
static const int ACTIVITY_MAZE_RACER_AMULET_MAX_TYPE = 10;							// 迷宫竞速 - 最大符咒类型数量

//-----------------------------勇闯地宫-----------------------------
const static int ACTIVITY_BRAVE_GROUND_LIMIT_ITEM_MAX = 100;		//最大限制物品数量
static const int ACTIVITY_BRAVE_GROUND_MAX_RANK = 100;			//勇闯地宫排行榜最大个数

//---------------------------------随机活动----------------------------------------
const static int MAX_RATE_COUNT_NUM = 10000;
const static int MAX_BONUS_COUNT_NUM = 128;
//-----------------------------随机活动-奇遇礼包------------------------------------
enum ADVENTURE_GIFT_CONDITION_TYPE
{
	ADVENTURE_GIFT_CONDITION_TYPE_ROLE_LEVEL,						//角色等级
	ADVENTURE_GIFT_CONDITION_TYPE_COURAGE_CHALLENGE,			//勇气挑战层数
	ADVENTURE_GIFT_CONDITION_TYPE_COURSE_OF_GROWTH_PROG,		//成就阶段
};

//-----------------------------月度投资------------------------------------
static const int RA_MONTH_INVESTMENT_MAX_NUM = 30;	   
static const int RA_MONTH_PURCHASE_REWARD = 5;			// 购买月度投资奖励

//-----------------------------评分活动------------------------------------
static const int SCORE_REWARD_MAX_NUM = 32;
UNSTD_STATIC_CHECK(SCORE_REWARD_MAX_NUM == 32);//如果还需要拓展,存储那边需要转移

enum ACTVITY_BUY_LIMIT_TYPE
{
	ACTVITY_BUY_LIMIT_TYPE_BEGIN = 1,
	ACTVITY_BUY_LIMIT_TYPE_ALL = ACTVITY_BUY_LIMIT_TYPE_BEGIN,										//总限购
	ACTVITY_BUY_LIMIT_TYPE_DAY = 2,										//每日限购
	
	ACTVITY_BUY_LIMIT_TYPE_MAX,
};

//-----------------------------小队竞技------------------------------------
const static int  TUAN_DUI_JING_JI_MAX_MEMBER_NUM = 3;

struct role_info
{
	role_info()	{ this->Reset(); }

	void Reset()
	{	
		role_id = 0;
		memset(role_name, 0, sizeof(GameName));
		obj_id = 0;
		level = 0;
		behavior_state = 0;
		vip_level = 0; 
		avatar_type = 0;
		profession = 0;
		appearance.Reset();
		move_speed = 0;
		pos_x = 0;
		pos_y = 0;
		distance = 0;
		dir = 0;
		scene_id = 0;
		escort_id = 0;
		memset(guildname, 0, sizeof(GuildName));
		is_leader = 0;
		is_in_team = 0;
		title_id = 0;
	}

	bool Invalid()
	{
		return role_id == 0;
	}
	
	int				role_id;		//!< 玩家ID
	GameName		role_name;		//!< 玩家名

	ObjID			obj_id;			//!< 玩家对象ID
	short			level;			//!< 玩家等级

	char			behavior_state;	//!< 行为状态 \see RoleStatusManager::ROLE_STATUS
	char			vip_level;		//!< vip等级
	short			avatar_type;	//!< 形象类型
	int				profession;		//!< 职业
	Appearance		appearance;		//!< 外观

	int				move_speed;		//!< 速度
	short			pos_x;			//!< 在场景中的位置X
	short			pos_y;			//!< 在场景中的位置Y
	Scalar			distance;		//!< 方向距离 (即在当前方向上要走的距离）
	Dir				dir;			//!< 方向 [-PI,-PI]
	int				scene_id;		//!< 标识这是哪个场景的obj
	int				escort_id;		//!< 镖车id
	GuildName		guildname;		//!< 家族名

	char			is_leader;		//!< 是否队长[0:否 1:是]
	char			is_in_team;		//!< 是否在队内[0:否 1:是]
	UInt16			title_id;		//!< 称号id
};


//-----------------------------限时活动报名------------------------------------
const static int MAX_SIGN_UP_NUM = 32;

//---------------------与活动关联,给玩家活动道具--------------------------------
enum ACTIVITY_GIVE_ITEM_TYPE
{
	ACTIVITY_GIVE_ITEM_TYPE_PATROL_MINING = 0,		//巡逻和采集
	ACTIVITY_GIVE_ITEM_TYPE_FIELD_BOSS = 1,			//野外BOSS
	ACTIVITY_GIVE_ITEM_TYPE_CRYSTAL_BOSS = 2,		//水晶副本BOSS
};




//-----------------------------首席竞选------------------------------------
static const int ACTIVITY_CHIEF_ELECTION_BATTLE_TIME = 600;			//首席竞选每回合最多战斗时间
static const int ACTIVITY_CHIEF_ELECTION_FIGTHER_NUM = 16;			//首席竞选参加前几位参加对战人数
static const int ACTIVITY_CHIEF_ELECTION_JOIN_PROF_NUM = 8;			//首席竞选 职业分类个数
UNSTD_STATIC_CHECK(ACTIVITY_CHIEF_ELECTION_JOIN_PROF_NUM >= PROFESSION_TYPE_NUM - 1)

struct ChiefElectionWinnerInfo
{
	int prof;
	int role_id;
};

//-----------------------------首席巅峰战------------------------------------
static const int ACTIVITY_CHIEF_PEAK_BATTLE_BATTLE_TIME = 600;			//首席巅峰战每回合最多战斗时间
static const int ACTIVITY_CHIEF_PEAK_BATTLE_FIGTHER_NUM = 8;			//首席巅峰战参加前几位参加对战人数
static const int ACTIVITY_CHIEF_PEAK_BATTLE_DECLARATION_LENGTH = 128;			//首席巅峰战 宣言长度

struct ChiefPeakBattleInfo
{
	int match_round;
	int role_id;
};

// ----------------------------- 家族答题 ----------------------------- //
struct GuildAnswerAwardTotalCon
{
	GuildAnswerAwardTotalCon(int _coin = 0, int _exp = 0, int _contri = 0) 
		: coin(_coin), experience(_exp), guild_contribution(_contri) {}

	int coin;
	int experience;
	int guild_contribution;

	std::map<int, std::map<int, int> > item_list;

	GuildAnswerAwardTotalCon & operator += (GuildAnswerAwardTotalCon & cobj)
	{
		this->coin += cobj.coin;
		this->experience += cobj.experience;
		this->guild_contribution += cobj.guild_contribution;

		for (std::map<int, std::map<int, int> >::iterator item_it = cobj.item_list.begin(); item_it != cobj.item_list.end(); ++item_it)
		{
			for (std::map<int, int>::iterator bind_it = item_it->second.begin(); bind_it != item_it->second.end(); ++bind_it)
			{
				this->item_list[item_it->first][bind_it->first] += bind_it->second;
			}
		}

		return *this;
	}

	void AddItemGet(const ItemConfigData & _item)
	{
		this->item_list[_item.item_id][_item.is_bind] += _item.num;
	}
};


// ----------------------------- 以一敌百 ----------------------------- //
static const int ONLY_FIGHT_SCNEE_MAX_NUM = 20;			//以一敌百等级场最大值
static const int ONLY_FIGHT_RANK_TITLE_MAX_NUM = 3;		//排名前三才有称号奖励

#pragma pack(push, 4)

// ----------------------------- 活动时间结构----------------------------- //
struct StatusTimeCfg											//有活动存库，记录开始关闭活动时间信息
{
	StatusTimeCfg() : status(0), start_hour(0), start_min(0), start_sec(0), dur_s(0) {}

	bool Invalid() { return 0 == dur_s; }

	int status;															// 状态
	int start_hour;														// 开始 H
	int start_min;														// 开始 M
	int start_sec;														// 开始 S
	int dur_s;															// 持续多少S
};

struct StatusTimeCfgList												//有活动存库，记录开始关闭活动时间信息
{
	StatusTimeCfg statustime[ACTIVITY_STATUS_MAX];
};

//-----------------------------------活动数据------------------------------
bool IsRandActivity(int activity_type);
bool IsNormalActivity(int activity_type);
unsigned int GetActivityDataLength(int activity_type);

struct RandActivityData
{
	static const int DATA_LENGTH = 899;

	void Reset()
	{
		begin_time = 0;
		end_time = 0;
		activity_open_type = RAND_ACTIVITY_OPEN_TYPE_NORMAL;
		memset(data, 0, sizeof(data));
	}

	unsigned int begin_time;
	unsigned int end_time;
	char data[DATA_LENGTH];
	char activity_open_type;
};

struct NormalActivityData					//默认用这个保存普通活动数据
{
	static const int DATA_LENGTH = 400;

	void Reset()
	{
		memset(data, 0, sizeof(data));
	}

	char data[DATA_LENGTH];
};

struct HuanJieZhanChangActivityData					//幻界战场活动数据
{
	void Reset()
	{
		season_num = 0;
	}

	int season_num;
};

struct ActivityWorldTeamArenaParam
{
	void Reset()
	{
		season_num = 1;
		start_sign_up_timestamp = 0;
		end_sign_up_timestamp = 0;
		activity_open_next_zero_timestamp = 0;
	}

	int season_num;				//赛季
	unsigned int start_sign_up_timestamp;		//开始报名时间戳
	unsigned int end_sign_up_timestamp;			//停止报名时间戳
	unsigned int activity_open_next_zero_timestamp;	//活动开始当晚0点时间戳
};


union ActivityData
{
	void Reset(int act_type);

	RandActivityData rand_activity_data;
	NormalActivityData activity_data;
	HuanJieZhanChangActivityData huan_jie_activity_data;
	ActivityWorldTeamArenaParam world_team_arena;
};

typedef char ActivityDataHex[sizeof(ActivityData) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ActivityDataHex) < 2048);

struct RandActivityParam
{
	RandActivityParam() {}

	void Reset()
	{
		memset(data_list, 0, sizeof(data_list));
	}

	RandActivityData data_list[RAND_ACTIVITY_TYPE_MAX];
};

struct CrossRandActivityDataParam
{
	CrossRandActivityDataParam() : activity_type(0), open_time(0), close_time(0), reserved_char(0)
	{
		memset(data, 0, sizeof(data));
	}

	int activity_type;
	unsigned int open_time;
	unsigned int close_time;
	char data[RandActivityData::DATA_LENGTH];
	char reserved_char;
};

struct RADelaySetTimeData
{
	RADelaySetTimeData() 
	{
		this->Reset();
	}

	void Reset()
	{
		is_force = 0;
		reserve_sh = 0;
		rand_activty_type = 0;
		begin_time = 0;
		end_time = 0;
		open_type = 0;
		delay_to_time = 0;
		memset(reserve_ll, 0, sizeof(reserve_ll));
	}

	short is_force;
	short reserve_sh;
	int rand_activty_type;
	unsigned int begin_time;
	unsigned int end_time;
	int open_type;
	unsigned int delay_to_time; // 延时到何时设置时间
	long long reserve_ll[2];
};

struct RADelaySetTimeParam
{
	static const int MAX_DELAY_NUM = 20;

	RADelaySetTimeParam()
	{
		this->Reset();
	}

	void Reset()
	{
		memset(reserve_ll, 0, sizeof(reserve_ll));
		delay_num = 0;
		for (int i = 0; i < MAX_DELAY_NUM; ++i)
		{
			delay_list[i].Reset();
		}
	}

	long long reserve_ll[4];
	int delay_num;
	RADelaySetTimeData delay_list[MAX_DELAY_NUM];
};

typedef char RADelaySetTimeParamHex[sizeof(RADelaySetTimeParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RADelaySetTimeParamHex) < 2048);

//-----------------------活动角色数据---------------------------------
struct ActivityRankBraveGroundRoleData
{
	void Reset()
	{
		role_id = 0;
		reached_level = 0;
		rand_param = 0;
		play_times = 0;
	}

	int role_id;
	int reached_level;
	int rand_param;
	int play_times;
};

struct ActivityChiefElectionRoleData
{
	void Reset()
	{
		base_prof = 0;
		capability = 0;
		memset(role_name, 0, sizeof(role_name));
		avator_type = 0;
		level = 0;
		headshot_id = 0;
		reserve_sh = 0;
	}

	int base_prof;		//防止隐藏服usercache为NULL，游戏服没连上usercache 是空的
	int capability;		//防止隐藏服usercache为NULL，
	GameName role_name;	//防止隐藏服usercache为NULL，游戏服没连上usercache 是空的
	short avator_type;	//防止隐藏服usercache为NULL，游戏服没连上usercache 是空的
	short level;		//防止隐藏服usercache为NULL，游戏服没连上usercache 是空的
	short headshot_id;  //防止隐藏服usercache为NULL，游戏服没连上usercache 是空的
	short reserve_sh;   //防止隐藏服usercache为NULL，游戏服没连上usercache 是空的
};

struct ActivityChiefPeakBattleRoleData
{
	void Reset()
	{
		bet_role_id = 0;
		bet_type = 0;
	}

	int bet_role_id;
	int bet_type;
};

struct ActivityHuanJieZhanChangRoleData
{
	void Reset()
	{
		rank_score = 0;
		max_rank_score = 0;
		winning_streak_times = 0;
		total_times = 0;
		win_times = 0;
		draw_times = 0;
		last_join_time = 0;
		memset(role_name, 0, sizeof(role_name));
		prof = 0;
		avatar_type = 0;
		is_finish_ding_ji = 0;
		today_get_huan_jie_coin = 0;
		today_win_times = 0;
		today_win_reward_flag = 0;
	}

	int rank_score;			//角色积分
	int max_rank_score;		//当前赛季达到最高排位积分
	int winning_streak_times;  //当前连胜次数
	int total_times;
	int win_times;
	int draw_times;
	unsigned int last_join_time;	//最后参加时间，相同分数用来排序
	GameName role_name;
	short prof;
	short avatar_type;
	int is_finish_ding_ji;			//是否完成了定级赛
	int today_get_huan_jie_coin;	//今天获得幻界币数量
	int today_win_times;	//今日胜场
	int today_win_reward_flag;	//今日胜场奖励领取标志
};
const static int MAX_RELIC_LOOTING_REWARD_NUM = 4;
struct ActivityRelicLootingRoleData
{
	void Reset()
	{
		memset(param, 0, sizeof(param));
		memset(reward_status, 0, sizeof(reward_status));
		show_flag = 0;
	}
	enum RELIC_LOOTING_PARAM_TYPE
	{
		RELIC_LOOTING_PARAM_TYPE_0 = 0,	//胜利次数
		RELIC_LOOTING_PARAM_TYPE_1 = 1,	//当前连胜
		RELIC_LOOTING_PARAM_TYPE_2 = 2,	//最高连胜次数
		RELIC_LOOTING_PARAM_TYPE_3 = 3, //失败次数
		RELIC_LOOTING_PARAM_TYPE_MAX,
	};

	int param[RELIC_LOOTING_PARAM_TYPE_MAX];
	int reward_status[MAX_RELIC_LOOTING_REWARD_NUM];//是否已经领取[0-1]
	int show_flag;
};

union ActivityRoleData
{
	void Reset(int act_type)
	{
		switch (act_type)
		{
			case ACTIVITY_TYPE_RELIC_LOOTING:
				{
					relic_looting_role_data.Reset();
				}
			break;
			case ACTIVITY_TYPE_BRAVE_GROUND:
				{
					brave_ground_role_data.Reset();
				}
				break;
			case ACTIVITY_TYPE_CHIEF_ELECTION:
				{
					chief_election_role_data.Reset();
				}
				break;
			case ACTIVITY_TYPE_CHIEF_PEAK_BATTLE:
				{
					chief_peak_battle_role_data.Reset();
				}
				break;
			case ACTIVITY_TYPE_HUAN_JIE_ZHAN_CHANG:
				{
					huan_jie_zhan_chang_role_data.Reset();
				}
				break;
			default:
				{
					memset(this, 0, sizeof(*this));
				}
				break;
		}
	}

	ActivityRelicLootingRoleData relic_looting_role_data;
	ActivityRankBraveGroundRoleData brave_ground_role_data;
	ActivityChiefElectionRoleData	chief_election_role_data;
	ActivityChiefPeakBattleRoleData chief_peak_battle_role_data;
	ActivityHuanJieZhanChangRoleData  huan_jie_zhan_chang_role_data;
};

typedef char ActivityRoleDataHex[sizeof(ActivityRoleData) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ActivityRoleDataHex) < 512);

// ----------------------------- 幻界战场----------------------------- //
static const int ACTIVITY_HUAN_JIE_ZHAN_CHANG_MAX_RAND_NUM = 6;   //允许随机匹配数量
static const int ACTIVITY_HUAN_JIE_ZHAN_CHANG_RANK_LIST_MAX_NUM = 100;   //每个排行榜最大上榜人数

struct ActivityHuanJieZhanChangRankItemCommon   //排行榜单项
{
	ActivityHuanJieZhanChangRankItemCommon()
	{
		role_id = 0;
		memset(role_name, 0, sizeof(role_name));
		prof = 0;
		score = 0;
	}

	int role_id;
	GameName role_name;
	int prof;
	int score;
};

struct ActivityHuanJieZhanChangSaveData			//存库
{
	int season_num;
};

//神兽降临
const static int SHEN_SHOU_JIANG_LIN_BAO_DI_MAX_NUM = 20;		//要保底奖励个数
struct RandActivityShenShouJiangLinParam
{
	RandActivityShenShouJiangLinParam()
	{
		draw_times = 0;
		gold_num = 0;
		item_num = 0;
	}

	int draw_times;
	int gold_num;
	int item_num;
};

#pragma pack(pop)


static const int MAX_ACTIVIYT_TIME_CFG_NUM = 5;							// 每个活动最多的配置数
static const int MAX_ACTIVITY_OPEN_WEEKDAY = 7;							// 每个活动开启的最大星期天数
struct ActivityCfg
{
	ActivityCfg() : type(ACTIVITY_TYPE_INVALID), is_open(false), statustime_list_count(0)
	{
		memset(open_weekday_list, 0, sizeof(open_weekday_list));
	}

	bool Invalid() { return ACTIVITY_TYPE_INVALID == type; }

	int type;
	bool is_open;

	int open_weekday_list[MAX_ACTIVITY_OPEN_WEEKDAY];

	int statustime_list_count;
	StatusTimeCfgList statustime_list[MAX_ACTIVIYT_TIME_CFG_NUM];
};

// 天命卜卦

struct TianMingDivinationInfo
{
	TianMingDivinationInfo() :uid(0), rate_item_id(0), item_num(0)
	{
		memset(role_name, 0, sizeof(GameName));
	}

	TianMingDivinationInfo& operator=(const TianMingDivinationInfo &data)
	{
		uid = data.uid;
		F_STRNCPY(role_name, data.role_name, sizeof(GameName));
		rate_item_id = data.rate_item_id;
		item_num = data.item_num;
		return *this;
	}

	int uid;
	GameName role_name;
	int rate_item_id;
	int item_num;
};

//劳动好礼数据
struct RandActivityLaoDongHaoLiParam
{
	RandActivityLaoDongHaoLiParam()
	{
		refresh_box_time = 0;
		close_box_time = 0;
		next_refresh_timestamp = 0;
		notice_tamp = 0;
	}
	void Reset()
	{
		refresh_box_time = 0;
		close_box_time = 0;
		next_refresh_timestamp = 0;
		notice_tamp = 0;
	}
	time_t refresh_box_time;
	time_t close_box_time;
	time_t next_refresh_timestamp;
	time_t notice_tamp;
};
UNSTD_STATIC_CHECK(sizeof(RandActivityLaoDongHaoLiParam) <= sizeof(ActivityRoleDataHex));

#endif // __ACTIVITY_DEF_HPP__