#ifndef __DRAW_DEF_HPP__
#define __DRAW_DEF_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/msgserialize.hpp"

//最大翻牌奖励数
static const int DRAW_GROUP_MAX_ITEM_NUM = 5;

//奖励组 默认超时时间(秒)
static const unsigned int DRAW_GROUP_DEF_OUT_TIME = 20;
static const int MAX_ITEM_LENGTH_NUM = 1256;

//奖励组类型
enum DrawGroupType
{
	DRAW_GROUP_TYPE_DEF = 0,					//奖励组类型 默认
	DRAW_GROUP_TYPE_MI_YI = 1,					//远征
	DRAW_GROUP_TYPE_FIELD_BOSS = 2,				//野外boss
	DRAW_GROUP_TYPE_SHAN_HAI = 3,				//山海降妖
	DRAW_GROUP_TYPE_CRYSTAL = 4,				//水晶副本通关
	DRAW_GROUP_TYPE_CRYSTAL_FAST_PASS = 5,		//水晶副本扫荡
	DRAW_GROUP_TYPE_PET_GOD_FB = 6,				//神印之地
	DRAW_GROUP_TYPE_PET_GOD_FB_AUTO = 7,		//神印之地 - 扫荡
	DRAW_GROUP_TYPE_HUNDRED_GHOST = 8,			//百鬼夜行

	DRAW_GROUP_TYPE_MAX,
};

//翻牌奖励物品vector
typedef std::vector<ItemDataWrapper> DrawGroupRewardItemVec;

struct ItemDrawInfo
{
	ItemDrawInfo()
	{
		this->Reset();
	}

	bool Serialize(TLVSerializer2 & outstream) const ;
	void Reset()
	{
		param_len = 0;
		item_info.Reset();
	}
	int param_len;
	ItemDataWrapper item_info;
};

//翻牌奖励货币map<DrawGroupVirtualType, num>
typedef std::map<int, long long> DrawGroupRewardMoneyMap;
enum DrawGroupVirtualType
{
	DRAW_GROUP_VIRTUAL_TYPE_LUCK_COIN = 0,		//气运值

	DRAW_GROUP_VIRTUAL_TYPE_MAX,
};

//翻牌奖励参数(现在没用 准备通过type允许一些特殊处理  要的时候在加吧)
typedef std::vector<int> DrawGroupRewardParamVec;

//奖励组信息 有vector成员
struct DrawGroupInfo
{
	struct DrawItem
	{
		DrawItem()
		{
			role_uid = 0;
			draw_index = -1;
		}
		int role_uid;		//拥有者id
		
		int draw_index;		//领取index
							//-1表示未领取 [0, DRAW_GROUP_MAX_ITEM_NUM) 表示玩家 手动 或 系统随机 翻牌时选择的序号

		//奖励信息
		//物品
		DrawGroupRewardItemVec item_list;
		//货币
		DrawGroupRewardMoneyMap money_type;

		//翻牌奖励参数(现在没用 准备通过type(按功能分类)处理一些特殊处理 要的时候在加吧)
 		//int param_type;
 		//DrawGroupRewardParamVec param_list;
		//传入一个静态函数指针用来支持一些特殊处理
		//static void (*special_fun)(Role *role , DrawInfo &this_info); 
	};
	
	DrawGroupInfo()
	{
		draw_group_id = 0;
		out_time = 0;
		draw_group_type = 0;

		draw_count = 0;
	}

	int draw_group_id;		//奖励组id 自动分配
	unsigned int out_time;			//超时时间戳
	int draw_group_type;			//奖励组 类型

	//翻牌奖励数量(奖励的所有者可以为空 0==role_uid )
	int draw_count;
	DrawItem draw_info_list[DRAW_GROUP_MAX_ITEM_NUM];
	
};

#endif // 翻牌   __DRAW_DEF_HPP__