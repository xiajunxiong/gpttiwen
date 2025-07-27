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
		MT_USER_LOGIN_ACK_SC = CHATCTRL_SC_BEGIN,			// �û���¼����
		MT_USER_BE_DESTROY_SC,								// ֪ͨ�û�������
		MT_INTO_CHANNEL_SC,									// �û�����Ƶ��
		MT_OUT_OF_CHANNEL_SC,								// �û��뿪Ƶ��
		MT_NOTICE_ONLINE_SC,								// ��������
		MT_NOTICE_OFFLINE_SC,								// ��������
		MT_NOTICE_ONLINE_STATUS_SC,							// ����״̬�仯����		

		MT_USER_LOGIN_CS = CHATCTRL_CS_BEGIN,				// �û������¼���������
		MT_MUTE_USER_CS,									// ����ĳ�û�
		MT_UNMUTE_USER_CS,									// ȡ������
		MT_CHANGE_ONLINE_STATUS_CS,							// �û��������״̬

		////////////////////////////////////////////////////////////////////////////////////////////////////////


	};
}

#endif // __CHATMSGCODE_H__

