#include "battleprotocol.h"
#include "servercommon/msgserialize.hpp"

namespace battlegameprotocol 
{
	bool WriteBegin(int msg_type, char* buffer, int max_len, int* out_len)
	{
		// 初始化
		if (*out_len + 4 >= max_len) return false;

		return WriteInt(msg_type, buffer, max_len, out_len);
	}

	bool ReadBegin(int& msg_type, const char* buffer, int max_len, int* out_len)
	{
		// 初始化
		if (*out_len + 4 >= max_len) return false;

		return ReadInt(msg_type, buffer, max_len, out_len);
	}

	bool GameBattleStartSinglePVEReq::Serialize(char* buffer, int buffer_size, int* out_length)
	{
		bool ret = true;

		ret = ret && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
		ret = ret && WriteShort(bg_netid.gateway_index, buffer, buffer_size, out_length);
		ret = ret && WriteUShort(bg_netid.net_id, buffer, buffer_size, out_length);
		ret = ret && WriteInt(uid, buffer, buffer_size, out_length);
		ret = ret && WriteShort(battle_mode, buffer, buffer_size, out_length);
		ret = ret && WriteShort(animation_speed_mode, buffer, buffer_size, out_length);
		ret = ret && WriteInt(monster_group_id, buffer, buffer_size, out_length);
		ret = ret && WriteInt(battle_id, buffer, buffer_size, out_length);
		ret = ret && WriteStrN((const char*)&business_data, sizeof(business_data), buffer, buffer_size, out_length);
		ret = ret && role_battle_data.Serialize(buffer, buffer_size, out_length);

		return ret;
	}

	bool GameBattleStartSinglePVEReq::Deserialize(const char* data, int data_length, int* out_offset)
	{
		bool ret = true;

		ret = ret && ReadBegin(header.msg_type, data, data_length, out_offset);
		ret = ret && ReadShort(bg_netid.gateway_index, data, data_length, out_offset);
		ret = ret && ReadUShort(bg_netid.net_id, data, data_length, out_offset);
		ret = ret && ReadInt(uid, data, data_length, out_offset);
		ret = ret && ReadShort(battle_mode, data, data_length, out_offset);
		ret = ret && ReadShort(animation_speed_mode, data, data_length, out_offset);
		ret = ret && ReadInt(monster_group_id, data, data_length, out_offset);
		ret = ret && ReadInt(battle_id, data, data_length, out_offset);
		ret = ret && ReadStrN((char*)&business_data, sizeof(business_data), sizeof(business_data), data, data_length, out_offset);
		ret = ret && role_battle_data.Deserialize(data, data_length, out_offset);
		
		return ret;
	}

	bool GameBattleStartSinglePVPReq::Serialize(char* buffer, int buffer_size, int* out_length)
	{
		bool ret = true;

		ret = ret && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
		ret = ret && WriteShort(attacker_bgnetid.gateway_index, buffer, buffer_size, out_length);
		ret = ret && WriteUShort(attacker_bgnetid.net_id, buffer, buffer_size, out_length);
		ret = ret && WriteInt(attacker_uid, buffer, buffer_size, out_length);
		ret = ret && WriteShort(defender_bgnetid.gateway_index, buffer, buffer_size, out_length);
		ret = ret && WriteUShort(defender_bgnetid.net_id, buffer, buffer_size, out_length);
		ret = ret && WriteInt(defender_uid, buffer, buffer_size, out_length);
		ret = ret && WriteShort(battle_mode, buffer, buffer_size, out_length);
		ret = ret && WriteShort(animation_speed_mode, buffer, buffer_size, out_length);
		ret = ret && WriteInt(battle_id, buffer, buffer_size, out_length);
		ret = ret && WriteStrN((const char*)&business_data, sizeof(business_data), buffer, buffer_size, out_length);
		ret = ret && attacker_battle_data.Serialize(buffer, buffer_size, out_length);
		ret = ret && defender_battle_data.Serialize(buffer, buffer_size, out_length);

		return ret;
	}

	bool GameBattleStartSinglePVPReq::Deserialize(const char* data, int data_length, int* out_offset)
	{
		bool ret = true;

		ret = ret && ReadBegin(header.msg_type, data, data_length, out_offset);
		ret = ret && ReadShort(attacker_bgnetid.gateway_index, data, data_length, out_offset);
		ret = ret && ReadUShort(attacker_bgnetid.net_id, data, data_length, out_offset);
		ret = ret && ReadInt(attacker_uid, data, data_length, out_offset);
		ret = ret && ReadShort(defender_bgnetid.gateway_index, data, data_length, out_offset);
		ret = ret && ReadUShort(defender_bgnetid.net_id, data, data_length, out_offset);
		ret = ret && ReadInt(defender_uid, data, data_length, out_offset);
		ret = ret && ReadShort(battle_mode, data, data_length, out_offset);
		ret = ret && ReadShort(animation_speed_mode, data, data_length, out_offset);
		ret = ret && ReadInt(battle_id, data, data_length, out_offset);
		ret = ret && ReadStrN((char*)&business_data, sizeof(business_data), sizeof(business_data), data, data_length, out_offset);
		ret = ret && attacker_battle_data.Deserialize(data, data_length, out_offset);
		ret = ret && defender_battle_data.Deserialize(data, data_length, out_offset);

		return ret;
	}

	bool GameBattleStartTeamPVEReq::Serialize(char* buffer, int buffer_size, int* out_length)
	{
		bool ret = true;

		ret = ret && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
		ret = ret && WriteInt(team_id, buffer, buffer_size, out_length);
		ret = ret && WriteShort(battle_mode, buffer, buffer_size, out_length);
		ret = ret && WriteShort(animation_speed_mode, buffer, buffer_size, out_length);
		ret = ret && WriteInt(monster_group_id, buffer, buffer_size, out_length);
		ret = ret && WriteInt(battle_id, buffer, buffer_size, out_length);
		ret = ret && WriteStrN((const char*)&business_data, sizeof(business_data), buffer, buffer_size, out_length);
		ret = ret && WriteShort(leader_idx, buffer, buffer_size, out_length);
		ret = ret && WriteShort(role_num, buffer, buffer_size, out_length);
		for (int i = 0; i < role_num; ++i)
		{
			ret = ret && WriteShort(member_battle_data[i].bg_netid.gateway_index, buffer, buffer_size, out_length);
			ret = ret && WriteUShort(member_battle_data[i].bg_netid.net_id, buffer, buffer_size, out_length);
			ret = ret && WriteInt(member_battle_data[i].uid, buffer, buffer_size, out_length);
			ret = ret && member_battle_data[i].role_battle_data.Serialize(buffer, buffer_size, out_length);
		}
		for (int i = 0; i < ARRAY_ITEM_COUNT(pos_id_list); ++i)
		{
			ret = ret && WriteInt(pos_id_list[i], buffer, buffer_size, out_length);
		}

		return ret;
	}

