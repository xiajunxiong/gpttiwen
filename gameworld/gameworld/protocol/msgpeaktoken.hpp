#ifndef __MSG_PEAK_TOKEN_HPP__
#define __MSG_PEAK_TOKEN_HPP__


#include "servercommon/userprotocal/msgheader.h"


#pragma pack(push) 
#pragma pack(4)


namespace Protocol
{
	//////����������
	//��Ҳ���  4455
	class CSPeaktokenRoleReq
	{
	public:
		CSPeaktokenRoleReq();
		MessageHeader		header;

		enum ReqType
		{
			REQ_TYPE_ROLE_INFO = 0,		//��ɫ����
			REQ_TYPE_ROLE_JOIN,			//���� p_1:�����obj_id
			REQ_TYPE_ROLE_FETCH,		//��ȡ���� p_1:�����obj_id
		};

		int req_type;		//��������
		int param_1;
	};
	//��ɫ����֪ͨ 4457
	class SCPeaktokenRoleStatusInfo
	{
	public:
		SCPeaktokenRoleStatusInfo();
		MessageHeader		header;

		long long peak_token_act_id;	//�id
		unsigned int role_join_time;	//����ʱ��
		int reward_times;				//��������

	};
	//��ɫ����֪ͨ 4458
	class SCPeaktokenRoleJoinInfo
	{
	public:
		SCPeaktokenRoleJoinInfo();
		MessageHeader		header;

		long long peak_token_act_id;	//�id
		unsigned int role_join_time;	//����ʱ��
		int reward_times;		//��������

		ItemID item_id;			//����id
		short num;				//��������
	};

	//�����Ϣ  4459
	class SCPeaktokenRoleInfo
	{
	public:
		SCPeaktokenRoleInfo();
		MessageHeader		header;

		int today_use_times;	//����ʹ�ô���
	};
};

#pragma pack(pop)

#endif