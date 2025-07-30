#ifndef __TOPIC_DEF_HPP__
#define __TOPIC_DEF_HPP__

#include <map>
#include "servercommon/serverdef.h"

enum TOPIC_TYPE
{
	TOPIC_TYPE_TEAM_CHANNEL = 0,					//!< 0: 组队频道
	TOPIC_TYPE_CHIEF_ELECTION_START = 1,			//!< 1: 首席竞选
	TOPIC_TYPE_CHIEF_ELECTION_END = 8,				//!< 8: 首席竞选
	TOPIC_TYPE_CHIEF_PEAK_BATTLE = 9,				//!< 9: 首席巅峰战
	TOPIC_TYPE_MARKET = 10,							//!< 10 市场
	TOPIC_TYPE_WORLD_BOSS_1 = 11,					//!< 11 世界boss
	TOPIC_TYPE_WORLD_BOSS_2 = 12,					//!< 12 巨龙boss
	TOPIC_TYPE_RA_PAI_MAI_HANG = 13,				//!< 13 运营活动-拍卖行
	TOPIC_TYPE_WORLD_BOSS_3 = 14,					//!< 14 世界boss3(天神降临)

	TOPIC_TYPE_MAX
};

struct Subscriber
{
	int role_id;
	GSNetID netid;
};

typedef std::map<int, Subscriber> SubscriberMap;

#endif