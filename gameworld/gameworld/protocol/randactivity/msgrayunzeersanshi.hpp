#ifndef __MSG_RA_YUN_ZE_ER_SAN_SHI_HPP__
#define __MSG_RA_YUN_ZE_ER_SAN_SHI_HPP__

#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push, 4)

namespace Protocol
{

	class SCRAYunZeErSanShiInfo				//!< 3120 随机活动-云泽二三事信息下发
	{
	public:
		SCRAYunZeErSanShiInfo();
		MessageHeader header;

		short times;
		short is_turntable_reward;			// 是否有转盘奖励
		int task_id;		
	};

	class SCRAYunZeErSanShiTurntableInfo			//!< 3126 随机活动-云泽二三事转盘信息下发
	{
	public:
		SCRAYunZeErSanShiTurntableInfo();
		MessageHeader header;

		int seq;
	};
}

#pragma pack(pop)

#endif