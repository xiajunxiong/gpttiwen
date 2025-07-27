/******************************************************************************************
FileName: crossdef.hpp
Author: dreamwang
Description: cross相关定义 
Version: 1.0
History: 
<author>    <time>					<version >    <desc>
dreamwang   2011/08/08/ 09:04:05     1.0           创建
*****************************************************************************************/

#ifndef __CROSS_DEF_HPP__
#define __CROSS_DEF_HPP__

#include "servercommon.h"
#include "gameworld/gameworld/gamedef.h"

static const int CROSS_CONNECT_ASYN_TIME_OUT_MS = 10000;		// 异步连接cross超时限制 毫秒

static const int CROSS_PROTOCAL_VERSION = 1;					// cross通讯协议版本号

static const int CROSS_USER_DB_INDEX = 2046;					// 跨服玩家固定db_index

static const int WAIT_SCENE_MAX = 16;							// 一个跨服活动等待场景最大数

static const int CROSS_MELEE_RESULT_KEEP_MAX_NUM = 4;
//-------------------------------------------------------------------------------------
//跨服个人排行榜
static const int RANK_CROSS_SERVER_MAX = 40;				 // 跨服服务器榜单数量限制
//红包
static const int HONGBAO_CROSS_SERVER_MAX = 32;				 // 红包跨服服务器数量限制

enum CROSS_AUTO_JION_TYPE
{
	CROSS_AUTO_JION_TYPE_INVALID = 0,
	CROSS_AUTO_JION_TYPE_ACTIVITY,
	CROSS_AUTO_JION_TYPE_FB,
};

struct CrossData_Business
{
	enum BUSINESS_TYPE
	{
		BUSINESS_TYPE_INVALID = 0,
		BUSINESS_TYPE_CREATE_CROSS_TEAM = 1,
		BUSINESS_TYPE_TEAM_SWITCH_TO_CROSS = 2,
		BUSINESS_TYPE_JOIN_CROSS_TEAM = 3,
		BUSINESS_TYPE_TEAM_GO_TO_HIDDEN = 4,
		BUSINESS_TYPE_ROLE_GO_TO_HIDDEN = 5,
		BUSINESS_TYPE_ORIGIN_INVITE_CROSS = 6,
		BUSINESS_TYPE_TEAM_SWITCH_TO_CROSS_AND_INVITE = 7,
		BUSINESS_TYPE_ENTER_WORLD_ARENA_BATTLE_SCENE = 8,
		BUSINESS_TYPE_ENTER_UTA_BATTLE_SCENE = 9,
		BUSINESS_TYPE_TEAM_ENTER_UTA_BATTLE_SCENE = 10,
		BUSINESS_TYPE_TEAM_ENTER_HONG_MENG_TIAN_JIE = 11,
	};

	CrossData_Business() : cross_business_type(BUSINESS_TYPE_INVALID), team_type(0), 
		level_limit_low(0), level_limit_high(0), join_cross_team_id(0)
	{
		this->Reset();
	}

	void Reset()
	{
		cross_business_type = BUSINESS_TYPE_INVALID;
		team_type = 0;
		is_match = 0;
		allown_red_join = 1;
		level_limit_low = 0;
		level_limit_high = 0;

		memset(teammate_list, 0, sizeof(teammate_list));
		join_cross_team_id = 0;
		
		auto_join_type = CROSS_AUTO_JION_TYPE_INVALID;
		auto_join_type_param1 = 0;		
		param1 = 0;
		param2 = 0;

		team_invite_target_uid = 0;
	}

	int cross_business_type;

	int origin_team_index;
	short team_type;
	char is_match;
	char allown_red_join;
	short level_limit_low;
	short level_limit_high;
	
	int teammate_list[4];	// 队友列表（上传到跨服，创好队伍后通知本服让他们跨服）		
	int join_cross_team_id;	// 进入跨服后立即进入某队伍

	int auto_join_type;
	int auto_join_type_param1;
	int param1;
	int param2;

	int team_invite_target_uid;
	int team_invite_type;
};

struct ReturnOriginData_Business
{
	enum BUSINESS_TYPE
	{
		BUSINESS_TYPE_INVALID = 0,
		BUSINESS_TYPE_TEAM_SWITCH_TO_ORIGIN = 1,
		BUSINESS_TYPE_JOIN_ORIGIN_TEAM = 2,
		BUSINESS_TYPE_MAX,
	};

	void Reset()
	{
		business_type = 0;
		
		origin_team_index = 0;
		team_type = 0;
		is_match = 0;
		allow_red_join = 1;
		level_limit_high = 0;
		level_limit_low = 0;

		memset(teammate_list, 0, sizeof(teammate_list));
		join_team_id = 0;
	}

	int business_type;

	int origin_team_index;
	short team_type;
	char is_match;
	char allow_red_join;
	short level_limit_low;
	short level_limit_high;

	int teammate_list[4];	// 队友列表(传回原服，创好队伍后通知隐藏服让他们回来）		
	int join_team_id;		// 进入原服后立即进入某队伍
};

#endif // __CROSS_DEF_HPP__

