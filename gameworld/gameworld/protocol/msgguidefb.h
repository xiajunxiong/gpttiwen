#ifndef __MSG_GUIDE_FB_H__
#define __MSG_GUIDE_FB_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/rolecrystalfbdef.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class SCGuideFbSceneInfo						//!< 引导副本场景信息 2900
	{
	public:
		SCGuideFbSceneInfo();
		MessageHeader		header;

		int layer;				//!<当前层数
		int box_num;			//!<宝箱数
		int monster_num;		//!<怪物数
	};


}

#pragma pack(pop)

#endif

