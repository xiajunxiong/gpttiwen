#ifndef __RA_ZAI_ZHAN_YUN_ZE_PARAM_HPP__
#define __RA_ZAI_ZHAN_YUN_ZE_PARAM_HPP__


#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//再战云泽
namespace RAZaiZhanYunZe
{
	//boss最大数
	static const int BOSS_MAX = 12;
	
	//boss通关奖励段数
	static const int BOSS_PASS_CFG_MAX = 3;

};

#pragma pack(push, 4)
struct RAZaiZhanYunZeParam
{
	RAZaiZhanYunZeParam()
	{
		this->Reset();
	}

	void Reset()
	{
		ra_begin_timestamp = 0;

		hold_ll = 0;

		memset(act_boss_info_, 0, sizeof(act_boss_info_));
	}

	struct ActBossInfo
	{
		ActBossInfo()
		{
			pass_time = 0;
			hold_int = 0;
		}
		int pass_time;
		int hold_int;
	};

	unsigned int ra_begin_timestamp;
	long long hold_ll;

	//活动boss信息[boss_index]
	ActBossInfo act_boss_info_[RAZaiZhanYunZe::BOSS_MAX];
};

#pragma pack(pop)
#endif