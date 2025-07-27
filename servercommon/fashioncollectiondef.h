#ifndef __FASHION_COLLECTION_DEF_H__
#define __FASHION_COLLECTION_DEF_H__

#include "servercommon/servercommon.h"
#include "servercommon/bitmapfunction.hpp"
#include "servercommon/surfacedef.hpp"

#pragma pack(push, 4)

static const int MAX_FASHION_COLLECTION_LEVEL = 20;		// ÿ��ʱװ���ȼ�

struct FashionCollectionParam
{
	FashionCollectionParam() { this->Reset(); }

	void Reset()
	{
		m_specialize = 0;
		memset(m_fashion_collection, 0, sizeof(m_fashion_collection));
	}
	
	int m_specialize;	// ר��������Ϣ(ר���ȼ�)
	unsigned char m_fashion_collection[SURFACE_MAX_NUM];		// ��ǰʱװ�ȼ�
};

typedef char FashionCollectionParamHex[sizeof(FashionCollectionParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(FashionCollectionParamHex) < 1536);
UNSTD_STATIC_CHECK(SURFACE_MAX_NUM == 600);		// ������Ҫ����4�ֽڶ���

#pragma pack(pop)
#endif
