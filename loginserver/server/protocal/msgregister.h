
#ifndef MSGREGISTER_H
#define MSGREGISTER_H

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/logindef.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	/*
		创建和销毁角色相关
	*/

	class CSCreateRole							//!< 7150 创角请求
	{
	public:
		CSCreateRole();
		MessageHeader		header;

		PlatName			pname;				//!< 账户名(带spid)，如 dev_aaa123
		GameName			role_name;			//!< 角色名
		unsigned int		login_time;			//!< php下发
		LoginStr			login_str;			//!< php下发
		short				server;				//!< 服号
		char				avatar_type;		//!< 虚拟人物类型(不带头像，所以char即可）
		char				color;				//!< 颜色
		long long			name_str_id;		//!< 名字的hash id
		int					job_id;				//!< 职业
		char				plat_spid_str[4];	//!< 客户端spid
		int					mini_game_id;		//!< 小游戏ID
		char                is_auto_rand_name;	//!< 是否自随机名字(0:不自动 1:自动) 
		char				audit_server_seq;	//!< 审核服seq
		short				reserve_sh;
	};

	class SCCreateRoleAck						//!< 7100 创角返回
	{
	public:
		SCCreateRoleAck();
		MessageHeader		header;
		int					result;				//!< 结果
		int					role_id;			//!< 角色ID
		GameName			role_name;			//!< 角色名字
		short				reserve_sh;			//!< 保留字
		char				avartar_type;		//!< 虚拟人物类型
		char				color;				//!< 颜色
		long long			create_role_time;	//!< 创建角色时间
	};

	class CSDestroyRole
	{
	public:
		CSDestroyRole();
		MessageHeader		header;

		PlatName			pname;
		unsigned int		login_time;
		LoginStr			login_str;
		char				anti_wallow;
		char				reserved1;
		short				server;

		int					role_id;
	};

	class SCDestroyRoleAck
	{
	public:
		SCDestroyRoleAck();
		MessageHeader		header;
		int					role_id;
		int					result;
	};

	class CSChangePlatName
	{
	public:
		CSChangePlatName();
		MessageHeader		header;

		int server;
		PlatName			old_name;
		PlatName			new_name;
	};

	class SCChangePlatNameAck
	{
	public:
		SCChangePlatNameAck();
		MessageHeader		header;

		int					result;
	};
}

#pragma pack(pop)

#endif
