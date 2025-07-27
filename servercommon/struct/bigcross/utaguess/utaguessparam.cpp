#include "utaguessparam.hpp"

#include "servercommon/struct/structcommon.h"
#include "servercommon/protobufmanager.hpp"


bool UTAGuessUserInfoData::GetPBData(ARG_OUT PB_UTAGuessUserInfoData * _out_data) const
{
	_out_data->set_uid(uid);
	_out_data->set_plat_id(plat_id);
	_out_data->set_server_id(server_id);
	_out_data->set_team_seq(team_seq);
	_out_data->set_match_seq(match_seq);
	_out_data->set_bet_coin_num(bet_coin_num);
	_out_data->set_is_get(is_get);

	return true;
}

bool UTAGuessUserInfoData::SetDataFromPB(const PB_UTAGuessUserInfoData& _pb_data)
{
	uid = _pb_data.uid();
	plat_id = _pb_data.plat_id();
	server_id = _pb_data.server_id();
	team_seq = _pb_data.team_seq();
	match_seq = _pb_data.match_seq();
	bet_coin_num = _pb_data.bet_coin_num();
	is_get = _pb_data.is_get();

	return true;
}

bool UTAGuessUserInfoData::Serialize(TLVSerializer &outstream) const
{
	if (!outstream.Push(change_state))
		return false;

	PB_UTAGuessUserInfoData* pb_data = (PB_UTAGuessUserInfoData*)ProtobufMgr::Instance()->GetProtoMessage(PROTO_UTA_GUESS_USER_INFO_DATA);
	if (NULL == pb_data) 
		return false;

	if (!this->GetPBData(pb_data)) 
		return false;

	{
		int data_length;
		char data_buffer[sizeof(UTAGuessUserInfoData) * 2] = { 0 };
		if (!pb_data->SerializeToArray(data_buffer, sizeof(data_buffer)))
		{
			assert(0);
			return false;
		}
		data_length = pb_data->GetCachedSize();

		if (!outstream.Push(data_length)) 
			return false;

		TLVSerializer serializer;
		serializer.Reset(data_buffer, data_length);
		serializer.MoveCurPos(data_length);
		if (!outstream.Push(serializer)) 
			return false;
	}

	return true;
}

bool UTAGuessUserInfoData::Unserialize(TLVUnserializer &instream)
{
	if (!instream.Pop(&change_state))
		return false;

	int data_length = 0;
	char data_buffer[sizeof(UTAGuessUserInfoData) * 2] = { 0 };

	if (!instream.Pop(&data_length)) 
		return false;

	{
		TLVUnserializer data_uns;
		if (!instream.Pop(&data_uns)) 
			return false;

		if (data_uns.Size() > sizeof(data_buffer)) 
			return false;

		memcpy(data_buffer, data_uns.Ptr(), data_uns.Size());
		data_length = static_cast<int>(data_uns.Size());

		PB_UTAGuessUserInfoData* pb_data = (PB_UTAGuessUserInfoData*)ProtobufMgr::Instance()->GetProtoMessage(PROTO_UTA_GUESS_USER_INFO_DATA);
		if (NULL == pb_data) 
			return false;

		if (!pb_data->ParseFromArray(data_buffer, data_length)) 
			return false;

		if (!this->SetDataFromPB(*pb_data)) 
			return false;
	}

	return true;
}

void UTAGuessUserInfoData::Reset()
{
	change_state = structcommon::CS_NONE;

	uid = 0;
	plat_id = -1;
	server_id = -1;
	team_seq = 0;
	match_seq = 0;
	bet_coin_num = 0;
	is_get = 0;
}

bool UTAGuessTeamInfoData::GetPBData(ARG_OUT PB_UTAGuessTeamInfoData * _out_data) const
{
	_out_data->set_team_seq(team_seq);
	_out_data->set_match_seq(match_seq);

	_out_data->set_plat_id(plat_id);
	_out_data->set_server_id(server_id);

	_out_data->set_total_bet_num(total_bet_num);
	_out_data->set_guess_status(guess_status);

	return true;
}

