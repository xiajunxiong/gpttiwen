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
		
		///////////////////////////////////////// ��¼��������� /////////////////////////////////////////
		MT_LOGIN_TO_ROLE_SC = 7000,					// ��¼��ɫ���󷵻أ����ؽ�ɫ���ڳ�����Ϣ���Լ�Ҫ���ӵ����صȣ�
		MT_LOGIN_TO_ACCOUNT_SC = 7001,				// ��¼�˺����󷵻أ������˻���Ϣ��
		MT_GET_THREAD_ACK_SC = 7002,				// �鿴������״̬����
		MT_ANTI_WALLOW_EX_RET_SC = 7003,			// ��������֤�����Է���
		MT_ACCOUNT_KEY_ERROR_SC = 7004,				// KEY����֪ͨ
		MT_MERGE_ROLE_LIST_SC = 7005,				// �Ϸ���Ľ�ɫ�б�
		MT_PROF_NUM_INFO_SC = 7006,					// ְҵ������Ϣ��
		MT_L_HEARTBEAT_SC = 7007,					// ����������

		MT_L_HEARTBEAT_CS = 7052,					// ������
		MT_GET_TREAD_CS = 7053,						// �鿴������������Ϣ
		MT_ANTI_WALLOW_CS = 7054,					// ͨ��������
		MT_ANTI_WALLOW_EX_CS = 7055,				// �����Լ���
		MT_LOGIN_TO_ACCOUNT_CS = 7056,				// ��¼�˺�����
		MT_LOGIN_TO_ROLE_CS = 7057,					// ��¼��ɫ����
		
		///////////////////////////////////////// ��ɫ��� /////////////////////////////////////////
		MT_CREATE_ROLE_ACK_SC = 7100,				// ������ɫ����
		MT_DESTROY_ROLE_ACK_SC = 7101,				// ���ٽ�ɫ����
		MT_CHANGE_PLATNAME_ACK_SC = 7102,			// �޸�ƽ̨������ ���ض�ģʽ��ʹ�ã�

		MT_CREATE_ROLE_REQ_CS = 7150,				// ������ɫ
		MT_DESTROY_ROLE_REQ_CS = 7151,				// ���ٽ�ɫ
		MT_CHANGE_PLATNAME_REQ_CS = 7152,			// �޸�ƽ̨��

		MT_GET_RANDOM_NAME_CS = 7153,				// ��ȡ�����������
		MT_GET_RANDOM_NAME_SC = 7154,				// ��ȡ������ַ���

		MT_STORY_BATTLE_INFO_CS = 7155,				// ��ȡ���������Ϣ
		MT_STORY_ROUND_RESULT_CS = 7156,			// ��ȡ����غ�ս��
		MT_STORY_NOT_AVALIABLE_SC = 7157,			// �����ݲ�����֪ͨ
	};
}

#endif // __LOGINMAGCODE_H__
