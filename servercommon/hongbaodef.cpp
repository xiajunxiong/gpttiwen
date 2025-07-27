#include "hongbaodef.hpp"

#include "servercommon/struct/structcommon.h"

void HongBaoItem::Reset()
{
	change_state = structcommon::CS_NONE;

	ll_unique_hong_bao_id = 0;
	hong_bao_id = 0;
	invalid_timestamp = 0;
	hongbao_type = -1;
	hongbao_sys_type = -1;
	send_uid = 0;
	send_avatar_type = 0;
	send_headshot_id = 0;
	memset(send_name, 0, sizeof(GameName));
	memset(greeting, 0, sizeof(HongBaoGreeting));
	used_val = 0;
	info_data.Reset();
	hongbao_reward_info.Reset();

	get_uid_set.clear();
}

HongBaoItem& HongBaoItem::operator=(const HongBaoItem & _o)
{
	change_state = _o.change_state;

	ll_unique_hong_bao_id = _o.ll_unique_hong_bao_id;
	hong_bao_id = _o.hong_bao_id;
	invalid_timestamp = _o.invalid_timestamp;
	hongbao_type = _o.hongbao_type;
	hongbao_sys_type = _o.hongbao_sys_type;
	send_uid = _o.send_uid;
	send_avatar_type = _o.send_avatar_type;
	send_headshot_id = _o.send_headshot_id;
	memcpy(send_name, _o.send_name, sizeof(GameName));
	memcpy(greeting, _o.greeting, sizeof(HongBaoGreeting));
	used_val = _o.used_val;
	info_data.SetData(_o.info_data.data);
	hongbao_reward_info = _o.hongbao_reward_info;

	std::set<int>::iterator it_begin = _o.get_uid_set.begin();
	std::set<int>::iterator it_end = _o.get_uid_set.end();
	get_uid_set.clear();
	for (; it_begin != it_end; ++it_begin)
	{
		get_uid_set.insert(*it_begin);
	}

	return *this;
}

HongBaoItem& HongBaoItem::operator=(const SyncHongBaoItem & _o)
{
	change_state = _o.change_state;

	ll_unique_hong_bao_id = _o.ll_unique_hong_bao_id;
	hong_bao_id = _o.hong_bao_id;
	invalid_timestamp = _o.invalid_timestamp;
	hongbao_type = _o.hongbao_type;
	hongbao_sys_type = _o.hongbao_sys_type;
	send_uid = _o.send_uid;
	send_avatar_type = _o.send_avatar_type;
	send_headshot_id = _o.send_headshot_id;
	memcpy(send_name, _o.send_name, sizeof(GameName));
	memcpy(greeting, _o.greeting, sizeof(HongBaoGreeting));
	used_val = _o.used_val;
	info_data.SetData(_o.info_data.data);
	memcpy(&hongbao_reward_info, &_o.hongbao_reward_info, sizeof(HongBaoItem::HongBaoRewardInfo));


	return *this;
}


void SyncHongBaoItem::Reset()
{
	change_state = 0;

	ll_unique_hong_bao_id = 0;
	hong_bao_id = 0;
	invalid_timestamp = 0;
	hongbao_type = -1;
	hongbao_sys_type = -1;
	send_uid = 0;
	send_avatar_type = 0;
	send_headshot_id = 0;
	memset(send_name, 0, sizeof(GameName));
	memset(greeting, 0, sizeof(HongBaoGreeting));
	used_val = 0;
	info_data.Reset();
	hongbao_reward_info.Reset();
}


SyncHongBaoItem& SyncHongBaoItem::operator=(const HongBaoItem & _o)
{
	change_state = _o.change_state;

	ll_unique_hong_bao_id = _o.ll_unique_hong_bao_id;
	hong_bao_id = _o.hong_bao_id;
	invalid_timestamp = _o.invalid_timestamp;
	hongbao_type = _o.hongbao_type;
	hongbao_sys_type = _o.hongbao_sys_type;
	send_uid = _o.send_uid;
	send_avatar_type = _o.send_avatar_type;
	send_headshot_id = _o.send_headshot_id;
	memcpy(send_name, _o.send_name, sizeof(GameName));
	memcpy(greeting, _o.greeting, sizeof(HongBaoGreeting));
	used_val = _o.used_val;
	info_data.SetData(_o.info_data.data);
	memcpy(&hongbao_reward_info, &_o.hongbao_reward_info, sizeof(HongBaoItem::HongBaoRewardInfo));


	return *this;
}

