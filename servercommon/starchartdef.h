#ifndef __STAR_CHART_DEF__
#define __STAR_CHART_DEF__

#include "servercommon/servercommon.h"

enum STAR_CHART_STATE
{
	STAR_CHART_STATE_SKILL_LOCK = -1,		// 星图技能锁定状态
	STAR_CHART_STATE_LOCK = 0,				// 星图星位锁定状态
	STAR_CHART_STATE_SKILL_UNLOCK = STAR_CHART_STATE_LOCK,	// 星图技能解锁状态
};

enum STAR_CHART_TYPE
{
	STAR_CHART_TYPE_ROLE = 0,		// 角色星图
	STAR_CHART_TYPE_PET = 1,		// 宠物星图
	STAR_CHART_TYPE_PARTNER = 2,	// 伙伴星图

	STAR_CHART_TYPE_MAX,
};

enum SUPER_GALAXIESCFG_TYPE
{
	SUPER_GALAXIESCFG_TYPE_ATTR = 0,		// 增加属性，参数1为属性类型，参数2为属性值
	SUPER_GALAXIESCFG_TYPE_PASSIVE = 1,		// 被动技能，参数1为技能id，参数2为等级

	SUPER_GALAXIESCFG_TYPE_MAX,
};

static const int MAX_STAR_CHART_POINT = 20;			// 星图节点数(19 + 1), +1为了字节对齐
static const int MAX_STAR_CHART_STAGE = 5;			// 星图阶数
static const int MAX_STAR_CHART_SKILL = 3;			// 星图技能数
static const int MAX_SUPER_GALAXIES_POINT = 168;	// 超星系节点数(166 + 2), +2为了字节对齐

static const int MAX_TAR_CHART_TYPE = 3;			// 星图最大类型
UNSTD_STATIC_CHECK(STAR_CHART_TYPE_MAX == MAX_TAR_CHART_TYPE)

static const int STAR_CHART_RESET_POINT_RATIO = 1000;	// 重置返还铜币千分比

#pragma pack(push, 4)
struct StarChartParam
{
	StarChartParam() { this->Reset(); }
	void Reset()
	{
		memset(super_galaxies, STAR_CHART_STATE_LOCK, sizeof(super_galaxies));
		memset(star_chart, STAR_CHART_STATE_LOCK, sizeof(star_chart));
		memset(star_chart_skill, STAR_CHART_STATE_SKILL_LOCK, sizeof(star_chart_skill));
	}

	char super_galaxies[MAX_SUPER_GALAXIES_POINT];										// 超星系
	char star_chart[MAX_TAR_CHART_TYPE][MAX_STAR_CHART_STAGE][MAX_STAR_CHART_POINT];	// 星图
	int  star_chart_skill[MAX_STAR_CHART_SKILL][MAX_TAR_CHART_TYPE];					// 星图技能, 存放的是技能在超星系里的索引
};
typedef char StarChartParamHex[sizeof(StarChartParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(StarChartParamHex) < 1024);

#pragma pack(pop)
#endif