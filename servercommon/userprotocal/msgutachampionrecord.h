#ifndef __MSG_UTA_CHAMPION_RECORD_H__
#define __MSG_UTA_CHAMPION_RECORD_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/userprotocal/msgcode.h"
#include "servercommon/commondata.hpp"

#include "servercommon/struct/bigcross/utachampionrecordparam.hpp"


#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	struct CSGetUTAChampionRecord				// 4462  �ͻ��˻�ȡ �ھ���¼�ֲ�[���]
	{
		CSGetUTAChampionRecord() :header(MT_GET_UTA_CHAMPION_RECORD_CS) {}
		MessageHeader header;

		int begin_index;
		int end_index;
	};

	struct SCGetUTAChampionRecordAck			// 4463 ���� �ھ���¼�ֲ�[���] ��Ϣ
	{
		struct UTACRInfo
		{
			UTACRInfo() 
			{
				rank = -1;
				memset(team_name, 0, sizeof(team_name));
				user_team_info.Reset();
			}

			int rank;										// ��X��
			GameName team_name;								// վ������
			UTAChampionRecordUserTeamInfo user_team_info;	// ս�������Ϣ
		};

		SCGetUTAChampionRecordAck() :header(MT_GET_UTA_CHAMPION_RECORD_ACK_SC) {}
		MessageHeader header;

		int count;
		UTACRInfo recrod_arr[MAX_ONCE_UTA_C_R_COUNT];
	};

}

#pragma pack(pop)

#endif // __MSG_UTA_CHAMPION_RECORD_H__

