#ifndef __LOGINMAGCODE_H__
#define __LOGINMAGCODE_H__

namespace Protocol
{
	enum
	{
		/*LOGIN_SC_BEGIN = 7000,
		LOGIN_CS_BEGIN = 7050,*/

		REGISTER_SC_BEGIN = 7100,
		REGISTER_CS_BEGIN = 7150,
	};

	enum _MsgTypeLogin
	{
		
		///////////////////////////////////////// 登录服务器相关 /////////////////////////////////////////
		MT_LOGIN_TO_ROLE_SC = 7000,					// 登录角色请求返回（返回角色所在场景信息，以及要连接的网关等）
		MT_LOGIN_TO_ACCOUNT_SC = 7001,				// 登录账号请求返回（返回账户信息）
		MT_GET_THREAD_ACK_SC = 7002,				// 查看服务器状态返回
		MT_ANTI_WALLOW_EX_RET_SC = 7003,			// 服务器验证防沉迷返回
		MT_ACCOUNT_KEY_ERROR_SC = 7004,				// KEY不对通知
		MT_MERGE_ROLE_LIST_SC = 7005,				// 合服后的角色列表
		MT_PROF_NUM_INFO_SC = 7006,					// 职业人数信息、
		MT_L_HEARTBEAT_SC = 7007,					// 心跳包返回

		MT_L_HEARTBEAT_CS = 7052,					// 心跳包
		MT_GET_TREAD_CS = 7053,						// 查看服务器分线信息
		MT_ANTI_WALLOW_CS = 7054,					// 通过防沉迷
		MT_ANTI_WALLOW_EX_CS = 7055,				// 防沉迷检验
		MT_LOGIN_TO_ACCOUNT_CS = 7056,				// 登录账号请求
		MT_LOGIN_TO_ROLE_CS = 7057,					// 登录角色请求
		
		///////////////////////////////////////// 角色相关 /////////////////////////////////////////
		MT_CREATE_ROLE_ACK_SC = 7100,				// 创建角色返回
		MT_DESTROY_ROLE_ACK_SC = 7101,				// 销毁角色返回
		MT_CHANGE_PLATNAME_ACK_SC = 7102,			// 修改平台名返回 （特定模式下使用）

		MT_CREATE_ROLE_REQ_CS = 7150,				// 创建角色
		MT_DESTROY_ROLE_REQ_CS = 7151,				// 销毁角色
		MT_CHANGE_PLATNAME_REQ_CS = 7152,			// 修改平台名

		MT_GET_RANDOM_NAME_CS = 7153,				// 获取随机名字请求
		MT_GET_RANDOM_NAME_SC = 7154,				// 获取随机名字返回

		MT_STORY_BATTLE_INFO_CS = 7155,				// 获取剧情基本信息
		MT_STORY_ROUND_RESULT_CS = 7156,			// 获取剧情回合战报
		MT_STORY_NOT_AVALIABLE_SC = 7157,			// 剧情暂不可用通知
	};
}

#endif // __LOGINMAGCODE_H__
