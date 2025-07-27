#ifndef __LGPROTOCAL_H__
#define __LGPROTOCAL_H__

#pragma pack(push) 
#pragma pack(4)

// loginserver && gameworld 的交互
#include "servercommon/logindef.h"

namespace lgprotocal
{

	enum _MsgType
	{
		MT_INVALID,
		MT_LG_PROTOCOL_BEGIN = 23000,
		MT_REGISTER_GS,					// gameworld注册到登录服务器
		MT_LG_REGISTER_GS,				// 登录服务器通知gameworld有其他gameworld注册
		MT_LG_UNREGISTER_GS,			// 登录服务器通知gameworld有其他gameworld反注册
		MT_ROLE_FORBID,					// gameworld通知login封号
		MT_KICK_ROLE,					// login封号后通知gameworld
		MT_LG_REGISTER_BATTLE,			// login通知gameworld连接battle
		MT_GL_REGISTER_CROSS_GATEWAY,	// 游戏服转发跨服gateway信息给登录服
		MT_GL_ON_CROSS_DISCONNECT,		// 与跨服断开了连接
		MT_GL_ONLINE_ROLE_NUM_SYNC,		// 同步当前在线数

		MT_LG_PROTOCOL_END,
	};

	struct MessageHeader
	{
		MessageHeader() : msg_type(MT_INVALID) {}
		MessageHeader(int type) : msg_type(type) {}

		int msg_type;
	};

#define MAX_SCENE_LIST_NUM	2048

	struct GLRegisterGS
	{
		GLRegisterGS() : header(MT_REGISTER_GS) {}
		MessageHeader header;

		int index;
		char ip[16];
		unsigned short port;
		unsigned short scene_count;
		int	scene_list[MAX_SCENE_LIST_NUM];
	};

	struct LGRegisterGS
	{
		LGRegisterGS() : header(MT_LG_REGISTER_GS) {}
		MessageHeader header;

		int index;
		unsigned short reserved;
		unsigned short scene_count;
		int scene_list[MAX_SCENE_LIST_NUM];
	};

	struct LGUnRegisterGS
	{
		LGUnRegisterGS() : header(MT_LG_UNREGISTER_GS) {}
		MessageHeader header;

		unsigned short reserved;
		unsigned short scene_count;
		int scene_list[MAX_SCENE_LIST_NUM];
	};

	struct RoleForbid
	{
		RoleForbid() : header(MT_ROLE_FORBID), forbit_time_s(0), role_id(0) { memset(reason, 0, sizeof(reason)); }
		MessageHeader header;

		PlatName plat_name;			// 被封用户平台名
		char reason[64];			// 原因
		int forbit_time_s;			// 禁号时长
		int role_id;				// 封禁的角色ID
	};

	struct LGKickRole
	{
		LGKickRole() : header(MT_KICK_ROLE) {}
		MessageHeader header;
		
		int role_id;				// 角色ID
		int forbid_time_s;			// 封号时长（秒）
	};

	struct LGRegisterBattle
	{
		LGRegisterBattle() : header(MT_LG_REGISTER_BATTLE) {}
		MessageHeader header;

		int index;
		char ip[16];
		unsigned short port;
		unsigned short reserve_sh;
	};

	struct GLRegisterCrossGateway
	{
		GLRegisterCrossGateway() : header(MT_GL_REGISTER_CROSS_GATEWAY) {}
		MessageHeader header;

		struct GatewayInfo
		{
			char hostname[64];
			unsigned short port;
			short reserve_sh;
		};

		int gateway_num;
		GatewayInfo gateway_list[5];
	};

	struct GLOnCrossDisconnect
	{
		GLOnCrossDisconnect() : header(MT_GL_ON_CROSS_DISCONNECT) {}
		MessageHeader header;
	};

	struct GLSyncOnlineRoleNum
	{
		GLSyncOnlineRoleNum() : header(MT_GL_ONLINE_ROLE_NUM_SYNC) {}
		MessageHeader header;

		int online_role_num;
	};
}

#pragma pack(pop)

#endif // __LGPROTOCAL_H__

