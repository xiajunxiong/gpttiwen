#ifndef __MSG_TITLE_H__
#define __MSG_TITLE_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/servercommon.h"
#include "servercommon/titledef.hpp"
#include "gamedef.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	
	class SCTitleAllInfo					//!< 1900		 称号全部信息
	{
	public:
		SCTitleAllInfo();
		MessageHeader		header;

		int count;
		TitleData title_list[TITLE_MAX_NUM];
	};

	class SCTitleSingleInfo					//!< 1901			 单个称号信息
	{
	public:
		SCTitleSingleInfo();
		MessageHeader		header;

		int notice_type;			
		int index;
		TitleData title;
	};
	class SCTitleOtherInfo					//!< 1902		称号其他信息
	{
	public:
		SCTitleOtherInfo();
		MessageHeader		header;

		int wear_title_id;		// 带着的称号id
	};

	class CSTitleOp					//!< 1903			称号操作请求
	{
	public:
		CSTitleOp();
		MessageHeader		header;
		enum OP_TYPE
		{
			WEAR = 1,			// 佩戴称号
			GETOFF = 2,			//脱下称号
		};
		int type;
		int param;
	};
	class SCTitleChange			//!< 1904			称号更改后,通知周围的人
	{
	public:
		SCTitleChange();
		MessageHeader		header;

		UInt16 title_id;	// 更改后的称号id
		ObjID objid;		// 场景管理生成的ID	
		int facescore_seq;	//!< 颜值系统 | 头衔seq
	};

	class SCTitleNotice			//!< 1905 通知称号获取或失去
	{
	public:
		SCTitleNotice();
		MessageHeader header;

		enum NOTICE_TYPE
		{
			NOTICE_TYPE_GET_TITLE = 0,		//获得称号
			NOTICE_TYPE_LOSE_TITLE = 1,		//失去称号
		};

		short notice;	
		short index;
		int title_id;			
	};
}

#pragma pack(pop)

#endif

