#include "battle_def.hpp"
#include "servercommon/msgserialize.hpp"
#include "servercommon/roleattrvermanager/rolebattledatavermgr.h"
#include "servercommon/roleattrvermanager/battlecharacterdatavermgr.h"
#include "servercommon/protobuf/battle_character_data.pb.h"
#include "servercommon/protobuf/role_battle_data.pb.h"
#include <string.h>

bool IsBattleModeMustUseMaxHpMp(int battle_mode)
{
	return (BATTLE_MODE_SINGLE_PVP_PRACTICE == battle_mode ||
		BATTLE_MODE_NORMAL_PRACTICE == battle_mode);
}

short RACE_RELATION_ARRAY[RACE_TYPE_MAX][RACE_TYPE_MAX];

int GetPercentNumByAttrType(int attr_type)
{
	return IsRatioAttrType(attr_type) ? BATTLE_ATTR_PERCENT_NUM : BATTLE_PERCENT_NUM;
}

bool IsRatioAttrType(int attr_type)
{
	switch (attr_type)
	{
	case BATTLE_ATTR_PHYSICAL_DEC_DAMAGE:
	case BATTLE_ATTR_MAGIC_DEC_DAMAGE:
	case BATTLE_ATTR_PHYSICAL_INC_DAMAGE:
	case BATTLE_ATTR_MAGIC_INC_DAMAGE:

	case BATTLE_ATTR_CORRECTED_VALUE_BEGIN:
	case BATTLE_ATTR_HIT:
	case BATTLE_ATTR_DODGE:
	case BATTLE_ATTR_COUNTER_ATTACK:

	case BATTLE_ATTR_CRITICAL_INC_VALUE:
	case BATTLE_ATTR_CRITICAL_DEC_VALUE:

	case BATTLE_ATTR_ANTI_VALUE_BEGIN:
	case BATTLE_ATTR_ANTI_FREEZE:
	case BATTLE_ATTR_ANTI_PETRIFY:
	case BATTLE_ATTR_ANTI_CHAOS:
	case BATTLE_ATTR_ANTI_DRUNK:
	case BATTLE_ATTR_ANTI_SILENCE:
	case BATTLE_ATTR_ANTI_CURSE:
	case BATTLE_ATTR_ANTI_SINK_HOLE:
	case BATTLE_ATTR_ANTI_SEVERE_INJURE:
	case BATTLE_ATTR_ANTI_DEFENCE_CRACKED:
	case BATTLE_ATTR_ANTI_CONTROL_RATE:
	case BATTLE_ATTR_CONTROL_HIT:
		{
			return true;
		}
		break;
	}

	UNSTD_STATIC_CHECK(13 == BATTLE_ATTR_NORMAL_VALUE_END);
	UNSTD_STATIC_CHECK(29 == BATTLE_ATTR_CORRECTED_VALUE_END);
	return false;
}

BattleAttr GetAttrChangeNum(int attr_type, BattleAttr cur_attr, int percent)
{
	if (IsRatioAttrType(attr_type)) return percent; // 如果该属性是按比例计算的，则直接返回percent

	else return (BattleAttr)(1.0 * cur_attr * percent / GetPercentNumByAttrType(attr_type));
}

void SetRaceSuppress(short race1, short race2)
{
	RACE_RELATION_ARRAY[race1][race2] = RACE_RELATION_TYPE_SUPPRESS;
	RACE_RELATION_ARRAY[race2][race1] = RACE_RELATION_TYPE_BEING_SUPPRESSED;
}

void SetRaceHalfSuppress(short race1, short race2)
{
	RACE_RELATION_ARRAY[race1][race2] = RACE_RELATION_TYPE_HALF_SUPPRESS;
	RACE_RELATION_ARRAY[race2][race1] = RACE_RELATION_TYPE_BEING_HALF_SUPPRESSED;
}

bool IsConformDamageEffectType(int damage_type, int damage_effect_type)
{
	if (damage_type < 0 || damage_type >= BATTLE_DAMAGE_TYPE_MAX) return false;
	if (BATTLE_DAMAGE_EFFECT_TYPE_BOTH == damage_effect_type) return true;

	bool is_conform = false;
	switch (damage_type)
	{
		case BATTLE_DAMAGE_TYPE_PHYSIC:
		{
			if (BATTLE_DAMAGE_EFFECT_TYPE_PHYSIC == damage_effect_type)
			{
				is_conform = true;
			}
		}
		break;
		case BATTLE_DAMAGE_TYPE_MAGIC:
		{
			if (BATTLE_DAMAGE_EFFECT_TYPE_MAGIC == damage_effect_type || BATTLE_DAMAGE_EFFECT_TYPE_MAGIC_BOTH == damage_effect_type)
			{
				is_conform = true;
			}
		}
		break;
		case BATTLE_DAMAGE_TYPE_HOLY_MAGIC:
		{
			if (BATTLE_DAMAGE_EFFECT_TYPE_HOLY_MAGIC == damage_effect_type || BATTLE_DAMAGE_EFFECT_TYPE_MAGIC_BOTH == damage_effect_type)
			{
				is_conform = true;
			}
		}
		break;
		default:
		break;
	}

	return is_conform;
}

bool IsConformDamageEffectTypeByReason(int modify_reason, int damage_effect_type)
{
	if (modify_reason < 0 || modify_reason >= BATTLE_MODIFY_HP_REASON_MAX) return false;
	if (BATTLE_DAMAGE_EFFECT_TYPE_BOTH == damage_effect_type) return true;

	bool is_conform = false;
	switch (modify_reason)
	{
		case BATTLE_MODIFY_HP_REASON_PHYSIC_SKILL_INJURE:
		{
			if (BATTLE_DAMAGE_EFFECT_TYPE_PHYSIC == damage_effect_type)
			{
				is_conform = true;
			}
		}
		break;
		case BATTLE_MODIFY_HP_REASON_MAGIC_SKILL_INJURE:
		{
			if (BATTLE_DAMAGE_EFFECT_TYPE_MAGIC == damage_effect_type || BATTLE_DAMAGE_EFFECT_TYPE_MAGIC_BOTH == damage_effect_type)
			{
				is_conform = true;
			}
		}
		break;
		case BATTLE_MODIFY_HP_REASON_HOLY_MAGIC_SKILL_INJURE:
		{
			if (BATTLE_DAMAGE_EFFECT_TYPE_HOLY_MAGIC == damage_effect_type || BATTLE_DAMAGE_EFFECT_TYPE_MAGIC_BOTH == damage_effect_type)
			{
				is_conform = true;
			}
		}
		break;
		default:
		break;
	}

	return is_conform;
}

