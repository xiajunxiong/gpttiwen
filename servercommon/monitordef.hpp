#ifndef __MONITORDEF_HPP__
#define __MONITORDEF_HPP__

#include "servercommon/servercommon.h"

enum MONITOR_TYPE
{
	MONITOR_TYPE_DAY_GOLD_GET,								// ÿ�ո����Ԫ������ (�ǳ�ֵ�ֶ�)
	MONITOR_TYPE_DAY_GOLD_CONSUME,							// ÿ�ո����Ԫ������
	MONITOR_TYPE_DAY_COINBIND_GET,							// ÿ�ո����ͭ�ҽ���
	MONITOR_TYPE_DAY_COINBIND_CONSUME,						// ÿ�ո����ͭ������
	MONITOR_TYPE_DAY_IMMORTALCOIN_GET,						// ÿ�ո�����ɱҽ���
	MONITOR_TYPE_DAY_IMMORTALCOIN_CONSUME,					// ÿ�ո�����ɱ�����
	MONITOR_TYPE_FETCH_ATTACHMENT_NUM,						// ��ȡ�ʼ�
	MONITOR_TYPE_MAX,
};

static const int MONITOR_LOG_INTERVAL = 1800;				// �����־���

#pragma pack(push, 4)

struct MonitorParam
{
	MonitorParam() { this->Reset(); }

	void Reset()
	{
		next_monitor_time = 0;
		memset(monitor_num_list, 0, sizeof(monitor_num_list));
	}

	unsigned int next_monitor_time;
	int monitor_num_list[MONITOR_TYPE_MAX];
};

typedef char MonitorParamHex[sizeof(MonitorParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(MonitorParamHex) < 256);

#pragma pack(pop)

#endif	// __MONITORDEF_HPP__
