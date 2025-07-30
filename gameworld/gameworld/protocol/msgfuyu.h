#ifndef __MSG_FU_YU_H__
#define __MSG_FU_YU_H__

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"
#include "servercommon/fuyudef.h"

#pragma pack(push, 4) 

namespace Protocol
{
	enum FU_YU_REQ_TYPE
	{
		FU_YU_REQ_TYPE_INFO = 0,		//!< 请求信息
		FU_YU_REQ_TYPE_PUT_ON = 1,		//!< 穿戴符玉			p1-伙伴id, p2-伙伴符玉装备index(0-2) p3-背包column, p4-背包index
		FU_YU_REQ_TYPE_TAKE_OFF = 2,	//!< 脱下符玉			p1-伙伴id, p2-伙伴符玉装备index(0-2)
		FU_YU_REQ_TYPE_DECOMPOSE = 3,	//!< 分解符玉			p1-背包column, p2-背包index
		FU_YU_REQ_TYPE_UPGRADE = 4,		//!< 强化符玉(穿戴)		p1-伙伴id, p2-伙伴符玉装备index(0-2), p3-消耗材料id, p4- 是否使用保护道具(1-使用, 0-不使用)
		FU_YU_REQ_TYPE_UPGRADE_2 = 5,	//!< 强化符玉(背包)		p1-背包column, p2-背包index, p3-消耗材料id, p4- 是否使用保护道具(1-使用, 0-不使用)
		FU_YU_REQ_TYPE_CONVERT_WEAR = 6,//!< 符玉转换(穿戴)		p1-伙伴id, p2-伙伴符玉装备index(0-2), p3-要转换的特效id1, p4-要转换的特效id2(混元/化神符玉需要, 普通符玉填0)
		FU_YU_REQ_TYPE_CONVERT_BAG = 7,	//!< 符玉转换(背包)		p1-背包column, p2-背包index, p3-要转换的特效id1, p4-要转换的特效id2(混元/化神符玉需要, 普通符玉填0)
		FU_YU_REQ_TYPE_SUBLIME_WEAR = 8,//!< 符玉升华(穿戴)		p1-伙伴id, p2-伙伴天赐符玉装备index(0-2), p3&p4-消耗天赐符玉column & index, p5&p6-消耗普通符玉column & index
		FU_YU_REQ_TYPE_SUBLIME_BAG = 9,	//!< 符玉升华(背包)		p1&p2-目标天赐符玉column & index, p3&p4-消耗天赐符玉column & index, p5&p6-消耗普通符玉column & index
	};

	class CSFuYuReq		//!<5188 符玉请求
	{
	public:
		CSFuYuReq();
		MessageHeader header;

		int req_type;
		int param1;
		int param2;
		int param3;
		int param4;
		int param5;
		int param6;
	};

	class SCFuYuInfo : public IMessageSerializer		//!<5186 已穿戴符玉单条信息下发
	{
	public:
		SCFuYuInfo();
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char * buffer, int buffer_size, int * out_length);

		int reason;			// FuYuManager::FU_YU_SEND_REASON
		FuYuItemInfo info;
	};

	class SCAllFuYuInfo : public IMessageSerializer		//!<5187 已穿戴符玉全部信息下发
	{
	public:
		SCAllFuYuInfo();
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char * buffer, int buffer_size, int * out_length);

		int count;
		FuYuItemInfo info_list[ItemNamespace::MAX_FU_YU_GRID_NUM];
	};
}

#pragma pack(pop) 
#endif
