#include "rankparam.hpp"
#include "servercommon/struct/structcommon.h"

bool PersonRankParam::Serialize(TLVSerializer &outstream) const
{
	bool ret = outstream.Push(count);
	if (!ret) return false;

	for (int i = 0; i < count && i < static_cast<int>(PERSON_RANK_MAX_SIZE); ++i)
	{
		ret = outstream.Push(person_rank_list[i].change_state) && outstream.Push(person_rank_list[i].index) && outstream.Push(person_rank_list[i].rank_type);
		if (!ret) return false;

		if (person_rank_list[i].change_state == structcommon::CS_DELETE) continue;

		TLVSerializer data;
		data.Reset((void*)&person_rank_list[i].role_info, sizeof(person_rank_list[i].role_info));
		data.MoveCurPos(sizeof(person_rank_list[i].role_info));

		ret =  outstream.Push(person_rank_list[i].rank_value)
			&& outstream.Push(person_rank_list[i].record_index)
			&& outstream.Push(person_rank_list[i].flexible_int)
			&& outstream.Push(person_rank_list[i].flexible_ll) 
			&& outstream.Push(data);
		if (!ret) return false;
	}

	return true;
}

bool PersonRankParam::Unserialize(TLVUnserializer &instream)
{
	bool ret = instream.Pop(&count);
	if (!ret) return false;

	for (unsigned int i = 0; i < count && i < static_cast<int>(PERSON_RANK_MAX_SIZE); ++i)
	{
		ret = instream.Pop(&person_rank_list[i].change_state) && instream.Pop(&person_rank_list[i].index) && instream.Pop(&person_rank_list[i].rank_type);
		if (!ret) return false;

		if (person_rank_list[i].change_state == structcommon::CS_DELETE) continue;

		TLVUnserializer data;
		ret =  instream.Pop(&person_rank_list[i].rank_value)
			&& instream.Pop(&person_rank_list[i].record_index)
			&& instream.Pop(&person_rank_list[i].flexible_int)
			&& instream.Pop(&person_rank_list[i].flexible_ll) 
			&& instream.Pop(&data);

		if (data.Size() <= sizeof(person_rank_list[i].role_info))
		{
			memcpy(&person_rank_list[i].role_info, data.Ptr(), data.Size());
		}
		else
		{
			printf("DESERIALIZE data.Size() > sizeof(RoleBaseInfo) error\n");
			return false;                                               
		}

		if (!ret) return false;
	}

	return true;
}

bool TeamRankParam::Serialize(TLVSerializer &outstream) const
{
	bool ret = outstream.Push(count);
	if (!ret) return false;

	for (int i = 0; i < count && i < TEAM_RANK_TYPE_MAX * TEAM_RANK_MAX_SIZE; ++i)
	{
		ret = outstream.Push(team_rank_list[i].change_state) && outstream.Push(team_rank_list[i].index);
		if (!ret) return false;

		if (team_rank_list[i].change_state == structcommon::CS_DELETE) continue;

		for (int j = 0; j < MAX_TEAM_MEMBER_NUM; ++j)
		{
			const char *user_name_tmp = team_rank_list[i].member_list[j].user_name;
			ret = outstream.Push(team_rank_list[i].member_list[j].uid) && outstream.Push(user_name_tmp);
			if (!ret) return false;
		}

		ret = outstream.Push(team_rank_list[i].rank_type)
			&& outstream.Push(team_rank_list[i].rank_value)
			&& outstream.Push(team_rank_list[i].flexible_int)
			&& outstream.Push(team_rank_list[i].flexible_ll);
		if (!ret) return false;
	}

	return true;
}

