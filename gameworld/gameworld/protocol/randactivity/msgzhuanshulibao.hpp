#ifndef __MSG_RA_ZHUAN_SHU_LI_BAO_HPP__
#define __MSG_RA_ZHUAN_SHU_LI_BAO_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

//////////////////////////////////////////// 2086  随机活动  专属礼包  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	//由协议 3000
	enum RA_ZHUAN_SHU_LI_BAO_REQ_TYPE
	{
		RA_ZHUAN_SHU_LI_BAO_REQ_TYPE_INFO = 0,				// 请求专属信息下发
		RA_ZHUAN_SHU_LI_BAO_REQ_TYPE_BUY_ITEM = 1,			// 购买专属礼包前配置检测 param1:type
		RA_ZHUAN_SHU_LI_BAO_REQ_TYPE_DAY_INFO = 2,			// 请求专属礼包-每日礼包信息下发
		RA_ZHUAN_SHU_LI_BAO_REQ_TYPE_BUY_DAY_INFO = 3,		// 购买专属礼包_每日礼包前配置检测 param1:phase p2:type
	};

	class SCRAZhuanShuLiBaoInfo						//<! 3400 专属礼包信息 
	{
	public:
		SCRAZhuanShuLiBaoInfo();
		MessageHeader header;

		ZhuanShuLiBaoAciveItem active_item[RA_ZHUAN_SHU_LI_BAO_MAX_EXIST_ITEM];		//激活了的礼包信息
	};

	class SCRAZhuanShuLiBaoDayInfo		//!< 3401 专属礼包-每日礼包信息下发
	{
	public:
		SCRAZhuanShuLiBaoDayInfo();
		MessageHeader header;

		ZhuanShuLiBaoDayItem day_item[RA_ZHUAN_SHU_LI_BAO_DAY_MAX_ITEM_NUM];		//每日礼包
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
