#ifndef __MSG_BOSS_CHAPTER_H__
#define __MSG_BOSS_CHAPTER_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/servercommon.h"
#include "servercommon/bosschapterdef.hpp"
#include "gamedef.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class SCBossChapterInfo			//!< 1860 信息下发
	{
	public:
		SCBossChapterInfo();
		MessageHeader header;

		struct Info
		{
			int monster_group_id;	//!< 怪物组ID
			char reward_status;		//!< 奖励领取状态  1：可领取  2：已领取
			char reserve_ch;		//!< 保留字
			short reserve_sh;		//!< 保留字
		};

		enum REASON
		{
			REASON_INIT = 0,		//!< 0 初始化
			REASON_ADD = 1,			//!< 1 新增
			REASON_INFO_CHANGE = 2,	//!< 2 更新信息
			REASON_OTHER_INFO_CHANGE = 3, //!< 3 更新其他信息
		};

		short num;		//!< 数量
		short reason;	//!< 下发协议原因 \see REASON
		long long chapter_reward_flag;				//!< 章节领取标记（二进制形式表示章节）
		Info info_list[BOSS_CHAPTER_MAX_BOSS_NUM];	//!< 信息列表 \see Info
	};


	class CSBossChapterReq		//!< 1861 boss章节请求
	{
	public:
		CSBossChapterReq();
		MessageHeader header;
		
		enum REQ_TYPE
		{
			REQ_TYPE_CLAIM_BOSS_REWARD = 0,		//!< 领取boss的奖励  param: 怪物组ID
			REQ_TYPE_CLAIM_CHAPTER_REWARD = 1,	//!< 领取章节奖励	 param: 章节ID
			REQ_TYPE_START_BATTLE = 2,			//!< 开始战斗		 param: 怪物组ID
		};

		int req_type;
		int param;
	};
}

#pragma pack(pop)

#endif

