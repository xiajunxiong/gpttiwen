#ifndef __MSG_LEVEL_COMPLEMENT_H__
#define __MSG_LEVEL_COMPLEMENT_H__

#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push, 4)

namespace Protocol
{
	class SCLevelComplementInfo					//!< 3562 - ������Ϣ
	{
	public:
		SCLevelComplementInfo();
		MessageHeader		header;

		long long accumulated_exp;					//!< �����
		unsigned int m_refresh_accumulated_tamp;	//!< �´����Ӿ����ʱ���
		unsigned int m_end_accumulate_tamp;			//!< ��������ʱ���

		int m_day_difference;						//!< �����������
		int m_add_exp_times;						//!< ��ˢ�´���

		long long m_alread_get_exp;					//!< �ۻ���ȡ����
		long long m_save_exp;						//!< �ۻ���ô洢����
		long long m_today_accumulated_exp;			//!< ����ɻ�ȡ����ֵ
	};

	class CSLevelComplementReq				//!< 3563 - ������Ϣ
	{
	public:
		enum LEVEL_COMPLEMENT_TYPE
		{
			LEVEL_COMPLEMENT_TYPE_0 = 0,//�����·�
			LEVEL_COMPLEMENT_TYPE_1 = 1,//������
		};
		CSLevelComplementReq();
		MessageHeader		header;

		int type;				// !< ��������
		int param;				// !< ����
	};

	class CSRoleNewLevelComplementReq		//!< 3566 �µȼ���������
	{
	public:
		CSRoleNewLevelComplementReq();
		MessageHeader header;

		enum REQ_TYPE
		{
			REQ_TYPE_INFO = 0,			// �����ɫ������Ϣ�·�
			REQ_TYPE_BUY_EXP = 1,		// �������ɫ����
		};

		int req_type;
	};

	class SCRoleNewLevelComplementInfo		//!< 3567 �µȼ�������ɫ��Ϣ�·�	
	{
	public:
		SCRoleNewLevelComplementInfo();
		MessageHeader header;

		long long accumulation_exp;					//�ۻ�����
		long long day_supplement_exp;				//�����賿����ľ�����ֵ
		unsigned int last_get_daily_exp_timestamp;	//�ϴλ���ճ�����ʱ���(���ǵ���)
		short day_buy_exp_count;					//���칺�������
		short level_difference;						//�ȼ���ֵ
	};

	class SCNewLevelComplementSpeicalInfo	//!< 3568 �µȼ�����������Ϣ�·�
	{
	public:
		SCNewLevelComplementSpeicalInfo();
		MessageHeader header;

		int my_server_average_level;		// ����ǰ��������ƽ���ȼ�
	};
}

#pragma pack(pop)
#endif