#ifndef __MSG_WILD_BOSS_HPP__
#define __MSG_WILD_BOSS_HPP__

#include "servercommon/activitydef.hpp"
#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/wildbossdef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum WILD_BOSS_NOTICE
	{
		WILD_BOSS_NOTICE_MONSTER_BATTLE,			//怪物正在战斗中
		WILD_BOSS_NOTICE_NOT_ENOUGH_MEMBERS,	//队伍人数不足
	};

	class SCWildBossNotice		//!<2470
	{
	public:
		SCWildBossNotice();
		MessageHeader header;
		
		int notice_type;
	};

	class SCWildBossInfo		//!<2472
	{
	public:
		SCWildBossInfo();
		MessageHeader header;

		int today_times;		//!< (今天挑战次数) 改成每周挑战次数
		unsigned int next_refresh_time[MAX_WILD_BOSS_NUM];
		int wild_number_list[MAX_WILD_BOSS_NUM];	//boss编号列表
	};

	class CSWildBossReq		//!<2473
	{
	public:
		CSWildBossReq();
		MessageHeader header;

		int req_type;		//!< 参数,1：下发刷新时间和已挑战次数 2:请求下发怪物seq的位置
		int param;			//!< 参数：怪物seq
	};

	class SCWildBossPosiInfo		//!<2474
	{
	public:
		SCWildBossPosiInfo();
		MessageHeader header;

		int monster_pos_x;
		int monster_pos_y;
	};
	
	struct SCWildBossPokerInfo			// 2475	野外boss翻牌奖励信息
	{
		SCWildBossPokerInfo();
		MessageHeader header;

		int draw_group_id;
		unsigned int timeout;
		WildbossPoker wbpoker[SIDE_MAX_ROLE_NUM];
	};

	struct CSWildBossPokerReq			// 2476 野外boss翻牌奖励请求  [弃用，改为 2260]
	{
		CSWildBossPokerReq();
		MessageHeader header;

		int req_reason;		// 请求原因 0 - 玩家主动翻牌   1 - 玩家没有翻牌，由系统默认帮忙翻，默认翻第一张
		int role_uid;
		int index;			//[1,5]
	};

	class SCWildBossSingleInfo			// 2477 野外BOSS单个信息下发
	{
	public:
		SCWildBossSingleInfo();
		MessageHeader header;

		int index;
		unsigned int next_refresh_time;
		int wild_number;
	};

	class SCWildBossOtherInfo		//!< 1147 野外首领其它信息下发
	{
	public:
		SCWildBossOtherInfo();
		MessageHeader header;

		int reward_times;				//已获得奖励次数
	};

}

#pragma pack(pop)

#endif