	bool GameBattleStartTeamPVEReq::Deserialize(const char* data, int data_length, int* out_offset)
	{
		bool ret = true;

		ret = ret && ReadBegin(header.msg_type, data, data_length, out_offset);
		ret = ret && ReadInt(team_id, data, data_length, out_offset);
		ret = ret && ReadShort(battle_mode, data, data_length, out_offset);
		ret = ret && ReadShort(animation_speed_mode, data, data_length, out_offset);
		ret = ret && ReadInt(monster_group_id, data, data_length, out_offset);
		ret = ret && ReadInt(battle_id, data, data_length, out_offset);
		ret = ret && ReadStrN((char*)&business_data, sizeof(business_data), sizeof(business_data), data, data_length, out_offset);
		ret = ret && ReadShort(leader_idx, data, data_length, out_offset);
		ret = ret && ReadShort(role_num, data, data_length, out_offset);
		for (int i = 0; i < role_num; ++i)
		{
			ret = ret && ReadShort(member_battle_data[i].bg_netid.gateway_index, data, data_length, out_offset);
			ret = ret && ReadUShort(member_battle_data[i].bg_netid.net_id, data, data_length, out_offset);
			ret = ret && ReadInt(member_battle_data[i].uid, data, data_length, out_offset);
			ret = ret && member_battle_data[i].role_battle_data.Deserialize(data, data_length, out_offset);
		}
		for (int i = 0; i < ARRAY_ITEM_COUNT(pos_id_list); ++i)
		{
			ret = ret && ReadInt(pos_id_list[i], data, data_length, out_offset);
		}

		return ret;
	}

	bool GameBattleStartMultiPVPReq::Serialize(char* buffer, int buffer_size, int* out_length)
	{
		bool ret = true;

		ret = ret && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
		ret = ret && WriteShort(battle_mode, buffer, buffer_size, out_length);
		ret = ret && WriteShort(animation_speed_mode, buffer, buffer_size, out_length);
		ret = ret && WriteInt(battle_id, buffer, buffer_size, out_length);
		ret = ret && WriteStrN((const char*)&business_data, sizeof(business_data), buffer, buffer_size, out_length);
		ret = ret && WriteShort(attacker_num, buffer, buffer_size, out_length);
		ret = ret && WriteShort(defender_num, buffer, buffer_size, out_length);
		for (int i = 0; i < attacker_num; ++i)
		{
			ret = ret && WriteShort(attacker_data_list[i].bg_netid.gateway_index, buffer, buffer_size, out_length);
			ret = ret && WriteUShort(attacker_data_list[i].bg_netid.net_id, buffer, buffer_size, out_length);
			ret = ret && WriteInt(attacker_data_list[i].uid, buffer, buffer_size, out_length);
			ret = ret && attacker_data_list[i].role_battle_data.Serialize(buffer, buffer_size, out_length);
		}
		for (int i = 0; i < defender_num; ++i)
		{
			ret = ret && WriteShort(defender_data_list[i].bg_netid.gateway_index, buffer, buffer_size, out_length);
			ret = ret && WriteUShort(defender_data_list[i].bg_netid.net_id, buffer, buffer_size, out_length);
			ret = ret && WriteInt(defender_data_list[i].uid, buffer, buffer_size, out_length);
			ret = ret && defender_data_list[i].role_battle_data.Serialize(buffer, buffer_size, out_length);
		}

		ret = ret && WriteInt(observer_num, buffer, buffer_size, out_length);
		for (int i = 0; i < observer_num; ++i)
		{
			ret = ret && WriteShort(observer[i].bg_netid.gateway_index, buffer, buffer_size, out_length);
			ret = ret && WriteUShort(observer[i].bg_netid.net_id, buffer, buffer_size, out_length);
			ret = ret && WriteShort(observer[i].side, buffer, buffer_size, out_length);
			ret = ret && WriteInt(observer[i].uid, buffer, buffer_size, out_length);
		}

		for (int i = 0; i < SIDE_MAX_ROLE_NUM; ++i)
		{
			ret = ret && WRITE_INT(attacker_pos_id_list[i]);
			ret = ret && WRITE_INT(defender_pos_id_list[i]);
		}

		return ret;
	}

