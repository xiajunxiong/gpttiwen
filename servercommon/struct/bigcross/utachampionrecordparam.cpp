#include "utachampionrecordparam.hpp"

#include "servercommon/struct/structcommon.h"
#include "servercommon/protobufmanager.hpp"
#include "servercommon/protobuf/utachampionrecord_data.pb.h"


bool UTAChampionRecordUserInfo::GetPBData(ARG_OUT PB_UTAChampionRecordUserInfoData * out_data) const
{
	out_data->set_uid(uid);
	out_data->set_role_name(role_name);
	out_data->set_plat_id(plat_id);
	out_data->set_server_id(server_id);
	out_data->set_level(level);
	out_data->set_fly_flag(fly_flag);
	out_data->set_top_level(top_level);
	out_data->set_guild_name(guild_name);
	out_data->set_guild_banner(guild_banner);
	out_data->set_title_id(title_id);
	out_data->set_facescore_id(facescore_id);
	out_data->set_prof_base(prof_base);
	out_data->set_advance_times(advance_times);
	out_data->set_avatar_type(avatar_type);
	out_data->set_color(color);
	out_data->set_weapon_id(weapon_id);
	out_data->set_sub_weapon_id(sub_weapon_id);
	out_data->set_headshot_id(headshot_id);
	out_data->set_special_appearance_type(special_appearance_type);
	out_data->set_special_appearance_param(special_appearance_param);

	out_data->mutable_wear_surface()->Clear();
	out_data->mutable_wear_surface()->Reserve(SURFACE_TYPE_MAX);
	for (int i = 0; i < SURFACE_TYPE_MAX && i < ARRAY_ITEM_COUNT(wear_surface); ++i)
	{
		out_data->add_wear_surface(wear_surface[i]);
	}

	out_data->set_smart_mounts_index(smart_mounts_index);
	out_data->set_is_use_advanced_flag(is_use_advanced_flag);

	return true;
}

bool UTAChampionRecordUserInfo::SetDataFromPB(const PB_UTAChampionRecordUserInfoData& pb_data)
{
	uid = pb_data.uid();
	F_STRNCPY(role_name, pb_data.role_name().c_str(), sizeof(role_name));
	plat_id = pb_data.plat_id();
	server_id = pb_data.server_id();
	level = pb_data.level();
	fly_flag = pb_data.fly_flag();
	top_level = pb_data.top_level();
	F_STRNCPY(guild_name, pb_data.guild_name().c_str(), sizeof(guild_name));
	F_STRNCPY(guild_banner, pb_data.guild_banner().c_str(), sizeof(guild_banner));
	title_id = pb_data.title_id();
	facescore_id = pb_data.facescore_id();
	prof_base = pb_data.prof_base();
	advance_times = pb_data.advance_times();
	avatar_type = pb_data.avatar_type();
	color = pb_data.color();
	weapon_id = pb_data.weapon_id();
	sub_weapon_id = pb_data.sub_weapon_id();
	headshot_id = pb_data.headshot_id();
	special_appearance_type = pb_data.special_appearance_type();
	special_appearance_param = pb_data.special_appearance_param();

	memset(wear_surface, 0, sizeof(wear_surface));
	assert(ARRAY_ITEM_COUNT(wear_surface) == pb_data.wear_surface_size());
	for (int i = 0; i < pb_data.wear_surface_size() && i < ARRAY_ITEM_COUNT(wear_surface); ++i)
	{
		wear_surface[i] = pb_data.wear_surface(i);
	}

	smart_mounts_index = pb_data.smart_mounts_index();
	is_use_advanced_flag = pb_data.is_use_advanced_flag();

	return true;
}

void UTAChampionRecordUserInfo::Reset()
{
	uid = 0;
	plat_id = 0;
	server_id = -1;
	level = 0;
	fly_flag = 0;
	top_level = 0;
	memset(guild_name, 0, sizeof(guild_name));
	memset(guild_banner, 0, sizeof(guild_banner));
	title_id = 0;
	facescore_id = -1;

	prof_base = 0;
	advance_times = 0;
	avatar_type = 0;
	color = 0;
	weapon_id = 0;
	sub_weapon_id = 0;
	headshot_id = 0;
	special_appearance_type = 0;
	special_appearance_param = 0;

	memset(wear_surface, 0, sizeof(wear_surface));

	smart_mounts_index = -1;
	is_use_advanced_flag = 0;

	reserve_sh = 0;		
}

bool UTAChampionRecordUserTeamInfo::GetPBData(ARG_OUT PB_UTAChampionRecordUserTeamInfoData * out_data) const
{
	bool ret = true;
	out_data->mutable_user_info()->Clear();
	out_data->mutable_user_info()->Reserve(MAX_TEAM_MEMBER_NUM);
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM && i < ARRAY_ITEM_COUNT(user_info); ++i)
	{
		out_data->add_user_info();
		ret = ret && user_info[i].GetPBData(out_data->mutable_user_info(i));
	}

	return ret;
}

