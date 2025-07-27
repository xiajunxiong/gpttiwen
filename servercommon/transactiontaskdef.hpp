#ifndef __TRANSACTION_TASK_DEF_HPP__
#define __TRANSACTION_TASK_DEF_HPP__

#include "servercommon/servercommon.h"
#include "bitmapfunction.hpp"

#pragma pack(push, 4)

static const int TRANSACTION_TASK_REWARD_MAX_NUM = 9;
static const int TRANSACTION_TASK_COUNT_MAX_NUM = 40;
static const int MAX_TRANSACTION_TASK_GROUP_RATE_COUNT_NUM = 10000;				//������ֵ
static const int MAX_GROUP_ID_NUM = 20;
static const int MAX_GROUP_SEQ_NUM = 30;

enum TRANSACTION_TASK_STATUS
{
	TRANSACTION_TASK_STATUS_ONGOING = 0,	// ������
	TRANSACTION_TASK_STATUS_FINISH,			// �����
};

struct TransactionTaskParam
{
	TransactionTaskParam() { this->Reset(); }
	void Reset()
	{
		task_id = 0;
		task_status = 0;
		level = 0;
		type = 0;
		memset(seq, 0, sizeof(seq));
		concondition_flag.Reset();
		version = 0;
		find_count = 0;
	}
	void DayReset()
	{
		task_id = 0;
		task_status = 0;
		level = 0;
		type = 0;
		memset(seq, 0, sizeof(seq));
		concondition_flag.Reset();
		version = 0;
	}

	int task_id;													//����ID
	int task_status;												//����״̬ 0:������ 1:�����
	short level;													//��ȡ����ʱ�ĵȼ�
	short type;														//type����һ���ȼ��鷶Χ ������鿴����
	short seq[TRANSACTION_TASK_REWARD_MAX_NUM];						//���9����Ӧ������seq   �������Ӧ
	short version;													//�汾��
	BitMap<TRANSACTION_TASK_REWARD_MAX_NUM> concondition_flag;		//��Ʒ��״̬
	int find_count;													//�����һش���
};

typedef char TransactionTaskParamHex[sizeof(TransactionTaskParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(TransactionTaskParamHex) < 128);

#pragma pack(pop)

#endif