void RaceRelationInit()
{
	memset(RACE_RELATION_ARRAY, 0, sizeof(RACE_RELATION_ARRAY));
	
	SetRaceSuppress(RACE_TYPE_HUMAN, RACE_TYPE_BEAST);
	SetRaceSuppress(RACE_TYPE_BEAST, RACE_TYPE_NATURE);
	SetRaceSuppress(RACE_TYPE_NATURE, RACE_TYPE_UNDEAD);
	SetRaceSuppress(RACE_TYPE_UNDEAD, RACE_TYPE_HUMAN);
	SetRaceSuppress(RACE_TYPE_INSECT, RACE_TYPE_MYSTERY);
	SetRaceSuppress(RACE_TYPE_MYSTERY, RACE_TYPE_GHOST);
	SetRaceSuppress(RACE_TYPE_GHOST, RACE_TYPE_DRAGON);
	SetRaceSuppress(RACE_TYPE_DRAGON, RACE_TYPE_FLY);
	SetRaceSuppress(RACE_TYPE_FLY, RACE_TYPE_INSECT);
	
	SetRaceHalfSuppress(RACE_TYPE_HUMAN, RACE_TYPE_INSECT);
	SetRaceHalfSuppress(RACE_TYPE_INSECT, RACE_TYPE_NATURE);
	SetRaceHalfSuppress(RACE_TYPE_NATURE, RACE_TYPE_MYSTERY);
	SetRaceHalfSuppress(RACE_TYPE_MYSTERY, RACE_TYPE_DRAGON);
	SetRaceHalfSuppress(RACE_TYPE_DRAGON, RACE_TYPE_BEAST);
	SetRaceHalfSuppress(RACE_TYPE_BEAST, RACE_TYPE_FLY);
	SetRaceHalfSuppress(RACE_TYPE_FLY, RACE_TYPE_UNDEAD);
	SetRaceHalfSuppress(RACE_TYPE_UNDEAD, RACE_TYPE_GHOST);
	SetRaceHalfSuppress(RACE_TYPE_GHOST, RACE_TYPE_HUMAN);

	UNSTD_STATIC_CHECK(RACE_TYPE_MAX == 10);
	//todo 此时尚欠缺上古系克制
}

short ELEMENT_RELATION_ARRAY[ELEMENT_TYPE_MAX][ELEMENT_TYPE_MAX];

void SetElementSuppress(short element1, short element2)
{
	ELEMENT_RELATION_ARRAY[element1][element2] = 1;
	ELEMENT_RELATION_ARRAY[element2][element1] = -1;
}

void ElementRelationInit()
{
	memset(ELEMENT_RELATION_ARRAY, 0, sizeof(ELEMENT_RELATION_ARRAY));

	SetElementSuppress(ELEMENT_TYPE_WATER, ELEMENT_TYPE_FIRE);
	SetElementSuppress(ELEMENT_TYPE_FIRE, ELEMENT_TYPE_WIND);
	SetElementSuppress(ELEMENT_TYPE_WIND, ELEMENT_TYPE_EARTH);
	SetElementSuppress(ELEMENT_TYPE_EARTH, ELEMENT_TYPE_WATER);
}

bool IsSpecialMove(int move_type)
{
	switch (move_type)
	{
	case BATTLE_MOVE_TYPE_INVALID:
	case BATTLE_MOVE_TYPE_NORMAL_ATTACK:
	case BATTLE_MOVE_TYPE_NORMAL_DEFENCE:
	case BATTLE_MOVE_TYPE_RUN_AWAY:
		{
			return false;
		}
		break;

	default:
		{
			return true;
		}
		break;
	}
}

void RoleBattleData::Reset()
{
	is_auto = 0;
	role_stand_row = BATTLE_ROW_TYPE_BACK;
	huanshou_level = 0;
	partner_num = 0;
	pet_num = 0;
	pet_helper_num = 0;
	fight_pet_idx = 0;
	auto_first_move_type = 0;
	auto_first_skill_id = 0; 
	auto_first_skill_level = 0; 
	auto_second_move_type = 0;
	auto_second_skill_id = 0; 
	auto_second_skill_level = 0; 
	weapon_attr_num = 0;
	weapon_sp_num = 0;
	other_sp_num = 0; 
	fabao_num = 0;
	wear_title_id = 0;
	top_level = 0;
	heart_skill_is_vaild = 1;
	heart_skill_count = 0;
	
	appearance.Reset();
	memset(weapon_attr_list, 0, sizeof(weapon_attr_list));
	memset(weapon_sp_list, 0, sizeof(weapon_sp_list));
	memset(other_sp_list, 0, sizeof(other_sp_list));
	memset(fabao_list, 0, sizeof(fabao_list));
	memset(guild_buff_list, 0, sizeof(guild_buff_list));

	role_character_data.Reset();
	for (int i = 0; i < ARRAY_ITEM_COUNT(partner_character_data); ++i)
	{
		partner_character_data[i].Reset();
	}

	for (int i = 0; i < ARRAY_ITEM_COUNT(pet_character_data); ++i)
	{
		pet_character_data[i].Reset();
	}

	for (int i = 0; i < ARRAY_ITEM_COUNT(pet_helper_character_data); ++i)
	{
		pet_helper_character_data[i].Reset();
	}

	for (int i = 0; i < ARRAY_ITEM_COUNT(heart_skill_list); i++)
	{
		heart_skill_list[i].Reset();
	}

	is_robot = 0;

	jieyi_num = 0;
	memset(jieyi_uid_list, 0, sizeof(jieyi_uid_list));

	jieyi_skill_num = 0;
	memset(jieyi_skill_list, 0, sizeof(jieyi_skill_list));

	soaring_skill_id = 0;
	soaring_skill_lv = 0;

	qingyuan_target_uid = 0;
	qingyuan_skill_num = 0;
	memset(qingyuan_skill_list, 0, sizeof(qingyuan_skill_list));
	career_suppress_value = 0;
	change_card_skill_id = 0;
	change_card_seq = 0;
	role_extra_capability = 0;
	huanshou_skill_info.Reset();
}

