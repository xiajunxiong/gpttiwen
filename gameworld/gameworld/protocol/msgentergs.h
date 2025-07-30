
#ifndef MSGENTERGS_H
#define MSGENTERGS_H

#include "gamedef.h"
#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/logindef.h"


#pragma pack(push) 
#pragma pack(4)


namespace Protocol
{
	/* 
	进入场景服务器相关，以及推出游戏请求
	*/
	class CSUserEnterGSReq					//!< 1050 请求进入场景协议 （首次连接游戏服时请求）
	{
	public:
		CSUserEnterGSReq();
		MessageHeader	header;				
		int				scene_id;			//!< 场景ID
		int				scene_key;			//!< 场景Key
		int				last_scene_id;		//!< 上一次场景ID
		int				role_id;			//!< 游戏角色ID
		GameName		role_name;			//!< 角色名
		int				time;				//!< 时间戳 (SCLoginToRole下发的time参数原样发过来)
		char			is_login;			//!< 登陆还是场景切换 (发1即可）（现在没有多场景服的架构，所以一定是登录）
		char			reserved1;			//!< 保留字
		short			server;				//!< 登录的服务器ID
		SessionKey		key;				//!< 登录服下发的验证key
		PlatName		pname;				//!< 带spid的账号名
		int				is_micro_pc;		//!< 填0
		char			plat_spid_str[4];	//!< 平台的渠道ID 
	};

	class SCServerBusy  //!< 1002 服务器忙
	{
	public:
		SCServerBusy();
		MessageHeader	header;
	};

	enum __EnterGSResult
	{
		ENTERGS_RESULT_SUC=0,
		ENTERGS_USER_EXIST=-1,
		ENTERGS_RESULT_SCENE_NO_EXIST=-2,
		ENTERGS_USER_NOT_ALLOWED = -3,
		ENTERGS_SERVER_NOT_READY = -4,	// 服务器尚未完全启动，请稍候
	};
	class SCUserEnterGSAck				//!< 1000 进入场景返回 
	{
	public:
		SCUserEnterGSAck();
		MessageHeader	header;
		int				result;			//!< 结果  \see __EnterGSResult
		int				is_in_cross;	//!< 是否在跨服中 0：否  1：是
	};

	class SCChangeGS    //暂时不用
	{
	public:
		SCChangeGS();
		MessageHeader	header;
		int				scene_id;
		int				scene_key;
		int				last_scene_id;
		int				uid;
		unsigned int	time;
		SessionKey		key;
		unsigned short	gs_index;
		unsigned short	is_login;
	};

	class CSUserLogout		//!< 1051 登出请求
	{
	public:
		CSUserLogout();
		MessageHeader	header;
	};

	class CSTransportReq	//!< 1052 从传送点切换场景
	{
	public:
		CSTransportReq();
		MessageHeader	header;
		int				door_id;	//!< 传送阵id
	};

	class CSSpecialTransportReq	//!< 1055 特别逻辑传送
	{
	public:
		CSSpecialTransportReq();
		MessageHeader	header;
		
		ObjID obj_id;			//要使用那个传送门的obj_id
		short reserved;
	};

	class CSTreasureChestReq	//!< 1056 宝箱请求
	{
	public:
		CSTreasureChestReq();
		MessageHeader	header;

		ObjID obj_id;			//宝箱objid
		short reserved;
	};
}


#pragma pack(pop)


#endif


