#ifndef __MSG_MAP_UNLOCK_H__
#define __MSG_MAP_UNLOCK_H__

#include "gamedef.h"
#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/mapunlockdef.h"

#pragma pack(push, 4)

namespace Protocol
{
	class SCMapUnlockAllListInfo
	{
	public:
		SCMapUnlockAllListInfo();		//!< 2230 - 地图解锁 所有不规则ID标记列表信息
		MessageHeader header;

		long long irregular_flag_list[IRREGULAR_ID_LIST_NUM_MAX];	//!< 目前解锁的区域，二进制数表示
		long long new_unlock_flag_list[IRREGULAR_ID_LIST_NUM_MAX];	//!< 新解锁的区域，用于客户端播放动画
	};

	class SCMapUnlockFristNotice
	{
	public:
		SCMapUnlockFristNotice();		//!< 2231 - 地图解锁 首次进入解锁场景通知
		MessageHeader header;

		int active_id;	//!< 解锁ID
	};

	class CSMapUnlockRemoveAnima
	{
	public:
		CSMapUnlockRemoveAnima();		//!< 2232 - 地图解锁 移除大地图解锁动画播放列表
		MessageHeader header;

		int remove_irregular_id;		// 要移除的不规则ID
	};

	class SCMapUnlockFirstEnterFbType
	{
	public:
		SCMapUnlockFirstEnterFbType();		//!< 2233 - 已经进入过的副本类型列表
		MessageHeader header;
		int count;
		long long fb_type_enter_flag[ECTYPE_PASS_RECORD_LIST_NUM_MAX];
	};
}

#pragma pack(pop)
#endif