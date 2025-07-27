#ifndef __LUN_HUI_WANG_CHUAN_DEF_H__
#define __LUN_HUI_WANG_CHUAN_DEF_H__

#include "servercommon/bitmapfunction.hpp"

const static int MAX_LUN_HUI_WANG_CHUAN_INDEX = 32;		//最大关卡序号

#pragma pack(push, 4)
struct LunHuiWangChuanParam
{
	/*************** **********************/
	LunHuiWangChuanParam() { this->Reset(); }

	void Reset()
	{
		battle_tamp = 0;
		fight_seq = 0;
		fight_level = 0;
		fight_refresh_tamp = 0;

		set_seq = 0;
		start_level = 0;
		battle_monster_id = 0;
		previous_refresh_tamp = 0;
		pass_list.Reset();
		reward_list.Reset();
		memset(reserve_ll, 0, sizeof(reserve_ll));
		pass_times = 0;
	}

	void Refresh()
	{
		set_seq = 0;
		start_level = 0;
		battle_monster_id = 0;
		previous_refresh_tamp = 0;
		pass_list.Reset();
		reward_list.Reset();
	}

	void OnFightFinish()
	{
		battle_tamp = 0;
		fight_seq = 0;
		fight_level = 0;
		//battle_monster_id = 0;
		fight_refresh_tamp = 0;
	}

	unsigned int battle_tamp;	//记录战斗开始时间戳
	int fight_seq;				//正在战斗seq
	int fight_level;
	unsigned int fight_refresh_tamp;//战斗时的刷新时间戳

	int start_level;			// 记录等级
	int set_seq;				// 发起战斗seq，值变化要调用void LunHuiWangChuan::OnPassSeqChange()
	int battle_monster_id;		// 战斗怪物组
	unsigned int previous_refresh_tamp;

	BitMap<MAX_LUN_HUI_WANG_CHUAN_INDEX> pass_list;

	BitMap<MAX_LUN_HUI_WANG_CHUAN_INDEX> reward_list;

	long long reserve_ll[3];
	int pass_times;				// 生涯通关次数
};

typedef char LunHuiWangChuanParamHex[sizeof(LunHuiWangChuanParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(LunHuiWangChuanParamHex) < 256);


#pragma pack(pop)
#endif