#ifndef __PET_MONOPOLY_DEF_HPP__
#define __PET_MONOPOLY_DEF_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#include "servercommon/struct/itemlistparam.h"

static const int PET_MONOPOLY_GRID_NUM = 36;
static const int PET_MONOPOLY_REWARD_RECORD_NUM = 12; // 拿到超过12个不同物品的概率极小
static const int PET_MONOPOLY_TURNTABLE_SECTOR_NUM = 8; // 转盘扇区数
static const int PET_MONOPOLY_MAX_REPLACE_GRID_NUM = 7;   //心愿卡最大替换格子数量
static const int PET_MONOPOLY_MAX_FAST_MOVE_GRID = 6;	//神行符最大移动距离
static const int PET_MONOPOLY_MAX_PET_REWARD_NUM = 6;	//最大可获得宠物数量
static const int PET_MONOPOLY_MAX_SPECIAL_ITEM = 5;		//特殊道具最大数量

typedef char PetMonopolyGridDetailData[16];

enum PET_MONOPOLY_GRID_TYPE		//!< 格子类型（也可以理解为格子的等级）
{
	PET_MONOPOLY_GRID_TYPE_STARTING_POINT = 0,	//!< 起点
	PET_MONOPOLY_GRID_TYPE_ENDING_POINT = 1,	//!< 终点
	PET_MONOPOLY_GRID_TYPE_NORMAL_GRID = 2,		//!< 普通格子
	PET_MONOPOLY_GRID_TYPE_MEDIUM_GRID = 3,		//!< 中级格子
	PET_MONOPOLY_GRID_TYPE_ADVANCE_GRID = 4,	//!< 高级格子
	PET_MONOPOLY_GRID_TYPE_MAX
};

enum PET_MONOPOLY_GRID_REWARD	//!< 格子奖励类型
{
	PET_MONOPOLY_GRID_REWARD_NONE = 0,			//!< 无奖励或奖励已被领取
	PET_MONOPOLY_GRID_REWARD_ITEM = 1,			//!< 道具奖励
	PET_MONOPOLY_GRID_REWARD_PET_MONSTER = 2,	//!< 宠物怪物组
	PET_MONOPOLY_GRID_REWARD_PET_FRAGMENT = 3,	//!< 宠物碎片
	PET_MONOPOLY_GRID_REWARD_TURNTABLE = 4,		//!< 转盘奖励
	PET_MONOPOLY_GRID_REWARD_OPPORTUNITY = 5,	//!< 机会点  
	PET_MONOPOLY_GRID_REWARD_MAX = 6,
};

enum PET_MONOPOLY_TURNTABLE_REWARD_TYPE
{
	PET_MONOPOLY_TURNTABLE_REWARD_ITEM = 0,
	PET_MONOPOLY_TURNTABLE_REWARD_PET_MONSTER = 1,
	PET_MONOPOLY_TURNTABLE_REWARD_PET_FRAGMENT = 2,
	PET_MONOPOLY_TURNTABLE_REWARD_MAX = 3,
};

enum PET_MONOPOLY_SPECIAL_ITEM_TYPE
{
	PET_MONOPOLY_SPECIAL_ITEM_LUBANDICE = 0,		//!<鲁班骰
	PET_MONOPOLY_SPECIAL_ITEM_DOUBLECARD = 1,		//!<双倍卡
	PET_MONOPOLY_SPECIAL_ITEM_FASTMOVEPAPER = 2,	//!<神行符
	PET_MONOPOLY_SPECIAL_ITEM_WISHCARD = 3,			//!<心愿卡
	PET_MONOPOLY_SPECIAL_ITEM_PORTAL= 4,			//!<传送门
	PET_MONOPOLY_SPECIAL_ITEM_MAX = 5,
};

#pragma pack(push)
#pragma pack(4)

struct PetMonopolyGridItem
{
	ItemID item_id;
	short item_num;
	char is_bind;
	char reserve_ch;
	short reserve_sh;
};

struct PetMonopolyGridPet
{
	int pet_id;
	int monster_group_id;
};

struct PetMonopolyGridTurntable
{
	int reward_group_id;
};

struct PetMonopolyGridSpecialItem
{
	int special_item_id;
};

union PetMonopolyGridDetail
{

	void Reset()
	{
		memset(data, 0, sizeof(data));
	}

