#ifndef __MSG_RA_JOY_SEEKS_FAVORS_HPP__
#define __MSG_RA_JOY_SEEKS_FAVORS_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rasportsparam.hpp"

//////////////////////////////////////////// 2067  ����  ����Ѱ��  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRAJoySeeksFavorsInfo					//<! 3024 ����Ѱ��ƥ��״̬
	{
	public:
		SCRAJoySeeksFavorsInfo();
		MessageHeader header;
		enum MATCH_TYPE
		{
			MATCH_TYPE_NO = 0,	// δƥ��
			MATCH_TYPE_ING,		// ƥ����
		};

		int is_match;
	};

	class SCRAJoySeeksFavorsMatchSucc				//!< 3025 ����Ѱ��-����֪ͨ
	{
	public:
		SCRAJoySeeksFavorsMatchSucc();
		MessageHeader header;

	};

	class CSRAJoySeeksFavorsMatchReq				//!< 3026  ����Ѱ�� ����ƥ��
	{
	public:
		CSRAJoySeeksFavorsMatchReq();
		MessageHeader header;

		enum TYPE
		{
			TYPE_CANCEL = 0,			//!<ȡ��ƥ��
			TYPE_MATCH,					//!<����ƥ��
		};

		int is_match;
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_JOY_SEEKS_FAVORS_HPP__


