#include "offlinebattedatadef.hpp"
#include "servercommon/protobufmanager.hpp"
#include "servercommon/protobuf/role_battle_data.pb.h"

bool OfflineUserBattleDataListForInit::Serialize(TLVSerializer &outstream) const
{
	bool ret = outstream.Push(next_id_from)
		&& outstream.Push((unsigned int)(paramlist.size()));
	if (!ret) return false;

	for (size_t i = 0; i < paramlist.size(); ++i)
	{
		ret = outstream.Push(paramlist[i].uid)
			&& outstream.Push(paramlist[i].today_offline_reward_times);
		if (!ret) return false;

		PB_RoleBattleData* pb_data = (PB_RoleBattleData*)ProtobufMgr::Instance()->GetProtoMessage(PROTO_ROLE_BATTLE_DATA);
		if (NULL == pb_data) return false;

		if (!paramlist[i].rbd.GetPBData(pb_data)) return false;

		{
			int data_length;
			char data_buffer[sizeof(RoleBattleData) * 2] = { 0 };
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
	}

	return true;
}

bool OfflineUserBattleDataListForInit::Unserialize(TLVUnserializer &instream)
{
	int count = 0;
	bool ret = instream.Pop(&next_id_from)
		&& instream.Pop(&count);
	if (!ret) return false;

	paramlist.reserve(count);
	for (int i = 0; i < count; ++i)
	{
		OfflineUserBattleDataParam param;
		ret = instream.Pop(&param.uid)
			&& instream.Pop(&param.today_offline_reward_times);
		if (!ret) return false;

		int data_length = 0;
		char data_buffer[sizeof(RoleBattleData) * 2] = { 0 };

		if (!instream.Pop(&data_length)) return false;

		{
			TLVUnserializer data_uns;
			if (!instream.Pop(&data_uns)) return false;

			if (data_uns.Size() > sizeof(data_buffer)) return false;

			memcpy(data_buffer, data_uns.Ptr(), data_uns.Size());
			data_length = static_cast<int>(data_uns.Size());

			PB_RoleBattleData* pb_data = (PB_RoleBattleData*)ProtobufMgr::Instance()->GetProtoMessage(PROTO_ROLE_BATTLE_DATA);
			if (NULL == pb_data) return false;

			if (!pb_data->ParseFromArray(data_buffer, data_length)) return false;

			if (!param.rbd.SetDataFromPB(*pb_data)) return false;
		}

		paramlist.push_back(param);
	}

	return true;
}

bool OfflineUserBattleDataListForSave::Serialize(TLVSerializer &outstream) const
{
	bool ret = outstream.Push((unsigned int)(paramlist.size()));
	if (!ret) return false;

	for (size_t i = 0; i < paramlist.size(); ++i)
	{
		ret = outstream.Push(paramlist[i].uid)
			&& outstream.Push(paramlist[i].today_offline_reward_times);
		if (!ret) return false;

		PB_RoleBattleData* pb_data = (PB_RoleBattleData*)ProtobufMgr::Instance()->GetProtoMessage(PROTO_ROLE_BATTLE_DATA);
		if (NULL == pb_data) return false;

		if (!paramlist[i].rbd.GetPBData(pb_data)) return false;

		{
			int data_length;
			char data_buffer[sizeof(RoleBattleData) * 2] = { 0 };
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
	}

	return true;
}

bool OfflineUserBattleDataListForSave::Unserialize(TLVUnserializer &instream)
{
	int count = 0;
	bool ret = instream.Pop(&count);
	if (!ret) return false;

	paramlist.reserve(count);
	for (int i = 0; i < count; ++i)
	{
		OfflineUserBattleDataParam param;
		ret = instream.Pop(&param.uid)
			&& instream.Pop(&param.today_offline_reward_times);
		if (!ret) return false;

		int data_length = 0;
		char data_buffer[sizeof(RoleBattleData) * 2] = { 0 };

		if (!instream.Pop(&data_length)) return false;

		{
			TLVUnserializer data_uns;
			if (!instream.Pop(&data_uns)) return false;

			if (data_uns.Size() > sizeof(data_buffer)) return false;

			memcpy(data_buffer, data_uns.Ptr(), data_uns.Size());
			data_length = static_cast<int>(data_uns.Size());

			PB_RoleBattleData* pb_data = (PB_RoleBattleData*)ProtobufMgr::Instance()->GetProtoMessage(PROTO_ROLE_BATTLE_DATA);
			if (NULL == pb_data) return false;

			if (!pb_data->ParseFromArray(data_buffer, data_length)) return false;

			if (!param.rbd.SetDataFromPB(*pb_data)) return false;
		}

		paramlist.push_back(param);
	}

	return true;
}