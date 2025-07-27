#include "rolequeryparam.hpp"

#define SERIALIZE_OTHER_PARAM(SERIALIZER, PARAM)					\
{																	\
	SERIALIZER.Reset((void*)&PARAM, sizeof(PARAM));					\
	SERIALIZER.MoveCurPos(sizeof(PARAM));							\
}																	

#define DESERIALIZE_OTHER_PARAM(DESERIALIZER, PARAM)				\
{																	\
	if (DESERIALIZER.Size() <= sizeof(PARAM))						\
	{																\
	memcpy(&PARAM, DESERIALIZER.Ptr(), DESERIALIZER.Size());		\
	}																\
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool RoleQueryParam::Serialize(TLVSerializer &outstream) const
{
	TLVSerializer role_init_data;
	SERIALIZE_OTHER_PARAM(role_init_data, role_init_param);

	TLVSerializer appearance_data;
	SERIALIZE_OTHER_PARAM(appearance_data, appearance_info);

	TLVSerializer pet_common_data;
	SERIALIZE_OTHER_PARAM(pet_common_data, pet_common_param);

	TLVSerializer partner_data;
	SERIALIZE_OTHER_PARAM(partner_data, partner_param);

	TLVSerializer common_data;
	SERIALIZE_OTHER_PARAM(common_data, common_param);

	TLVSerializer god_print_data;
	SERIALIZE_OTHER_PARAM(god_print_data, god_print_param);

	TLVSerializer face_score_data;
	SERIALIZE_OTHER_PARAM(face_score_data, face_score_param);

	TLVSerializer smart_mounts_data;
	SERIALIZE_OTHER_PARAM(smart_mounts_data, smart_mounts_param);

	const char *p_rolename = role_name;

	bool ret = outstream.Push(role_id) &&
		outstream.Push(p_rolename) &&
		outstream.Push(sex) &&
		outstream.Push(prof) &&
		outstream.Push(vip_level) &&
		outstream.Push(rank_score) &&
		outstream.Push(is_finish_ding_ji) &&
		outstream.Push(level) &&
		outstream.Push(top_level) &&
		outstream.Push(avatar_type) &&
		outstream.Push(headshot_id) &&
		outstream.Push(avatar_timestamp) &&
		outstream.Push(kill_value) &&
		outstream.Push(course_fraction_count) &&
		outstream.Push(role_init_data) &&
		outstream.Push(appearance_data) &&
		outstream.Push(pet_common_data) &&
		outstream.Push(partner_data) &&
		outstream.Push(common_data) &&
		outstream.Push(god_print_data) &&
		outstream.Push(face_score_data) &&
		outstream.Push(smart_mounts_data);

	if (!ret)
	{
		return false;
	}

	return true;
}

bool RoleQueryParam::Unserialize(TLVUnserializer &instream)
{	
	TLVUnserializer role_init_data;
	TLVUnserializer appearance_data;
	TLVUnserializer pet_common_data;
	TLVUnserializer partner_data;
	TLVUnserializer common_data;
	TLVUnserializer god_print_data;
	TLVUnserializer face_score_data;
	TLVUnserializer smart_mounts_data;

	const char *p_rolename = NULL;

	bool ret = instream.Pop(&role_id) &&
		instream.Pop(&p_rolename) &&
		instream.Pop(&sex) &&
		instream.Pop(&prof) &&
		instream.Pop(&vip_level) &&
		instream.Pop(&rank_score) &&
		instream.Pop(&is_finish_ding_ji) &&
		instream.Pop(&level) &&
		instream.Pop(&top_level) &&
		instream.Pop(&avatar_type) &&
		instream.Pop(&headshot_id) &&
		instream.Pop(&avatar_timestamp) &&
		instream.Pop(&kill_value) &&
		instream.Pop(&course_fraction_count) &&
		instream.Pop(&role_init_data) &&
		instream.Pop(&appearance_data) &&
		instream.Pop(&pet_common_data) &&
		instream.Pop(&partner_data) &&
		instream.Pop(&common_data) &&
		instream.Pop(&god_print_data) &&
		instream.Pop(&face_score_data) &&
		instream.Pop(&smart_mounts_data);

	if (!ret)
	{
		return false;
	}

	DESERIALIZE_OTHER_PARAM(role_init_data, role_init_param);
	DESERIALIZE_OTHER_PARAM(appearance_data, appearance_info);
	DESERIALIZE_OTHER_PARAM(pet_common_data, pet_common_param);
	DESERIALIZE_OTHER_PARAM(partner_data, partner_param);
	DESERIALIZE_OTHER_PARAM(common_data, common_param);
	DESERIALIZE_OTHER_PARAM(face_score_data, face_score_param);
	DESERIALIZE_OTHER_PARAM(smart_mounts_data, smart_mounts_param);
	STRNCPY(role_name, p_rolename, sizeof(role_name));

	return true;
}