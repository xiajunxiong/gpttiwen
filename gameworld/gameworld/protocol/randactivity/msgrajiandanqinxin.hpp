#ifndef __MSG_RA_JIAN_DAN_QIN_XIN_HPP__
#define __MSG_RA_JIAN_DAN_QIN_XIN_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rajiandanqinxinparam.hpp"

//////////////////////////////////////////// 2073  ����  ��������  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// ��Э��  3000
	enum RA_JIAN_DAN_QIN_XIN_OPERA_TYPE
	{
		RA_JIAN_DAN_QIN_XIN_OPERA_TYPE_INFO = 0,			//��������������Ϣ�·�	
		RA_JIAN_DAN_QIN_XIN_OPERA_TYPE_BATTLE_REQ = 1,		//����������ս�ؿ�    ��Ҫparam1
		RA_JIAN_DAN_QIN_XIN_OPERA_TYPE_BATTLE_RESULT = 2,	//ս�����    ������ڲ����� 
	};

	struct SCRAJianDanQinxinInfo		//!< 3027 ����������Ϣ�·�
	{
		SCRAJianDanQinxinInfo();
		MessageHeader header;

		int open_day;											//������ڼ���
		BitMap<MAX_RA_JIANDANQINXIN_CHECKPOINT_NUM> flag;		//ǰ�߸�Ϊ�ͼ��Ѷ�,���߸�Ϊ���Ѷ�(��������seq1)
	};

}

#pragma pack(pop)

#endif  // __MSG_RA_JIAN_DAN_QIN_XIN_HPP__