bool RoleBattleData::Serialize(char* buffer, int buffer_size, int* out_length)
{
	bool ret = true;
	ret = WriteShort(is_auto, buffer, buffer_size, out_length) &&
		WriteShort(role_stand_row, buffer, buffer_size, out_length) &&
		WriteShort(partner_num, buffer, buffer_size, out_length) &&
		WriteShort(pet_num, buffer, buffer_size, out_length) &&
		WriteShort(pet_helper_num, buffer, buffer_size, out_length) &&
		WriteShort(fight_pet_idx, buffer, buffer_size, out_length) &&
		WriteShort(auto_first_move_type, buffer, buffer_size, out_length) &&
		WriteShort(auto_first_skill_id, buffer, buffer_size, out_length) &&
		WriteShort(auto_first_skill_level, buffer, buffer_size, out_length) &&
		WriteShort(auto_second_move_type, buffer, buffer_size, out_length) &&
		WriteShort(auto_second_skill_id, buffer, buffer_size, out_length) &&
		WriteShort(auto_second_skill_level, buffer, buffer_size, out_length) &&
		WriteShort(weapon_attr_num, buffer, buffer_size, out_length) &&
		WriteShort(weapon_sp_num, buffer, buffer_size, out_length) &&
		WriteShort(other_sp_num, buffer, buffer_size, out_length) &&
		WriteShort(fabao_num, buffer, buffer_size, out_length) &&
		WriteInt(top_level, buffer, buffer_size, out_length) &&
		WriteChar(heart_skill_is_vaild, buffer, buffer_size, out_length) &&
		WriteChar(heart_skill_count, buffer, buffer_size, out_length) &&
		WriteInt(jieyi_num, buffer, buffer_size, out_length) &&
		WriteInt(jieyi_skill_num, buffer, buffer_size, out_length) &&
		WriteUShort(soaring_skill_id, buffer, buffer_size, out_length) &&
		WriteShort(soaring_skill_lv, buffer, buffer_size, out_length) &&
		WriteInt(qingyuan_target_uid, buffer, buffer_size, out_length) &&
		WriteInt(qingyuan_skill_num, buffer, buffer_size, out_length) &&
		WriteChar(is_robot, buffer, buffer_size, out_length) &&
		WriteInt(career_suppress_value, buffer, buffer_size, out_length) &&
		WriteInt(change_card_skill_id, buffer, buffer_size, out_length) &&
		WriteInt(change_card_seq, buffer, buffer_size, out_length) &&
		WriteInt(role_extra_capability, buffer, buffer_size, out_length) &&
		WriteShort(hallow_num, buffer, buffer_size, out_length) &&
		WriteShort(fight_hallow_idx, buffer, buffer_size, out_length) &&
		WriteShort(huanshou_level, buffer, buffer_size, out_length);

	if (!ret) return false;
	
	
	ret = WriteStrN((const char*)&role_character_data, sizeof(role_character_data), buffer, buffer_size, out_length);
	if (!ret) return false;

	for (int i = 0; i < partner_num && i < ARRAY_ITEM_COUNT(partner_character_data); ++i)
	{
		ret = WriteStrN((const char*)&partner_character_data[i], sizeof(partner_character_data[i]), buffer, buffer_size, out_length);
		if (!ret) return false;
	}

	for (int i = 0; i < pet_num && i < ARRAY_ITEM_COUNT(pet_character_data); ++i)
	{
		ret = WriteStrN((const char*)&pet_character_data[i], sizeof(pet_character_data[i]), buffer, buffer_size, out_length);
		if (!ret) return false;
	}

	for (int i = 0; i < pet_helper_num && i < ARRAY_ITEM_COUNT(pet_helper_character_data); ++i)
	{
		ret = WriteStrN((const char*)&pet_helper_character_data[i], sizeof(pet_helper_character_data[i]), buffer, buffer_size, out_length);
		if (!ret) return false;
	}
#ifdef _DEBUG
	assert(pet_helper_num <= ROLE_BATTLE_PET_HELPER_NUM);
#endif

	for (int i = 0; i < weapon_attr_num && i < ARRAY_ITEM_COUNT(weapon_attr_list); ++i)
	{
		ret = ret && WriteShort(weapon_attr_list[i].attr_type, buffer, buffer_size, out_length);
		ret = ret && WriteShort(weapon_attr_list[i].attr_value, buffer, buffer_size, out_length);
	}

	for (int i = 0; i < weapon_sp_num && i < ARRAY_ITEM_COUNT(weapon_sp_list); ++i)
	{
		ret = ret && WriteInt(weapon_sp_list[i], buffer, buffer_size, out_length);
	}

	for (int i = 0; i < other_sp_num && i < ARRAY_ITEM_COUNT(other_sp_list); ++i)
	{
		ret = ret && WriteInt(other_sp_list[i], buffer, buffer_size, out_length);
	}

	for (int i = 0; i < fabao_num && i < ARRAY_ITEM_COUNT(fabao_list); ++i)
	{
		ret = ret && WriteStrN((const char*)&fabao_list[i], sizeof(fabao_list[i]), buffer, buffer_size, out_length);
	}

	ret = ret && WriteStrN((const char*)&appearance, sizeof(appearance), buffer, buffer_size, out_length);
	for (int i = 0; i < heart_skill_count && i < ARRAY_ITEM_COUNT(heart_skill_list); ++i)
	{
		ret = ret && WriteStrN((const char*)&heart_skill_list[i], sizeof(heart_skill_list[i]), buffer, buffer_size, out_length);
	}

	for (int i = 0; i < jieyi_num && i < ARRAY_ITEM_COUNT(jieyi_uid_list); ++i)
	{
		ret = ret && WriteInt(jieyi_uid_list[i], buffer, buffer_size, out_length);
	}

	for (int i = 0; i < jieyi_skill_num && i < ARRAY_ITEM_COUNT(jieyi_skill_list); ++i)
	{
		ret = ret && WriteInt(jieyi_skill_list[i], buffer, buffer_size, out_length);
	}

	for (int i = 0; i < qingyuan_skill_num && i < ARRAY_ITEM_COUNT(qingyuan_skill_list); ++i)
	{
		ret = ret && WriteInt(qingyuan_skill_list[i], buffer, buffer_size, out_length);
	}

	for (int i = 0; i < hallow_num 
		&& i < ARRAY_ITEM_COUNT(hallow_list); ++i)
	{
		ret = ret && WriteStrN((const char*)&hallow_list[i], sizeof(hallow_list[i]), buffer, buffer_size, out_length);
	}

	ret = ret && WriteStrN((const char*)&huanshou_skill_info, sizeof(huanshou_skill_info), buffer, buffer_size, out_length);

	return ret;
}

