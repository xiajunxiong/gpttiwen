
#ifndef ROLEINFOLIST_H
#define ROLEINFOLIST_H

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"

class TLVSerializer;
class TLVUnserializer;

class RoleInfoList
{
public:
	bool Serialize(TLVSerializer *outstream) const;
	bool Unserialize(TLVUnserializer *instream);

	struct __roleinfo
	{
		bool is_in_cross;
		char color;
		short avatar_type;
		short headshot_id;
		short fly_flag;
		
		int role_id;
		int scene_id;
		int last_scene_id;
		int level;
		int profession;
		int weapon_id;
		int top_level;
		unsigned int create_time;
		unsigned int last_save_time;
		unsigned int forbid_time;
		GameName role_name;
		short surface_list[SURFACE_TYPE_MAX];
	};

	int recent_login_role_index;
	int count;
	__roleinfo role_info_list[MAX_ACCOUNT_ROLE_NUM];
};

#endif
