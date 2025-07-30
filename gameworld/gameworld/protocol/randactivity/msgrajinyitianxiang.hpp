#ifndef __MSG_RA_JIN_YI_TIAN_XIANG_HPP__
#define __MSG_RA_JIN_YI_TIAN_XIANG_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

//////////////////////////////////////////// 2191  ����  ��������  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRAJinYiTianXiang		//<! 4790 ����������Ϣ
	{
	public:
		SCRAJinYiTianXiang();
		MessageHeader header;

		int draw_times;
		unsigned int get_reward_flag;
		int has_draw_value;
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