bool RoleBattleData::Deserialize(const char* data, int data_length, int* out_offset)
{
	bool ret = true;

	ret = ReadShort(is_auto, data, data_length, out_offset) &&
		ReadShort(role_stand_row, data, data_length, out_offset) &&
		ReadShort(partner_num, data, data_length, out_offset) &&
		ReadShort(pet_num, data, data_length, out_offset) &&
		ReadShort(pet_helper_num, data, data_length, out_offset) &&
		ReadShort(fight_pet_idx, data, data_length, out_offset) &&
		ReadShort(auto_first_move_type, data, data_length, out_offset) &&
		ReadShort(auto_first_skill_id, data, data_length, out_offset) &&
		ReadShort(auto_first_skill_level, data, data_length, out_offset) &&
		ReadShort(auto_second_move_type, data, data_length, out_offset) &&
		ReadShort(auto_second_skill_id, data, data_length, out_offset) &&
		ReadShort(auto_second_skill_level, data, data_length, out_offset) &&
		ReadShort(weapon_attr_num, data, data_length, out_offset) &&
		ReadShort(weapon_sp_num, data, data_length, out_offset) &&
		ReadShort(other_sp_num, data, data_length, out_offset) &&
		ReadShort(fabao_num, data, data_length, out_offset) &&
		ReadInt(top_level, data, data_length, out_offset) &&
		ReadChar(heart_skill_is_vaild, data, data_length, out_offset) &&
		ReadChar(heart_skill_count, data, data_length, out_offset) &&
		ReadInt(jieyi_num, data, data_length, out_offset) &&
		ReadInt(jieyi_skill_num, data, data_length, out_offset) &&
		ReadUShort(soaring_skill_id, data, data_length, out_offset) &&
		ReadShort(soaring_skill_lv, data, data_length, out_offset) &&
		ReadInt(qingyuan_target_uid, data, data_length, out_offset) &&
		ReadInt(qingyuan_skill_num, data, data_length, out_offset) &&
		ReadChar(is_robot, data, data_length, out_offset) &&
		ReadInt(career_suppress_value, data, data_length, out_offset) &&
		ReadInt(change_card_skill_id, data, data_length, out_offset) &&
		ReadInt(change_card_seq, data, data_length, out_offset) &&
		ReadInt(role_extra_capability, data, data_length, out_offset) &&
		ReadShort(hallow_num, data, data_length, out_offset) &&
		ReadShort(fight_hallow_idx, data, data_length, out_offset) &&
		ReadShort(huanshou_level, data, data_length, out_offset);

	if (!ret) return false;

	partner_num = GetMin(partner_num, (short)ARRAY_ITEM_COUNT(partner_character_data));
	pet_num = GetMin(pet_num, (short)ARRAY_ITEM_COUNT(pet_character_data));
	pet_helper_num = GetMin(pet_helper_num, (short)ARRAY_ITEM_COUNT(pet_helper_character_data));

	ret = ReadStrN((char*)&role_character_data, sizeof(role_character_data), sizeof(role_character_data), data, data_length, out_offset);
	if (!ret) return false;	

	for (int i = 0; i < partner_num && i < ARRAY_ITEM_COUNT(partner_character_data); ++i)
	{
		ret = ReadStrN((char*)&partner_character_data[i], sizeof(partner_character_data[i]), sizeof(partner_character_data[i]), data, data_length, out_offset);
		if (!ret) return false;
	}

	for (int i = 0; i < pet_num && i < ARRAY_ITEM_COUNT(pet_character_data); ++i)
	{
		ret = ReadStrN((char*)&pet_character_data[i], sizeof(pet_character_data[i]), sizeof(pet_character_data[i]), data, data_length, out_offset);

		if (!ret) return false;
	}

	for (int i = 0; i < pet_helper_num && i < ARRAY_ITEM_COUNT(pet_helper_character_data); ++i)
	{
		ret = ReadStrN((char*)&pet_helper_character_data[i], sizeof(pet_helper_character_data[i]), sizeof(pet_helper_character_data[i]), data, data_length, out_offset);

		if (!ret) return false;
	}

	for (int i = 0; i < weapon_attr_num && i < ARRAY_ITEM_COUNT(weapon_attr_list); ++i)
	{
		ret = ret && ReadShort(weapon_attr_list[i].attr_type, data, data_length, out_offset);
		ret = ret && ReadShort(weapon_attr_list[i].attr_value, data, data_length, out_offset);
	}

	for (int i = 0; i < weapon_sp_num && i < ARRAY_ITEM_COUNT(weapon_sp_list); ++i)
	{
		ret = ret && ReadInt(weapon_sp_list[i], data, data_length, out_offset);
	}

	for (int i = 0; i < other_sp_num && i < ARRAY_ITEM_COUNT(other_sp_list); ++i)
	{
		ret = ret && ReadInt(other_sp_list[i], data, data_length, out_offset);
	}

	for (int i = 0; i < fabao_num && i < ARRAY_ITEM_COUNT(fabao_list); ++i)
	{
		ret = ret && ReadStrN((char*)&fabao_list[i], sizeof(fabao_list[i]), sizeof(fabao_list[i]), data, data_length, out_offset);
	}

	ret = ret && ReadStrN((char*)&appearance, sizeof(appearance), sizeof(appearance), data, data_length, out_offset);
	for (int i = 0; i < heart_skill_count && i < ARRAY_ITEM_COUNT(heart_skill_list); ++i)
	{
		ret = ret && ReadStrN((char*)&heart_skill_list[i], sizeof(heart_skill_list[i]), sizeof(heart_skill_list[i]), data, data_length, out_offset);
	}

	for (int i = 0; i < jieyi_num && i < ARRAY_ITEM_COUNT(jieyi_uid_list); ++i)
	{
		ret = ret && ReadInt(jieyi_uid_list[i], data, data_length, out_offset);
	}

	for (int i = 0; i < jieyi_skill_num && i < ARRAY_ITEM_COUNT(jieyi_skill_list); ++i)
	{
		ret = ret && ReadInt(jieyi_skill_list[i], data, data_length, out_offset);
	}

	for (int i = 0; i < qingyuan_skill_num && i < ARRAY_ITEM_COUNT(qingyuan_skill_list); ++i)
	{
		ret = ret && ReadInt(qingyuan_skill_list[i], data, data_length, out_offset);
	}

	for (int i = 0; i < hallow_num 
		&& i < ARRAY_ITEM_COUNT(hallow_list); ++i)
	{
		ret = ret && ReadStrN((char*)&hallow_list[i], sizeof(hallow_list[i]), sizeof(hallow_list[i]), data, data_length, out_offset);
	}

	ret = ret && ReadStrN((char*)&huanshou_skill_info, sizeof(huanshou_skill_info), sizeof(huanshou_skill_info), data, data_length, out_offset);

	return ret;
}