bool UTAGuessTeamInfoData::SetDataFromPB(const PB_UTAGuessTeamInfoData& _pb_data)
{
	team_seq = _pb_data.team_seq();
	match_seq = _pb_data.match_seq();

	plat_id = _pb_data.plat_id();
	server_id = _pb_data.server_id();

	total_bet_num = _pb_data.total_bet_num();
	guess_status = _pb_data.guess_status();

	return true;
}

void UTAGuessTeamInfoData::Reset()
{
	team_seq = 0;
	match_seq = 0;
	plat_id = -1;
	server_id = -1;
	total_bet_num = 0;
	guess_status = GUESS_STATUS_TYPE_START;
}


bool UTAGuessMatchInfoData::GetPBData(ARG_OUT PB_UTAGuessMatchInfoData * _out_data) const
{
	bool ret = true;

	_out_data->set_match_seq(match_seq);
	_out_data->mutable_team_info()->Clear();
	_out_data->mutable_team_info()->Reserve(MAX_MATCH_TEAM_NUM);
	for (int i = 0; i < MAX_MATCH_TEAM_NUM && i < ARRAY_ITEM_COUNT(team_info); ++i)
	{
		_out_data->add_team_info();
		ret = ret && team_info[i].GetPBData(_out_data->mutable_team_info(i));
	}

	return ret;
}

bool UTAGuessMatchInfoData::SetDataFromPB(const PB_UTAGuessMatchInfoData& _pb_data)
{
	bool ret = true;

	match_seq = _pb_data.match_seq();
	memset(team_info, 0, sizeof(team_info));
	for (int i = 0; i < _pb_data.team_info_size() && i < ARRAY_ITEM_COUNT(team_info); ++i)
	{
		ret = ret && team_info[i].SetDataFromPB(_pb_data.team_info(i));
	}

	return ret;
}

bool UTAGuessMatchInfoData::Serialize(TLVSerializer &outstream) const
{
	if (!outstream.Push(change_state))
		return false;

	if (!outstream.Push(match_seq))
		return false;

	for (int i = 0; i < MAX_MATCH_TEAM_NUM; ++i)
	{
		PB_UTAGuessTeamInfoData * pb_data = (PB_UTAGuessTeamInfoData*)ProtobufMgr::Instance()->GetProtoMessage(PROTO_UTA_GUESS_TEAM_INFO_DATA);
		if (NULL == pb_data)
			return false;

		if (!team_info[i].GetPBData(pb_data))
			return false;

		{
			int data_length;
			char data_buffer[sizeof(UTAGuessTeamInfoData) * 2] = { 0 };
			if (!pb_data->SerializeToArray(data_buffer, sizeof(data_buffer)))
			{
				assert(0);
				return false;
			}
			data_length = pb_data->GetCachedSize();

			if (!outstream.Push(data_length))
				return false;

			TLVSerializer serializer;
			serializer.Reset(data_buffer, data_length);
			serializer.MoveCurPos(data_length);
			if (!outstream.Push(serializer))
				return false;
		}
	}

	return true;
}

bool UTAGuessMatchInfoData::Unserialize(TLVUnserializer &instream)
{
	if (!instream.Pop(&change_state))
		return false;

	if (!instream.Pop(&match_seq))
		return false;

	for (int i = 0; i < MAX_MATCH_TEAM_NUM; ++i)
	{
		int data_length = 0;
		char data_buffer[sizeof(UTAGuessTeamInfoData) * 2] = { 0 };

		if (!instream.Pop(&data_length))
			return false;

		{
			TLVUnserializer data_uns;
			if (!instream.Pop(&data_uns))
				return false;

			if (data_uns.Size() > sizeof(data_buffer))
				return false;

			memcpy(data_buffer, data_uns.Ptr(), data_uns.Size());
			data_length = static_cast<int>(data_uns.Size());

			PB_UTAGuessTeamInfoData* pb_data = (PB_UTAGuessTeamInfoData*)ProtobufMgr::Instance()->GetProtoMessage(PROTO_UTA_GUESS_TEAM_INFO_DATA);
			if (NULL == pb_data)
				return false;

			if (!pb_data->ParseFromArray(data_buffer, data_length))
				return false;

			if (!team_info[i].SetDataFromPB(*pb_data))
				return false;
		}
	}

	return true;
}

