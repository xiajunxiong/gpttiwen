#include "platformbattledef.hpp"
#include "servercommon/protobuf/platform_battle_role_data.pb.h"
#include "servercommon/protobuf/platform_battle_pet.pb.h"
#include "servercommon/protobuf/platform_battle_partner.pb.h"

bool PlatformBattleRoleData::IsRobot() const
{
	return role_id < 0;
}

int PlatformBattleRoleData::GetRobotDataIndex() const
{
	if (this->IsRobot())
	{
		return param1 - 1;
	}

	return -1;
}

int PlatformBattleRoleData::GetAllCapacity() const
{
	int capacity = 0;
	for (int i = 0; i < PLATFORM_BATTLE_PET_PARTNER_MAX; ++i)
	{
		if (battling_pet[i].IsValid())
		{
			capacity += battling_pet[i].pet_capability;
		}

		if (battling_partner[i].IsValid())
		{
			capacity += battling_partner[i].capability;
		}
	}

	return capacity;
}

bool PlatformBattleRoleData::SetDataFromPB(const PB_PlatformBattleRoleData & pb_data)
{
	role_id = pb_data.role_id();
	role_pet_train_skill_level = pb_data.role_pet_train_skill_level();
	role_level = pb_data.role_level();
	avatar_type = pb_data.avatar_type();
	headshort_id = pb_data.headshot_id();
	F_STRNCPY(role_name, pb_data.role_name().c_str(), sizeof(role_name));
	prof = static_cast<short>(pb_data.prof());
	param1 = static_cast<short>(pb_data.param1());
	top1_time = pb_data.top1_time();

	for (int i = 0; i < PLATFORM_BATTLE_PET_PARTNER_MAX && i < pb_data.battling_pet_size(); ++i)
	{
		const PB_PlatformBattlePet & pb_pet = pb_data.battling_pet(i);
		if (!battling_pet[i].SetDataFromPB(pb_pet))
		{
			return false;
		}
	}

	for (int i = 0; i < PLATFORM_BATTLE_PET_PARTNER_MAX && i < pb_data.battling_partner_size(); ++i)
	{
		const PB_PlatformBattlePartner & pb_partner = pb_data.battling_partner(i);
		if (!battling_partner[i].SetDataFromPB(pb_partner))
		{
			return false;
		}
	}

	return true;
}

bool PlatformBattleRoleData::GetPBData(ARG_OUT PB_PlatformBattleRoleData * out_data) const
{
	out_data->set_role_id(role_id);
	out_data->set_role_pet_train_skill_level(role_pet_train_skill_level);
	out_data->set_role_level(role_level);
	out_data->set_avatar_type(avatar_type);
	out_data->set_headshot_id(headshort_id);
	out_data->set_role_name(role_name);
	out_data->set_prof(prof);
	out_data->set_param1(param1);
	out_data->set_top1_time(top1_time);

	out_data->mutable_battling_pet()->Clear();	//repeated使用前Clear是好习惯
	out_data->mutable_battling_pet()->Reserve(PLATFORM_BATTLE_PET_PARTNER_MAX);
	for (int i = 0; i < PLATFORM_BATTLE_PET_PARTNER_MAX; ++i)
	{
		PB_PlatformBattlePet * pb_pet = out_data->add_battling_pet();
		if (!battling_pet[i].GetPBData(pb_pet))
		{
			return false;
		}
	}

	out_data->mutable_battling_partner()->Clear();	//repeated使用前Clear是好习惯
	out_data->mutable_battling_partner()->Reserve(PLATFORM_BATTLE_PET_PARTNER_MAX);
	for (int i = 0; i < PLATFORM_BATTLE_PET_PARTNER_MAX; ++i)
	{
		PB_PlatformBattlePartner * pb_partner = out_data->add_battling_partner();
		if (!battling_partner[i].GetPBData(pb_partner))
		{
			return false;
		}
	}

	return true;
}

