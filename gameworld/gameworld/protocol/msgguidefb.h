#ifndef __MSG_GUIDE_FB_H__
#define __MSG_GUIDE_FB_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/rolecrystalfbdef.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class SCGuideFbSceneInfo						//!< ��������������Ϣ 2900
	{
	public:
		SCGuideFbSceneInfo();
		MessageHeader		header;

		int layer;				//!<��ǰ����
		int box_num;			//!<������
		int monster_num;		//!<������
	};


}

#pragma pack(pop)

#endif

