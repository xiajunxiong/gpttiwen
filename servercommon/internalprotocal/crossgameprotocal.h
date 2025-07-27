#ifndef __CROSS_GAME_PROTOCAL_H__
#define __CROSS_GAME_PROTOCAL_H__

#include "servercommon/serverdef.h"
#include "servercommon/crossdef.hpp"
#include "servercommon/teamdef.hpp"
#include "servercommon/activitydef.hpp"
#include "servercommon/maildef.hpp"
#include "servercommon/chatserverdef.h"
#include "servercommon/guilddef.hpp"
#include "servercommon/struct/global/worldstatusparam.hpp"
#include "servercommon/commondata.hpp"
#include "servercommon/rankdef.hpp"
#include "servercommon/msgserialize.hpp"
#include "servercommon/platformbattledef.hpp"
#include "servercommon/struct/global/couragechallengerankdef.hpp"
#include "servercommon/struct/system/publicnoticelist.h"
#include "servercommon/gatherdreamdef.hpp"
#include "servercommon/commonsavedef.h"
#include "servercommon/roleactivity/raquanmindiscountparam.hpp"
#include "servercommon/struct/global/serverfirstkilldef.h"
#include "servercommon/struct/global/marketmanagerparamhpp.h"
#include "gameworld/gameworld/battle/battle_video_manager.hpp"
#include "servercommon/frienddef.hpp"
#include "servercommon/hongbaodef.hpp"
#include "servercommon/bigdipperdef.h"
#include "servercommon/struct/friendparam.h"
#include "servercommon/struct/friendintimacyparam.h"
#include "servercommon/struct/cowreportgoodnewsparam.h"
#include "servercommon/struct/global/worldarenaparam.hpp"
#include "servercommon/struct/global/worldteamarenaparam.hpp"
#include "servercommon/cycleactivity/lanternriddieparam.hpp"
#include "gameworld/gameworld/protocol/msgguild.hpp"
#include "servercommon/struct/bigcross/universeteamarena/universeteamarenadef.hpp"
#include "servercommon/roleactivity/ragodbeastadventparam.hpp"
#include "gameworld/gameworld/protocol/randactivity/msgraservercompetition.hpp"
#include "servercommon/roleactivity/rahorcruxtianchengparam.hpp"
#include "servercommon/struct/global/kuafuyingxiongludef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace crossgameprotocal
{

	/*
	* 协议命名规则
	* GAME_CROSS 表示从游戏服发到跨服
	* CROSS_HIDDEN 表示从跨服发到隐藏服
	* HIDDEN_CROSS 表示从隐藏服发到跨服
	* CROSS_GAME 表示从跨服发到游戏服
	* GAME_HIDDEN 表示从游戏服经由跨服转发到隐藏服
	* HIDDEN_GAME 表示从隐藏服经由跨服转发到游戏服
	* GAME_TO_GAME 表示从游戏服经跨服发到另一个游戏服
	*/
	//ADDMARK 
	enum _MsgType
	{
		MT_INVALID,

		MT_CROSS_GAME_BEGIN = 29000,
		MT_GAME_CROSS_REGISTER_GAME_SERVER,				// 注册gameserver
		MT_CROSS_GAME_HELLO,							// cross到game的心跳包
		MT_CROSS_HIDDEN_REGISTER_GAME_SERVER,			// 隐藏服注册gameserver
		MT_HIDDEN_GAME_REGISTER_GAME_SERVER_RET,		// 隐藏服注册gameserver结果返回
		MT_GAME_CROSS_START_CROSS_REQ,					// 本服请求角色跨服
		MT_CROSS_HIDDEN_START_CROSS_REQ,				// 跨服把本服的请求转发到隐藏服
		MT_HIDDEN_CROSS_USER_LOGIN,						// 通知cross角色已登录到隐藏服
		MT_HIDDEN_CROSS_USER_LOGOUT,					// 通知cross角色退出隐藏服
		MT_HIDDEN_CROSS_START_CROSS_ACK,				// 隐藏服返回跨服请求
		MT_CROSS_GAME_START_CROSS_ACK,					// 跨服返回隐藏服，跨服请求返回
		MT_CROSS_GAME_SYNC_CROSS_USER,					// 跨服同步游戏服正在跨服的玩家的名单
		MT_GAME_CROSS_KICK_CROSS_USER,					// game请求cross踢掉跨服角色
		MT_CROSS_HIDDEN_KICK_USER,						// cross请求hidden踢人
		MT_HIDDEN_CROSS_KICK_USER_ACK,					// hidden踢人返回cross
		MT_CROSS_GAME_KICK_USER_ACK,					// cross踢人返回game
		MT_CROSS_GAME_USER_ENTERING_HIDDEN,				// 跨服告知原服，玩家正在登录隐藏服
		MT_HIDDEN_CROSS_USER_ENTERING_HIDDEN,			// 隐藏服告知跨服，玩家正在登录隐藏服
		MT_GAME_CROSS_USER_ALLOWED_ENTER_HIDDEN,		// 原服告知跨服，玩家被允许登录隐藏服
		MT_CROSS_HIDDEN_USER_ALLOWED_ENTER_HIDDEN,		// 跨服告知隐藏服，玩家被允许登录隐藏服
		MT_HIDDEN_CROSS_TEAMMATE_JOIN_TEAM,				// 隐藏服告知跨服，玩家可以开始跨服加入队伍
		MT_CROSS_GAME_TEAMMATE_JOIN_TEAM,				// 跨服告知游戏服，玩家可以开始跨服加入队伍
		MT_GAME_HIDDEN_MESSAGE,							// 本服发向隐藏服（经跨服中转）
		MT_HIDDEN_GAME_MESSAGE,							// 隐藏服发向本服（经跨服中转）
		MT_GAME_HIDDEN_USER_START_CROSS_MATCH,			// 玩家开始跨服匹配
		MT_GAME_HIDDEN_USER_STOP_CROSS_MATCH,			// 玩家停止跨服匹配
		MT_HIDDEN_GAME_SYNC_CROSS_TEAM_INFO,			// 隐藏服向原服同步跨服队伍信息
		MT_GAME_HIDDEN_TRADE_MARKET_USER_SHELVE_INFO,	// 原服向隐藏服请求玩家的交易所信息
		MT_GAME_HIDDEN_TRADE_MARKET_SEARCH_BY_ID,		// 原服向隐藏服请求交易所，用ID搜索
		MT_GAME_HIDDEN_TRADE_MARKET_SEARCH_BY_TYPE,		// 原服向隐藏服请求交易所，用type搜索
		MT_GAME_HIDDEN_TRADE_MARKET_SEARCH_BY_BIG_TYPE,	// 原服向隐藏服请求交易所，用bigtype搜索
		MT_GAME_HIDDEN_TRADE_MARKET_SEARCH_BY_TYPE_WITH_LEVEL, // 原服向隐藏服请求交易所，用type与level搜索
		MT_GAME_HIDDEN_TRADE_MARKET_SEARCH_MEDAL_BY_COLOR,	// 隐藏服向原服请求交易所，用color搜令牌
		MT_GAME_HIDDEN_TRADE_MARKET_SEARCH_MEDAL_BY_SP_TYPE_AND_COLOR, // 隐藏服向原服请求交易所，用特效类型和颜色搜令牌
		MT_GAME_HIDDEN_TRADE_MARKET_GET_TRANSACTION_RECORD,	// 原服向隐藏服请求交易所，获取交易数据
		MT_GAME_HIDDEN_TRADE_MARKET_GET_ROLE_TRADE_RECORD,	// 原服向隐藏服请求交易所，获取自己的交易记录
		MT_GAME_HIDDEN_TRADE_MARKET_GET_ROLE_FOCUS_LIST,	// 原服向隐藏服请求交易所，获取自己的关注列表
		MT_GAME_HIDDEN_TRADE_MARKET_GET_FOCUS_ITEM_INFO,	// 原服向隐藏服请求交易所，获取点击的那个物品的信息
		MT_GAME_HIDDEN_TRADE_MARKET_REMOVE_FOCUS_ITEM,		// 原服向隐藏服请求交易所，清除一个关注的物品
		MT_GAME_HIDDEN_TRADE_MARKET_ADD_FOCUS_ITEM,			// 原服向隐藏服请求交易所，添加一个关注的物品
		MT_GAME_HIDDEN_TRADE_MARKET_GET_MERCHANDISE_NUM,	// 原服向隐藏服请求交易所，获取商品数量信息
		MT_GAME_HIDDEN_TRADE_MARKET_GET_SELLING_LIST,		// 原服向隐藏服请求交易所，获取某个物品正在售卖的信息

		MT_GAME_HIDDEN_TRADE_MARKET_REQUEST_BUY,			// 原服向隐藏服请求交易所，请求购买
		MT_HIDDEN_GAME_TRADE_MARKET_REQUEST_BUY_RESP,		// 隐藏服向原服返回购买请求结果
		MT_GAME_HIDDEN_TRADE_MARKET_REQUEST_BUY_CONFIRM,	// 原服向隐藏服请求交易所，请求购买确认单
		MT_HIDDEN_GAME_TRADE_MARKET_REQUEST_BUY_CONFIRM_RESP, // 隐藏服向原服返回购买确认请求结果

		MT_HIDDEN_GAME_TRADE_MARKET_REQUEST_BUY_CONFIRM_ERROR_RESP, // 隐藏服向原服返回购买请求出错结果

		MT_GAME_HIDDEN_TRADE_MARKET_REQUEST_UPSHELVE,		// 原服向隐藏服请求上架
		MT_HIDDEN_GAME_TRADE_MARKET_REQUEST_UPSHELVE_RESP,	// 隐藏服向原服请求返回上架结果

		MT_GAME_HIDDEN_TRADE_MARKET_OFFSHELVE_REQ,			// 原服向隐藏服请求下架 
		MT_HIDDEN_GAME_TRADE_MARKET_OFFSHELVE_RESP,			// 隐藏服向原服请求返回下架结果

		MT_GAME_HIDDEN_TRADE_MARKET_REDO_UPSHELVE_REQ,		// 原服向隐藏服请求重新上架
		MT_HIDDEN_GAME_TRADE_MARKET_REDO_UPSHELVE_RESP,		// 隐藏服向原服请求返回重新上架结果

		MT_GAME_HIDDEN_TRADE_MARKET_CASH_OUT_REQ,			// 原服向隐藏服请求提现
		MT_HIDDEN_GAME_TRADE_MARKET_CASH_OUT_RESP,			// 隐藏服向原服返回提现结果

		MT_GAME_HIDDEN_TRADE_MARKET_SEARCH_PET_FOR_QUICK_BUY,// 原服向隐藏服请求搜索宠物用于快速购买
		MT_GAME_HIDDEN_TRADE_MARKET_SEARCH_ADVERTISE_REQ,	// 原服向隐藏服请求搜索别人打广告的商品

		MT_GAME_HIDDEN_TRADE_MARKET_ADVERTISE_REQ,			// 原服向隐藏服请求发广告
		MT_HIDDEN_GAME_TRADE_MARKET_ADVERTISE_RESP,			// 隐藏服向原服返回发广告需扣款
		MT_GAME_HIDDEN_TRADE_MARKET_ADVERTISE_CONFIRM_BOARDCAST, // 原服扣款后向隐藏服确认发送广播


		MT_GAME_HIDDEN_USER_APPLY_JOIN_CROSS_TEAM,			// 玩家请求加入跨服队伍
		MT_HIDDEN_GAME_SEND_MAIL_TRANSFER,					// 隐藏服向原服转发邮件
		MT_GAME_TO_GAME_ROLE_CHAT_TRANSFER,					// 玩家从游戏服将聊天信息发送到另一个游戏服

		//> -- [好友] ------------
		MT_GAME_HIDDEN_GET_FRIEND_INFO,						// 原服向隐藏服  获取玩家的好友信息
		MT_HIDDEN_GAME_FRIEND_LIST_TO_ROLE,					// 隐藏服向原服	 同步玩家的好友列表
		MT_GAME_HIDDEN_ADD_FRIEND_REQ,						// 原服向隐藏服  同步玩家申请添加好友操作
		MT_HIDDEN_GAME_FRIEND_ASK_LIST_CHANGE,				// 隐藏服向原服  同步玩家的好友申请列表变更
		MT_GAME_HIDDEN_ADD_FRIEND_REPLY_REQ,				// 原服向隐藏服  同步好友应答操作
		MT_GAME_HIDDEN_REMOVE_FRIEND_REQ,					// 原服向隐藏服  同步删除好友操作
		MT_GAME_HIDDEN_ADD_BLACK_REQ,						// 原服向隐藏服  同步添加黑名单操作
		MT_HIDDEN_GAME_BLACK_LIST,							// 隐藏服向原服  同步玩家的黑名单信息
		MT_HIDDEN_GAME_BLACK_LIST_CHANGE,					// 隐藏服向原服  同步玩家的黑名单信息变更
		MT_GAME_HIDDEN_REMOVE_BLACK_REQ,					// 原服向隐藏服  同步删除黑名单操作
		MT_GAME_HIDDEN_FRIEND_GROUP_REQ,					// 原服向隐藏服  同步好友组操作
		MT_HIDDEN_GAME_ASK_LIST_TO_ROLE,					// 隐藏服向原服  同步玩家的好友申请列表
		MT_GAME_HIDDEN_RECENTLY_REQ,						// 原服向隐藏服  同步玩家处理近聊相关操作
		MT_HIDDEN_GAME_RECENTLY_INFO,						// 隐藏服向原服  同步玩家的近聊相关信息
		MT_HIDDEN_GAME_FRIEND_CHANGE_TO_ROLE,				// 隐藏服向原服  同步好友变更信息
		MT_GAME_HIDDEN_ROLE_LOGIN_LOGOUT_FRIENDS_NOTIFY,	// 原服向隐藏服  同步玩家上||下线通知到隐藏服
		MT_SYNC_FRIEND_INFO,								// 同步好友信息  , 不知道从哪同步到哪..需要看具体情况
		MT_SYNC_ADD_FRINED_OFFLINE_EVENT,					// 同步好友添加时玩家处于离线状态 , 则上线时需要处理一下成就等相关的操作
		
		//> -- [好友 - 黑名单] ------------
		MT_SYNC_BLACK_INFO,								// 同步黑名单信息  , 不知道从哪同步到哪..需要看具体情况


		MT_HIDDEN_GAME_REQUEST_GUILD_LIST,				// 隐藏服向游戏服请求所有家族列表
		MT_GAME_HIDDEN_RESPONSE_GUILD_LIST,				// 游戏服向隐藏服返回所有家族列表
		MT_GAME_HIDDEN_SYNC_GUILD_MEMBER_CHANGE,		// 游戏服向隐藏服同步家族成员变更 
		MT_GAME_HIDDEN_SYNC_GUILD_CHANGE,				// 游戏服向隐藏服同步家族变更
		MT_HIDDEN_GAME_ADD_TITLE_TO_GUILD_MEMBER,		// 隐藏服向游戏服请求发放称号(家族大乱斗)
		MT_GAME_HIDDEN_REQ_SYNC_WORLD_STATUS,			// 游戏服向隐藏服请求世界状态
		MT_GAME_HIDDEN_SYNC_WORLD_STATUS,				// 游戏服向隐藏服世界状态
		MT_GAME_HIDDEN_ACTIVITY_FORCE_TO_NEXT_STATE,	// 游戏服向隐藏服活动gm命令
		MT_GAME_HIDDEN_REQ_SYNC_ACTIVITY_STATUS,		// 游戏服向隐藏服请求活动状态	
		MT_HIDDEN_GAME_SYNC_ACTIVITY_STATUS,			// 隐藏服向游戏服活动状态					
		MT_GAME_HIDDEN_USERCACHE_CHANGE_SYNC,			// 游戏服向跨服同步usercache信息
		MT_HIDDEN_GAME_USERCACHE_CHANGE_SYNC,			// 跨服向游戏服同步usercache信息
		MT_CROSS_HIDDEN_SERVER_DISCONNECT,				// 通知跨服场景服，主服断开连接
		MT_GAME_CROSS_RA_INFO_SYNC,						// 游戏服向跨服同步运营活动信息
		MT_CROSS_GAME_RA_INFO_RET,						// 跨服向游戏服返回运营活动信息
		MT_CROSS_HIDDEN_RA_INFO_SYNC,					// 跨服向隐藏服同步原服的运营活动信息
		MT_GAME_HIDDEN_FORCE_GET_GOLD,					// 游戏服向隐藏服转发充值cmd
		MT_GAME_HIDDEN_SYNC_CROSS_PERSON_RANK_INFO,		// 同步个人排行榜信息到跨服
		MT_GAME_HIDDEN_SYNC_CROSS_PET_RANK_INFO,		// 同步宠物排行榜信息到跨服
		MT_GAME_HIDDEN_ONLINE_STATUS_SYNC,				// 同步玩家在线状态
		MT_CROSS_GAME_USER_MSG,							// cross给用户发消息 Protocol
		MT_GAME_CROSS_USER_MSG,							// 用户给cross发消息 Protocol

		//------------------首席竞选-----------------------
		MT_HIDDEN_GAME_CHIEF_ELECTION_REQ,			// 玩家从游戏服发送请求到隐藏服
		//------------------首席巅峰战-----------------------
		MT_HIDDEN_GAME_CHIEF_PEAK_BATTLE_REQ,			// 玩家从游戏服发送请求到隐藏服
		//------------------工会荣誉战-----------------------
		MT_HIDDEN_GAME_GUILD_HONOR_BATTLE_REQ_ROUTE,
		MT_HIDDEN_GAME_GUILD_HONOR_BATTLE_MATCH_RECORD_SYNC_INFO,	//家族荣誉战隐藏服向游戏服同步比赛记录
		//------------------工会-----------------------
		MT_HIDDEN_GAME_CHANGE_GUILD_INFO,
		//---------------------------------------
		MT_HIDDEN_GAME_CHAT_ITEM_INFO_REQ,
		MT_GAME_HIDDEN_CHAT_ITEM_INFO_REQ,
		MT_GAME_HIDDEN_CHAT_ITEM_INFO_SYN,

		MT_HIDDEN_GAME_TEAM_MATCH_SUCC_JOIN_TEAM_REQ,
		// -----------------以一敌百(个人版家族大乱斗)--------------
		MT_HIDDEN_GAME_ADD_TITLE_TO_ROLE,					// 隐藏服向游戏服请求发放称号(以一敌百)

		MT_CROSS_GAME_HIDDEN_NOT_CONNECTED,					// 跨服返回游戏服通知玩家隐藏服尚未连接
		MT_GAME_CROSS_SEND_ALL_USER,						// 游戏服发到跨服，由跨服转发到所有游戏服玩家（包括隐藏服）
		MT_GAME_HIDDEN_QUERY_REQ,							// 游戏服向隐藏服发送玩家查询
		
		MT_HIDDEN_GAME_NOTICE_LOG,							// 隐藏服向游戏服打log
		MT_HIDDEN_GAME_MAZER_ACER_REQ,						// 迷宫竞速从游戏服发送请求到隐藏服
		MT_HIDDEN_GAME_COMMON_INFO_NOTICE_HIDDEN,				// 角色登录游戏服通知隐藏服
		MT_HIDDEN_GAME_FIRST_KILL_INFO_REQ,					// 隐藏服向游戏服请求首杀信息
		MT_GAME_HIDDEN_FIRST_KILL_INFO_REQ,					// 游戏服向隐藏服请求首杀信息

		MT_GAME_HIDDEN_CHAT_GROUP,							// 游戏服向隐藏服发送聊天组信息
		MT_HIDDEN_GAME_CHAT_GROUP,							// 隐藏服向游戏服发送聊天组信息
		MT_GAME_HIDDEN_CHAT_REQ,							// 游戏服向隐藏服发送聊天组操作
		MT_HIDDEN_GAME_CHAT_REQ,							// 隐藏服向游戏服发送聊天组操作

		MT_GAME_HIDDEN_TEMP_GROUP_BECOME_OFFICIAL_REQ,		// 游戏服向隐藏服发送临时聊天群组转正请求
		MT_HIDDEN_GAME_TEMP_GROUP_BECOME_OFFICIAL_RESP,		// 隐藏服向游戏服发送临时聊天群组转正返回
		
		MT_HIDDEN_GAME_VIDEO_SIMPLE_INFO_REQ,				// 跨服向原服请求简单的战报信息
		MT_GAME_HIDDEN_VIDEO_SIMPLE_INFO_REQ,				// 原服向隐藏服请求简单的战报信息
		MT_HIDDEN_GAME_VIDEO_DETAIL_INFO_REQ,				// 跨服向原服请求详细的战报信息
		MT_GAME_HIDDEN_VIDEO_DETAIL_INFO_RESP,				// 原服向跨服返回详细的战报信息
		MT_GAME_HIDDEN_VIDEO_DETAIL_INFO_REQ,				// 原服向隐藏服请求详细的战报信息
		MT_HIDDEN_GAME_VIDEO_DETAIL_INFO_RESP,				// 隐藏服向原服返回详细的战报信息

		MT_HIDDEN_GAME_HUAN_JIE_ZHAN_CHANG_REQ,				// 幻界战场游戏服发送请求到隐藏服
		MT_HIDDEN_GAME_HUAN_JIE_ZHAN_CHANG_REQ_2,			// 幻界战场游戏服发送请求到隐藏服2

		MT_HIDDEN_GAME_GUILD_EVENTHANDLER_SEND,				// 隐藏服向游戏服发送此时需要添加的家族日志信息
		
		MT_GAME_HIDDEN_SERVER_LOGIN_CHATGROUP_DATA_SYN,		// 游戏服登入向跨服同步群组信息
		MT_HIDDEN_GAME_SERVER_LOGIN_CHATGROUP_DATA_SYN,		// 跨服返回跨服群组信息
		MT_HIDDEN_GAME_CHATGROUP_INFO_SYN,					// 跨服向游戏服同步群组信息

		MT_HIDDEN_GAME_ONLY_FIGHT_SYN_INFO,					// 隐藏服向游戏服同步以一敌百信息
		MT_GAME_HIDDEN_ONLY_FIGHT_SYN_INFO_REQ,				// 游戏服请求隐藏同步以一敌百信息

		MT_GAME_HIDDEN_PLATFORM_BATTLE_REQ,					// 擂台战游戏服向隐藏服请求
		MT_HIDDEN_GAME_RET_PLATFORM_BATTLE_ROLE_INFO,		// 隐藏服返回游戏服擂台战角色信息
			
		MT_HIDDEN_GAME_COURAGE_CHALLENGE_RANK_REQ,			// 隐藏服向游戏服请求锢魔之塔排行信息
		MT_HIDDEN_GAME_COURAGE_CHALLENGE_ROLE_CHANGE,		// 隐藏服向游戏服通知角色有关锢魔之塔信息改变

		MT_HIDDEN_GAME_SYNC_LOG_QUICK,						// 隐藏服向游戏服同步LogQuick
		MT_HIDDEN_GAME_SYNC_LOG_TO_ORIGIN_SERVER,			// 隐藏服向游戏服同步日志

		MT_HIDDEN_GAME_GUILD_BOSS_CHAPTER_RANK_REQ,			// 隐藏服向游戏服请求家族试炼进度排行信息

		MT_GAME_HIDDEN_PUBLIC_NOTICE_SYNC,					// 游戏服向隐藏服同步公告
		MT_GAME_HIDDEN_ONLINE_INFO_SYNC,					// 游戏服向隐藏服同步在线情况

		MT_HIDDEN_GAME_INVITE_JOIN_TEAM,				// 隐藏服向游戏服发送邀请组队
		MT_GAME_HIDDEN_INVITE_JOIN_TEAM_RET,			// 游戏服向隐藏服发送邀请组队结果

		MT_GAME_HIDDEN_INVITE_JOIN_TEAM_IN_ORIGIN,			// 游戏服向隐藏服发送原服邀请组队
		MT_HIDDEN_GAME_INVITE_JOIN_TEAM_IN_ORIGIN_RET,		// 隐藏服向游戏服发送原服邀请组队结果

		MT_GAME_HIDDEN_SYSTEM_MAIL_SYNC,				// 游戏服向隐藏服发送系统邮件
		MT_HIDDEN_GAME_SYSTEM_MAIL_SYNC,				// 隐藏服向游戏服发送系统邮件

		MT_HIDDEN_GAME_GUILD_GATHER_DREAM_PUBLISH_LIST_REQ,	// 隐藏服向游戏服请求家族集愿列表
		MT_HIDDEN_GAME_GUILD_GATHER_DREAM_RECORD_LIST_REQ,	// 隐藏服向游戏服请求玩家集愿记录列表
		MT_HIDDEN_GAME_GUILD_GATHER_DREAM_PUBLISH_REQ,		// 隐藏服向游戏服请求发布集愿
		MT_GAME_HIDDEN_GUILD_GATHER_DREAM_PUBLISH_RET,		// 游戏服向隐藏服返回发布集愿
		MT_HIDDEN_GAME_GUILD_GATHER_DREAM_GIFT_REQ,			// 隐藏服向游戏服请求赠送集愿
		MT_GAME_HIDDEN_GUILD_GATHER_DREAM_GIFT_RET,			// 游戏服向隐藏服返回赠送集愿
		MT_HIDDEN_GAME_GUILD_GATHER_DREAM_GIFT_ADD_RECORD,	// 隐藏服向游戏服添加集愿记录
		MT_HIDDEN_GAME_GUILD_GATHER_DREAM_FETCH_REQ,		// 隐藏服向游戏服请求领取集愿
		MT_GAME_HIDDEN_GUILD_GATHER_DREAM_FETCH_RET,		// 游戏服向隐藏服返回领取情况
		MT_HIDDEN_GAME_GUILD_GATHER_DREAM_FETCH_CHANGE,		// 隐藏服向游戏服领取集愿变化
		MT_HIDDEN_GAME_BRAVE_GROUND_REQ,					// 勇闯地宫游戏服发送请求到隐藏服
		MT_HIDDEN_GAME_SYNC_HUAN_JIE_ACTIVITY_STATUS,		// 隐藏服向游戏服幻界战场活动状态		
		MT_HIDDEN_GAME_SYNC_HUAN_JIE_ROLE_INFO,				// 隐藏服向游戏服同步玩家信息
		MT_HIDDEN_GAME_GUILD_GATHER_DREAM_ROLE_INFO_CHANGE,	// 隐藏服向游戏服同步角色集愿信息

		MT_HIDDEN_GAME_HUAN_JIE_SYNC_TIAN_TI_RECORD,			// 隐藏服向游戏服同步玩家幻界对战记录
		MT_GAME_HIDDEN_SYNC_TIAN_TI_RECORD_ACK,				// 游戏服向游戏服同步幻界对战记录确认
			
		MT_HIDDEN_GAME_GUILD_CHAT,							// 隐藏服向游戏服发家族聊天
		MT_GAME_HIDDEN_GUILD_CHAT,							// 游戏服向隐藏服发家族聊天

		MT_GAME_HIDDEN_SENSITIVE_WORDS_RELOAD,				// 游戏服向隐藏服通知敏感字列表需要客户端重读
		MT_GAME_HIDDEN_CMD_KICK_ROLE,						// 游戏服向隐藏服发送，踢掉某玩家
		MT_GAME_HIDDEN_CMD_MUTE_ROLE,						// 游戏服向隐藏服发送，禁言某玩家
		MT_HIDDEN_GAME_CMD_MUTE_ROLE,						// 隐藏服向游戏服发送，禁言某玩家（隐藏服找不到玩家，可能刚好下线或返回了原服，所以在原服禁言）

		MT_GAME_HIDDEN_CMD_NOTICE_NEW_MESSAGE,				// 游戏服向隐藏服发送，通知有新的客服信息
		MT_HIDDEN_GAME_CMD_NOTICE_NEW_MESSAGE,				// 隐藏服向游戏服发送，通知有新的客服信息

		MT_GAME_HIDDEN_RA_TIANMING,				// 游戏服向隐藏服发送数据
		MT_HIDDEN_GAME_RA_TIANMING,				// 隐藏服向游戏服发送数据

		MT_GAME_HIDDEN_CONSUME_CAROUSEL_ADD_RECORD,			// 游戏服向隐藏服发送新的消费转盘记录
		MT_HIDDEN_GAME_CONSUME_CAROUSEL_ADD_RECORD,			// 隐藏服向游戏服发送新的消费转盘记录
		// [个人排行榜:新增] --------------------------  
		MT_HIDDEN_GAME_ROLE_INFO_CHANGE,					// 从跨服同步角色信息变更到主服 (用于玩家在跨服中做出影响排行数据变更的信息同步)

		// [宠物排行榜:新增] --------------------------  
		MT_HIDDEN_GAME_PET_INFO_CHANGE,					// 从跨服同步宠物排行榜信息变更到主服 (用于玩家在跨服中做出影响排行数据变更的信息同步)

		MT_GAME_HIDDEN_RESET_PUBLIC_NOTICE,					// 游戏服向隐藏服发送重置公告指令
		MT_HIDDEN_GAME_SYNC_PLAT_INCROSS_ROLE_NUM,			// 隐藏服向游戏服发送子渠道玩家在跨服的数量
		
		MT_HIDDEN_GAME_SYNC_GUILD_MEMBER_CHANGE,			// 隐藏服向游戏服同步家族成员变更 

		MT_HIDDEN_GAME_SYNC_WORLD_BOSS_INFO,		// 世界boss同步到本服
		MT_HIDDEN_GAME_SYNC_WORLD_BOSS2_INFO,		// 世界boss2同步到本服
		MT_HIDDEN_GAME_SYNC_WORLD_BOSS3_INFO,		// 世界boss3同步到本服


		MT_HIDDEN_GAME_WORLD_BOSS_SYNC_RANK_RESULT_MSG,	// 隐藏服向游戏服同步WorldBoss排名协议
		MT_HIDDEN_GAME_WORLD_BOSS_2_SYNC_RANK_RESULT_MSG,//隐藏服向游戏服同步WorldBoss2排名协议
		MT_HIDDEN_GAME_WORLD_BOSS_3_SYNC_RANK_RESULT_MSG,//隐藏服向游戏服同步WorldBoss3排名协议

		MT_HIDDEN_GAME_WORLD_BOSS_SYNC_PARTICIPATE_UID_LIST,	// 隐藏服向游戏服同步WorldBoss参与人员列表
		MT_HIDDEN_GAME_WORLD_BOSS_2_SYNC_PARTICIPATE_UID_LIST,	// 游戏服向隐藏服同步WorldBoss2参与人员列表
		MT_HIDDEN_GAME_WORLD_BOSS_3_SYNC_PARTICIPATE_UID_LIST,	// 游戏服向隐藏服同步WorldBoss3参与人员列表

		MT_HIDDEN_GAME_REQEST_QUANMIN_ACTIVITY_INFO,	// 隐藏服向游戏服请求全民活动数据
		MT_GAME_HIDDEN_RESPONSE_QUANMIN_ACTIVITY_INFO,	// 游戏服向隐藏服返回全民活动数据

		MT_GAME_HIDDEN_WILD_BOSS_INFO_REQ,						// 原服向隐藏服请求跨服野外BOSS信息
		MT_HIDDEN_GAME_SYNC_BRAVE_GROUND_INFO,					// 隐藏服同步到游戏服勇闯地宫一些信息
		MT_GAME_HIDDEN_FIRST_KILL_REQ,							// 游戏服向隐藏服首杀请求

		MT_HIDDEN_GAME_ROLE_RETURN_ORIGIN_SERVER_REQ,			// 隐藏服向原服请求玩家返回原服
		MT_GAME_HIDDEN_ROLE_RETURN_ORIGIN_SERVER_ACK,			// 原服向隐藏服回复玩家返回原服

		MT_GAME_HIDDEN_TEAMMATE_JOIN_ORIGIN_SERVER_TEAM,		// 原服向隐藏服发送，让队友回到原服加入队长的队伍
		MT_HIDDEN_GAME_SYNC_HUAN_JIE_INFO,					// 隐藏服同步到游戏服幻界战场一些信息
		MT_GAME_CROSS_QUERY_RANK_INFO_REQ,						// 游戏服或隐藏服向跨服请求查询玩家排行信息
		MT_GAME_HIDDEN_BRAVE_GROUND_GET_INFO,		// 原服向隐藏服获取勇闯地宫信息
		MT_HIDDEN_GAME_BRAVE_GROUND_INFO_RET,		// 隐藏服向原服返回勇闯地宫信息
		MT_GAME_HIDDEN_BRAVE_GROUND_SET_INFO,		// 原服向隐藏服设置勇闯地宫信息

		MT_GAME_HIDDEN_TEST_NETWORK,						// 测试发包性能

		MT_HIDDEN_GAME_SPECIAL_LOGIC_ACTIVITY_INFO_SYNC,		// 特殊活动找回信息同步
		MT_HIDDEN_GAME_GET_RANK_LIST_REQ,						//  隐藏服向原服请求排行榜

		MT_GAME_HIDDEN_TRADE_MARKET_HISTORY_TOP_PRICE_REQ,		// 游戏服向隐藏服请求交易所历史最高价商品信息列表

		MT_HIDDEN_GAME_SHEN_SHOU_JIANG_LIN_REQ,			//  隐藏服向原服神兽降临请求
		MT_GAME_HIDDEN_SHEN_SHOU_JIANG_LIN_DRAW_RET,			//  原服向隐藏服返回神兽降临的抽奖结果

		MT_HIDDEN_GAME_HEART_BEAT,								// 隐藏服对游戏服发送心跳
		MT_GAME_HIDDEN_ROLE_LOGIN_SEND_CROSS_CHAT_GROUP_SIMPLE_INFO,// 游戏服对隐藏服请求发送玩家的跨服群组信息
		MT_HIDDEN_GAME_COLOSSEUM_REQ_ROUTER,						// 隐藏服转发主角竞技场请求到游戏服
		MT_GAME_HIDDEN_DELETE_MUTE_PLAYER_CHAT_RECORD,					// 游戏服向隐藏服请求删除禁言角色的聊天记录

		MT_GAME_HIDDEN_TRADE_MARKET_SALE_ITEM_EVENTHANDLER,				// 交易行道具销售成功,玩家原服不在线,原服向隐藏服请求事件处理
		MT_HIDEEN_GAME_TRADE_MARKET_SALE_ITEM_EVENTHANDLER,				// 交易行道具销售成功,玩家原服不在线,隐藏服向原服请求事件处理

		MT_GAME_HIDDEN_TRADE_CREDIT_ADD_CREDIT_EVENTHANDLER,			// 交易信用增加,玩家原服不在线,原服向隐藏服请求事件处理
		MT_GAME_HIDDEN_CHAT_GROUP_LEADERSHIP_HANDOVER,					// 移交群组群主

		MT_HIDDEN_GAME_OTHER_SERVER_ROLE_ONLINE_STATUS_SYNC,			// 同步别服玩家在线状态

		MT_GAME_HIDDEN_SHAN_HAI_BOSS_INFO_REQ,							// 原服向隐藏服请求跨服山海BOSS信息
		MT_GAME_HIDDEN_ROLE_INFO_NOTICE_REQ_ROUTE,							
		MT_GAME_HIDDEN_RELIC_LOOTING_REQ,								// 游戏服向隐藏服发送遗迹夺宝请求
		MT_HIDDEN_GAME_RELIC_LOOTING_REQ,								// 隐藏服向游戏服发送遗迹夺宝返回

		MT_GAME_HIDDEN_SYNC_CLOUD_ARENA_USER_DATA_REQ,					// 游戏服向隐藏服请求同步玩家数据
		MT_HIDDEN_GAME_SYNC_CLOUD_ARENA_USER_DATA_RESP,					// 隐藏服向游戏服返回玩家数据
		
		MT_GAME_HIDDEN_CLOUD_ARENA_REAL_TIME_INFO_REQ,					// 游戏服向隐藏服请求玩家实时排名积分信息
		MT_GAME_HIDDEN_ROLE_LOGIN_CLOUD_ARENA_USER_INFO_REQ,			// 游戏服向隐藏服请求玩家信息

		MT_GAME_HIDDEN_MARKET_BUY_ITEM_RECORD_SYN_REQ,					// 游戏服向隐藏服同步新购买的交易记录
		MT_HIDDEN_GAME_MARKET_BUY_ITEM_RECORD_SYN_REQ,					// 隐藏服向游戏服同步新购买的交易记录
		MT_GAME_HIDDEN_MARKET_ALL_TRANSACTION_RECORD_SYN_REQ,			// 游戏服向隐藏服同步所有交易记录
		MT_HIDDEN_GAME_MARKET_ALL_TRANSACTION_RECORD_SYN_REQ,			// 隐藏服向游戏服同步所有交易记录

		MT_GAME_HIDDEN_VIDEO_CHANGE_REQ,								// 游戏服询问跨服是否需要替换录像
		MT_HIDDEN_GAME_VIDEO_CHANGE_RET,								// 跨服返回是否需要替换录像

		MT_GAME_CROSS_QUERY_CHIVALROUS_RANK_LIST_TOP100_REQ,			// 游戏服/隐藏服询问跨服top100侠义值排行榜
		MT_GAME_CROSS_QUERY_CHIVALROUS_RANK_LIST_ROLE_LIST_REQ,			// 游戏服/隐藏服询问跨服指定角色列表的排行榜数据
		MT_CROSS_GAME_CHIVALROUS_RANK_LIST_RESP,						// 跨服返回游戏服/隐藏服侠义值排行榜数据

		MT_GAME_HIDDEN_REQ_JOIN_TEAM_INFO,				// 游戏服向隐藏服请求队伍信息
		MT_HIDDEN_GAME_JOIN_TEAM_INFO_RET,				// 隐藏服向游戏服发送队伍信息

		MT_HIDDEN_GAME_SYNC_CLOUD_ARENA_SEASON_INFO,	// 隐藏服向游戏服发赛季信息
		
		//跨服帮派排行榜
		MT_GAME_CROSS_SYNC_CROSS_GUILD_RANK_INFO,				// 由主服向跨服同步跨服排行榜信息
		MT_CROSS_GAME_SYNC_GUILD_RANK_REWARD_RANK_INFO,			// 由跨服向主服同步公会排名信息[主要用于排行榜奖励]
		MT_HIDDEN_GAME_ROLE_ACTIVE_CHANGE,						// 从隐藏服同步角色活跃度信息变更回主服
		MT_GAME_HIDDEN_GUILD_ACTIVE_INFO_CHANGE,				// 从主服向隐藏服同步公会活跃度变更
		MT_HIDDEN_GAME_ROLE_ACTIVE_GIFT_INFO,					// 从隐藏服同步角色礼包领取信息回主服
		MT_GM_GAME_CROSS_GUILD_ACTIVE_TEST,						// 由主服到跨服结算一次奖励
		MT_GM_CROSS_GAME_GUILD_ACTIVE_TEST,						// 由跨服到主服清理排行榜信息

		MT_HIDDEN_GAME_PERSON_RANK_FRIEND_REQ,			//隐藏服向游戏服请求个人排行好友榜

		// 红包系统
		MT_GAME_HIDDEN_SYNC_HONG_BAO_INFO,				// 从主服向隐藏服同步红包信息
		MT_HIDDEN_GAME_SYNC_HONG_BAO_REQ_GIVE,			// 从隐藏服向主服同步发红包操作
		MT_HIDDEN_GAME_SYNC_HONG_BAO_REQ_GET,			// 从隐藏服向主服同步领红包操作
		MT_GAME_HIDDEN_SYNC_HONG_BAO_REQ_GIVE_RET,		// 从主服向隐藏服同步发红包操作结果
		MT_GAME_HIDDEN_SYNC_HONG_BAO_REQ_GET_RET,		// 从主服向隐藏服同步领红包操作结果
		MT_HIDDEN_GAME_SYNC_HONG_BAO_REQ_ERROR,			// 当从主服同步操作结果到隐藏服失败时[找不到玩家],再同步回主服做处理
		
		MT_GAME_HIDDEN_GIVE_GIFT_REQ,			// 从游戏服向隐藏服请求礼物操作
		MT_GAME_HIDDEN_GIVE_GIFT_ADD_GIFT,			// 从游戏服向隐藏服添加礼物
		MT_GAME_HIDDEN_GIVE_GIFT_INFO_RET,			// 游戏服向跨服领取礼物返回物品信息

		MT_HIDDEN_GAME_SHI_TU_SEEK_REQ,			// 游戏服向跨服领取礼物返回物品信息

		MT_HIDDEN_GAME_BIG_DIPPER_SYNC_RET,				// 跨服北斗七星数据同步回原服

		MT_GAME_HIDDEN_HUNDRED_GHOST_BOSS_INFO_REQ,		// 原服向隐藏服请求跨服百鬼夜行BOSS信息

		MT_HIDDEN_GAME_CONTINUE_CROSS_BATTLE,		// 隐藏服拉原服玩家到跨服继续战斗

		// 福牛报喜
		MT_HIDDEN_GAME_COW_REPORT_GOOD_NEWS_CHECK_CAN_BUY,		// 隐藏服 ->主服  检查能否购买福牛报喜 
		MT_GAME_HIDDEN_COW_REPORT_GOOD_NEWS_CHECK_CAN_BUY_ACK,	// 主服 ->隐藏服  检查能否购买福牛报喜应答 
		MT_HIDDEN_GAME_COW_REPORT_GOOD_NEWS_BUY_ITEM,			// 隐藏服 ->主服  购买福牛报喜 
		MT_GAME_HIDDEN_COW_REPORT_GOOD_NEWS_BUY_ITEM_ACK,		// 主服 ->隐藏服  购买福牛报喜 应答 
		MT_SYNC_COW_REPORT_GOOD_NEWS_BUY_ITEM_OFFLINE_EVENT ,	// 可能是主服 ->隐藏服  也可能是隐藏服 ->主服  看逻辑  购买福牛报喜购买失败的元宝返还事件的
		MT_HIDDEN_GAME_COW_REPORT_GOOD_NEWS_GET_INFO,			// 隐藏服 ->主服  获取福牛报喜的信息  


		MT_HIDDEN_GAME_SYNC_CROSS_DATA,				// 隐藏服与原服连通后 , 请求原服同步跨服数据到隐藏服

		MT_GAME_HIDDEN_CREAT_AUTO_ROBOT_INFO,		// 原服->隐藏服 自动机器人 创建通知
		MT_HIDDEN_GAME_TIAN_DI_DAO_HEN_SYNC_INFO,		// 隐藏服天地道痕同步信息到游戏服

		MT_GAME_HIDDEN_SYN_COLOSSENUM_RANK_INFO,		// 游戏服向隐藏服同步竞技场排名信息	

		//// [好友亲密度] ////////////////////////////////////////////////////////
		MT_SYNC_FRIEND_INTIMACY_INFO,					// 同步好友亲密度信息 ,可能 隐藏服->原服  也可能是 原服->隐藏服,具体需要看逻辑
		MT_GAME_HIDDEN_ADD_INTIMACY_REQ,				// 从主服向隐藏服   同步变更亲密度操作信息
		MT_GAME_HIDDEN_INTIMACY_RELATION_REQ,			// 从主服向隐藏服   同步好友亲密关系操作
		MT_GAME_HIDDEN_INTIMACY_SINGLE_CHAT_TIMES,		// 从主服向隐藏服   同步好友亲密度的私聊时间戳
		MT_GAME_HIDDEN_GIVE_FLOWER_NOTICE,				// 从主服向隐藏服   同步好友送花行为广播 , 用于广播特效和传闻
		MT_HIDDEN_GAME_GIVE_FLOWER_NOTICE,				// 从隐藏服向主服   同步好友送花行为广播 , 用于广播特效和传闻
		MT_GAME_HIDDEN_GIVE_FLOWER_BACK_KISS,			// 从主服向隐藏服   同步好友送花回吻
		MT_HIDDEN_GAME_GIVE_FLOWER_BACK_KISS,			// 从隐藏服向主服   同步好友送花回吻

		MT_HIDDEN_GAME_ACTIVITY_END_INFO,				// 隐藏服向游戏服同步活动结束信息
		MT_HIDDEN_GAME_SERVER_COURSE_INFO_REQ,			// 隐藏服向游戏服请求历程完成信息
		MT_HIDDEN_GAME_SERVER_COURSE_CHANGE_REQ,		// 隐藏服向游戏服请求历练信息改变
		MT_HIDDEN_GAME_ROLE_WIN_CHIEF_ELECTION_SYN,		// 隐藏服向游戏服同步角色夺得门派首席

		MT_GAME_HIDDEN_JIE_YI_ZU_CREATE,		// 游戏服向隐藏服结义组创建
		MT_GAME_HIDDEN_JIE_YI_ZU_REQ,			// 游戏服向隐藏服结义组操作
		MT_HIDDEN_GAME_JIE_YI_ZU_INFO_SYNC,		// 隐藏服向游戏服同步结义信息		
		MT_HIDDEN_GAME_JIE_YI_ZU_DISMISS,		// 隐藏服向游戏服解散		
		MT_GAME_HIDDEN_JIE_YI_ZU_NAME,				// 游戏服向隐藏服结义组命名	
		MT_GAME_HIDDEN_JIE_YI_ZU_MEMBER_NAME,		// 游戏服向隐藏服结义组成员命名
		MT_GAME_HIDDEN_JIE_YI_ZU_TEXT,		// 游戏服向隐藏服结义组编辑文字
		MT_GAME_HIDDEN_JIE_YI_ZU_LEAVE,		// 游戏服向隐藏服结义组离开
		MT_GAME_HIDDEN_JIE_YI_ZU_KICK,		// 游戏服向隐藏服结义组踢人
		MT_GAME_HIDDEN_JIE_YI_ZU_TAN_HE,		// 游戏服向隐藏服结义组弹劾
		MT_GAME_HIDDEN_JIE_YI_ZU_TAN_HE_VOTE,		// 游戏服向隐藏服结义组弹劾投票
		MT_GAME_HIDDEN_JIE_YI_ZU_INVITE_REPLY,
		MT_GAME_HIDDEN_JIE_YI_ZU_ROLE_XUAN_YAN,		// 游戏服向隐藏服自荐
		MT_GAME_HIDDEN_JIE_YI_ZU_XUAN_YAN,		// 游戏服向隐藏服结义组宣言
		MT_GAME_HIDDEN_JIE_YI_XUAN_YAN_LIST,		// 游戏服向隐藏服结义组宣言列表
		MT_HIDDEN_GAME_JIE_YI_SYNC_ROLE_DATA,		// 隐藏服向游戏服同步结义角色数据
		MT_HIDDEN_GAME_JIE_YI_ROLE_CHANGE,		// 隐藏服向游戏服同步结义角色变化

		MT_HIDDEN_CROSS_ENDLESS_RANK_REWARD,	// 无尽之塔跨服排行奖励
		MT_CROSS_HIDDEN_ENDLESS_RANK_REWARD_RET,// 无尽之塔跨服排行奖励

		MT_HIDDEN_GAME_SYNC_ROLE_BATTLE_DATA,	// 同步主角战斗数据到原服
		MT_GAME_HIDDEN_GIVE_FLOWER_CHANGE,		// 游戏服向隐藏服修改送花后的相关信息
		MT_HIDDEN_GAME_GIVE_FLOWER_CHANGE,		// 隐藏服向游戏服修改送花后的相关信息

		
		MT_HIDDEN_GAME_HORCRUX_TIAN_CHENG_REQ,			//隐藏服向原服魂器天成请求
		MT_GAME_HIDDEN_HORCRUX_TIAN_CHENG_DRAW_RET,		//原服向隐藏服返回魂器天成的抽奖结果

		MT_GAME_HIDDEN_QUERY_ITEM_RECOMMEND_PRICE,	// 原服向隐藏服请求交易行物品推荐价格

		MT_HIDDEN_GAME_QUERY_TARGET_GLADIATOR_BEFORE_FIGHT_REQ, // 玩家挑战竞技场，隐藏服向原服请求目标战斗数据
		MT_GAME_HIDDEN_QUERY_TARGET_GLADIATOR_BEFORE_FIGHT_RESP,// 原服向隐藏服返回目标的战斗数据，并开始战斗
		MT_HIDDEN_GAME_COLOSSEUM_SYS_REPORT,		// 跨服挑战竞技场，同步战斗结果回原服
		MT_GAME_HIDDEN_COLOSSEUM_SYNC_NEW_RANK_AFTER_SYS_REPORT,// 原服处理完战斗结果后把最新排名发回跨服处理

		MT_HIDDEN_GAME_JIXING_GAOZHAO_REQ,				//隐藏服向原服吉星高照请求
		MT_GAME_HIDDEN_JIXING_GAOZHAO_DRAW_RET,			//原服向隐藏服返回吉星高照的抽奖结果

		MT_GAME_CROSS_HOTFIX,							// 通知跨服读取热更新代码

		MT_GAME_HIDDEN_QUERY_IF_SYNC_GUILD_SUCCEED,		// 游戏服向隐藏服发起查询，看是否已同步家族数据成功
		MT_HIDDEN_GAME_SYNC_GUILD_SUCC_NOTICE,			// 隐藏服向游戏服通知家族数据已同步成功
			
		// 寅虎纳福
		MT_HIDDEN_GAME_TIGER_BRINGS_BLESSING_CHECK_CAN_BUY,		// 隐藏服 ->主服  检查能否购买寅虎纳福
		MT_GAME_HIDDEN_TIGER_BRINGS_BLESSING_CHECK_CAN_BUY_ACK,	// 主服 ->隐藏服  检查能否购买寅虎纳福应答 
		MT_HIDDEN_GAME_TIGER_BRINGS_BLESSING_BUY_ITEM,			// 隐藏服 ->主服  购买寅虎纳福
		MT_GAME_HIDDEN_TIGER_BRINGS_BLESSING_BUY_ITEM_ACK,		// 主服 ->隐藏服  购买寅虎纳福 应答 
		MT_SYNC_TIGER_BRINGS_BLESSING_BUY_ITEM_OFFLINE_EVENT,	// 可能是主服 ->隐藏服  也可能是隐藏服 ->主服  看逻辑  购买寅虎纳福购买失败的元宝返还事件的
		MT_HIDDEN_GAME_TIGER_BRINGS_BLESSING_GET_INFO,			// 隐藏服 ->主服  获取寅虎纳福的信息  

		MT_CROSS_GAME_DATA_SYNC_CHECK_ACK,						//  跨服 - > 主服 或 隐藏服 -> 主服 数据同步验证回执

		MT_GAME_HIDDEN_SYNC_SPECIAL_INFO_REQ,					// 原服向隐藏服请求同步一些特殊数据
		MT_HIDDEN_GAME_SYNC_SPECIAL_INFO_RET,					// 隐藏服向原服返回特殊数据

		MT_GAME_HIDDEN_QING_YUAN_CREATE,			// 游戏服向隐藏服情缘创建
		MT_HIDDEN_GAME_QING_YUAN_DISMISS,		// 隐藏服向游戏服情缘解散	
		MT_GAME_HIDDEN_QING_YUAN_LEAVE,
		MT_GAME_HIDDEN_QING_YUAN_SINGLE_LEAVE,
		MT_GAME_HIDDEN_QING_YUAN_FORCE_LEAVE,
		MT_GAME_HIDDEN_QING_YUAN_CANCEL_LEAVE,
		MT_HIDDEN_GAME_QING_YUAN_INFO_SYNC,		
		MT_GAME_HIDDEN_QING_YUAN_GIFT_GiVE,		// 游戏服向隐藏服激活礼物
		MT_GAME_HIDDEN_QING_YUAN_GIFT_FETCH_REWARD,		// 游戏服向隐藏服领取激活礼物奖励
		MT_HIDDEN_GAME_QING_YUAN_GIFT_FETCH_REWARD_RET,		// 隐藏服向游戏服领取激活礼物奖励返回
		MT_GAME_HIDDEN_QING_YUAN_GIFT_FETCH_DAY_REWARD,
		MT_HIDDEN_GAME_QING_YUAN_GIFT_FETCH_DAY_REWARD_RET,
		MT_GAME_HIDDEN_QING_YUAN_SET_SKILL_FLAG,

		MT_HIDDEN_CROSS_FALLEN_GOD_RANK_REWARD,		// 殒神之地跨服排行奖励
		MT_CROSS_HIDDEN_FALLEN_GOD_RANK_REWARD_RET,	// 殒神之地跨服排行奖励

		//情缘-婚宴
		MT_G_H_WEDDING_ROLE_REQ,		//部分角色操作转发
		MT_H_G_WEDDING_INFO_SYNC,		//婚宴信息通知

		MT_GAME_HIDDEN_QING_YUAN_COMMON_DATA,		// 游戏服向隐藏服 同步公共数据

		MT_GAME_HIDDEN_WORLD_ARENA_OPER_TRANSFER,	// 游戏服向隐藏服转发论剑请求
		MT_GAME_HIDDEN_WORLD_ARENA_SIGNUP,			// 游戏服向隐藏服报名论剑
		MT_HIDDEN_GAME_WORLD_ARENA_GIVE_WIN_BOX_REWARD,	// 隐藏服向游戏服下发胜场宝箱
		MT_HIDDEN_GAME_WORLD_ARENA_GIVE_SIGNUP_REWARD,	// 隐藏服向游戏服下发报名奖励
		MT_HIDDEN_GAME_WORLD_ARENA_GIVE_DAILY_REWARD,	// 隐藏服向游戏服下发每日奖励
		MT_GAME_HIDDEN_WORLD_ARENA_UPDATE_SIGNUP_INFO,  // 游戏服向隐藏服同步玩家报名信息	

		MT_GAME_CROSS_WORLD_TEAM_ARENA_QUERY_RANK_REQ,			// 游戏服向跨服查询排行榜
		MT_CROSS_GAME_WORLD_TEAM_ARENA_QUERY_RANK_RET,			// 跨服向游戏服返回查询结果
		MT_GAME_HIDDEN_ROLE_INFO_CHANGE_WORLD_TEAM_ARENA_SYN,	// 原服向隐藏服同步角色信息
		MT_HIDDEN_GAME_WORLD_TEAM_ARENA_SYN_ROLE_TITLE_INFO,	// 隐藏服向游戏服同步英雄会武角色称号信息
		MT_HIDDEN_GAME_WORLD_TEAM_ARENA_SYN_ACTIVITY_INFO,		// 隐藏服向游戏服同步英雄会武活动信息
		MT_GAME_HIDDEN_WORLD_TEAM_ARENA_ACITIVITY_INFO_SYN_REQ, // 游戏服向隐藏服请求同步英雄会武信息
		MT_GAME_HIDDEN_WORLD_TEAM_ARENA_FINALLY_RANK_REQ,		// 游戏服向隐藏服请求最终榜单信息

		MT_HIDDEN_GAME_ROLE_QI_FU_CHANGE,						// 从隐藏服祈福增加值返回原服
		MT_GAME_HIDDEN_ROLE_QI_FU_CHANGE,						// 原服祈福值同步到跨服

		MT_GAME_HIDDEN_QINGYUAN_DUIDUIPENG_UPDATA_RANK,			// 从游戏服向隐藏服 更新排行榜信息
		MT_GAME_HIDDEN_QINGYUAN_DUIDUIPENG_RANK_REQ,			// 从游戏服向隐藏服 请求排行榜信息
		MT_HIDDEN_GAME_QINGYUAN_DUIDUIPENG_RANK_INFO,			// 从隐藏服向游戏服 下发排行榜信息

		MT_GAME_HIDDEN_SYN_LEVEL_COMPLEMENT_INFO,				// 游戏服向隐藏服同步等级助力相关信息

		MT_HIDDEN_GAME_SYNC_HIDDEN_OPEN_SERVER_TIME,				// 隐藏服向游戏服同步同步隐藏服开服时间

		MT_HIDDEN_GAME_GUILD_SPEED_UP_INFO,						// 隐藏服向原服发起加速请求
		MT_GAME_HIDDEN_GUILD_SPEED_UP_INFO,						// 原服向隐藏服同步加速信息
		MT_GAME_HIDDEN_GUILD_SYN_BUILD_ALL_INFO,				// 向跨服同步所有家族建设信息
		MT_SYNC_GUILD_BUILDING_NOTIFY_INFO,						// 向原服或者跨服发一条同步漂字协议

		MT_GAME_HIDDEN_GET_UTA_CHAMPION_RECORD,			// 主服 -> 隐藏服  获取天下第一比武[组队] 冠军记录手册
		MT_GAME_HIDDEN_TRANSFER_MSG_TO_BIGCROSS,		// 原服 -> 隐藏服 -> 大跨服

		MT_HIDDEN_GAME_SYNC_ROLE_REQ_RET,				// 隐藏服向原服  同步玩家操作结果主要用于errornum

		MT_HIDDEN_GAME_GIVE_LOTTERY_TO_ROLE,			// 隐藏服向原服  给玩家发放竞猜币
		MT_HIDDEN_GAME_UTA_GIVE_ITEMS_TO_USER,			// 隐藏服向原服  给玩家发物品

		MT_HIDDEN_GAME_UTA_SYNC_BASE_INFO,		
		MT_HIDDEN_GAME_UTA_SYNC_SCHEDULES,

		MT_GAME_HIDDEN_USER_REQUEST_BIGCROSS_VIDEO,
		MT_HIDDEN_GAME_TRANSFER_BIGCROSS_MSG_TO_ORIGIN,	// 隐藏服转发大跨服的协议到原服

		MT_GAME_HIDDEN_USER_LOGIN_IN_ORIGIN,

		MT_GAME_HIDDEN_QUERY_WORLD_TEAM_ARENA_OTHER_INFO,	// 游戏服向隐藏服请求查询英雄会武其它信息
		MT_HIDDEN_GAME_UTA_GIVE_SERVER_ITEMS,

		MT_GAME_HIDDEN_QING_YUAN_LEICHONG_ADD_CHONGZHI,	// 游戏服向隐藏服 增加 情缘累充充值金额

		// 异兽临世
		MT_GAME_HIDDEN_GOD_BEAST_ADVENT_BUY_SERVER_LIMIT_ITEM_REQ,	// 原服->隐藏服 请求购买跨服限购道具
		MT_HIDDEN_GAME_GOD_BEAST_ADVENT_BUY_SERVER_LIMIT_ITEM_INFO,	// 隐藏服->原服 返回购买请求信息
		MT_HIDDEN_GAME_GOD_BEAST_ADVENT_BUY_NUM_INFO,				// 隐藏服->原服 同步限购道具数量,以及活动开启数据
		MT_GAME_HIDDEN_GOD_BEAST_ADVENT_RECEIVE_TRADE_MARKET_REQ,	// 原服->隐藏服 请求回收跨服交易行的过期道具

		// UTA
		MT_HIDDEN_CROSS_UTA_QUERY_TEAM_MEMBER_RANK_REQ,				// 隐藏服向跨服请求UTA队伍成员的排名
		MT_CROSS_HIDDEN_UTA_QUERY_TEAM_MEMBER_RANK_RESP,			// 跨服返回隐藏服UTA队伍成员排名信息

		MT_GAME_HIDDEN_HONGMENG_TIANJIE_REQ,						// 原服向隐藏服 鸿蒙天界请求 

		MT_HIDDEN_GAME_SERVER_COMPETITION_RANK_REQ,					// 跨服向原服请求新服比拼排行榜信息
		MT_GAME_HIDDEN_SERVER_COMPETITION_RANK_RESP,				// 原服向跨服返回新服比拼排行榜信息

		MT_GAME_HIDDEN_SALTY_SWEET_BATTLE_CHANGE_REQ,				// 原服向跨服发送咸甜之争变更
		MT_HIDDEN_GAME_SALTY_SWEET_BATTLE_VOTE_REQ,					// 跨服向原服请求咸甜之争投票

		MT_HIDDEN_GAME_TEAM_ROLE_RECRUIT_SYNC,						// 跨服向原服同步组队招募弹窗信息

		MT_GAME_HIDDEN_SYNC_PET_TOP_RANK_LIST,						// 原服向隐藏服同步宠物前10名列表
		MT_GAME_HIDDEN_SYNC_PARTNER_TOP_RANK_LIST,					// 原服向隐藏服同步伙伴前10名列表

		MT_HIDDEN_CROSS_MEILI_RANK_REWARD,							// 魅力榜跨服排行奖励请求
		MT_CROSS_HIDDEN_MEILI_RANK_REWARD_RET,						// 魅力榜跨服排行奖励返回

		MT_HIDDEN_GAME_FRIEND_INITIMACY_ADD,						// 魅力值发往原服寻找主人
		MT_GAME_HIDDEN_FRIEND_INITIMACY_ADD,						// 魅力值发往跨服寻找主人

		MT_GAME_HIDDEN_SEND_MAIL_TRANSFER_TO_ANOTHER_SERVER,		// 转发邮件到另一个原服

		MT_HIDDEN_GAME_QUERY_ORIGIN_SERVER_RA_STATUS,				// 隐藏服向原服请求原服活动当前的状态
		MT_GAME_HIDDEN_SEND_ORIGIN_SERVER_RA_STATUS,				// 原服向隐藏服发送原服活动的状态，只下发给隐藏服中属于该原服的玩家

		MT_GAME_HIDDEN_CLEAN_QUERY_INFO,							// 向隐藏服清空玩家缓存
		MT_HIDDEN_GAME_CLEAN_QUERY_INFO,							// 向原服清空玩家缓存

		MT_GAME_HIDDEN_CMD_TO_ROLE,									// 游戏服向隐藏服发送cmd命令
		MT_HIDDEN_GAME_CMD_TO_ROLE,									// 隐藏服向游戏服发送cmd命令（隐藏服找不到玩家，可能刚好下线或返回了原服，所以返回原服操作）

		MT_GAME_HIDDEN_QUERY_OFFLINE_BATTLE_DATA_FOR_ROBOT,		
		MT_HIDDEN_GAME_QUERY_OFFLINE_BATTLE_DATA_FOR_ROBOT,	
		MT_GAME_HIDDEN_NOTIFY_OFFLINE_BATTLE_DATA_FOR_ROBOT_RESULT,	
		MT_GAME_HIDDEN_REAL_ROLE_ROBOT_LEAVE_TEAM_NOTICE,
		MT_GAME_HIDDEN_SYNC_OFFLINE_BATTLE_DATA,
		MT_HIDDEN_GAME_QUERY_BATTLE_DATA_FOR_OFFLINE_BATTLE_DATA_SYSTEM, 

		MT_GAME_HIDDEN_REQ_SYN_KUA_FU_YING_XIONG_LU_DATA,			// 原服向跨服请求同步跨服英雄录数据
		MT_HIDDEN_GAME_RET_SYN_KUA_FU_YING_XIONG_LU_DATA,			// 跨服向原服同步跨服英雄录数据

		MT_HIDDEN_CROSS_REQ_KUA_FU_YING_XIONG_LU_RANK_END,			// 隐藏服跨服英雄录榜单结算,向跨服请求榜单信息
		MT_CROSS_HIDDEN_RET_KUA_FU_YING_XIONG_LU_RANK_END,			// 跨服返回跨服英雄录榜单结束请求

		MT_HIDDEN_GAME_ADD_SINGLE_CHAT_RECORD,
		MT_HIDDEN_GAME_USER_QUERY_CHAT_RECORD,

		MT_GAME_HIDDEN_HUAN_SHOU_ANN_INFO_REQ,						// 原服向隐藏服请求幻兽公告信息
		MT_GAME_HIDDEN_HUAN_SHOU_ANN_INFO_SYNC,						// 原服向隐藏服同步幻兽公告信息
		MT_HIDDEN_GAME_HUAN_SHOU_ANN_INFO_SYNC,						// 隐藏服向原服同步幻兽公告信息

		MT_GC_PROTOCOL_MAX,
		MT_GC_PROTOCAL_BEGIN = 29500,	
	};
	UNSTD_STATIC_CHECK(MT_GC_PROTOCOL_MAX <= MT_GC_PROTOCAL_BEGIN);

	//ADDMARK 
	bool WriteBegin(int msg_type, char* buffer, int max_len, int* out_len);
	bool ReadBegin(int& msg_type, const char* buffer, int max_len, int* out_len);

	struct MessageHeader
	{
		MessageHeader() : msg_type(MT_INVALID) {}
		MessageHeader(int _msg_type) : msg_type(_msg_type) {}
		bool Serialize(TLVSerializer2 * outstream) const { return outstream->Push(msg_type); }
		bool Unserialize(TLVUnserializer2 & instream) { return  instream.Pop(&msg_type); }
			
		int	msg_type;
	};

	struct GameCrossRegisterGameServer
	{
		GameCrossRegisterGameServer() : header(MT_GAME_CROSS_REGISTER_GAME_SERVER) {}
		MessageHeader		header;

		static const int MAX_SERVER_ID_NUM = 1024;

		int					plat_type;
		int					count;
		char				data_access_ip[16];
		int					data_access_port;
		char				gateway_hostname[64];
		unsigned short		gateway_port;
		short				reserve_sh;
		unsigned int		server_open_timestamp;				// 每个母服开启的时间
		long long			role_other_init_param_length;
		int					server_id_list[MAX_SERVER_ID_NUM];
	};

	struct CrossGameHello
	{
		CrossGameHello() : header(MT_CROSS_GAME_HELLO) {}
		MessageHeader	header;

		int is_cross_connected_hidden;
	};

	struct CrossHiddenRegisterGameServer
	{
		CrossHiddenRegisterGameServer() : header(MT_CROSS_HIDDEN_REGISTER_GAME_SERVER) {}
		MessageHeader header;

		static const int MAX_SERVER_ID_NUM = 1024;

		int					plat_type;
		int					count;
		char				data_access_ip[16];
		int					data_access_port;
		char				gateway_hostname[64];
		unsigned short		gateway_port;
		short				reserve_sh;
		unsigned int		server_open_timestamp;				// 每个母服开启的时间
		unsigned int		old_server_open_timestamp;			// 跨服最老的开服时间戳
		long long			role_other_init_param_length;
		int					server_id_list[MAX_SERVER_ID_NUM];
	};

	enum RegisterGameServerRet
	{
		ROLE_OTHER_INIT_PARAM_ERROR = -2,
		PLAT_TYPE_ERROR = -1,
		SUCC = 0,
	};

	struct HiddenGameRegisterGameServerRet
	{
		HiddenGameRegisterGameServerRet() : header(MT_HIDDEN_GAME_REGISTER_GAME_SERVER_RET) {}
		MessageHeader header;

		struct GatewayInfo
		{
			short				reserve_sh;
			unsigned short		cross_gateway_port;			// 用于转发到本服loginserver
			char				cross_gateway_hostname[64]; // 用于转发到本服loginserver
		};

		short				ret;
		short				gateway_num;
		unsigned int		cross_open_server_tamp;//跨服开服时间戳
		unsigned int		cross_old_server_open_tamp;//整个跨服最老的服务器开服时间戳
		unsigned int		cross_lun_hui_wang_chuan_refresh_tamp;
		int					hidden_server_idx;
		GatewayInfo			gateway_list[5];
	};

	struct GameCrossStartCrossReq
	{
		GameCrossStartCrossReq() : header(MT_GAME_CROSS_START_CROSS_REQ) {}
		MessageHeader header;

		int				plat_type;
		int				uid;
		int				server_id;
		PlatName		pname;

		CrossData_Business cross_business_data;
	};

	struct CrossHiddenStartCrossReq
	{
		CrossHiddenStartCrossReq() : header(MT_CROSS_HIDDEN_START_CROSS_REQ) {}
		MessageHeader header;

		int				plat_type;
		int				uid;
		int				server_id;
		PlatName		pname;

		CrossData_Business cross_business_data;
	};

	struct HiddenCrossUserLogin
	{
		HiddenCrossUserLogin() : header(MT_HIDDEN_CROSS_USER_LOGIN) {}
		MessageHeader	header;

		int				uid;

		int				original_plat_type;
		int				original_uid;
		GameName		original_user_name;
	};

	struct HiddenCrossUserLogout
	{
		HiddenCrossUserLogout() : header(MT_HIDDEN_CROSS_USER_LOGOUT) {}
		MessageHeader	header;

		int				uid;
	};

	struct HiddenCrossStartCrossAck
	{
		HiddenCrossStartCrossAck() : header(MT_HIDDEN_CROSS_START_CROSS_ACK) {}
		MessageHeader header;

		int result;
		int origin_plat_type;
		int origin_uid;
		int origin_server_id;
		PlatName orgin_platname;
		GameName role_name;
		int new_uid;
		int scene_id;
		int last_scene_id;
		unsigned int time;
		
		char gateway_hostname[64];
		int gateway_port;

		SessionKey session_key;
	};

	struct CrossGameStartCrossAck
	{
		CrossGameStartCrossAck() : header(MT_CROSS_GAME_START_CROSS_ACK) {}
		MessageHeader header;

		int result;
		int origin_uid;
		int origin_server_id;

		GameName role_name;
		int new_uid;
		int scene_id;
		int last_scene_id;
		unsigned int time;

		char gateway_hostname[64];
		int gateway_port;

		SessionKey session_key;
	};

	struct CrossGameSyncCrossUser
	{
		CrossGameSyncCrossUser() : header(MT_CROSS_GAME_SYNC_CROSS_USER) {}
		MessageHeader	header;

		enum
		{
			SYNC_CROSS_USER_INIT = 0,
			SYNC_CROSS_USER_ADD,
			SYNC_CROSS_USER_REMOVE,
		};

		static const int MAX_IN_CROSS_USER_NUM = 2048;

		int				sync_type;
		int				count;
		int				in_cross_uid_list[MAX_IN_CROSS_USER_NUM];
	};

	struct GameCrossKickCrossUser
	{
		GameCrossKickCrossUser() : header(MT_GAME_CROSS_KICK_CROSS_USER) {}
		MessageHeader	header;

		int	origin_plat_type;
		int origin_server_id;
		int origin_uid;
	};

	struct CrossHiddenKickUser
	{
		CrossHiddenKickUser() : header(MT_CROSS_HIDDEN_KICK_USER) {}
		MessageHeader	header;

		int	bekick_uid;
		int origin_plat_type;
		int origin_server_id;
		int origin_uid;
	};

	struct CrossHiddenGSDisconnect
	{
	public:
		CrossHiddenGSDisconnect() : header(MT_CROSS_HIDDEN_SERVER_DISCONNECT) {}
		MessageHeader header;

		int origin_plat_type;
		int origin_server_id;
	};

	struct HiddenCrossKickUserAck
	{
		HiddenCrossKickUserAck() : header(MT_HIDDEN_CROSS_KICK_USER_ACK) {}
		MessageHeader header;

		int	bekick_cross_uid;
		int origin_plat_type;
		int origin_server_id;
		int origin_uid;
	};

	struct CrossGameKickUserAck
	{
		CrossGameKickUserAck() : header(MT_CROSS_GAME_KICK_USER_ACK) {}
		MessageHeader header;

		int	bekick_origin_uid;
	};

	struct CrossGameUserEnteringHidden
	{
		CrossGameUserEnteringHidden() : header(MT_CROSS_GAME_USER_ENTERING_HIDDEN) {}
		MessageHeader header;

		int entering_uid;
		int new_uid;
	};

	struct HiddenCrossUserEnteringHidden
	{
		HiddenCrossUserEnteringHidden() : header(MT_HIDDEN_CROSS_USER_ENTERING_HIDDEN) {}
		MessageHeader header;

		int entering_new_uid;
		int origin_uid;
		int origin_server_id;
		int origin_plat_type;
	};

	struct GameCrossUserAllowedEnterHidden
	{
		GameCrossUserAllowedEnterHidden() : header(MT_GAME_CROSS_USER_ALLOWED_ENTER_HIDDEN) {}
		MessageHeader header;

		int allowed_uid;
		int plat_type;
		int allowed_new_uid;
	};

	struct CrossHiddenUserAllowedEnterHidden
	{
		CrossHiddenUserAllowedEnterHidden() : header(MT_CROSS_HIDDEN_USER_ALLOWED_ENTER_HIDDEN) {}
		MessageHeader header;

		int allowed_uid;
	};

	struct HiddenCrossTeammateJoinCrossTeam
	{
		HiddenCrossTeammateJoinCrossTeam() : header(MT_HIDDEN_CROSS_TEAMMATE_JOIN_TEAM) 
		{
			activity_type = ACTIVITY_TYPE_INVALID;
		}
		MessageHeader header;

		int origin_plat_type;
		int origin_server_id;
		int cross_team_index;
		int origin_team_index;
		int activity_type;
		int teammate_list[4];
	};

	struct HiddenGameTeamMatchSuccJoinTeam
	{
		HiddenGameTeamMatchSuccJoinTeam() : header(MT_HIDDEN_GAME_TEAM_MATCH_SUCC_JOIN_TEAM_REQ) { }

		MessageHeader header;
		int origin_plat_type;
		int origin_server_id;
		int cross_team_index;
		int role_id;
	};
	
	struct CrossGameTeammateJoinCrossTeam
	{
		CrossGameTeammateJoinCrossTeam() : header(MT_CROSS_GAME_TEAMMATE_JOIN_TEAM) 
		{
			cross_business_data.Reset();
		}
		MessageHeader header;

		int cross_team_index;
		CrossData_Business cross_business_data;
	};

	enum GAME_HIDDEN_MESSAGE_TYPE
	{
		GAME_HIDDEN_MESSAGE_TYPE_DEFAULT,
		GAME_HIDDEN_MESSAGE_TYPE_SEND_ALL_GAME_USER,			//发给所有玩家
		GAME_HIDDEN_MESSAGE_TYPE_SEND_GUILD_ALL_MEMBER,			//发给指定工会的所有玩家
		GAME_HIDDEN_MESSAGE_TYPE_SEND_UNIQUE_SERVER_ALL_USER,	//发给指定主服的所有玩家
		GAME_HIDDEN_MESSAGE_TYPE_SEND_TO_BIG_CROSS,				//发送到bigcross
	};

	struct GameHiddenMessage
	{
		GameHiddenMessage() : header(MT_GAME_HIDDEN_MESSAGE) {}
		MessageHeader header;

		int origin_plat_type;
		int origin_server_id;
		int type;
		union
		{
			struct
			{
		long long send_to_uuid;		// 若不为0，则直接把内容发给玩家(其uuid为send_to_cross_user_of_uuid）
			};

			struct 
			{
				int guild_id;
			};

		};

		// 紧接着内容
	};

	enum HIDDEN_GAME_MESSAGE_TYPE
	{
		HIDDEN_GAME_MESSAGE_TYPE_DEFAULT,
		HIDDEN_GAME_MESSAGE_TYPE_SEND_ALL_GAME_USER,			//发给所有玩家
		HIDDEN_GAME_MESSAGE_TYPE_SEND_GUILD_ALL_MEMBER,		//发给指定工会的所有玩家
	};

	struct HiddenGameMessage
	{
		HiddenGameMessage() : header(MT_HIDDEN_GAME_MESSAGE) 
		{
			type = HIDDEN_GAME_MESSAGE_TYPE_DEFAULT;
		}
		MessageHeader header;

		int type;
		int origin_plat_type;
		int origin_server_id;

		union 
		{
			struct 
			{
				int send_to_uid;			// 若不为0，则直接把内容发给玩家
			};

			struct 
			{
				int guild_id;
			};
		};

		// 紧接着内容
	};

	struct GameHiddenTransferMsgToBigcross
	{
		GameHiddenTransferMsgToBigcross() : header(MT_GAME_HIDDEN_TRANSFER_MSG_TO_BIGCROSS) {}
		MessageHeader header;

		int origin_plat_type;
		int origin_server_id;

		// 紧接着内容
	};

	struct HiddenGameTransferBigcrossMsgToOrigin
	{
		HiddenGameTransferBigcrossMsgToOrigin() : header(MT_HIDDEN_GAME_TRANSFER_BIGCROSS_MSG_TO_ORIGIN) {}
		MessageHeader header;

		// 紧接着内容
	};

	struct GameHiddenUserStartCrossMatch
	{
		GameHiddenUserStartCrossMatch() : header(MT_GAME_HIDDEN_USER_START_CROSS_MATCH){}
		MessageHeader header;

		int origin_plat_type;
		int origin_server_id;
		int origin_role_id;
		int role_level;
		int match_team_type;
		int role_guild_id;
		bool is_auto_create;			// 一定时间匹配不到人是否自动创建队伍[0:否 1 : 是]
		char reserve_ch;
		short reserve_sh;	
	};

	struct GameHiddenUserStopCrossMatch
	{
		GameHiddenUserStopCrossMatch() : header(MT_GAME_HIDDEN_USER_STOP_CROSS_MATCH) {}
		MessageHeader header;

		int origin_plat_type;
		int origin_role_id;
	};

	struct HiddenGameSyncCrossTeamInfo
	{
		HiddenGameSyncCrossTeamInfo() : header(MT_HIDDEN_GAME_SYNC_CROSS_TEAM_INFO) {}
		MessageHeader header;

		enum NOTIFY_REASON
		{
			NOTIFY_REASON_INIT = 0,
			NOTIFY_REASON_INFO_CHANGE = 1,
			NOTIFY_REASON_INIT_AGAIN = 2,   // 继续初始化   应对一次发不完的情况
			NOTIFY_REASON_ADD = 3,			// 新增
			NOTIFY_REASON_REMOVE = 4,		// 去掉
		};

		short notify_reason;
		short team_count;

		TeamItem team_list[MAX_TEAM_LIST_NUM];
	};

	struct GameHiddenTradeMarketUserShelveInfo
	{
		GameHiddenTradeMarketUserShelveInfo() : header(MT_GAME_HIDDEN_TRADE_MARKET_USER_SHELVE_INFO) {}
		MessageHeader header;

		long long uuid;

		int plat_type;					//!< 用于返回
		int server_id;					//!< 用于返回
	};

	struct GameHiddenTradeMarketSearchById
	{
		GameHiddenTradeMarketSearchById() : header(MT_GAME_HIDDEN_TRADE_MARKET_SEARCH_BY_ID) {}
		MessageHeader header;

		long long uuid;					//!< 请求者的uuid

		ItemID item_id;					//!< 物品ID
		short req_page;					//!< 请求的页数
		short is_announce;				//!< 是否查找正在公示期的 0：否   1：是
		short order_type;				//!< 排序类型  0:按价格低到高   1:按价格高到低
		
		int plat_type;					//!< 用于返回
		int server_id;					//!< 用于返回
	};

	struct GameHiddenTradeMarketSearchByType
	{
		GameHiddenTradeMarketSearchByType() : header(MT_GAME_HIDDEN_TRADE_MARKET_SEARCH_BY_TYPE) {}
		MessageHeader header;

		long long uuid;

		short item_search_type;			//!< 物品搜索类型 \note 见 J-交易行.xls -> 筛选sheet
		short req_page;					//!< 请求的页数

		short is_announce;				//!< 是否查找正在公示期的 0：否   1：是
		short order_type;				//!< 排序类型  0:按价格低到高   1:按价格高到低

		short item_big_search_type;		//!< 值为-1时，则按小类(item_search_type)搜索，否则则按大类搜索
		short param;					//!< //!< 特殊参数（当搜索装备时param代表等级，当搜索令牌时param代表特效职业）(为-1时则按"全部"搜索)

		int plat_type;					//!< 用于返回
		int server_id;					//!< 用于返回
	};

	struct GameHiddenTradeMarketSearchByBigType
	{
		GameHiddenTradeMarketSearchByBigType() : header(MT_GAME_HIDDEN_TRADE_MARKET_SEARCH_BY_BIG_TYPE) {}
		MessageHeader header;

		long long uuid;

		short item_search_type;			//!< 物品搜索类型 \note 见 J-交易行.xls -> 筛选sheet
		short req_page;					//!< 请求的页数

		short is_announce;				//!< 是否查找正在公示期的 0：否   1：是
		short order_type;				//!< 排序类型  0:按价格低到高   1:按价格高到低

		short item_big_search_type;		//!< 值为-1时，则按小类(item_search_type)搜索，否则则按大类搜索
		short param;					//!< 特殊参数（当搜索装备时param代表等级，当搜索令牌时param代表特效职业）(为-1时则按"全部"搜索)

		int plat_type;					//!< 用于返回
		int server_id;					//!< 用于返回
	};

	struct GameHiddenTradeMarketSearchByTypeWithLevel
	{
		GameHiddenTradeMarketSearchByTypeWithLevel() : header(MT_GAME_HIDDEN_TRADE_MARKET_SEARCH_BY_TYPE_WITH_LEVEL) {}
		MessageHeader header;

		long long uuid;

		short item_search_type;			//!< 物品搜索类型 \note 见 J-交易行.xls -> 筛选sheet
		short req_page;					//!< 请求的页数

		short is_announce;				//!< 是否查找正在公示期的 0：否   1：是
		short order_type;				//!< 排序类型  0:按价格低到高   1:按价格高到低

		short item_big_search_type;		//!< 值为-1时，则按小类(item_search_type)搜索，否则则按大类搜索
		short level_param;				//!< 等级参数（当搜索装备时）(为0时则按"全部"搜索)

		int plat_type;					//!< 用于返回
		int server_id;					//!< 用于返回
	};

	struct GameHiddenTradeMarketSearchMedalByColor
	{
		GameHiddenTradeMarketSearchMedalByColor() : header(MT_GAME_HIDDEN_TRADE_MARKET_SEARCH_MEDAL_BY_COLOR) {}
		MessageHeader header;

		long long uuid;

		short medal_color;				//!< 令牌颜色
		short req_page;					//!< 请求的页数

		short is_announce;				//!< 是否查找正在公示期的 0：否   1：是
		short order_type;				//!< 排序类型  0:按价格低到高   1:按价格高到低

		int prof_param;					//!< 令牌特效职业参数 -1为全部 0为通用，[1,8]为职业

		int plat_type;					//!< 用于返回
		int server_id;					//!< 用于返回
	};

	struct GameHiddenTradeMarketSearchMedalByEffectTypeAndColor
	{
		GameHiddenTradeMarketSearchMedalByEffectTypeAndColor() : header(MT_GAME_HIDDEN_TRADE_MARKET_SEARCH_MEDAL_BY_SP_TYPE_AND_COLOR) {}
		MessageHeader header;

		long long uuid;

		short sp_type;					//!< 特效类型
		short medal_color;				//!< 令牌颜色

		short reserve_sh;				//!< 保留字
		short req_page;					//!< 请求的页数

		short is_announce;				//!< 是否查找正在公示期的 0：否   1：是
		short order_type;				//!< 排序类型  0:按价格低到高   1:按价格高到低

		int plat_type;					//!< 用于返回
		int server_id;					//!< 用于返回
	};

	struct GameHiddenTradeMarketSearchPetForQuickBuy
	{
		GameHiddenTradeMarketSearchPetForQuickBuy() : header(MT_GAME_HIDDEN_TRADE_MARKET_SEARCH_PET_FOR_QUICK_BUY) {}
		MessageHeader header;

		int role_id;

		short order_type;						//!< 筛选顺序 \see TRADE_PET_INDEX_ORDER
		ItemID item_id;							//!< 要搜的宠物物品ID

		int last_seller_uid;					//!< 上次搜到的最后一件商品的卖家UID （第一次搜索或更换筛选顺序时发0）
		int last_price;							//!< 上次搜到的最后一件商品的价格	 （第一次搜索或更换筛选顺序时发0）
		unsigned int last_put_on_shelve_time;	//!< 上次搜到的最后一件商品的上架时间（第一次搜索或更换筛选顺序时发0）
		short last_str_level;					//!< 上次搜到的最后一件商品的强化等级（第一次搜索或更换筛选顺序时发0）
		short quality;							//!< 宠物品质

		int plat_type;							//!< 用于返回
		int server_id;							//!< 用于返回
	};

	struct GameHiddenTradeMarketSearchAdvertiseItemReq
	{
		GameHiddenTradeMarketSearchAdvertiseItemReq() : header(MT_GAME_HIDDEN_TRADE_MARKET_SEARCH_ADVERTISE_REQ) {}
		MessageHeader header;

		int role_id;
		int plat_type;						//!< 用于返回
		int server_id;						//!< 用于返回

		int seller_uid;						//!< 卖家uid
		unsigned int put_on_shelve_time;	//!< 上架时间
		int price;							//!< 价格
		short sale_index;					//!< 在货架上的序号
		ItemID item_id;						//!< 物品ID
		long long unique_key;				//!< 唯一标识
	};

	struct GameHiddenTradeMarketGetTransactionRecord
	{
		GameHiddenTradeMarketGetTransactionRecord() : header(MT_GAME_HIDDEN_TRADE_MARKET_GET_TRANSACTION_RECORD) {}
		MessageHeader header;

		long long uuid;

		ItemID item_id;						//!< 物品ID
		short reserve_sh;					//!< 保留字

		int plat_type;					//!< 用于返回
		int server_id;					//!< 用于返回
	};

	struct GameHiddenTradeMarketGetRoleTradeRecord
	{
		GameHiddenTradeMarketGetRoleTradeRecord() : header(MT_GAME_HIDDEN_TRADE_MARKET_GET_ROLE_TRADE_RECORD) {}
		MessageHeader header;

		long long uuid;

		int plat_type;					//!< 用于返回
		int server_id;					//!< 用于返回
	};

	struct GameHiddenTradeMarketGetRoleFocusList
	{
		GameHiddenTradeMarketGetRoleFocusList() : header(MT_GAME_HIDDEN_TRADE_MARKET_GET_ROLE_FOCUS_LIST){}
		MessageHeader header;

		long long uuid;

		int plat_type;					//!< 用于返回
		int server_id;					//!< 用于返回
	};

	struct GameHiddenTradeMarketGetFocusItemInfo
	{
		GameHiddenTradeMarketGetFocusItemInfo() : header(MT_GAME_HIDDEN_TRADE_MARKET_GET_FOCUS_ITEM_INFO) {}
		MessageHeader header;

		long long uuid;

		int focus_index;					//!< 商品在我的关注列表中的序号  \note 详见SCTradeMarketGetFocusList中的focus_index
		short cur_page;						//!< 当前正在浏览哪一页
		short is_announce;					//!< 当前正在浏览的是公示期还是发售期的关注列表

		int plat_type;						//!< 用于返回
		int server_id;						//!< 用于返回
	};

	struct GameHiddenTradeMarketRemoveFocusItem
	{
		GameHiddenTradeMarketRemoveFocusItem() : header(MT_GAME_HIDDEN_TRADE_MARKET_REMOVE_FOCUS_ITEM) {}
		MessageHeader header;

		long long uuid;

		int focus_index;					//!< 商品在我的关注列表中的序号  \note 详见SCTradeMarketGetFocusList中的focus_index
		short cur_page;						//!< 当前正在浏览哪一页
		short is_announce;					//!< 当前正在浏览的是公示期还是发售期的关注列表

		int plat_type;						//!< 用于返回
		int server_id;						//!< 用于返回
	};

	struct GameHiddenTradeMarketAddFocusItem
	{
		GameHiddenTradeMarketAddFocusItem() : header(MT_GAME_HIDDEN_TRADE_MARKET_ADD_FOCUS_ITEM) {}
		MessageHeader header;

		long long uuid;

		int seller_uid;						//!< 卖家uid
		short sale_index;					//!< 在卖家货架上的序号
		short reserve_sh;					//!< 保留字
		unsigned int upshelve_time;			//!< 商品上架时间 （用于核对物品）

		int plat_type;						//!< 用于返回
		int server_id;						//!< 用于返回
	};

	struct GameHiddenTradeMarketGetMerchandiseNum
	{
		GameHiddenTradeMarketGetMerchandiseNum() : header(MT_GAME_HIDDEN_TRADE_MARKET_GET_MERCHANDISE_NUM) {}
		MessageHeader header;

		long long uuid;

		short is_announce;		//!< 0:发售期   1:公示期
		short big_search_type;	//!< 大类  1：武器	2：防具  3：宠物  10：灵饰

		int plat_type;						//!< 用于返回
		int server_id;						//!< 用于返回
	};

	struct GameHiddenTradeMarketGetSellingList
	{
		GameHiddenTradeMarketGetSellingList() : header(MT_GAME_HIDDEN_TRADE_MARKET_GET_SELLING_LIST) {}
		MessageHeader header;

		long long uuid;

		ItemID item_id;
		short special_effect_type;

		int plat_type;						//!< 用于返回
		int server_id;						//!< 用于返回
	};

	struct GameHiddenUserApplyJoinCrossTeam
	{
		GameHiddenUserApplyJoinCrossTeam() : header(MT_GAME_HIDDEN_USER_APPLY_JOIN_CROSS_TEAM) {}
		MessageHeader header;

		int origin_plat_type;
		int origin_server_id;
		int origin_role_id;
		int cross_team_index;

		int level;
		int profession;
		long long avatar_timestamp;
		GameName name;
		short avatar_type;
		short headshot_id;

		int guild_id;
	};

	struct HiddenGameSendMailTransfer
	{
		HiddenGameSendMailTransfer() : header(MT_HIDDEN_GAME_SEND_MAIL_TRANSFER) {}
		MessageHeader header;

		int origin_plat_type;
		int origin_server_id;
		int recieve_uid;

		MailParam mail_param;
	};

	struct GameHiddenSendMailTransferToAnotherServer
	{
		GameHiddenSendMailTransferToAnotherServer() : header(MT_GAME_HIDDEN_SEND_MAIL_TRANSFER_TO_ANOTHER_SERVER) {}
		MessageHeader header;

		int origin_plat_type;
		int origin_server_id;
		int recieve_uid;

		MailParam mail_param;
	};

	struct HiddenGameRequestGuildList
	{
		HiddenGameRequestGuildList() : header(MT_HIDDEN_GAME_REQUEST_GUILD_LIST) {}
		MessageHeader header;
	};

	struct GameHiddenResponseGuildList 
	{
		GameHiddenResponseGuildList() : header(MT_GAME_HIDDEN_RESPONSE_GUILD_LIST) {}
		MessageHeader header;		
		
		int plat_type;
		int server_id;
		short is_first;			// 是否第一批（第一批则应清掉之前那些）
		short is_last;			// 是否最后一批

		// 以下是Serialize过的AllGuildParam数据，取出时需要先Deserialize
	};

	struct GameHiddenQueryIfSyncGuildSucceed
	{
		GameHiddenQueryIfSyncGuildSucceed() : header(MT_GAME_HIDDEN_QUERY_IF_SYNC_GUILD_SUCCEED){}
		MessageHeader header;

		int plat_type;
		int server_id;
	};

	struct HiddenGameSyncGuildSuccNotice
	{
		HiddenGameSyncGuildSuccNotice() : header(MT_HIDDEN_GAME_SYNC_GUILD_SUCC_NOTICE) {}
		MessageHeader header;
	};

	// 发送到cross查找crossuser，若没有则转发到originserver，若有则转发到hiddenserver
	struct G2GRoleChatTransfer
	{
		bool Push(void * src, unsigned int length)
		{
			if (data_size + length > MAX_DATA_SIZE)
			{
				return false;
			}

			memcpy(data + data_size, src, length);
			data_size += length;

			return true;
		}

		bool Pop(void * dst, unsigned int length)
		{
			if (pop_offset + length > data_size)
			{
				return false;
			}

			memcpy(dst, data + pop_offset, length);
			pop_offset += length;

			return true;
		}

		G2GRoleChatTransfer() : header(MT_GAME_TO_GAME_ROLE_CHAT_TRANSFER) , pop_offset(0), data_size(0){}
		MessageHeader header;

		int plat_type;
		int to_gameserver_id;
		int to_role_id;
		int send_to_hidden_first;		// 是否先发到隐藏服

		const unsigned int static MAX_DATA_SIZE = 8192;
		unsigned int pop_offset;
		unsigned int data_size;
		char data[MAX_DATA_SIZE];
	};

	enum ReasonType
	{
		SYNC_REASON_ADD = 0,
		SYNC_REASON_REMOVE = 1,
	};

	// 好友

	enum GAME_HIDDEN_GET_FRIEND_INFO_TYPE
	{
		GAME_HIDDEN_GET_FRIEND_INFO_TYPE_INVALID= 0,		
		GAME_HIDDEN_GET_FRIEND_INFO_TYPE_FRIEND_LIST = 1,	//好友列表
		GAME_HIDDEN_GET_FRIEND_INFO_TYPE_ASK_LIST = 2,		//好友申请列表
		GAME_HIDDEN_GET_FRIEND_INFO_TYPE_BLACK_LIST = 3,	//黑名单列表
	
		GAME_HIDDEN_GET_FRIEND_INFO_TYPE_MAX,
	};

	struct GameHiddenGetFriendInfo						// 原服向隐藏服 同步客户端 好友操作请求
	{
		GameHiddenGetFriendInfo() : header(MT_GAME_HIDDEN_GET_FRIEND_INFO) {}
		MessageHeader header;

		UniqueServerID	usid;							// 操作请求的玩家来至哪个服
		int role_id;									// 操作请求的玩家ID
		int info_type;									// 需要获取的信息类型
	};
	
	struct HiddenGameFriendListToRole					// 隐藏服向原服 同步玩家的好友列表
	{
		HiddenGameFriendListToRole() : header(MT_HIDDEN_GAME_FRIEND_LIST_TO_ROLE) {}
		MessageHeader header;

		UniqueServerID	usid;								// 接受该信息的玩家来至哪个服
		int role_id;										// 接受该信息的玩家ID
		int				count;								// 表长
		FriendInfo		friend_list[MAX_FRIEND_NUM];		//!< 好友信息，大小100
	};

	struct GameHiddenAddFriendReq						// 原服向隐藏服  同步玩家申请添加好友操作
	{
		GameHiddenAddFriendReq() : header(MT_GAME_HIDDEN_ADD_FRIEND_REQ) {}
		MessageHeader header;

		UniqueServerID usid;
		int role_id;				// 申请人
		int target_id;				// 待审核人
		bool is_yijian;				// 是否是一键[0:否 , 1:是]

									// 申请人的基础信息
		GameName role_name;
		int avatar_type;
		int prof;
		int level;
		int sex;
		int headshot_id;
		int top_level;
		unsigned int avatar_timestamp;
	};

	struct HiddenGameSyncRoleReqRet					// 隐藏服向原服  同步玩家操作结果主要用于errornum
	{
		HiddenGameSyncRoleReqRet() : header(MT_HIDDEN_GAME_SYNC_ROLE_REQ_RET) {}
		MessageHeader header;

		int role_id;			// 操作人
		int error_num;			// 操作结果
	};

	enum HIDDEN_GAME_FRIEND_ASK_LIST_CHANGE_STATUS
	{
		HIDDEN_GAME_FRIEND_ASK_LIST_CHANGE_STATUS_INVALID = 0,		// 无效
		HIDDEN_GAME_FRIEND_ASK_LIST_CHANGE_STATUS_INSERT = 1,		// 添加			
		HIDDEN_GAME_FRIEND_ASK_LIST_CHANGE_STATUS_DELETE = 2,		// 删除		
		HIDDEN_GAME_FRIEND_ASK_LIST_CHANGE_STATUS_MAX,
	};

	struct HiddenGameFriendAskListChange					// 隐藏服向原服  同步玩家的好友申请列表变更
	{
		HiddenGameFriendAskListChange() : header(MT_HIDDEN_GAME_FRIEND_ASK_LIST_CHANGE) {}
		MessageHeader header;

		int role_id;				// 申请人
		int target_id;				// 待审核人
		char change_type;			// 变更类型
		bool is_yijian;				// 是否是一键[0:否 , 1:是]
		bool is_send_friend_route;	// 是否已经发送过好友申请了 [0:没有 , 1:发送过了]

		GameName role_name;
		int avatar_type;
		int prof;
		int level;
		int sex;
		int headshot_id;
		int top_level;
		unsigned int avatar_timestamp;
	};

	struct GameHiddenAddFriendReplyReq			// 同步好友申请操作结果 , 不知道从哪同步到哪..需要看具体情况 
	{
		GameHiddenAddFriendReplyReq() : header(MT_GAME_HIDDEN_ADD_FRIEND_REPLY_REQ) {}
		MessageHeader header;

		int role_id;			// 操作人
		int apply_id;			// 目标对象
		int is_accept;			// 是否同意
	};

	struct GameHiddenRemoveFriendReq			// 原服向隐藏服同步 删除好友操作
	{
		GameHiddenRemoveFriendReq() : header(MT_GAME_HIDDEN_REMOVE_FRIEND_REQ) {}
		MessageHeader header;

		int role_id;			// 操作人
		int target_id;			// 目标对象
	};

	struct GameHiddenAddBlackReq				// 原服向隐藏服  同步添加黑名单操作
	{
		GameHiddenAddBlackReq() : header(MT_GAME_HIDDEN_ADD_BLACK_REQ) {}
		MessageHeader header;

		UniqueServerID  usid;	// 操作人来至哪个服
		int role_id;			// 操作人
		int target_id;			// 目标对象
	};

	struct HiddenGameBlackList								// 隐藏服向原服 同步玩家的黑名单列表
	{
		HiddenGameBlackList() : header(MT_HIDDEN_GAME_BLACK_LIST) {}
		MessageHeader header;

		UniqueServerID	usid;								// 接受该信息的玩家来至哪个服
		int role_id;										// 接受该信息的玩家ID
		int count;											// 表长
		FriendInfo			black_info[MAX_BLACK_NUM];		// 黑名单信息表
	};

	struct HiddenGameBlackListChange						// 隐藏服向原服 同步玩家的黑名单列表变更
	{
		HiddenGameBlackListChange() : header(MT_HIDDEN_GAME_BLACK_LIST_CHANGE) {}
		MessageHeader header;

		int role_id;										// 接受该信息的玩家ID
		int change_type;									// 变更类型
		FriendInfo			black_info;						// 黑名单角色信息
	};

	struct GameHiddenRemoveBlackReq							// 原服向隐藏服同步 删除黑名单操作
	{
		GameHiddenRemoveBlackReq() : header(MT_GAME_HIDDEN_REMOVE_BLACK_REQ) {}
		MessageHeader header;

		UniqueServerID usid;	// 操作人所在服务器
		int role_id;			// 操作人
		int target_id;			// 目标对象
	};

	enum GAME_HIDDEN_FRIEND_GROUP_REQ_TYPE
	{
		GAME_HIDDEN_FRIEND_GROUP_REQ_TYPE_INVALID = 0,				// 无效
		GAME_HIDDEN_FRIEND_GROUP_REQ_TYPE_DEL_GROUP = 1,			// 删除组		
		GAME_HIDDEN_FRIEND_GROUP_REQ_TYPE_EDIT_GROUP = 2,			// 编辑组	
		GAME_HIDDEN_FRIEND_GROUP_REQ_TYPE_RENAME_GROUP = 3,			// 重命名组
		GAME_HIDDEN_FRIEND_GROUP_REQ_TYPE_MOVE_GROUP = 4,			// 移动组
		GAME_HIDDEN_FRIEND_GROUP_REQ_TYPE_MAX,
	};

	struct GameHiddenFriendGroupReq							// 原服向隐藏服同步 好友组操作
	{
		GameHiddenFriendGroupReq() : header(MT_GAME_HIDDEN_FRIEND_GROUP_REQ) {}
		MessageHeader header;

		int role_id;						// 操作人
		int req_type;						// 操作类型

		int group_type;
		GameName group_name;
		int count;							// 表长
		int friend_id_arr[MAX_FRIEND_NUM];	// 移动的好友角色id，不用补齐100
	};

	struct HiddenGameAskListToRole							// 隐藏服向原服 同步玩家的好友申请列表
	{
		HiddenGameAskListToRole() : header(MT_HIDDEN_GAME_ASK_LIST_TO_ROLE) {}
		MessageHeader header;

		UniqueServerID	usid;								// 接受该信息的玩家来至哪个服
		int role_id;										// 接受该信息的玩家ID
		int count;											// 表长
		FriendInfo			ask_info[MAX_FRIEND_NUM];		// 好友申请列表信息
	};
	
	enum GAME_HIDDEN_RECENTLY_REQ_TYPE
	{
		GAME_HIDDEN_RECENTLY_REQ_TYPE_ADD_CHAT,				// 添加近聊 
		GAME_HIDDEN_RECENTLY_REQ_TYPE_GET_CHAT_INFO,		// 获取近聊信息
		GAME_HIDDEN_RECENTLY_REQ_TYPE_GET_TEAM_INFO,		// 获取最近组队信息
		GAME_HIDDEN_RECENTLY_REQ_TYPE_ADD_TEAM,				// 添加最近组队
	};

	struct GameHiddenRecentlyReq							// 原服向隐藏服  同步玩家处理近聊相关操作
	{
		GameHiddenRecentlyReq() : header(MT_GAME_HIDDEN_RECENTLY_REQ) {}
		MessageHeader header;

		UniqueServerID	usid;								// 操作人来至哪个服
		int role_id;										// 操作人
		int req_type;										// 操作类型

		int count;
		int target_id[MAX_RECENTLY_CHAT_NUM];				// 目标ID
										
	};

	enum HIDDEN_GAME_RECENTLY_INFO_TYPE
	{
		HIDDEN_GAME_RECENTLY_INFO_TYPE_INVALID = 0,			
		HIDDEN_GAME_RECENTLY_INFO_TYPE_ADD_CHAT,			// 近聊信息变更
		HIDDEN_GAME_RECENTLY_INFO_TYPE_GET_CHAT_INFO,		// 获取近聊信息
		HIDDEN_GAME_RECENTLY_INFO_TYPE_GET_TEAM_INFO,		// 获取最近组队信息
		HIDDEN_GAME_RECENTLY_INFO_TYPE_ADD_TEAM,			// 最近组队变更
	};

	UNSTD_STATIC_CHECK(MAX_RECENTLY_CHAT_NUM == MAX_RECENTLY_TEAM_NUM);

	struct HiddenGameRecentlyInfo									// 隐藏服向原服  同步玩家的近聊相关信息
	{
		HiddenGameRecentlyInfo() : header(MT_HIDDEN_GAME_RECENTLY_INFO) {}
		MessageHeader header;

		UniqueServerID	usid;										// 接受该信息的玩家来至哪个服
		int role_id;												// 接受该信息的玩家ID
		int info_type;												// 信息类型

		int count;													// 表长
		FriendInfo			info_arr[MAX_RECENTLY_CHAT_NUM];		// 好友申请列表信息 ,近聊和最近组队都用这个
	};

	struct HiddenGameFriendChangeToRole								// 隐藏服向原服  同步好友变更信息
	{
		HiddenGameFriendChangeToRole() : header(MT_HIDDEN_GAME_FRIEND_CHANGE_TO_ROLE) {}
		MessageHeader header;

		int role_id;												// 接受该信息的玩家ID
		int change_type;											// 变更类型

		FriendInfo	friend_info;									// 好友信息
	};

	struct GameHiddenRoleLoginLogoutFriendNotify							// 原服向隐藏服  同步玩家上||下线通知到隐藏服
	{	
		GameHiddenRoleLoginLogoutFriendNotify() : header(MT_GAME_HIDDEN_ROLE_LOGIN_LOGOUT_FRIENDS_NOTIFY) {}
		MessageHeader header;

		int role_id;												// 玩家ID
		int online_status;											// 是否是登录
	};


	enum SYNC_FRIEND_TO_CROSS_STATUS
	{
		SYNC_FRIEND_TO_CROSS_STATUS_INVALID = 0,	// 无效
		SYNC_FRIEND_TO_CROSS_STATUS_INSERT = 1,		// 添加		
		SYNC_FRIEND_TO_CROSS_STATUS_UPDATA = 2,		// 更新		
		SYNC_FRIEND_TO_CROSS_STATUS_DELETE = 3,		// 删除		
		SYNC_FRIEND_TO_CROSS_STATUS_MAX,
	};

	struct SyncFriendInfo								// 从原服向隐藏服同步好友数据
	{
		SyncFriendInfo() : header(MT_SYNC_FRIEND_INFO) {}
		MessageHeader header;

		struct FriendItemSyncInfo
		{
			FriendItemSyncInfo() : sync_type(0) {};
			unsigned int sync_type;						// 同步状态 // 单数据才有效 , 多数据不看这个
			FriendItem friend_item;
		};

		UniqueServerID from_usid;						// 来至哪个原服 [可能用不上]
		bool is_init;									// 初始的数据全覆盖
		char is_init_add;								// 0 : 覆盖  1 : 添加
		short reserve_sh;

		int count;										// 表长
		FriendItemSyncInfo item_list[MAX_ONCE_FRIENDS_COUNT];
	};


	struct SyncAddFriendOfflineEvent			// 同步好友添加时玩家处于离线状态 , 则上线时需要处理一下成就等相关的操作
	{
		SyncAddFriendOfflineEvent() : header(MT_SYNC_ADD_FRINED_OFFLINE_EVENT) {}
		MessageHeader header;

		int role_id;			// 操作人
		int req;				// 0:删除 , 1:添加	
	};

	// 黑名单
	enum SYNC_BLACK_TO_CROSS_STATUS
	{
		SYNC_BLACK_TO_CROSS_STATUS_INVALID = 0,		// 无效
		SYNC_BLACK_TO_CROSS_STATUS_INSERT = 1,		// 添加		
		SYNC_BLACK_TO_CROSS_STATUS_UPDATA = 2,		// 更新		
		SYNC_BLACK_TO_CROSS_STATUS_DELETE = 3,		// 删除		
		SYNC_BLACK_TO_CROSS_STATUS_MAX,
	};
	struct SyncBlackInfo						// 从原服向隐藏服同步黑名单信息
	{
		SyncBlackInfo() : header(MT_SYNC_BLACK_INFO) {}
		MessageHeader header;

		struct BlackItemSyncInfo
		{
			BlackItemSyncInfo() : sync_type(0) {};
			unsigned int sync_type;						// 同步状态 // 单数据才有效 , 多数据不看这个
			BlackItem black_item;
		};

		UniqueServerID from_usid;						// 来至哪个原服 [可能用不上]
		bool is_init;									// 初始的数据全覆盖
		char is_init_add;								// 0 : 覆盖  1 : 添加
		short reserve_sh;

		int count;										// 表长
		BlackItemSyncInfo item_list[MAX_ONCE_BLACK_COUNT];
	};

	struct GameHiddenSyncGuildMemberChange
	{
		GameHiddenSyncGuildMemberChange() : header(MT_GAME_HIDDEN_SYNC_GUILD_MEMBER_CHANGE) {}
		MessageHeader header;

		enum OP_TYPE
		{
			OP_TYPE_ADD = 0,
			OP_TYPE_REMOVE = 1,
			OP_TYPE_UPDATE = 2,
			OP_TYPE_UPDATE_2 = 3,
		};

		int guild_id;
		int plat_type;
		int op_type;
		int param1;
		GuildMemberParam::Member member;
	};

	struct HiddenGameSyncGuildMemberChange
	{
		HiddenGameSyncGuildMemberChange() : header(MT_HIDDEN_GAME_SYNC_GUILD_MEMBER_CHANGE) {}
		MessageHeader header;

		enum OP_TYPE
		{			
			OP_TYPE_UPDATE = 2,
		};

		int guild_id;
		int plat_type;
		int op_type;
		int param1;
		GuildMemberParam::Member member;
	};

	struct GameHiddenSyncGuildChange : public IMessageSerializer, public IMessageDeserializer
	{
		GameHiddenSyncGuildChange() : header(MT_GAME_HIDDEN_SYNC_GUILD_CHANGE) {}
		MessageHeader header;

		virtual ~GameHiddenSyncGuildChange() {}
		virtual int GetMessageType() const { return header.msg_type; }

		enum OP_TYPE
		{
			OP_TYPE_ADD = 0,
			OP_TYPE_REMOVE = 1,
			OP_TYPE_UPDATE = 2,
		};

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT(guild_id)
				&& WRITE_INT(plat_type)
				&& WRITE_INT(op_type);

			assert(result);
			if (!result) return false;

			if (OP_TYPE_REMOVE == op_type) return result;

			result = WRITE_STRN((const char*)&param.base_param, sizeof(param.base_param))
				&& WRITE_STRN((const char*)&param.gwt_param, sizeof(param.gwt_param))
				&& WRITE_STRN((const char*)&param.guild_boss_param, sizeof(param.guild_boss_param));

			assert(result);
			if (!result) return false;

			result = WRITE_INT(param.member_param.count);
			if (!result) return false;

			for (int i = 0; i < param.member_param.count && i < ARRAY_ITEM_COUNT(param.member_param.member_list); ++i)
			{
				result = WRITE_STRN((const char*)&param.member_param.member_list[i], sizeof(param.member_param.member_list[i]));
				assert(result);
				if (!result) return false;
			}

			assert(result);
			return result;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			bool result = true;

			result = READ_BEGIN(header.msg_type)
				&& READ_INT(guild_id)
				&& READ_INT(plat_type)
				&& READ_INT(op_type);

			if (!result) return false;

			if (OP_TYPE_REMOVE == op_type) return result;

			result = READ_STRN((char*)&param.base_param, sizeof(param.base_param), sizeof(param.base_param))
				&& READ_STRN((char*)&param.gwt_param, sizeof(param.gwt_param), sizeof(param.gwt_param))
				&& READ_STRN((char*)&param.guild_boss_param, sizeof(param.guild_boss_param), sizeof(param.guild_boss_param));

			if (!result) return false;

			result = READ_INT(param.member_param.count);
			if (!result) return false;

			for (int i = 0; i < param.member_param.count && i < ARRAY_ITEM_COUNT(param.member_param.member_list); ++i)
			{
				result = READ_STRN((char*)&param.member_param.member_list[i], 
					sizeof(param.member_param.member_list[i]), 
					sizeof(param.member_param.member_list[i]));
				if (!result) return false;
			}

			return result;
		}
		
		int guild_id;
		int plat_type;
		int op_type;

		AllGuildParam::GuildParam param;
	};

	struct HiddenGameGuildMemberAddTitle
	{
		HiddenGameGuildMemberAddTitle() : header(MT_HIDDEN_GAME_ADD_TITLE_TO_GUILD_MEMBER) {}
		MessageHeader header;

		int guild_id;
		int title_id;
		char reason[40];
	};

	//----------------------------------------------------------------
	struct GameHiddenReqSyncWorldStatus
	{
		GameHiddenReqSyncWorldStatus() : header(MT_GAME_HIDDEN_REQ_SYNC_WORLD_STATUS) {}
		MessageHeader	header;

		int server_id;
	};

	struct HiddenGameSyncWorldStatus
	{
		HiddenGameSyncWorldStatus() : header(MT_GAME_HIDDEN_SYNC_WORLD_STATUS) {}
		MessageHeader	header;

		ChiefElectionDataParam chief_election_data;
		ChiefPeakBattleDataParam chief_peak_battle_data;
		int world_wst_group_id;								//世界答题题目组
		int world_wst_answer_subject;						//世界答题题组ID
		int world_wst_answer;								//世界答题答案ID
		long long world_wst_answer_stamp;					//世界答题结束回答时间戳
		long long world_wst_show_answer_stamp;				//世界答题展示答案时间戳
		long long world_wst_next_subject_refresh_stamp;		//世界答题下一次题目刷新时间
		long long answer_end_stamp;							//世界答题结束时间戳
		long long next_answer_stamp;						//世界答题下一次活动开启时间戳
		unsigned int can_find_end_timestamp_list[MAX_DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_NUM];		//特殊可找回奖励结束时间戳
		WorldStatusGuildFightParam guild_fight_info;
		unsigned int open_cross_server_zero_timestamp;		//跨服服务器配置的开服0点时间戳
	};


	struct HiddenGameActivityForceToNextState
	{
		HiddenGameActivityForceToNextState() : header(MT_GAME_HIDDEN_ACTIVITY_FORCE_TO_NEXT_STATE) {}
		MessageHeader	header;

		int activity_type;
		unsigned int next_status_switch_timestamp;
	};

	struct GameHiddenReqSyncActivityStatus
	{
		GameHiddenReqSyncActivityStatus() : header(MT_GAME_HIDDEN_REQ_SYNC_ACTIVITY_STATUS) {}
		MessageHeader	header;

		int server_id;
	};

	struct HiddenGameSyncActivityStatus
	{
		HiddenGameSyncActivityStatus() : header(MT_HIDDEN_GAME_SYNC_ACTIVITY_STATUS) {}
		MessageHeader	header;

		int activity_type;
		int activity_status;
		unsigned int next_status_switch_time;
	};

	//-------------------------------首席----------------------------
	
	//竞选
	struct GameHiddenChiefElectionReq
	{
		GameHiddenChiefElectionReq() : header(MT_HIDDEN_GAME_CHIEF_ELECTION_REQ) {}
		MessageHeader header;

		int origin_plat_type;
		int origin_server_id;
		int origin_role_id;
		int base_prof;
		int req_type;
		int p1;
		int p2;
	};

	//巅峰战
	struct GameHiddenChiefPeakBattleReq
	{
		GameHiddenChiefPeakBattleReq() : header(MT_HIDDEN_GAME_CHIEF_PEAK_BATTLE_REQ) {}
		MessageHeader header;

		int origin_plat_type;
		int origin_server_id;
		int origin_role_id;
		int req_type;
		int p1;
		int p2;
	};

	//-------------------------------工会荣耀战-------------------------
	struct GameHiddenGuildHonorBattleReqRoute
	{
		GameHiddenGuildHonorBattleReqRoute() : header(MT_HIDDEN_GAME_GUILD_HONOR_BATTLE_REQ_ROUTE) {}
		MessageHeader header;

		int role_id;
		int role_guild_id;
		int req_type;
		int p1;
		int p2;
	};

	struct HiddenGameGuildHonorBattleMatchRecordSyncInfo
	{
		HiddenGameGuildHonorBattleMatchRecordSyncInfo() : header(MT_HIDDEN_GAME_GUILD_HONOR_BATTLE_MATCH_RECORD_SYNC_INFO) {}
		MessageHeader header;

		int is_notice;
		CommonSaveGuildHonorRecordData match_record_data;
	};
	//------------------------------------------------------

	// 玩家基本信息发生改变，同步到跨服
	struct GameHiddenUserCacheSync
	{
		GameHiddenUserCacheSync() : header(MT_GAME_HIDDEN_USERCACHE_CHANGE_SYNC) {}
		MessageHeader header;

		short is_name_changed;
		short is_new_node;
		int online_status;
		GameName old_name;
		UserCacheParam usercache_param;
	};


	// 玩家登录状态发生改变，同步到跨服
	struct GameHiddenOnlineStatusSync
	{
		GameHiddenOnlineStatusSync() : header(MT_GAME_HIDDEN_ONLINE_STATUS_SYNC) {}
		MessageHeader header;

		int role_id;
		int online_status;
	};

	struct HiddenGameOtherServerRoleOnlineStatusSync
	{
		HiddenGameOtherServerRoleOnlineStatusSync() : header(MT_HIDDEN_GAME_OTHER_SERVER_ROLE_ONLINE_STATUS_SYNC) {}
		MessageHeader header;

		int role_id;
		int online_status;
	};

	//---------------------------------工会-------------------------
	enum HIDDEN_GAME_GUILD_CHANGE_TYPE
	{
		HIDDEN_GAME_GUILD_CHANGE_TYPE_ADD_EXP,					//增加经验
		HIDDEN_GAME_GUILD_CHANGE_TYPE_SET_GUILD_HONOR_TITLE,	//设置工会荣耀战称号
		HIDDEN_GAME_GUILD_CHANGE_TYPE_WEEK_TASK_EVENT,			//周任务事件
		HIDDEN_GAME_GUILD_CHANGE_TYPE_SET_GUILD_FIGHT_TITLE,	//设置家族大乱斗称号
		HIDDEN_GAME_GUILD_CHANGE_TYPE_ADD_GONGXIAN,				//增加贡献
		HIDDEN_GAME_GUILD_CHANGE_TYPE_ROLE_GUILD_TASK_FINISH,		//玩家家族任务完成		
		HIDDEN_GAME_GUILD_CHANGE_TYPE_ROLE_GUILD_TASK_SOLICIT,		//玩家家族任务征集
		HIDDEN_GAME_GUILD_CHANGE_TYPE_ROLE_GUILD_TASK_CLEAR_SOLICIT,	//玩家家族任务删除征集
		HIDDEN_GAME_GUILD_CHANGE_TYPE_ROLE_GUILD_TASK_GIVE,		//玩家家族任务给与
		HIDDEN_GAME_GUILD_CHANGE_TYPE_ROLE_GUILD_TASK_FETCH,		//玩家家族任务领取征集
		HIDDEN_GAME_GUILD_CHANGE_TYPE_ROLE_GUILD_TASK_MAIL_NOTICE,		//玩家家族任务通知
	};

	struct HiddenGameChangeGuildInfo
	{
		HiddenGameChangeGuildInfo() : header(MT_HIDDEN_GAME_CHANGE_GUILD_INFO) {}
		MessageHeader header;

		int guild_id;
		int change_type;
		int param;
		int param2;
		int param3;
		int param4;
	};

	//---------------------------------------------------------------
	// 跨服运营活动相关
	struct GameCrossRandActivityInfoSync
	{
		GameCrossRandActivityInfoSync() : header(MT_GAME_CROSS_RA_INFO_SYNC), count(0) {}
		MessageHeader header;

		int plat_type;
		int server_id;
		int is_all_sync;

		int count;
		CrossRandActivityDataParam param_list[RAND_ACTIVITY_TYPE_MAX];
	};

	// 跨服接收且处理后下发给各个服务器
	struct CrossGameRandActivityInfoSyncRet
	{
		CrossGameRandActivityInfoSyncRet() : header(MT_CROSS_GAME_RA_INFO_RET), count(0) {}
		MessageHeader header;

		int count;
		CrossRandActivityDataParam param_list[RAND_ACTIVITY_TYPE_MAX];
	};

	// 跨服接收原服运营活动情况后发给隐藏服
	struct CrossHiddenRandActivityInfoSync
	{
		CrossHiddenRandActivityInfoSync() : header(MT_CROSS_HIDDEN_RA_INFO_SYNC) {}
		MessageHeader header;

		int plat_type;
		int server_id;
		int is_all_sync;

		int count;
		CrossRandActivityDataParam param_list[RAND_ACTIVITY_TYPE_MAX];
	};

	//---------聊天中物品信息-----------
	struct HiddenGameChatItemInfoReq
	{
		HiddenGameChatItemInfoReq() : header(MT_HIDDEN_GAME_CHAT_ITEM_INFO_REQ) {}
		MessageHeader header;

		int role_id;
		long long unique_item_seq;
	};

	struct GameHiddenChatItemInfoReq
	{
		GameHiddenChatItemInfoReq() : header(MT_GAME_HIDDEN_CHAT_ITEM_INFO_REQ) {}
		MessageHeader header;

		int role_id;
		long long unique_item_seq;
	};

	class GameHiddenChatItemSyn : public IMessageSerializer
	{
	public:
		GameHiddenChatItemSyn() : header(MT_GAME_HIDDEN_CHAT_ITEM_INFO_SYN) {}
		virtual ~GameHiddenChatItemSyn() {}
		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WRITE_BEGIN(header.msg_type);
			result = result && WRITE_LL(item_info_seq);
			result = result && WRITE_USHORT(item_id);
			result = result && WRITE_SHORT(num);
			result = result && WRITE_CHAR(is_bind);
			result = result && WRITE_CHAR(has_param);
			result = result && WRITE_SHORT(param_length);
			result = result && WRITE_UINT(invalid_time);
			if (0 != has_param)
			{
				result = result && WRITE_STRN(param_data, param_length);
			}

			result = result && WRITE_INT(has_attrs);
			if (has_attrs != 0)
			{
				for (int i = 0; i < ARRAY_LENGTH(attrs); ++i)
				{
					int value = attrs[i];
					result = result && WRITE_INT(value);
				}
			}

			return result;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_LL(item_info_seq)
				&& READ_USHORT(item_id)
				&& READ_SHORT(num)
				&& READ_CHAR(is_bind)
				&& READ_CHAR(has_param)
				&& READ_SHORT(param_length)
				&& READ_UINT(invalid_time);

			if (0 != has_param)
			{
				ret = READ_STRN(param_data, param_length, sizeof(NetValueItemParamData));
			}

			if (!ret) return false;

			ret = READ_INT(has_attrs);
			if (has_attrs != 0)
			{
				for (int i = 0; i < BATTLE_ATTR_MAX; ++i)
				{
					int &value = attrs[i];
					ret = READ_INT(value);
				}
			}

			return ret;
		}

		MessageHeader		header;
		long long	item_info_seq;				// 
		ItemID			item_id;		//!< 物品id
		short				num;			//!< 数量
		char				is_bind;		//!< 是否绑定
		char				has_param;		//!< 是否有净值参数
		short				param_length;	//!< 净值参数的数据长度 （当has_param为1时可读）
		UInt32				invalid_time;	//!< 过期时间
		NetValueItemParamData param_data;	//!< 净值参数的数据，根据param_length的值来读取 （当has_param为1时可读）

		int has_attrs;						//这个不为0就要读下面那个数组
		int attrs[BATTLE_ATTR_MAX];
	};

	struct GameHiddenForceGetGold
	{
		GameHiddenForceGetGold() : header(MT_GAME_HIDDEN_FORCE_GET_GOLD) {}
		MessageHeader header;

		int role_id;
		int server_id;		// 日志用
	};

	struct GameHiddenAddCredit
	{
		GameHiddenAddCredit() : header(MT_GAME_HIDDEN_TRADE_CREDIT_ADD_CREDIT_EVENTHANDLER) {}
		MessageHeader header;

		int role_id;
		int credit_num;
	};

	enum SYNC_RANK_TO_CROSS_STATUS				
	{
		SYNC_RANK_TO_CROSS_STATUS_INVALID = 0,	// 无效
		SYNC_RANK_TO_CROSS_STATUS_INSERT = 1,	// 添加		index 无效, rank_info为新数据
		SYNC_RANK_TO_CROSS_STATUS_UPDATA = 2,	// 更新		index 旧index , rank_info为新数据
		SYNC_RANK_TO_CROSS_STATUS_DELETE = 3,	// 删除		index 旧index , rank_info 无效
		SYNC_RANK_TO_CROSS_STATUS_MAX,
	};

	struct GameCrossSyncCrossPersonRankInfo		//由本服发送个人排行榜改变信息到跨服
	{
		GameCrossSyncCrossPersonRankInfo() : header(MT_GAME_HIDDEN_SYNC_CROSS_PERSON_RANK_INFO) {}
		MessageHeader	header;

		struct PersonRankSyncInfo
		{
			PersonRankSyncInfo() : sync_type(0) , index(-1) {}; 
			unsigned int sync_type;						// 同步状态 // 单人数据才有效 , 多人数据不看这个
			int index;									// 旧排名 , 插入时无效, 只对删除和更新有效
			PersonRankItem rank_info;
		};

		UniqueServerID from_usid;
		bool is_init;									// 初始的数据全覆盖
		char is_init_add;								// 0 : 覆盖  1 : 添加
		short reserve_sh;
		unsigned int last_change_time;
		int rank_type;
		int count;

		PersonRankSyncInfo item_list[CROSS_PERSON_RANK_SYNC_PAGE_ELEMENT_COUNT];
	};

	struct GameCrossSyncCrossPetRankInfo		//由本服发送个人排行榜改变信息到跨服
	{
		GameCrossSyncCrossPetRankInfo() : header(MT_GAME_HIDDEN_SYNC_CROSS_PET_RANK_INFO) {}
		MessageHeader	header;

		struct PetRankSyncInfo
		{
			PetRankSyncInfo() : sync_type(0), index(-1) {};

			unsigned int sync_type;						// 同步状态 // 单人数据才有效 , 多人数据不看这个
			int index;									// 旧排名 , 插入时无效, 只对删除和更新有效
			PetRankItem rank_info;
		};

		UniqueServerID from_usid;
		bool is_init;									// 初始的数据全覆盖
		char is_init_add;								// 0 : 覆盖  1 : 添加
		short reserve_sh;
		unsigned int last_change_time;
		int rank_type;
		int count;

		PetRankSyncInfo item_list[CROSS_PET_RANK_SYNC_PAGE_ELEMENT_COUNT];
	};

	struct CrossGameUserMsg
	{
		CrossGameUserMsg() : header(MT_CROSS_GAME_USER_MSG) {}
		MessageHeader		header;

		int					uid;
		// 后面紧跟 Protocol 协议内容
	};

	struct GameCrossUserMsg
	{
		GameCrossUserMsg() : header(MT_GAME_CROSS_USER_MSG) {}
		MessageHeader		header;

		long long			uuid;
		int					merge_server_id;
		// 后面紧跟 Protocol 协议内容
	};

	// -----------------以一敌百(个人版家族大乱斗)--------------
	struct HiddenGameRoleAddTitle
	{
		HiddenGameRoleAddTitle() : header(MT_HIDDEN_GAME_ADD_TITLE_TO_ROLE) {}
		MessageHeader header;

		int uid;
		int title_id;
		char reason[64];
	};

	// -------------------跨服通知游戏服玩家,隐藏服尚未连接-------------
	struct CrossGameHiddenNotConnected
	{
		CrossGameHiddenNotConnected() : header(MT_CROSS_GAME_HIDDEN_NOT_CONNECTED) {}
		MessageHeader header;

		int uid;
	};

	struct GameCrossSyncCrossQuery		//由本服发送请求玩家信息到跨服
	{
		GameCrossSyncCrossQuery() : header(MT_GAME_HIDDEN_QUERY_REQ) {}
		MessageHeader	header;

		int target_uid;
		UniqueUserID viewer_uid;
	};

	struct GameCrossSendAllUser		//发送给连接上跨服的所有游戏服(包括隐藏服)中的所有玩家
	{
		GameCrossSendAllUser() : header(MT_GAME_CROSS_SEND_ALL_USER) {}
		MessageHeader header;

		int origin_plat_type;
		int origin_server_id;
		int send_to_uuid;
		//后面紧跟着Protocol协议内容
	};

	struct GameHiddenSendChatGroup		//游戏服发送聊天组信息到跨服
	{
		GameHiddenSendChatGroup() : header(MT_GAME_HIDDEN_CHAT_GROUP) {}
		MessageHeader header;

		long long group_id;
		int main_server_id;
		//后面紧跟着Protocol协议内容
	};

	struct HiddenGameSendChatGroup		//跨服发送聊天组信息到游戏服
	{
		HiddenGameSendChatGroup() : header(MT_HIDDEN_GAME_CHAT_GROUP) {}
		MessageHeader header;

		long long group_id;
		int main_server_id;
		//后面紧跟着Protocol协议内容
	};

	struct HiddenGameNoticeLog			//隐藏服通知本地服打log(活动参与log,用于后台统计参与人数)
	{
		HiddenGameNoticeLog() : header(MT_HIDDEN_GAME_NOTICE_LOG) {}
		MessageHeader header;

		int uid;
		int activity_type;
		int param1;
		GameName role_name;
		PlatName plat_name;
		char reason_str[64];
	};

	enum CHAT_OPRETA_TYPE
	{
		CHAT_OPRETA_TYPE_DISSMISS_GROUP = 1,				// 解散聊天组
		CHAT_OPRETA_TYPE_DISSMISS_GROUP_RET,				// 解散返回

		CHAT_OPRETA_TYPE_TICKOUT_MEMBER,					// 踢出聊天组
		CHAT_OPRETA_TYPE_TICKOUT_MEMBER_RET,				// 踢出聊天组返回
		CHAT_OPRETA_TYPE_INVITE_MEMBER,						// 邀请进入
		CHAT_OPRETA_TYPE_ENTER_GROUP,						// 加入聊天组
		CHAT_OPRETA_TYPE_ENTER_GROUP_RET,					// 加入聊天组返回
		CHAT_OPRETA_TYPE_LOGIN_GROUP,						// 玩家上线
		CHAT_OPRETA_TYPE_LOGOUT_GROUP,						// 玩家下线
		CHAT_OPRETA_TYPE_CHANGE_NAME,						// 玩家改名
		CHAT_OPRETA_TYPE_CHANGE_NAME_RET,					// 玩家改名返回
		CHAT_OPRETA_TYPE_CHANGE_GROUP_NAME_OR_NOTICE,		// 群组改名或宣言
		CHAT_OPRETA_TYPE_CHANGE_PROF,						// 玩家修改职业
		CHAT_OPRETA_TYPE_CHANGE_AVA,						// 玩家修改头像

		CHAT_OPRETA_TYPE_LEAVEN_GROUP,						// 玩家退出群聊
		CHAT_OPRETA_TYPE_LEAVEN_GROUP_RET,					// 玩家退出群聊返回

		CHAT_OPRETA_TYPE_CREATE_GROUP,						// 玩家创建跨服群组
		CHAT_OPRETA_TYPE_CREATE_GROUP_RET,					// 玩家创建跨服群组返回
		CHAT_OPERATE_TYPE_LEADER_CHANGE_SYNC_GAME,			// 群主改变同步回原服
	};

	enum CHAT_OPRETA_RET_TYPE
	{
		CHAT_OPRETA_RET_TYPE_SUCC = 0,						// 成功返回
		CHAT_OPRETA_RET_TYPE_GROUP_INVALID = -1,			// 群组不存在
		CHAT_OPRETA_RET_TYPE_GROUP_FULL = -2,				// 群组满人
		CHAT_OPRETA_RET_TYPE_ALREADY_EXISTS = -3,			// 已经在群组了
		CHAT_OPRETA_RET_TYPE_ADD_FAIL = -4,					// 加入失败
	};

	struct GameCrossSyncChatOpreta		//由本服发送聊天组操作到隐藏服
	{
		GameCrossSyncChatOpreta() : header(MT_GAME_HIDDEN_CHAT_REQ) {}
		MessageHeader	header;

		int opreta_type;
		int uid;
		long long group_id;
		int is_cross;
		int inviter_id;
		int param1;
		int param2;
		int param3;
		int param4;
		int param5;
		GameName group_master;
		GameName group_name;
		GuildNotice group_notice;
	};

	struct CrossGameSyncChatOpreta		//由隐藏服发送聊天组操作到本服
	{
		CrossGameSyncChatOpreta() : header(MT_HIDDEN_GAME_CHAT_REQ) {}
		MessageHeader	header;

		int opreta_type;
		int uid;
		long long group_id;
		int is_cross;
		int inviter_id;
		int param1;
		int param2;
		int param3;
		int param4;
		int param5;
		GameName group_master;
		GameName group_name;
		GuildNotice group_notice;
	};

	struct GameHiddenTempGroupBecomeOfficialReq
	{
		GameHiddenTempGroupBecomeOfficialReq() : header(MT_GAME_HIDDEN_TEMP_GROUP_BECOME_OFFICIAL_REQ) {}
		MessageHeader header;

		struct MemberInfo
		{
			int uid;
			int avatar_type;
			int profession;
			int level;
			GameName name;
		};

		int plat_type;				//!< 用于返回
		int server_id;				//!< 用于返回
		GameName group_name;
		int member_num;
		MemberInfo member_list[5];	//!< 带头的是群主
	};

	struct HiddenGameTempGroupBecomeOfficialResp
	{
		HiddenGameTempGroupBecomeOfficialResp() : header(MT_HIDDEN_GAME_TEMP_GROUP_BECOME_OFFICIAL_RESP) {}
		MessageHeader header;

		int result;
		long long group_id;
		int member_uid_list[5]; // 带头的是群主
		GameName group_name;
		GameName creator_name;
	};

	struct GameCrossSyncRATianMing		//由本服发送聊天组操作到隐藏服
	{
		GameCrossSyncRATianMing() : header(MT_GAME_HIDDEN_RA_TIANMING) {}
		MessageHeader	header;

		UniqueServerID info_from_server_id;
		TianMingDivinationInfo info;
	};

	struct CrossGameSyncRATianMing		//由隐藏服发送聊天组操作到本服
	{
		CrossGameSyncRATianMing() : header(MT_HIDDEN_GAME_RA_TIANMING) {}
		MessageHeader	header;

		UniqueServerID info_from_server_id;
		TianMingDivinationInfo info;
	};

	//迷宫竞速
	enum GAME_HIDDEN_MAZE_RACER_REQ_TYPE
	{
		GAME_HIDDEN_MAZE_RACER_REQ_TYPE_RANK,  //排行榜
	};


	struct GameHiddenMazerAcerReq
	{
		GameHiddenMazerAcerReq() : header(MT_HIDDEN_GAME_MAZER_ACER_REQ) {}
		MessageHeader header;


		int origin_plat_type;
		int origin_server_id;
		int origin_role_id;
		int req_type;
	};

	//游戏服某些信息通知隐藏服
	enum GAME_HIDDEN_COMMON_INFO_NOTICE_TYPE
	{
		GAME_HIDDEN_COMMON_INFO_NOTICE_TYPE_ROLE_LOGIN,
		GAME_HIDDEN_COMMON_INFO_NOTICE_TYPE_ROLE_LOGOUT,
	};

	struct GameHiddenCommonInfoNoticeHidden
	{
		GameHiddenCommonInfoNoticeHidden() : header(MT_HIDDEN_GAME_COMMON_INFO_NOTICE_HIDDEN) {}
		MessageHeader header;

		int server_id;
		int is_all_sync;
		int type;
		int param1;
	};

	//隐藏服向游戏服请求首杀信息
	struct HiddenGameFirstKillInfoReq
	{
		HiddenGameFirstKillInfoReq() : header(MT_HIDDEN_GAME_FIRST_KILL_INFO_REQ) {}
		MessageHeader header;

		long long  uuid;
		int type;
		int level_group;
	};

	//游戏服向隐藏服请求首杀信息
	struct GameHiddenFirstKillInfoReq
	{
		GameHiddenFirstKillInfoReq() : header(MT_GAME_HIDDEN_FIRST_KILL_INFO_REQ) {}
		MessageHeader header;

		int uid;
		int type;
		int level_group;
	};

	// 跨服向原服请求简单的战报信息
	struct HiddenGameVideoSimpleInfoReq
	{
		HiddenGameVideoSimpleInfoReq() : header(MT_HIDDEN_GAME_VIDEO_SIMPLE_INFO_REQ) {}
		MessageHeader header;

		long long uuid;

		int monster_group_id;		//!< 怪物组ID
		int param1;					//!< 原样返回 
		int param2;					//!< 原样返回
		int param3;					//!< 原样返回
		int is_server_first_kill;	//!< 是否首杀
	};

	// 原服向隐藏服请求简单的战报信息
	struct GameHiddenVideoSimpleInfoReq
	{
		GameHiddenVideoSimpleInfoReq() : header(MT_GAME_HIDDEN_VIDEO_SIMPLE_INFO_REQ) {}
		MessageHeader header;

		int uid;

		int monster_group_id;		//!< 怪物组ID
		int param1;					//!< 原样返回 
		int param2;					//!< 原样返回
		int param3;					//!< 原样返回
		int is_server_first_kill;	//!< 是否首杀
	};

	enum VIDEO_DETAIL_INFO_REQ_TYPE
	{
		VIDEO_DETAIL_INFO_REQ_TYPE_WATCH = 0,		//观看录像   需要uuid
		VIDEO_DETAIL_INFO_REQ_TYPE_SYN = 1,			//同步本服录像到跨服
	};

	// 跨服向原服请求详细的战报信息
	struct HiddenGameVideoDetailInfoReq
	{
		HiddenGameVideoDetailInfoReq() : header(MT_HIDDEN_GAME_VIDEO_DETAIL_INFO_REQ){}
		MessageHeader header;

		int type;
		long long uuid;

		int monster_group_id;
		int is_server_first_kill;
	};

	struct GameHiddenVideoDetailInfoResp : public IMessageSerializer, public IMessageDeserializer
	{
		GameHiddenVideoDetailInfoResp() : header(MT_GAME_HIDDEN_VIDEO_DETAIL_INFO_RESP),
			uuid(0), video_length(0), cur_buffer_len(0)
		{
			memset(video_buffer, 0, sizeof(video_buffer));
			info_param.Reset();
		}

		virtual ~GameHiddenVideoDetailInfoResp()
		{
			cur_buffer_len = 0;
		}

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT(type)
				&& WRITE_LL(uuid)
				&& WRITE_INT(video_length)
				&& WRITE_SHORT(info_param.is_server_first_skill)
				&& WRITE_SHORT(info_param.version)
				&& WRITE_SHORT(info_param.battle_mode)
				&& WRITE_INT(info_param.monster_group_id)
				&& WRITE_INT(info_param.average_score)
				&& WRITE_UINT(info_param.record_timestamp)
				&& WRITE_LL(info_param.video_file_id);

			if (!ret) return false;

			ret = WRITE_STRN(video_buffer, video_length);
			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_INT(type)
				&& READ_LL(uuid)
				&& READ_INT(video_length)
				&& READ_SHORT(info_param.is_server_first_skill)
				&& READ_SHORT(info_param.version)
				&& READ_SHORT(info_param.battle_mode)
				&& READ_INT(info_param.monster_group_id)
				&& READ_INT(info_param.average_score)
				&& READ_UINT(info_param.record_timestamp)
				&& READ_LL(info_param.video_file_id);

			if (!ret) return false;

			ret = READ_STRN(video_buffer, video_length, MAX_VIDEO_BUFFER);

			return ret;
		}

		static const int MAX_VIDEO_BUFFER = 200000;

		MessageHeader header;

		int type;
		long long uuid;
		int video_length;
		char video_buffer[MAX_VIDEO_BUFFER];
		int cur_buffer_len;	// 当前的buffer长度，仅作记录

		BattleVideoParam info_param;
	};

	// 原服向隐藏服请求详细的战报信息
	struct GameHiddenVideoDetailInfoReq
	{
		GameHiddenVideoDetailInfoReq() : header(MT_GAME_HIDDEN_VIDEO_DETAIL_INFO_REQ) {}
		MessageHeader header;

		int uid;

		int monster_group_id;
		int is_server_first_kill;
	};

	// 隐藏服向原服返回详细的战报信息
	struct HiddenGameVideoDetailInfoResp : public IMessageSerializer, public IMessageDeserializer
	{
		HiddenGameVideoDetailInfoResp() : header(MT_HIDDEN_GAME_VIDEO_DETAIL_INFO_RESP),
			uid(0), video_length(0), cur_buffer_len(0), monster_group_id(0), is_server_first_kill(0)
		{
			memset(video_buffer, 0, sizeof(video_buffer));
		}

		virtual ~HiddenGameVideoDetailInfoResp()
		{
			cur_buffer_len = 0;
		}

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT(uid)
				&& WRITE_INT(video_length)
				&& WRITE_INT(monster_group_id)
				&& WRITE_INT(is_server_first_kill);

			if (!ret) return false;

			ret = WRITE_STRN(video_buffer, video_length);
			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_INT(uid)
				&& READ_INT(video_length)
				&& READ_INT(monster_group_id)
				&& READ_INT(is_server_first_kill);

			if (!ret) return false;

			ret = READ_STRN(video_buffer, video_length, MAX_VIDEO_BUFFER);

			return ret;
		}

		static const int MAX_VIDEO_BUFFER = 200000;

		MessageHeader header;

		int uid;
		int video_length;
		char video_buffer[MAX_VIDEO_BUFFER];
		int cur_buffer_len;	// 当前的buffer长度，仅作记录

		int monster_group_id;
		int is_server_first_kill;
	};

	//幻界战场
	struct GameHiddenHuanJieZhanChangeReq
	{
		GameHiddenHuanJieZhanChangeReq() : header(MT_HIDDEN_GAME_HUAN_JIE_ZHAN_CHANG_REQ) {}
		MessageHeader header;

		int origin_role_id;
		int req_type;
		int p1;
		int p2;
	};

	struct GameHiddenHuanJieZhanChangeReq2
	{
		GameHiddenHuanJieZhanChangeReq2() : header(MT_HIDDEN_GAME_HUAN_JIE_ZHAN_CHANG_REQ_2) {}
		MessageHeader header;
		
		int origin_role_id;			//请求的玩家
		int count;
		int role_id_list[100];			//要获取玩家排行信息的role_id列表
	};

	struct HiddenGameGuildEventhandlerSend
	{
		HiddenGameGuildEventhandlerSend() : header(MT_HIDDEN_GAME_GUILD_EVENTHANDLER_SEND) {}
		MessageHeader header;

		GuildID guild_id;
		GuildEventhandlerParam eventhandler_node;
	};

	struct GameCrossSyncCrossChatGroupInfo		//由本服发送聊天群组改变信息到跨服
	{
		GameCrossSyncCrossChatGroupInfo() : header(MT_GAME_HIDDEN_SERVER_LOGIN_CHATGROUP_DATA_SYN) {}
		MessageHeader	header;
		const static int MAX_ONE_SEND_LIST = ONCE_SEND_CHAT_GROUP_COUNT / 5;
		UniqueServerID from_usid;
		int count;

		ChatGroupInfo chat_group_list[MAX_ONE_SEND_LIST];
	};

	UNSTD_STATIC_CHECK(sizeof(GameCrossSyncCrossChatGroupInfo) <= 100000);//一条协议最大不超过10W

	struct CrossGameSyncCrossChatGroupInfo		//由跨服发送聊天群组改变信息到本服
	{
		CrossGameSyncCrossChatGroupInfo() : header(MT_HIDDEN_GAME_SERVER_LOGIN_CHATGROUP_DATA_SYN) {}
		MessageHeader	header;
		const static int MAX_ONE_SEND_LIST = ONCE_SEND_CHAT_GROUP_COUNT / 5;
		UniqueServerID from_usid;
		int count;

		ChatGroupInfo chat_group_list[MAX_ONE_SEND_LIST];
	};

	UNSTD_STATIC_CHECK(sizeof(CrossGameSyncCrossChatGroupInfo) <= 100000);//一条协议最大不超过10W

	struct HiddenGameSyncCrossChatGroupInfo		//由跨服返回聊天群组信息-单个
	{
		HiddenGameSyncCrossChatGroupInfo() : header(MT_HIDDEN_GAME_CHATGROUP_INFO_SYN) {}
		MessageHeader	header;

		int user_id;
		ChatGroupInfo chat_group_list;
	};

	struct HiddenGameSynOnlyFightInfo
	{
		HiddenGameSynOnlyFightInfo() : header(MT_HIDDEN_GAME_ONLY_FIGHT_SYN_INFO) {}
		MessageHeader header;
		
		OnlyFightDataParam param;
	};

	struct GameHiddenSynOnlyFightInfoReq
	{
		GameHiddenSynOnlyFightInfoReq() : header(MT_GAME_HIDDEN_ONLY_FIGHT_SYN_INFO_REQ) {}
		MessageHeader header;

		int server_id;
	};


	//----------------------擂台战-----------------------------
	enum GAME_HIDDEN_PLATFORM_BATTLE_REQ_TYPE
	{
		GAME_HIDDEN_PLATFORM_BATTLE_REQ_TYPE_ROLE_INFO,
		GAME_HIDDEN_PLATFORM_BATTLE_REQ_TYPE_REMOVE,
		GAME_HIDDEN_PLATFORM_BATTLE_REQ_TYPE_RQE_ROUTE_HIDDEN,
		GAME_HIDDEN_PLATFORM_BATTLE_REQ_TYPE_PLATFORM_INFO,
	};

	struct GameHiddenPlatformBattleReq  
	{
		GameHiddenPlatformBattleReq() : header(MT_GAME_HIDDEN_PLATFORM_BATTLE_REQ) {}
		MessageHeader	header;

		int type;
		int param1;
		int param2;
		short param3;
		short param4;
	};
	//---------------------------------------------------


	//----------------------------------锢魔之塔排行-----------------------------------------
	struct HiddenGameCourageChallengeRankReq				//跨服请求本服排行信息
	{
		HiddenGameCourageChallengeRankReq() : header(MT_HIDDEN_GAME_COURAGE_CHALLENGE_RANK_REQ) {}
		MessageHeader header;

		long long m_uuid;
		short rank_type;
		short count;
		int role_uid_list[MAX_COURAGE_CHALLENGE_RANK_NUM];	//好友与家族排行需要
	};

	struct HiddenGameCourageChallengeRankRoleChange			//跨服中玩家有关锢魔之塔排行信息改变,发回原服
	{
		HiddenGameCourageChallengeRankRoleChange() : header(MT_HIDDEN_GAME_COURAGE_CHALLENGE_ROLE_CHANGE) {}
		MessageHeader header;

		SynCourageChallengeRankInfo role_info;
	};

	static const int SYNC_LOG_BUFFER_SIZE = 4096;
	struct HiddenGameSyncLogToOriginServer
	{
		HiddenGameSyncLogToOriginServer() : header(MT_HIDDEN_GAME_SYNC_LOG_TO_ORIGIN_SERVER) {}
		MessageHeader header;

		int log_type;
		int length;
		char buffer[SYNC_LOG_BUFFER_SIZE];
	};
	
	struct HiddenGameBossChapterRankReq			// 隐藏服向本服请求家族试炼进度排行
	{
		HiddenGameBossChapterRankReq() : header(MT_HIDDEN_GAME_GUILD_BOSS_CHAPTER_RANK_REQ) {}
		MessageHeader header;

		long long uuid;
	};

	struct GameHiddenSyncPublicNotice : public IMessageSerializer, public IMessageDeserializer		// 原服向隐藏服
	{
		GameHiddenSyncPublicNotice() : header(MT_GAME_HIDDEN_PUBLIC_NOTICE_SYNC) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type) &&
				WRITE_INT(plat_type) &&
				WRITE_INT(server_id) &&
				WRITE_INT(count);
			if (!ret)
			{
				return false;
			}

			for (int i = 0; i < count && i < PublicNoticeList::MAX_PUBLICNOTICE_NUM; ++i)
			{
				ret = WRITE_INT(notice_list[i].notice_id) &&
					WRITE_INT(notice_list[i].type) &&
					WRITE_INT(notice_list[i].color) &&
					WRITE_INT(notice_list[i].urgent) &&
					WRITE_LL(notice_list[i].begin_time) &&
					WRITE_LL(notice_list[i].end_time) &&
					WRITE_LL(notice_list[i].send_interval) &&
					WRITE_INT(notice_list[i].creator.length()) &&
					WRITE_INT(notice_list[i].notice.length());

				if (!ret) return false;

				ret = WRITE_STRN(notice_list[i].creator.c_str(), notice_list[i].creator.length());
				if (!ret) return false;

				ret = WRITE_STRN(notice_list[i].notice.c_str(), notice_list[i].notice.length());
				if (!ret) return false;
			}

			return true;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			bool ret = READ_BEGIN(header.msg_type) &&
				READ_INT(plat_type) &&
				READ_INT(server_id) &&
				READ_INT(count);
			if (!ret)
			{
				return false;
			}

			for (int i = 0; i < count && i < PublicNoticeList::MAX_PUBLICNOTICE_NUM; ++i)
			{
				int creator_len = 0;
				int notice_len = 0;
				ret = READ_INT(notice_list[i].notice_id) &&
					READ_INT(notice_list[i].type) &&
					READ_INT(notice_list[i].color) &&
					READ_INT(notice_list[i].urgent) &&
					READ_LL(notice_list[i].begin_time) &&
					READ_LL(notice_list[i].end_time) &&
					READ_LL(notice_list[i].send_interval) &&
					READ_INT(creator_len) &&
					READ_INT(notice_len);

				if (!ret) return false;

				ret = READ_STRING(notice_list[i].creator, creator_len);
				if (!ret) return false;

				ret = READ_STRING(notice_list[i].notice, notice_len);
				if (!ret) return false;
			}

			return true;
		}

		int plat_type;
		int server_id;

		int count;
		PublicNoticeList::NoticeAttr notice_list[PublicNoticeList::MAX_PUBLICNOTICE_NUM];
	};

	struct GameHiddenSyncOnlineInfo
	{
		GameHiddenSyncOnlineInfo() : header(MT_GAME_HIDDEN_ONLINE_INFO_SYNC) {}
		MessageHeader header;

		int plat_type;
		int server_id;

		int online_role_num;
		int incross_role_num;
		int crossing_role_num;
	};

	struct HiddenGameInviteJoinTeam
	{
		HiddenGameInviteJoinTeam() : header(MT_HIDDEN_GAME_INVITE_JOIN_TEAM) {}
		MessageHeader header;

		int cross_team_index;		
		int team_type;
		short team_level_limit_low;
		short team_level_limit_high;
		int team_member_num;
		MsgRoleBaseInfo inviter_info;
		TeamItem team_item;

		int target_uid;		
		int invite_type;
	};

	struct GameHiddenInviteJoinTeamInOrigin
	{
		GameHiddenInviteJoinTeamInOrigin() : header(MT_GAME_HIDDEN_INVITE_JOIN_TEAM_IN_ORIGIN) {}
		MessageHeader header;

		int team_index;
		int team_type;
		short team_level_limit_low;
		short team_level_limit_high;
		int team_member_num;
		MsgRoleBaseInfo inviter_info;
		TeamItem team_item;

		int target_uid;
		int invite_type;
	};

	struct GameHiddenReqJoinTeamInfo
	{
		GameHiddenReqJoinTeamInfo() : header(MT_GAME_HIDDEN_REQ_JOIN_TEAM_INFO) {}
		MessageHeader header;

		int cross_team_index;
		int inviter_uid;
		int target_uid;		
	};

	struct HiddenGameJoinTeamInfoRet
	{
		HiddenGameJoinTeamInfoRet() : header(MT_HIDDEN_GAME_JOIN_TEAM_INFO_RET) {}
		MessageHeader header;

		int is_exist;
		int cross_team_index;
		int team_type;
		int team_member_num;
		MsgRoleBaseInfo inviter_info;
		TeamItem team_item;
		int target_uid;
	};

	struct GameHiddenInvationJoinTeamRet
	{
		GameHiddenInvationJoinTeamRet() : header(MT_GAME_HIDDEN_INVITE_JOIN_TEAM_RET) {}
		MessageHeader header;
		
		int result;
		int notice_num;
		int inviter_uid;
	};

	struct HiddenGameInviteJoinTeamInOriginRet
	{
		HiddenGameInviteJoinTeamInOriginRet() : header(MT_HIDDEN_GAME_INVITE_JOIN_TEAM_IN_ORIGIN_RET) {}
		MessageHeader header;

		int result;
		int notice_num;
		int inviter_uid;
	};

	struct HiddenGameGuildGatherDreamPublishListReq
	{
		HiddenGameGuildGatherDreamPublishListReq() : header(MT_HIDDEN_GAME_GUILD_GATHER_DREAM_PUBLISH_LIST_REQ) {}
		MessageHeader header;

		int uid;
		int guild_id;
	};

	struct HiddenGameGuildGatherDreamRecordListReq
	{
		HiddenGameGuildGatherDreamRecordListReq() : header(MT_HIDDEN_GAME_GUILD_GATHER_DREAM_RECORD_LIST_REQ) {}
		MessageHeader header;

		int uid;
		int type;
	};

	struct HiddenGameGuildGatherDreamPublishReq
	{
		HiddenGameGuildGatherDreamPublishReq() : header(MT_HIDDEN_GAME_GUILD_GATHER_DREAM_PUBLISH_REQ) {}
		MessageHeader header;

		int uid;
		int guild_id;
		int is_publish_send;		//0:不发送 1:发送
		GuildGatherDreamInfo info;
	};

	struct GameHiddenGuildGatherDreamPublishRet
	{
		GameHiddenGuildGatherDreamPublishRet() : header(MT_GAME_HIDDEN_GUILD_GATHER_DREAM_PUBLISH_RET) {}
		MessageHeader header;

		enum RESULT_TYPE
		{
			RESULT_TYPE_SUCC = 0,			//发布成功
			RESULT_TYPE_CONFIG_ERROR,		//获取配置失败
			RESULT_TYPE_NOT_PRAY,			//该宠物不能集愿
		};

		int is_publish_send;		//0:不发送 1:发送
		int result;		
		int guild_id;
		int uid;
		int item_id;				//发布成功时使用			
	};
	
	struct HiddenGameGuildGatherDreamGiftReq
	{
		HiddenGameGuildGatherDreamGiftReq() : header(MT_HIDDEN_GAME_GUILD_GATHER_DREAM_GIFT_REQ) {}
		MessageHeader header;

		int gift_role_uid;
		int get_role_uid;			//获赠玩家的uid
		int guild_id;
	};

	struct GameHiddenGuildGatherDreamGiftRet
	{
		GameHiddenGuildGatherDreamGiftRet() : header(MT_GAME_HIDDEN_GUILD_GATHER_DREAM_GIFT_RET) {}
		MessageHeader header;

		enum RESULT_TYPE
		{
			RESULT_TYPE_SUCC = 0,			//判断顺利通过
			RESULT_TYPE_NOT_PUBLISH,		//玩家没有发布
			RESULT_TYPE_IS_FINISH,			//玩家已集齐
			RESULT_TYPE_CONFIG_ERROR,		//获取配置失败
			RESULT_TYPE_ERR_DIF_GUILD,		//不在同一家族
		};

		int result;
		SynGuildGatherDreamGiftInfo info;
	};

	struct HiddenGameGuildGatherDreamGiftAddRecord
	{
		HiddenGameGuildGatherDreamGiftAddRecord() : header(MT_HIDDEN_GAME_GUILD_GATHER_DREAM_GIFT_ADD_RECORD) {}
		MessageHeader header;

		SynGuildGatherDreamGiftInfo info;
	};
	
	struct HiddenGameGuildGatherDreamFetchReq
	{
		HiddenGameGuildGatherDreamFetchReq() : header(MT_HIDDEN_GAME_GUILD_GATHER_DREAM_FETCH_REQ) {}
		MessageHeader header;

		int uid;
		int guild_id;
	};

	struct GameHiddenGuildGatherDreamFetchRet
	{
		GameHiddenGuildGatherDreamFetchRet() : header(MT_GAME_HIDDEN_GUILD_GATHER_DREAM_FETCH_RET) {}
		MessageHeader header;

		enum RESULT_TYPE
		{
			RESULT_TYPE_SUCC = 0,				// 顺利通过判断
			RESULT_TYPE_NOT_PUBLISH,			// 玩家没有发布
			RESULT_TYPE_NOT_HAS_FETCH,			// 没有多余可领
		};

		int result;
		int uid;
		int index;
		int guild_id;
		ItemID item_id;
		short fetch_count;
	};

	struct HiddenGameGuildGatherDreamFetchChange
	{
		HiddenGameGuildGatherDreamFetchChange() : header(MT_HIDDEN_GAME_GUILD_GATHER_DREAM_FETCH_CHANGE) {}
		MessageHeader header;

		int uid;
		int index;
		int guild_id;
		short fetch_count;
		short is_succ;
	};

	//勇闯地宫
	struct GameHiddenHBraveGroundReq
	{
		GameHiddenHBraveGroundReq() : header(MT_HIDDEN_GAME_BRAVE_GROUND_REQ) {}
		MessageHeader header;

		int origin_role_id;
		int req_type;
		int p1;
		int p2;
	};

	struct HiddenGameSyncHuanJieActivityStatus
	{
		HiddenGameSyncHuanJieActivityStatus() : header(MT_HIDDEN_GAME_SYNC_HUAN_JIE_ACTIVITY_STATUS) {}
		MessageHeader	header;

		int season_num;
	};

	struct GameHiddenSendSystemMail
	{
		GameHiddenSendSystemMail() : header(MT_GAME_HIDDEN_SYSTEM_MAIL_SYNC) {}
		MessageHeader header;

		int plat_type;
		int server_id;

		int send_to_uid;
		MailParam mail_param;
	};

	struct HiddenGameSendSystemMail
	{
		HiddenGameSendSystemMail() : header(MT_HIDDEN_GAME_SYSTEM_MAIL_SYNC) {}
		MessageHeader header;

		int send_to_uid;
		MailParam mail_param;
	};

	struct HiddenGameSyncHuanJieRoleInfo
	{
		HiddenGameSyncHuanJieRoleInfo() : header(MT_HIDDEN_GAME_SYNC_HUAN_JIE_ROLE_INFO) {}
		MessageHeader	header;

		int role_uid;
		int rank_score;
		int is_finish_ding_ji;
	};

	struct HiddenGameGuildGatherDreamRoleChange
	{
		HiddenGameGuildGatherDreamRoleChange() : header(MT_HIDDEN_GAME_GUILD_GATHER_DREAM_ROLE_INFO_CHANGE) {}
		MessageHeader	header;

		int role_uid;
		int guild_id;
		SynGatherDreamRecord role_info;
	};

	struct HiddenGameHuanJieSyncTianTiRecord
	{
		HiddenGameHuanJieSyncTianTiRecord() : header(MT_HIDDEN_GAME_HUAN_JIE_SYNC_TIAN_TI_RECORD) {}
		MessageHeader	header;

		int role_id;
		int record_id;
		CommonSaveHuanJieTianTiReceordData record_data;
	};

	struct GameHiddenSyncSingleRecordAck
	{
		GameHiddenSyncSingleRecordAck() : header(MT_GAME_HIDDEN_SYNC_TIAN_TI_RECORD_ACK) {}
		MessageHeader	header;

		int role_id;
		int record_id;
	};

	struct HiddenGameGuildChatSync
	{
		HiddenGameGuildChatSync() : header(MT_HIDDEN_GAME_GUILD_CHAT) {}
		MessageHeader header;

		int guild_id;
		int chat_msg_len;

		char chat_msg[4096];
	};

	struct GameHiddenGuildChatSync
	{
		GameHiddenGuildChatSync() : header(MT_GAME_HIDDEN_GUILD_CHAT) {}
		MessageHeader header;

		int guild_id;
		int chat_msg_len;

		char chat_msg[4096];
	};

	// 玩家基本信息在跨服发生改变，同步到游戏服
	struct HiddenGameUserCacheSync
	{
		HiddenGameUserCacheSync() : header(MT_HIDDEN_GAME_USERCACHE_CHANGE_SYNC) {}
		MessageHeader header;

		int online_status;
		UserCacheParam usercache_param;
	};

	struct GameHiddenSensitiveWordsReload
	{
		GameHiddenSensitiveWordsReload() : header(MT_GAME_HIDDEN_SENSITIVE_WORDS_RELOAD) {}
		MessageHeader	header;

		int plat_type;
		int server_id;
	};

	struct GameHiddenCmdKickRole
	{
		GameHiddenCmdKickRole() : header(MT_GAME_HIDDEN_CMD_KICK_ROLE) {}
		MessageHeader header;

		int plat_type;
		int server_id;
		int role_id;
	};

	struct GameHiddenCmdMuteRole
	{
		GameHiddenCmdMuteRole() : header(MT_GAME_HIDDEN_CMD_MUTE_ROLE) {}
		MessageHeader header;

		int plat_type;
		int server_id;
		int role_id;
		unsigned int mute_second;
		int forbid_talk_type;
	};

	struct HiddenGameCmdMuteRole
	{
		HiddenGameCmdMuteRole() : header(MT_HIDDEN_GAME_CMD_MUTE_ROLE) {}
		MessageHeader header;

		int role_id;
		unsigned int mute_second;
		int forbid_talk_type;
	};

	// [个人排行榜:新增] --------------------------  

	enum HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE
	{
		HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_INVALID = 0,
		HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_PROF = 1 ,					//职业变更   // param_1 : old_prof  , param_2 : curr_prof
		HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_NAME = 2,					//名字变更   // rank_item.role_info.user_name : 新名字
		HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_LEVEL = 3,					//等级变更   //  rank_item.rank_value : 对应类型的等级 
		HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_CAPABILITY = 4,			//战斗力	 //  rank_item.rank_value : 对应类型的战斗力
		HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_HUA_SHEN = 5,				//化身战斗力 //  rank_item.rank_value : 化身战斗力
		HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_DREAM_NOTES = 6,			//梦渊笔录	// rank_item.rank_value : 层数
		HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_LUNHUIWANGCHUAN = 7,		//轮回忘川	// rank_item.rank_value : 层数
		HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_CHIVALROUS = 8,			//侠义值	// rank_item.rank_value : 本周累计获得侠义值
		HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_NETHER_REWARD = 9,				//幽冥炼狱	// rank_item.rank_value : 获得宝箱数
		HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_KILL_BOSS = 10,			//北斗七星	// rank_item.rank_value : 击败回合数
		HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_ENDLESS_TOWER = 11,				//无尽之塔 // rank_item.rank_value  : 无尽之塔通关seq
		HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_FALLEN_GOD = 12,					//殒神之地	// rank_item.rank_value : 殒神sn
		HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_MEILI_QIXI = 13,					//魅力七夕 // rank_item_rank_value : 魅力值
		HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_NEW_COURAGE_CHALLENGE = 14,		//弑神之塔 // rank_item_rank_value : 总星星 
		HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_MAX,
	};

	struct HiddenGameRoleInfoChange
	{
		HiddenGameRoleInfoChange() : header(MT_HIDDEN_GAME_ROLE_INFO_CHANGE) {}
		MessageHeader header;

		void Reset()
		{
			origin_plat_type = -1;
			origin_server_id = -1;
			sync_info_type = HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_INVALID;
			param_1 = 0;
			param_2 = 0;
			rank_item.Reset();
		}

		int origin_plat_type;
		int origin_server_id;

		int sync_info_type;
		int param_1;
		int param_2;

		PersonRankItem rank_item;
	};

	// [个人排行榜:新增] --------------------------  

	enum HIDDEN_GAME_PET_INFO_CHANGE_TYPE
	{
		HIDDEN_GAME_PET_INFO_CHANGE_TYPE_CHANGE_INVALID = 0,
		HIDDEN_GAME_PET_INFO_CHANGE_TYPE_CHANGE_ROLE_INFO = 1,				//角色基础信息变更  
		HIDDEN_GAME_PET_INFO_CHANGE_TYPE_CHANGE_INDEX = 2,					//宠物Index变更		// 已作废 
		HIDDEN_GAME_PET_INFO_CHANGE_TYPE_CHANGE_REMOVE = 3,					//宠物被移除		// param_1 : pet_unique_id高32位  param_2 : pet_unique_id低32位 
		HIDDEN_GAME_PET_INFO_CHANGE_TYPE_CHANGE_CAPABILITY = 4,				//战斗力			//  rank_item.rank_value : 宠物战斗力
		HIDDEN_GAME_PET_INFO_CHANGE_TYPE_CHANGE_HUA_SHEN = 5,				//化身战斗力		//  rank_item.rank_value : 化身战斗力
		HIDDEN_GAME_PET_INFO_CHANGE_TYPE_CHANGE_MAX,
	};

	struct HiddenGamePetInfoChange
	{
		HiddenGamePetInfoChange() : header(MT_HIDDEN_GAME_PET_INFO_CHANGE) {}
		MessageHeader header;

		void Reset()
		{
			origin_plat_type = -1;
			origin_server_id = -1;
			sync_info_type = HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_INVALID;
			param_1 = 0;
			param_2 = 0;
			rank_item.Reset();
		}

		int origin_plat_type;
		int origin_server_id;

		int sync_info_type;
		int param_1;
		int param_2;

		PetRankItem rank_item;
	};

	struct GameHiddenCmdNoticeNewMessage
	{
		GameHiddenCmdNoticeNewMessage() : header(MT_GAME_HIDDEN_CMD_NOTICE_NEW_MESSAGE) {}
		MessageHeader header;

		int plat_type;
		int server_id;
		int role_id;
		int msg_count;
	};

	struct HiddenGameCmdNoticeNewMessage
	{
		HiddenGameCmdNoticeNewMessage() : header(MT_HIDDEN_GAME_CMD_NOTICE_NEW_MESSAGE){}
		MessageHeader header;

		int role_id;
		int msg_count;
	};

	struct GameHiddenConsumeCarouselAddNewRecord
	{
		GameHiddenConsumeCarouselAddNewRecord() : header(MT_GAME_HIDDEN_CONSUME_CAROUSEL_ADD_RECORD) {}
		MessageHeader header;

		int plat_type;
		int server_id;
		unsigned short item_id;
		short item_num;
		GameName role_name;
	};

	struct HiddenGameConsumeCarouselAddNewRecord
	{
		HiddenGameConsumeCarouselAddNewRecord() : header(MT_HIDDEN_GAME_CONSUME_CAROUSEL_ADD_RECORD) {}
		MessageHeader header;

		unsigned short item_id;
		short item_num;
		GameName role_name;
	};

	struct GameHiddenCmdResetPublicNotice
	{
		GameHiddenCmdResetPublicNotice() : header(MT_GAME_HIDDEN_RESET_PUBLIC_NOTICE) {}
		MessageHeader header;

		int plat_type;
		int server_id;
	};

	struct HiddenGameSyncPlatInCrossRoleNum
	{
		HiddenGameSyncPlatInCrossRoleNum() : header(MT_HIDDEN_GAME_SYNC_PLAT_INCROSS_ROLE_NUM) {}
		MessageHeader header;

		int sub_plat_type;
		int incross_role_num;
	};

	struct CrossGameSyncWorldBossInfo		//由隐藏服发送世界boss到本服
	{
		CrossGameSyncWorldBossInfo() : header(MT_HIDDEN_GAME_SYNC_WORLD_BOSS_INFO) {}
		MessageHeader	header;

		int m_boss_npc_id;
		int m_boss_npc_seq;
		int m_send_mail_boss_npc_id;

		int m_boss_scene_id;
		int m_boss_scene_key;
		Posi m_boss_pos;

		int m_boss_cur_hp;
		int m_boss_max_hp;

		int m_open_day;	
		char m_is_kill;
		char is_send_boss_pos;
		short reserve_sh;
	};

	struct CrossGameSyncWorldBoss2Info		//由隐藏服发送世界boss2到本服
	{
		CrossGameSyncWorldBoss2Info() : header(MT_HIDDEN_GAME_SYNC_WORLD_BOSS2_INFO) {}
		MessageHeader	header;

		int m_boss_npc_id;
		int m_boss_npc_seq;
		int m_send_mail_boss_npc_id;

		int m_boss_scene_id;
		int m_boss_scene_key;
		Posi m_boss_pos;

		int m_boss_cur_hp;
		int m_boss_max_hp;

		int m_open_day;
		char m_is_kill;
		char is_send_boss_pos;
		short reserve_sh;
	};

	struct CrossGameSyncWorldBoss3Info		//由隐藏服发送世界boss3到本服
	{
		CrossGameSyncWorldBoss3Info() : header(MT_HIDDEN_GAME_SYNC_WORLD_BOSS3_INFO) {}
		MessageHeader	header;

		int m_boss_npc_id;
		int m_boss_npc_seq;
		int m_send_mail_boss_npc_id;

		int m_boss_scene_id;
		int m_boss_scene_key;
		Posi m_boss_pos;

		int m_boss_cur_hp;
		int m_boss_max_hp;

		int m_open_day;
		char m_is_kill;
		char is_send_boss_pos;
		short reserve_sh;
	};

	struct HiddenGameWorldBossSyncRankResultMsg
	{
		HiddenGameWorldBossSyncRankResultMsg() : header(MT_HIDDEN_GAME_WORLD_BOSS_SYNC_RANK_RESULT_MSG) {}
		MessageHeader header;

		int length;
		char msg_content[8192];
	};

	struct HiddenGameWorldBoss2SyncRankResultMsg
	{
		HiddenGameWorldBoss2SyncRankResultMsg() : header(MT_HIDDEN_GAME_WORLD_BOSS_2_SYNC_RANK_RESULT_MSG) {}
		MessageHeader header;

		int length;
		char msg_content[8192];
	};

	struct HiddenGameWorldBoss3SyncRankResultMsg
	{
		HiddenGameWorldBoss3SyncRankResultMsg() : header(MT_HIDDEN_GAME_WORLD_BOSS_3_SYNC_RANK_RESULT_MSG) {}
		MessageHeader header;

		int length;
		char msg_content[8192];
	};

	struct HiddenGameWorldBossSyncParticipateUidList
	{
		HiddenGameWorldBossSyncParticipateUidList() : header(MT_HIDDEN_GAME_WORLD_BOSS_SYNC_PARTICIPATE_UID_LIST) {}
		MessageHeader header;

		short is_start;
		short uid_num;
		int uid_list[100];
	};

	struct HiddenGameWorldBoss2SyncParticipateUidList
	{
		HiddenGameWorldBoss2SyncParticipateUidList() : header(MT_HIDDEN_GAME_WORLD_BOSS_2_SYNC_PARTICIPATE_UID_LIST) {}
		MessageHeader header;

		short is_start;
		short uid_num;
		int uid_list[100];
	};

	struct HiddenGameWorldBoss3SyncParticipateUidList
	{
		HiddenGameWorldBoss3SyncParticipateUidList() : header(MT_HIDDEN_GAME_WORLD_BOSS_3_SYNC_PARTICIPATE_UID_LIST) {}
		MessageHeader header;

		short is_start;
		short uid_num;
		int uid_list[100];
	};

	struct HiddenGameQuanMinActivityInfoReq
	{
		HiddenGameQuanMinActivityInfoReq() : header(MT_HIDDEN_GAME_REQEST_QUANMIN_ACTIVITY_INFO) {}
		MessageHeader header;

		int uid;
	};

	struct GameHiddenQuanMinActivityInfoResp
	{
		GameHiddenQuanMinActivityInfoResp() : header(MT_GAME_HIDDEN_RESPONSE_QUANMIN_ACTIVITY_INFO) {}
		MessageHeader header;

		int plat_type;
		int server_id;

		int uid;
		int count;
		double cur_discount[QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM];
		int buy_count[QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM];
	};

	struct GameHiddenWildBossInfoReq
	{
		GameHiddenWildBossInfoReq() : header(MT_GAME_HIDDEN_WILD_BOSS_INFO_REQ) {}
		MessageHeader header;

		int uid;
		int req_type;
		int param;
		int param1;
	};

	struct GameHiddenShanHaiBossInfoReq
	{
		GameHiddenShanHaiBossInfoReq() : header(MT_GAME_HIDDEN_SHAN_HAI_BOSS_INFO_REQ) {}
		MessageHeader header;

		int uid;
		int req_type;
		int param;
		int param1;
	};

	struct HiddenGameSyncBraveGroundInfo
	{
		HiddenGameSyncBraveGroundInfo() : header(MT_HIDDEN_GAME_SYNC_BRAVE_GROUND_INFO) {}
		MessageHeader header;
		
		int is_notice;
		int top_role_id[BRAVE_GROUND_TOP_ITEM_NUM][BRAVE_GROUND_TOP_ROLE_NUM];
	};

	struct GameHiddenFirstKillReq : public IMessageSerializer, public IMessageDeserializer
	{
		GameHiddenFirstKillReq() : header(MT_GAME_HIDDEN_FIRST_KILL_REQ), video_length(0) 
		{
			memset(video_buffer, 0, sizeof(video_buffer));
			info.Reset();
		}

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
					&& WRITE_INT(video_length)
					&& WRITE_SHORT(info_param.is_server_first_skill)
					&& WRITE_SHORT(info_param.version)
					&& WRITE_SHORT(info_param.battle_mode)
					&& WRITE_INT(info_param.monster_group_id)
					&& WRITE_INT(info_param.average_score)
					&& WRITE_UINT(info_param.record_timestamp)
					&& WRITE_LL(info_param.video_file_id);
			if (!ret) return false;

			ret = WRITE_STRN(video_buffer, video_length);
			if (!ret) return false;

			ret = WRITE_INT(info.id)
			   && WRITE_LL(info.first_kill_info.battle_video_record_file_id)
			   && WRITE_INT(info.first_kill_info.server_id)
			   && WRITE_SHORT(info.first_kill_info.type)
			   && WRITE_SHORT(info.first_kill_info.level_group)
			   && WRITE_SHORT(info.first_kill_info.seq)
   		  	   && WRITE_SHORT(info.first_kill_info.member_count)
			   && WRITE_SHORT(info.first_kill_info.round_num)
			   && WRITE_SHORT(info.first_kill_info.kill_flag)
		       && WRITE_INT(info.first_kill_info.monster_group_id)
			   && WRITE_LL(info.first_kill_info.kill_timestamp)
			   && WRITE_LL(info.first_kill_info.kill_game_timestamp)
			   && WRITE_INT(info.first_kill_info.reward_data.coin)
			   && WRITE_USHORT(info.first_kill_info.reward_data.data.item_id)
			   && WRITE_SHORT(info.first_kill_info.reward_data.data.is_bind)
			   && WRITE_INT(info.first_kill_info.reward_data.data.item_num);
			if (!ret) return false;

			for (int i = 0; i < info.first_kill_info.member_count && i < MEMBER_NAME_MAX_NUM; i++)
			{
				ret = WRITE_INT(info.first_kill_info.uid[i])
				   && WRITE_STRN(info.first_kill_info.member_name[i], sizeof(info.first_kill_info.member_name[i]));

				if (!ret) return false;
			}

			return ret;
		}
		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			bool ret = READ_BEGIN(header.msg_type)
					&& READ_INT(video_length)
					&& READ_SHORT(info_param.is_server_first_skill)
					&& READ_SHORT(info_param.version)
					&& READ_SHORT(info_param.battle_mode)
					&& READ_INT(info_param.monster_group_id)
					&& READ_INT(info_param.average_score)
					&& READ_UINT(info_param.record_timestamp)
					&& READ_LL(info_param.video_file_id);
			if (!ret) return false;

			ret = READ_STRN(video_buffer, video_length, MAX_VIDEO_BUFFER);
			if (!ret) return false;

			ret = READ_INT(info.id)
				&& READ_LL(info.first_kill_info.battle_video_record_file_id)
				&& READ_INT(info.first_kill_info.server_id)
				&& READ_SHORT(info.first_kill_info.type)
				&& READ_SHORT(info.first_kill_info.level_group)
				&& READ_SHORT(info.first_kill_info.seq)
				&& READ_SHORT(info.first_kill_info.member_count)
				&& READ_SHORT(info.first_kill_info.round_num)
				&& READ_SHORT(info.first_kill_info.kill_flag)
				&& READ_INT(info.first_kill_info.monster_group_id)
				&& READ_LL(info.first_kill_info.kill_timestamp)
				&& READ_LL(info.first_kill_info.kill_game_timestamp)
				&& READ_INT(info.first_kill_info.reward_data.coin)
				&& READ_USHORT(info.first_kill_info.reward_data.data.item_id)
				&& READ_SHORT(info.first_kill_info.reward_data.data.is_bind)
				&& READ_INT(info.first_kill_info.reward_data.data.item_num);
			if (!ret) return false;

			for (int i = 0; i < info.first_kill_info.member_count && i < MEMBER_NAME_MAX_NUM; i++)
			{
				ret = READ_INT(info.first_kill_info.uid[i])
					&& READ_STRN(info.first_kill_info.member_name[i], sizeof(info.first_kill_info.member_name[i]), sizeof(GameName));

				if (!ret) return false;
				UNSTD_STATIC_CHECK(sizeof(info.first_kill_info.member_name[i]) <= sizeof(GameName));
			}

			return ret;
		}

		static const int MAX_VIDEO_BUFFER = 200000;
		MessageHeader header;

		int video_length;
		char video_buffer[MAX_VIDEO_BUFFER];

		BattleVideoParam info_param;
		ServerFirstKillParam::ServerFirstKillSaveEntry info;
	};

	struct HiddenGameRoleReturnOriginServerReq
	{
		HiddenGameRoleReturnOriginServerReq() : header(MT_HIDDEN_GAME_ROLE_RETURN_ORIGIN_SERVER_REQ) {}
		MessageHeader header;

		int role_id;
		ReturnOriginData_Business business_data;
	};

	struct GameHiddenRoleReturnOriginServerAck
	{
		GameHiddenRoleReturnOriginServerAck() : header(MT_GAME_HIDDEN_ROLE_RETURN_ORIGIN_SERVER_ACK) {}
		MessageHeader header;

		int result;
		int role_id;
		ReturnOriginData_Business business_data;
	};

	struct GameHiddenTeammateJoinOriginServerTeam
	{
		GameHiddenTeammateJoinOriginServerTeam() : header(MT_GAME_HIDDEN_TEAMMATE_JOIN_ORIGIN_SERVER_TEAM)
		{
			business_data.Reset();
		}
		MessageHeader header;

		int team_index;
		ReturnOriginData_Business business_data;
	};

	struct HiddenGameHuanJieZhanChangeSyncInfo
	{
		HiddenGameHuanJieZhanChangeSyncInfo() : header(MT_HIDDEN_GAME_SYNC_HUAN_JIE_INFO) {}
		MessageHeader header;

		int is_notice;
		int role_id[HUAN_JIE_RANK_TITLE_MAX_NUM];
	};

	struct GameCrossQueryRankInfoReq	 //游戏服或隐藏服向跨服请求查询玩家排行信息
	{
		GameCrossQueryRankInfoReq() : header(MT_GAME_CROSS_QUERY_RANK_INFO_REQ) {}
		MessageHeader header;

		UniqueUserID unique_viewer_uid;
		UniqueUserID unique_user_id;
		short prof;
		short is_ret_original_place;		//协议转发回的时候是否原路返回 1:原路返回
	};

	struct GameHiddenTradeMarketRequestBuy //!< 原服请求跨服购买
	{
		GameHiddenTradeMarketRequestBuy() : header(MT_GAME_HIDDEN_TRADE_MARKET_REQUEST_BUY) {}
		MessageHeader header;

		int plat_type;						//!< 平台
		int server_id;						//!< 服务器ID
		int role_id;						//!< 买家uid

		int seller_uid;						//!< 卖家uid
		short sale_index;					//!< 在卖家货架上的序号
		short buy_num;						//!< 购买数量
		unsigned int upshelve_time;			//!< 商品上架时间 （用于核对物品）
		ItemID buy_item_id;					//!< 购买的物品ID （用于核对物品以及自动购买需求）
		short reserve_sh;					//!< 保留字
		int expect_price;					//!< 预期的价格（用于自动购买时，核对是否超过预期价格）
		UniqueKeyStr unique_key_str;		//!< 唯一标识字符串 （用于核对物品）

		long long order_id;					//!< 订单ID
	};

	struct HiddenGameTradeMarketRequestBuyResp //!< 跨服返回原服价格
	{
		HiddenGameTradeMarketRequestBuyResp() : header(MT_HIDDEN_GAME_TRADE_MARKET_REQUEST_BUY_RESP) {}
		MessageHeader header;

		int role_id;						//!< 买家uid
		int total_price;					//!< 总价 （要扣的元宝数）

		int seller_uid;						//!< 卖家uid
		short sale_index;					//!< 在卖家货架上的序号
		short buy_num;						//!< 购买数量
		unsigned int upshelve_time;			//!< 商品上架时间 （用于核对物品）
		ItemID buy_item_id;					//!< 购买的物品ID （用于核对物品以及自动购买需求）
		short reserve_sh;					//!< 保留字
		int expect_price;					//!< 预期的价格（用于自动购买时，核对是否超过预期价格）
		UniqueKeyStr unique_key_str;		//!< 唯一标识字符串 （用于核对物品）
		long long order_id;					//!< 订单ID
	};

	struct GameHiddenTradeMarketRequestBuyConfirm //!< 原服确认已扣钱并让跨服促成交易
	{
		GameHiddenTradeMarketRequestBuyConfirm() : header(MT_GAME_HIDDEN_TRADE_MARKET_REQUEST_BUY_CONFIRM) {}
		MessageHeader header;

		int plat_type;						//!< 平台
		int server_id;						//!< 服务器ID
		int role_id;						//!< 买家uid
		GameName role_name;					//!< 买家名字
		PlatName role_pname;				//!< 买家账号

		int seller_uid;						//!< 卖家uid
		short sale_index;					//!< 在卖家货架上的序号
		short buy_num;						//!< 购买数量
		unsigned int upshelve_time;			//!< 商品上架时间 （用于核对物品）
		ItemID buy_item_id;					//!< 购买的物品ID （用于核对物品以及自动购买需求）
		short reserve_sh;					//!< 保留字
		int expect_price;					//!< 预期的价格（用于自动购买时，核对是否超过预期价格）
		UniqueKeyStr unique_key_str;		//!< 唯一标识字符串 （用于核对物品）
		long long order_id;					//!< 订单ID
		int consumed_gold;					//!< 付款数
	};

	struct HiddenGameTradeMarketRequestBuyConfirmResp
	{
		HiddenGameTradeMarketRequestBuyConfirmResp() : header(MT_HIDDEN_GAME_TRADE_MARKET_REQUEST_BUY_CONFIRM_RESP) {}
		MessageHeader header;

		int role_id;						//!< 买家uid  已购买
		GameName role_name;					//!< 买家名字
		PlatName role_pname;				//!< 买家账号

		int seller_uid;						//!< 卖家uid
		short sale_index;					//!< 在卖家货架上的序号
		short buy_num;						//!< 购买数量
		unsigned int upshelve_time;			//!< 商品上架时间 （用于核对物品）
		ItemID buy_item_id;					//!< 购买的物品ID （用于核对物品以及自动购买需求）
		short reserve_sh;					//!< 保留字
		int expect_price;					//!< 预期的价格（用于自动购买时，核对是否超过预期价格）
		UniqueKeyStr unique_key_str;		//!< 唯一标识字符串 （用于核对物品）

		ItemDataWrapper wrapper;			//!< 玩家买到的物品
		long long consumed_gold;			//!< 付款数

		long long order_id;					//!< 订单ID
	};

	struct HiddenGameTradeMarketRequestBuyConfirmErrorResp
	{
		HiddenGameTradeMarketRequestBuyConfirmErrorResp() : header(MT_HIDDEN_GAME_TRADE_MARKET_REQUEST_BUY_CONFIRM_ERROR_RESP) {}
		MessageHeader header;

		int role_id;						//!< 买家uid

		int seller_uid;						//!< 卖家uid
		short sale_index;					//!< 在卖家货架上的序号
		short buy_num;						//!< 购买数量

		ItemID buy_item_id;					//!< 购买的物品ID （用于核对物品以及自动购买需求）
		short reserve_sh;					//!< 保留字
		UniqueKeyStr unique_key_str;		//!< 唯一标识字符串 （用于核对物品）

		char error_msg[128];				
		int error_num;
		long long order_id;					//!< 订单ID
	};

	struct GameHiddenTradeMarketRequestUpshelve
	{
		GameHiddenTradeMarketRequestUpshelve() : header(MT_GAME_HIDDEN_TRADE_MARKET_REQUEST_UPSHELVE) {}
		MessageHeader header;

		int plat_type;						//!< 平台
		int server_id;						//!< 服务器ID

		int role_id;						//!< 玩家uid
		GameName role_name;					//!< 玩家角色名
		PlatName role_pname;				//!< 玩家账号

		int gold_price;						//!< 单价  元宝
		ItemDataWrapper item_wrapper;		//!< 物品数据

		int service_fee;					//!< 上架铜币费用
		int credit_fee;						//!< 上架信用扣除

		long long order_id;					//!< 订单ID
	};

	struct HiddenGameTradeMarketRequestUpshelveResp
	{
		HiddenGameTradeMarketRequestUpshelveResp() : header(MT_HIDDEN_GAME_TRADE_MARKET_REQUEST_UPSHELVE_RESP) {}
		MessageHeader header;

		enum RESULT_TYPE
		{
			RESULT_SUCC = 0,
			RESULT_NO_EMPTY_SPACE = 1,
			RESULT_ADD_SALE_ITEM_FAIL = 2,
		};

		int role_id;
		int result;
		int service_fee;					//!< 服务费  （用于返还）
		int trade_credit;					//!< 交易信用（用于返还）
		ItemDataWrapper item_wrapper;		//!< 物品数据 (用于返还）
		long long order_id;					//!< 订单ID
	};

	struct GameHiddenTradeMarketOffshelveReq
	{
		GameHiddenTradeMarketOffshelveReq() : header(MT_GAME_HIDDEN_TRADE_MARKET_OFFSHELVE_REQ) {}
		MessageHeader header;

		int plat_type;						//!< 平台
		int server_id;						//!< 服务器ID

		int role_id;
		GameName role_name;					//!< 玩家角色名
		PlatName role_pname;				//!< 玩家账号

		int sale_index;						//!< 货物在货架上的序号 [0,8)
	};

	struct HiddenGameTradeMarketOffshelveResp
	{
		HiddenGameTradeMarketOffshelveResp() : header(MT_HIDDEN_GAME_TRADE_MARKET_OFFSHELVE_RESP) {}
		MessageHeader header;

		enum RESULT_TYPE
		{
			RESULT_SUCC = 0,
		};

		int role_id;
		int sale_index;
		int result;
		ItemDataWrapper item_wrapper;		
		int return_credit;
	};

	struct GameHiddenTradeMarketRedoUpshelveReq
	{
		GameHiddenTradeMarketRedoUpshelveReq() : header(MT_GAME_HIDDEN_TRADE_MARKET_REDO_UPSHELVE_REQ) {}
		MessageHeader header;

		int plat_type;						//!< 平台
		int server_id;						//!< 服务器ID

		int role_id;
		GameName role_name;					//!< 玩家角色名
		PlatName role_pname;				//!< 玩家账号

		int sale_index;						//!< 序号
		int price;							//!< 单价
	};

	struct HiddenGameTradeMarketRedoUpshelveResp
	{
		HiddenGameTradeMarketRedoUpshelveResp() : header(MT_HIDDEN_GAME_TRADE_MARKET_REDO_UPSHELVE_RESP) {}
		MessageHeader header;

		enum RESULT_TYPE
		{
			RESULT_SUCC = 0,
			RESULT_INPUT_PARAM_ERROR = 1,
			RESULT_GET_INFO_FAILED = 2,
			RESULT_NOT_OFFSHELVE = 3,
			RESULT_CASH_OUT_FIRST = 4,
			RESULT_SOLD_OUT = 5,
			RESULT_CANNOT_TRADE = 6,
			RESULT_ITEM_CONFIG_ERROR = 7,
			RESULT_PRICE_NOT_IN_RANGE = 8,
			RESULT_REDO_ADD_FAILED = 9,
		};

		int role_id;
		int sale_index;
		int result;
	};

	struct GameHiddenTradeMarketCashOutReq
	{
		GameHiddenTradeMarketCashOutReq() : header(MT_GAME_HIDDEN_TRADE_MARKET_CASH_OUT_REQ) {}
		MessageHeader header;

		int plat_type;						//!< 平台
		int server_id;						//!< 服务器ID

		int role_id;
		GameName role_name;					//!< 玩家角色名
		PlatName role_pname;				//!< 玩家账号

		int sale_index;						//!< 序号
	};

	struct HiddenGameTradeMarketCashOutResp
	{
		HiddenGameTradeMarketCashOutResp() : header(MT_HIDDEN_GAME_TRADE_MARKET_CASH_OUT_RESP) {}
		MessageHeader header;

		enum RESULT_TYPE
		{
			RESULT_SUCC = 0,
			RESULT_ITEM_NOT_FOUND = 1,
			RESULT_NO_SOLD_NUM = 2,
			RESULT_CASH_OUT_FAILED = 3,
		};

		int role_id;
		int result;
		int add_gold;
	};

	struct GameHiddenTradeMarketAdvertiseReq
	{
		GameHiddenTradeMarketAdvertiseReq() : header(MT_GAME_HIDDEN_TRADE_MARKET_ADVERTISE_REQ) {}
		MessageHeader header;

		int role_id;
		int plat_type;
		int server_id;
		int sale_index;
	};

	struct HiddenGameTradeMarketAdvertiseResp
	{
		HiddenGameTradeMarketAdvertiseResp() : header(MT_HIDDEN_GAME_TRADE_MARKET_ADVERTISE_RESP) {}
		MessageHeader header;

		int role_id;
		int gold_cost;

		int price;
		ItemName item_name;
		int sale_index;
		unsigned int put_on_shelve_time;
		int item_id;
		long long unique_key;
	};

	struct GameHiddenTradeMarketAdvertiseConfirmBoardcast
	{
		GameHiddenTradeMarketAdvertiseConfirmBoardcast() : header(MT_GAME_HIDDEN_TRADE_MARKET_ADVERTISE_CONFIRM_BOARDCAST) {}
		MessageHeader header;

		int role_id;
		int plat_type;
		int server_id;

		int price;
		ItemName item_name;
		int sale_index;
		unsigned int put_on_shelve_time;
		int item_id;
		long long unique_key;
	};

	// 测试发包性能
	struct GameHiddenTestNetwork
	{
		GameHiddenTestNetwork() : header(MT_GAME_HIDDEN_TEST_NETWORK) {}
		MessageHeader header;
	
		short is_start;
		short is_end;

		int send_length;
		char buffer[200000];
	};

	struct GameHiddenBraveGroundGetInfo
	{
		GameHiddenBraveGroundGetInfo() : header(MT_GAME_HIDDEN_BRAVE_GROUND_GET_INFO) {}
		MessageHeader header;

		int role_id;
	};

	struct HiddenGameBraveGroundInfoRet
	{
		HiddenGameBraveGroundInfoRet() : header(MT_HIDDEN_GAME_BRAVE_GROUND_INFO_RET) {}
		MessageHeader header;

		int role_id;
		int reached_level;
	};

	struct GameHiddenBraveGroundSetInfo
	{
		GameHiddenBraveGroundSetInfo() : header(MT_GAME_HIDDEN_BRAVE_GROUND_SET_INFO) {}
		MessageHeader header;

		int role_id;
		int reached_level;
	};

	struct HiddenGameSpecialLogicActivityInfoSync
	{
		HiddenGameSpecialLogicActivityInfoSync() : header(MT_HIDDEN_GAME_SPECIAL_LOGIC_ACTIVITY_INFO_SYNC) {}
		MessageHeader header;

		int role_id;
		int special_logic_activity_type;
		unsigned int fetch_reward_zero_timestamp;
	};

	struct HiddenGameGetRankListReq
	{
		HiddenGameGetRankListReq() : header(MT_HIDDEN_GAME_GET_RANK_LIST_REQ) {}
		MessageHeader header;

		int role_id;
		int rank_type;
		int special_type;
		short rank_start;
		short rank_end;
	};

	struct HiddenGameGetFriendRankListReq
	{
		HiddenGameGetFriendRankListReq() : header(MT_HIDDEN_GAME_PERSON_RANK_FRIEND_REQ) {}
		MessageHeader header;

		int role_id;
		int rank_type;
		short rank_start;
		short rank_end;
		int friend_count;
		int m_friend_list[MAX_FRIEND_NUM];
	};

	struct GameHiddenTradeMarketHistoryTopPriceReq
	{
		GameHiddenTradeMarketHistoryTopPriceReq() : header(MT_GAME_HIDDEN_TRADE_MARKET_HISTORY_TOP_PRICE_REQ) {}
		MessageHeader header;

		int role_id;
	};

	enum HIDDEN_GAME_SHEN_SHOU_JIANG_LIN_REQ_TYPE
	{
		HIDDEN_GAME_SHEN_SHOU_JIANG_LIN_REQ_TYPE_INFO,
		HIDDEN_GAME_SHEN_SHOU_JIANG_LIN_REQ_TYPE_DRAW,
	};

	struct HiddenGameShenShouJiangLinReq
	{
		HiddenGameShenShouJiangLinReq() : header(MT_HIDDEN_GAME_SHEN_SHOU_JIANG_LIN_REQ) {}
		MessageHeader header;

		long long req_id;  //用来记log
		int role_id;
		int req_type;
	};

	struct HiddenGameHeartBeat
	{
		HiddenGameHeartBeat() : header(MT_HIDDEN_GAME_HEART_BEAT) {}
		MessageHeader header;
	};

	struct GameHiddenRoleInfoNoticeReqRoute
	{
		GameHiddenRoleInfoNoticeReqRoute() : header(MT_GAME_HIDDEN_ROLE_INFO_NOTICE_REQ_ROUTE) {}
		MessageHeader header;

		int role_id;
	};

	struct GameHiddenCloudArenaRealTimeInfoReq
	{
		GameHiddenCloudArenaRealTimeInfoReq() : header(MT_GAME_HIDDEN_CLOUD_ARENA_REAL_TIME_INFO_REQ) {}
		MessageHeader header;

		int role_id;
	};

	struct GameHiddenRoleLoginSendCrossChatGroupSimpleInfo
	{
	public:
		GameHiddenRoleLoginSendCrossChatGroupSimpleInfo() : header(MT_GAME_HIDDEN_ROLE_LOGIN_SEND_CROSS_CHAT_GROUP_SIMPLE_INFO) {}
		MessageHeader header;

		int plat_type;
		int server_id;
		int uid;

		long long cross_chat_group_list[MAX_ROLE_CROSS_GROUP];
	};

	struct GameHiddenDeleteMutePlayerChatRecord
	{
	public:
		GameHiddenDeleteMutePlayerChatRecord() : header(MT_GAME_HIDDEN_DELETE_MUTE_PLAYER_CHAT_RECORD) { }
		MessageHeader header;

		int uid;
		int forbid_seconds;
	};

	struct GameHiddenTradeMarketSaleItemEventHandler
	{
	public:
		GameHiddenTradeMarketSaleItemEventHandler() : header(MT_GAME_HIDDEN_TRADE_MARKET_SALE_ITEM_EVENTHANDLER) { }
		MessageHeader header;

		int uid;
		ItemID sel_item_id;
		short reserve_sh;
	};

	struct HiddenGameTradeMarketSaleItemEventHandler
	{
	public:
		HiddenGameTradeMarketSaleItemEventHandler() : header(MT_HIDEEN_GAME_TRADE_MARKET_SALE_ITEM_EVENTHANDLER) { }
		MessageHeader header;

		int uid;
		ItemID sel_item_id;
		short reserve_sh;
	};

	struct HiddenGameRoleColosseumReqRouter
	{
	public:
		HiddenGameRoleColosseumReqRouter() : header(MT_HIDDEN_GAME_COLOSSEUM_REQ_ROUTER) {}
		MessageHeader header;

		int uid;
		int req_type;
		int param;
	};

	struct GameHiddenChatGroupLeadershipHandover
	{
	public:
		GameHiddenChatGroupLeadershipHandover() : header(MT_GAME_HIDDEN_CHAT_GROUP_LEADERSHIP_HANDOVER) {}
		MessageHeader header;

		long long group_id;
		int old_leader_uid;
		int new_leader_uid;
	};

	struct GameHiddenRelicLootingReq
	{
	public:
		GameHiddenRelicLootingReq() : header(MT_GAME_HIDDEN_RELIC_LOOTING_REQ) {}
		MessageHeader header;

		int uid;
		int req_type;
		int param;
	};

	struct HiddenGameRelicLootingReq
	{
	public:
		HiddenGameRelicLootingReq() : header(MT_HIDDEN_GAME_RELIC_LOOTING_REQ) {}
		MessageHeader header;

		int uid;
		int req_type;
		int param;
	};

	struct GameHiddenSyncCloudArenaUserDataReq
	{
	public:
		GameHiddenSyncCloudArenaUserDataReq() : header(MT_GAME_HIDDEN_SYNC_CLOUD_ARENA_USER_DATA_REQ) {}
		MessageHeader header;

		int uid;
		int plat_type;
		int server_id;
	};

	struct HiddenGameSyncCloudArenaUserDataResp
	{
	public:
		HiddenGameSyncCloudArenaUserDataResp() : header(MT_HIDDEN_GAME_SYNC_CLOUD_ARENA_USER_DATA_RESP) {}
		MessageHeader header;

		int uid;
		unsigned int season_timestamp;
		int last_season_rank;
		int season_highest_dan_id;
		int season_highest_rank;
		int season_highest_score;
	};

	struct GameHiddenRoleLoginSyncCloudArenaUserInfo
	{
		GameHiddenRoleLoginSyncCloudArenaUserInfo() : header(MT_GAME_HIDDEN_ROLE_LOGIN_CLOUD_ARENA_USER_INFO_REQ) {}
		MessageHeader header;

		int uid;
	};

	struct GameHiddenMarketBuyItemRecordSynReq
	{
		GameHiddenMarketBuyItemRecordSynReq() : header(MT_GAME_HIDDEN_MARKET_BUY_ITEM_RECORD_SYN_REQ) {}
		MessageHeader header;

		int server_id;
		TransactionRecordInfo tran_record;
	};

	struct HiddenGameMarketBuyItemRecordSynReq
	{
		HiddenGameMarketBuyItemRecordSynReq() : header(MT_HIDDEN_GAME_MARKET_BUY_ITEM_RECORD_SYN_REQ) {}
		MessageHeader header;

		int server_id;
		int is_has_change_price;		//0:未修改 1:已修改(修改需要下发)
		MarketItemInfo item_info;
	}; 

	struct GameHiddenHundredGhostBossInfoReq
	{
		GameHiddenHundredGhostBossInfoReq() : header(MT_GAME_HIDDEN_HUNDRED_GHOST_BOSS_INFO_REQ) {}
		MessageHeader header;

		int uid;
	};

	struct GameHiddenMarketAllTransactionRecordSynReq : public IMessageSerializer, public IMessageDeserializer
	{
		GameHiddenMarketAllTransactionRecordSynReq() : header(MT_GAME_HIDDEN_MARKET_ALL_TRANSACTION_RECORD_SYN_REQ) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char * buffer, int buffer_size, int * out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type) &&  WRITE_SHORT(add_count);
			if (!ret) return false;

			for (int i = 0; i < add_count && i < MAX_MARKET_ITEM_NUM; i++)
			{
				ret = WRITE_USHORT(disc_cross_add_record_list[i].item_id) && WRITE_INT(disc_cross_add_record_list[i].transaction_num);

				if (!ret) return false;
			}
			
			return ret;
		}
		virtual bool Deserialize(const char * data, int data_length, int * out_offset)
		{
			bool ret = READ_BEGIN(header.msg_type) && READ_SHORT(add_count);
			if (!ret) return false;

			for (int i = 0; i < add_count && i < MAX_MARKET_ITEM_NUM; i++)
			{
				ret = READ_USHORT(disc_cross_add_record_list[i].item_id) && READ_INT(disc_cross_add_record_list[i].transaction_num);

				if (!ret) return false;
			}

			return ret;
		}

		UniqueServerID usid;
		short add_count;
		TransactionRecordInfo disc_cross_add_record_list[MAX_MARKET_ITEM_NUM];
	};

	struct HiddenGameMarketAllTransactionRecordSynReq : public IMessageSerializer, public IMessageDeserializer
	{
		HiddenGameMarketAllTransactionRecordSynReq() : header(MT_HIDDEN_GAME_MARKET_ALL_TRANSACTION_RECORD_SYN_REQ) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char * buffer, int buffer_size, int * out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type) && WRITE_INT(op_type) && WRITE_INT(count);
			if (!ret) return false;

			for (int i = 0; i < count && i < MAX_MARKET_ITEM_NUM; i++)
			{
				ret = WRITE_USHORT(item_list[i].item_id) && 
					WRITE_INT(item_list[i].cur_price) &&
					WRITE_INT(item_list[i].yesterday_price) &&
					WRITE_INT(item_list[i].transaction_num) &&
					WRITE_LL(item_list[i].transaction_count) &&
					WRITE_INT(item_list[i].version);

				if (!ret) return false;
			}

			return ret;
		}
		virtual bool Deserialize(const char * data, int data_length, int * out_offset)
		{
			bool ret = READ_BEGIN(header.msg_type) && READ_INT(op_type) && READ_INT(count);
			if (!ret) return false;

			for (int i = 0; i < count && i < MAX_MARKET_ITEM_NUM; i++)
			{
				ret = READ_USHORT(item_list[i].item_id) &&
					READ_INT(item_list[i].cur_price) &&
					READ_INT(item_list[i].yesterday_price) &&
					READ_INT(item_list[i].transaction_num) &&
					READ_LL(item_list[i].transaction_count) &&
					READ_INT(item_list[i].version);

				if (!ret) return false;
			}
			return ret;
		}

		enum OP_TYPE
		{
			OP_TYPE_HIDDEN_CONNECT_CROSS = 0,			//隐藏服连接上跨服后请求游戏服同步交易记录
			OP_TYPE_HIDDEN_RECV_GAME_INFO_SYN = 1,		//隐藏服收到游戏服交易信息后转发到各个游戏服
		};

		int op_type;
		int count;
		MarketItemInfo item_list[MAX_MARKET_ITEM_NUM];
	};

	struct GameHiddenChangeVideoReq
	{
		GameHiddenChangeVideoReq() : header(MT_GAME_HIDDEN_VIDEO_CHANGE_REQ) {}
		MessageHeader header;

		int uid;
		int monster_group_id;
		int video_score;
	};

	struct HiddenGameChangeVideoRet
	{
		HiddenGameChangeVideoRet() : header(MT_HIDDEN_GAME_VIDEO_CHANGE_RET) {}
		MessageHeader header;
		enum RET_REASON
		{
			RET_REASON_NO = 0,
			RET_REASON_YES = 1,
		};
		int ret_reason;
		int monster_group_id;
	};

	struct GameCrossQueryChivalrousRankListTop100Req
	{
		GameCrossQueryChivalrousRankListTop100Req() : header(MT_GAME_CROSS_QUERY_CHIVALROUS_RANK_LIST_TOP100_REQ) {}
		MessageHeader header;

		int uid;
		int server_id;
		int plat_type;
	};

	struct GameCrossQuerySomeRoleChivalrousRankReq
	{
		GameCrossQuerySomeRoleChivalrousRankReq() : header(MT_GAME_CROSS_QUERY_CHIVALROUS_RANK_LIST_ROLE_LIST_REQ) {}
		MessageHeader header;

		int uid;
		int server_id;
		int plat_type;
		int rank_type;

		int count;
		int role_id_list[CHIVALROUS_RANK_LIST_NUM];
	};

	struct CrossGameChivalrousRankListResp
	{
		CrossGameChivalrousRankListResp() : header(MT_CROSS_GAME_CHIVALROUS_RANK_LIST_RESP) {}
		MessageHeader header;

		int uid;
		int count;
		int rank_type;
		ChivalrousRankInfo rank_list[CHIVALROUS_RANK_LIST_NUM];
	};

	struct HiddenGameSyncCloudArenaSeasonInfo
	{
		HiddenGameSyncCloudArenaSeasonInfo() : header(MT_HIDDEN_GAME_SYNC_CLOUD_ARENA_SEASON_INFO) {}
		MessageHeader header;

		unsigned int cur_season_start_timestamp;
		unsigned int cur_season_end_timestamp;
	};
	
		
	struct GameCrossSyncCrossGuildRankInfo				// 29226  由主服向隐藏服同步跨服公会排行榜信息
	{
		GameCrossSyncCrossGuildRankInfo() : header(MT_GAME_CROSS_SYNC_CROSS_GUILD_RANK_INFO) {}
		MessageHeader header;

		struct GuildRankSyncInfo
		{
			GuildRankSyncInfo() : sync_type(0), index(-1) {};
			unsigned int sync_type;						// 同步状态 // 单人数据才有效 , 多人数据不看这个
			int index;									// 旧排名 , 插入时无效, 只对删除和更新有效
			GuildRankItem rank_info;
		};

		UniqueServerID from_usid;
		bool is_init;									// 初始的数据全覆盖
		char is_init_add;								// 0 : 覆盖  1 : 添加
		short reserve_sh;
		unsigned int last_change_time;
		int rank_type;
		int count;

		GuildRankSyncInfo item_list[GUILD_RANK_MAX_SIZE];
	};

	struct CrossGameSyncGuildRankRewardRankInfo						// 29227  由跨服向主服同步公会排名信息[主要用于排行榜奖励]
	{
		CrossGameSyncGuildRankRewardRankInfo() : header(MT_CROSS_GAME_SYNC_GUILD_RANK_REWARD_RANK_INFO) {}
		MessageHeader header;

		UniqueServerID server_id;							// 哪个服
		int rank_type;										// 排行榜类型
		int guild_id;										// 公会ID
		int rank_index;										// 排名
		long long rank_val;									// 对应排行榜类型的数值
	};

	struct HiddenGameRoleActiveChange						// 29228  从隐藏服同步角色活跃度信息变更回主服
	{
		HiddenGameRoleActiveChange() : header(MT_HIDDEN_GAME_ROLE_ACTIVE_CHANGE) {}
		MessageHeader header;

		UniqueServerID server_id;							// 哪个服
		int guild_id;										// 公会ID
		int role_id;										// 角色ID
		int add_val;										// 添加数值
	};

	struct GameHiddenGuildActiveInfoChange					// 29229  从主服向隐藏服同步公会活跃度变更
	{
		GameHiddenGuildActiveInfoChange() : header(MT_GAME_HIDDEN_GUILD_ACTIVE_INFO_CHANGE) {}
		MessageHeader header;

		int guild_id;										// 公会ID
		int day_active_val;									// 帮派日活跃度
		int week_active_val;								// 帮派周活跃度
		unsigned int change_times;							// 变更时间

		int role_id;										// 角色ID		[非0时 , 下面参数有效]
		int member_week_active_val;							// 周活跃值
		int reward_flag;									// 领取每日奖励标记
		
		int member_seven_today_active;						// 家族成员的今日获得的总活跃(从进入家族开始算)
	};

	struct HiddenGameRoleActiveGiftInfo						//  从隐藏服同步角色活跃礼包信息变更回主服
	{
		HiddenGameRoleActiveGiftInfo() : header(MT_HIDDEN_GAME_ROLE_ACTIVE_GIFT_INFO) {}
		MessageHeader header;

		int guild_id;										// 公会ID
		int role_id;										// 角色ID
		int flag;											// 添加数值
		unsigned int get_timestamp;							// 领取的时间戳
	};

	struct HiddenGameQiFuChange						// 29360  从隐藏服祈福增加值返回原服
	{
		HiddenGameQiFuChange() : header(MT_HIDDEN_GAME_ROLE_QI_FU_CHANGE) {}
		MessageHeader header;

		int guild_id;										// 公会ID
		int role_id;										// 角色ID
		int add_val;										// 添加数值
	};

	struct GameHiddenGuildQiFuValueChange					// 29361  从主服向隐藏服同步祈福值变更
	{
		GameHiddenGuildQiFuValueChange() : header(MT_GAME_HIDDEN_ROLE_QI_FU_CHANGE) {}
		MessageHeader header;

		int guild_id;										// 公会ID
		int role_id;										// 角色ID
		int value;											// 祈福值
	};

	struct GmGameCrossGuildActiveTest						// 29230  由主服到跨服结算一次奖励
	{
		GmGameCrossGuildActiveTest() : header(MT_GM_GAME_CROSS_GUILD_ACTIVE_TEST) {}
		MessageHeader header;

	};

	struct GmCrossGameGuildActiveTest						// 29231  由跨服到主服清理排行榜信息
	{
		GmCrossGameGuildActiveTest() : header(MT_GM_CROSS_GAME_GUILD_ACTIVE_TEST) {}
		MessageHeader header;
	};

	struct HiddenGameBigDipperSynRet
	{
		HiddenGameBigDipperSynRet() : hedaer(MT_HIDDEN_GAME_BIG_DIPPER_SYNC_RET) {}
		MessageHeader hedaer;

		BigDipperMonsterParam info;
	};


	// 红包
	enum SYNC_HONG_BAO_TO_CROSS_STATUS
	{
		SYNC_HONG_BAO_TO_CROSS_STATUS_INVALID = 0,	// 无效
		SYNC_HONG_BAO_TO_CROSS_STATUS_INSERT = 1,	// 添加		
		SYNC_HONG_BAO_TO_CROSS_STATUS_UPDATA = 2,	// 更新		
		SYNC_HONG_BAO_TO_CROSS_STATUS_DELETE = 3,	// 删除		
		SYNC_HONG_BAO_TO_CROSS_STATUS_MAX,
	};

	struct GameHiddenSyncHongBaoInfo					//从主服向隐藏服同步红包信息 
	{
		GameHiddenSyncHongBaoInfo() : header(MT_GAME_HIDDEN_SYNC_HONG_BAO_INFO) {}
		MessageHeader	header;

		struct HongBaoItemSyncInfo
		{
			HongBaoItemSyncInfo() : sync_type(0) {};
			unsigned int sync_type;						// 同步状态 // 单数据才有效 , 多数据不看这个
			int index;									// 更新时有用
			SyncHongBaoItem hongbap_item;
		};

		UniqueServerID from_usid;
		bool is_init;									// 初始的数据全覆盖
		char is_init_add;								// 0 : 覆盖  1 : 添加
		short reserve_sh;
		int sys_type;

		int count;
		HongBaoItemSyncInfo item_list[MAX_ONCE_HONG_BAO_COUNT];
	};


	struct HiddenGameSyncHongBaoReqGive					// 从隐藏服向主服同步发红包操作 
	{
		HiddenGameSyncHongBaoReqGive() : header(MT_HIDDEN_GAME_SYNC_HONG_BAO_REQ_GIVE) {}
		MessageHeader	header;

		GiveHongBaoUserInfo		give_hongbao_user_info;
	};

	struct HiddenGameSyncHongBaoReqGet			// 从隐藏服向主服同步发红包操作 
	{
		HiddenGameSyncHongBaoReqGet() : header(MT_HIDDEN_GAME_SYNC_HONG_BAO_REQ_GET) {}
		MessageHeader	header;

		UniqueServerID from_usid;

		int target_hongbao_id;					// 领取的红包ID
		int hongbao_sys_type;					// 红包所属于的系统类型	[例如:世界,家族等等..]
		int uid;								// 领取人UID
		GameName name;							// 领取人名称

		HongBaoItem::InfoData info_data;		// 根据sys的不同用处不同 , 也可能没用到,具体看sys_type
	};

	struct GameHiddenSyncHongBaoReqGiveRet			// 从主服向隐藏服同步发红包操作结果
	{
		GameHiddenSyncHongBaoReqGiveRet() : header(MT_GAME_HIDDEN_SYNC_HONG_BAO_REQ_GIVE_RET) {}
		MessageHeader	header;

		UniqueServerID from_usid;
		int req_uid;							// 操作人员UID

		int ret;								// 结果 : 0 失败 , 1 成功

		int hongbao_id;							// 红包_ID
		int sys_type;							// 红包系统类型
		int type;								// 红包类型
		HongBaoGreeting greeting;				// 祝福语
	};

	struct GameHiddenSyncHongBaoReqGetRet		// 从主服向隐藏服同步领红包操作结果
	{
		GameHiddenSyncHongBaoReqGetRet() : header(MT_GAME_HIDDEN_SYNC_HONG_BAO_REQ_GET_RET) {}
		MessageHeader	header;

		UniqueServerID from_usid;
		int req_uid;							// 操作人员UID
		int ret;								// 结果 : 0 失败 , 1 成功

		int hongbao_id;							// 红包_ID
		int sys_type;							// 红包系统类型
		int val;								// 如果领取成功 则为领取到的货币数量 , 如果失败 这是错误码
	};

	enum HiddenGameSyncHongBaoReqError_TYPE
	{
		HiddenGameSyncHongBaoReqError_TYPE_GIVE = 0,	// 发红包
		HiddenGameSyncHongBaoReqError_TYPE_GET = 1,		// 领红包
	};

	struct HiddenGameSyncHongBaoReqError		// 从主服向隐藏服同步领红包操作结果
	{
		HiddenGameSyncHongBaoReqError() : header(MT_HIDDEN_GAME_SYNC_HONG_BAO_REQ_ERROR) {}
		MessageHeader	header;

		UniqueServerID from_usid;
		int req_uid;							// 操作人员UID
		int req_type;							// 操作的类型

		int hongbao_id;							// 红包_ID	
		int sys_type;							// 红包系统类型
		int type;								// 红包类型

		int val;								// 如果领取成功 则为领取到的货币数量
	};

	enum GAME_HIDDEN_GIVE_GIFT_REQ_TYPE 
	{
		GAME_HIDDEN_GIVE_GIFT_REQ_TYPE_GM_FETCH,
		GAME_HIDDEN_GIVE_GIFT_REQ_TYPE_FETCH,
		GAME_HIDDEN_GIVE_GIFT_REQ_TYPE_ACK,			//游戏服发送确认物品放进背包		
	};

	struct GameHiddenGiveGiftReq		// // 从游戏服向隐藏服请求礼物操作
	{
		GameHiddenGiveGiftReq() : header(MT_GAME_HIDDEN_GIVE_GIFT_REQ) {}
		MessageHeader	header;

		int req_type;

		int role_id;
		union 
		{
			long long give_gift_id_ll;
			int p1;
			int p2;
		};
		
		int p3;		
	};

	struct GameHiddenGiveGiftAddGift 	// 从游戏服向隐藏服请求礼物操作
	{
		GameHiddenGiveGiftAddGift() : header(MT_GAME_HIDDEN_GIVE_GIFT_ADD_GIFT) {}
		MessageHeader	header;

		CommonSaveGiveGiftData gift_data;
	};

	struct HiddenGameGiveGiftRetInfo		//游戏服向跨服领取礼物返回物品信息
	{
		HiddenGameGiveGiftRetInfo() : header(MT_GAME_HIDDEN_GIVE_GIFT_INFO_RET) {}
		MessageHeader	header;

		int role_id;
		long long give_gift_id_ll;
		CommonSaveGiveGiftData gift_data;
	};

	enum HIDDEN_GAME_SHI_TU_SEEK_REQ_TYPE 
	{
		HIDDEN_GAME_SHI_TU_SEEK_REQ_TYPE_INFO,
	};

	struct HiddenGameShiTuSeekReq		//隐藏富向游戏服请求师徒宣言信息
	{
		HiddenGameShiTuSeekReq() : header(MT_HIDDEN_GAME_SHI_TU_SEEK_REQ) {}
		MessageHeader	header;

		int type;
		int role_id;
		int p1;
	};

	struct HiddenGameContinueCrossBattle
	{
		HiddenGameContinueCrossBattle() : header(MT_HIDDEN_GAME_CONTINUE_CROSS_BATTLE) {}
		MessageHeader header;

		int role_id;
	};

	
	struct HiddenGameCowReportGoodNewsCheckCanBuy			// 隐藏服 ->主服  检查能否购买福牛报喜 
	{
		HiddenGameCowReportGoodNewsCheckCanBuy() : header(MT_HIDDEN_GAME_COW_REPORT_GOOD_NEWS_CHECK_CAN_BUY) {}
		MessageHeader header;

		int role_id;										// 请求购买的玩家UID
	};

	struct GameHiddenCowReportGoodNewsCheckCanBuyAck			// 主服 ->隐藏服  检查能否购买福牛报喜应答
	{
		GameHiddenCowReportGoodNewsCheckCanBuyAck() : header(MT_GAME_HIDDEN_COW_REPORT_GOOD_NEWS_CHECK_CAN_BUY_ACK) {}
		MessageHeader header;

		int role_id;										// 请求购买的玩家UID
		int ret;											// 0 :为可以购买 , 非0为返回的错误码
	};

	struct HiddenGameCowReportGoodNewsBuyItem			// 隐藏服 ->主服  购买福牛报喜
	{
		HiddenGameCowReportGoodNewsBuyItem() : header(MT_HIDDEN_GAME_COW_REPORT_GOOD_NEWS_BUY_ITEM) {}
		MessageHeader header;

		int role_id;
		short  avatar_type;									// 体型类型
		short  headshot_id;									// 头像ID
		GameName role_name;
	};

	struct GameHiddenCowReportGoodNewsBuyItemAck			// 主服 ->隐藏服   购买福牛报喜结果应答
	{
		GameHiddenCowReportGoodNewsBuyItemAck() : header(MT_GAME_HIDDEN_COW_REPORT_GOOD_NEWS_BUY_ITEM_ACK) {}
		MessageHeader header;

		int role_id;
		int ret;											// 0 :购买成功 , 非0为返回的错误码 [需要返还元宝的]
	};

	struct SyncCowReportGoodNewsBuyItemOfflineEvent			// 同步购买福牛报喜购买失败的元宝返还事件的
	{
		SyncCowReportGoodNewsBuyItemOfflineEvent() : header(MT_SYNC_COW_REPORT_GOOD_NEWS_BUY_ITEM_OFFLINE_EVENT) {}
		MessageHeader header;

		int role_id;				// 操作人
		int return_val;				// 0:删除 , > 0 需要记录的返还的元宝数量
	};

	enum HIDDEN_GAME_COW_REPORT_GOOD_NEWS_GET_INFO_TYPE
	{
		HIDDEN_GAME_COW_REPORT_GOOD_NEWS_GET_INFO_TYPE_SYS_INFO ,	// 获取福牛报喜的系统信息
		HIDDEN_GAME_COW_REPORT_GOOD_NEWS_GET_INFO_TYPE_REWARD_INFO,	// 获取福牛报喜的得奖列表
		HIDDEN_GAME_COW_REPORT_GOOD_NEWS_GET_INFO_TYPE_BUY_INFO,	// 获取玩家的购买信息

	};

	struct HiddenGameCowReportGoodNewsGetInfo			// 隐藏服 ->主服  获取福牛报喜的系统信息  
	{
		HiddenGameCowReportGoodNewsGetInfo() : header(MT_HIDDEN_GAME_COW_REPORT_GOOD_NEWS_GET_INFO) {}
		MessageHeader header;

		int role_id;				// 操作人
		int type;					// 获取的信息类型
	};

	/////////////////////////////////
	struct HiddenGameTigerBringsBlessingCheckCanBuy			// 隐藏服 ->主服  检查能否购买寅虎纳福
	{
		HiddenGameTigerBringsBlessingCheckCanBuy() : header(MT_HIDDEN_GAME_TIGER_BRINGS_BLESSING_CHECK_CAN_BUY) {}
		MessageHeader header;

		int role_id;										// 请求购买的玩家UID
	};

	struct GameHiddenTigerBringsBlessingCheckCanBuyAck			// 主服 ->隐藏服  检查能否购买寅虎纳福应答
	{
		GameHiddenTigerBringsBlessingCheckCanBuyAck() : header(MT_GAME_HIDDEN_TIGER_BRINGS_BLESSING_CHECK_CAN_BUY_ACK) {}
		MessageHeader header;

		int role_id;										// 请求购买的玩家UID
		int ret;											// 0 :为可以购买 , 非0为返回的错误码
	};

	struct HiddenGameTigerBringsBlessingBuyItem			// 隐藏服 ->主服  购买寅虎纳福
	{
		HiddenGameTigerBringsBlessingBuyItem() : header(MT_HIDDEN_GAME_TIGER_BRINGS_BLESSING_BUY_ITEM) {}
		MessageHeader header;

		int role_id;
		short  avatar_type;									// 体型类型
		short  headshot_id;									// 头像ID
		GameName role_name;
	};

	struct GameHiddenTigerBringsBlessingBuyItemAck			// 主服 ->隐藏服   购买寅虎纳福结果应答
	{
		GameHiddenTigerBringsBlessingBuyItemAck() : header(MT_GAME_HIDDEN_TIGER_BRINGS_BLESSING_BUY_ITEM_ACK) {}
		MessageHeader header;

		int role_id;
		int ret;											// 0 :购买成功 , 非0为返回的错误码 [需要返还元宝的]
	};

	struct SyncTigerBringsBlessingBuyItemOfflineEvent			// 同步购买寅虎纳福购买失败的元宝返还事件的
	{
		SyncTigerBringsBlessingBuyItemOfflineEvent() : header(MT_SYNC_TIGER_BRINGS_BLESSING_BUY_ITEM_OFFLINE_EVENT) {}
		MessageHeader header;

		int role_id;				// 操作人
		int return_val;				// 0:删除 , > 0 需要记录的返还的元宝数量
	};

	enum HIDDEN_GAME_TIGER_BRINGS_BLESSING_GET_INFO_TYPE
	{
		HIDDEN_GAME_TIGER_BRINGS_BLESSING_GET_INFO_TYPE_SYS_INFO,	// 获取寅虎纳福的系统信息
		HIDDEN_GAME_TIGER_BRINGS_BLESSING_GET_INFO_TYPE_REWARD_INFO,	// 获取寅虎纳福的得奖列表
		HIDDEN_GAME_TIGER_BRINGS_BLESSING_GET_INFO_TYPE_BUY_INFO,	// 获取玩家的购买信息

	};

	struct HiddenGameTigerBringsBlessingGetInfo			// 隐藏服 ->主服  获取寅虎纳福的系统信息  
	{
		HiddenGameTigerBringsBlessingGetInfo() : header(MT_HIDDEN_GAME_TIGER_BRINGS_BLESSING_GET_INFO) {}
		MessageHeader header;

		int role_id;				// 操作人
		int type;					// 获取的信息类型
	};


	enum DATA_SYNC_CHECK_TYPE
	{
		DATA_SYNC_CHECK_TYPE_CROSS_USER_REGISTER, 
		DATA_SYNC_CHECK_TYPE_GUILD,
		DATA_SYNC_CHECK_TYPE_ACTIVITY,
		DATA_SYNC_CHECK_TYPE_WORLD_STATUS,
		DATA_SYNC_CHECK_TYPE_USER_CACHE,
		DATA_SYNC_CHECK_TYPE_CHAT,
		DATA_SYNC_CHECK_TYPE_RANK,
		DATA_SYNC_CHECK_TYPE_ORDER,
		DATA_SYNC_CHECK_TYPE_MARKET,
		DATA_SYNC_CHECK_TYPE_HONG_BAO,
		DATA_SYNC_CHECK_TYPE_FRIEND_INTIMACY,
		DATA_SYNC_CHECK_TYPE_FRIEND,
		DATA_SYNC_CHECK_TYPE_RAND_ACTIVITY,
		DATA_SYNC_CHECK_TYPE_KUA_FU_YING_XIONG_LU,
		DATA_SYNC_CHECK_TYPE_MAX
	};


	struct CrossGameDataSyncCheckAck					// 跨服 - > 主服 或 隐藏服 -> 主服 数据同步验证回执
	{
		CrossGameDataSyncCheckAck() : header(MT_CROSS_GAME_DATA_SYNC_CHECK_ACK) {}
		MessageHeader header;

		int type;					// 同步的数据类型
	};
	


	////////////////////////////////////////////////////////////////////////////////////////////////////////////


	struct HiddenGameSyncCrossData
	{
		HiddenGameSyncCrossData() : header(MT_HIDDEN_GAME_SYNC_CROSS_DATA) {}
		MessageHeader header;
	};

	struct GameHiddenCreatAutoRobotInfo
	{
		GameHiddenCreatAutoRobotInfo() : header(MT_GAME_HIDDEN_CREAT_AUTO_ROBOT_INFO) {}
		MessageHeader header;

		UniqueServerID mother_server_usid;
		int can_creat_static_robot;	// 可创建自动机器人
	};
	
	struct HiddenGameTianDiDaoHenSyncInfo
	{
		HiddenGameTianDiDaoHenSyncInfo() : header(MT_HIDDEN_GAME_TIAN_DI_DAO_HEN_SYNC_INFO) {}
		MessageHeader header;

		int state;
		long long next_state_time;
	};

	struct GameHiddenColosseumRankInfoSyn
	{
		GameHiddenColosseumRankInfoSyn() : header(MT_GAME_HIDDEN_SYN_COLOSSENUM_RANK_INFO) {}
		MessageHeader header;

		int role_id;
		int colosseum_rank;
	};


	enum SYNC_FRIEND_INTIMACY_TO_CROSS_STATUS
	{
		SYNC_FRIEND_INTIMACY_TO_CROSS_STATUS_INVALID = 0,		// 无效
		SYNC_FRIEND_INTIMACY_TO_CROSS_STATUS_INSERT = 1,		// 添加		
		SYNC_FRIEND_INTIMACY_TO_CROSS_STATUS_UPDATA = 2,		// 更新		
		SYNC_FRIEND_INTIMACY_TO_CROSS_STATUS_DELETE = 3,		// 删除		
		SYNC_FRIEND_INTIMACY_TO_CROSS_STATUS_MAX,
	};

	struct SyncFriendIntimacyInfo						// 同步好友亲密度信息 ,可能 隐藏服->原服  也可能是 原服->隐藏服,具体需要看逻辑
	{
		SyncFriendIntimacyInfo() : header(MT_SYNC_FRIEND_INTIMACY_INFO) {}
		MessageHeader header;

		struct FriendItemSyncInfo
		{
			FriendItemSyncInfo() : sync_type(0) {};
			unsigned int sync_type;						// 同步状态 // 单数据才有效 , 多数据不看这个
			FriendIntimacyInfo friend_item;
		};

		UniqueServerID from_usid;						// 来至哪个原服 [可能用不上]
		bool is_init;									// 初始的数据全覆盖
		char is_init_add;								// 0 : 覆盖  1 : 添加
		short reserve_sh;

		int count;										// 表长
		FriendItemSyncInfo item_list[MAX_ONCE_FRIENDS_INTIMACY_COUNT];
	};
	
	struct GameHiddenAddIntimacyReq						// 从主服向隐藏服   同步变更亲密度操作信息
	{
		GameHiddenAddIntimacyReq() : header(MT_GAME_HIDDEN_ADD_INTIMACY_REQ) {}
		MessageHeader header;

		int role_uid;									// 操作人ID
		int friend_uid;									// 好友ID
		int add_val;									// 变更数值
	};
	
	struct GameHiddenIntimacyRelationReq						// 从主服向隐藏服   同步变更亲密度操作信息
	{
		GameHiddenIntimacyRelationReq() : header(MT_GAME_HIDDEN_INTIMACY_RELATION_REQ) {}
		MessageHeader header;

		int role_uid;											// 操作人ID
		int friend_uid;											// 好友ID
		int is_add;												// 0:删除 , 1: 添加
	};
	
	struct GameHiddenIntimacySingleChatTimes					// 从主服向隐藏服   同步好友亲密度的私聊时间戳
	{
		GameHiddenIntimacySingleChatTimes() : header(MT_GAME_HIDDEN_INTIMACY_SINGLE_CHAT_TIMES) {}
		MessageHeader header;

		int role_uid;											// 操作人ID
		int friend_uid;											// 好友ID
		int is_to;												// 0:好友发给玩家 , 1: 玩家发给好友
		unsigned int timestamp;									// 消息的时间
	};
	
	struct GameHiddenGiveFlowerNotice							// 同步好友送花行为广播 , 用于广播特效和传闻
	{
		GameHiddenGiveFlowerNotice() : header(MT_GAME_HIDDEN_GIVE_FLOWER_NOTICE) {}
		MessageHeader header;

		int role_uid;											// 操作人ID
		int friend_uid;											// 好友ID
		int item_id;											// 花的物品ID
		int type;												// 传闻类型
	};
	
	struct HiddenGameGiveFlowerNotice							// 从隐藏服向主服   同步好友送花行为广播, 用于广播特效和传闻
	{
		HiddenGameGiveFlowerNotice() : header(MT_HIDDEN_GAME_GIVE_FLOWER_NOTICE) {}
		MessageHeader header;

		int role_id;											// 目标玩家
		GameName role_name;										// 送花人名字
		int friend_id;											// 收花人ID
		GameName friend_name;									// 收花人名字
		int item_id;											// 花的物品ID
		int type;												// 传闻特效类型
	};

	struct GameHiddenGiveFlowerBackKiss							// 主服向隐藏服同步   同步好友送花回吻
	{
		GameHiddenGiveFlowerBackKiss() : header(MT_GAME_HIDDEN_GIVE_FLOWER_BACK_KISS) {}
		MessageHeader header;

		int role_uid;											// 赠送人ID
		GameName role_name;										// 赠送者名字
		int to_role_id;											// 接收人ID
	};

	struct HiddenGameGiveFlowerBackKiss							// 隐藏服向主服同步   同步好友送花回吻
	{
		HiddenGameGiveFlowerBackKiss() : header(MT_HIDDEN_GAME_GIVE_FLOWER_BACK_KISS) {}
		MessageHeader header;

		int role_uid;											// 赠送人ID
		GameName role_name;										// 赠送者名字
		int to_role_id;											// 接收人ID
	};

	struct HiddenGameActivityEndInfo
	{
		HiddenGameActivityEndInfo() : header(MT_HIDDEN_GAME_ACTIVITY_END_INFO) {}
		MessageHeader header;

		int role_id;
		int activity_type;
		int param1;
	};

	struct HiddenGameServerCourseInfoReq
	{
		HiddenGameServerCourseInfoReq() : header(MT_HIDDEN_GAME_SERVER_COURSE_INFO_REQ) {}
		MessageHeader header;

		int role_id;
	};

	struct HiddenGameServerCourseChangeReq
	{
		HiddenGameServerCourseChangeReq() : header(MT_HIDDEN_GAME_SERVER_COURSE_CHANGE_REQ) {}
		MessageHeader header;

		int role_id;
		int seq;
		int is_add;
	};

	struct HiddenGameRoleWinChiefElectionSyn
	{
		HiddenGameRoleWinChiefElectionSyn() : header(MT_HIDDEN_GAME_ROLE_WIN_CHIEF_ELECTION_SYN) {}
		MessageHeader header;

		int role_id;
		int prof;
	};

	struct GameHiddenJieYiZuCreate
	{
		GameHiddenJieYiZuCreate() : header(MT_GAME_HIDDEN_JIE_YI_ZU_CREATE) {}
		MessageHeader header;

		int zu_zhang_id;
		JieYIZuName jie_yi_name;
		int count;
		CommonSaveJieYiZuRoleInfoItem list[JIE_YI_ZU_MAX_ROLE_ITEM_NUM];
	};
	
	struct HiddenGameJieYiZuSync
	{
		HiddenGameJieYiZuSync() : header(MT_HIDDEN_GAME_JIE_YI_ZU_INFO_SYNC) {}
		MessageHeader header;
		
		int sync_type;
		CommonSaveJieYiZu jie_yi_zu_info;
	};

	struct HiddenGameJieYiZuDismiss
	{
		HiddenGameJieYiZuDismiss() : header(MT_HIDDEN_GAME_JIE_YI_ZU_DISMISS) {}
		MessageHeader header;

		long long jie_yi_zu_id;
	};

	struct GameHiddenJieYiZuName
	{
		GameHiddenJieYiZuName() : header(MT_GAME_HIDDEN_JIE_YI_ZU_NAME) {}
		MessageHeader header;

		long long jie_yi_zu_id;
		int role_id;
		JieYIZuName ji_yi_zu_name;
	};

	struct GameHiddenJieYiZuMemberName
	{
		GameHiddenJieYiZuMemberName() : header(MT_GAME_HIDDEN_JIE_YI_ZU_MEMBER_NAME) {}
		MessageHeader header;

		long long jie_yi_zu_id;
		int role_id;
		JieYIZuMemberName member_name;
	};

	struct GameHiddenJieYiZuText
	{
		GameHiddenJieYiZuText() : header(MT_GAME_HIDDEN_JIE_YI_ZU_TEXT) {}
		MessageHeader header;

		long long jie_yi_zu_id;
		int role_id;
		JieYIZuText text;
	};

	struct GameHiddenJieYiZuLeave
	{
		GameHiddenJieYiZuLeave() : header(MT_GAME_HIDDEN_JIE_YI_ZU_LEAVE) {}
		MessageHeader header;

		long long jie_yi_zu_id;
		int role_id;
		int is_cancel;
	};

	struct GameHiddenJieYiZuKick
	{
		GameHiddenJieYiZuKick() : header(MT_GAME_HIDDEN_JIE_YI_ZU_KICK) {}
		MessageHeader header;

		long long jie_yi_zu_id;
		int role_id;
		int target_role_id;
	};

	struct GameHiddenJieYiZuTanHe
	{
		GameHiddenJieYiZuTanHe() : header(MT_GAME_HIDDEN_JIE_YI_ZU_TAN_HE) {}
		MessageHeader header;

		long long jie_yi_zu_id;
		int role_id;
	};

	struct GameHiddenJieYiZuTanHeVote
	{
		GameHiddenJieYiZuTanHeVote() : header(MT_GAME_HIDDEN_JIE_YI_ZU_TAN_HE_VOTE) {}
		MessageHeader header;

		long long jie_yi_zu_id;
		int role_id;
		int target_uid;
	};

	struct GameHiddenJieYiZuInviteReply
	{
		GameHiddenJieYiZuInviteReply() : header(MT_GAME_HIDDEN_JIE_YI_ZU_INVITE_REPLY) {}
		MessageHeader header;

		long long jie_yi_zu_id;
		int role_id;
		int is_agree;
	};

	struct GameHiddenJieYiRoleXuanYan
	{
		GameHiddenJieYiRoleXuanYan() : header(MT_GAME_HIDDEN_JIE_YI_ZU_ROLE_XUAN_YAN) {}
		MessageHeader header;

		int is_delete;
		int role_id;
		JieYIZuXuanYanText text;
	};

	struct GameHiddenJieYiZuXuanYan
	{
		GameHiddenJieYiZuXuanYan() : header(MT_GAME_HIDDEN_JIE_YI_ZU_XUAN_YAN) {}
		MessageHeader header;

		int is_delete;
		int role_id;
		JieYIZuXuanYanText text;
	};

	struct HiddenGameJieYiSyncRoleData
	{
		HiddenGameJieYiSyncRoleData() : header(MT_HIDDEN_GAME_JIE_YI_SYNC_ROLE_DATA) {}
		MessageHeader header;
		
		CommonSaveJieYiZuRoleInfoItem role_data;
	};

	struct GameHiddenJieYiXuanYanList
	{
		GameHiddenJieYiXuanYanList() : header(MT_GAME_HIDDEN_JIE_YI_XUAN_YAN_LIST) {}
		MessageHeader header;

		int role_id;
		int type;
	};

	struct HiddenGameJieYiRoleChange
	{
		HiddenGameJieYiRoleChange() : header(MT_HIDDEN_GAME_JIE_YI_ROLE_CHANGE) {}
		MessageHeader header;

		int role_id;
		int join_jie_yi_day;
	};

	struct HiddenCrossEndlessRankReward
	{
		HiddenCrossEndlessRankReward() : header(MT_HIDDEN_CROSS_ENDLESS_RANK_REWARD) {}
		MessageHeader header;
	};

	struct CrossHiddenEndlessRankRewardRet
	{
		CrossHiddenEndlessRankRewardRet() : header(MT_CROSS_HIDDEN_ENDLESS_RANK_REWARD_RET) {}
		MessageHeader header;
		int count;
		PersonRankItem rank_list[CHIVALROUS_RANK_LIST_NUM];
	};

	struct GameHiddenGiveFlowerChange
	{
		GameHiddenGiveFlowerChange() : header(MT_GAME_HIDDEN_GIVE_FLOWER_CHANGE) {}
		MessageHeader header;

		int role_id;
	};

	struct HiddenGameGiveFlowerChange
	{
		HiddenGameGiveFlowerChange() : header(MT_HIDDEN_GAME_GIVE_FLOWER_CHANGE) {}
		MessageHeader header;

		int role_id;
	};
	enum HIDDEN_GAME_HORCRUX_TIAN_CHENG_REQ_TYPE
	{
		HIDDEN_GAME_HORCRUX_TIAN_CHENG_REQ_TYPE_INFO,
		HIDDEN_GAME_HORCRUX_TIAN_CHENG_REQ_TYPE_DRAW,
	};

	struct HiddenGameHorcruxTianChengReq
	{
		HiddenGameHorcruxTianChengReq() : header(MT_HIDDEN_GAME_HORCRUX_TIAN_CHENG_REQ) {}
		MessageHeader header;

		long long req_id;  //用来记log
		int role_id;
		int req_type;
		int rand_times;
		BitMap<MAX_HORCRUX_TIANCHENG_REWARD_GROUP_NUM> floor_bitmap;
	};


	struct GameHiddenQueryItemRecommendPrice
	{
		GameHiddenQueryItemRecommendPrice() : header(MT_GAME_HIDDEN_QUERY_ITEM_RECOMMEND_PRICE) {}
		MessageHeader header;

		int plat_type;						//!< 平台
		int server_id;						//!< 服务器ID

		int role_id;						//!< 玩家uid

		ItemDataWrapper item_wrapper;		//!< 物品数据

		// 以下字段原样保留
		char is_from_pet;					//!< 是否宠物 [0:否  1:是]
		char reserve_ch;					//!< 保留字
		short reserve_sh;					//!< 保留字
		short index;						//!< 序号 （当为宠物时，是货物在宠物背包的序号，否则是货物在背包的序号）
		short column;						//!< 物品栏号 （仅当货物是道具时有效，表示货物在背包中的哪个物品栏）
	};

	struct HiddenGameQueryTargetGladiatorBeforeFightReq
	{
		HiddenGameQueryTargetGladiatorBeforeFightReq() : header(MT_HIDDEN_GAME_QUERY_TARGET_GLADIATOR_BEFORE_FIGHT_REQ) {}
		MessageHeader header;

		int role_id;
		int target_index;
		int is_sweep;		// 是否是扫荡
	};

	struct HiddenGameColosseumFightSysReport
	{
		HiddenGameColosseumFightSysReport() : header(MT_HIDDEN_GAME_COLOSSEUM_SYS_REPORT) {}
		MessageHeader header;

		int role_id;
		int target_uid;
		int is_win;
		int is_sweep;
		ColosseumFightStatistic stat;
	};

	struct GameHiddenColosseumSyncNewRankAfterSysReport
	{
		GameHiddenColosseumSyncNewRankAfterSysReport() : header(MT_GAME_HIDDEN_COLOSSEUM_SYNC_NEW_RANK_AFTER_SYS_REPORT) {}
		MessageHeader header;

		int role_id;
		int target_uid;
		int old_rank;
		int new_rank;
		char is_win;
		char is_in_pool;
		char target_is_in_pool;
		char is_sweep;		//是否是扫荡
	};

	enum HIDDEN_GAME_JIXING_GAOZHAO_REQ_TYPE
	{
		HIDDEN_GAME_JIXING_GAOZHAO_REQ_TYPE_INFO,
		HIDDEN_GAME_JIXING_GAOZHAO_REQ_TYPE_DRAW,
	};

	struct HiddenGameJiXingGaoZhaoReq
	{
		HiddenGameJiXingGaoZhaoReq() : header(MT_HIDDEN_GAME_JIXING_GAOZHAO_REQ) {}
		MessageHeader header;

		long long req_id;  //用来记log
		int role_id;
		int req_type;
		int rand_times;
		BitMap<MAX_HORCRUX_TIANCHENG_REWARD_GROUP_NUM> floor_bitmap;
	};

	struct GameCrossHotfix
	{
		GameCrossHotfix() : header(MT_GAME_CROSS_HOTFIX) {}
		MessageHeader header;

		char so_file_name[128];
		char symbol_str[512];
	};

	struct GameHiddenSyncSpecialInfoReq
	{
		GameHiddenSyncSpecialInfoReq() : header(MT_GAME_HIDDEN_SYNC_SPECIAL_INFO_REQ) {}
		MessageHeader header;

		int server_id;
	};

	struct HiddenGameSyncSpecialInfoRet
	{
		HiddenGameSyncSpecialInfoRet() : header(MT_HIDDEN_GAME_SYNC_SPECIAL_INFO_RET) {}
		MessageHeader header;

		unsigned int cross_open_server_timestamp;
		int hidden_server_idx;
	};

	//--------------情缘begin
	struct GameHiddenQingYuanCreate
	{
		GameHiddenQingYuanCreate() : header(MT_GAME_HIDDEN_QING_YUAN_CREATE) {}
		MessageHeader header;

		int item_type;
		CommonSaveQingYuanRoleInfoItem list[QING_YUAN_MAX_ROLE_NUM];
	};

	struct GameHiddenQingYuanLeave
	{
		GameHiddenQingYuanLeave() : header(MT_GAME_HIDDEN_QING_YUAN_LEAVE) {}
		MessageHeader header;

		long long qing_yuan_id;
	};

	struct GameHiddenQingYuanSingleLeave
	{
		GameHiddenQingYuanSingleLeave() : header(MT_GAME_HIDDEN_QING_YUAN_SINGLE_LEAVE) {}
		MessageHeader header;

		int role_id;
	};

	struct GameHiddenQingYuanForceLeave
	{
		GameHiddenQingYuanForceLeave() : header(MT_GAME_HIDDEN_QING_YUAN_FORCE_LEAVE) {}
		MessageHeader header;

		int role_id;
	};

	struct GameHiddenQingYuanCancelLeave
	{
		GameHiddenQingYuanCancelLeave() : header(MT_GAME_HIDDEN_QING_YUAN_CANCEL_LEAVE) {}
		MessageHeader header;

		int role_id;
	};

	struct HiddenGameQingYuanSyncInfo
	{
		HiddenGameQingYuanSyncInfo() : header(MT_HIDDEN_GAME_QING_YUAN_INFO_SYNC) { is_create = false; item_type = 0; }
		MessageHeader header;

		int sync_type;
		int is_create;
		int item_type;
		CommonSaveQingYuan qing_yuan_info;
	};

	struct GameHiddenQingYuanGiftGive
	{
		GameHiddenQingYuanGiftGive() : header(MT_GAME_HIDDEN_QING_YUAN_GIFT_GiVE) {}
		MessageHeader header;

		int role_id;
		int gift_type;
	};

	struct GameHiddenQingYuanGiftFetchReward
	{
		GameHiddenQingYuanGiftFetchReward() : header(MT_GAME_HIDDEN_QING_YUAN_GIFT_FETCH_REWARD) {}
		MessageHeader header;

		int role_id;
		int gift_type;
	};

	struct HiddenGameQingYuanGiftFetchRewardRet
	{
		HiddenGameQingYuanGiftFetchRewardRet() : header(MT_HIDDEN_GAME_QING_YUAN_GIFT_FETCH_REWARD_RET) {}
		MessageHeader header;

		int role_id;
		int gift_type;
	};

	struct GameHiddenQingYuanGiftFetchDayReward
	{
		GameHiddenQingYuanGiftFetchDayReward() : header(MT_GAME_HIDDEN_QING_YUAN_GIFT_FETCH_DAY_REWARD) {}
		MessageHeader header;

		int role_id;
	};

	struct HiddenGameQingYuanGiftFetchDayRewardRet
	{
		HiddenGameQingYuanGiftFetchDayRewardRet() : header(MT_HIDDEN_GAME_QING_YUAN_GIFT_FETCH_DAY_REWARD_RET) {}
		MessageHeader header;

		int role_id;
		int gift_type;
	};

	struct GameHiddenQingYuanSetSkillFlag
	{
		GameHiddenQingYuanSetSkillFlag() : header(MT_GAME_HIDDEN_QING_YUAN_SET_SKILL_FLAG) {}
		MessageHeader header;

		int role_id;
		unsigned long long skill_flag;
	};

	//--------------情缘end

	struct CrossHiddenFallenGodRankRewardRet
	{
		CrossHiddenFallenGodRankRewardRet() : header(MT_CROSS_HIDDEN_FALLEN_GOD_RANK_REWARD_RET) {}
		MessageHeader header;
		int count;
		PersonRankItem rank_list[CHIVALROUS_RANK_LIST_NUM];
	};

	struct HiddenGameQingYuanDismiss
	{
		HiddenGameQingYuanDismiss() : header(MT_HIDDEN_GAME_QING_YUAN_DISMISS) {}
		MessageHeader header;

		long long qing_yuan_id;
	};

	struct GameHiddenWeddingRoleReq
	{
		GameHiddenWeddingRoleReq() : header(MT_G_H_WEDDING_ROLE_REQ) {}
		MessageHeader header;

		int uid;
		int type;
		int param_1;
		int param_2;
	};
	

	//MT_H_G_WEDDING_INFO_SYNC,		//婚宴信息通知
	
	struct HiddenGameWeddingInfoSync
	{
		HiddenGameWeddingInfoSync() : header(MT_H_G_WEDDING_INFO_SYNC) {}
		MessageHeader header;

		int type;
		int param_1;
		int param_2;
		int param_3;

		enum Type
		{
			WEDDING_BEGIN,	//婚宴开启 P1:wedding_id
			WEDDING_END,	//婚宴结束 P1:wedding_id
		};
	};

	struct GameHiddenQingYuanCommonData
	{
		GameHiddenQingYuanCommonData() : header(MT_GAME_HIDDEN_QING_YUAN_COMMON_DATA) {}
		MessageHeader header;

		long long qingyuan_id;
		CommonSaveQingYuanCommonData common_data;
	};

	struct GameHiddenQingYuanLeiChongAddChongZhi
	{
		GameHiddenQingYuanLeiChongAddChongZhi() : header(MT_GAME_HIDDEN_QING_YUAN_LEICHONG_ADD_CHONGZHI) {}
		MessageHeader header;

		long long qingyuan_id;
		int add_chongzhi;
	};

	struct GameHiddenWorldArenaOperate
	{
		GameHiddenWorldArenaOperate() : header(MT_GAME_HIDDEN_WORLD_ARENA_OPER_TRANSFER) {}
		MessageHeader header;

		int plat_type;
		int uid;

		int oper_type;
		int param1;
		int param2;
	};

	struct HiddenGameWorldArenaGiveWinBoxReward
	{
		HiddenGameWorldArenaGiveWinBoxReward() : header(MT_HIDDEN_GAME_WORLD_ARENA_GIVE_WIN_BOX_REWARD) {}
		MessageHeader header;

		int uid;
		int sn;
	};

	struct HiddenGameWorldArenaGiveSignupReward
	{
		HiddenGameWorldArenaGiveSignupReward() : header(MT_HIDDEN_GAME_WORLD_ARENA_GIVE_SIGNUP_REWARD) {}
		MessageHeader header;

		int uid;
	};

	struct HiddenGameWorldArenaGiveDailyReward
	{
		HiddenGameWorldArenaGiveDailyReward() : header(MT_HIDDEN_GAME_WORLD_ARENA_GIVE_DAILY_REWARD) {}
		MessageHeader header;

		int uid;
	};

	struct GameCrossWorldTeamArenaQueryRankReq
	{
		GameCrossWorldTeamArenaQueryRankReq() : header(MT_GAME_CROSS_WORLD_TEAM_ARENA_QUERY_RANK_REQ) {}
		MessageHeader header;

		int server_id;
		int plat_type;
		long long team_unqiue_id;
		int send_uid;		//查询后发给该玩家
		int notice_type;
		int send_type;		//区分下发 0:战队详细信息 1:队伍特殊信息
		WorldTeamArenaQueryRankUserInfo rank_user_info[ACTIVITY_WORLD_TEAM_ARENA_TEAM_MEMBER_NUM];
	};

	struct CrossGameWorldTeamArenaQueryRankRet
	{
		CrossGameWorldTeamArenaQueryRankRet() : header(MT_CROSS_GAME_WORLD_TEAM_ARENA_QUERY_RANK_RET) {}
		MessageHeader header;

		long long team_unique_id;
		int send_uid;		//查询后发给该玩家
		int notice_type;
		int send_type;		//区分下发 0:战队详细信息 1:队伍特殊信息
		WorldTeamArenaQueryRankUserInfo rank_user_info[ACTIVITY_WORLD_TEAM_ARENA_TEAM_MEMBER_NUM];
	};

	struct GameHiddenRoleInfoChangeWorldTeamArenaSyn
	{
		GameHiddenRoleInfoChangeWorldTeamArenaSyn() : header(MT_GAME_HIDDEN_ROLE_INFO_CHANGE_WORLD_TEAM_ARENA_SYN) {}
		MessageHeader header;

		WorldTeamArenaUserInfo member_info;
	};

	struct HiddenGameWorldTeamArenaSynRoleTitleInfo
	{
		HiddenGameWorldTeamArenaSynRoleTitleInfo() : header(MT_HIDDEN_GAME_WORLD_TEAM_ARENA_SYN_ROLE_TITLE_INFO) {}
		MessageHeader header;

		int role_id;
		char reason[64];
		WorldTeamArenaRoleInfo info;
	};

	struct HiddenGameWorldTeamArenaSynActivityInfo
	{
		HiddenGameWorldTeamArenaSynActivityInfo() : header(MT_HIDDEN_GAME_WORLD_TEAM_ARENA_SYN_ACTIVITY_INFO) {}
		MessageHeader header;

		ActivityWorldTeamArenaParam activity_info;
	};

	struct GameHiddenWorldTeamArenaActivityInfoSynReq
	{
		GameHiddenWorldTeamArenaActivityInfoSynReq() : header(MT_GAME_HIDDEN_WORLD_TEAM_ARENA_ACITIVITY_INFO_SYN_REQ) {}
		MessageHeader header;

		int server_id;
	};

	struct GameHiddenWorldTeamArenaFinallyRankReq
	{
		GameHiddenWorldTeamArenaFinallyRankReq() : header(MT_GAME_HIDDEN_WORLD_TEAM_ARENA_FINALLY_RANK_REQ) {}
		MessageHeader header;

		int role_id;
		int hidden_server_idx;
	};

	struct GameHiddenSynLevelComplementInfo
	{
		GameHiddenSynLevelComplementInfo() : header(MT_GAME_HIDDEN_SYN_LEVEL_COMPLEMENT_INFO) {}
		MessageHeader header;

		int server_id;
		int average_level_by_level_complement;
	};


	struct GameHiddenQingYuanDuiDuiPengUpDataRankInfo
	{
		GameHiddenQingYuanDuiDuiPengUpDataRankInfo() : header(MT_GAME_HIDDEN_QINGYUAN_DUIDUIPENG_UPDATA_RANK) {}
		MessageHeader header;

		int role_id;
		GameName name;
		short jifen;
		short pass_times;
	};

	struct GameHiddenQingYuanDuiDuiPengRankReq
	{
		GameHiddenQingYuanDuiDuiPengRankReq() : header(MT_GAME_HIDDEN_QINGYUAN_DUIDUIPENG_RANK_REQ) {}
		MessageHeader header;

		int role_id;
	};

	struct HiddenGameQingYuanDuiDuiPengUpRankInfo
	{
		HiddenGameQingYuanDuiDuiPengUpRankInfo() : header(MT_HIDDEN_GAME_QINGYUAN_DUIDUIPENG_RANK_INFO) {}
		MessageHeader header;

		struct RoleData
		{
			int uid;
			GameName name;
			short jifen;
			short pass_times;
		};

		int role_id;
		int count;
		RoleData rank_list[100];
	};

	struct HiddenGameSyncHiddenOpenServerTime
	{
		HiddenGameSyncHiddenOpenServerTime() : header(MT_HIDDEN_GAME_SYNC_HIDDEN_OPEN_SERVER_TIME) {}
		MessageHeader header;

		unsigned int hidden_open_server_time;
	};

	struct GameHiddenSynGuildSpeedUpInfo
	{
		GameHiddenSynGuildSpeedUpInfo() : header(MT_GAME_HIDDEN_GUILD_SPEED_UP_INFO){}
		MessageHeader header;

		int guild_id;										// 家族id
		int user_id;
		int ret;
		unsigned int single_building_finish_timestamp;		// 建筑升级完成时间戳
	};

	struct HiddenGameSynGuildSpeedUpInfo
	{
		HiddenGameSynGuildSpeedUpInfo() : header(MT_HIDDEN_GAME_GUILD_SPEED_UP_INFO) {}
		MessageHeader header;

		int guild_id;					// 家族id
		int user_id;					
	};

	struct GameHiddenSynGuildBuildAllInfo
	{
		GameHiddenSynGuildBuildAllInfo() : header(MT_GAME_HIDDEN_GUILD_SYN_BUILD_ALL_INFO) {}
		MessageHeader header;

		int guild_id;		// 家族id 
		short build_level[GUILD_CONSTRUCT_BUILDING];		// [1-5],下标0-当前正在建造的建筑号
		unsigned int single_building_finish_timestamp;		// 建筑升级完成时间戳
		short quicken_building_count;						// 今日已开启加速次数
		short can_speed_up;									// 是否可加速
		GameName opener_name;								// 加速开启人的名字
		int opener_post;									// 开启人职位
	};

	struct SyncGuildBuildingNotifyInfo
	{
		SyncGuildBuildingNotifyInfo() : header(MT_SYNC_GUILD_BUILDING_NOTIFY_INFO) {}
		MessageHeader header;

		int guild_id;
		Protocol::SCGuildUseBuildMaterialInfo info;
	};


	struct GameHiddenGetUTAChampionRecord		//	主服 -> 隐藏服  获取天下第一比武[组队] 冠军记录手册
	{
		GameHiddenGetUTAChampionRecord() : header(MT_GAME_HIDDEN_GET_UTA_CHAMPION_RECORD) {}
		MessageHeader header;

		int uid;
		int begin_index;
		int end_index;
	};

	struct HiddenGameGiveLotteryToRole			// 隐藏服向原服  给玩家发放竞猜币
	{
		HiddenGameGiveLotteryToRole() : header(MT_HIDDEN_GAME_GIVE_LOTTERY_TO_ROLE) {}
		MessageHeader header;

		int uid;
		int lottery_id;
		int num;
		int reason_type;
	};

	struct HiddenGameUTABaseInfoSync
	{
		HiddenGameUTABaseInfoSync() : header(MT_HIDDEN_GAME_UTA_SYNC_BASE_INFO) {}
		MessageHeader header;

		short cur_status;
		short season_index;
		short round_idx;
		short reserve_sh;
		unsigned int next_battle_timestamp;
		unsigned int active_reward_begin_timestamp;
		unsigned int active_reward_end_timestamp;
		unsigned int season_end_timestamp;
	};

	struct GameHiddenUserRequestWatchBigCrossVideo
	{
		GameHiddenUserRequestWatchBigCrossVideo() : header(MT_GAME_HIDDEN_USER_REQUEST_BIGCROSS_VIDEO) {}
		MessageHeader header;

		int role_id;
		long long video_id;
	};

	struct GameHiddenUserLoginInOrigin
	{
		GameHiddenUserLoginInOrigin() : header(MT_GAME_HIDDEN_USER_LOGIN_IN_ORIGIN) {}
		MessageHeader header;

		int role_id;
	};

	struct GameHiddenQueryWorldTeamArenaOtherInfo
	{
		GameHiddenQueryWorldTeamArenaOtherInfo() : header(MT_GAME_HIDDEN_QUERY_WORLD_TEAM_ARENA_OTHER_INFO) {}
		MessageHeader header;

		int role_id;
	};
	struct GameHiddenGodBeastAdventReq
	{
		GameHiddenGodBeastAdventReq():header(MT_GAME_HIDDEN_GOD_BEAST_ADVENT_BUY_SERVER_LIMIT_ITEM_REQ){}
		MessageHeader header;
	
		int plat_type;
		int server_id;
		int uid;
		int seq;		// 道具索引
	};

	struct HiddenGameGodBeastAdventInfo
	{
		HiddenGameGodBeastAdventInfo() :header(MT_HIDDEN_GAME_GOD_BEAST_ADVENT_BUY_SERVER_LIMIT_ITEM_INFO) {}
		MessageHeader header;

		int uid;
		int ret;
		int seq;		// 道具索引
	};

	
	struct HiddenGameGodBeastAdventNumInfo
	{
		HiddenGameGodBeastAdventNumInfo() :header(MT_HIDDEN_GAME_GOD_BEAST_ADVENT_BUY_NUM_INFO) {}
		MessageHeader header;

		int num;
	};

	struct GameHiddenGodBeastAdventReceiveTradeMarketReq
	{
		GameHiddenGodBeastAdventReceiveTradeMarketReq() :header(MT_GAME_HIDDEN_GOD_BEAST_ADVENT_RECEIVE_TRADE_MARKET_REQ) {}
		MessageHeader header;

		int uid;
		int item_id;
		int exchange_group;
	};

	struct HiddenCrossUTAQueryTeamMemberRankReq
	{
		HiddenCrossUTAQueryTeamMemberRankReq() : header(MT_HIDDEN_CROSS_UTA_QUERY_TEAM_MEMBER_RANK_REQ) {}
		MessageHeader header;

		long long unique_team_id;
		int member_uid_list[UTA_TEAM_MEMBER_NUM];
		int member_profession_list[UTA_TEAM_MEMBER_NUM];
	};

	struct CrossHiddenUTAQueryTeamMemberRankResp
	{
		CrossHiddenUTAQueryTeamMemberRankResp() : header(MT_CROSS_HIDDEN_UTA_QUERY_TEAM_MEMBER_RANK_RESP) {}
		MessageHeader header;

		long long unique_team_id;

		int rank_prof[UTA_TEAM_MEMBER_NUM];			//!< 职业排名 -1代表未上榜
		int rank_capability[UTA_TEAM_MEMBER_NUM];	//!< 评分排名 -1代表未上榜
		int rank_level[UTA_TEAM_MEMBER_NUM];		//!< 等级排名 -1代表未上榜
	};
	struct GameHiddenHongMengTianJieReq
	{
		GameHiddenHongMengTianJieReq() : header(MT_GAME_HIDDEN_HONGMENG_TIANJIE_REQ) {}
		MessageHeader header;

		int role_uid;
		int req_type;
		int param;
	};

	struct HiddenGameServerCompetitionRankReq
	{
		HiddenGameServerCompetitionRankReq() : header(MT_HIDDEN_GAME_SERVER_COMPETITION_RANK_REQ) {}
		MessageHeader header;

		int role_uid;
	};

	struct GameHiddenServerCompetitionRankResp
	{
		GameHiddenServerCompetitionRankResp() : header(MT_GAME_HIDDEN_SERVER_COMPETITION_RANK_RESP) {}
		MessageHeader header;

		int role_uid;
		Protocol::SCRAServerCompetitionAllRankInfo msg;
	};

	struct GameHiddenSaltySweetBattleChangeReq
	{
		GameHiddenSaltySweetBattleChangeReq() : header(MT_GAME_HIDDEN_SALTY_SWEET_BATTLE_CHANGE_REQ) {}
		MessageHeader header;

		UniqueServerID unique_server_id;
		int pre_win_type;
		int salty_value;
		int sweet_value;
		int status_mod;
		int to_uid;
	};

	struct HiddenGameSaltySweetBattleVoteReq
	{
		HiddenGameSaltySweetBattleVoteReq() : header(MT_HIDDEN_GAME_SALTY_SWEET_BATTLE_VOTE_REQ) {}
		MessageHeader header;

		int role_uid;
		int add_type;
		int add_value;
	};

	struct HiddenGameTeamRoleRecruitSync
	{
		HiddenGameTeamRoleRecruitSync() : header(MT_HIDDEN_GAME_TEAM_ROLE_RECRUIT_SYNC) {}
		MessageHeader header;

		int team_id;
		int team_type;
		GameName leader_name;
		short limit_lv_low;
		short limit_lv_high;
		short cur_member_num;
		short max_member_num;
	};
	struct HiddenCrossMeiLiRankRewardReq
	{
		HiddenCrossMeiLiRankRewardReq() : header(MT_HIDDEN_CROSS_MEILI_RANK_REWARD) {}
		MessageHeader header;
	};

	struct CrossHiddenMeiLiRankRewardRet
	{
		CrossHiddenMeiLiRankRewardRet() : header(MT_CROSS_HIDDEN_MEILI_RANK_REWARD_RET) {}
		MessageHeader header;

		int count;
		PersonRankItem rank_list[CHIVALROUS_RANK_LIST_NUM];
	};

	struct HiddenGameFriendInitmacy
	{
		HiddenGameFriendInitmacy() : header(MT_HIDDEN_GAME_FRIEND_INITIMACY_ADD) {}
		MessageHeader header;

		int to_uid;										// 魅力值主人
		int add_val;									// 变更数值
	};

	struct GameHiddenFriendInitmacy
	{
		GameHiddenFriendInitmacy() : header(MT_GAME_HIDDEN_FRIEND_INITIMACY_ADD) {}
		MessageHeader header;

		int to_uid;										// 魅力值主人
		int add_val;									// 变更数值
	};

	struct HiddenGameQueryOriginServerRAStatus
	{
		HiddenGameQueryOriginServerRAStatus() : header(MT_HIDDEN_GAME_QUERY_ORIGIN_SERVER_RA_STATUS) {}
		MessageHeader header;

		int query_uid;
		int query_activity_type;
	};

	struct GameHiddenSendOriginServerRAStatus
	{
		GameHiddenSendOriginServerRAStatus() : header(MT_GAME_HIDDEN_SEND_ORIGIN_SERVER_RA_STATUS) {}
		MessageHeader header;

		int send_to_plat_type;
		int send_to_server_id;
		int open_type;
		short activity_type;
		short status;
		unsigned int next_status_switch_time;
		unsigned int param_1;
		unsigned int param_2;
	};

	struct GameHiddenQueryOfflineBattleDataForRobot
	{
		GameHiddenQueryOfflineBattleDataForRobot() : header(MT_GAME_HIDDEN_QUERY_OFFLINE_BATTLE_DATA_FOR_ROBOT) {}
		MessageHeader header;

		int team_id;
		int server_id;
		int level_min;
		int level_max;
		int best_level_min;
		int best_level_max;
		int leader_level;
		int exclude_uid_num;
		int exclude_uid_list[MAX_TEAM_MEMBER_NUM];
	};

	struct GameHiddenNotifyOfflineBattleDataForRobotResult
	{
		GameHiddenNotifyOfflineBattleDataForRobotResult() : header(MT_GAME_HIDDEN_NOTIFY_OFFLINE_BATTLE_DATA_FOR_ROBOT_RESULT) {}
		MessageHeader header;

		enum Result
		{
			SUCC = 0,
			FAIL = 1,
		};

		int uid;
		int result; 
	};

	struct GameHiddenRealRoleRobotLeaveTeamNotice
	{
		GameHiddenRealRoleRobotLeaveTeamNotice() : header(MT_GAME_HIDDEN_REAL_ROLE_ROBOT_LEAVE_TEAM_NOTICE) {}
		MessageHeader header;

		int uid;
	};

	struct GameHiddenReqSynKuaFuYingXiongLuData
	{
		GameHiddenReqSynKuaFuYingXiongLuData() : header(MT_GAME_HIDDEN_REQ_SYN_KUA_FU_YING_XIONG_LU_DATA) {}
		MessageHeader header;

		int server_id;
	};

	struct HiddenGameRetSynKuaFuYingXiongLuData
	{
		HiddenGameRetSynKuaFuYingXiongLuData() : hedaer(MT_HIDDEN_GAME_RET_SYN_KUA_FU_YING_XIONG_LU_DATA) {}
		MessageHeader hedaer;

		KuaFuYingXiongLuActivityParam data;
	};
	
	struct HiddenCrossReqKuaFuYingXiongLuRankEnd
	{
		HiddenCrossReqKuaFuYingXiongLuRankEnd() : header(MT_HIDDEN_CROSS_REQ_KUA_FU_YING_XIONG_LU_RANK_END) {}
		MessageHeader header;

		int kfyxl_rank_type;
		int person_rank_type;
		int rank_count;						//获取榜单数量
		long long activity_unique_id;		//添加标识传输,防止后续回传时活动重开多次结算
	};

	struct CrossHiddenRetKuaFuYingXiongLuRankEnd
	{
		CrossHiddenRetKuaFuYingXiongLuRankEnd() : hedaer(MT_CROSS_HIDDEN_RET_KUA_FU_YING_XIONG_LU_RANK_END) {}
		MessageHeader hedaer;

		int kfyxl_rank_type;
		int person_rank_type;
		long long activity_unique_id;		//添加标识传输,防止后续回传时活动重开多次结算
		int rank_uid_list[MAX_KUA_FU_YING_XIONG_LU_RANK_TYPE_SEQ_NUM];
	};

	struct GameHiddenHuanShouAnnInfoReq
	{
		GameHiddenHuanShouAnnInfoReq() : header(MT_GAME_HIDDEN_HUAN_SHOU_ANN_INFO_REQ) {}
		MessageHeader header;

		int role_uid;
	};

	struct GameHiddenHuanShouAnnInfoSync
	{
		GameHiddenHuanShouAnnInfoSync() : header(MT_GAME_HIDDEN_HUAN_SHOU_ANN_INFO_SYNC) {}
		MessageHeader header;

		HuanShouAnnouncementInfo ann_info;
	};

	struct HiddenGameHuanShouAnnInfoSync
	{
		HiddenGameHuanShouAnnInfoSync() : header(MT_HIDDEN_GAME_HUAN_SHOU_ANN_INFO_SYNC) {}
		MessageHeader header;

		HuanShouAnnouncementInfo ann_info;
	};
}
#pragma pack(pop)

