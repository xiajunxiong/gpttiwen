/******************************************************************************************
FileName: chatserverdef.hpp
Author: dreamwang
Description: ��¼��������ض��� 
Version: 1.0
History: 
<author>    <time>					<version >    <desc>
dreamwang   2011/08/08/ 09:04:05     1.0           ����
*****************************************************************************************/
#ifndef __LOGINDEF_H__
#define __LOGINDEF_H__

#include "common/tlvprotocol.h"

typedef char	PlatName[64];
typedef char	LoginStr[32];

enum CREATE_ROLE_RESULT_TYPE
{
	CREATE_ROLE_RESULT_TYPE_SUCC = 0,					// �ɹ�
	CREATE_ROLE_RESULT_TYPE_NO_SPACE = -1,				// ӵ�н�ɫ̫�࣬û�пռ��ٴ���ɫ
	CREATE_ROLE_RESULT_TYPE_EXIST_NAME = -2,			// ����
	CREATE_ROLE_RESULT_TYPE_NAME_INVALID = -3,			// ���ֲ��Ϸ�
	CREATE_ROLE_RESULT_TYPE_OLD_SERVER_NEW_USER = -4,   // �������Ѵ�����
	CREATE_ROLE_RESULT_TYPE_SERVER_ERROR = -5,			// ��������������
};
	

struct AccountInfo
{
	AccountInfo() : db_index(0), role_id_1(0), role_id_2(0), role_id_3(0), role_id_4(0), role_id_5(0),
		last_login_time(0u), create_time(0u), forbid_time(0u), is_anti_wallow(0) {}

	bool Serialize(TLVSerializer &out_stream) const
	{
		bool ret = out_stream.Pushf("iiiiiiiiib",
			db_index, role_id_1, role_id_2, role_id_3, role_id_4, role_id_5, last_login_time, create_time, forbid_time, is_anti_wallow);
		return ret;
	}

	bool Unserialize(TLVUnserializer &instream)
	{
		bool ret = instream.Popf("iiiiiiiiib",
			&db_index, &role_id_1, &role_id_2, &role_id_3, &role_id_4, &role_id_5, &last_login_time, &create_time, &forbid_time, &is_anti_wallow);

		return ret;
	}

	int db_index;
	int role_id_1;
	int role_id_2;
	int role_id_3;
	int role_id_4;
	int role_id_5;
	unsigned int last_login_time;
	unsigned int create_time;
	unsigned int forbid_time;
	char is_anti_wallow;
};

#endif
