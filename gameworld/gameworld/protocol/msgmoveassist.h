#ifndef __MSG_MOVE_ASSIST_H__
#define __MSG_MOVE_ASSIST_H__

#include "gamedef.h"
#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push, 4)

namespace Protocol
{
	//移动协助请求
	class SCMoveAssistReq
	{
	public:
		SCMoveAssistReq();
		MessageHeader header;

		unsigned int move_assist_id;	//寻路辅助模块id
		int scene_id;			//场景id
		int scene_key;			//场景key

		ObjID obj_id;			//请求者obj_id
		short assist_type;		//辅助类型  寻路  场景巡逻  交互

		int speed;				//请求者当前速度

		short cur_pos_x;		//起始位置
		short cur_pos_y;		//起始位置
		short target_pos_x;		//目标位置
		short target_pos_y;		//目标位置
	};

	//移动协助返回
	class CSMoveAssistRet
	{
	public:
		CSMoveAssistRet();
		MessageHeader header;

		unsigned int move_assist_id;
		int scene_id;
		int scene_key;

		ObjID obj_id;
		short assist_type;		//辅助类型  寻路  场景巡逻  交互

		short cur_pos_x;
		short cur_pos_y;
		short target_pos_x;
		short target_pos_y;

		int way_point_count;	//路径点数量

		struct WayPoint
		{
			static const int WAY_POINT_MAX_NUM = 120;
			short pos_x;
			short pos_y;
		};

		WayPoint way_point_list[WayPoint::WAY_POINT_MAX_NUM];
	};
}
#pragma pack(pop)
#endif