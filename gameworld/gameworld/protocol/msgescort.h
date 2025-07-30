#ifndef __MSG_ESCORT_H__
#define __MSG_ESCORT_H__

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class SCEscortAllInfo					//!< 所有信息2220
	{
	public:
		SCEscortAllInfo();
		MessageHeader		header;

		int				today_count;			//!< 今天参与了几次
		int				task_seq;				//!< 任务id
		int				npc_id;
		unsigned int	end_time;				//日常活动运镖结束的时间戳  0代表没有倒计时
	};

	class CSEscortOp						//!< 运镖操作2221
	{
	public:
		CSEscortOp();
		MessageHeader		header;

		enum OP_TYPE
		{
			ACCEPT_TASK = 0,		//!< 接任务 param1:运镖类型 param2:任务ID(运镖类型为奇遇时)
			COMMIT_TASK,			//!< 提交任务
			GIVEUP_TASK,			//!< 放弃任务
		};

		int op_type;
		int param1;
		int param2;
	};

	class SCEscortResultNotic					//!< 运镖结果通知2222
	{
	public:
		SCEscortResultNotic();
		MessageHeader		header;

		int is_win;//!<0为失败 1为胜利
	};

	class SCEscortStatusChangeNotice		// 2223 运镖状态改变后，通知周围玩家
	{
	public:
		SCEscortStatusChangeNotice();
		MessageHeader header;

		ObjID role_obj_id;		//!< 状态改变的玩家obj_id
		short reserve_sh;		//!< 保留字
		int escort_seq;			//!< 运镖任务seq id
	};
}

#pragma pack(pop)

#endif