//ADDMARK
namespace crossgameprotocal
{
	struct HiddenGameRetPlatformBattleRoleInfo : public IMessageSerializer, public IMessageDeserializer
	{
		virtual int GetMessageType() const;
		virtual bool Serialize(char * buffer, int buffer_size, int * out_length);
		virtual bool Deserialize(const char * data, int data_length, int * out_offset);

		HiddenGameRetPlatformBattleRoleInfo() : header(MT_HIDDEN_GAME_RET_PLATFORM_BATTLE_ROLE_INFO) {}
		MessageHeader	header;

		int type;
		std::vector<MsgPlatformBattleRoleInfoItemList> all_role_info;
	};

	struct HiddenGameSyncLogQuick : public IMessageSerializer, public IMessageDeserializer
	{
		HiddenGameSyncLogQuick() : header(MT_HIDDEN_GAME_SYNC_LOG_QUICK),
			n1(0LL), n2(0LL), n3(0LL), n4(0LL),
			n5(0LL), n6(0LL), n7(0LL), n8(0LL), n9(0LL) {}

		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_LL(n1)
				&& WRITE_LL(n2)
				&& WRITE_INT(z1.length())
				&& WRITE_STRN(z1.c_str(), z1.length())
				&& WRITE_INT(z2.length())
				&& WRITE_STRN(z2.c_str(), z2.length())
				&& WRITE_LL(n3)
				&& WRITE_LL(n4)
				&& WRITE_INT(z3.length())
				&& WRITE_STRN(z3.c_str(), z3.length())
				&& WRITE_INT(z4.length())
				&& WRITE_STRN(z4.c_str(), z4.length())
				&& WRITE_LL(n5)
				&& WRITE_LL(n6)
				&& WRITE_LL(n7)
				&& WRITE_LL(n8)
				&& WRITE_LL(n9);

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			int z1_len = 0;
			int z2_len = 0;
			int z3_len = 0;
			int z4_len = 0;