	bool GameBattleStartMultiPVPReq::Deserialize(const char* data, int data_length, int* out_offset)
	{
		bool ret = true;

		ret = ret && ReadBegin(header.msg_type, data, data_length, out_offset);
		ret = ret && ReadShort(battle_mode, data, data_length, out_offset);
		ret = ret && ReadShort(animation_speed_mode, data, data_length, out_offset);
		ret = ret && ReadInt(battle_id, data, data_length, out_offset);
		ret = ret && ReadStrN((char*)&business_data, sizeof(business_data), sizeof(business_data), data, data_length, out_offset);
		ret = ret && ReadShort(attacker_num, data, data_length, out_offset);
		ret = ret && ReadShort(defender_num, data, data_length, out_offset);
		for (int i = 0; i < attacker_num; ++i)
		{
			ret = ret && ReadShort(attacker_data_list[i].bg_netid.gateway_index, data, data_length, out_offset);
			ret = ret && ReadUShort(attacker_data_list[i].bg_netid.net_id, data, data_length, out_offset);
			ret = ret && ReadInt(attacker_data_list[i].uid, data, data_length, out_offset);
			ret = ret && attacker_data_list[i].role_battle_data.Deserialize(data, data_length, out_offset);
		}
		for (int i = 0; i < defender_num; ++i)
		{
			ret = ret && ReadShort(defender_data_list[i].bg_netid.gateway_index, data, data_length, out_offset);
			ret = ret && ReadUShort(defender_data_list[i].bg_netid.net_id, data, data_length, out_offset);
			ret = ret && ReadInt(defender_data_list[i].uid, data, data_length, out_offset);
			ret = ret && defender_data_list[i].role_battle_data.Deserialize(data, data_length, out_offset);
		}

		ret = ret && ReadInt(observer_num, data, data_length, out_offset);
		for (int i = 0; i < observer_num; ++i)
		{
			ret = ret && ReadShort(observer[i].bg_netid.gateway_index, data, data_length, out_offset);
			ret = ret && ReadUShort(observer[i].bg_netid.net_id, data, data_length, out_offset);
			ret = ret && ReadShort(observer[i].side, data, data_length, out_offset);
			ret = ret && ReadInt(observer[i].uid, data, data_length, out_offset);
		}

		for (int i = 0; i < SIDE_MAX_ROLE_NUM; ++i)
		{
			ret = ret && READ_INT(attacker_pos_id_list[i]);
			ret = ret && READ_INT(defender_pos_id_list[i]);
		}

		return ret;
	}

	bool GameBattleStartSingleFightShadowReq::Serialize(char* buffer, int buffer_size, int* out_length)
	{
		bool ret = true;

		ret = ret && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
		ret = ret && WriteShort(attacker_bgnetid.gateway_index, buffer, buffer_size, out_length);
		ret = ret && WriteUShort(attacker_bgnetid.net_id, buffer, buffer_size, out_length);
		ret = ret && WriteInt(attacker_uid, buffer, buffer_size, out_length);
		ret = ret && WriteShort(battle_mode, buffer, buffer_size, out_length);
		ret = ret && WriteShort(animation_speed_mode, buffer, buffer_size, out_length);
		ret = ret && WriteInt(battle_id, buffer, buffer_size, out_length);
		ret = ret && WriteStrN((const char*)&business_data, sizeof(business_data), buffer, buffer_size, out_length);
		ret = ret && attacker_battle_data.Serialize(buffer, buffer_size, out_length);
		ret = ret && defender_battle_data.Serialize(buffer, buffer_size, out_length);

		return ret;
	}

	bool GameBattleStartSingleFightShadowReq::Deserialize(const char* data, int data_length, int* out_offset)
	{
		bool ret = true;

		ret = ret && ReadBegin(header.msg_type, data, data_length, out_offset);
		ret = ret && ReadShort(attacker_bgnetid.gateway_index, data, data_length, out_offset);
		ret = ret && ReadUShort(attacker_bgnetid.net_id, data, data_length, out_offset);
		ret = ret && ReadInt(attacker_uid, data, data_length, out_offset);
		ret = ret && ReadShort(battle_mode, data, data_length, out_offset);
		ret = ret && ReadShort(animation_speed_mode, data, data_length, out_offset);
		ret = ret && ReadInt(battle_id, data, data_length, out_offset);
		ret = ret && ReadStrN((char*)&business_data, sizeof(business_data), sizeof(business_data), data, data_length, out_offset);
		ret = ret && attacker_battle_data.Deserialize(data, data_length, out_offset);
		ret = ret && defender_battle_data.Deserialize(data, data_length, out_offset);

		return ret;
	}

	bool GameBattleGuildHonorFightSendBackup::Serialize(char * buffer, int buffer_size, int * out_length)
	{
		bool ret = true;

		ret = ret && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
		ret = ret && WriteInt(battle_id, buffer, buffer_size, out_length);
		ret = ret && WriteShort(attacker_num, buffer, buffer_size, out_length);
		ret = ret && WriteShort(defender_num, buffer, buffer_size, out_length);
		for (int i = 0; i < attacker_num && i < ARRAY_LENGTH(attacker_data_list); ++i)
		{
			 MemberData & member_data = attacker_data_list[i];
			 ret = ret && WriteShort(member_data.bg_netid.gateway_index, buffer, buffer_size, out_length);
			 ret = ret && WriteUShort(member_data.bg_netid.net_id, buffer, buffer_size, out_length);
			 ret = ret && WriteInt(member_data.uid, buffer, buffer_size, out_length);
			ret = ret && member_data.role_battle_data.Serialize(buffer, buffer_size, out_length);
		}

		for (int i = 0; i < defender_num && i < ARRAY_LENGTH(defender_data_list); ++i)
		{
			MemberData & member_data = defender_data_list[i];
			ret = ret && WriteShort(member_data.bg_netid.gateway_index, buffer, buffer_size, out_length);
			ret = ret && WriteUShort(member_data.bg_netid.net_id, buffer, buffer_size, out_length);
			ret = ret && WriteInt(member_data.uid, buffer, buffer_size, out_length);
			ret = ret && member_data.role_battle_data.Serialize(buffer, buffer_size, out_length);
		}
		
		return ret;
	}

	bool GameBattleGuildHonorFightSendBackup::Deserialize(const char * data, int data_length, int * out_offset)
	{
		bool ret = true;

		ret = ret && ReadBegin(header.msg_type, data, data_length, out_offset);
		ret = ret && ReadInt(battle_id, data, data_length, out_offset);
		ret = ret && ReadShort(attacker_num, data, data_length, out_offset);
		ret = ret && ReadShort(defender_num, data, data_length, out_offset);
		for (int i = 0; i < attacker_num && i < ARRAY_LENGTH(attacker_data_list); ++i)
		{
			MemberData & member_data = attacker_data_list[i];
			ret = ret && ReadShort(member_data.bg_netid.gateway_index, data, data_length, out_offset);
			ret = ret && ReadUShort(member_data.bg_netid.net_id, data, data_length, out_offset);
			ret = ret && ReadInt(member_data.uid, data, data_length, out_offset);
			ret = ret && member_data.role_battle_data.Deserialize(data, data_length, out_offset);
		}

		for (int i = 0; i < defender_num && i < ARRAY_LENGTH(defender_data_list); ++i)
		{
			MemberData & member_data = defender_data_list[i];
			ret = ret && ReadShort(member_data.bg_netid.gateway_index, data, data_length, out_offset);
			ret = ret && ReadUShort(member_data.bg_netid.net_id, data, data_length, out_offset);
			ret = ret && ReadInt(member_data.uid, data, data_length, out_offset);
			ret = ret && member_data.role_battle_data.Deserialize(data, data_length, out_offset);
		}

		return ret;
	}

