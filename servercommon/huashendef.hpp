#ifndef __HUASHEN_DEF_HPP__
#define __HUASHEN_DEF_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/fabaodef.hpp"
#include "servercommon/attributedef.hpp"
#include "servercommon/struct/skilllistparam.hpp"
#include "servercommon/traindef.hpp"
#include "servercommon/commondata.hpp"
//#include "other/attributelist.hpp"

#pragma pack(push, 4)

static const int MAX_HUASHEN_ATTR_PLAN_NUM = 3;
static const int MAX_HUASHEN_DELETE_ITEM_NUM = 1;		//同步时道具移除数量大于等于该值,则自动整理背包

struct HuaShenParam
{
	HuaShenParam() { this->Reset(); }

	void Reset()
	{
		season_reset = 0;
		plan_reset_flag = 0;
		max_level = 0;
		memset(reserve_char, 0, sizeof(reserve_char));
		is_finish_ding_ji = 0;
		is_rule_change_flag = 0;
		is_reset_grid_flag = 0;
		is_finish_guide = 0;
		reserve_long_long = 0;
		pet_train.Reset();
		for (int i = 0; i < TARIN_TYPE_MAX; i++)
		{
			train_list[i].Reset();
		}
		season_reset_first_dijing = 0;
		use_new_pet_status_array = 0;
		memset(pet_status_index_OBSOLETE, -1, sizeof(pet_status_index_OBSOLETE));
		memset(new_pet_status_index, -1, sizeof(new_pet_status_index));
		capability = 0;
		max_capability = 0;
		battle_position = 0;
		cur_plan_index = 0;
		for (int i = 0; i < MAX_HUASHEN_ATTR_PLAN_NUM; i++)
		{
			attr_plan_list[i].Reset();
		}
		rank_score = 0;
		//memset(m_fabao_equipment_bar, FABAO_EQUIP_BAR_NO, sizeof(m_fabao_equipment_bar));
	}
	//赛季重置
	void SeasonReset(bool is_reset_dingji_flag)
	{
		season_reset = 0;
		max_level = 0;
		memset(reserve_char, 0, sizeof(reserve_char));
		is_finish_ding_ji = 0;
		is_finish_guide = 0;
		reserve_long_long = 0;
		pet_train.Reset();
		for (int i = 0; i < TARIN_TYPE_MAX; i++)
		{
			train_list[i].Reset();
		}
		season_reset_first_dijing = is_reset_dingji_flag ? 0 : season_reset_first_dijing;
		//use_new_pet_status_array = 0; 赛季重置就不用重置这个标记了
		memset(pet_status_index_OBSOLETE, -1, sizeof(pet_status_index_OBSOLETE));
		memset(new_pet_status_index, -1, sizeof(new_pet_status_index));
		capability = 0;
		max_capability = 0;
		battle_position = 0;
		cur_plan_index = 0;
		for (int i = 0; i < MAX_HUASHEN_ATTR_PLAN_NUM; i++)
		{
			attr_plan_list[i].Reset();
		}
		rank_score = 0;
	}

	char season_reset;											// 0:本次赛季未重置  1本次赛季已重置(一个赛季只能重置一次)
	char plan_reset_flag;										// 老号重置属性点  0:未重置 1已重置
	short max_level;											// 本次赛季选择的战场
	char reserve_char[76];										// 保留字,以保持数据不错乱
	char is_finish_ding_ji;										// 是否完成了定级赛	0:未完成
	char is_rule_change_flag;									// 规则变化赛季重置  5:已重置
	char is_reset_grid_flag;									// 清除重复数据  1:已重置
	char is_finish_guide;										// 是否完成化身指引 0 : 未完成 1:已完成
	long long reserve_long_long;								// 保留字

	TrainData pet_train;										//宠物修炼技能
	TrainData train_list[TARIN_TYPE_MAX];						//人物修炼技能

	char season_reset_first_dijing;								// 0:赛季重置还未进入 1:赛季重置已进入(定级赛提示）
	char use_new_pet_status_array;								//转移宠物状态数组标记
	short pet_status_index_OBSOLETE[PET_STATUS_MAX_OBSOLETE];	//宠物状态(已废弃）

	int capability;												//化身当前战力
	int max_capability;											//化身历史最高战力	(此处只是提供一个化身战力保存位置)

	int battle_position;										//化身战斗站位（0：后排   1：前排）

	int cur_plan_index;											// 当前方案序号 范围[0,3)
	AttributePlan attr_plan_list[MAX_HUASHEN_ATTR_PLAN_NUM];	// 化身潜能方案 固定三套
	int rank_score;												// 排位分
	
	short new_pet_status_index[NEW_PET_STATUS_MAX];
};
UNSTD_STATIC_CHECK(8 == NEW_PET_STATUS_MAX);
UNSTD_STATIC_CHECK(5 == PET_STATUS_MAX_OBSOLETE);

typedef char HuaShenParamHex[sizeof(HuaShenParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(HuaShenParamHex) < 2048);

#pragma pack(pop)

#endif