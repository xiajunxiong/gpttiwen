#ifndef __MSG_SURFACE_H__
#define __MSG_SURFACE_H__

#include "servercommon/userprotocal/msgheader.h"
#include "other/surface/surfaceconfig.hpp"
#include "servercommon/servercommon.h"
#include "gamedef.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	
	struct SurfaceData
	{
		SurfaceData() { this->Reset(); }
		void Reset(){}

		int id;
		int level;
		int end_time;
	};

	class SCSurfaceAllInfo					//!< 1980	幻化全部信息
	{
	public:
		SCSurfaceAllInfo();
		MessageHeader		header;

		short wear_surface[SURFACE_TYPE_MAX];			//!< 当前各部位使用的幻化id 数组下标为配置表的type
		int surface_count;
		SurfaceData surface_list[SURFACE_MAX_NUM];
	};

	class SCSurfaceWearInfo					//!< 1983	幻化穿戴信息
	{
	public:
		SCSurfaceWearInfo();
		MessageHeader		header;

		short wear_surface[SURFACE_TYPE_MAX];			//!< 当前各部位使用的幻化id 数组下标为配置表的type
	};

	class SCSurfaceSingleInfo					//!< 1982	幻化单个信息
	{
	public:
		SCSurfaceSingleInfo();
		MessageHeader		header;

		enum SURFACE_REASON_TYPE
		{
			SURFACE_REASON_TYPE_DEFAULT = 0,				//默认 不提示
			SURFACE_REASON_TYPE_ACTIVATE_NOTICE = 1,		//激活时装道具 提示	
			SURFACE_REASON_TYPE_END_TIME_NOTICE = 2,		//到期 提示	
		};

		int reason;					
		SurfaceData surface;
	};

	class CSSurfaceOp						//!< 1981	幻化请求
	{
	public:
		CSSurfaceOp();
		MessageHeader		header;
		enum OP_TYPE
		{
			WEAR = 1,			//!< param1 = 幻化id  为0时脱下装备   param2 为要脱的部位
			EXCHANGE ,			//!< param1 = 要购买seq
		};

		int op_type;
		int param1;
		int param2;
	};
	class CSSurefaceDecompose			//!< 1984 时装分解(通用物品分解)(之前的逻辑看上去已屏蔽,现由外观升级系统使用, fashioncollection.hpp)
	{
	public:
		CSSurefaceDecompose();
		MessageHeader		header;

		short column;		//物品所在的背包栏
		short index;		//物品所在的格子
		int num;			//分解的数量
	};

}

#pragma pack(pop)

#endif

