#ifndef __MSG_RA_GOD_TOKEN_HPP__
#define __MSG_RA_GOD_TOKEN_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/ragodtokenparam.hpp"

//////////////////////////////////////////// 2081 ����  ����ս��  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// ��Э��  3000
	enum RA_GOD_TOKEN_OPERA_TYPE
	{
		RA_GOD_TOKEN_OP_TYPE_INFO = 0,					// ��Ϣ�·�
		RA_GOD_TOKEN_OP_TYPE_FETCH_COMMON = 1,			// ��ȡ��ͨ���� param1 - seq
		RA_GOD_TOKEN_OP_TYPE_FETCH_SENIOR = 2,			// ��ȡ�߼����� param1 - seq
		RA_GOD_TOKEN_OP_TYPE_BUY_EXP = 3,				// ������		param1 - count
		RA_GOD_TOKEN_OP_TYPE_EXCHANGE = 4,				// �һ�			param1 - seq
		RA_GOD_TOKEN_OP_TYPE_FETCH_SURPLUS = 5,			// ��ȡ�������
		RA_GOD_TOKEN_OP_TYPE_FETCH_DAILY_TASK = 6,		// ��ȡÿ�������� param1 - seq
		RA_GOD_TOKEN_OP_TYPE_FETCH_WEEKLY_TASK = 7,		// ��ȡÿ�������� param1 - seq
	};


	struct SCRAGodTokenInfo					//!<��3039 ����ս�������·�
	{
	public:
		SCRAGodTokenInfo();
		MessageHeader header;

		char time_seq;
		char is_open;
		char is_buy_token;
		char sh_reserver; //����

		short token_level;
		short token_exp;

		int time_level;

		BitMap<RA_GOD_TOKEN_LEVEL_BIT_INFO> fetch_common_flag;				//��ͨ������ȡ��־λ��RA_GOD_TOKEN_LEVEL_BIT_INFO 64��
		BitMap<RA_GOD_TOKEN_LEVEL_BIT_INFO> fetch_senior_flag;				//�߼�������ȡ��־λ��RA_GOD_TOKEN_LEVEL_BIT_INFO 64��

		short daily_level;
		char daily_type[RA_GOD_TOKEN_DAILY_TASK_NUM];						//ÿ���������� RA_GOD_TOKEN_DAILY_TASK_NUM-6
		int daily_type_finish_count[RA_GOD_TOKEN_DAILY_TASK_NUM];			//ÿ������������ɴ��� RA_GOD_TOKEN_DAILY_TASK_NUM-6  -1��������ȡ

		short weekly_level;
		char weekly_type[RA_GOD_TOKEN_WEEKLY_TASK_NUM];						//ÿ���������� RA_GOD_TOKEN_WEEKLY_TASK_NUM-10
		int weekly_type_finish_count[RA_GOD_TOKEN_WEEKLY_TASK_NUM];			//ÿ������������ɴ��� RA_GOD_TOKEN_WEEKLY_TASK_NUM-10  -1��������ȡ

		short surplus_exp;													//�������
		short surplus_receive_count;										//�����������ȡ����

		int count;
		int exchange_count[RA_GOD_TOKEN_EXCHANGE_MAXNUM];					//�һ�����
	};

}

#pragma pack(pop)

#endif  // __MSG_RA_GOD_TOKEN_HPP__