void UTAGuessMatchInfoData::Reset()
{
	change_state = structcommon::CS_NONE;
	match_seq = -1;
	for (int i = 0; i < MAX_MATCH_TEAM_NUM; ++i)
	{
		team_info[i].Reset();
	}
}


bool UTAGuessMatchInfoParamList::Serialize(TLVSerializer &outstream) const
{
	if (!outstream.Push(id_from))
	{
		return false;
	}

	if (!outstream.Push(count))
	{
		return false;
	}

	for (int i = 0; i < count && i < MAX_ONCE_UTA_GUESS_COUNT; ++i)
	{
		if (!match_info_arr[i].Serialize(outstream))
			return false;
	}

	return true;
}

bool UTAGuessMatchInfoParamList::Unserialize(TLVUnserializer &instream)
{
	if (!instream.Pop(&id_from))
	{
		return false;
	}

	if (!instream.Pop(&count))
	{
		return false;
	}

	for (int i = 0; i < count && i < MAX_ONCE_UTA_GUESS_COUNT; ++i)
	{
		UTAGuessMatchInfoData& info = match_info_arr[i];

		if (!info.Unserialize(instream))
			return false;
	}

	return true;
}

bool UTAGuessUserInfoParamList::Serialize(TLVSerializer &outstream) const
{
	if (!outstream.Push(id_from))
	{
		return false;
	}

	if (!outstream.Push(count))
	{
		return false;
	}

	for (int i = 0; i < count && i < MAX_ONCE_UTA_GUESS_COUNT  ; ++i)
	{
		if (!user_info_arr[i].Serialize(outstream))
			return false;
	}

	return true;
}

bool UTAGuessUserInfoParamList::Unserialize(TLVUnserializer &instream)
{
	if (!instream.Pop(&id_from))
	{
		return false;
	}

	if (!instream.Pop(&count))
	{
		return false;
	}

	for (int i = 0; i < count && i < MAX_ONCE_UTA_GUESS_COUNT; ++i)
	{
		UTAGuessUserInfoData & info = user_info_arr[i];

		if (!info.Unserialize(instream))
			return false;
	}

	return true;
}

bool UTAGuessUserEarningsInfoParam::Serialize(TLVSerializer &outstream) const
{
	bool ret = true;
	ret &= outstream.Push(change_state);
	ret &= outstream.Push(uid);
	ret &= outstream.Push(plat_id);
	ret &= outstream.Push(server_id);
	ret &= outstream.Push(earnings);

	return ret;
}

bool UTAGuessUserEarningsInfoParam::Unserialize(TLVUnserializer &instream)
{
	bool ret = true;
	ret &= instream.Pop(&change_state);
	ret &= instream.Pop(&uid);
	ret &= instream.Pop(&plat_id);
	ret &= instream.Pop(&server_id);
	ret &= instream.Pop(&earnings);

	return ret;
}

void UTAGuessUserEarningsInfoParam::Reset()
{
	change_state = structcommon::CS_NONE;

	uid = 0;
	plat_id = -1;
	server_id = -1;
	earnings = 0;
}

bool UTAGuessEarningsInfoParamList::Serialize(TLVSerializer &outstream) const
{
	if (!outstream.Push(id_from))
	{
		return false;
	}

	if (!outstream.Push(count))
	{
		return false;
	}

	for (int i = 0; i < count && i < MAX_ONCE_UTA_GUESS_COUNT; ++i)
	{
		if (!earnings_info_arr[i].Serialize(outstream))
			return false;
	}

	return true;
}

bool UTAGuessEarningsInfoParamList::Unserialize(TLVUnserializer &instream)
{
	if (!instream.Pop(&id_from))
	{
		return false;
	}

	if (!instream.Pop(&count))
	{
		return false;
	}

	for (int i = 0; i < count && i < MAX_ONCE_UTA_GUESS_COUNT; ++i)
	{
		UTAGuessUserEarningsInfoParam & info = earnings_info_arr[i];

		if (!info.Unserialize(instream))
			return false;
	}

	return true;
}
