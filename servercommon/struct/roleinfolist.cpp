#include "roleinfolist.h"
#include "common/tlvprotocol.h"

#include "servercommon/servercommon.h"

OLD_USE_INTERNAL_NETWORK();

bool RoleInfoList::Serialize(TLVSerializer *outstream) const
{
	bool ret = outstream->Push(count);
	if (!ret) return false;

	for (int i = 0; i < count && i < MAX_ACCOUNT_ROLE_NUM; ++i)
	{
		ret = outstream->Push(role_info_list[i].role_id)
			&& outstream->Push(role_info_list[i].scene_id)
			&& outstream->Push(role_info_list[i].last_scene_id)
			&& outstream->Push(role_info_list[i].level)
			&& outstream->Push(role_info_list[i].profession)
			&& outstream->Push(role_info_list[i].create_time)
			&& outstream->Push(role_info_list[i].avatar_type)
			&& outstream->Push(role_info_list[i].headshot_id)
			&& outstream->Push(role_info_list[i].fly_flag)
			&& outstream->Push(role_info_list[i].color)
			&& outstream->Push(role_info_list[i].weapon_id)
			&& outstream->Push(role_info_list[i].is_in_cross)
			&& outstream->Push(role_info_list[i].last_save_time)
			&& outstream->Push(role_info_list[i].forbid_time)
			&& outstream->Push(role_info_list[i].top_level);

		if (!ret) return false;

		const char *name = role_info_list[i].role_name;
		ret = outstream->Push(name);
		if (!ret) return false;

		for (int j = 0; j < SURFACE_TYPE_MAX; j++)
		{
			ret = outstream->Push(role_info_list[i].surface_list[j]);
			if (!ret) return false;
		}
	}

	ret = outstream->Push(recent_login_role_index);
	if (!ret) return false;

	return ret;
}

bool RoleInfoList::Unserialize(TLVUnserializer *instream)
{
	bool ret = instream->Pop(&count);
	if (!ret) return false;

	for (int i = 0; i < count && i < MAX_ACCOUNT_ROLE_NUM; ++i)
	{
		ret = instream->Pop(&role_info_list[i].role_id)
			&& instream->Pop(&role_info_list[i].scene_id)
			&& instream->Pop(&role_info_list[i].last_scene_id)
			&& instream->Pop(&role_info_list[i].level)
			&& instream->Pop(&role_info_list[i].profession)
			&& instream->Pop(&role_info_list[i].create_time)
			&& instream->Pop(&role_info_list[i].avatar_type)
			&& instream->Pop(&role_info_list[i].headshot_id)
			&& instream->Pop(&role_info_list[i].fly_flag)
			&& instream->Pop(&role_info_list[i].color)
			&& instream->Pop(&role_info_list[i].weapon_id)
			&& instream->Pop(&role_info_list[i].is_in_cross)
			&& instream->Pop(&role_info_list[i].last_save_time)
			&& instream->Pop(&role_info_list[i].forbid_time)
			&& instream->Pop(&role_info_list[i].top_level);

		if (!ret) return false;

		const char *name = 0;
		ret = instream->Pop(&name);
		if (!ret) return false;

		STRNCPY(role_info_list[i].role_name, name, sizeof(role_info_list[i].role_name));

		for (int j = 0; j < SURFACE_TYPE_MAX; j++)
		{
			ret = instream->Pop(&role_info_list[i].surface_list[j]);
			if (!ret) return false;
		}
	}

	ret = instream->Pop(&recent_login_role_index);
	if (!ret) return false;

	return ret;
}