	const char* GetDetailDataLog(int reward_type)
	{
		static char buffer[256];
		memset(buffer, 0, sizeof(buffer));

		switch (reward_type)
		{
		case PET_MONOPOLY_GRID_REWARD_ITEM:
			{
				SNPRINTF(buffer, sizeof(buffer), "REWARD_ITEM{item_id[%d] item_num[%d] is_bind[%d]}",
					item.item_id, item.item_num, (int)item.is_bind);
			}
			break;

		case PET_MONOPOLY_GRID_REWARD_PET_FRAGMENT:
			{
				SNPRINTF(buffer, sizeof(buffer), "REWARD_PET_FRAGMENT{item_id[%d] item_num[%d] is_bind[%d]}",
					item.item_id, item.item_num, (int)item.is_bind);
			}
			break;

		case PET_MONOPOLY_GRID_REWARD_PET_MONSTER:
			{
				SNPRINTF(buffer, sizeof(buffer), "REWARD_PET_MONSTER{pet_id[%d] monster_group[%d]}",
					pet.pet_id, pet.monster_group_id);
			}
			break;

		case PET_MONOPOLY_GRID_REWARD_TURNTABLE:
			{
				SNPRINTF(buffer, sizeof(buffer), "REWARD_TURNTABLE{table_group[%d]}",
					turntable.reward_group_id);
			}
			break;

		case PET_MONOPOLY_GRID_REWARD_OPPORTUNITY:
			{
				SNPRINTF(buffer, sizeof(buffer), "REWARD_SPECIAL_ITEM{special_item_id[%d]}",
					special_item.special_item_id);
			}
			break;

		case PET_MONOPOLY_GRID_REWARD_NONE:
			{
				SNPRINTF(buffer, sizeof(buffer), "REWARD_NONE{}");
			}
			break;

		default:
			{
				SNPRINTF(buffer, sizeof(buffer), "ERROR:UNKNOWN REWARD_TYPE");
			}
			break;
		}

		return buffer;
	}

	PetMonopolyGridItem item;
	PetMonopolyGridPet pet;
	PetMonopolyGridTurntable turntable;
	PetMonopolyGridDetailData data;
	PetMonopolyGridSpecialItem special_item;
};



UNSTD_STATIC_CHECK(sizeof(PetMonopolyGridPet) <= sizeof(PetMonopolyGridDetailData));
UNSTD_STATIC_CHECK(sizeof(PetMonopolyGridItem) <= sizeof(PetMonopolyGridDetailData));



struct PetMonopolyGrid
{
	PetMonopolyGrid()
	{
		this->Reset();
	}

	void Reset()
	{
		grid_type = 0;
		reward_type = 0;
		reserve_ll = 0LL;
		reserve_int = 0;
		reserve_sh = 0;
		version = 0;
		grid_detail.Reset();
	}

	short grid_type;			//!< 格子的类型
	short reward_type;			//!< 格子的奖励类型
	short version;				//!< 版本号
	short reserve_sh;			//!< 保留字
	int reserve_int;			//!< 保留字
	long long reserve_ll;	//!< 保留字

	PetMonopolyGridDetail grid_detail;	//!< 格子奖励具体数据
};

struct PetMonopolySectorInfo
{
	PetMonopolySectorInfo() { this->Reset(); }

	void Reset()
	{
		reward_type = 0;
		reserve_sh = 0;
		reward_detail.Reset();
	}

	short reward_type;
	short reserve_sh;
	PetMonopolyGridDetail reward_detail;
};

struct PetMonopolyParam
{
	PetMonopolyParam()
	{
		this->Reset();
	}

	void Reset()
	{
		expect_silver_pet_id = 0;
		expect_gold_pet_id = 0;
		cur_level = 0;
		cur_grid_idx = 0;
		double_reward_surplus = 0;
		
		memset(reserve_ll, 0, sizeof(reserve_ll));
		memset(special_items, 0, sizeof(special_items));
		memset(fast_move_num,-1,sizeof(fast_move_num));

		for (int grid_idx = 0; grid_idx < ARRAY_ITEM_COUNT(grid_list); ++grid_idx)
		{
			grid_list[grid_idx].Reset();
		}

		reward_record_num = 0;
		for (int item_idx = 0; item_idx < ARRAY_ITEM_COUNT(reward_list); ++item_idx)
		{
			reward_list[item_idx].Reset();
		}

		add_exp_times = 0;
		turntable_sector_idx = -1;
		
		is_catch_pet_before = 0;
		reserve_ch = 0;
		reserve_sh = 0;
		reserve_int = 0;
	}