	bool GameBattleStartPlatformFightReq::Serialize(char* buffer, int buffer_size, int* out_length)
	{
		bool ret = true;

		ret = ret && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
		ret = ret && WriteInt(attacker_uid, buffer, buffer_size, out_length);
		ret = ret && WriteShort(bg_netid.gateway_index, buffer, buffer_size, out_length);
		ret = ret && WriteUShort(bg_netid.net_id, buffer, buffer_size, out_length);	
		ret = ret && WriteInt(battle_id, buffer, buffer_size, out_length);
		ret = ret && WriteInt(animation_speed_mode, buffer, buffer_size, out_length);

		// 攻方数据
		ret = ret && WriteInt(atk_pet_num, buffer, buffer_size, out_length);
		for (int i = 0; i < atk_pet_num; ++i)
		{
			ret = ret && WriteStrN((const char*)&atk_pet_list[i], sizeof(atk_pet_list[i]), buffer, buffer_size, out_length);
			ret = ret && WriteInt(atk_pet_pos_list[i], buffer, buffer_size, out_length);
			if (!ret) return false;
		}
		ret = ret && WriteInt(atk_partner_num, buffer, buffer_size, out_length);
		for (int i = 0; i < atk_partner_num; ++i)
		{
			ret = ret && WriteStrN((const char*)&atk_partner_list[i], sizeof(atk_partner_list[i]), buffer, buffer_size, out_length);
			ret = ret && WriteInt(atk_partner_pos_list[i], buffer, buffer_size, out_length);
			if (!ret) return false;
		}

		// 守方数据
		ret = ret && WriteInt(defender_uid, buffer, buffer_size, out_length);
		ret = ret && WriteInt(def_pet_num, buffer, buffer_size, out_length);
		for (int i = 0; i < def_pet_num; ++i)
		{
			ret = ret && WriteStrN((const char*)&def_pet_list[i], sizeof(def_pet_list[i]), buffer, buffer_size, out_length);
			ret = ret && WriteInt(def_pet_pos_list[i], buffer, buffer_size, out_length);
			if (!ret) return false;
		}
		ret = ret && WriteInt(def_partner_num, buffer, buffer_size, out_length);
		for (int i = 0; i < def_partner_num; ++i)
		{
			ret = ret && WriteStrN((const char*)&def_partner_list[i], sizeof(def_partner_list[i]), buffer, buffer_size, out_length);
			ret = ret && WriteInt(def_partner_pos_list[i], buffer, buffer_size, out_length);
			if (!ret) return false;
		}

		// 业务数据
		ret = WriteStrN((const char*)&business_data, sizeof(business_data), buffer, buffer_size, out_length);

		return ret;
	}

	bool GameBattleStartPlatformFightReq::Deserialize(const char* data, int data_length, int* out_offset)
	{
		bool ret = true;

		ret = ret && ReadBegin(header.msg_type, data, data_length, out_offset);	
		ret = ret && ReadInt(attacker_uid, data, data_length, out_offset);
		ret = ret && ReadShort(bg_netid.gateway_index, data, data_length, out_offset);
		ret = ret && ReadUShort(bg_netid.net_id, data, data_length, out_offset);
		ret = ret && ReadInt(battle_id, data, data_length, out_offset);
		ret = ret && ReadInt(animation_speed_mode, data, data_length, out_offset);

		// 攻方数据
		ret = ret && ReadInt(atk_pet_num, data, data_length, out_offset);
		for (int i = 0; i < atk_pet_num; ++i)
		{
			ret = ret && ReadStrN((char*)&atk_pet_list[i], sizeof(atk_pet_list[i]), sizeof(atk_pet_list[i]), data, data_length, out_offset);
			ret = ret && ReadInt(atk_pet_pos_list[i], data, data_length, out_offset);
			if (!ret) return false;
		}
		ret = ret && ReadInt(atk_partner_num, data, data_length, out_offset);
		for (int i = 0; i < atk_partner_num; ++i)
		{
			ret = ret && ReadStrN((char*)&atk_partner_list[i], sizeof(atk_partner_list[i]), sizeof(atk_partner_list[i]), data, data_length, out_offset);
			ret = ret && ReadInt(atk_partner_pos_list[i], data, data_length, out_offset);
			if (!ret) return false;
		}

		// 守方数据
		ret = ret && ReadInt(defender_uid, data, data_length, out_offset);
		ret = ret && ReadInt(def_pet_num, data, data_length, out_offset);
		for (int i = 0; i < def_pet_num; ++i)
		{
			ret = ret && ReadStrN((char*)&def_pet_list[i], sizeof(def_pet_list[i]), sizeof(def_pet_list[i]), data, data_length, out_offset);
			ret = ret && ReadInt(def_pet_pos_list[i], data, data_length, out_offset);
			if (!ret) return false;
		}
		ret = ret && ReadInt(def_partner_num, data, data_length, out_offset);
		for (int i = 0; i < def_partner_num; ++i)
		{
			ret = ret && ReadStrN((char*)&def_partner_list[i], sizeof(def_partner_list[i]), sizeof(def_partner_list[i]), data, data_length, out_offset);
			ret = ret && ReadInt(def_partner_pos_list[i], data, data_length, out_offset);
			if (!ret) return false;
		}

		// 业务数据
		ret = ReadStrN((char*)&business_data, sizeof(business_data), sizeof(business_data), data, data_length, out_offset);

		return ret;
	}

