#ifndef __MSGCHATMSG_H__
#define __MSGCHATMSG_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/chatserverdef.h"
#include "servercommon/userprotocal/msgcode.h"
#include "servercommon/commondata.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum CHAT_CONTENT_TYPE
	{
		CHAT_CONTENT_TYPE_TEXT,
		CHAT_CONTENT_TYPE_AUDIO, 

		CHAT_CONTENT_TYPE_MAX,
	};



	struct CSChannelChat		//!< 2130	请求频道聊天
	{
		CSChannelChat() :header(MT_CHANNEL_CHAT_CS) {}
		MessageHeader header;

		char content_type;
		char reserve_ch;
		short channel_type;		
		long long group_id;											// 群组id, 非群组聊天填0

		ChatMsgItemInfo item_info[CHAT_MSG_ITEM_INFO_MAX_NUM];		//p1 p2 物品所在格子

		int msg_length;
		// 后面紧跟着消息内容
	};

	struct CSSingleChat				//!< 2131	请求私聊
	{
		CSSingleChat() :header(MT_SINGLE_CHAT_CS) {}
		MessageHeader header;

		char content_type;
		char reserve_ch;
		short reserve_sh;

		ChatMsgItemInfo item_info[CHAT_MSG_ITEM_INFO_MAX_NUM];	//p1 p2 物品所在格子（-1 无效）

		int to_uid;
		int msg_length;
		// 后面紧跟着消息内容
	};


	struct SCChannelChat		//!< 2132 	下发频道聊天
	{
		SCChannelChat() : header(MT_CHANNEL_CHAT_SC) {}
		MessageHeader header;

		enum CHANNEL_CHAT_MSG_TYPE
		{
			CHANNEL_CHAT_MSG_TYPE_BEGIN = 0,	
			CHANNEL_CHAT_MSG_TYPE_TEAM = 1,		//组队信息
			CHANEEL_CHAT_MSG_TYPE_COMMENT = 2,	//弹幕,此处保留不做使用(客户端)
			CHANEEL_CHAT_MSG_TYPE_SPEAKER = 3,	//喇叭
		};

		int	from_uid;
		GameName username;		

		short avatar_type;
		short prof;

		short headshot_id;
		short content_type;
		long long avatar_timestamp;

		short level;
		short channel_type;
		int top_level;
		short reserve_sh;
		short fly_flag;			// 角色飞升阶段
		unsigned int msg_timestamp;

		int face_chatframe;		// 聊天框 | 冒泡框
		int face_signature;		// 签名档 | 尾标
		int msg_type;	
		long long group_id;		// 群组id 非群组聊天默认0

		GuildName guild_name;
		
		ChatMsgItemInfo item_info[CHAT_MSG_ITEM_INFO_MAX_NUM];		//p1 是物品id(0 无效)，p2 是用来查询物品详细信息的id

		unsigned int msg_length;
		// 后面紧跟着消息内容
		char content[MAX_CHAT_LENGTH];
	};


	struct SCSingleChat				// 2133	下发私聊
	{
		SCSingleChat() : header(MT_SINGLE_CHAT_SC) {}
		MessageHeader header;

		int	from_uid;
		int to_uid;
		GameName username;		
		short avatar_type;
		short prof;
		short headshot_id;
		short fly_flag;			// 角色飞升阶段
		char authority_type;
		char content_type;
		short level;
		int top_level;
		long long avatar_timestamp;
		unsigned int msg_timestamp;

		int face_chatframe;		// 聊天框 | 冒泡框
		int face_signature;		// 签名档 | 尾标

		ChatMsgItemInfo item_info[CHAT_MSG_ITEM_INFO_MAX_NUM];		//p1 是物品id(0 无效)，p2 是用来查询物品详细信息的id

		unsigned int msg_length;
		// 后面紧跟着消息内容
		char content[MAX_CHAT_LENGTH];
	};

	struct SCSingleChatUserNotExist		// 2134	通知目标不在线
	{
		SCSingleChatUserNotExist() :header(MT_SINGLE_CHAT_USET_NOT_EXIST_SC) {}
		MessageHeader header;

		int	to_uid;
	};

	struct CSChatItemInfoReq				//!< 2135	聊天中物品信息请求
	{
		CSChatItemInfoReq() : header(MT_CHAT_ITEM_INFO_REQ_CS) {}
		MessageHeader header;

		long long	item_info_seq;
	};

	class SCChatItemInfoRet : public IMessageSerializer //!< 2136 聊天物品信息
	{
	public:
		SCChatItemInfoRet() : header(MT_CHAT_ITEM_INFO_RET_SC) {}
		virtual ~SCChatItemInfoRet() {}
		MessageHeader		header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
			result = result && WriteLL(item_info_seq, buffer, buffer_size, out_length);
			result = result && WriteUShort(item_id, buffer, buffer_size, out_length);
			result = result && WriteShort(num, buffer, buffer_size, out_length);
			result = result && WriteChar(is_bind, buffer, buffer_size, out_length);
			result = result && WriteChar(has_param, buffer, buffer_size, out_length);
			result = result && WriteUInt(invalid_time, buffer, buffer_size, out_length);
			if (0 != has_param)
			{
				result = result && WriteShort(param_length, buffer, buffer_size, out_length);
				result = result && WriteStrN(param_data, param_length, buffer, buffer_size, out_length);
			}

			result = result && WriteUInt(has_attrs, buffer, buffer_size, out_length);
			if (has_attrs != 0)
			{
				for (int i = 0; i < ARRAY_LENGTH(attrs); ++i)
				{
					int value = attrs[i];
					result = result && WriteInt(value, buffer, buffer_size, out_length);
				}
			}

			return result;
		}

		long long	item_info_seq;				// 
		ItemID			item_id;		//!< 物品id
		short				num;			//!< 数量
		char				is_bind;		//!< 是否绑定
		char				has_param;		//!< 是否有净值参数
		UInt32			invalid_time;	//!< 过期时间
		short				param_length;	//!< 净值参数的数据长度 （当has_param为1时可读）
		NetValueItemParamData param_data;	//!< 净值参数的数据，根据param_length的值来读取 （当has_param为1时可读）
		
		int has_attrs;						//这个不为0就要读下面那个数组
		int attrs[BATTLE_ATTR_MAX];
	};

	class CSRealTimeComment			//!< 2137 发送弹幕
	{
	public:
		CSRealTimeComment() : header(MT_REALTIME_COMMENT_CS) {}
		MessageHeader header;

		enum COMMENT_TYPE
		{
			COMMENT_TYPE_BATTLE = 0,			//!< 0 战斗
			COMMENT_TYPE_ACT_RACE_HORSE = 1,	//!< 1 跑马
			COMMENT_TYPE_WEDDING_FB = 2,		//!< 2 婚宴
		};

		int comment_type;
		unsigned int msg_length;
		// 紧接聊天内容

		char msg_content[REAL_TIME_COMMENT_LEN];
	};

	class SCRealTimeComment			//!< 2138 发送弹幕回包
	{
	public:
		SCRealTimeComment() : header(MT_REALTIME_COMMENT_SC) {}
		MessageHeader header;

		int comment_type;
		GameName sender_name;	//!< 发送者名称
		int sender_uid;			//!< 发送者uid

		unsigned int msg_length;
		// 紧接聊天内容

		char msg_content[REAL_TIME_COMMENT_LEN];
	};

	struct MsgRoleGroupInfo
	{
		MsgRoleGroupInfo() : group_id(0), is_cross(0), group_master_id(0), member_count(0)
		{
			memset(group_name, 0, sizeof(group_name));
			memset(top_user_ava, 0, sizeof(top_user_ava));
		}

		long long group_id;
		int is_cross;
		GameName group_name;
		int group_master_id;
		int top_user_ava[3];//3个头像，第一个是群主，暂时写为角色类型
		int member_count;
	};

	class CSGetRoleGroupList		// 3267 获取群聊列表
	{
	public:
		CSGetRoleGroupList() : header(MT_CHAT_GROUP_GET_GROUP_LIST_CS) {}
		MessageHeader header;
	};

	class CSGetGroupInfo			// 3268 获取群聊详细信息，以及成员信息
	{
	public:
		CSGetGroupInfo() : header(MT_CHAT_GROUP_GET_GROUP_INFO_CS) {}
		MessageHeader header;

		long long group_id;
	};

	class SCRoleGroupInfo			// 3241 所有群组的缩略信息
	{
	public:
		SCRoleGroupInfo() : header(MT_CHAT_GROUP_ROLE_GROUP_INFO_SC) {}
		MessageHeader header;

		enum SEDN_TYPE_REASON
		{
			SEDN_TYPE_REASON_DEF = 0,
			SEDN_TYPE_REASON_LOGIN = 1,
		};

		int reason;
		int group_count;
		MsgRoleGroupInfo group_list[MAX_ROLE_GROUP];		// 20
	};

	class SCGroupAllInfo			// 3242 单个群组的详细信息
	{
	public:
		SCGroupAllInfo() : header(MT_CHAT_GROUP_ALL_INFO_SC) {}
		MessageHeader header;

		ChatGroupInfo info;
	};

	class SCRoleInviteEnterGroup	// 3243 发送邀请群组信息
	{
	public:
		SCRoleInviteEnterGroup() : header(MT_CHAT_GROUP_ROLE_INVITE_ENTER_SC) {}
		MessageHeader header;

		int is_cross;
		int invite_server_id;
		int invite_uid;
		GameName inviter_name;
		long long invite_group_id;
		GameName invite_group_name;
	};

	class SCRoleOneGroupInfo			// 3244 单个群组的缩略信息
	{
	public:
		SCRoleOneGroupInfo() : header(MT_CHAT_GROUP_ONE_GROUP_INFO_SC) {}
		MessageHeader header;

		enum REASON
		{
			REASON_ADD = 0,		// 新增群组
			REASON_REMOVE = 1,  // 去除群组
			REASON_INFO_CHANGE = 2, // 群组缩略信息改变
		};

		int reason;
		MsgRoleGroupInfo group;
	};

	class CSRoleInviteEnterGroup	// 3261 请求邀请加入群组
	{
	public:
		CSRoleInviteEnterGroup() : header(MT_CHAT_GROUP_ROLE_INVITE_ENTER_CS) {}
		MessageHeader header;

		long long group_id;
		int be_invite_uid;
	};

	class CSEnterGroupReq			// 3262 接受邀请后加入群组 (拒绝直接忽略信息
	{
	public:
		CSEnterGroupReq() : header(MT_CHAT_GROUP_ROLE_ENTER_GROUP_CS) {}
		MessageHeader header;

		int is_cross;
		int group_server_id;
		int invite_uid;
		long long group_id;
	};

	class CSCreateGroupReq			// 3263 创建群组
	{
	public:
		CSCreateGroupReq() : header(MT_CHAT_GROUP_CREATE_GROUP_CS) {}
		MessageHeader header;
	
		int is_cross;
		GameName group_name;
	};

	class CSGroupKickMember			// 3264 移除玩家出群聊
	{
	public:
		CSGroupKickMember() : header(MT_CHAT_GROUP_KICK_MEMBER_CS) {}
		MessageHeader header;

		long long group_id;
		int be_kicker;
	};

	class CSGroupDissmiss			// 3265 解散群聊
	{
	public:
		CSGroupDissmiss() : header(MT_CHAT_GROUP_DISSMISS_CS) {}
		MessageHeader header;

		long long group_id;
	};

	class CSGroupLeaveReq			// 3266 离开群聊
	{
	public:
		CSGroupLeaveReq() : header(MT_CHAT_GROUP_LEAVE_CS) {}
		MessageHeader header;

		long long group_id;
		int group_server_id;
	};

	class CSChangeGroupNoticeReq			// 3269 修改群组宣言
	{
	public:
		CSChangeGroupNoticeReq() : header(MT_CHAT_GROUP_CHANGE_NOTICE_CS) {}
		MessageHeader header;

		long long group_id;
		GameName group_name;
		GuildNotice group_notice;
	};

	class SCTempGroupSimpleInfo			// 3270 临时群组的简易信息下发
	{
	public:
		SCTempGroupSimpleInfo() : header(MT_TEMP_CHAT_GROUP_SIMPLE_INFO_SC) {}
		MessageHeader header;

		MsgRoleGroupInfo group;
	};

	class SCTempGroupLeaveNotice	// 3271 临时群组离开通知
	{
	public:
		SCTempGroupLeaveNotice() : header(MT_TEMP_CHAT_GROUP_LEAVE_SC) {} ;
		MessageHeader header;
	};

	class SCChatGroupMsgNotice		// 3272 群组通知信息显示
	{
	public:
		SCChatGroupMsgNotice() : header(MT_CHAT_GROUP_MSG_NOTICE_SC) {};
		MessageHeader header;

		long long group_id;			// 群组ID，值为1时表示临时群组
		int msg_length;				// 通知信息长度
		char msg_content[1024];		// 通知信息内容 （按msg_length读取msg_content)
	};

	class CSTempChatGroupBecomeOfficialReq	// 3273 临时群组转正
	{
	public:
		CSTempChatGroupBecomeOfficialReq() : header(MT_TEMP_CHAT_GROUP_BECOME_OFFICIAL_CS){};
		MessageHeader header;

		GameName group_name;	// 群组名
	};

	class SCTempBecomeOfficialNotice	//!< 3274 临时群组转正成功通知
	{
	public:
		SCTempBecomeOfficialNotice() : header(MT_TEMP_CHAT_GROUP_BECOME_OFFICIAL_NOTICE_SC) {}
		MessageHeader header;

		long long group_id;
	};

	class CSHandoverChatGroupLeadership	// 3275 移交群主
	{
	public:
		CSHandoverChatGroupLeadership() : header(MT_CHAT_GROUP_HANDOVER_TO_OTHERS_CS) {}
		MessageHeader header;

		long long group_id;				// 群组ID
		int new_leader_uid;				// 移交的新群主UID
	};

	class SCPlayerIsForbidden	// 2139 是否被禁言
	{
	public:
		SCPlayerIsForbidden() : header(MT_REPLY_WHETHER_FORBIDEN_SC) {}
		MessageHeader header;

		short is_forbidden;		// 是否被禁言，1 表示被禁言，0 表示没被禁言
		short forbid_talk_type;	// 禁言模式 \see FORBID_TALK_TYPE_MAX
		unsigned int forbid_talk_end_timestamp;	// 禁言结束时间戳
	};

	class SCChatRecordList	// 2147 快捷聊天
	{
	public:
		SCChatRecordList() : header(MT_CHAT_RECORD_LIST_SC) {}
		MessageHeader header;

		long long recent_chat_list[CHAT_CORD_MAX_LIST];
	};

	class CSChatRecordSet	// 2148 快捷聊天设置
	{
	public:
		CSChatRecordSet() : header(MT_CHAT_RECORD_SET_CS) {}
		MessageHeader header;

		long long recent_chat_list[CHAT_CORD_MAX_LIST];
	};

	class CSChatSimpleRecordSet	// 2150 快捷发言设置
	{
	public:
		CSChatSimpleRecordSet() : header(MT_CHAT_SIMPLE_RECORD_SET_CS) {}
		MessageHeader header;

		int index;
		GameName simple_data;
	};

	class SCChatSimpleRecordInfo	// 2149 快捷发言下发
	{
	public:
		SCChatSimpleRecordInfo() : header(MT_CHAT_SIMPLE_RECORD_LIST_SC) {}
		MessageHeader header;

		GameName simple_data_list[QUICK_SPEECH_MAX_NUM];
	};

	class SCMutePlayerDeleteChatRecord		//!< 2151 删除被禁言玩家的聊天
	{
	public:
		SCMutePlayerDeleteChatRecord() : header(MT_DELETE_MUTE_PLAYER_CHAT_RECORD_SC) {}
		MessageHeader header;

		int role_id;	//!< 被禁言的玩家
	};

	class CSUserChatRecord		//!< 2153 查询用户聊天记录 
	{
	public:
		CSUserChatRecord() : header(MT_USER_CHAT_RECORD_CS) {}
		MessageHeader header;
	};
}

#pragma pack(pop)

#endif // __MSGCHATMSG_H__

