#ifndef __MSG_RA_HELP_OTHERS_HPP__
#define __MSG_RA_HELP_OTHERS_HPP__

#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push, 4)

namespace Protocol
{

	// 由协议3000
	enum RA_HELP_OTHERS_REQ_TYPE
	{
		RA_HELP_OTHERS_REQ_TYPE_START = 0,			//!< 完成客户端伪任务
		RA_HELP_OTHERS_REQ_TYPE_ACCEPT = 1,			//!< 接取
		RA_HELP_OTHERS_REQ_TYPE_DELIVERY = 2,		//!< 交货	param1 : column param2 : index
		RA_HELP_OTHERS_REQ_TYPE_TALK = 3,			//!< 交谈
		RA_HELP_OTHERS_REQ_TYPE_DELIVERY_PET = 4,	//!< 上交宠物 param1 : pet_index
		RA_HELP_OTHERS_REQ_TYPE_BATTLE_FINISH = 5,	//!< 打怪完成,提交任务
	};

	class SCRAHelpOthersInfo			//!< 3072  随机活动-义胆助人信息下发
	{
	public:
		SCRAHelpOthersInfo();
		MessageHeader header;

		short pass_count;				//
		short task_id;					//任务ID
		short task_status;				//任务状态 HELP_OTHERS_TASK_TYPE
		short progress;					//打怪任务类型时需要
	};
}

#pragma pack(pop)

#endif	//__MSG_RA_HELP_OTHERS_HPP__