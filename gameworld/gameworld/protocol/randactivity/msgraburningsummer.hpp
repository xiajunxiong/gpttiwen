#ifndef __MSG_RA_BURNING_SUMMER_HPP__
#define __MSG_RA_BURNING_SUMMER_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/raburningsummerparam.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	//初夏历练
	enum RA_SUMMER_LI_LIAN_REQ_TYPE		//由协议3000
	{
		RA_SUMMER_LI_LIAN_REQ_TYPE_CHALLENGE = 0,				// 挑战			param1:配置活动天数activity_time
		RA_SUMMER_LI_LIAN_REQ_TYPE_FETCH_ALL_PASS_REWARD = 1,	// 领取全通奖励 
	};

	class SCRASummerLiLianAllInfo				//!< 3075 随机活动-初夏历练信息下发
	{	
	public:
		SCRASummerLiLianAllInfo();
		MessageHeader header;

		short open_day;												//活动开启天数
		char is_fetch_all_pass_flag;								//是否领取首通奖励标识 0:未领取 1:已领取
		char reserve_ch;
		BitMap<MAX_RA_SUMMER_LI_LIAN_BOSS_NUM> m_first_flag;		//是否首杀标识 MAX_RA_SUMMER_LI_LIAN_BOSS_NUM - 31	[1,31]
	};

	//多彩花灯
	enum RA_COLORFUL_LANTERN_REQ_TYPE	//由协议3000
	{
		RA_COLORFUL_LANTERN_REQ_TYPE_START_GAME = 0,				// 开始游戏
		RA_COLORFUL_LANTERN_REQ_TYPE_CATCH_LANTERN = 1,				// 捕抓花灯		param1:type
		RA_COLORFUL_LANTERN_REQ_TYPE_FETCH_REWARD = 2,				// 领取任务奖励	param1:task_id
		RA_COLORFUL_LANTERN_REQ_TYPE_FORCE_CLOSE = 3,				// 强制关闭
		RA_COLORFUL_LANTERN_REQ_TYPE_FLOW_AWAY = 4,					// 花灯流走		param1:type

		RA_COLORFUL_LANTERN_REQ_TYPE_MAX
	};

	class SCRAColorfulLanternAllInfo		//!< 3076 随机活动-多彩花灯信息下发
	{
	public:
		SCRAColorfulLanternAllInfo();
		MessageHeader header;

		short last_game_score_count;								//上一场游戏获得的积分
		short day_single_score_count;								//今天单次积分最大值
		short activity_single_score_count;							//活动内单次积分最大值
		short day_join_times;										//今日参与次数
		short activity_join_times;									//活动内参与次数
		short day_score_count;										//今日获得的总分
		int activity_score_count;									//活动内获得的总积分
		BitMap<MAX_RA_COLORFUL_LANTERN_TASK_NUM> m_task_list_flag;	//任务列表标识 0:未完成 1:已完成
	};

	enum RA_COLORFUL_LANTERN_GAME_NOTICE_TYPE
	{
		RA_COLORFUL_LANTERN_GAME_NOTICE_TYPE_START_GAME = 0,			//游戏开始
		RA_COLORFUL_LANTERN_GAME_NOTICE_TYPE_IN_THE_GAME = 1,			//游戏进行中
		RA_COLORFUL_LANTERN_GAME_NOTICE_TYPE_GAME_OVER = 2,				//游戏结束下发结算

		RA_COLORFUL_LANTERN_GAME_NOTICE_TYPE_MAX
	};

	class SCRAColorfulLanternGameInfo	//!< 3077 随机活动-多彩花灯游戏信息下发
	{
	public:
		SCRAColorfulLanternGameInfo();
		MessageHeader header;

		short notice_type;							//RA_COLORFUL_LANTERN_GAME_NOTICE_TYPE_MAX
		short single_score_count;					//本次游戏内的积分
		unsigned int game_end_timestamp;			//本次游戏结束时间戳
	};

	class SCRAColorfulLanternLightInfo	//!< 3078 随机活动-多彩花灯花灯信息下发
	{
	public:
		SCRAColorfulLanternLightInfo();
		MessageHeader header;

		short light_list[MAX_RA_COLORFUL_LANTERN_LIGHT_TYPE_NUM];		//花灯数量  MAX_RA_COLORFUL_LANTERN_LIGHT_TYPE_NUM  -- 10
	};

	//夏日商店
	enum RA_SUMMER_STORE_REQ_TYPE
	{
		RA_SUMMER_STORE_REQ_TYPE_INFO = 0,									//请求信息下发
		RA_SUMMER_STORE_REQ_TYPE_BUY_ITEM = 1,								//请求购买道具 param1:seq
		RA_SUMMER_STORE_REQ_TYPE_BUY_CMD_ITEM_CONFIG_CHECK = 2,				//购买直购道具前请求这个进行配置检测 param:seq
		RA_SUMMER_STORE_REQ_TYPE_OPEN_ACTIVITY = 3,							//打开活动界面
	};

	class SCRASummerStoreInfo		//!< 3079 随机活动-夏日商店信息下发
	{
	public:
		SCRASummerStoreInfo();
		MessageHeader header;

		short is_need_recal_phase_flag;											//0:不重算 1:重算阶段
		short is_open_today_flag;												//今天是否打开过界面 0:没打开 1：打开过
		short today_buy_times[MAX_RA_SUMMER_STORE_PHASE_SHOP_ITEM_NUM];			//每日限购
		short activity_buy_times[MAX_RA_SUMMER_STORE_SHOP_ITEM_NUM];			//活动限购
	};

	class SCRASummerStoreSingleInfo		//!< 3082 随机活动-夏日商店单个信息下发
	{
	public:
		SCRASummerStoreSingleInfo();
		MessageHeader header;

		short seq;				
		short index;
		short today_buy_times;
		short activity_buy_times;
	};

	//好运花签
	enum RA_LUCKY_SIGN_REQ_TYPE
	{
		RA_LUCKY_SIGN_REQ_TYPE_INFO = 0,						//请求信息下发
		RA_LUCKY_SIGN_REQ_TYPE_SET_SHOW_FLAG = 1,				//请求设置当前活动界面 param1: 0.签文界面 1.解签界面
		RA_LUCKY_SIGN_REQ_TYPE_DAY_SIGN = 2,					//请求每日签到
	};

	class SCRALuckySignInfo			//!< 3080 随机活动-好运花签信息下发
	{
	public:
		SCRALuckySignInfo();
		MessageHeader header;

		char is_base_show_flag;			//0:签文界面 1:解签界面
		char is_fetch_reward_flag;		//0:未领取	1: 已领取
		short sign_id;
		int reward_type_seq;
	};

	enum RA_SHAN_HAI_ZHENG_FENG_REQ_TYPE
	{
		RA_SHAN_HAI_ZHENG_FENG_REQ_TYPE_CHALLENGE = 0,				// 挑战			param1:配置活动天数activity_time
		RA_SHAN_HAI_ZHENG_FENG_REQ_TYPE_FETCH_ALL_PASS_REWARD = 1,	// 领取全通奖励 
	};

	class SCRAShanHaiZhengFengAllInfo				//!< 3121 随机活动-山海争锋信息下发
	{
	public:
		SCRAShanHaiZhengFengAllInfo();
		MessageHeader header;

		short open_day;												//活动开启天数
		char is_fetch_all_pass_flag;								//是否领取首通奖励标识 0:未领取 1:已领取
		char reserve_ch;
		BitMap<MAX_RA_SUMMER_LI_LIAN_BOSS_NUM> m_first_flag;		//是否首杀标识 MAX_RA_SUMMER_LI_LIAN_BOSS_NUM - 31	[1,31]
	};
}


#pragma pack(pop)

#endif // __MSG_RA_BURNING_SUMMER_HPP__
