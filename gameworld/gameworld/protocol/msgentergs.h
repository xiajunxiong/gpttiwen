
#ifndef MSGENTERGS_H
#define MSGENTERGS_H

#include "gamedef.h"
#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/logindef.h"


#pragma pack(push) 
#pragma pack(4)


namespace Protocol
{
	/* 
	���볡����������أ��Լ��Ƴ���Ϸ����
	*/
	class CSUserEnterGSReq					//!< 1050 ������볡��Э�� ���״�������Ϸ��ʱ����
	{
	public:
		CSUserEnterGSReq();
		MessageHeader	header;				
		int				scene_id;			//!< ����ID
		int				scene_key;			//!< ����Key
		int				last_scene_id;		//!< ��һ�γ���ID
		int				role_id;			//!< ��Ϸ��ɫID
		GameName		role_name;			//!< ��ɫ��
		int				time;				//!< ʱ��� (SCLoginToRole�·���time����ԭ��������)
		char			is_login;			//!< ��½���ǳ����л� (��1���ɣ�������û�жೡ�����ļܹ�������һ���ǵ�¼��
		char			reserved1;			//!< ������
		short			server;				//!< ��¼�ķ�����ID
		SessionKey		key;				//!< ��¼���·�����֤key
		PlatName		pname;				//!< ��spid���˺���
		int				is_micro_pc;		//!< ��0
		char			plat_spid_str[4];	//!< ƽ̨������ID 
	};

	class SCServerBusy  //!< 1002 ������æ
	{
	public:
		SCServerBusy();
		MessageHeader	header;
	};

	enum __EnterGSResult
	{
		ENTERGS_RESULT_SUC=0,
		ENTERGS_USER_EXIST=-1,
		ENTERGS_RESULT_SCENE_NO_EXIST=-2,
		ENTERGS_USER_NOT_ALLOWED = -3,
		ENTERGS_SERVER_NOT_READY = -4,	// ��������δ��ȫ���������Ժ�
	};
	class SCUserEnterGSAck				//!< 1000 ���볡������ 
	{
	public:
		SCUserEnterGSAck();
		MessageHeader	header;
		int				result;			//!< ���  \see __EnterGSResult
		int				is_in_cross;	//!< �Ƿ��ڿ���� 0����  1����
	};

	class SCChangeGS    //��ʱ����
	{
	public:
		SCChangeGS();
		MessageHeader	header;
		int				scene_id;
		int				scene_key;
		int				last_scene_id;
		int				uid;
		unsigned int	time;
		SessionKey		key;
		unsigned short	gs_index;
		unsigned short	is_login;
	};

	class CSUserLogout		//!< 1051 �ǳ�����
	{
	public:
		CSUserLogout();
		MessageHeader	header;
	};

	class CSTransportReq	//!< 1052 �Ӵ��͵��л�����
	{
	public:
		CSTransportReq();
		MessageHeader	header;
		int				door_id;	//!< ������id
	};

	class CSSpecialTransportReq	//!< 1055 �ر��߼�����
	{
	public:
		CSSpecialTransportReq();
		MessageHeader	header;
		
		ObjID obj_id;			//Ҫʹ���Ǹ������ŵ�obj_id
		short reserved;
	};

	class CSTreasureChestReq	//!< 1056 ��������
	{
	public:
		CSTreasureChestReq();
		MessageHeader	header;

		ObjID obj_id;			//����objid
		short reserved;
	};
}


#pragma pack(pop)


#endif


