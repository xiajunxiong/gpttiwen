#ifndef __MSGSYSTEM_H__
#define __MSGSYSTEM_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/logindef.h"
#include "servercommon/userprotocal/systemmsgcode.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	/*
		系统相关
	*/

	#define PROTOCAL_MAX_SYSTEM_MSG_LENGTH 2048
	#define SPID_MSG_LENGTH 64
	class SCSystemMsg				//!< 701 系统信息
	{
	public:
		SCSystemMsg() : header(MT_SYSTEM_MSG_SC) {}
		MessageHeader header;

		unsigned int send_time;
		short reserve_sh;
		short min_level;				// 等于0时代表没有限制	
		short max_level;				// 等于0时代表没有限制
		short msg_length;
		int	display_pos;
		int color;
		int msg_type;	
		char spid_id[SPID_MSG_LENGTH];
		char msg[PROTOCAL_MAX_SYSTEM_MSG_LENGTH];
	};

	class SCNoticeNum
	{
	public:
		SCNoticeNum() : header(MT_SYSTEM_NOTICE_CODE_SC) {}
		MessageHeader header;

		int notice_num;
	};

	class SCZeroHour
	{
	public:
		SCZeroHour();
		MessageHeader header;
	};

	class CSHeartbeatReq //!< 1053 心跳请求
	{
	public:
		CSHeartbeatReq();
		MessageHeader header;
	};

	class SCHeartbeatResp //!< 1003 心跳返回
	{
	public:
		SCHeartbeatResp();
		MessageHeader header;
	};

	class SCClientHeartbeatCheckModify	// 704 控制客户端的心跳检测的自动断线逻辑开关
	{
	public:
		SCClientHeartbeatCheckModify();
		MessageHeader header;

		int is_open;		// 是否开启心跳检测自动断线的逻辑   0:关闭  1:开启  

		//!< \note  开始热更前会下发这条协议，客户端收到后暂时关闭“检测到没有心跳时会断开连接”的逻辑，
		//!<        但不要关闭心跳逻辑，心跳照常发就好了。过一分钟后会再次下发这条协议，重新开启
		//!<        “检测到没有心跳时会断开连接”的逻辑
	};

	class SCForbidReason //!< 705 封号理由返回
	{
	public:
		SCForbidReason();
		MessageHeader header;

		int forbid_time_log;//封号持续时间
	};
}

#pragma pack(pop)

#endif