			bool ret = READ_BEGIN(header.msg_type)
				&& READ_LL(n1)
				&& READ_LL(n2)
				&& READ_INT(z1_len)
				&& READ_STRING(z1, z1_len)
				&& READ_INT(z2_len)
				&& READ_STRING(z2, z2_len)
				&& READ_LL(n3)
				&& READ_LL(n4)
				&& READ_INT(z3_len)
				&& READ_STRING(z3, z3_len)
				&& READ_INT(z4_len)
				&& READ_STRING(z4, z4_len)
				&& READ_LL(n5)
				&& READ_LL(n6)
				&& READ_LL(n7)
				&& READ_LL(n8)
				&& READ_LL(n9);

			return ret;
		}

		long long n1;
		long long n2;
		std::string z1;
		std::string z2;
		long long n3;
		long long n4;
		std::string z3;
		std::string z4;
		long long n5;
		long long n6;
		long long n7;
		long long n8;
		long long n9;
	};


	struct GameHiddenShenShouJiangLinDrawRet : public IMessageSerializer2,public IMessageDeserializer2
	{
		GameHiddenShenShouJiangLinDrawRet() : header(MT_GAME_HIDDEN_SHEN_SHOU_JIANG_LIN_DRAW_RET) {}
		MessageHeader header;

		long long req_id;
		int role_id;
		int seq;
		int bao_di_id;
		int gold_num;
		std::vector<ItemConfigData> items;
		std::string mail_str;
		std::string broadcast_str;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(TLVSerializer2 * outstream) const
		{
			bool ret = true;

			TLVSerializer2 & out = *outstream;
			ret = ret && out.Push(header.msg_type);
			ret = ret && out.Push(req_id);
			ret = ret && out.Push(role_id);
			ret = ret && out.Push(seq);
			ret = ret && out.Push(bao_di_id);
			ret = ret && out.Push(gold_num);
			int s = (int)items.size();
			ret = ret && out.Push(s);
			for (int i = 0; ret && i < s; ++i)
			{
				const ItemConfigData & curr = items[i];

				ret = ret && out.Push(curr.item_id);
				ret = ret && out.Push(curr.is_bind);
				ret = ret && out.Push(curr.num);
			}

			ret = ret && out.Push(broadcast_str);
			ret = ret && out.Push(mail_str);

			return ret;
		}

		virtual bool Unserialize(TLVUnserializer2 & instream)
		{
			bool ret = true;

			ret = ret && instream.Pop(&header.msg_type);
			ret = ret && instream.Pop(&req_id);
			ret = ret && instream.Pop(&role_id);
			ret = ret && instream.Pop(&seq);
			ret = ret && instream.Pop(&bao_di_id);
			ret = ret && instream.Pop(&gold_num);
			int s = 0;
			ret = ret && instream.Pop(&s);
			for (int i = 0; ret && i < s; ++i)
			{
				ItemConfigData  curr;
				ret = ret && instream.Pop(&curr.item_id);
				ret = ret && instream.Pop(&curr.is_bind);
				ret = ret && instream.Pop(&curr.num);
				items.push_back(curr);
			}
			ret = ret && instream.Pop(&broadcast_str);
			ret = ret && instream.Pop(&mail_str);

			return ret;
		}
	};

	struct HiddenGameSyncRoleBattleData : public IMessageDeserializer, public IMessageSerializer
	{
		enum BUSINESS_TYPE
		{
			BUSINESS_TYPE_COLOSSEUM = 0,
			BUSINESS_TYPE_MAX
		};

		HiddenGameSyncRoleBattleData() : header(MT_HIDDEN_GAME_SYNC_ROLE_BATTLE_DATA) {}
		MessageHeader header;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}		

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT(role_id)
				&& WRITE_INT(business_type)
				&& role_battle_data.Serialize(buffer, buffer_size, out_length);
			
			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_INT(role_id)
				&& READ_INT(business_type)
				&& role_battle_data.Deserialize(data, data_length, out_offset);

			return ret;
		}

		int role_id;
		int business_type;
		RoleBattleData role_battle_data;
	};

	struct GameHiddenQeuryTargetGladiatorBeforeFightResp : public IMessageDeserializer, public IMessageSerializer
	{
		GameHiddenQeuryTargetGladiatorBeforeFightResp() : header(MT_GAME_HIDDEN_QUERY_TARGET_GLADIATOR_BEFORE_FIGHT_RESP) {}
		MessageHeader header;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT(role_id)
				&& WRITE_INT(target_index)
				&& WRITE_INT(target_uid)
				&& WRITE_INT(is_sweep)
				&& WRITE_INT(user_total_capability)
				&& WRITE_INT(target_total_capability)
				&& target_battle_data.Serialize(buffer, buffer_size, out_length);

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_INT(role_id)
				&& READ_INT(target_index)
				&& READ_INT(target_uid)
				&& READ_INT(is_sweep)
				&& READ_INT(user_total_capability)
				&& READ_INT(target_total_capability)
				&& target_battle_data.Deserialize(data, data_length, out_offset);

			return ret;
		}

		int role_id;
		int target_index;
		int target_uid;
		int is_sweep;
		int user_total_capability;
		int target_total_capability;
		RoleBattleData target_battle_data;
	};

	struct GameHiddenHorcruxTianChengDrawRet : public IMessageSerializer2, public IMessageDeserializer2
	{
		GameHiddenHorcruxTianChengDrawRet() : header(MT_GAME_HIDDEN_HORCRUX_TIAN_CHENG_DRAW_RET) {}
		MessageHeader header;

		long long req_id;
		int role_id;
		int seq;
		short is_jackpot;
		short money_type;
		int reward_num;
		int floor_back;
		std::vector<ItemConfigData> items;
		std::string broadcast_str;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(TLVSerializer2 * outstream) const
		{
			bool ret = true;

			TLVSerializer2 & out = *outstream;
			ret = ret && out.Push(header.msg_type);
			ret = ret && out.Push(req_id);
			ret = ret && out.Push(role_id);
			ret = ret && out.Push(seq);
			ret = ret && out.Push(is_jackpot);
			ret = ret && out.Push(money_type);
			ret = ret && out.Push(reward_num);
			ret = ret && out.Push(floor_back);
			int s = (int)items.size();
			ret = ret && out.Push(s);
			for (int i = 0; ret && i < s; ++i)
			{
				const ItemConfigData & curr = items[i];

				ret = ret && out.Push(curr.item_id);
				ret = ret && out.Push(curr.is_bind);
				ret = ret && out.Push(curr.num);
			}

			ret = ret && out.Push(broadcast_str);

			return ret;
		}

		virtual bool Unserialize(TLVUnserializer2 & instream)
		{
			bool ret = true;

			ret = ret && instream.Pop(&header.msg_type);
			ret = ret && instream.Pop(&req_id);
			ret = ret && instream.Pop(&role_id);
			ret = ret && instream.Pop(&seq);
			ret = ret && instream.Pop(&is_jackpot);
			ret = ret && instream.Pop(&money_type);
			ret = ret && instream.Pop(&reward_num);
			ret = ret && instream.Pop(&floor_back);
			int s = 0;
			ret = ret && instream.Pop(&s);
			for (int i = 0; ret && i < s; ++i)
			{
				ItemConfigData  curr;
				ret = ret && instream.Pop(&curr.item_id);
				ret = ret && instream.Pop(&curr.is_bind);
				ret = ret && instream.Pop(&curr.num);
				items.push_back(curr);
			}
			ret = ret && instream.Pop(&broadcast_str);

			return ret;
		}
	};

	struct GameHiddenJiXingGaoZhaoDrawRet : public IMessageSerializer2, public IMessageDeserializer2
	{
		GameHiddenJiXingGaoZhaoDrawRet() : header(MT_GAME_HIDDEN_JIXING_GAOZHAO_DRAW_RET) {}
		MessageHeader header;

		long long req_id;
		int role_id;
		int seq;
		short is_jackpot;
		short reward_type;
		int reward_num;
		int floor_back;
		std::vector<ItemConfigData> items;
		std::string broadcast_str;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(TLVSerializer2 * outstream) const
		{
			bool ret = true;

			TLVSerializer2 & out = *outstream;
			ret = ret && out.Push(header.msg_type);
			ret = ret && out.Push(req_id);
			ret = ret && out.Push(role_id);
			ret = ret && out.Push(seq);
			ret = ret && out.Push(is_jackpot);
			ret = ret && out.Push(reward_type);
			ret = ret && out.Push(reward_num);
			ret = ret && out.Push(floor_back);
			int s = (int)items.size();
			ret = ret && out.Push(s);
			for (int i = 0; ret && i < s; ++i)
			{
				const ItemConfigData & curr = items[i];

				ret = ret && out.Push(curr.item_id);
				ret = ret && out.Push(curr.is_bind);
				ret = ret && out.Push(curr.num);
			}

			ret = ret && out.Push(broadcast_str);

			return ret;
		}

		virtual bool Unserialize(TLVUnserializer2 & instream)
		{
			bool ret = true;

			ret = ret && instream.Pop(&header.msg_type);
			ret = ret && instream.Pop(&req_id);
			ret = ret && instream.Pop(&role_id);
			ret = ret && instream.Pop(&seq);
			ret = ret && instream.Pop(&is_jackpot);
			ret = ret && instream.Pop(&reward_type);
			ret = ret && instream.Pop(&reward_num);
			ret = ret && instream.Pop(&floor_back);
			int s = 0;
			ret = ret && instream.Pop(&s);
			for (int i = 0; ret && i < s; ++i)
			{
				ItemConfigData  curr;
				ret = ret && instream.Pop(&curr.item_id);
				ret = ret && instream.Pop(&curr.is_bind);
				ret = ret && instream.Pop(&curr.num);
				items.push_back(curr);
			}
			ret = ret && instream.Pop(&broadcast_str);

			return ret;
		}
	};

	struct GameHiddenWorldArenaSignup : public IMessageDeserializer, public IMessageSerializer
	{
		GameHiddenWorldArenaSignup() : header(MT_GAME_HIDDEN_WORLD_ARENA_SIGNUP) {}
		MessageHeader header;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& signup_info.Serialize(buffer, buffer_size, out_length)
				&& role_battle_data.Serialize(buffer, buffer_size, out_length);

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			bool ret = READ_BEGIN(header.msg_type)
				&& signup_info.Deserialize(data, data_length, out_offset)
				&& role_battle_data.Deserialize(data, data_length, out_offset);

			return ret;
		}

		WorldArenaSignUpInfo signup_info;
		RoleBattleData role_battle_data;
	};

	struct GameHiddenWorldArenaUpdateSignupInfo : public IMessageDeserializer, public IMessageSerializer
	{
		GameHiddenWorldArenaUpdateSignupInfo() : header(MT_GAME_HIDDEN_WORLD_ARENA_UPDATE_SIGNUP_INFO) {}
		MessageHeader header;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& signup_info.Serialize(buffer, buffer_size, out_length);

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			bool ret = READ_BEGIN(header.msg_type)
				&& signup_info.Deserialize(data, data_length, out_offset);

			return ret;
		}

		WorldArenaSignUpInfo signup_info;
	};

	struct HiddenGameUTAGiveItemsToUser : public IMessageDeserializer, public IMessageSerializer
	{
		HiddenGameUTAGiveItemsToUser() : header(MT_HIDDEN_GAME_UTA_GIVE_ITEMS_TO_USER) {}
		MessageHeader header;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT(role_id)
				&& WRITE_SHORT(put_reason_type)
				&& WRITE_SHORT(only_send_mail)
				&& WRITE_INT((int)itemlist.size())
				&& WRITE_INT(mail_subject.length())
				&& WRITE_INT(mail_content.length())
				&& WRITE_STRN(mail_subject.c_str(), mail_subject.length())
				&& WRITE_STRN(mail_content.c_str(), mail_content.length());
			if (!ret) return false;

			for (size_t i = 0; i < itemlist.size(); ++i)
			{
				ret = WRITE_USHORT(itemlist[i].item_id)
					&& WRITE_INT(itemlist[i].num)
					&& WRITE_SHORT(itemlist[i].is_bind)
					&& WRITE_SHORT(itemlist[i].has_param)
					&& WRITE_UINT(itemlist[i].invalid_time);
				if (!ret) return false;

				if (itemlist[i].has_param > 0)
				{
					ret = WRITE_STRN(itemlist[i].param_data, sizeof(itemlist[i].param_data));
					if (!ret) return false;
				}
			}

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			int subject_len = 0;
			int content_len = 0;
			int itemlist_size = 0;
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_INT(role_id)
				&& READ_SHORT(put_reason_type)
				&& READ_SHORT(only_send_mail)
				&& READ_INT(itemlist_size)
				&& READ_INT(subject_len)
				&& READ_INT(content_len);
			if (!ret) return false;
			if (subject_len < 0 || subject_len >= MAX_MSG_SIZE) return false;
			if (content_len < 0 || content_len >= MAX_MSG_SIZE) return false;

			ret = READ_STRING(mail_subject, subject_len)
				&& READ_STRING(mail_content, content_len);
			if (!ret) return false;
		
			for (int i = 0; i < itemlist_size; ++i)
			{
				ItemDataWrapper wrapper;
				ret = READ_USHORT(wrapper.item_id)
					&& READ_INT(wrapper.num)
					&& READ_SHORT(wrapper.is_bind)
					&& READ_SHORT(wrapper.has_param)
					&& READ_UINT(wrapper.invalid_time);
				if (!ret) return false;

				if (wrapper.has_param > 0)
				{
					ret = READ_STRN(wrapper.param_data, sizeof(wrapper.param_data), sizeof(wrapper.param_data));
					if (!ret) return false;
				}

				itemlist.push_back(wrapper);
			}

			return ret;
		}

		int role_id;
		short put_reason_type;
		short only_send_mail;
		std::string mail_subject;
		std::string mail_content;
		std::vector<ItemDataWrapper> itemlist;
	};

	struct MsgScheduleInfo
	{
		MsgScheduleInfo() : schedule_id(0), is_main_match(0), is_win_group(0),
			atk_team_unique_id(0LL), atk_plat_type(0), atk_hidden_server_idx(0),
			def_team_unique_id(0LL), def_plat_type(0), def_hidden_server_idx(0)
		{
			memset(atk_team_name, 0, sizeof(GameName));
			memset(def_team_name, 0, sizeof(GameName));
			memset(atk_member_uid_list, 0, sizeof(atk_member_uid_list));
			memset(def_member_uid_list, 0, sizeof(def_member_uid_list));
		}

		long long schedule_id;
		short is_main_match;
		short is_win_group;

		long long atk_team_unique_id;
		int atk_plat_type;
		int atk_hidden_server_idx;
		GameName atk_team_name;
		int atk_member_uid_list[UTA_TEAM_MEMBER_NUM];

		long long def_team_unique_id;
		int def_plat_type;
		int def_hidden_server_idx;
		GameName def_team_name;
		int def_member_uid_list[UTA_TEAM_MEMBER_NUM];
	};

	struct HiddenGameUTASyncSchedules : public IMessageDeserializer, public IMessageSerializer
	{
		HiddenGameUTASyncSchedules() : header(MT_HIDDEN_GAME_UTA_SYNC_SCHEDULES) {}
		MessageHeader header;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT(cur_status)
				&& WRITE_INT(round_idx)
				&& WRITE_INT((int)schedule_list.size());
			if (!ret) return false;

			for (size_t i = 0; i < schedule_list.size(); ++i)
			{
				ret = WRITE_LL(schedule_list[i].schedule_id)
					&& WRITE_SHORT(schedule_list[i].is_main_match)
					&& WRITE_SHORT(schedule_list[i].is_win_group)
					&& WRITE_LL(schedule_list[i].atk_team_unique_id)
					&& WRITE_INT(schedule_list[i].atk_plat_type)
					&& WRITE_INT(schedule_list[i].atk_hidden_server_idx)
					&& WRITE_STRN(schedule_list[i].atk_team_name, sizeof(schedule_list[i].atk_team_name))
					&& WRITE_LL(schedule_list[i].def_team_unique_id)
					&& WRITE_INT(schedule_list[i].def_plat_type)
					&& WRITE_INT(schedule_list[i].def_hidden_server_idx)
					&& WRITE_STRN(schedule_list[i].def_team_name, sizeof(schedule_list[i].def_team_name));
				if (!ret) return false;

				for (int k = 0; k < ARRAY_ITEM_COUNT(schedule_list[i].atk_member_uid_list); ++k)
				{
					ret = WRITE_INT(schedule_list[i].atk_member_uid_list[k]);
					if (!ret) return false;
				}
				for (int k = 0; k < ARRAY_ITEM_COUNT(schedule_list[i].def_member_uid_list); ++k)
				{
					ret = WRITE_INT(schedule_list[i].def_member_uid_list[k]);
					if (!ret) return false;
				}
			}

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			int schdule_list_size = 0;
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_INT(cur_status)
				&& READ_INT(round_idx)
				&& READ_INT(schdule_list_size);
			if (!ret) return false;

			schedule_list.reserve(schdule_list_size);
			for (int i = 0; i < schdule_list_size; ++i)
			{
				MsgScheduleInfo schedule;
				ret = READ_LL(schedule.schedule_id)
					&& READ_SHORT(schedule.is_main_match)
					&& READ_SHORT(schedule.is_win_group)
					&& READ_LL(schedule.atk_team_unique_id)
					&& READ_INT(schedule.atk_plat_type)
					&& READ_INT(schedule.atk_hidden_server_idx)
					&& READ_STRN(schedule.atk_team_name, sizeof(schedule.atk_team_name), sizeof(schedule.atk_team_name))
					&& READ_LL(schedule.def_team_unique_id)
					&& READ_INT(schedule.def_plat_type)
					&& READ_INT(schedule.def_hidden_server_idx)
					&& READ_STRN(schedule.def_team_name, sizeof(schedule.def_team_name), sizeof(schedule.def_team_name));
				if (!ret) return false;

				for (int k = 0; k < ARRAY_ITEM_COUNT(schedule.atk_member_uid_list); ++k)
				{
					ret = READ_INT(schedule.atk_member_uid_list[k]);
					if (!ret) return false;
				}
				for (int k = 0; k < ARRAY_ITEM_COUNT(schedule.def_member_uid_list); ++k)
				{
					ret = READ_INT(schedule.def_member_uid_list[k]);
					if (!ret) return false;
				}
				schedule_list.push_back(schedule);
			}

			return ret;
		}

		int cur_status;
		int round_idx;
		std::vector<MsgScheduleInfo> schedule_list;
	};

	struct HiddenGameUTAGiveItemsToServer : public IMessageDeserializer, public IMessageSerializer
	{
		HiddenGameUTAGiveItemsToServer() : header(MT_HIDDEN_GAME_UTA_GIVE_SERVER_ITEMS) {}
		MessageHeader header;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT((int)itemlist.size())
				&& WRITE_INT(mail_subject.length())
				&& WRITE_INT(mail_content.length())
				&& WRITE_STRN(mail_subject.c_str(), mail_subject.length())
				&& WRITE_STRN(mail_content.c_str(), mail_content.length());
			if (!ret) return false;

			for (size_t i = 0; i < itemlist.size(); ++i)
			{
				ret = WRITE_USHORT(itemlist[i].item_id)
					&& WRITE_INT(itemlist[i].num)
					&& WRITE_SHORT(itemlist[i].is_bind)
					&& WRITE_SHORT(itemlist[i].has_param)
					&& WRITE_UINT(itemlist[i].invalid_time);
				if (!ret) return false;

				if (itemlist[i].has_param > 0)
				{
					ret = WRITE_STRN(itemlist[i].param_data, sizeof(itemlist[i].param_data));
					if (!ret) return false;
				}
			}

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			int subject_len = 0;
			int content_len = 0;
			int itemlist_size = 0;
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_INT(itemlist_size)
				&& READ_INT(subject_len)
				&& READ_INT(content_len);
			if (!ret) return false;
			if (subject_len < 0 || subject_len >= MAX_MSG_SIZE) return false;
			if (content_len < 0 || content_len >= MAX_MSG_SIZE) return false;

			ret = READ_STRING(mail_subject, subject_len)
				&& READ_STRING(mail_content, content_len);
			if (!ret) return false;

			for (int i = 0; i < itemlist_size; ++i)
			{
				ItemDataWrapper wrapper;
				ret = READ_USHORT(wrapper.item_id)
					&& READ_INT(wrapper.num)
					&& READ_SHORT(wrapper.is_bind)
					&& READ_SHORT(wrapper.has_param)
					&& READ_UINT(wrapper.invalid_time);
				if (!ret) return false;

				if (wrapper.has_param > 0)
				{
					ret = READ_STRN(wrapper.param_data, sizeof(wrapper.param_data), sizeof(wrapper.param_data));
					if (!ret) return false;
				}
				itemlist.push_back(wrapper);
			}

			return ret;
		}

		std::vector<ItemDataWrapper> itemlist;
		std::string mail_subject;
		std::string mail_content;
	};

	struct GameHiddenSyncPetTopRankInfo : public IMessageDeserializer, public IMessageSerializer
	{
		GameHiddenSyncPetTopRankInfo() : header(MT_GAME_HIDDEN_SYNC_PET_TOP_RANK_LIST) {}
		MessageHeader header;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		struct RankInfo
		{
			int owner_uid;
			int pet_id;
			long long pet_unique_id;
		};

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT((int)rank_list.size())
				&& WRITE_INT(server_id);
			if (!ret) return false;

			for (size_t i = 0; i < rank_list.size(); ++i)
			{
				ret = WRITE_INT(rank_list[i].owner_uid)
					&& WRITE_INT(rank_list[i].pet_id)
					&& WRITE_LL(rank_list[i].pet_unique_id);
				if (!ret) return false;
			}

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			int list_size = 0;
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_INT(list_size)
				&& READ_INT(server_id);
			if (!ret) return false;

			for (int i = 0; i < list_size; ++i)
			{
				RankInfo rankinfo;
				ret = READ_INT(rankinfo.owner_uid)
					&& READ_INT(rankinfo.pet_id)
					&& READ_LL(rankinfo.pet_unique_id);
				if (!ret) return false;

				rank_list.push_back(rankinfo);
			}

			return ret;
		}

		int server_id;
		std::vector<RankInfo> rank_list;
	};

	struct GameHiddenSyncPartnerTopRankInfo : public IMessageDeserializer, public IMessageSerializer
	{
		GameHiddenSyncPartnerTopRankInfo() : header(MT_GAME_HIDDEN_SYNC_PARTNER_TOP_RANK_LIST) {}
		MessageHeader header;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		struct RankInfo
		{
			int owner_uid;
			int partner_id;
		};

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT((int)rank_list.size())
				&& WRITE_INT(server_id);
			if (!ret) return false;

			for (size_t i = 0; i < rank_list.size(); ++i)
			{
				ret = WRITE_INT(rank_list[i].owner_uid)
					&& WRITE_INT(rank_list[i].partner_id);
				if (!ret) return false;
			}

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			int list_size = 0;
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_INT(list_size)
				&& READ_INT(server_id);
			if (!ret) return false;

			for (int i = 0; i < list_size; ++i)
			{
				RankInfo rankinfo;
				ret = READ_INT(rankinfo.owner_uid)
					&& READ_INT(rankinfo.partner_id);
				if (!ret) return false;

				rank_list.push_back(rankinfo);
			}

			return ret;
		}

		int server_id;
		std::vector<RankInfo> rank_list;
	};


	struct GameHiddenClearQueryInfo		//由本服发送请求玩家信息到跨服
	{
		GameHiddenClearQueryInfo() : header(MT_GAME_HIDDEN_CLEAN_QUERY_INFO) {}
		MessageHeader	header;

		UniqueUserID clear_target_uid;
	};

	struct HiddenGameClearQueryInfo		//由本服发送请求玩家信息到跨服
	{
		HiddenGameClearQueryInfo() : header(MT_HIDDEN_GAME_CLEAN_QUERY_INFO) {}
		MessageHeader	header;

		UniqueUserID clear_target_uid;
	};

	struct GameHiddenCmdToRole
	{
		GameHiddenCmdToRole() : header(MT_GAME_HIDDEN_CMD_TO_ROLE) {}
		MessageHeader header;

		int plat_type;
		int server_id;
		int role_id;
		unsigned int effect_type;
		int effect_param;
		int effect_param1;
	};

	struct HiddenGameCmdToRole
	{
		HiddenGameCmdToRole() : header(MT_HIDDEN_GAME_CMD_TO_ROLE) {}
		MessageHeader header;

		int role_id;
		unsigned int effect_type;
		int effect_param;
		int effect_param1;
	};
	struct HiddenGameQueryOfflineBattleDataForRobot : public IMessageDeserializer, public IMessageSerializer
	{
		HiddenGameQueryOfflineBattleDataForRobot() : header(MT_HIDDEN_GAME_QUERY_OFFLINE_BATTLE_DATA_FOR_ROBOT){}
		MessageHeader header;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT(team_id)
				&& WRITE_INT(uid)
				&& rbd.Serialize(buffer, buffer_size, out_length);

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_INT(team_id)
				&& READ_INT(uid)
				&& rbd.Deserialize(data, data_length, out_offset);

			return ret;
		}

		int team_id;
		int uid;
		RoleBattleData rbd;
	};

	struct GameHiddenSyncOfflineBattleData : public IMessageDeserializer, public IMessageSerializer
	{
		GameHiddenSyncOfflineBattleData() : header(MT_GAME_HIDDEN_SYNC_OFFLINE_BATTLE_DATA) {}
		MessageHeader header;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT(uid)
				&& rbd.Serialize(buffer, buffer_size, out_length);

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_INT(uid)
				&& rbd.Deserialize(data, data_length, out_offset);

			return ret;
		}

		int uid;
		RoleBattleData rbd;
	};

	struct HiddenGameQueryBattleDataForOfflineBattleDataSystem : public IMessageDeserializer, public IMessageSerializer
	{
		HiddenGameQueryBattleDataForOfflineBattleDataSystem() : header(MT_HIDDEN_GAME_QUERY_BATTLE_DATA_FOR_OFFLINE_BATTLE_DATA_SYSTEM) {}
		MessageHeader header;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			int count = (int)exclude_uid_set.size();
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT(count);

			for (std::set<int>::iterator it = exclude_uid_set.begin(); it != exclude_uid_set.end(); ++it)
			{
				int uid = *it;
				ret =  ret && WRITE_INT(uid);
			}

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			int count = 0;
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_INT(count);

			for (int i = 0; i < count; ++i)
			{
				int uid = 0;
				ret = ret && READ_INT(uid);
				exclude_uid_set.insert(uid);
			}

			return ret;
		}

		std::set<int> exclude_uid_set;
	};

	struct HiddenGameAddSingleChatRecord : public IMessageDeserializer, public IMessageSerializer
	{
		HiddenGameAddSingleChatRecord() : header(MT_HIDDEN_GAME_ADD_SINGLE_CHAT_RECORD) {}
		MessageHeader header;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT(uid)
				&& WRITE_INT(chat_with_uid)
				&& WRITE_INT(msg_length)
				&& WRITE_STRN(msg, msg_length);

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_INT(uid)
				&& READ_INT(chat_with_uid)
				&& READ_INT(msg_length)
				&& READ_STRN(msg, msg_length, sizeof(msg));

			return ret;
		}

		int uid;
		int chat_with_uid;
		int msg_length;
		char msg[4096];
	};

	struct HiddenGameQueryUserChatRecord : public IMessageDeserializer, public IMessageSerializer
	{
		MessageHeader header;
		HiddenGameQueryUserChatRecord() : header(MT_HIDDEN_GAME_USER_QUERY_CHAT_RECORD) {}

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			int count = (int)chat_with_uid_set.size();
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT(uid)
				&& WRITE_INT(count);

			for (std::set<int>::iterator it = chat_with_uid_set.begin(); it != chat_with_uid_set.end(); ++it)
			{
				int uid = *it;
				ret = ret && WRITE_INT(uid);
			}

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			int count = 0;
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_INT(uid)
				&& READ_INT(count);

			for (int i = 0; i < count; ++i)
			{
				int uid = 0;
				ret = ret && READ_INT(uid);
				chat_with_uid_set.insert(uid);
			}

			return ret;
		}

		int uid;
		std::set<int> chat_with_uid_set;
	};
}

//ADDMARK

#endif	// __CROSS_GAME_PROTOCAL_H__


