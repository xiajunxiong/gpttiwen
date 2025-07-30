#ifndef __MSG_ADVANCE_CAREER_H__
#define __MSG_ADVANCE_CAREER_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/rolecrystalfbdef.h"
#include "servercommon/commondata.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class CSRoleCareerAdvanceReq					//!< 1456 角色晋升（包括第一次转职）
	{
	public:
		CSRoleCareerAdvanceReq();
		MessageHeader header;

		int to_profession;							//!< 晋升到哪个职业[小阶段时没用]
		int to_color;								//!< 晋升后改变颜色[小阶段时没用]
	};

	class SCAdvanceCareerSubInfo					//!< 4156 职业晋阶子阶信息	
	{
	public:
		SCAdvanceCareerSubInfo();
		MessageHeader		header;

		int sub_level;							// 当前子阶段的阶数
		char is_mission_complete;				// 当前阶级的条件是否完成(非战斗力和等级)
		char reserve_ch;						// 备用_ch
		short reserve_sh;						// 备用_sh
	};
	
	class CSRoleChangeColorReq					// 1459 角色职业颜色变更请求
	{
	public:
		CSRoleChangeColorReq();
		MessageHeader		header;

		int to_color;							// 目标颜色
	};
}

#pragma pack(pop)

#endif	// __MSG_ADVANCE_CAREER_H__

