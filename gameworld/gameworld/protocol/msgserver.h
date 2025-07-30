#ifndef __MSGSERVER_H__
#define __MSGSERVER_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/serverdef.h"
#include "servercommon/logindef.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class CSEnterGlobalServerReq
	{
	public:
		CSEnterGlobalServerReq();
		MessageHeader header;

		int	scene_id;					// ���ֶζ�globalserverû��ʲô���� ����������֤key
		int	scene_key;					// ͬ��
		int	last_scene_id;
		int	role_id;
		GameName role_name;
		int time;
		SessionKey key;
		PlatName pname;
		short server;
		short is_micro_pc;
	};

	enum __EnterGolbalServerResult
	{
		EGS_RESULT_SUC = 0,
		EGS_USER_EXIST = -1,
	};
	
	class SCEnterGlobalServerAck
	{
	public:
		SCEnterGlobalServerAck();
		MessageHeader header;

		int result;
	};

	class CSTimeReq		//!< 9051 ����ʱ��
	{
	public:
		CSTimeReq();
		MessageHeader header;
	};

	class SCTimeAck		//!< 9001 ����ʱ��
	{
	public:
		SCTimeAck();
		MessageHeader header;

		unsigned int server_time;				//!< ��������ǰʱ���
		unsigned int server_real_start_time;	//!< �������״ο���ʱ���
		int open_days;							//!< �������ѿ�������
		unsigned int server_real_combine_time;	//!< �������Ϸ�ʱ���
		unsigned int server_zero_start_time;	//!< ���������賿0��Ϊ��׼������ķ���������ʱ��
		char is_server_driving_send;			//!< 1:����������·�
		char reserve_ch;
		short reserve_sh;
	};

	class SCWallowNotice
	{
	public:
		SCWallowNotice();
		MessageHeader header;

		int notice_type;
	};

	class SCTimeDateInfo		//!< 9004 �����·�
	{
	public:
		SCTimeDateInfo();
		MessageHeader header;

		unsigned int time;
		unsigned short year;
		unsigned short mon;
		unsigned char day;
		unsigned char hour;
		unsigned char min;
		unsigned char sec;
	};

	class SCChangeThread
	{
	public:
		SCChangeThread();
		MessageHeader header;
		int game_thread;
	};

	class SCDisconnectNotice
	{
	public:
		SCDisconnectNotice();
		MessageHeader header;

		int reason;
	};

	class SCCrossConnectInfo		//9005 ���������Ϣ�·�
	{
	public:
		SCCrossConnectInfo();
		MessageHeader header;

		char is_connected_cross;		//0: δ���� 1��������
		char is_cross;					//0: ��ԭ�� 1���ڿ��
		short reserve_sh;
	};

}

#pragma pack(pop)

#endif