bool RoleBattleData::GetPBData(ARG_OUT PB_RoleBattleData* out_data) const
{
	out_data->set_is_auto(is_auto > 0);
	out_data->set_is_role_stand_front(role_stand_row > 0);

	role_character_data.GetPBData(out_data->mutable_role_character_data());

	out_data->set_huanshou_level(huanshou_level);

	out_data->mutable_partner_character_data()->Clear(); // repeated使用前Clear是个好习惯
	out_data->mutable_partner_character_data()->Reserve(partner_num);
	for (int i = 0; i < partner_num && i < ARRAY_ITEM_COUNT(partner_character_data); ++i)
	{
		PB_PartnerBattleData* pb_pbd = out_data->add_partner_character_data();
		if (!partner_character_data[i].GetPBData(pb_pbd))
		{
			return false;
		}
	}
	
	out_data->set_fight_pet_idx(fight_pet_idx);
	out_data->mutable_pet_character_data()->Clear(); // repeated使用前Clear是个好习惯
	out_data->mutable_pet_character_data()->Reserve(pet_num);
	for (int i = 0; i < pet_num && i < ARRAY_ITEM_COUNT(pet_character_data); ++i)
	{
		PB_PetBattleData* pb_pbd = out_data->add_pet_character_data();
		if (!pet_character_data[i].GetPBData(pb_pbd))
		{
			return false;
		}
	}

	out_data->mutable_pet_helper_data()->Clear(); // repeated使用前Clear是个好习惯
	out_data->mutable_pet_helper_data()->Reserve(pet_helper_num);
	for (int i = 0; i < pet_helper_num && i < ARRAY_ITEM_COUNT(pet_helper_character_data); ++i)
	{
		PB_RoleBattleData_PetPartnerBattleData* pb_pbd = out_data->add_pet_helper_data();
		pb_pbd->set_battle_column(pet_helper_character_data[i].battle_column);
		pb_pbd->set_battle_row(pet_helper_character_data[i].battle_row);
		if (!pet_helper_character_data[i].pet_data.GetPBData(pb_pbd->mutable_pet_data()))
		{
			return false;
		}
	}

	out_data->set_auto_first_move_type(auto_first_move_type);
	out_data->set_auto_first_skill_id(auto_first_skill_id);
	out_data->set_auto_first_skill_level(auto_first_skill_level);
	out_data->set_auto_second_move_type(auto_second_move_type);
	out_data->set_auto_second_skill_id(auto_second_skill_id);
	out_data->set_auto_second_skill_level(auto_second_skill_level);

	out_data->mutable_weapon_attr_list()->Clear(); // repeated使用前Clear是个好习惯
	out_data->mutable_weapon_attr_list()->Reserve(weapon_attr_num);
	for (int i = 0; i < weapon_attr_num && i < ARRAY_ITEM_COUNT(weapon_attr_list); ++i)
	{
		PB_RoleBattleData_AttrInfo* pb_attr_info = out_data->add_weapon_attr_list();
		pb_attr_info->set_attr_type(weapon_attr_list[i].attr_type);
		pb_attr_info->set_attr_value(weapon_attr_list[i].attr_value);
	}

	out_data->mutable_weapon_sp_list()->Clear(); // repeated使用前Clear是个好习惯
	out_data->mutable_weapon_sp_list()->Reserve(weapon_sp_num);
	for (int i = 0; i < weapon_sp_num && i < ARRAY_ITEM_COUNT(weapon_sp_list); ++i)
	{
		out_data->add_weapon_sp_list(weapon_sp_list[i]);
	}

	out_data->mutable_other_sp_list()->Clear(); // repeated使用前Clear是个好习惯
	out_data->mutable_other_sp_list()->Reserve(other_sp_num);
	for (int i = 0; i < other_sp_num; ++i)
	{
		out_data->add_other_sp_list(other_sp_list[i]);
	}

	out_data->mutable_fabao_list()->Clear(); // repeated使用前Clear是个好习惯
	out_data->mutable_fabao_list()->Reserve(fabao_num);
	for (int i = 0; i < fabao_num && i < ARRAY_ITEM_COUNT(fabao_list); ++i)
	{
		PB_RoleBattleData_BattleFabao* pb_fabao = out_data->add_fabao_list();
		pb_fabao->set_fabao_id(fabao_list[i].fabao_id);
		pb_fabao->set_fabao_level(fabao_list[i].fabao_level);
		pb_fabao->set_fabao_skill_type(fabao_list[i].fabao_skill_type);
		pb_fabao->set_fabao_grade(fabao_list[i].fabao_grade);
		pb_fabao->set_fabao_effect_percent_num(fabao_list[i].fabao_effect_percent_num);
		pb_fabao->set_fabao_effect_percent_num_2(fabao_list[i].fabao_effect_percent_num_2);
	}

	out_data->set_appearance(&appearance, sizeof(appearance));

	out_data->mutable_guild_buff_list()->Clear(); // repeated使用前Clear是个好习惯
	out_data->mutable_guild_buff_list()->Reserve(ARRAY_ITEM_COUNT(guild_buff_list));
	for (int i = 0; i < ARRAY_ITEM_COUNT(guild_buff_list); ++i)
	{
		out_data->add_guild_buff_list(guild_buff_list[i]);
	}
	out_data->set_top_level(top_level);

	out_data->set_heart_skill_is_vaild(heart_skill_is_vaild);
	out_data->mutable_heart_skill_list()->Clear();			// repeated使用前Clear是个好习惯
	out_data->mutable_heart_skill_list()->Reserve(heart_skill_count); 
	for (int i = 0; i < heart_skill_count && i < ARRAY_ITEM_COUNT(heart_skill_list); ++i)
	{
		PB_RoleBattleData_BattleHeartSkill * pb_heart_skill = out_data->add_heart_skill_list();
		pb_heart_skill->set_skill_id(heart_skill_list[i].skill_id);
		pb_heart_skill->set_skill_level(heart_skill_list[i].level);
		pb_heart_skill->set_remain_times(heart_skill_list[i].remain_times);
	}

	out_data->mutable_jieyi_uid_list()->Clear();
	out_data->mutable_jieyi_uid_list()->Reserve(jieyi_num);
	for (int i = 0; i < jieyi_num && i < ARRAY_ITEM_COUNT(jieyi_uid_list); ++i)
	{
		out_data->add_jieyi_uid_list(jieyi_uid_list[i]);
	}

	out_data->mutable_jieyi_skill_list()->Clear();
	out_data->mutable_jieyi_skill_list()->Reserve(jieyi_skill_num);
	for (int i = 0; i < jieyi_skill_num && i < ARRAY_ITEM_COUNT(jieyi_skill_list); ++i)
	{
		out_data->add_jieyi_skill_list(jieyi_skill_list[i]);
	}

	out_data->set_soaring_skill_id(soaring_skill_id);
	out_data->set_soaring_skill_lv(soaring_skill_lv);

	out_data->set_qingyuan_target_uid(qingyuan_target_uid);
	out_data->mutable_qingyuan_skill_list()->Clear();
	out_data->mutable_qingyuan_skill_list()->Reserve(qingyuan_skill_num);
	for (int i = 0; i < qingyuan_skill_num && i < ARRAY_ITEM_COUNT(qingyuan_skill_list); ++i)
	{
		out_data->add_qingyuan_skill_list(qingyuan_skill_list[i]);
	}

	out_data->set_is_robot(is_robot > 0);
	out_data->set_career_suppress_value(career_suppress_value);
	out_data->set_change_card_skill_id(change_card_skill_id);
	out_data->set_change_card_seq(change_card_seq);
	out_data->set_role_extra_capability(role_extra_capability);

	out_data->set_fight_hallow_idx(fight_hallow_idx);
	out_data->mutable_hallow_list()->Clear();			// repeated使用前Clear是个好习惯
	out_data->mutable_hallow_list()->Reserve(hallow_num);
	for (int i = 0; i < hallow_num && i < ARRAY_ITEM_COUNT(hallow_list); ++i)
	{
		PB_RoleBattleData_BattleHallowData * pb_hallow = out_data->add_hallow_list();
		pb_hallow->set_hallow_id(hallow_list[i].hallow_id);
		pb_hallow->set_hallow_lv(hallow_list[i].hallow_lv);
		pb_hallow->set_attr_active(hallow_list[i].attr_active);
		pb_hallow->set_attr_active_max(hallow_list[i].attr_active_max);
		pb_hallow->set_attr_damage(hallow_list[i].attr_damage);
		pb_hallow->set_attr_recover(hallow_list[i].attr_recover);
		pb_hallow->set_attr_knock_off(hallow_list[i].attr_knock_off);
		pb_hallow->set_attr_anti_knock_off(hallow_list[i].attr_anti_knock_off);
		pb_hallow->set_attr_intervene(hallow_list[i].attr_intervene);
		pb_hallow->set_attr_speed(hallow_list[i].attr_speed);
		
		pb_hallow->mutable_skill_id_list()->Clear(); // repeated使用前Clear是个好习惯
		pb_hallow->mutable_skill_id_list()->Reserve(hallow_list[i].skill_num);
	
		pb_hallow->mutable_skill_lv_list()->Clear(); // repeated使用前Clear是个好习惯
		pb_hallow->mutable_skill_lv_list()->Reserve(hallow_list[i].skill_num);
	
		for (int k = 0; k < hallow_list[i].skill_num 
			&& k < BATTLE_HALLOW_SKILL_NUM; ++k)
		{
			pb_hallow->add_skill_id_list(hallow_list[i].skill_id_list[k]);
			pb_hallow->add_skill_lv_list(hallow_list[i].skill_lv_list[k]);
		}

		pb_hallow->mutable_gift_list()->Clear(); // repeated使用前Clear是个好习惯
		pb_hallow->mutable_gift_list()->Reserve(hallow_list[i].gift_num);
		for (int k = 0; k < hallow_list[i].gift_num
			&& k < BATTLE_HALLOW_GIFT_NUM; ++k)
		{
			pb_hallow->add_gift_list(hallow_list[i].gift_id_list[k]);
		}
	}

	PB_RoleBattleData_BattleHuanShouSkill* pb_huanshou_skill = out_data->mutable_huanshou_skill();
	pb_huanshou_skill->set_skill_id(huanshou_skill_info.skill_id);
	pb_huanshou_skill->set_skill_level(huanshou_skill_info.skill_level);
	pb_huanshou_skill->set_cd_start(huanshou_skill_info.cd_start);
	pb_huanshou_skill->set_cd_perform(huanshou_skill_info.cd_perform);

	return true;
}

