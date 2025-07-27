#ifndef __RA_TU_JIAN_TREASURE_PARAM_HPP__
#define __RA_TU_JIAN_TREASURE_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//////////////////////////////////////////// 2080 开服活动 - 图鉴夺宝  //////////////////////////////////////////

enum TU_JIAN_TREASURE_TYPE						//图鉴夺宝类型
{
	TU_JIAN_TREASURE_TYPE_BEGIN = 0,
	TU_JIAN_TREASURE_TYPE_WLID_BOSS,			//野外boss
	TU_JIAN_TREASURE_TYPE_FB,					//副本
	TU_JIAN_TREASURE_TYPE_MARKET,				//交易行

	TU_JIAN_TREASURE_TYPE_TYPE_MAX
};

static const int RA_TU_JIAN_TREASURE_MAXNUM = 36;					//图鉴夺宝最大seq
static const int RA_TU_JIAN_TREASURE_LOGIN_MAXNUM = 32;				//图鉴夺宝最大登陆天数

struct TuJianTreasureItem
{
	TuJianTreasureItem() { this->Reset(); }
	void Reset()
	{
		param1 = -1;
		param2 = -1;
		count = 0;
		type = 0;
	}
	int param1;				//类型1:野外bossID	类型2:副本seq	类型3:图鉴ID(0：任意图鉴)
	short param2;			//类型1、2:掉落图鉴卡ID (0：不需要掉落)	类型3:交易行操作（出售0|购买1|出售购买均可2）
	char count;
	char type;				//TU_JIAN_TREASURE_TYPE
};

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RATuJianTreasureParam
{
	RATuJianTreasureParam() { this->Reset(); }
	void Reset()
	{
		ra_tu_jian_treasure_begin_timestamp = 0;
		ra_tu_jian_treasure_end_timestamp = 0;
		ra_tu_jian_treasure_login_day = 0;
		ra_tu_jian_treasure_fetch_flag.Reset();
		ra_tu_jian_treasure_day_fetch_flag.Reset();
		for (int i = 0; i < RA_TU_JIAN_TREASURE_MAXNUM; ++i)
		{
			ra_tu_jian_treasure_item[i].Reset();
		}
		ra_tu_jian_treasure_day_id = 0;
	}

	unsigned int ra_tu_jian_treasure_begin_timestamp;
	unsigned int ra_tu_jian_treasure_end_timestamp;												//图鉴夺宝结束时间戳
	int ra_tu_jian_treasure_login_day;
	BitMap<RA_TU_JIAN_TREASURE_LOGIN_MAXNUM> ra_tu_jian_treasure_day_fetch_flag;				//登陆天数领取标志位
	BitMap<RA_TU_JIAN_TREASURE_MAXNUM> ra_tu_jian_treasure_fetch_flag;							//领取标志位
	TuJianTreasureItem	ra_tu_jian_treasure_item[RA_TU_JIAN_TREASURE_MAXNUM];

	unsigned int ra_tu_jian_treasure_day_id;
};

UNSTD_STATIC_CHECK(sizeof(RATuJianTreasureParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_TU_JIAN_TREASURE_PARAM_HPP__
