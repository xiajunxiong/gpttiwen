#ifndef __MSG_COLLECTION_H__
#define __MSG_COLLECTION_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/servercommon.h"
#include "servercommon/collectiondef.hpp"
#include "gamedef.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{

	class SCCollectionAllInfo					//!< ��Ʒ������Ϣ 1950
	{
	public:
		SCCollectionAllInfo();
		MessageHeader		header;

		unsigned char collection_activate_flag[COLLECTION_MAX_NUM];		//!< ������ 0 Ϊδ����
	};

	class SCCollectionSingleInfo					//!< ��Ʒ������Ϣ 1951
	{
	public:
		SCCollectionSingleInfo();
		MessageHeader		header;

		short seq;
		short is_activate;		//!< ������ 0 Ϊδ����
	};

	class CSCollectionOp					//!< ��Ʒ���� 1952
	{
	public:
		CSCollectionOp();
		MessageHeader		header;
		enum OP_TYPE
		{
			ACTIVATE = 1,		//!< ���� p1:��seq
			CONVERT = 2,		//!< �һ� p1:��seq
			INCLUDED = 3,		//!< ��¼ p1:��seq
			TREASURES = 4,		//!< ��Ʒ p1:��seq
		};

		int op_type;
		int p1;
	};

	class SCCollectionIncluded	//!< ��Ʒ-��¼�·� 1953
	{
	public:
		SCCollectionIncluded();
		MessageHeader		header;
		
		unsigned int collection_included_ready_flag[COLLECTION_MAX_NUM_INT];		//!< �������� 0 Ϊ������
		unsigned int collection_included_activate_flag[COLLECTION_MAX_NUM_INT];		//!< ������ 0 Ϊδ����
	};

	class SCCollectionIncludedSignle	//!< ��Ʒ-��¼�����·� 1954
	{
	public:
		SCCollectionIncludedSignle();
		MessageHeader		header;
		unsigned short seq;
		unsigned char ready_flag;		//!< �������� 0 Ϊ������
		unsigned char activate_flag;	//!< ������ 0 Ϊδ����
	};

	class SCCollectionAtlasCardInfo	//!< ��Ʒ-ͼ�����·� 1955
	{
	public:
		SCCollectionAtlasCardInfo();
		MessageHeader		header;
		
		unsigned short new_card_level[MAX_ATLAS_CARD_NUM];
	};

	class SCCollectionAtlasCardSignleInfo	//!< ��Ʒ-ͼ���������·� 1956
	{
	public:
		SCCollectionAtlasCardSignleInfo();
		MessageHeader		header;
		unsigned short seq;
		unsigned short level;
	};
}

#pragma pack(pop)

#endif

