#ifndef __MSG_STAR_CHART__
#define __MSG_STAR_CHART__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/commondata.hpp"
#include "servercommon/starchartdef.h"

#pragma pack(push, 4)

namespace Protocol
{

class CSStarChartRequest
{
public:
	CSStarChartRequest();		//!<5320 星图系统请求
	MessageHeader header;

	enum OP_TYPE
	{
		OP_TYPE_STAR_CHART_LEVEL_UP = 0,			// 星图星位升级, param1-星图类型(0-2) param2-星图星位索引, param3-星图阶数
		OP_TYPE_SUPER_GALAXIES_LEVEL_UP = 1,		// 超星系星位升级, param1-超星系星位索引
// 		OP_TYPE_STAR_CHART_RESET = 2,				// 重置星图, param1-星图类型	(策划: 取消小星图的重置功能)
		OP_TYPE_SUPER_GALAXIES_RESET = 3,			// 重置超星系
// 		OP_TYPE_STAR_CHART_UNLOCK_SKILL = 4,		// 解锁技能孔位, param1-星图类型(0-2) param2-技能孔位索引	(策划要自动解锁, 此条作废)
		OP_TYPE_STAR_CHART_INSTALL_SKILL = 5,		// 装备技能, param1-星图类型(0-2) param2-技能孔位索引 param3-技能在超星系的索引
		OP_TYPE_STAR_CHART_ONE_KEY_UPGRADE = 6,		// 星图星位一键升级, param1-星图类型(0-2) param2-星图星位索引, param3-星图阶数
		OP_TYPE_SUPER_GALAXIES_ONE_KEY_UPGRADE = 7,	// 超星系星位一键升级, param1-超星系星位索引
	};

	int op_type;
	int param1;
	int param2;
	int param3;
};

class SCStarChartAllInfo		//!<5321 星图系统信息下发
{
public:
	SCStarChartAllInfo();
	MessageHeader header;

	char super_galaxies[MAX_SUPER_GALAXIES_POINT];										// 超星系
	char star_chart[MAX_TAR_CHART_TYPE][MAX_STAR_CHART_STAGE][MAX_STAR_CHART_POINT];	// 星图
	int  star_chart_skill[MAX_STAR_CHART_SKILL][MAX_TAR_CHART_TYPE];					// 星图技能, 存放的是技能在超星系里的索引
};

class SCStarChartInstallSkillNotice		//!<5322 星图装备技能通知
{
public:
	enum NOTICE_TYPE	// 通知类型
	{
		NOTICE_TYPE_USED = 0,	// 装备技能
		NOTICE_TYPE_ACTIVE = 1,	// 激活技能
	};

public:
	SCStarChartInstallSkillNotice();
	MessageHeader header;

	int notice_type;	// SCStarChartInstallSkillNotice::NOTICE_TYPE
	int skill_seq;		// 装备的技能在超星系的索引
};


};

#pragma pack(pop)
#endif