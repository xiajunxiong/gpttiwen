#ifndef __DUEL_DEF_H__
#define __DUEL_DEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"

#pragma pack(push, 4)

enum DUEL_BATTLE_TYPE
{
	DUEL_BATTLE_TYPE_BEGIN = 0,

	DUEL_BATTLE_TYPE_DUEL = 1,					// �������졢�״������
	DUEL_BATTLE_TYPE_ADJUDICATION = 2,			// �þ� (�졢�״����)
	DUEL_BATTLE_TYPE_ESCORT = 3,				// ���ڣ��졢�״�������ң�����������ۺ�ף�����Ϊ������ң���

	DUEL_BATTLE_TYPE_END,
};

struct DuelParam
{
	DuelParam()
	{
		this->Reset();
	}

	void Reset()
	{
		kill_value = 0;
		use_reduce_item_num = 0;
		kill_title_id = 0;
		kill_titel_stage = -1;

		last_reduce_time = 0;

		escort_task_seq = 0;
		escort_npc_id = 0;
	}

	int kill_value;							// ɱ¾ֵ(���ֵ)
	int use_reduce_item_num;				// ʹ�ü���ɱ¾���ߴ���
	int kill_title_id;						// ��ǰ�����ɱ¾�ƺ�
	int kill_titel_stage;					// ��ǰ�����ɱ¾�ƺŽ׶Σ��п��ܳƺ�ID��ͬ���׶β�ͬ����Ҫ�����ԣ�

	unsigned int last_reduce_time;

	//����
	int escort_task_seq;
	int escort_npc_id;
};

typedef char DuelParamHex[sizeof(DuelParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(DuelParamHex) < 256);

#pragma pack(pop)
#endif
