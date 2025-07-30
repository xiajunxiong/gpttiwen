#ifndef __MSG_RA_TRUN_TABLE_HPP__
#define __MSG_RA_TRUN_TABLE_HPP__

#include "servercommon/userprotocal/msgheader.h"

//////////////////////////////////////////// 2051 ����  ����� -- ת�̻  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// ��Э��  3000
	enum RA_TRUN_TABLE_OPERA_TYPE
	{
		RA_TRUN_TABLE_OPERA_TYPE_GET_INFO = 0,						// ��ȡ��Ϣ
		RA_TRUN_TABLE_OPERA_TYPE_FREE_TRUN_TABLE = 1,				// ���ת�� 	
		RA_TRUN_TABLE_OPERA_TYPE_KILL_BOSS_TRUN_TABLE = 2,			// ����BOSS��ɱ��ת��
		RA_TRUN_TABLE_OPERA_TYPE_GET_REWARD = 3,					// ��ȡת�̽���  param_1:   param_2:  
	};

	class SCRandActivityTurnTableInfo								//!<3003 ����-����ת��
	{
	public:
		SCRandActivityTurnTableInfo();
		MessageHeader header;

		int ra_has_free_turn_table_num;								// ���ת�̴���    �������Ϊ1,������ż�����ʱ
		int kill_boss_num;											// ��ǰBOSS��ɱ��
		unsigned int next_refresh_time;								// �´����ת��ˢ��ʱ��
	};

	class SCRandActivityTurnTableReward								//!<3004 ����-����ת��-������
	{
	public:
		SCRandActivityTurnTableReward();
		MessageHeader header;

		int index;
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_TRUN_TABLE_HPP__
