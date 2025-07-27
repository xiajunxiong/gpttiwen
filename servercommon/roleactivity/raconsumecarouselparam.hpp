#ifndef __RA_CONSUME_CAROUSEL_PARAM_HPP__
#define __RA_CONSUME_CAROUSEL_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//////////////////////////////////////////// 2055 随机活动 - 消费转盘  //////////////////////////////////////////

//-----------------------------消费转盘------------------------------------
static const int CONSUME_CAROUSEL_MAX_NUM = 12;
static const int RA_SHOW_INFO_MAX_NUM = 10;

struct RAShowInfoParam
{
	RAShowInfoParam()
	{
		this->Reset();
	}
	void Reset()
	{
		count = 0;
		for (int i = 0; i < RA_SHOW_INFO_MAX_NUM; i++)
		{
			m_ra_show_data_list[i].Reset();
		}

	}
	struct RAShowInfoDataParam
	{
		RAShowInfoDataParam()
		{
			this->Reset();
		}
		void Reset()
		{
			item_id = 0;
			num = 0;
			memset(role_name, 0, sizeof(role_name));
		}

		unsigned short item_id;
		short num;
		GameName role_name;
	};
	int count;			//当前抽取到珍稀物品的次数 不大于10
	RAShowInfoDataParam m_ra_show_data_list[RA_SHOW_INFO_MAX_NUM];
};


#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RAConsumerCarouselParam
{
	RAConsumerCarouselParam() { this->Reset(); }

	void Reset()
	{
		ra_consume_carousel_begin_timestamp = 0;
		old_ra_consume_carousel_last_num = 0;
		old_ra_consume_carousel_consume_num = 0;
		ra_consume_carousel_last_index = -1;
		old_num_data_move_flag = 0;
		reserve_ch = 0;
		ra_today_consume_num = 0;
		ra_consume_num_in_time = 0;
		new_ra_consume_carousel_last_num = 0;
		new_ra_consume_carousel_consume_num = 0;
	}

	unsigned int ra_consume_carousel_begin_timestamp;
	short old_ra_consume_carousel_last_num;								//剩余次数 -> 废弃
	short old_ra_consume_carousel_consume_num;							//消耗次数 -> 废弃
	short ra_consume_carousel_last_index;								//上次抽取的道具index
	char old_num_data_move_flag;										//旧次数迁移标识 0:未迁移 1:已迁移
	char reserve_ch;													//保留
	unsigned int ra_today_consume_num;									//今日累积消费
	unsigned int ra_consume_num_in_time;								//活动时间内累积消费
	int new_ra_consume_carousel_last_num;								//剩余次数
	int new_ra_consume_carousel_consume_num;							//消耗次数
};

UNSTD_STATIC_CHECK(sizeof(RAConsumerCarouselParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_CONSUME_CAROUSEL_PARAM_HPP__