	bool GameBattleStartMultiFightShadowReq::Serialize(char* buffer, int buffer_size, int* out_length)
	{
		bool ret = true;

		ret = ret && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
		ret = ret && WriteShort(battle_mode, buffer, buffer_size, out_length);
		ret = ret && WriteShort(animation_speed_mode, buffer, buffer_size, out_length);
		ret = ret && WriteInt(battle_id, buffer, buffer_size, out_length);

		ret = ret && WriteStrN((const char*)&business_data, sizeof(business_data), buffer, buffer_size, out_length);
		
		ret = ret && WriteShort(attacker_num, buffer, buffer_size, out_length);
		ret = ret && WriteShort(defender_num, buffer, buffer_size, out_length);

		for (int i = 0; i < attacker_num; ++i)
		{
			ret = ret && WriteShort(attacker_list[i].bgnetid.gateway_index, buffer, buffer_size, out_length);
			ret = ret && WriteUShort(attacker_list[i].bgnetid.net_id, buffer, buffer_size, out_length);
			ret = ret && WriteInt(attacker_list[i].uid, buffer, buffer_size, out_length);
		}

		for (int i = 0; i < attacker_num; ++i)
		{
			ret = ret && attacker_data_list[i].Serialize(buffer, buffer_size, out_length);
		}

		for (int i = 0; i < defender_num; ++i)
		{
			ret = ret && defender_data_list[i].Serialize(buffer, buffer_size, out_length);
		}

		return true;
	}

	bool GameBattleStartMultiFightShadowReq::Deserialize(const char* data, int data_length, int* out_offset)
	{
		bool ret = true;

		ret = ret && ReadBegin(header.msg_type, data, data_length, out_offset);
		ret = ret && ReadShort(battle_mode, data, data_length, out_offset);
		ret = ret && ReadShort(animation_speed_mode, data, data_length, out_offset);
		ret = ret && ReadInt(battle_id, data, data_length, out_offset);

		ret = ReadStrN((char*)&business_data, sizeof(business_data), sizeof(business_data), data, data_length, out_offset);
		
		ret = ret && ReadShort(attacker_num, data, data_length, out_offset);
		ret = ret && ReadShort(defender_num, data, data_length, out_offset);

		for (int i = 0; i < attacker_num; ++i)
		{
			ret = ret && ReadShort(attacker_list[i].bgnetid.gateway_index, data, data_length, out_offset);
			ret = ret && ReadUShort(attacker_list[i].bgnetid.net_id, data, data_length, out_offset);
			ret = ret && ReadInt(attacker_list[i].uid, data, data_length, out_offset);
		}

		for (int i = 0; i < attacker_num; ++i)
		{
			ret = ret && attacker_data_list[i].Deserialize(data, data_length, out_offset);
		}

		for (int i = 0; i < defender_num; ++i)
		{
			ret = ret && defender_data_list[i].Deserialize(data, data_length, out_offset);
		}

		return true;
	}

	bool GameBattleStartSpecialReq::Serialize(char* buffer, int buffer_size, int* out_length)
	{
		bool ret = true;

		ret = ret && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
		ret = ret && WriteShort(battle_mode, buffer, buffer_size, out_length);
		ret = ret && WriteInt(battle_id, buffer, buffer_size, out_length);
		ret = ret && WriteInt(attacker_count, buffer, buffer_size, out_length);

		for (int i = 0; i < SIDE_MAX_ROLE_NUM && i < attacker_count;i++)
		{
			ret = ret && WriteShort(attacker_data[i].bg_netid.gateway_index, buffer, buffer_size, out_length);
			ret = ret && WriteUShort(attacker_data[i].bg_netid.net_id, buffer, buffer_size, out_length);
			ret = ret && WriteInt(attacker_data[i].uid, buffer, buffer_size, out_length);
			ret = ret && WriteStrN((const char*)&attacker_data[i].role_name, sizeof(GameName), buffer, buffer_size, out_length);
			ret = ret && WriteShort(attacker_data[i].m_avatar_type, buffer, buffer_size, out_length);
			ret = ret && WriteShort(attacker_data[i].reserve_sh, buffer, buffer_size, out_length);
			ret = ret && WriteLL(attacker_data[i].m_avatar_timestamp, buffer, buffer_size, out_length);
		}
		ret = ret && WriteInt(defender_count, buffer, buffer_size, out_length);
		for (int i = 0; i < SIDE_MAX_ROLE_NUM && i < defender_count; i++)
		{
			ret = ret && WriteShort(defender_data[i].bg_netid.gateway_index, buffer, buffer_size, out_length);
			ret = ret && WriteUShort(defender_data[i].bg_netid.net_id, buffer, buffer_size, out_length);
			ret = ret && WriteInt(defender_data[i].uid, buffer, buffer_size, out_length);
			ret = ret && WriteStrN((const char*)&defender_data[i].role_name, sizeof(GameName), buffer, buffer_size, out_length);
			ret = ret && WriteShort(defender_data[i].m_avatar_type, buffer, buffer_size, out_length);
			ret = ret && WriteShort(defender_data[i].reserve_sh, buffer, buffer_size, out_length);
			ret = ret && WriteLL(defender_data[i].m_avatar_timestamp, buffer, buffer_size, out_length);
		}
		return ret;
	}

