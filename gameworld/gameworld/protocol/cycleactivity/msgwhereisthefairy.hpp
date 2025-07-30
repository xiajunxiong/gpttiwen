#ifndef __MSG_WHERE_IS_THE_FAIRY_HPP__
#define __MSG_WHERE_IS_THE_FAIRY_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/servercommon.h"

#pragma pack(push, 4)

namespace Protocol
{
	class CSRoleWhereIsTheFairyOperaReq			//!< 4730 ����ȥ�Ķ���������
	{
	public:
		CSRoleWhereIsTheFairyOperaReq();
		MessageHeader header;

		int opera_req_type;
	};

	enum CA_WHERE_IS_THE_FAIRY_OPERA_REQ_TYPE
	{
		CA_WHERE_IS_THE_FAIRY_OPERA_REQ_TYPE_INFO = 0,			//�����Ϣ����
		CA_WHERE_IS_THE_FAIRY_OPERA_REQ_TYPE_FINISH_GAME = 1,	//�����Ϸ����
		CA_WHERE_IS_THE_FAIRY_OPERA_REQ_TYPE_MANAGER_INFO = 2,	//����������Ϣ
	};

	class SCRoleWhereIsTheFairyInfo		//!< 4731 ����ȥ�Ķ������Ϣ�·�
	{
	public:
		SCRoleWhereIsTheFairyInfo();
		MessageHeader header;

		char is_day_fetch_where_is_the_fairy_flag;	// �����Ƿ���ȡ������ȥ�Ķ��Ľ��� 0:δ��ȡ
		char reserve_ch;
		short reserve_sh;
	};

	class SCWhereIsTheFairyInfo			//!< 4732 ����ȥ�Ķ�������Ϣ�·�
	{
	public:
		SCWhereIsTheFairyInfo();
		MessageHeader header;

		int scene_id;							//�������ĸ�����
		unsigned int next_refresh_timestamp;	//�����´�ˢ��ʱ���
	};

}

#pragma pack(pop)

#endif