
#ifndef MSGLOGIN_H
#define MSGLOGIN_H

#include "servercommon/serverdef.h"
#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/logindef.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	// 登陆相关
	class CSLoginToAccount
	{
	public:
		CSLoginToAccount();
		MessageHeader		header;
		int					rand_val1;		// 随机数 100w-1000w
		unsigned int		login_time;
		LoginStr			login_str;
		PlatName			pname;
		int					rand_val2;		// 随机数 100w-1000w
		short				reserved1;
		short				server;
	};

	enum __LoginAckResult
	{
		LOGIN_RESULT_SUC = 0,
		LOGIN_NO_THREAD = -1,
		LOGIN_SERVER_ERROR = -2,
		LOGIN_RESULT_EXIST = -3,
		LOGIN_SCENE_NOT_EXIST = -4,
		LOGIN_RESULT_NO_GATEWAY = -5,
		LOGIN_RESULT_NO_ROLE = -6,
		LOGIN_THREAD_BUSY = -7,
		LOGIN_LOGIN_FORBID = -8,
		LOGIN_ANTI_WALLOW = -9,
		LOGIN_FORBID_NEW_ROLE = -10,
		LOGIN_LIMIT_CREATE_NEW_ROLE = -11,
		LOGIN_LIMIT_LOGIN = -12,
	};

	struct RoleItem 
	{
		int					role_id;		//!< 角色id
		GameName			role_name;		//!< 角色名字
		int					profession;		//!< 角色职业
		short				avatar_type;	//!< 角色模型
		short				color;			//!< 角色颜色
		short				level;			//!< 角色等级
		short				headshot_id;	//!< 角色头像
		int					weapon_id;		//!< 武器ID
		int					top_level;		//!< 巅峰等级
		short				fly_flag;		//!< 飞升阶段
		short				reserve_sh;
		short				surface_list[SURFACE_TYPE_MAX]; //穿戴中的幻化资源
		unsigned int		forbid_time;	//!< 封禁解封时间戳 为0或小于当前时间则没有被封禁或已被解封
	};

	class SCLoginToAccount	// 7001 登录返回账号信息
	{
	public:
		SCLoginToAccount();
		MessageHeader header;

		short				most_recent_role_index;
		short				result;
		int					count;
		RoleItem role_item[MAX_ACCOUNT_ROLE_NUM];
	};

	class CSLoginToRole
	{
	public:
		CSLoginToRole();
		MessageHeader		header;

		int					rand_val1;		// 随机数 100w-1000w
		unsigned int		login_time;
		LoginStr			login_str;
		PlatName			pname;
		short				server;
		char				reserved1;
		char				anti_wallow;

		int					rand_val2;		// 随机数 100w-1000w
		int					role_id;
	};

	class SCLoginToRole		// 7000
	{
	public:
		SCLoginToRole();
		MessageHeader		header;

		int					uid;
		short				result;
		char				is_cross;
		char				is_merged_server;
		int					scene_id;
		int					last_scene_id;
		SessionKey			key;
		unsigned int		time;
		char				gs_hostname[64];	// 修改该长度的同时，必须修改lwprotocal::WLRegisterGW::hostname的长度
		unsigned short		gs_port;
		unsigned short		gs_index;
		unsigned int		server_time;
		GameName			role_name;
		char				backup_gs_hostname[64];	// 备用gateway hostname
		unsigned short		backup_gs_port;			// 备用gateway 端口
		short				reserve_sh;
		unsigned int		create_time;
	};

	class CSRandomNameReq
	{
	public:
		CSRandomNameReq();
		MessageHeader		header;

		int					sex;			// 角色性别
	};

	class SCRandomNameRet	//!< 7154 获取随机名字
	{
	public:
		SCRandomNameRet();
		MessageHeader header;

		short front_sex;	//!< 前缀性别
		short front_id;		//!< 前缀名字id
		short middle_sex;	//!< 中缀性别
		short middle_id;	//!< 中缀名字id
		short last_sex;		//!< 后缀性别
		short last_id;		//!< 后缀名字id
		long long name_str_id; //!< 名字id (创角时，若使用服务端提供的随机名字，则原样发给服务端)
	};

	class CSLHeartBeat		// 7052 心跳请求
	{
	public:
		CSLHeartBeat();
		MessageHeader		header;
	};

	class SCLHeartBeat		// 7007 心跳返回
	{
	public:
		SCLHeartBeat();
		MessageHeader		header;
	};

	class CSGetThreadInfo
	{
	public:
		CSGetThreadInfo();
		MessageHeader		header;
		PlatName			pname;
		unsigned int		login_time;
		LoginStr			login_str;
		short				reserved1;
		short				server;
	};


	enum THREAD_STATE
	{
		THREAD_STATE_IDLE=0,
		THREAD_STATE_NORMAL,
		THREAD_STATE_BUSY,
		THREAD_STATE_FULL,
	};

	class SCGetThreadAck
	{
	public:
		SCGetThreadAck();
		MessageHeader		header;
		int					thread_state;
	};

	class CSPassAntiWallow
	{
	public:
		CSPassAntiWallow();
		MessageHeader		header;
		PlatName			pname;
		unsigned int		login_time;
		LoginStr			login_str;
		char				anti_wallow;
		char				reserved1;
		short				server;
	};

	class CSPassAntiWallowEx
	{
	public:
		CSPassAntiWallowEx();
		MessageHeader		header;
		PlatName			pname;
		unsigned int		login_time;
		LoginStr			login_str;
		char				anti_wallow;
		char				reserved1;
		short				server;

		char				name[20];
		char				id[20];
	};

	enum ANTIWALLOW_RET
	{
		ANTIWALLOW_SUC=0,
		ANTIWALLOW_ID_INVALID=-1,
		ANTIWALLOW_ID_REPEATE=-2,
		ANTIWALLOW_FAIL=-3,
		ANTIWALLOW_NOT_ENOUGH_18=-4,
	};

	class SCAntiWallowRet
	{
	public:
		SCAntiWallowRet();
		MessageHeader		header;
		int					ret;		// 0 成功 -1身份证无效 -2身份证重复 -3失败
	};

	class SCAccountKeyError
	{
	public:
		SCAccountKeyError();
		MessageHeader		header;
	};

	class SCProfNumInfo
	{
	public:
		SCProfNumInfo();
		MessageHeader		header;

		int prof1_num;
		int prof2_num;
		int prof3_num;
		int prof4_num;
	};

	class CSStoryBattleInfo	// 7155  获取剧情基本信息（会下发SCBattleInfo）
	{
	public:
		CSStoryBattleInfo();
		MessageHeader header;
	};

	class CSStoryRoundResult // 7156 获取剧情战报信息（会下发战报）
	{
	public:
		CSStoryRoundResult();
		MessageHeader header;

		int round;		// 从0开始，一共两个回合
	};

	class SCStoryNotAvaliable // 7157 剧情暂不可用通知
	{
	public:
		SCStoryNotAvaliable();
		MessageHeader header;
	};
}

#pragma pack(pop)

#endif

