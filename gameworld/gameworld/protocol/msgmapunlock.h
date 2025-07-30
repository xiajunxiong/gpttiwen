#ifndef __MSG_MAP_UNLOCK_H__
#define __MSG_MAP_UNLOCK_H__

#include "gamedef.h"
#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/mapunlockdef.h"

#pragma pack(push, 4)

namespace Protocol
{
	class SCMapUnlockAllListInfo
	{
	public:
		SCMapUnlockAllListInfo();		//!< 2230 - ��ͼ���� ���в�����ID����б���Ϣ
		MessageHeader header;

		long long irregular_flag_list[IRREGULAR_ID_LIST_NUM_MAX];	//!< Ŀǰ���������򣬶���������ʾ
		long long new_unlock_flag_list[IRREGULAR_ID_LIST_NUM_MAX];	//!< �½������������ڿͻ��˲��Ŷ���
	};

	class SCMapUnlockFristNotice
	{
	public:
		SCMapUnlockFristNotice();		//!< 2231 - ��ͼ���� �״ν����������֪ͨ
		MessageHeader header;

		int active_id;	//!< ����ID
	};

	class CSMapUnlockRemoveAnima
	{
	public:
		CSMapUnlockRemoveAnima();		//!< 2232 - ��ͼ���� �Ƴ����ͼ�������������б�
		MessageHeader header;

		int remove_irregular_id;		// Ҫ�Ƴ��Ĳ�����ID
	};

	class SCMapUnlockFirstEnterFbType
	{
	public:
		SCMapUnlockFirstEnterFbType();		//!< 2233 - �Ѿ�������ĸ��������б�
		MessageHeader header;
		int count;
		long long fb_type_enter_flag[ECTYPE_PASS_RECORD_LIST_NUM_MAX];
	};
}

#pragma pack(pop)
#endif