bool RoleBattleData::SetDataFromPB(const PB_RoleBattleData& pb_data)
{
	bool result = true;
	is_auto = pb_data.is_auto() ? 1 : 0;
	role_stand_row = pb_data.is_role_stand_front() ? 1 : 0;
	result = role_character_data.SetDataFromPB(pb_data.role_character_data());
	if (!result) return false;

	huanshou_level = static_cast<short>(pb_data.huanshou_level());
	partner_num = 0;
	for (int i = 0; i < pb_data.partner_character_data_size() && partner_num < ARRAY_ITEM_COUNT(partner_character_data); ++i)
	{
		const PB_PartnerBattleData& pb_partner = pb_data.partner_character_data(i);
		result = partner_character_data[partner_num].SetDataFromPB(pb_partner);
		if (!result) return false;

		++partner_num;
	}

	fight_pet_idx = pb_data.fight_pet_idx();
	pet_num = 0;
	for (int i = 0; i < pb_data.pet_character_data_size() && pet_num < ARRAY_ITEM_COUNT(pet_character_data); ++i)
	{
		const PB_PetBattleData& pb_pet = pb_data.pet_character_data(i);
		result = pet_character_data[pet_num].SetDataFromPB(pb_pet);
		if (!result) return false;

		++pet_num;
	}

	pet_helper_num = 0;
	for (int i = 0; i < pb_data.pet_helper_data_size() && pet_helper_num < ARRAY_ITEM_COUNT(pet_helper_character_data); ++i)
	{
		result = pet_helper_character_data[pet_helper_num].pet_data.SetDataFromPB(pb_data.pet_helper_data(i).pet_data());
		if (!result) return false;

		pet_helper_character_data[pet_helper_num].battle_column = pb_data.pet_helper_data(i).battle_column();
		pet_helper_character_data[pet_helper_num].battle_row = pb_data.pet_helper_data(i).battle_row();
		pet_helper_num += 1;
	}

	auto_first_move_type = pb_data.auto_first_move_type();
	auto_first_skill_id = pb_data.auto_first_skill_id();
	auto_first_skill_level = pb_data.auto_first_skill_level();
	auto_second_move_type = pb_data.auto_second_move_type();
	auto_second_skill_id = pb_data.auto_second_skill_id();
	auto_second_skill_level = pb_data.auto_second_skill_level();

	weapon_attr_num = 0;
	for (int i = 0; i < pb_data.weapon_attr_list_size() && i < ARRAY_ITEM_COUNT(weapon_attr_list); ++i)
	{
		weapon_attr_list[weapon_attr_num].attr_type = pb_data.weapon_attr_list(i).attr_type();
		weapon_attr_list[weapon_attr_num].attr_value = pb_data.weapon_attr_list(i).attr_value();
		++weapon_attr_num;
	}

	weapon_sp_num = 0;
	for (int i = 0; i < pb_data.weapon_sp_list_size() && i < ARRAY_ITEM_COUNT(weapon_sp_list); ++i)
	{
		weapon_sp_list[weapon_sp_num++] = pb_data.weapon_sp_list(i);
	}

	other_sp_num = 0;
	for (int i = 0; i < pb_data.other_sp_list_size() && i < ARRAY_ITEM_COUNT(other_sp_list); ++i)
	{
		other_sp_list[other_sp_num++] = pb_data.other_sp_list(i);
	}

	fabao_num = 0;
	for (int i = 0; i < pb_data.fabao_list_size() && i < ARRAY_ITEM_COUNT(fabao_list); ++i)
	{
		fabao_list[fabao_num].fabao_id = pb_data.fabao_list(i).fabao_id();
		fabao_list[fabao_num].fabao_level = pb_data.fabao_list(i).fabao_level();
		fabao_list[fabao_num].fabao_skill_type = pb_data.fabao_list(i).fabao_skill_type();
		fabao_list[fabao_num].fabao_grade = pb_data.fabao_list(i).fabao_grade();
		fabao_list[fabao_num].fabao_effect_percent_num = pb_data.fabao_list(i).fabao_effect_percent_num();
		fabao_list[fabao_num].fabao_effect_percent_num_2 = pb_data.fabao_list(i).fabao_effect_percent_num_2();
		++fabao_num;
	}

	if (sizeof(appearance) >= pb_data.appearance().size())
	{
		memcpy(&appearance, pb_data.appearance().c_str(), pb_data.appearance().size());
	}
	else
	{
		result = false;
#ifdef _DEBUG
		assert(0);
#endif
	}

	for (int i = 0; i < pb_data.guild_buff_list_size() && i < ARRAY_ITEM_COUNT(guild_buff_list); ++i)
	{
		guild_buff_list[i] = pb_data.guild_buff_list(i);
	}
	
	top_level = pb_data.top_level();

	heart_skill_is_vaild = pb_data.heart_skill_is_vaild();
	heart_skill_count = 0;
	for (int i = 0; i < pb_data.heart_skill_list_size() && i < ARRAY_ITEM_COUNT(heart_skill_list); ++i)
	{
		heart_skill_list[i].skill_id = pb_data.heart_skill_list(i).skill_id();
		heart_skill_list[i].level = pb_data.heart_skill_list(i).skill_level();
		heart_skill_list[i].remain_times = pb_data.heart_skill_list(i).remain_times();
		heart_skill_count++;
	}

	jieyi_num = 0;
	for (int i = 0; i < pb_data.jieyi_uid_list_size() && i < ARRAY_ITEM_COUNT(jieyi_uid_list); ++i)
	{
		jieyi_uid_list[jieyi_num++] = pb_data.jieyi_uid_list(i);
	}

	jieyi_skill_num = 0;
	for (int i = 0; i < pb_data.jieyi_skill_list_size() && i < ARRAY_ITEM_COUNT(jieyi_skill_list); ++i)
	{
		jieyi_skill_list[jieyi_skill_num++] = pb_data.jieyi_skill_list(i);
	}

	soaring_skill_id = pb_data.soaring_skill_id();
	soaring_skill_lv = pb_data.soaring_skill_lv();

	qingyuan_target_uid = pb_data.qingyuan_target_uid();
	qingyuan_skill_num = 0;
	for (int i = 0; i < pb_data.qingyuan_skill_list_size() && i < ARRAY_ITEM_COUNT(qingyuan_skill_list); ++i)
	{
		qingyuan_skill_list[qingyuan_skill_num++] = pb_data.qingyuan_skill_list(i);
	}

	is_robot = pb_data.is_robot() ? 1 : 0;
	career_suppress_value = pb_data.career_suppress_value();
	change_card_skill_id = pb_data.change_card_skill_id();
	change_card_seq = pb_data.change_card_seq();
	role_extra_capability = pb_data.role_extra_capability();


	fight_hallow_idx = pb_data.fight_hallow_idx();
	hallow_num = 0;
	for (int i = 0; i < pb_data.hallow_list_size() 
		&& i < ARRAY_ITEM_COUNT(hallow_list); ++i)
	{
		const PB_RoleBattleData_BattleHallowData& hallow = pb_data.hallow_list(i);
		hallow_list[hallow_num].hallow_id = hallow.hallow_id();
		hallow_list[hallow_num].hallow_lv = hallow.hallow_lv();
		hallow_list[hallow_num].attr_active = hallow.attr_active();
		hallow_list[hallow_num].attr_active_max = hallow.attr_active_max();
		hallow_list[hallow_num].attr_damage = hallow.attr_damage();
		hallow_list[hallow_num].attr_recover = hallow.attr_recover();
		hallow_list[hallow_num].attr_knock_off = hallow.attr_knock_off();
		hallow_list[hallow_num].attr_anti_knock_off = hallow.attr_anti_knock_off();
		hallow_list[hallow_num].attr_intervene = hallow.attr_intervene();
		hallow_list[hallow_num].attr_speed = hallow.attr_speed();

		hallow_list[hallow_num].skill_num = 0;
		int skill_num = 0;
		for (int k = 0; k < hallow.skill_id_list_size() 
			&& k < hallow.skill_lv_list_size()
			&& k < BATTLE_HALLOW_SKILL_NUM; ++k)
		{
			hallow_list[hallow_num].skill_id_list[skill_num] = hallow.skill_id_list(k);
			hallow_list[hallow_num].skill_lv_list[skill_num] = hallow.skill_lv_list(k);
			skill_num += 1;
		}
		hallow_list[hallow_num].skill_num = skill_num;

		hallow_list[hallow_num].gift_num = 0;
		int gift_num = 0;
		for (int k = 0; k < hallow.gift_list_size()
			&& k < BATTLE_HALLOW_GIFT_NUM; ++k)
		{
			hallow_list[hallow_num].gift_id_list[gift_num] = hallow.gift_list(k);
			gift_num += 1;
		}
		hallow_list[hallow_num].gift_num = gift_num;

		hallow_num += 1;
	}

	const PB_RoleBattleData_BattleHuanShouSkill & pb_huanshou_skill = pb_data.huanshou_skill();
	huanshou_skill_info.skill_id = pb_huanshou_skill.skill_id();
	huanshou_skill_info.skill_level = pb_huanshou_skill.skill_level();
	huanshou_skill_info.cd_start = pb_huanshou_skill.cd_start();
	huanshou_skill_info.cd_perform = pb_huanshou_skill.cd_perform();

	return result;
}