bool TeamRankParam::Unserialize(TLVUnserializer &instream)
{
	bool ret = instream.Pop(&count);
	if (!ret) return false;

	for (int i = 0; i < count && i < TEAM_RANK_TYPE_MAX * TEAM_RANK_MAX_SIZE; ++i)
	{
		ret = instream.Pop(&team_rank_list[i].change_state) && instream.Pop(&team_rank_list[i].index);
		if (!ret) return false;

		if (team_rank_list[i].change_state == structcommon::CS_DELETE) continue;

		for (int j = 0; j < MAX_TEAM_MEMBER_NUM; ++j)
		{
			const char *user_name_tmp = NULL;
			ret = instream.Pop(&team_rank_list[i].member_list[j].uid) && instream.Pop(&user_name_tmp);
			if (!ret) return false;

			STRNCPY(team_rank_list[i].member_list[j].user_name, user_name_tmp, sizeof(team_rank_list[i].member_list[j].user_name));
		}

		ret = instream.Pop(&team_rank_list[i].rank_type)
			&& instream.Pop(&team_rank_list[i].rank_value)
			&& instream.Pop(&team_rank_list[i].flexible_int)
			&& instream.Pop(&team_rank_list[i].flexible_ll);
		if (!ret) return false;
	}

	return true;
}

bool PetRankParam::Serialize(TLVSerializer &outstream) const
{
	bool ret = outstream.Push(count);
	if (!ret) return false;

	for (int i = 0; i < count && i < PET_RANK_MAX_SIZE; ++i)
	{
		ret = outstream.Push(pet_rank_list[i].change_state) && outstream.Push(pet_rank_list[i].index) && outstream.Push(pet_rank_list[i].rank_type);
		if (!ret) return false;

		if (pet_rank_list[i].change_state == structcommon::CS_DELETE) continue;

		TLVSerializer role_data;
		role_data.Reset((void*)&pet_rank_list[i].role_info, sizeof(pet_rank_list[i].role_info));
		role_data.MoveCurPos(sizeof(pet_rank_list[i].role_info));

		TLVSerializer pet_data;
		pet_data.Reset((void*)&pet_rank_list[i].pet_info, sizeof(pet_rank_list[i].role_info));
		pet_data.MoveCurPos(sizeof(pet_rank_list[i].pet_info));

		TLVSerializer pet_attr_data;
		pet_attr_data.Reset((void*)&pet_rank_list[i].pet_attr, sizeof(pet_rank_list[i].pet_attr));
		pet_attr_data.MoveCurPos(sizeof(pet_rank_list[i].pet_attr));

		TLVSerializer pet_god_print_data;
		pet_god_print_data.Reset((void*)&pet_rank_list[i].pet_god_print, sizeof(pet_rank_list[i].pet_god_print));
		pet_god_print_data.MoveCurPos(sizeof(pet_rank_list[i].pet_god_print));

		ret = outstream.Push(pet_rank_list[i].rank_value)
			&& outstream.Push(pet_rank_list[i].record_index)
			&& outstream.Push(pet_rank_list[i].flexible_int)
			&& outstream.Push(pet_rank_list[i].flexible_ll)
			&& outstream.Push(pet_rank_list[i].str_buff_level)
			&& outstream.Push(role_data)
			&& outstream.Push(pet_data)
			&& outstream.Push(pet_attr_data)
			&& outstream.Push(pet_god_print_data);
		if (!ret) return false;
	}

	return true;
}