	bool GameBattleStartSpecialReq::Deserialize(const char* data, int data_length, int* out_offset)
	{
		bool ret = true;

		ret = ret && ReadBegin(header.msg_type, data, data_length, out_offset);
		ret = ret && ReadShort(battle_mode, data, data_length, out_offset);
		ret = ret && ReadInt(battle_id, data, data_length, out_offset);
		ret = ret && ReadInt(attacker_count, data, data_length, out_offset);
		for (int i = 0; i < SIDE_MAX_ROLE_NUM && i < attacker_count; i++)
		{
			ret = ret && ReadShort(attacker_data[i].bg_netid.gateway_index, data, data_length, out_offset);
			ret = ret && ReadUShort(attacker_data[i].bg_netid.net_id, data, data_length, out_offset);
			ret = ret && ReadInt(attacker_data[i].uid, data, data_length, out_offset);
			ret = ret && ReadStrN((char*)&attacker_data[i].role_name, sizeof(GameName), sizeof(GameName), data, data_length, out_offset);
			ret = ret && ReadShort(attacker_data[i].m_avatar_type, data, data_length, out_offset);
			ret = ret && ReadShort(attacker_data[i].reserve_sh, data, data_length, out_offset);
			ret = ret && ReadLL(attacker_data[i].m_avatar_timestamp, data, data_length, out_offset);
		}
		ret = ret && ReadInt(defender_count, data, data_length, out_offset);
		for (int i = 0; i < SIDE_MAX_ROLE_NUM && i < defender_count; i++)
		{
			ret = ret && ReadShort(defender_data[i].bg_netid.gateway_index, data, data_length, out_offset);
			ret = ret && ReadUShort(defender_data[i].bg_netid.net_id, data, data_length, out_offset);
			ret = ret && ReadInt(defender_data[i].uid, data, data_length, out_offset);
			ret = ret && ReadStrN((char*)&defender_data[i].role_name, sizeof(GameName), sizeof(GameName), data, data_length, out_offset);
			ret = ret && ReadShort(defender_data[i].m_avatar_type, data, data_length, out_offset);
			ret = ret && ReadShort(defender_data[i].reserve_sh, data, data_length, out_offset);
			ret = ret && ReadLL(defender_data[i].m_avatar_timestamp, data, data_length, out_offset);
		}
		return ret;
	}

	bool BattleGameRoleRunAway::Serialize(char* buffer, int buffer_size, int* out_length)
	{
		return WRITE_BEGIN(header.msg_type)
			&& WRITE_INT(battle_id)
			&& WRITE_INT(battle_mode)
			&& WRITE_INT(uid)
			&& WRITE_SHORT(reserve_sh)
			&& WRITE_SHORT(is_leave_team)
			&& WRITE_STRN((const char*)&business_data, sizeof(business_data))
			&& WRITE_SHORT(special_business_data.battle_mode)
			&& WRITE_SHORT(special_business_data.data_type)
			&& WRITE_INT(special_business_data.data_length)
			&& WRITE_STRN(special_business_data.data, special_business_data.data_length);
	}

	bool BattleGameRoleRunAway::Deserialize(const char* data, int data_length, int* out_offset)
	{
		return READ_BEGIN(header.msg_type)
			&& READ_INT(battle_id)
			&& READ_INT(battle_mode)
			&& READ_INT(uid)
			&& READ_SHORT(reserve_sh)
			&& READ_SHORT(is_leave_team)
			&& READ_STRN((char*)&business_data, sizeof(business_data), sizeof(business_data))
			&& READ_SHORT(special_business_data.battle_mode)
			&& READ_SHORT(special_business_data.data_type)
			&& READ_INT(special_business_data.data_length)
			&& READ_STRN(special_business_data.data, special_business_data.data_length, sizeof(special_business_data.data));
	}

