#ifndef __MSG_RA_HERITAGE_TREASURE_HPP__
#define __MSG_RA_HERITAGE_TREASURE_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/raheritagetreasureparam.hpp"

//////////////////////////////////////////// 2057 ����  -- ���й屦  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// ��Э��  3000
	enum RA_HERITAGE_TREASURE_OPERA_TYPE
	{
		RA_HERITAGE_TREASURE_OPERA_TYPE_GET_INFO = 0,							// ��ȡ��Ϣ
		RA_HERITAGE_TREASURE_OPERA_TYPE_ACTIVE_CLUES = 1,						// �������� ,param1: ��Ƭindex
		RA_HERITAGE_TREASURE_OPERA_TYPE_FETCH_REWARD = 2,						// ��ȡ���� ,param1: ��Ƭindex
		RA_HERITAGE_TREASURE_OPERA_TYPE_FETCH_FINALE_REWARD = 3,				// ��ȡ��������� 

	};


	struct SCRAHeritageTreasureInfo			//<! 3013 ���й屦��Ϣ
	{
	public:
		SCRAHeritageTreasureInfo();
		MessageHeader header;

		unsigned char ra_heritage_treasure_piece[RA_HERITAGE_TREASURE_MAX_PIECE];	 //��Ӧ����seq
		unsigned char ra_heritage_treasure_clue[RA_HERITAGE_TREASURE_MAX_PIECE];	 //�����˶��ٸ�������255����ȡ�˽���
		unsigned char ra_heritage_treasure_is_fetch_final_reward;					//���ս�����ȡ��־
		unsigned char ra_heritage_treasure_reserved_ch;
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_HERITAGE_TREASURE_HPP__
