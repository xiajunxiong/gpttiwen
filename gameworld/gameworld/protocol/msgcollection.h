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

	class SCCollectionAllInfo					//!< 藏品激活信息 1950
	{
	public:
		SCCollectionAllInfo();
		MessageHeader		header;

		unsigned char collection_activate_flag[COLLECTION_MAX_NUM];		//!< 激活标记 0 为未激活
	};

	class SCCollectionSingleInfo					//!< 藏品激活信息 1951
	{
	public:
		SCCollectionSingleInfo();
		MessageHeader		header;

		short seq;
		short is_activate;		//!< 激活标记 0 为未激活
	};

	class CSCollectionOp					//!< 藏品操作 1952
	{
	public:
		CSCollectionOp();
		MessageHeader		header;
		enum OP_TYPE
		{
			ACTIVATE = 1,		//!< 激活 p1:发seq
			CONVERT = 2,		//!< 兑换 p1:发seq
			INCLUDED = 3,		//!< 收录 p1:发seq
			TREASURES = 4,		//!< 藏品 p1:发seq
		};

		int op_type;
		int p1;
	};

	class SCCollectionIncluded	//!< 藏品-收录下发 1953
	{
	public:
		SCCollectionIncluded();
		MessageHeader		header;
		
		unsigned int collection_included_ready_flag[COLLECTION_MAX_NUM_INT];		//!< 允许激活标记 0 为不允许
		unsigned int collection_included_activate_flag[COLLECTION_MAX_NUM_INT];		//!< 激活标记 0 为未激活
	};

	class SCCollectionIncludedSignle	//!< 藏品-收录单例下发 1954
	{
	public:
		SCCollectionIncludedSignle();
		MessageHeader		header;
		unsigned short seq;
		unsigned char ready_flag;		//!< 允许激活标记 0 为不允许
		unsigned char activate_flag;	//!< 激活标记 0 为未激活
	};

	class SCCollectionAtlasCardInfo	//!< 珍品-图鉴卡下发 1955
	{
	public:
		SCCollectionAtlasCardInfo();
		MessageHeader		header;
		
		unsigned short new_card_level[MAX_ATLAS_CARD_NUM];
	};

	class SCCollectionAtlasCardSignleInfo	//!< 珍品-图鉴卡单例下发 1956
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

