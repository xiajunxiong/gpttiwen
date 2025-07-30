#ifndef __MSG_FUNC_GUIDE_H__
#define __MSG_FUNC_GUIDE_H__

#include "gamedef.h"
#include "servercommon/serverdef.h"
#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/funcguidedef.h"

#pragma pack(push, 4)

namespace Protocol
{
	class CSFuncGuideOperator
	{
	public:
		CSFuncGuideOperator();		//!< 1830 - 功能引导 操作请求
		MessageHeader header;

		enum FuncGuideOpType
		{
			FUNC_GUIDE_CHOOSE_PET = 0,		// 选择新手宠物 param：选项的 seq
			FUNC_GUIDE_CATCH_PET_BATTLE = 1,// 进入抓宠战斗
			FUNC_GUIDE_FINISH_CLOUD_ARENA_GUIDE = 2, // 完成云端竞技指引
			FUNC_GUIDE_FINISH_LUNHUI_WANGCHUAN_GUIDE = 3, // 完成轮回忘川指引
		};

		int op_type;
		int param;
	};

	class SCFuncGuideFlagInfo
	{
	public:
		SCFuncGuideFlagInfo();		//!< 1831 - 功能引导 事件标记
		MessageHeader header;

		BitMap<FUNC_GUIDE_FLAG_MAX> flag_info;	// 事件标记 - 对应位为 1 表示此事件已触发；
	};

	class SCPetDeadSummonGuideInfo		//!< 1833 触发宠物死亡召唤指引的每天次数信息下发
	{
	public:
		SCPetDeadSummonGuideInfo();
		MessageHeader header;

		short guide_trigger_times[PET_DEAD_SUMMON_GUIDE_RECORD_DAYS];	// 每天的触发次数，数组长度8
	};

	class CSPetDeadSummonGuideTrigger	//!< 1832 触发宠物死亡召唤指引
	{
	public:
		CSPetDeadSummonGuideTrigger();
		MessageHeader header;
	};
}

#pragma pack(pop)
#endif