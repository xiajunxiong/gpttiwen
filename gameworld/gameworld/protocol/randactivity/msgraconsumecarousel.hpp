#ifndef __MSG_RA_CONSUME_CAROUSEL_HPP__
#define __MSG_RA_CONSUME_CAROUSEL_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/raconsumecarouselparam.hpp"

//////////////////////////////////////////// 2055 随机活动  消费转盘  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// 由协议  3000
	enum RA_CONSUME_CAROUSEL_OPERA_TYPE
	{
		RA_CONSUME_CAROUSEL_OPERA_TYPE_GET_INFO = 0,						// 获取信息
		RA_CONSUME_CAROUSEL_OPERA_TYPE_CONSUME_ONE = 1,						// 消费转盘启动  转动1次 
		RA_CONSUME_CAROUSEL_OPERA_TYPE_CONSUME_ALL = 2,						// 消费转盘启动  转盘全部次数
		RA_CONSUME_CAROUSEL_OPERA_TYPE_GET_REWARD = 3,						// 获取奖励  param_1:序号

	};


	struct SCRAConsumeCarouselInfo             //3008 消费转盘信息
	{
	public:
		SCRAConsumeCarouselInfo();
		MessageHeader header;

		int last_num;													//剩余次数
		unsigned int consume_num_day;									//今日消费数量
		unsigned int consume_count;										//累计总消费
	};

	struct SCRAConsumeCarouselReward		//3009 消费转盘单次转动下发
	{
	public:
		SCRAConsumeCarouselReward();
		MessageHeader header;

		int index;
	};

	struct SCRAConsumeCarouselShow			//3010 消费转盘珍稀显示下发
	{
	public:
		SCRAConsumeCarouselShow();
		MessageHeader header;

		int count;																					//当前已经抽取的人数      0-10
		RAShowInfoParam::RAShowInfoDataParam consume_carousel_list[RA_SHOW_INFO_MAX_NUM];			//抽取到珍稀物品的列表    RA_SHOW_INFO_MAX_NUM -- 10
	};

}

#pragma pack(pop)

#endif  // __MSG_RA_CONSUME_CAROUSEL_HPP__
