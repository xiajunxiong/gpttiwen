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



	struct CSChannelChat		//!< 2130	����Ƶ������
	{
		CSChannelChat() :header(MT_CHANNEL_CHAT_CS) {}
		MessageHeader header;

		char content_type;
		char reserve_ch;
		short channel_type;		
		long long group_id;											// Ⱥ��id, ��Ⱥ��������0

		ChatMsgItemInfo item_info[CHAT_MSG_ITEM_INFO_MAX_NUM];		//p1 p2 ��Ʒ���ڸ���

		int msg_length;
		// �����������Ϣ����
	};

	struct CSSingleChat				//!< 2131	����˽��
	{
		CSSingleChat() :header(MT_SINGLE_CHAT_CS) {}
		MessageHeader header;

		char content_type;
		char reserve_ch;
		short reserve_sh;

		ChatMsgItemInfo item_info[CHAT_MSG_ITEM_INFO_MAX_NUM];	//p1 p2 ��Ʒ���ڸ��ӣ�-1 ��Ч��

		int to_uid;
		int msg_length;
		// �����������Ϣ����
	};


	struct SCChannelChat		//!< 2132 	�·�Ƶ������
	{
		SCChannelChat() : header(MT_CHANNEL_CHAT_SC) {}
		MessageHeader header;

		enum CHANNEL_CHAT_MSG_TYPE
		{
			CHANNEL_CHAT_MSG_TYPE_BEGIN = 0,	
			CHANNEL_CHAT_MSG_TYPE_TEAM = 1,		//�����Ϣ
			CHANEEL_CHAT_MSG_TYPE_COMMENT = 2,	//��Ļ,�˴���������ʹ��(�ͻ���)
			CHANEEL_CHAT_MSG_TYPE_SPEAKER = 3,	//����
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
		short fly_flag;			// ��ɫ�����׶�
		unsigned int msg_timestamp;

		int face_chatframe;		// ����� | ð�ݿ�
		int face_signature;		// ǩ���� | β��
		int msg_type;	
		long long group_id;		// Ⱥ��id ��Ⱥ������Ĭ��0

		GuildName guild_name;
		
		ChatMsgItemInfo item_info[CHAT_MSG_ITEM_INFO_MAX_NUM];		//p1 ����Ʒid(0 ��Ч)��p2 ��������ѯ��Ʒ��ϸ��Ϣ��id

		unsigned int msg_length;
		// �����������Ϣ����
		char content[MAX_CHAT_LENGTH];
	};


	struct SCSingleChat				// 2133	�·�˽��
	{
		SCSingleChat() : header(MT_SINGLE_CHAT_SC) {}
		MessageHeader header;

		int	from_uid;
		int to_uid;
		GameName username;		
		short avatar_type;
		short prof;
		short headshot_id;
		short fly_flag;			// ��ɫ�����׶�
		char authority_type;
		char content_type;
		short level;
		int top_level;
		long long avatar_timestamp;
		unsigned int msg_timestamp;

		int face_chatframe;		// ����� | ð�ݿ�
		int face_signature;		// ǩ���� | β��

		ChatMsgItemInfo item_info[CHAT_MSG_ITEM_INFO_MAX_NUM];		//p1 ����Ʒid(0 ��Ч)��p2 ��������ѯ��Ʒ��ϸ��Ϣ��id

		unsigned int msg_length;
		// �����������Ϣ����
		char content[MAX_CHAT_LENGTH];
	};

	struct SCSingleChatUserNotExist		// 2134	֪ͨĿ�겻����
	{
		SCSingleChatUserNotExist() :header(MT_SINGLE_CHAT_USET_NOT_EXIST_SC) {}
		MessageHeader header;

		int	to_uid;
	};

	struct CSChatItemInfoReq				//!< 2135	��������Ʒ��Ϣ����
	{
		CSChatItemInfoReq() : header(MT_CHAT_ITEM_INFO_REQ_CS) {}
		MessageHeader header;

		long long	item_info_seq;
	};

	class SCChatItemInfoRet : public IMessageSerializer //!< 2136 ������Ʒ��Ϣ
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
		ItemID			item_id;		//!< ��Ʒid
		short				num;			//!< ����
		char				is_bind;		//!< �Ƿ��
		char				has_param;		//!< �Ƿ��о�ֵ����
		UInt32			invalid_time;	//!< ����ʱ��
		short				param_length;	//!< ��ֵ���������ݳ��� ����has_paramΪ1ʱ�ɶ���
		NetValueItemParamData param_data;	//!< ��ֵ���������ݣ�����param_length��ֵ����ȡ ����has_paramΪ1ʱ�ɶ���
		
		int has_attrs;						//�����Ϊ0��Ҫ�������Ǹ�����
		int attrs[BATTLE_ATTR_MAX];
	};

	class CSRealTimeComment			//!< 2137 ���͵�Ļ
	{
	public:
		CSRealTimeComment() : header(MT_REALTIME_COMMENT_CS) {}
		MessageHeader header;

		enum COMMENT_TYPE
		{
			COMMENT_TYPE_BATTLE = 0,			//!< 0 ս��
			COMMENT_TYPE_ACT_RACE_HORSE = 1,	//!< 1 ����
			COMMENT_TYPE_WEDDING_FB = 2,		//!< 2 ����
		};

		int comment_type;
		unsigned int msg_length;
		// ������������

		char msg_content[REAL_TIME_COMMENT_LEN];
	};

	class SCRealTimeComment			//!< 2138 ���͵�Ļ�ذ�
	{
	public:
		SCRealTimeComment() : header(MT_REALTIME_COMMENT_SC) {}
		MessageHeader header;

		int comment_type;
		GameName sender_name;	//!< ����������
		int sender_uid;			//!< ������uid

		unsigned int msg_length;
		// ������������

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
		int top_user_ava[3];//3��ͷ�񣬵�һ����Ⱥ������ʱдΪ��ɫ����
		int member_count;
	};

	class CSGetRoleGroupList		// 3267 ��ȡȺ���б�
	{
	public:
		CSGetRoleGroupList() : header(MT_CHAT_GROUP_GET_GROUP_LIST_CS) {}
		MessageHeader header;
	};

	class CSGetGroupInfo			// 3268 ��ȡȺ����ϸ��Ϣ���Լ���Ա��Ϣ
	{
	public:
		CSGetGroupInfo() : header(MT_CHAT_GROUP_GET_GROUP_INFO_CS) {}
		MessageHeader header;

		long long group_id;
	};

	class SCRoleGroupInfo			// 3241 ����Ⱥ���������Ϣ
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

	class SCGroupAllInfo			// 3242 ����Ⱥ�����ϸ��Ϣ
	{
	public:
		SCGroupAllInfo() : header(MT_CHAT_GROUP_ALL_INFO_SC) {}
		MessageHeader header;

		ChatGroupInfo info;
	};

	class SCRoleInviteEnterGroup	// 3243 ��������Ⱥ����Ϣ
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

	class SCRoleOneGroupInfo			// 3244 ����Ⱥ���������Ϣ
	{
	public:
		SCRoleOneGroupInfo() : header(MT_CHAT_GROUP_ONE_GROUP_INFO_SC) {}
		MessageHeader header;

		enum REASON
		{
			REASON_ADD = 0,		// ����Ⱥ��
			REASON_REMOVE = 1,  // ȥ��Ⱥ��
			REASON_INFO_CHANGE = 2, // Ⱥ��������Ϣ�ı�
		};

		int reason;
		MsgRoleGroupInfo group;
	};

	class CSRoleInviteEnterGroup	// 3261 �����������Ⱥ��
	{
	public:
		CSRoleInviteEnterGroup() : header(MT_CHAT_GROUP_ROLE_INVITE_ENTER_CS) {}
		MessageHeader header;

		long long group_id;
		int be_invite_uid;
	};

	class CSEnterGroupReq			// 3262 ������������Ⱥ�� (�ܾ�ֱ�Ӻ�����Ϣ
	{
	public:
		CSEnterGroupReq() : header(MT_CHAT_GROUP_ROLE_ENTER_GROUP_CS) {}
		MessageHeader header;

		int is_cross;
		int group_server_id;
		int invite_uid;
		long long group_id;
	};

	class CSCreateGroupReq			// 3263 ����Ⱥ��
	{
	public:
		CSCreateGroupReq() : header(MT_CHAT_GROUP_CREATE_GROUP_CS) {}
		MessageHeader header;
	
		int is_cross;
		GameName group_name;
	};

	class CSGroupKickMember			// 3264 �Ƴ���ҳ�Ⱥ��
	{
	public:
		CSGroupKickMember() : header(MT_CHAT_GROUP_KICK_MEMBER_CS) {}
		MessageHeader header;

		long long group_id;
		int be_kicker;
	};

	class CSGroupDissmiss			// 3265 ��ɢȺ��
	{
	public:
		CSGroupDissmiss() : header(MT_CHAT_GROUP_DISSMISS_CS) {}
		MessageHeader header;

		long long group_id;
	};

	class CSGroupLeaveReq			// 3266 �뿪Ⱥ��
	{
	public:
		CSGroupLeaveReq() : header(MT_CHAT_GROUP_LEAVE_CS) {}
		MessageHeader header;

		long long group_id;
		int group_server_id;
	};

	class CSChangeGroupNoticeReq			// 3269 �޸�Ⱥ������
	{
	public:
		CSChangeGroupNoticeReq() : header(MT_CHAT_GROUP_CHANGE_NOTICE_CS) {}
		MessageHeader header;

		long long group_id;
		GameName group_name;
		GuildNotice group_notice;
	};

	class SCTempGroupSimpleInfo			// 3270 ��ʱȺ��ļ�����Ϣ�·�
	{
	public:
		SCTempGroupSimpleInfo() : header(MT_TEMP_CHAT_GROUP_SIMPLE_INFO_SC) {}
		MessageHeader header;

		MsgRoleGroupInfo group;
	};

	class SCTempGroupLeaveNotice	// 3271 ��ʱȺ���뿪֪ͨ
	{
	public:
		SCTempGroupLeaveNotice() : header(MT_TEMP_CHAT_GROUP_LEAVE_SC) {} ;
		MessageHeader header;
	};

	class SCChatGroupMsgNotice		// 3272 Ⱥ��֪ͨ��Ϣ��ʾ
	{
	public:
		SCChatGroupMsgNotice() : header(MT_CHAT_GROUP_MSG_NOTICE_SC) {};
		MessageHeader header;

		long long group_id;			// Ⱥ��ID��ֵΪ1ʱ��ʾ��ʱȺ��
		int msg_length;				// ֪ͨ��Ϣ����
		char msg_content[1024];		// ֪ͨ��Ϣ���� ����msg_length��ȡmsg_content)
	};

	class CSTempChatGroupBecomeOfficialReq	// 3273 ��ʱȺ��ת��
	{
	public:
		CSTempChatGroupBecomeOfficialReq() : header(MT_TEMP_CHAT_GROUP_BECOME_OFFICIAL_CS){};
		MessageHeader header;

		GameName group_name;	// Ⱥ����
	};

	class SCTempBecomeOfficialNotice	//!< 3274 ��ʱȺ��ת���ɹ�֪ͨ
	{
	public:
		SCTempBecomeOfficialNotice() : header(MT_TEMP_CHAT_GROUP_BECOME_OFFICIAL_NOTICE_SC) {}
		MessageHeader header;

		long long group_id;
	};

	class CSHandoverChatGroupLeadership	// 3275 �ƽ�Ⱥ��
	{
	public:
		CSHandoverChatGroupLeadership() : header(MT_CHAT_GROUP_HANDOVER_TO_OTHERS_CS) {}
		MessageHeader header;

		long long group_id;				// Ⱥ��ID
		int new_leader_uid;				// �ƽ�����Ⱥ��UID
	};

	class SCPlayerIsForbidden	// 2139 �Ƿ񱻽���
	{
	public:
		SCPlayerIsForbidden() : header(MT_REPLY_WHETHER_FORBIDEN_SC) {}
		MessageHeader header;

		short is_forbidden;		// �Ƿ񱻽��ԣ�1 ��ʾ�����ԣ�0 ��ʾû������
		short forbid_talk_type;	// ����ģʽ \see FORBID_TALK_TYPE_MAX
		unsigned int forbid_talk_end_timestamp;	// ���Խ���ʱ���
	};

	class SCChatRecordList	// 2147 �������
	{
	public:
		SCChatRecordList() : header(MT_CHAT_RECORD_LIST_SC) {}
		MessageHeader header;

		long long recent_chat_list[CHAT_CORD_MAX_LIST];
	};

	class CSChatRecordSet	// 2148 �����������
	{
	public:
		CSChatRecordSet() : header(MT_CHAT_RECORD_SET_CS) {}
		MessageHeader header;

		long long recent_chat_list[CHAT_CORD_MAX_LIST];
	};

	class CSChatSimpleRecordSet	// 2150 ��ݷ�������
	{
	public:
		CSChatSimpleRecordSet() : header(MT_CHAT_SIMPLE_RECORD_SET_CS) {}
		MessageHeader header;

		int index;
		GameName simple_data;
	};

	class SCChatSimpleRecordInfo	// 2149 ��ݷ����·�
	{
	public:
		SCChatSimpleRecordInfo() : header(MT_CHAT_SIMPLE_RECORD_LIST_SC) {}
		MessageHeader header;

		GameName simple_data_list[QUICK_SPEECH_MAX_NUM];
	};

	class SCMutePlayerDeleteChatRecord		//!< 2151 ɾ����������ҵ�����
	{
	public:
		SCMutePlayerDeleteChatRecord() : header(MT_DELETE_MUTE_PLAYER_CHAT_RECORD_SC) {}
		MessageHeader header;

		int role_id;	//!< �����Ե����
	};

	class CSUserChatRecord		//!< 2153 ��ѯ�û������¼ 
	{
	public:
		CSUserChatRecord() : header(MT_USER_CHAT_RECORD_CS) {}
		MessageHeader header;
	};
}

#pragma pack(pop)

#endif // __MSGCHATMSG_H__