bool PlatformBattlePet::SetDataFromPB(const PB_PlatformBattlePet & pb_data)
{
	item_unique_id = pb_data.item_unique_id();
	pet_id = pb_data.pet_id();
	pos = pb_data.pos();
	pet_capability = pb_data.pet_capability();
	pet_level = pb_data.pet_level();
	pet_strengthen_level = pb_data.pet_strengthen_level();
	F_STRNCPY(pet_name, pb_data.pet_name().c_str(), sizeof(pet_name));
	if (!pet_battle_data.SetDataFromPB(pb_data.pet_battle_data()))
	{
		return false;
	}

	return true;
}

bool PlatformBattlePet::GetPBData(ARG_OUT PB_PlatformBattlePet * out_data) const
{
	out_data->set_item_unique_id(item_unique_id);
	out_data->set_pet_id(pet_id);
	out_data->set_pos(pos);
	out_data->set_pet_capability(pet_capability);
	out_data->set_pet_level(pet_level);
	out_data->set_pet_strengthen_level(pet_strengthen_level);
	out_data->set_pet_name(pet_name, sizeof(pet_name));

	if (!pet_battle_data.GetPBData(out_data->mutable_pet_battle_data()))
	{
		return false;
	}

	return true;
}

bool PlatformBattlePartner::SetDataFromPB(const PB_PlatformBattlePartner & pb_data)
{
	pos = pb_data.pos();
	level = pb_data.level();
	partner_id = pb_data.partner_id();
	capability = pb_data.capability();

	if (!partner_battle_data.SetDataFromPB(pb_data.partner_battle_data()))
	{
		return false;
	}

	return true;
}

bool PlatformBattlePartner::GetPBData(ARG_OUT PB_PlatformBattlePartner * out_data) const
{
	out_data->set_pos(pos);
	out_data->set_level(level);
	out_data->set_partner_id(partner_id);
	out_data->set_capability(capability);

	if (!partner_battle_data.GetPBData(out_data->mutable_partner_battle_data()))
	{
		return false;
	}

	return true;
}

PlatformBattleRoleBattleInfoItem::PlatformBattleRoleBattleInfoItem()
{
	platform_number = 0;
	rank = 0;
}


PlatformBattleRoleBattleInfoItem::PlatformBattleRoleBattleInfoItem(int platform_num,int rank)
{
	this->platform_number = platform_num;
	this->rank = rank;
}

void PlatformBattleRoleBattleInfoItem::SetPetAndPartnerData(const PlatformBattleRoleData & rank_item)
{
	for (int i = 0; i < PLATFORM_BATTLE_PET_PARTNER_MAX; ++i)
	{
		if (rank_item.battling_pet[i].item_unique_id != 0)
		{
			this->battling_pet.push_back(std::pair<unsigned long long, int>(rank_item.battling_pet[i].item_unique_id, rank_item.battling_pet[i].pos));
		}

		if (rank_item.battling_partner[i].partner_id != 0)
		{
			this->battling_partner.push_back(std::pair<int, int>(rank_item.battling_partner[i].partner_id, rank_item.battling_partner[i].pos));
		}
	}
}

bool PlatformBattleRoleBattleInfoItem::IsInvaild() const
{
	if (!battling_partner.empty() || !battling_pet.empty())
	{
		return false;
	}

	return true;
}