	bool BattleGameFightResultAck::Serialize(char* buffer, int buffer_size, int* out_length)
	{
		bool ret = WRITE_BEGIN(header.msg_type)
			&& WRITE_INT(battle_id)
			&& WRITE_INT(battle_mode)
			&& WRITE_INT(monster_group_id)
			&& WRITE_INT(battle_result);
		if (!ret)
		{
			return false;
		}

		ret = ret && WRITE_INT(attacker_list.role_count);
		for (int i = 0; i < attacker_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
		{
			ret = ret && WRITE_STRN((const char*)&attacker_list.role_list[i], sizeof(attacker_list.role_list[i]));
		}

		ret = ret && WRITE_INT(defender_list.role_count);
		for (int i = 0; i < defender_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
		{
			ret = ret && WRITE_STRN((const char*)&defender_list.role_list[i], sizeof(defender_list.role_list[i]));
		}
		if (!ret)
		{
			return false;
		}

		ret = WRITE_STRN((const char*)&business_data, sizeof(business_data))
			&& WRITE_STRN((const char*)&statistic_data, sizeof(statistic_data))
			&& WRITE_SHORT(special_business_data.battle_mode)
			&& WRITE_SHORT(special_business_data.data_type)
			&& WRITE_INT(special_business_data.data_length)
			&& WRITE_STRN(special_business_data.data, special_business_data.data_length);

		return ret;
	}

	bool BattleGameFightResultAck::Deserialize(const char* data, int data_length, int* out_offset)
	{
		bool ret = READ_BEGIN(header.msg_type)
			&& READ_INT(battle_id)
			&& READ_INT(battle_mode)
			&& READ_INT(monster_group_id)
			&& READ_INT(battle_result);
		if (!ret)
		{
			return false;
		}

		ret = ret && READ_INT(attacker_list.role_count);
		for (int i = 0; i < attacker_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
		{
			ret = ret && READ_STRN((char*)&attacker_list.role_list[i], sizeof(attacker_list.role_list[i]), sizeof(attacker_list.role_list[i]));
		}

		ret = ret && READ_INT(defender_list.role_count);
		for (int i = 0; i < defender_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
		{
			ret = ret && READ_STRN((char*)&defender_list.role_list[i], sizeof(defender_list.role_list[i]), sizeof(defender_list.role_list[i]));
		}
		if (!ret)
		{
			return false;
		}

		ret = READ_STRN((char*)&business_data, sizeof(business_data), sizeof(business_data))
			&& READ_STRN((char*)&statistic_data, sizeof(statistic_data), sizeof(statistic_data))
			&& READ_SHORT(special_business_data.battle_mode)
			&& READ_SHORT(special_business_data.data_type)
			&& READ_INT(special_business_data.data_length)
			&& READ_STRN(special_business_data.data, special_business_data.data_length, sizeof(special_business_data.data));

		return ret;
	}

	bool BattleGameStartFightAck::Serialize(char* buffer, int buffer_size, int* out_length)
	{
		bool ret = WRITE_BEGIN(header.msg_type)
			&& WRITE_INT(battle_id)
			&& WRITE_INT(battle_mode)
			&& WRITE_SHORT(is_pvp)
			&& WRITE_SHORT(is_team_up)
			&& WRITE_SHORT(animation_speed_type)
			&& WRITE_SHORT(reserve_sh);
		if (!ret)
		{
			return false;
		}

		ret = ret && WRITE_INT(role_count);
		for (int i = 0; i < role_count && i < BATTLE_MAX_ROLE_NUM; ++i)
		{
			ret = ret && WRITE_STRN((const char*)&role_list[i], sizeof(role_list[i]));
		}
		if (!ret)
		{
			return false;
		}

		ret = WRITE_SHORT(special_business_data.battle_mode)
			&& WRITE_SHORT(special_business_data.data_type)
			&& WRITE_INT(special_business_data.data_length)
			&& WRITE_STRN(special_business_data.data, special_business_data.data_length);

		return ret;
	}

	bool BattleGameStartFightAck::Deserialize(const char* data, int data_length, int* out_offset)
	{
		bool ret = READ_BEGIN(header.msg_type)
			&& READ_INT(battle_id)
			&& READ_INT(battle_mode)
			&& READ_SHORT(is_pvp)
			&& READ_SHORT(is_team_up)
			&& READ_SHORT(animation_speed_type)
			&& READ_SHORT(reserve_sh);
		if (!ret)
		{
			return false;
		}

		ret = ret && READ_INT(role_count);
		for (int i = 0; i < role_count && i < BATTLE_MAX_ROLE_NUM; ++i)
		{
			ret = ret && READ_STRN((char*)&role_list[i], sizeof(role_list[i]), sizeof(role_list[i]));
		}
		if (!ret)
		{
			return false;
		}

		ret = READ_SHORT(special_business_data.battle_mode)
			&& READ_SHORT(special_business_data.data_type)
			&& READ_INT(special_business_data.data_length)
			&& READ_STRN(special_business_data.data, special_business_data.data_length, sizeof(special_business_data.data));

		return ret;
	}

	bool BattleGameForceFinishFight::Serialize(char* buffer, int buffer_size, int* out_length)
	{
		bool ret = WRITE_BEGIN(header.msg_type)
			&& WRITE_INT(battle_id)
			&& WRITE_INT(battle_mode)
			&& WRITE_INT(monster_group_id)
			&& WRITE_INT(result_type);
		if (!ret)
		{
			return false;
		}

		ret = ret && WRITE_INT(attacker_list.role_count);
		for (int i = 0; i < attacker_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
		{
			ret = ret && WRITE_STRN((const char*)&attacker_list.role_list[i], sizeof(attacker_list.role_list[i]));
		}

		ret = ret && WRITE_INT(defender_list.role_count);
		for (int i = 0; i < defender_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
		{
			ret = ret && WRITE_STRN((const char*)&defender_list.role_list[i], sizeof(defender_list.role_list[i]));
		}
		if (!ret)
		{
			return false;
		}

		ret = WRITE_STRN((const char*)&business_data, sizeof(business_data))
			&& WRITE_STRN((const char*)&statistic_data, sizeof(statistic_data))
			&& WRITE_SHORT(special_business_data.battle_mode)
			&& WRITE_SHORT(special_business_data.data_type)
			&& WRITE_INT(special_business_data.data_length)
			&& WRITE_STRN(special_business_data.data, special_business_data.data_length);

		return ret;
	}

	bool BattleGameForceFinishFight::Deserialize(const char* data, int data_length, int* out_offset)
	{
		bool ret = READ_BEGIN(header.msg_type)
			&& READ_INT(battle_id)
			&& READ_INT(battle_mode)
			&& READ_INT(monster_group_id)
			&& READ_INT(result_type);
		if (!ret)
		{
			return false;
		}

		ret = ret && READ_INT(attacker_list.role_count);
		for (int i = 0; i < attacker_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
		{
			ret = ret && READ_STRN((char*)&attacker_list.role_list[i], sizeof(attacker_list.role_list[i]), sizeof(attacker_list.role_list[i]));
		}

		ret = ret && READ_INT(defender_list.role_count);
		for (int i = 0; i < defender_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
		{
			ret = ret && READ_STRN((char*)&defender_list.role_list[i], sizeof(defender_list.role_list[i]), sizeof(defender_list.role_list[i]));
		}
		if (!ret)
		{
			return false;
		}

		ret = READ_STRN((char*)&business_data, sizeof(business_data), sizeof(business_data))
			&& READ_STRN((char*)&statistic_data, sizeof(statistic_data), sizeof(statistic_data))
			&& READ_SHORT(special_business_data.battle_mode)
			&& READ_SHORT(special_business_data.data_type)
			&& READ_INT(special_business_data.data_length)
			&& READ_STRN(special_business_data.data, special_business_data.data_length, sizeof(special_business_data.data));

		return ret;
	}

	bool BattleGameSpecialFightResultAck::Serialize(char* buffer, int buffer_size, int* out_length)
	{
		bool ret = true;

		ret = ret && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
		ret = ret && WriteShort(battle_mode, buffer, buffer_size, out_length);
		ret = ret && WriteInt(battle_id, buffer, buffer_size, out_length);
		ret = ret && WriteInt(battle_result, buffer, buffer_size, out_length);
		ret = ret && WriteInt(attacker_num, buffer, buffer_size, out_length);
		ret = ret && WriteInt(defender_num, buffer, buffer_size, out_length);

		for (int idx = 0; idx < attacker_num && idx < SIDE_MAX_ROLE_NUM; idx++)
		{
			ret = ret && WriteShort(attacker_data[idx].bg_netid.gateway_index, buffer, buffer_size, out_length);
			ret = ret && WriteUShort(attacker_data[idx].bg_netid.net_id, buffer, buffer_size, out_length);
			ret = ret && WriteInt(attacker_data[idx].uid, buffer, buffer_size, out_length);
			ret = ret && WriteStrN((const char*)&attacker_data[idx].role_name, sizeof(GameName), buffer, buffer_size, out_length);
			ret = ret && WriteShort(attacker_data[idx].m_avatar_type, buffer, buffer_size, out_length);
			ret = ret && WriteShort(attacker_data[idx].reserve_sh, buffer, buffer_size, out_length);
			ret = ret && WriteLL(attacker_data[idx].m_avatar_timestamp, buffer, buffer_size, out_length);
		}
		
		for (int idx = 0; idx < defender_num && idx < SIDE_MAX_ROLE_NUM; idx++)
		{
			ret = ret && WriteShort(defender_data[idx].bg_netid.gateway_index, buffer, buffer_size, out_length);
			ret = ret && WriteUShort(defender_data[idx].bg_netid.net_id, buffer, buffer_size, out_length);
			ret = ret && WriteInt(defender_data[idx].uid, buffer, buffer_size, out_length);
			ret = ret && WriteStrN((const char*)&defender_data[idx].role_name, sizeof(GameName), buffer, buffer_size, out_length);
			ret = ret && WriteShort(defender_data[idx].m_avatar_type, buffer, buffer_size, out_length);
			ret = ret && WriteShort(defender_data[idx].reserve_sh, buffer, buffer_size, out_length);
			ret = ret && WriteLL(defender_data[idx].m_avatar_timestamp, buffer, buffer_size, out_length);
		}

		return ret;
	}

	bool BattleGameSpecialFightResultAck::Deserialize(const char* data, int data_length, int* out_offset)
	{
		bool ret = true;

		ret = ret && ReadBegin(header.msg_type, data, data_length, out_offset);
		ret = ret && ReadShort(battle_mode, data, data_length, out_offset);
		ret = ret && ReadInt(battle_id, data, data_length, out_offset);
		ret = ret && ReadInt(battle_result, data, data_length, out_offset);
		ret = ret && ReadInt(attacker_num, data, data_length, out_offset);
		ret = ret && ReadInt(defender_num, data, data_length, out_offset);

		for (int idx = 0; idx < SIDE_MAX_ROLE_NUM && idx < attacker_num; idx++)
		{
			ret = ret && ReadShort(attacker_data[idx].bg_netid.gateway_index, data, data_length, out_offset);
			ret = ret && ReadUShort(attacker_data[idx].bg_netid.net_id, data, data_length, out_offset);
			ret = ret && ReadInt(attacker_data[idx].uid, data, data_length, out_offset);
			ret = ret && ReadStrN((char*)&attacker_data[idx].role_name, sizeof(GameName), sizeof(GameName), data, data_length, out_offset);
			ret = ret && ReadShort(attacker_data[idx].m_avatar_type, data, data_length, out_offset);
			ret = ret && ReadShort(attacker_data[idx].reserve_sh, data, data_length, out_offset);
			ret = ret && ReadLL(attacker_data[idx].m_avatar_timestamp, data, data_length, out_offset);
		}
		
		for (int idx = 0; idx < SIDE_MAX_ROLE_NUM && idx < defender_num; idx++)
		{
			ret = ret && ReadShort(defender_data[idx].bg_netid.gateway_index, data, data_length, out_offset);
			ret = ret && ReadUShort(defender_data[idx].bg_netid.net_id, data, data_length, out_offset);
			ret = ret && ReadInt(defender_data[idx].uid, data, data_length, out_offset);
			ret = ret && ReadStrN((char*)&defender_data[idx].role_name, sizeof(GameName), sizeof(GameName), data, data_length, out_offset);
			ret = ret && ReadShort(defender_data[idx].m_avatar_type, data, data_length, out_offset);
			ret = ret && ReadShort(defender_data[idx].reserve_sh, data, data_length, out_offset);
			ret = ret && ReadLL(defender_data[idx].m_avatar_timestamp, data, data_length, out_offset);
		}

		return ret;
	}

	bool GameBattleStartPartnerPVEReq::Serialize(char* buffer, int buffer_size, int* out_length)
	{
		bool ret = true;

		ret = ret && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
		ret = ret && WriteInt(attacker_uid, buffer, buffer_size, out_length);
		ret = ret && WriteShort(bg_netid.gateway_index, buffer, buffer_size, out_length);
		ret = ret && WriteUShort(bg_netid.net_id, buffer, buffer_size, out_length);
		ret = ret && WriteInt(battle_id, buffer, buffer_size, out_length);
		ret = ret && WriteShort(battle_mode, buffer, buffer_size, out_length);
		ret = ret && WriteShort(animation_speed_mode, buffer, buffer_size, out_length);
		ret = ret && WriteInt(monster_group_id, buffer, buffer_size, out_length);

		ret = ret && WriteInt(atk_partner_num, buffer, buffer_size, out_length);
		for (int i = 0; i < atk_partner_num; ++i)
		{
			ret = ret && WriteStrN((const char*)&atk_partner_list[i], sizeof(atk_partner_list[i]), buffer, buffer_size, out_length);
			ret = ret && WriteInt(atk_partner_pos_list[i], buffer, buffer_size, out_length);
			if (!ret) return false;
		}

		ret = WriteStrN((const char*)&business_data, sizeof(business_data), buffer, buffer_size, out_length);

		return ret;
	}

	bool GameBattleStartPartnerPVEReq::Deserialize(const char* data, int data_length, int* out_offset)
	{
		bool ret = true;

		ret = ret && ReadBegin(header.msg_type, data, data_length, out_offset);
		ret = ret && ReadInt(attacker_uid, data, data_length, out_offset);
		ret = ret && ReadShort(bg_netid.gateway_index, data, data_length, out_offset);
		ret = ret && ReadUShort(bg_netid.net_id, data, data_length, out_offset);
		ret = ret && ReadInt(battle_id, data, data_length, out_offset);
		ret = ret && ReadShort(battle_mode, data, data_length, out_offset);
		ret = ret && ReadShort(animation_speed_mode, data, data_length, out_offset);
		ret = ret && ReadInt(monster_group_id, data, data_length, out_offset);

		ret = ret && ReadInt(atk_partner_num, data, data_length, out_offset);
		for (int i = 0; i < atk_partner_num; ++i)
		{
			ret = ret && ReadStrN((char*)&atk_partner_list[i], sizeof(atk_partner_list[i]), sizeof(atk_partner_list[i]), data, data_length, out_offset);
			ret = ret && ReadInt(atk_partner_pos_list[i], data, data_length, out_offset);
			if (!ret) return false;
		}

		ret = ReadStrN((char*)&business_data, sizeof(business_data), sizeof(business_data), data, data_length, out_offset);

		return ret;
	}

}
