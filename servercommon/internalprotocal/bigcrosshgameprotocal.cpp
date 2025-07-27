#include "bigcrosshgameprotocal.h"

bool bigchgameprotocol::WriteBegin(int msg_type, char* buffer, int max_len, int* out_len)
{
	// 初始化
	if (*out_len + 4 >= max_len) return false;

	return WriteInt(msg_type, buffer, max_len, out_len);
}

bool bigchgameprotocol::ReadBegin(int& msg_type, const char* buffer, int max_len, int* out_len)
{
	// 初始化
	if (*out_len + 4 >= max_len) return false;

	return ReadInt(msg_type, buffer, max_len, out_len);
}

int bigchgameprotocol::HGameBigcUTASyncTeamInfoList::GetMessageType() const
{
	return header.msg_type;
}

bool bigchgameprotocol::HGameBigcUTASyncTeamInfoList::Serialize(char* buffer, int buffer_size, int* out_length)
{
	bool ret = WRITE_BEGIN(header.msg_type) && WRITE_INT(static_cast<int>(team_list.size()));
	if (!ret) return false;

	for (size_t i = 0; i < team_list.size(); ++i)
	{
		ret = team_list[i].Serialize(buffer, buffer_size, out_length);
		if (!ret) return false;
	}

	return ret;
}

bool bigchgameprotocol::HGameBigcUTASyncTeamInfoList::Deserialize(const char* data, int data_length, int* out_offset)
{
	int team_num = 0;
	bool ret = READ_BEGIN(header.msg_type) && READ_INT(team_num);
	if (!ret) return false;

	team_list.reserve(team_num);
	for (int i = 0; i < team_num; ++i)
	{
		MsgUTASyncTeamInfo team_info;
		ret = team_info.Deserialize(data, data_length, out_offset);
		if (!ret) return false;

		team_list.push_back(team_info);
	}

	return ret;
}

bool bigchgameprotocol::MsgUTASyncTeamInfo::Serialize(char* buffer, int buffer_size, int* out_length)
{
	bool ret = WRITE_INT(plat_type) 
		&& WRITE_INT(hidden_idx) 
		&& WRITE_INT(rank)
		&& WRITE_LL(team_id_in_hidden) 
		&& WRITE_STRN(team_name, sizeof(team_name));
	if (!ret) return false;

	for (int i = 0; i < ARRAY_ITEM_COUNT(member_list); ++i)
	{
		ret = member_list[i].Serialize(buffer, buffer_size, out_length);
		if (!ret) return false;
	}

	return ret;
}

bool bigchgameprotocol::MsgUTASyncTeamInfo::Deserialize(const char* data, int data_length, int* out_offset)
{
	bool ret = READ_INT(plat_type)
		&& READ_INT(hidden_idx)
		&& READ_INT(rank)
		&& READ_LL(team_id_in_hidden)
		&& READ_STRN(team_name, sizeof(team_name), sizeof(team_name));
	if (!ret) return false;

	for (int i = 0; i < ARRAY_ITEM_COUNT(member_list); ++i)
	{
		ret = member_list[i].Deserialize(data, data_length, out_offset);
		if (!ret) return false;
	}

	return ret;
}