bool MsgPlatformBattleRoleInfoItemList::Serialize(TLVSerializer & outstream) const
{
	bool result = true;
	unsigned int count = role_info_items.size();

	if (count > (unsigned int)PLATFORM_BATTLE_MAX_JOIN_NUM)
	{
		return false;
	}

	result = result && outstream.Push(role_id);
	result = result && outstream.Push(count);

	for (std::map<int, PlatformBattleRoleBattleInfoItem >::const_iterator iter = role_info_items.begin(); iter != role_info_items.end(); ++iter)
	{
		const PlatformBattleRoleBattleInfoItem & curr = iter->second;
		result = result && outstream.Push(curr.platform_number);
		result = result && outstream.Push(curr.rank);
		if (curr.rank != 0)
		{
			size_t pet_count = curr.battling_pet.size();
			if (pet_count > (size_t)PLATFORM_BATTLE_PET_PARTNER_MAX)
			{
				return false;
			}

			result = result && outstream.Push(pet_count);
			for (std::vector<std::pair<unsigned long long, int> >::const_iterator iter = curr.battling_pet.begin(); iter != curr.battling_pet.end(); ++iter)
			{
				result = result &&outstream.Push(iter->first);
				result = result &&outstream.Push(iter->second);
			}

			size_t partner_count = curr.battling_partner.size();
			if (partner_count > (size_t)PLATFORM_BATTLE_PET_PARTNER_MAX)
			{
				return false;
			}

			result = result && outstream.Push(partner_count);
			for (std::vector<std::pair<int, int> >::const_iterator iter = curr.battling_partner.begin(); iter != curr.battling_partner.end(); ++iter)
			{
				result = result && outstream.Push(iter->first);
				result = result && outstream.Push(iter->second);
			}
		}
	}

	return result;
}

bool MsgPlatformBattleRoleInfoItemList::Unserialize(TLVUnserializer & instream)
{
	bool result = true;
	unsigned int count = 0;

	result = result && instream.Pop(&role_id);
	result = result && instream.Pop(&count);

	if (count > (unsigned int)PLATFORM_BATTLE_MAX_JOIN_NUM)
	{
		return false;
	}

	role_info_items.clear();
	for (unsigned int i = 0; i < count; ++i)
	{
		PlatformBattleRoleBattleInfoItem curr;
		result = result && instream.Pop(&curr.platform_number);
		result = result && instream.Pop(&curr.rank);
		if (curr.rank != 0)
		{
			size_t pet_count = 0;			
			result = result && instream.Pop(&pet_count);

			if (!result || pet_count > (size_t)PLATFORM_BATTLE_PET_PARTNER_MAX)
			{
				return false;
			}

			for (size_t i  = 0; i < pet_count ; ++i)
			{
				std::pair<unsigned long long, int> p;
				result = result && instream.Pop(&p.first);
				result = result && instream.Pop(&p.second);
				curr.battling_pet.push_back(p);
			}

			size_t partner_count = 0;
			result = result && instream.Pop(&partner_count);
			if (!result || partner_count > (size_t)PLATFORM_BATTLE_PET_PARTNER_MAX)
			{
				return false;
			}

			for (size_t i = 0; i < partner_count; ++i)
			{
				std::pair<int, int> p;
				result = result && instream.Pop(&p.first);
				result = result && instream.Pop(&p.second);
				curr.battling_partner.push_back(p);
			}
		}

		MapAddOrUpdate(role_info_items, curr.platform_number, curr);
	}

	return result;
}

MsgPlatformBattleRoleInfoItem::MsgPlatformBattleRoleInfoItem(const PlatformBattleRoleBattleInfoItem & role_info)
{
	platform_number = role_info.platform_number;
	rank = role_info.rank;

	for (int i = 0; i < PLATFORM_BATTLE_PET_PARTNER_MAX; ++i)
	{
		pet_unique_id[i] = 0;
		pet_pos[i] = 0;

		if (i < (int)role_info.battling_pet.size())
		{
			pet_unique_id[i] = role_info.battling_pet[i].first;
			pet_pos[i] = (unsigned char)role_info.battling_pet[i].second;
		}
	}

	for (int i = 0; i < PLATFORM_BATTLE_PET_PARTNER_MAX; ++i)
	{
		partner_play_item[i].partner_id = 0;
		partner_play_item[i].partner_battle_pos = 0;

		if (i < (int)role_info.battling_partner.size())
		{
			partner_play_item[i].partner_id = (short)role_info.battling_partner[i].first;
			partner_play_item[i].partner_battle_pos = (short)role_info.battling_partner[i].second;
		}
	}
}