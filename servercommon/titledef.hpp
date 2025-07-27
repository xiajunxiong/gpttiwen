#ifndef __TITLE_DEF_HPP__
#define __TITLE_DEF_HPP__

#include "servercommon/servercommon.h"

#pragma pack(push, 4)

enum TITLESOURCETYPE
{
	TITLESOURCETYPE_INVALID = 0,

	TITLESOURCETYPE_DEFAULT = 1,					// 保存
	TITLESOURCETYPE_CALCULATION = 2,				// 不保存，算出来
	TITLESOURCETYPE_COLOSSEUM = 3,					// 古神宝库
	TITLESOURCETYPE_BRAVE_GROUND_TOP_ROLE = 4,					// 勇闯地宫排行榜前3
	TITLESOURCETYPE_HUAN_JIE_ZHAN_CHANG_TOP_ROLE = 5,					// 幻界战场赛季排名称号
	TITLESOURCETYPE_GUILD_HONOR_BATTLE = 6,					// 家族荣誉战
	TITLESOURCETYPE_CLOUD_ARENA = 7,				// 云端竞技
	TITLESOURCETYPE_QING_YUAN = 8,				// 情缘

	TITLESOURCETYPE_MAX,
};


static const int TITLE_MAX_NUM = 200;

struct TitleData
{
	TitleData(UInt16 _title_id = 0, UInt16 _reserve_ush = 0, UInt32 _due_time = 0)
	{
		title_id = _title_id;
		reserve_ush = _reserve_ush;
		due_time = _due_time;
	}

	void Reset()
	{
		title_id = 0;
		reserve_ush = 0;
		due_time = 0;
	}

	UInt16 title_id;				// 称号id
	UInt16 reserve_ush;
	UInt32 due_time;				// 到期时间  0 表示不限时
};

struct TitleParam
{
	TitleParam() { this->Reset(); }

	void Reset()
	{
		wear_index = -1;
		wear_title_id = 0;
		for (int i = 0; i < TITLE_MAX_NUM; ++i)
		{
			title_list[i].Reset();
		}
		is_first_create_role = 0;
		is_first_syn_title_to_facescore = 0;
		reserve_short = 0;
	}

	short wear_index;						// 带着的称号index
	UInt16 wear_title_id;					// 带着的称号id，防止有些称号不保存，登录时候计算才计算是否拥有，index就会错
	TitleData title_list[TITLE_MAX_NUM];
	char is_first_create_role;				// 判断玩家是否是刚创建角色 0 刚创建 1 老角色
	char is_first_syn_title_to_facescore;	// 是否同步称号到颜值系统 0:未同步 1:已同步
	short reserve_short;
};

typedef char TitleParamHex[sizeof(TitleParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(TitleParamHex) < 4096);

#pragma pack(pop)
#endif // !__TITLE_DEF_HPP__