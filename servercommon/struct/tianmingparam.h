#ifndef __TIAN_MING_PARAM_H__
#define __TIAN_MING_PARAM_H__

#include "servercommon/servercommon.h"

enum TIAN_MING_ADD_ATTR_SYS_TYPE
{
	TIAN_MING_ADD_ATTR_SYS_TYPE_INVALID  = 0 ,
	TIAN_MING_ADD_ATTR_SYS_TYPE_PEI_SHI_HU_JING = 1,				// 配饰护镜
	TIAN_MING_ADD_ATTR_SYS_TYPE_PEI_SHI_HUA_DIAN = 2,				// 配饰花钿
	TIAN_MING_ADD_ATTR_SYS_TYPE_PEI_SHI_JING_DAI = 3,				// 配饰锦带
	TIAN_MING_ADD_ATTR_SYS_TYPE_PEI_SHI_XIANG_NANG = 4,				// 配饰香囊
	TIAN_MING_ADD_ATTR_SYS_TYPE_PEI_SHI_XIU_JIAN = 5,				// 配饰护镜
	TIAN_MING_ADD_ATTR_SYS_TYPE_EQUIP_STRENGTHEN = 6,				// 配饰全身装备强化
	TIAN_MING_ADD_ATTR_SYS_TYPE_GEM = 7,							// 配饰全身宝石
	TIAN_MING_ADD_ATTR_SYS_TYPE_EQUIPMENT = 8,						// 配饰全身装备
	TIAN_MING_ADD_ATTR_SYS_TYPE_MAX,
};

static const int MAX_TIAN_MING_LEVEL_COUNT = 401;					// 天命系统最大等级400  

/////////////////////// [天命] ///////////////////////

struct TianMingParam
{
	TianMingParam() { this->Reset(); }

	void Reset()
	{
		level = 0;
		exp = 0;
		reserve_ll = 0;

		memset(sys_level, 0, sizeof(sys_level));
	}

	int level;													// 天命系统 当前总等级
	int exp;													// 天命系统 当前经验

	long long reserve_ll;										// 备用 , 也许会有别的需要

	int sys_level[TIAN_MING_ADD_ATTR_SYS_TYPE_MAX];				// 天命系统每个分项的当前等级 , 放在最后方便扩展系统加成类型
};

typedef char TianMingParamHex[sizeof(TianMingParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(TianMingParamHex) < 128);


#endif	//__TIAN_MING_PARAM_H__