	int expect_silver_pet_id;
	int expect_gold_pet_id;
	short cur_level;
	short cur_grid_idx;
	char is_catch_pet_before;
	char reserve_ch;
	short reserve_sh;
	int reserve_int;
	long long reserve_ll[3];
	PetMonopolyGrid grid_list[PET_MONOPOLY_GRID_NUM];

	int reward_record_num;
	ItemDataWrapperOld reward_list_old[PET_MONOPOLY_REWARD_RECORD_NUM];

	short add_exp_times;
	short turntable_sector_idx;
	PetMonopolySectorInfo turntable_sector_list[PET_MONOPOLY_TURNTABLE_SECTOR_NUM];

	int special_items[PET_MONOPOLY_SPECIAL_ITEM_MAX];
	int double_reward_surplus;
	int fast_move_num[PET_MONOPOLY_MAX_FAST_MOVE_GRID];

	ItemDataWrapper reward_list[PET_MONOPOLY_REWARD_RECORD_NUM];
};

typedef char PetMonopolyParamHex[sizeof(PetMonopolyParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(PetMonopolyParamHex) < 42000);

struct PetMonopolyItemParam
{
	PetMonopolyItemParam():item_id(0),is_bind(0),num(0)
	{}

	void Reset()
	{
		item_id = 0;
		is_bind = 0;
		num = 0;
	}

	ItemID item_id;
	short is_bind;
	int num;
};

struct NewPetMonopolyParam
{
	NewPetMonopolyParam()
	{
		this->Reset();
	}

	void Reset()
	{
		expect_silver_pet_id = 0;
		expect_gold_pet_id = 0;
		cur_level = 0;
		cur_grid_idx = 0;
		double_reward_surplus = 0;
		reward_record_num = 0;
		pet_record_num = 0;

		memset(special_items, 0, sizeof(special_items));
		memset(fast_move_num, -1, sizeof(fast_move_num));
		memset(reserve_arr, 0, sizeof(reserve_arr));

		for (int grid_idx = 0; grid_idx < ARRAY_ITEM_COUNT(grid_list); ++grid_idx)
		{
			grid_list[grid_idx].Reset();
		}

		for (int pet_idx = 0;pet_idx < ARRAY_ITEM_COUNT(pet_list);++pet_idx)
		{
			pet_list[pet_idx].Reset();
		}

		for (int item_idx = 0; item_idx < ARRAY_ITEM_COUNT(reward_list); ++item_idx)
		{
			reward_list[item_idx].Reset();
		}

		for (int turntable_idx = 0; turntable_idx < ARRAY_ITEM_COUNT(turntable_sector_list); ++turntable_idx)
		{
			turntable_sector_list[turntable_idx].Reset();
		}

		add_exp_times = 0;
		turntable_sector_idx = -1;

		is_catch_pet_before = 0;
		old_param_flag = 0;
		reserve_sh = 0;
	}

	int expect_silver_pet_id;
	int expect_gold_pet_id;
	short cur_level;
	short cur_grid_idx;
	char is_catch_pet_before;
	char old_param_flag;				// 数据迁移flag
	short reserve_sh;
	PetMonopolyGrid grid_list[PET_MONOPOLY_GRID_NUM];

	short reward_record_num;
	short pet_record_num;

	short add_exp_times;
	short turntable_sector_idx;
	PetMonopolySectorInfo turntable_sector_list[PET_MONOPOLY_TURNTABLE_SECTOR_NUM];

	int special_items[PET_MONOPOLY_MAX_SPECIAL_ITEM];
	int double_reward_surplus;
	int fast_move_num[PET_MONOPOLY_MAX_FAST_MOVE_GRID];

	int reserve_arr[20];

	ItemDataWrapper pet_list[PET_MONOPOLY_MAX_PET_REWARD_NUM];
	PetMonopolyItemParam reward_list[PET_MONOPOLY_REWARD_RECORD_NUM];
};
typedef char NewPetMonopolyParamHex[sizeof(NewPetMonopolyParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(NewPetMonopolyParamHex) < 16384);

#pragma pack(pop)

#endif