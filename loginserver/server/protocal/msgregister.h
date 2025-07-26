
#ifndef MSGREGISTER_H
#define MSGREGISTER_H

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/logindef.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	/*
		���������ٽ�ɫ���
	*/

	class CSCreateRole							//!< 7150 ��������
	{
	public:
		CSCreateRole();
		MessageHeader		header;

		PlatName			pname;				//!< �˻���(��spid)���� dev_aaa123
		GameName			role_name;			//!< ��ɫ��
		unsigned int		login_time;			//!< php�·�
		LoginStr			login_str;			//!< php�·�
		short				server;				//!< ����
		char				avatar_type;		//!< ������������(����ͷ������char���ɣ�
		char				color;				//!< ��ɫ
		long long			name_str_id;		//!< ���ֵ�hash id
		int					job_id;				//!< ְҵ
		char				plat_spid_str[4];	//!< �ͻ���spid
		int					mini_game_id;		//!< С��ϷID
		char                is_auto_rand_name;	//!< �Ƿ����������(0:���Զ� 1:�Զ�) 
		char				audit_server_seq;	//!< ��˷�seq
		short				reserve_sh;
	};

	class SCCreateRoleAck						//!< 7100 ���Ƿ���
	{
	public:
		SCCreateRoleAck();
		MessageHeader		header;
		int					result;				//!< ���
		int					role_id;			//!< ��ɫID
		GameName			role_name;			//!< ��ɫ����
		short				reserve_sh;			//!< ������
		char				avartar_type;		//!< ������������
		char				color;				//!< ��ɫ
		long long			create_role_time;	//!< ������ɫʱ��
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