const char* RoleBattleData::GetRoleCharacterAttrStr() const
{
	static char buffer[1024];
	memset(buffer, 0, sizeof(buffer));

	const BattleCharacterData& bcd = role_character_data;
	SNPRINTF(buffer, sizeof(buffer),
		"maxhp[%d] maxmp[%d] atk[%d] def[%d] memtal[%d] recover[%d] agi[%d]",
		bcd.attr_list[BATTLE_ATTR_MAX_HP],
		bcd.attr_list[BATTLE_ATTR_MAX_MP],
		bcd.attr_list[BATTLE_ATTR_ATTACK],
		bcd.attr_list[BATTLE_ATTR_DEFENSE],
		bcd.attr_list[BATTLE_ATTR_MENTAL],
		bcd.attr_list[BATTLE_ATTR_RECOVERY],
		bcd.attr_list[BATTLE_ATTR_AGILITY]);

	return buffer;
}

bool BattleCharacterData::GetPBData(ARG_OUT PB_BattleCharacterData* out_data) const
{
	out_data->set_character_id(character_id);
	out_data->set_id_param(id_param);
	out_data->set_character_type(character_type);
	out_data->set_is_use_cur_hp_mp(is_use_cur_hp_mp > 0);
	out_data->set_level(level);
	out_data->set_name(name);
	out_data->set_quality(quality);
	out_data->set_profession(profession);
	out_data->set_race(race);
	out_data->set_weapon_type(weapon_type);
	out_data->set_weapon_id(weapon_id);

	out_data->mutable_attr_list()->Clear(); // repeated使用前Clear是个好习惯
	out_data->mutable_attr_list()->Reserve(BATTLE_ATTR_MAX);
	for (int i = 0; i < BATTLE_ATTR_MAX; ++i)
	{
		out_data->add_attr_list(attr_list[i]);
	}
	out_data->set_param(param);
	out_data->set_is_long_range_unit(is_long_range_unit);

	out_data->mutable_skill_list()->Clear(); // repeated使用前Clear是个好习惯
	out_data->mutable_skill_list()->Reserve(skill_num);
	for (int i = 0; i < skill_num && i < MAX_BATTLE_SKILL_NUM; ++i)
	{
		PB_BattleCharacterData_SkillItem* pb_skill_item = out_data->add_skill_list();
		pb_skill_item->set_skill_id(skill_list[i].skill_id);
		pb_skill_item->set_skill_level(skill_list[i].skill_level);
	}

	out_data->set_capability(capability);
	out_data->set_fly_flag(fly_flag);
	out_data->set_wear_element_type(wear_element_type);

	return true;
}