bool PetRankParam::Unserialize(TLVUnserializer &instream)
{
	bool ret = instream.Pop(&count);
	if (!ret) return false;

	for (int i = 0; i < count && i < PET_RANK_MAX_SIZE; ++i)
	{
		ret = instream.Pop(&pet_rank_list[i].change_state) && instream.Pop(&pet_rank_list[i].index) && instream.Pop(&pet_rank_list[i].rank_type);
		if (!ret) return false;

		if (pet_rank_list[i].change_state == structcommon::CS_DELETE) continue;

		TLVUnserializer role_data;
		TLVUnserializer pet_data;
		TLVUnserializer pet_attr_data;
		TLVUnserializer pet_god_print_data;
		ret =  instream.Pop(&pet_rank_list[i].rank_value)
			&& instream.Pop(&pet_rank_list[i].record_index)
			&& instream.Pop(&pet_rank_list[i].flexible_int)
			&& instream.Pop(&pet_rank_list[i].flexible_ll)
			&& instream.Pop(&pet_rank_list[i].str_buff_level)
			&& instream.Pop(&role_data)
			&& instream.Pop(&pet_data)
			&& instream.Pop(&pet_attr_data)
			&& instream.Pop(&pet_god_print_data);

		if (role_data.Size() <= sizeof(pet_rank_list[i].role_info))
		{
			memcpy(&pet_rank_list[i].role_info, role_data.Ptr(), role_data.Size());
		}
		else
		{
			printf("DESERIALIZE data.Size() > sizeof(RoleBaseInfo) error\n");
			return false;
		}

		if (pet_data.Size() <= sizeof(pet_rank_list[i].pet_info))
		{
			memcpy(&pet_rank_list[i].pet_info, pet_data.Ptr(), pet_data.Size());
		}
		else
		{
			printf("DESERIALIZE data.Size() > sizeof(pet_rank_list[i].pet_info) error\n");
			return false;
		}

		if (pet_attr_data.Size() <= sizeof(pet_rank_list[i].pet_attr))
		{
			memcpy(&pet_rank_list[i].pet_attr, pet_attr_data.Ptr(), pet_attr_data.Size());
		}
		else
		{
			printf("DESERIALIZE data.Size() > sizeof(pet_rank_list[i].pet_attr) error\n");
			return false;
		}

		if (pet_god_print_data.Size() <= sizeof(pet_rank_list[i].pet_god_print))
		{
			memcpy(&pet_rank_list[i].pet_god_print, pet_god_print_data.Ptr(), pet_god_print_data.Size());
		}
		else
		{
			printf("DESERIALIZE data.Size() > sizeof(pet_rank_list[i].pet_god_print) error\n");
			return false;
		}

		if (!ret) return false;
	}

	return true;
}

bool PartnerRankParam::Serialize(TLVSerializer & outstream) const
{
	bool ret = outstream.Push(count);
	if (!ret) return false;

	for (int i = 0; i < count && i < PARTNER_RANK_MAX_SIZE; ++i)
	{
		ret = outstream.Push(partner_rank_list[i].change_state) && outstream.Push(partner_rank_list[i].index);
		if (!ret) return false;

		if (partner_rank_list[i].change_state == structcommon::CS_DELETE) continue;

		ret =  outstream.Push(partner_rank_list[i].user_uid)
			&& outstream.Push(partner_rank_list[i].partner_id)
			&& outstream.Push(partner_rank_list[i].rank_type)
			&& outstream.Push(partner_rank_list[i].rank_value)
			&& outstream.Push(partner_rank_list[i].flexible_int)
			&& outstream.Push(partner_rank_list[i].flexible_ll)
			;
		if (!ret) return false;
	}

	return true;
}

bool PartnerRankParam::Unserialize(TLVUnserializer & instream)
{
	bool ret = instream.Pop(&count);
	if (!ret) return false;

	for (int i = 0; i < count && i < PARTNER_RANK_MAX_SIZE; ++i)
	{
		ret = instream.Pop(&partner_rank_list[i].change_state) && instream.Pop(&partner_rank_list[i].index);
		if (!ret) return false;

		if (partner_rank_list[i].change_state == structcommon::CS_DELETE) continue;

		ret = instream.Pop(&partner_rank_list[i].user_uid)
			&& instream.Pop(&partner_rank_list[i].partner_id)
			&& instream.Pop(&partner_rank_list[i].rank_type)
			&& instream.Pop(&partner_rank_list[i].rank_value)
			&& instream.Pop(&partner_rank_list[i].flexible_int)
			&& instream.Pop(&partner_rank_list[i].flexible_ll)
			;
		if (!ret) return false;
	}

	return true;
}
