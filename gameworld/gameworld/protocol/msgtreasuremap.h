#ifndef __MSG_TREASURE_MAP_H__
#define __MSG_TREASURE_MAP_H__

#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum TREASURE_MAP_REQ_TYPE
	{
		TREASURE_MAP_REQ_TYPE_INFO,		//!< 请求信息
		TREASURE_MAP_REQ_TYPE_ACCPET,		//!< 接受宝图任务
		TREASURE_MAP_REQ_TYPE_GIVE_UP,	//!< 放弃宝图任务
		TREASURE_MAP_REQ_TYPE_ONE_CLICK_USE, //!< 一键使用高级藏宝图,param1-使用数量
	};

	class CSTreasureMapReq						//!<2210 
	{
	public:
		CSTreasureMapReq();
		MessageHeader		header;

		int req_type;		
		int param1;
		short param2;
		short param3;
	};

	class SCTreasureMapInfo						//!< 2211 
	{
	public:
		SCTreasureMapInfo();
		MessageHeader		header;

		short play_times;
		short get_reward_times;
		int x;
		int y;
		int monster_id;
		int scene_id;
		int task_id;
		short is_new;
		short reserve_sh;
	};

	class SCTreasureMapItemOpenResult						//!< 2213
	{
	public:
		SCTreasureMapItemOpenResult();
		MessageHeader		header;

		int seq;
		int index;
		int item_id;
	};

	static const int MAX_TREASURE_MAP_REWARD_NOTICE = 100;
	class SCTreasureMapBatchUsedResult		//!<2214 高级藏宝图批量使用获得物品信息
	{
	public:
		SCTreasureMapBatchUsedResult();
		MessageHeader header;

		struct ItemInfo
		{
			ItemInfo():item_id(0), is_bind(0), num(0)
			{}

			ItemID item_id;
			short is_bind;
			int num;
		};

		int coin;
		int count;
		ItemInfo rewards[MAX_TREASURE_MAP_REWARD_NOTICE];
	};

}

#pragma pack(pop)

#endif

