#ifndef __MSG_RA_SHAN_HAI_MEET_HPP__
#define __MSG_RA_SHAN_HAI_MEET_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rashanhaimeetparam.hpp"

//////////////////////////////////////////// 2140  ɽ�����  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// ��Э��  3000
	enum RA_SHAN_HAI_MEET_OPERA_TYPE
	{
		RA_SHAN_HAI_MEET_TYPE_GET_INFO = 0,				//������Ϣ�·�
		RA_SHAN_HAI_MEET_OPERA_TYPE_FETCH = 1,			//������ȡ���� param1 - seq
		RA_SHAN_HAI_MEET_OPERA_TYPE_POKER = 2,			//������	   param1 - index0-5
		RA_SHAN_HAI_MEET_OPERA_TYPE_BUY = 3,			//������
		RA_SHAN_HAI_MEET_OPERA_RESET_POKER = 4,			//�������÷���
	};

	struct SCRAShanHaiMeetInfo							//<! 3110 ɽ�������Ϣ
	{
	public:
		SCRAShanHaiMeetInfo();
		MessageHeader header;

		char			is_buy;												// 1:�ѹ��� 0:δ����
		char			sh_server;
		short			ch_server;

		BitMap<RA_SHAN_HAI_MEET_POKER_MAX_INDEX> poker_idx_flag;			// �����±��־λ RA_SHAN_HAI_MEET_POKER_MAX_INDEX-6
		BitMap<RA_SHAN_HAI_MEET_TASK_ID_MAX> fetch_flag;					// ��ȡseq��־λ RA_SHAN_HAI_MEET_TASK_ID_MAX-32

		int poker_item_seq[RA_SHAN_HAI_MEET_POKER_MAX_INDEX];				// ���ƽ�������-6

		int				count;
		int				finish_count[RA_SHAN_HAI_MEET_ACT_TYPE_MAX];
	};

}

#pragma pack(pop)

#endif  // __MSG_RA_SHAN_HAI_MEET_HPP__
