#ifndef __MSG_RA_COLLECT_WORD_HPP__
#define __MSG_RA_COLLECT_WORD_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/racollectwordparam.hpp"

//////////////////////////////////////////// 2068  ����  ���ֻ  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// ��Э��  3000
	enum RA_COLLECT_WORD_OPERA_TYPE
	{
		RA_COLLECT_WORD_OPERA_TYPE_ROLE_INFO = 0,		// �����Ϣ	
		RA_COLLECT_WORD_OPERA_TYPE_EXCHANGE = 1,		// �һ���Ʒ param_1 : ���ñ����seq
		RA_COLLECT_WORD_OPERA_TYPE_DAY_FREE = 2,		// ��ȡÿ��������
	};


	struct SCRaCollectWordInfo			//<! 3029 ���ֺ�����Ϣ
	{
	public:
		SCRaCollectWordInfo();
		MessageHeader header;

		short day_gift_is_fetch;			//��������ÿ�������� 0:δ��ȡ 1:����ȡ
		short reserve_sh;
		unsigned short exchange_times[RA_COLLECT_WORD_MAX_ENTRY_NUM];
		unsigned short total_exchange_times[RA_COLLECT_WORD_MAX_ENTRY_NUM];
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_COLLECT_WORD_HPP__


