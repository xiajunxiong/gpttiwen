#ifndef __CHATMSGCODE_H__
#define __CHATMSGCODE_H__

namespace Protocol
{
	enum
	{
		CHATCTRL_SC_BEGIN = 8000,
		CHATCTRL_CS_BEGIN = 8050,

		CHATMSG_SC_BEGIN = 8100,
		CHATMSG_CS_BEGIN = 8150,
	};

	enum _MsgTypeChat
	{
		MT_USER_LOGIN_ACK_SC = CHATCTRL_SC_BEGIN,			// 用户登录返回
		MT_USER_BE_DESTROY_SC,								// 通知用户被销毁
		MT_INTO_CHANNEL_SC,									// 用户进入频道
		MT_OUT_OF_CHANNEL_SC,								// 用户离开频道
		MT_NOTICE_ONLINE_SC,								// 上线提醒
		MT_NOTICE_OFFLINE_SC,								// 下线提醒
		MT_NOTICE_ONLINE_STATUS_SC,							// 在线状态变化提醒		

		MT_USER_LOGIN_CS = CHATCTRL_CS_BEGIN,				// 用户请求登录聊天服务器
		MT_MUTE_USER_CS,									// 屏蔽某用户
		MT_UNMUTE_USER_CS,									// 取消屏蔽
		MT_CHANGE_ONLINE_STATUS_CS,							// 用户变更在线状态

		////////////////////////////////////////////////////////////////////////////////////////////////////////


	};
}

#endif // __CHATMSGCODE_H__

