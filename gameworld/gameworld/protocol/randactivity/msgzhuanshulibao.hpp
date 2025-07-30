#ifndef __MSG_RA_ZHUAN_SHU_LI_BAO_HPP__
#define __MSG_RA_ZHUAN_SHU_LI_BAO_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

//////////////////////////////////////////// 2086  ����  ר�����  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	//��Э�� 3000
	enum RA_ZHUAN_SHU_LI_BAO_REQ_TYPE
	{
		RA_ZHUAN_SHU_LI_BAO_REQ_TYPE_INFO = 0,				// ����ר����Ϣ�·�
		RA_ZHUAN_SHU_LI_BAO_REQ_TYPE_BUY_ITEM = 1,			// ����ר�����ǰ���ü�� param1:type
		RA_ZHUAN_SHU_LI_BAO_REQ_TYPE_DAY_INFO = 2,			// ����ר�����-ÿ�������Ϣ�·�
		RA_ZHUAN_SHU_LI_BAO_REQ_TYPE_BUY_DAY_INFO = 3,		// ����ר�����_ÿ�����ǰ���ü�� param1:phase p2:type
	};

	class SCRAZhuanShuLiBaoInfo						//<! 3400 ר�������Ϣ 
	{
	public:
		SCRAZhuanShuLiBaoInfo();
		MessageHeader header;

		ZhuanShuLiBaoAciveItem active_item[RA_ZHUAN_SHU_LI_BAO_MAX_EXIST_ITEM];		//�����˵������Ϣ
	};

	class SCRAZhuanShuLiBaoDayInfo		//!< 3401 ר�����-ÿ�������Ϣ�·�
	{
	public:
		SCRAZhuanShuLiBaoDayInfo();
		MessageHeader header;

		ZhuanShuLiBaoDayItem day_item[RA_ZHUAN_SHU_LI_BAO_DAY_MAX_ITEM_NUM];		//ÿ�����
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