bool HongBaoParam::Serialize(TLVSerializer &outstream) const
{
	bool ret = outstream.Push(count);
	if (!ret) return false;

	for (int i = 0; i < count && i < MAX_ONCE_HONG_BAO_COUNT; ++i)
	{
		ret = outstream.Push(hong_bao_list[i].change_state);
		ret = ret && outstream.Push(hong_bao_list[i].ll_unique_hong_bao_id);
		ret = ret && outstream.Push(hong_bao_list[i].hong_bao_id);
		ret = ret && outstream.Push(hong_bao_list[i].hongbao_sys_type);

		if (!ret) return false;

		if (hong_bao_list[i].change_state == structcommon::CS_DELETE) continue;

		const char *send_name_tmp = hong_bao_list[i].send_name;
		const char *hongbao_greeting_tmp = hong_bao_list[i].greeting;

		TLVSerializer hongbao_reward_info_data;
		hongbao_reward_info_data.Reset((char*)&hong_bao_list[i].hongbao_reward_info, sizeof(hong_bao_list[i].hongbao_reward_info));
		hongbao_reward_info_data.MoveCurPos(sizeof(hong_bao_list[i].hongbao_reward_info));

		TLVSerializer info_data;
		info_data.Reset((char*)&hong_bao_list[i].info_data, sizeof(hong_bao_list[i].info_data));
		info_data.MoveCurPos(sizeof(hong_bao_list[i].info_data));

		ret = ret && outstream.Push(hong_bao_list[i].invalid_timestamp);
		ret = ret && outstream.Push(hong_bao_list[i].hongbao_type);
		ret = ret && outstream.Push(hong_bao_list[i].send_uid);
		ret = ret && outstream.Push(hong_bao_list[i].send_avatar_type);
		ret = ret && outstream.Push(hong_bao_list[i].send_headshot_id);
		ret = ret && outstream.Push(send_name_tmp);
		ret = ret && outstream.Push(hongbao_greeting_tmp);
		ret = ret && outstream.Push(hong_bao_list[i].used_val);
		ret = ret && outstream.Push(info_data);
		ret = ret && outstream.Push(hongbao_reward_info_data);
		if (!ret) return false;
	}

	return true;
}

bool HongBaoParam::Unserialize(TLVUnserializer &instream)
{
	bool ret = instream.Pop(&count);
	if (!ret) return false;

	for (int i = 0; i < count && i < MAX_ONCE_HONG_BAO_COUNT; ++i)
	{
		ret = ret && instream.Pop(&hong_bao_list[i].change_state);
		ret = ret && instream.Pop(&hong_bao_list[i].ll_unique_hong_bao_id);
		ret = ret && instream.Pop(&hong_bao_list[i].hong_bao_id);
		ret = ret && instream.Pop(&hong_bao_list[i].hongbao_sys_type);

		if (!ret) return false;

		if (hong_bao_list[i].change_state == structcommon::CS_DELETE) continue;

		TLVUnserializer hongbao_reward_info_data;
		TLVUnserializer info_data;
		const char *send_name_tmp = NULL;
		const char *hongbao_greeting_tmp = NULL;

		ret = instream.Pop(&hong_bao_list[i].invalid_timestamp)
			&& instream.Pop(&hong_bao_list[i].hongbao_type)
			&& instream.Pop(&hong_bao_list[i].send_uid)
			&& instream.Pop(&hong_bao_list[i].send_avatar_type)
			&& instream.Pop(&hong_bao_list[i].send_headshot_id)
			&& instream.Pop(&send_name_tmp)
			&& instream.Pop(&hongbao_greeting_tmp)
			&& instream.Pop(&hong_bao_list[i].used_val)
			&& instream.Pop(&info_data)
			&& instream.Pop(&hongbao_reward_info_data);
		if (!ret) return false;

		STRNCPY(hong_bao_list[i].send_name, send_name_tmp, sizeof(GameName));
		STRNCPY(hong_bao_list[i].greeting, hongbao_greeting_tmp, sizeof(HongBaoGreeting));

		if (info_data.Size() <= sizeof(hong_bao_list[i].info_data))
		{
			memcpy(&hong_bao_list[i].info_data, info_data.Ptr(), info_data.Size());
		}
		else
		{
			printf("DESERIALIZE data.Size() > sizeof(HongBaoItem::InfoData) error\n");
			return false;
		}

		if (hongbao_reward_info_data.Size() <= sizeof(hong_bao_list[i].hongbao_reward_info))
		{
			memcpy(&hong_bao_list[i].hongbao_reward_info, hongbao_reward_info_data.Ptr(), hongbao_reward_info_data.Size());
		}
		else
		{
			printf("DESERIALIZE data.Size() > sizeof(HongBaoItem::HongBaoRewardInfo) error\n");
			return false;
		}
	}

	return true;
}

void GiveHongBaoUserInfo::Reset()
{
	hongbao_type = HONGBAO_TYPE_INVALID;
	hongbao_sys_type = -1;
	send_uid = 0;
	send_avatar_type = 0;
	send_headshot_id = 0;
	memset(send_name, 0, sizeof(GameName));
	memset(greeting, 0, sizeof(HongBaoGreeting));
	info_data.Reset();
}
