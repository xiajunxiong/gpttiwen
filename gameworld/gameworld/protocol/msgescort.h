#ifndef __MSG_ESCORT_H__
#define __MSG_ESCORT_H__

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class SCEscortAllInfo					//!< ������Ϣ2220
	{
	public:
		SCEscortAllInfo();
		MessageHeader		header;

		int				today_count;			//!< ��������˼���
		int				task_seq;				//!< ����id
		int				npc_id;
		unsigned int	end_time;				//�ճ�����ڽ�����ʱ���  0����û�е���ʱ
	};

	class CSEscortOp						//!< ���ڲ���2221
	{
	public:
		CSEscortOp();
		MessageHeader		header;

		enum OP_TYPE
		{
			ACCEPT_TASK = 0,		//!< ������ param1:�������� param2:����ID(��������Ϊ����ʱ)
			COMMIT_TASK,			//!< �ύ����
			GIVEUP_TASK,			//!< ��������
		};

		int op_type;
		int param1;
		int param2;
	};

	class SCEscortResultNotic					//!< ���ڽ��֪ͨ2222
	{
	public:
		SCEscortResultNotic();
		MessageHeader		header;

		int is_win;//!<0Ϊʧ�� 1Ϊʤ��
	};

	class SCEscortStatusChangeNotice		// 2223 ����״̬�ı��֪ͨ��Χ���
	{
	public:
		SCEscortStatusChangeNotice();
		MessageHeader header;

		ObjID role_obj_id;		//!< ״̬�ı�����obj_id
		short reserve_sh;		//!< ������
		int escort_seq;			//!< ��������seq id
	};
}

#pragma pack(pop)

#endif

