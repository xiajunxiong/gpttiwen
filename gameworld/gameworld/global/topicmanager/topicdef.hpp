#ifndef __TOPIC_DEF_HPP__
#define __TOPIC_DEF_HPP__

#include <map>
#include "servercommon/serverdef.h"

enum TOPIC_TYPE
{
	TOPIC_TYPE_TEAM_CHANNEL = 0,					//!< 0: ���Ƶ��
	TOPIC_TYPE_CHIEF_ELECTION_START = 1,			//!< 1: ��ϯ��ѡ
	TOPIC_TYPE_CHIEF_ELECTION_END = 8,				//!< 8: ��ϯ��ѡ
	TOPIC_TYPE_CHIEF_PEAK_BATTLE = 9,				//!< 9: ��ϯ�۷�ս
	TOPIC_TYPE_MARKET = 10,							//!< 10 �г�
	TOPIC_TYPE_WORLD_BOSS_1 = 11,					//!< 11 ����boss
	TOPIC_TYPE_WORLD_BOSS_2 = 12,					//!< 12 ����boss
	TOPIC_TYPE_RA_PAI_MAI_HANG = 13,				//!< 13 ��Ӫ�-������
	TOPIC_TYPE_WORLD_BOSS_3 = 14,					//!< 14 ����boss3(������)

	TOPIC_TYPE_MAX
};

struct Subscriber
{
	int role_id;
	GSNetID netid;
};

typedef std::map<int, Subscriber> SubscriberMap;

#endif