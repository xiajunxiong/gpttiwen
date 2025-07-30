#ifndef __MSG_DU_JIE_H__
#define __MSG_DU_JIE_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/commondata.hpp"
#include "servercommon/servercommon.h"
#include "gamedef.h"

#include "servercommon/dujiedef.hpp"

#pragma pack(push) 
#pragma pack(4)


namespace Protocol
{
	//////渡劫
	//渡劫 用户操作	3935
	class CSDuJieReq
	{
	public:
		CSDuJieReq();
		MessageHeader		header;

		enum ReqType
		{
			REQ_TYPE_ROLE_INFO = 0,		//角色数据
		};

		int req_type;		//操作类型
		int param_1;
	};

	//角色信息  3936
	class SCDuJieRoleInfo
	{
	public:
		SCDuJieRoleInfo();
		MessageHeader		header;
		
		int max_pass_fb_seq;			//最高关卡
		int next_front_gate_pass;		//前置任务参数(最高关卡下一关的)

		int day_assist_times;
	};

	//副本信息  3937
	class SCDuJieFBInfo
	{
	public:
		SCDuJieFBInfo();
		MessageHeader		header;

		int m_fb_level;		//副本level
		int m_is_finish;	//副本完成标记
	};
};

#pragma pack(pop)

#endif