#ifndef __MSG_RA_DAN_BI_FAN_LI_HPP__
#define __MSG_RA_DAN_BI_FAN_LI_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/radanbifanliparam.hpp"

//////////////////////////////////////////// 2223  ����  ���ʷ���  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// ��Э��  3000
	enum RA_DAN_BI_FAN_LI_OPERA_TYPE
	{
		RA_DAN_BI_FAN_LI_OPERA_TYPE_INFO = 0,					//��ȡ��Ϣ
		RA_DAN_BI_FAN_LI_OPERA_TYPE_BUY_CHECK = 1,				//������� param1 pool_seq param2:seq
	};

	struct SCRADanBiFanLiInfo			//<! 5325 ���ʷ�����Ϣ�·�
	{
	public:
		SCRADanBiFanLiInfo();
		MessageHeader header;

		short today_buy_times_pool_list[MAX_RA_DAN_BI_FAN_LI_POOL_NUM];			//���칺�����
		short activity_buy_times_pool_list[MAX_RA_DAN_BI_FAN_LI_POOL_NUM];		//��������
	};

	class SCRADanBiFanLiSingleInfo		//!< 5326 ���ʷ���������Ϣ�·�
	{
	public:
		SCRADanBiFanLiSingleInfo();
		MessageHeader header;

		short reserve_sh;
		short pool_seq;
		short today_buy_times;
		short activity_buy_times;
	};

}

#pragma pack(pop)

#endif  // __MSG_RA_DAN_BI_FAN_LI_HPP__
