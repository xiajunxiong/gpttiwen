#ifndef __MSG_SHAN_HAI_QUESTION_HPP__ 
#define __MSG_SHAN_HAI_QUESTION_HPP__

#include "servercommon/userprotocal/msgheader.h"

//////////////////////////////////////////// 2140 随机活动 - 山海问答  //////////////////////////////////////////


#pragma pack(push,4)


namespace Protocol
{

	struct SCRAShanHaiQuestionInfo			//!< 3092 山海问答数据下发
	{
	public:
		SCRAShanHaiQuestionInfo();
		MessageHeader header;

		short is_can_answer;		// 是否在答题时间段内 1-在，0-不在
		short times;				// 玩家剩余答题次数
		short question_id;			// 题目id
		short reward_seq;			// 奖励组索引
	};

	struct SCRAShanHaiQuestionStatusInfo	//!< 3099 山海问答答题状态信息下发
	{
	public:
		SCRAShanHaiQuestionStatusInfo();
		MessageHeader header;

		int is_can_answer;		// 是否在答题时间段内 1-在，0-不在
	};

}

#pragma pack(pop)

#endif
