#ifndef __MSGSERVER_H__
#define __MSGSERVER_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/serverdef.h"
#include "servercommon/logindef.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class CSEnterGlobalServerReq
	{
	public:
		CSEnterGlobalServerReq();
		MessageHeader header;

		int	scene_id;					// 该字段对globalserver没有什么意义 纯粹用于验证key
		int	scene_key;					// 同上
		int	last_scene_id;
		int	role_id;
		GameName role_name;
		int time;
		SessionKey key;
		PlatName pname;
		short server;
		short is_micro_pc;
	};

	enum __EnterGolbalServerResult
	{
		EGS_RESULT_SUC = 0,
		EGS_USER_EXIST = -1,
	};
	
	class SCEnterGlobalServerAck
	{
	public:
		SCEnterGlobalServerAck();
		MessageHeader header;

		int result;
	};

	class CSTimeReq		//!< 9051 请求时间
	{
	public:
		CSTimeReq();
		MessageHeader header;
	};

	class SCTimeAck		//!< 9001 返回时间
	{
	public:
		SCTimeAck();
		MessageHeader header;

		unsigned int server_time;				//!< 服务器当前时间戳
		unsigned int server_real_start_time;	//!< 服务器首次开服时间戳
		int open_days;							//!< 服务器已开服天数
		unsigned int server_real_combine_time;	//!< 服务器合服时间戳
		unsigned int server_zero_start_time;	//!< 服务器以凌晨0点为基准，计算的服务器开服时间
		char is_server_driving_send;			//!< 1:服务端主动下发
		char reserve_ch;
		short reserve_sh;
	};

	class SCWallowNotice
	{
	public:
		SCWallowNotice();
		MessageHeader header;

		int notice_type;
	};

	class SCTimeDateInfo		//!< 9004 日期下发
	{
	public:
		SCTimeDateInfo();
		MessageHeader header;

		unsigned int time;
		unsigned short year;
		unsigned short mon;
		unsigned char day;
		unsigned char hour;
		unsigned char min;
		unsigned char sec;
	};

	class SCChangeThread
	{
	public:
		SCChangeThread();
		MessageHeader header;
		int game_thread;
	};

	class SCDisconnectNotice
	{
	public:
		SCDisconnectNotice();
		MessageHeader header;

		int reason;
	};

	class SCCrossConnectInfo		//9005 跨服连接信息下发
	{
	public:
		SCCrossConnectInfo();
		MessageHeader header;

		char is_connected_cross;		//0: 未连接 1：已连接
		char is_cross;					//0: 在原服 1：在跨服
		short reserve_sh;
	};

}

#pragma pack(pop)

#endif