bool UTAChampionRecordUserTeamInfo::SetDataFromPB(const PB_UTAChampionRecordUserTeamInfoData& pb_data)
{
	bool ret = true;

	memset(user_info, 0, sizeof(user_info));
	for (int i = 0; i < pb_data.user_info_size() && i < ARRAY_ITEM_COUNT(user_info); ++i)
	{
		ret = ret && user_info[i].SetDataFromPB(pb_data.user_info(i));
	}

	return ret;
}

void UTAChampionRecordUserTeamInfo::Reset()
{
	memset(user_info, 0, sizeof(user_info));
}

bool UTAChampionRecordParam::GetPBData(ARG_OUT PB_UTAChampionRecordData * out_data) const
{
	bool ret = true;
	out_data->set_change_state(change_state);
	out_data->set_rank(rank);
	out_data->set_team_name(team_name);
	ret = ret && user_team_info.GetPBData(out_data->mutable_user_team_info());

	return ret;
}

bool UTAChampionRecordParam::SetDataFromPB(const PB_UTAChampionRecordData& pb_data)
{
	bool ret = true;
	change_state = pb_data.change_state();
	rank = pb_data.rank();
	F_STRNCPY(team_name, pb_data.team_name().c_str(), sizeof(team_name));
	user_team_info.Reset();
	user_team_info.SetDataFromPB(pb_data.user_team_info());

	return ret;
}

bool UTAChampionRecordParam::Serialize(TLVSerializer &outstream) const
{
	PB_UTAChampionRecordData* pb_data = (PB_UTAChampionRecordData*)ProtobufMgr::Instance()->GetProtoMessage(PROTO_UTA_CHAMPION_RECORD_DATA);
	if (NULL == pb_data) return false;

	if (!this->GetPBData(pb_data)) return false;

	{
		int data_length;
		char data_buffer[sizeof(UTAChampionRecordParam) * 2] = { 0 };
		if (!pb_data->SerializeToArray(data_buffer, sizeof(data_buffer)))
		{
			assert(0);
			return false;
		}
		data_length = pb_data->GetCachedSize();

		if (!outstream.Push(data_length)) return false;

		TLVSerializer serializer;
		serializer.Reset(data_buffer, data_length);
		serializer.MoveCurPos(data_length);
		if (!outstream.Push(serializer)) return false;
	}

	return true;
}

bool UTAChampionRecordParam::Unserialize(TLVUnserializer &instream)
{
	int data_length = 0;
	char data_buffer[sizeof(UTAChampionRecordParam) * 2] = { 0 };

	if (!instream.Pop(&data_length)) return false;

	{
		TLVUnserializer data_uns;
		if (!instream.Pop(&data_uns)) return false;

		if (data_uns.Size() > sizeof(data_buffer)) return false;

		memcpy(data_buffer, data_uns.Ptr(), data_uns.Size());
		data_length = static_cast<int>(data_uns.Size());

		PB_UTAChampionRecordData* pb_data = (PB_UTAChampionRecordData*)ProtobufMgr::Instance()->GetProtoMessage(PROTO_UTA_CHAMPION_RECORD_DATA);
		if (NULL == pb_data) return false;

		if (!pb_data->ParseFromArray(data_buffer, data_length)) return false;

		if (!this->SetDataFromPB(*pb_data)) return false;
	}

	return true;
}

void UTAChampionRecordParam::Reset()
{
	change_state = structcommon::CS_NONE;
	rank = -1;
	memset(team_name, 0, sizeof(team_name));
	user_team_info.Reset();
}

bool UTAChampionRecordParamList::Serialize(TLVSerializer &outstream) const
{
	if (!outstream.Push(id_from))
	{
		return false;
	}

	if (!outstream.Push(count))
	{
		return false;
	}

	for (int i = 0; i < count && i < MAX_ONCE_UTA_C_R_COUNT; ++i)
	{
		if (!record_arr[i].Serialize(outstream)) 
			return false;
	}

	return true;
}

bool UTAChampionRecordParamList::Unserialize(TLVUnserializer &instream)
{
	if (!instream.Pop(&id_from))
	{
		return false;
	}

	if (!instream.Pop(&count))
	{
		return false;
	}

	for (int i = 0; i < count && i < MAX_ONCE_UTA_C_R_COUNT; ++i)
	{
		UTAChampionRecordParam& info = record_arr[i];

		if (!info.Unserialize(instream)) 
			return false;
	}

	return true;
}
