#ifndef __MSG_RA_JOY_SEEKS_FAVORS_HPP__
#define __MSG_RA_JOY_SEEKS_FAVORS_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rasportsparam.hpp"

//////////////////////////////////////////// 2067  Ëæ»ú»î¶¯  »¶ÀÖÑ°³è  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRAJoySeeksFavorsInfo					//<! 3024 »¶ÀÖÑ°³èÆ¥Åä×´Ì¬
	{
	public:
		SCRAJoySeeksFavorsInfo();
		MessageHeader header;
		enum MATCH_TYPE
		{
			MATCH_TYPE_NO = 0,	// Î´Æ¥Åä
			MATCH_TYPE_ING,		// Æ¥ÅäÖÐ
		};

		int is_match;
	};

	class SCRAJoySeeksFavorsMatchSucc				//!< 3025 »¶ÀÖÑ°³è-µ¹ÊýÍ¨Öª
	{
	public:
		SCRAJoySeeksFavorsMatchSucc();
		MessageHeader header;

	};

	class CSRAJoySeeksFavorsMatchReq				//!< 3026  »¶ÀÖÑ°³è ÇëÇóÆ¥Åä
	{
	public:
		CSRAJoySeeksFavorsMatchReq();
		MessageHeader header;

		enum TYPE
		{
			TYPE_CANCEL = 0,			//!<È¡ÏûÆ¥Åä
			TYPE_MATCH,					//!<ÇëÇóÆ¥Åä
		};

		int is_match;
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_JOY_SEEKS_FAVORS_HPP__


