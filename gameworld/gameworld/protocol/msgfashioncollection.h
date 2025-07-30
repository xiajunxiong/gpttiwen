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
		FASHION_COLLECTION_REQ_TYPE_LEVEL_UP = 1,		// ����, param1-ʱװid
		FASHION_COLLECTION_REQ_TYPE_SPECIALIZE = 2,		// ר������
	};

	class CSFashionCollectionRequest	//!<1985 ʱװ��������
	{
	public:
		CSFashionCollectionRequest();
		MessageHeader header;

		int req_type;
		int param1;
	};

	class SCFashionCollectionInfo		//!<1986 ʱװ����ȫ����Ϣ
	{
	public:
		SCFashionCollectionInfo();
		MessageHeader header;

		int m_specialize_level;	// ��ǰר���ȼ�
		unsigned char m_fashion_collection[SURFACE_MAX_NUM];		// ��ǰʱװ�ȼ�
	};


}

#pragma pack(pop)
#endif