bool BattleCharacterData::SetDataFromPB(const PB_BattleCharacterData& pb_data)
{
	character_id = pb_data.character_id();
	id_param = pb_data.id_param();
	character_type = static_cast<char>(pb_data.character_type());
	is_use_cur_hp_mp = pb_data.is_use_cur_hp_mp() ? 1 : 0;
	level = static_cast<short>(pb_data.level());
	F_STRNCPY(name, pb_data.name().c_str(), sizeof(name));
	quality = pb_data.quality();
	profession = pb_data.profession();
	race = static_cast<char>(pb_data.race());
	weapon_type = static_cast<char>(pb_data.weapon_type());
	weapon_id = static_cast<char>(pb_data.weapon_id());
	memset(attr_list, 0, sizeof(attr_list));
	for (int i = 0; i < pb_data.attr_list_size() && i < ARRAY_ITEM_COUNT(attr_list); ++i)
	{
		attr_list[i] = pb_data.attr_list(i);
	}
	param = static_cast<char>(pb_data.param());
	is_long_range_unit = pb_data.is_long_range_unit() ? 1 : 0;

	skill_num = 0;
	for (int i = 0; i < pb_data.skill_list_size() && skill_num < ARRAY_ITEM_COUNT(skill_list); ++i)
	{
		skill_list[skill_num].skill_id = pb_data.skill_list(i).skill_id();
		skill_list[skill_num].skill_level = pb_data.skill_list(i).skill_level();
		++skill_num;
	}

	capability = pb_data.capability();
	fly_flag = pb_data.fly_flag();
	wear_element_type = pb_data.wear_element_type();

	return true;
}

bool PetBattleData::GetPBData(ARG_OUT PB_PetBattleData* out_data) const
{
	if (!battle_data.GetPBData(out_data->mutable_battle_data()))
	{
		return false;
	}

	out_data->mutable_passive_list()->Clear(); // repeated使用前Clear是个好习惯
	out_data->mutable_passive_list()->Reserve(BATTLE_PET_GIFT_NUM);
	for (int k = 0; k < BATTLE_PET_GIFT_NUM; ++k)
	{
		out_data->add_passive_list(passive_list[k]);
	}

	out_data->mutable_passive_level_list()->Clear(); // repeated使用前Clear是个好习惯
	out_data->mutable_passive_level_list()->Reserve(BATTLE_PET_GIFT_NUM);
	for (int k = 0; k < BATTLE_PET_GIFT_NUM; ++k)
	{
		out_data->add_passive_level_list(passive_level_list[k]);
	}

	out_data->set_auto_move_type(auto_move_type);
	out_data->set_auto_skill_id(auto_skill_id);
	out_data->set_auto_skill_level(auto_skill_level);
	out_data->set_exclusive_passive_skill_obsolete(exclusive_passive_skill_OBSOLETE);
	out_data->set_skin_id(skin_id);

	out_data->mutable_other_sp_list()->Clear();
	out_data->mutable_other_sp_list()->Reserve(BATTLE_OTHER_EFFECT_NUM);
	for (int k = 0; k < BATTLE_OTHER_EFFECT_NUM; ++k)
	{
		out_data->add_other_sp_list(other_sp_list[k]);
	}

	out_data->set_unique_id(unique_id);
	out_data->set_lianyao_pet_id(lianyao_pet_id);
	out_data->set_lianyao_pet_str_lv(lianyao_pet_str_lv);
	out_data->set_pet_exclusive_skill_disable_flag(pet_exclusive_skill_disable_flag);
	out_data->set_lian_yao_exclusive_skill_disable_flag(lian_yao_exclusive_skill_disable_flag);
	return true;
}

bool PetBattleData::SetDataFromPB(const PB_PetBattleData& pb_data)
{
	if (!battle_data.SetDataFromPB(pb_data.battle_data()))
	{
		return false;
	}

	for (int k = 0; k < pb_data.passive_list_size() && k < ARRAY_ITEM_COUNT(passive_list); ++k)
	{
		passive_list[k] = pb_data.passive_list(k);
	}

	for (int k = 0; k < pb_data.passive_level_list_size() && k < ARRAY_ITEM_COUNT(passive_level_list); ++k)
	{
		passive_level_list[k] = pb_data.passive_level_list(k);
	}

	auto_move_type = pb_data.auto_move_type();
	auto_skill_id = pb_data.auto_skill_id();
	auto_skill_level = pb_data.auto_skill_level();
	exclusive_passive_skill_OBSOLETE = pb_data.exclusive_passive_skill_obsolete();
	skin_id = pb_data.skin_id();

	for (int k = 0; k < pb_data.other_sp_list_size() && k < ARRAY_ITEM_COUNT(other_sp_list); ++k)
	{
		other_sp_list[k] = pb_data.other_sp_list(k);
	}

	unique_id = pb_data.unique_id();
	lianyao_pet_id = pb_data.lianyao_pet_id();
	lianyao_pet_str_lv = pb_data.lianyao_pet_str_lv();
	pet_exclusive_skill_disable_flag = pb_data.pet_exclusive_skill_disable_flag();
	lian_yao_exclusive_skill_disable_flag = pb_data.lian_yao_exclusive_skill_disable_flag();
	return true;
}

bool PartnerBattleData::GetPBData(ARG_OUT PB_PartnerBattleData* out_data) const
{
	if (!battle_data.GetPBData(out_data->mutable_battle_data()))
	{
		return false;
	}

	out_data->set_skin_id(skin_id);
	out_data->set_battle_row(battle_row);
	out_data->set_battle_column(battle_column);

	out_data->mutable_other_sp_list()->Clear();
	out_data->mutable_other_sp_list()->Reserve(BATTLE_PARTNER_OTHER_EFFECT_NUM);
	for (int k = 0; k < other_sp_num && k < BATTLE_PARTNER_OTHER_EFFECT_NUM; ++k)
	{
		out_data->add_other_sp_list(other_sp_list[k]);
	}

	out_data->set_str_lv(str_lv);

	return true;
}

bool PartnerBattleData::SetDataFromPB(const PB_PartnerBattleData& pb_data)
{
	if (!battle_data.SetDataFromPB(pb_data.battle_data()))
	{
		return false;
	}

	skin_id = pb_data.skin_id();
	battle_row = pb_data.battle_row();
	battle_column = pb_data.battle_column();

	other_sp_num = 0;
	for (int k = 0; k < pb_data.other_sp_list_size() && k < ARRAY_ITEM_COUNT(other_sp_list); ++k)
	{
		other_sp_list[other_sp_num++] = pb_data.other_sp_list(k);
	}

	str_lv = pb_data.str_lv();

	return true;
}
