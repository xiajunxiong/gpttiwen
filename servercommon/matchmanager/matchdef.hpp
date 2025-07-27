#ifndef __MATCH_DEF_HPP__
#define __MATCH_DEF_HPP__

#include <vector>
#include <cstring>
#include "servercommon/serverdef.h"

enum MATCH_BUSINESS_TYPE
{
	MATCH_BUSINESS_NONE = 0,
	MATCH_BUSINESS_ACTIVITY_TEAM_FIGHT = 1,	//!< Ð¡¶Ó¶·¼¼
	MATCH_BUSINESS_ACTIVITY_JOY_SEEK_PET = 2,	//!< »¶ÀÖÑ°³è
	MATCH_BUSINESS_ACTIVITY_RELIC_LOOTING = 3,	//!< ÒÅ¼£¶á±¦
	MATCH_BUSINESS_MAX,
};

struct MatchUserLogic_Basic
{
	int level;
	long long capability;
};

union MatchUserLogic
{
	MatchUserLogic_Basic mulb;
};

class MatchUser
{
public:
	void Reset()
	{
		uid = 0;
		logic_type = 0;
		level = 0;
		profession = 0;
		avatar_type = 0;
		memset(user_name, 0, sizeof(user_name));
		memset(&user_info, 0, sizeof(user_info));
	}

	static MatchUser* Create()
	{
		return new MatchUser();
	}

	void Destroy()
	{
		delete this;
	}	

	int uid;
	int logic_type;
	int profession;
	short level;
	short avatar_type;
	short headshot_id;
	GameName user_name;
	MatchUserLogic user_info;

private:
	MatchUser() 
	{
		this->Reset();
	}

	~MatchUser()
	{

	}
};


struct MatchTeamLogic_Basic
{
	int average_level;
	int total_level;
	long long average_capability;
	long long total_capability;
};

union MatchTeamLogic
{
	MatchTeamLogic_Basic mtlb;
};

struct MatchTeam
{
	typedef std::vector<MatchUser*> MatchUserList;

	bool IsValid() { return is_valid; }

	MatchTeam() : is_valid(false), logic_type(0), start_match_timestamp(0), team_id(0)
	{

	}

	~MatchTeam()
	{
		for (size_t i = 0; i < user_list.size(); ++i)
		{
			if (NULL != user_list[i])
			{
				user_list[i]->Destroy();
				user_list[i] = NULL;
			}
			
		}
	}

	void Destroy()
	{
		delete this;
	}

	bool is_valid;
	int logic_type;
	unsigned int start_match_timestamp;
	long long team_id;
	MatchTeamLogic team_info;
	MatchUserList user_list;
};

#endif