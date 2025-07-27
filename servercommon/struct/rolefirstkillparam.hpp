#ifndef __ROLE_FIRST_KILL_PARAM_HPP__
#define __ROLE_FIRST_KILL_PARAM_HPP__

#include "servercommon/servercommon.h"

struct RoleFirstKillInfo
{
	RoleFirstKillInfo()
	{
		this->Reset();
	}

	void Reset()
	{
		monster_group_id = 0;
		change_state = 0;
		timestamp = 0;
		team_member_num = 0;
		win_round = 0;
		team_level_high = 0;
		team_level_low = 0;
		team_level_average = 0;
		dead_role_num = 0;
		week_kill_flag = 0;
	}

	int monster_group_id;
	char change_state;
	unsigned int timestamp;
	int team_member_num;
	int win_round;
	int team_level_high;
	int team_level_low;
	int team_level_average;
	int dead_role_num;
	int week_kill_flag;			// 0: 本周未击杀  1：本周已击杀
};

static const int MAX_FIRST_KILL_RECORD_NUM = 5000;

class TLVSerializer;
class TLVUnserializer;
class RoleFirstKillParamList
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	int count;
	RoleFirstKillInfo info_list[MAX_FIRST_KILL_RECORD_NUM];
};

#endif