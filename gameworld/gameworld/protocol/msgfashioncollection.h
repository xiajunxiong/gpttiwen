#ifndef __MSG_FASHION_COLLECTION_H__
#define __MSG_FASHION_COLLECTION_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/servercommon.h"
#include "gamedef.h"
#include "servercommon/fashioncollectiondef.h"

#pragma pack(push, 4) 

namespace Protocol
{
	enum FASHION_COLLECTION_REQ_TYPE
	{
		FASHION_COLLECTION_REQ_TYPE_INFO = 0,
		FASHION_COLLECTION_REQ_TYPE_LEVEL_UP = 1,		// 升级, param1-时装id
		FASHION_COLLECTION_REQ_TYPE_SPECIALIZE = 2,		// 专精激活
	};

	class CSFashionCollectionRequest	//!<1985 时装升级操作
	{
	public:
		CSFashionCollectionRequest();
		MessageHeader header;

		int req_type;
		int param1;
	};

	class SCFashionCollectionInfo		//!<1986 时装升级全部信息
	{
	public:
		SCFashionCollectionInfo();
		MessageHeader header;

		int m_specialize_level;	// 当前专精等级
		unsigned char m_fashion_collection[SURFACE_MAX_NUM];		// 当前时装等级
	};


}

#pragma pack(pop)
#endif
