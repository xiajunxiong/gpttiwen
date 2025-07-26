
#ifndef MSGLOGIN_H
#define MSGLOGIN_H

#include "servercommon/serverdef.h"
#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/logindef.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	// ��½���
	class CSLoginToAccount
	{
	public:
		CSLoginToAccount();
		MessageHeader		header;
		int					rand_val1;		// ����� 100w-1000w
		unsigned int		login_time;
		LoginStr			login_str;
		PlatName			pname;
		int					rand_val2;		// ����� 100w-1000w
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
		int					role_id;		//!< ��ɫid
		GameName			role_name;		//!< ��ɫ����
		int					profession;		//!< ��ɫְҵ
		short				avatar_type;	//!< ��ɫģ��
		short				color;			//!< ��ɫ��ɫ
		short				level;			//!< ��ɫ�ȼ�
		short				headshot_id;	//!< ��ɫͷ��
		int					weapon_id;		//!< ����ID
		int					top_level;		//!< �۷�ȼ�
		short				fly_flag;		//!< �����׶�
		short				reserve_sh;
		short				surface_list[SURFACE_TYPE_MAX]; //�����еĻû���Դ
		unsigned int		forbid_time;	//!< ������ʱ��� Ϊ0��С�ڵ�ǰʱ����û�б�������ѱ����
	};

	class SCLoginToAccount	// 7001 ��¼�����˺���Ϣ
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

		int					rand_val1;		// ����� 100w-1000w
		unsigned int		login_time;
		LoginStr			login_str;
		PlatName			pname;
		short				server;
		char				reserved1;
		char				anti_wallow;

		int					rand_val2;		// ����� 100w-1000w
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
		char				gs_hostname[64];	// �޸ĸó��ȵ�ͬʱ�������޸�lwprotocal::WLRegisterGW::hostname�ĳ���
		unsigned short		gs_port;
		unsigned short		gs_index;
		unsigned int		server_time;
		GameName			role_name;
		char				backup_gs_hostname[64];	// ����gateway hostname
		unsigned short		backup_gs_port;			// ����gateway �˿�
		short				reserve_sh;
		unsigned int		create_time;
	};

	class CSRandomNameReq
	{
	public:
		CSRandomNameReq();
		MessageHeader		header;

		int					sex;			// ��ɫ�Ա�
	};

	class SCRandomNameRet	//!< 7154 ��ȡ�������
	{
	public:
		SCRandomNameRet();
		MessageHeader header;

		short front_sex;	//!< ǰ׺�Ա�
		short front_id;		//!< ǰ׺����id
		short middle_sex;	//!< ��׺�Ա�
		short middle_id;	//!< ��׺����id
		short last_sex;		//!< ��׺�Ա�
		short last_id;		//!< ��׺����id
		long long name_str_id; //!< ����id (����ʱ����ʹ�÷�����ṩ��������֣���ԭ�����������)
	};

	class CSLHeartBeat		// 7052 ��������
	{
	public:
		CSLHeartBeat();
		MessageHeader		header;
	};

	class SCLHeartBeat		// 7007 ��������
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
		int					ret;		// 0 �ɹ� -1���֤��Ч -2���֤�ظ� -3ʧ��
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

	class CSStoryBattleInfo	// 7155  ��ȡ���������Ϣ�����·�SCBattleInfo��
	{
	public:
		CSStoryBattleInfo();
		MessageHeader header;
	};

	class CSStoryRoundResult // 7156 ��ȡ����ս����Ϣ�����·�ս����
	{
	public:
		CSStoryRoundResult();
		MessageHeader header;

		int round;		// ��0��ʼ��һ�������غ�
	};

	class SCStoryNotAvaliable // 7157 �����ݲ�����֪ͨ
	{
	public:
		SCStoryNotAvaliable();
		MessageHeader header;
	};
}

#pragma pack(pop)

#endif

