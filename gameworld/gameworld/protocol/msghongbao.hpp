#ifndef __MSG_HONG_BAO_H__
#define __MSG_HONG_BAO_H__

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"
#include "servercommon/hongbaodef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class CSHongBaoClientReq					//!< 4000 ��� - �ͻ�������
	{
	public:
		CSHongBaoClientReq();
		MessageHeader header;

		enum OPERA_TYPE
		{
			OPERA_TYPE_SEND_LIST_REQ = 0,		//!< 0 ����ɷ����б���Ϣ		[]
			OPERA_TYPE_GET_LIST_REQ = 1,		//!< 1 �������ȡ�б���Ϣ		[param_1:���ϵͳ����]
			OPERA_TYPE_BASE_INFO_REQ = 2,		//!< 2 ���󵥸��������ϸ��Ϣ	[param_1:���ϵͳ���� , param_2:���ID]
			OPERA_TYPE_SEND_HONG_BAO = 3,		//!< 3 ����� [param_1:���ϵͳ���� , param_2:������� , greeting: ���ף����]
			OPERA_TYPE_GET_HONG_BAO = 4,		//!< 4 ���� [param_1:���ϵͳ���� , param_2:���ID]		
		};

		int req_type;							//!< ��Ϣ����  \see OPERA_TYPE
		int param1;
		int param2;
		HongBaoGreeting greeting;				// ���ף����
	};


	class SCHongBaoSendList						//!< 4001 ��� - �ɷ����б���Ϣ
	{
	public:
		SCHongBaoSendList();
		MessageHeader header;

		struct SendInfo
		{
			SendInfo():type(0), count(0), day_count(0), total_count(0){}

			int type;							// �ɷ��͵ĺ������
			int count;
			int day_count;						// ÿ���ѻ�õĿɷ��ͺ������
			int total_count;					// �ۼ��ѻ�õĿɷ��ͺ������
		};

		int count;								// �ɷ����б��
		SendInfo send_list[MAX_HONG_BAO_TYPE_COUNT];
	};

	class SCHongBaoGetList						//!< 4002 ��� - ����ȡ�б���Ϣ
	{
	public:
		SCHongBaoGetList();
		MessageHeader header;

		struct GetInfo
		{
			GetInfo() :hongbao_id(0), send_avatar_type(0), send_headshot_id(0)
			{
				memset(send_name, 0, sizeof(send_name));
			}

			int hongbao_id;								// ���ID
			int send_avatar_type;						// ����������ģ��
			int send_headshot_id;						// ������ͷ��ID
			GameName send_name;							// ����������
		};

		int hongbao_sys_type;							// ���������ϵͳ����
		int count;										// ��
		// ����ӿ���ȡ�ĺ���� �� count �� GetInfo ���
	};

	class SCHongBaoBaseInfo								//!< 4003 ��� - ��ϸ��Ϣ
	{
	public:
		SCHongBaoBaseInfo();
		MessageHeader header;

		int hong_bao_id;								// ���ID
		unsigned int invalid_timestamp;					// ���ʧЧʱ��

		int hongbao_type;								// �������	[�߻����õ�����]
		int hongbao_sys_type;							// ��������ڵ�ϵͳ����	[����:����,����ȵ�..]

		int send_uid;									// ������˵�uid
		GameName send_name;								// ������˵�����
		HongBaoGreeting greeting;						// ���ף����

		int used_val;									// ��ʹ�õĽ��

		int get_count;									// ��ȡ�������Ա����
		HongBaoItem::RewardInfo get_info[MAX_HONG_BAO_GET_MEMBER_COUNT];	// ��ȡ�������Ա��Ϣ
	};

	class SCHongBaoSendListChange						//!< 4004 ��� - �ɷ����б���Ϣ���
	{
	public:
		SCHongBaoSendListChange();
		MessageHeader header;

		int hongbao_type;								// �������
		int num;										// �������
		int day_count;									// ÿ���ѻ�õĿɷ��ͺ������
		int total_count;								// �ۼ��ѻ�õĿɷ��ͺ������
	};

	class SCHongBaoGetListChange						//!< 4005 ��� - ����ȡ�б���Ϣ���
	{
	public:
		SCHongBaoGetListChange();
		MessageHeader header;

		enum CHANGE_TYPE
		{
			CHANGE_TYPE_ADD = 0,						//!< 0 ���µĿ���ȡ��		[change_type]
			CHANGE_TYPE_DEL = 1,						//!< 1 �к����ʧʱ�䵽��	[change_type]
		};

		struct GetInfo
		{
			GetInfo() :hongbao_id(0), send_avatar_type(0), send_headshot_id(0)
			{
				memset(send_name, 0, sizeof(send_name));
			}

			int hongbao_id;								// ���ID
			int send_avatar_type;						// ����������ģ��
			int send_headshot_id;						// ������ͷ��ID
			GameName send_name;							// ����������
		};
	
		int change_type;								// �������
		int hongbao_sys_type;							// ���ϵͳ����
		GetInfo info;									// �����Ϣ
	};

	class SCHongBaoBaseInfoChange						//!< 4006 ��� - ��ϸ��Ϣ���
	{
	public:
		SCHongBaoBaseInfoChange();
		MessageHeader header;

		enum CHANGE_TYPE
		{
			CHANGE_TYPE_ADD = 0,						//!< 0 ���µ���ȡ��			[change_type]
			CHANGE_TYPE_DEL = 1,						//!< 1 ���ʱ�䵽��			[change_type]
		};

		int change_type;								// �������

		int hong_bao_id;								// ���ID
		unsigned int invalid_timestamp;					// ���ʧЧʱ��

		int hongbao_type;								// �������	[�߻����õ�����]
		int hongbao_sys_type;							// ��������ڵ�ϵͳ����	[����:����,����ȵ�..]

		int used_val;									// ��ʹ�õĽ��

		HongBaoItem::RewardInfo get_info;				// ��ȡ����Ϣ	
	};
}

#pragma pack(pop)

#endif	//__MSG_HONG_BAO_H__

