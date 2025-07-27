#include "couragechallengerankdef.hpp"

bool CourageChallengeRankParam::Serialize(TLVSerializer & outstream) const
{
	bool ret = true;
	ret = outstream.Push((int)data_list.size());
	if (!ret)
	{
		return false;
	}
	for (std::map<int, CourageChallengeRankItem>::const_iterator it = data_list.begin(); it != data_list.end(); it++)
	{
		ret = outstream.Push(it->second.uid);
		if (!ret || it->second.uid <= 0)
		{
			return false;
		}
		ret = outstream.Push(it->second.pos) &&
			outstream.Push(it->second.change_state) &&
			outstream.Push(it->second.avatar_type) &&
			outstream.Push(it->second.headshot_id) &&
			outstream.Push(it->second.prof) &&
			outstream.Push(it->second.star) &&
			outstream.Push(it->second.layer_level) &&
			outstream.Push(it->second.get_timestamp);

		if (!ret)
		{
			return false;
		}
		const char * role_name = it->second.user_name;
		ret = outstream.Push(role_name);
		if (!ret)
		{
			return false;
		}
	}
	return ret;
}

bool CourageChallengeRankParam::Unserialize(TLVUnserializer & instream)
{
	std::map<int, CourageChallengeRankItem>().swap(data_list);

	bool ret = true;
	int count = 0;
	ret = instream.Pop(&count);
	if (!ret)
	{
		return false;
	}
	int uid = 0;
	for (int i = 0; i < count; i++)
	{
		ret = instream.Pop(&uid);
		if (!ret || uid <= 0)
		{
			return false;
		}

		CourageChallengeRankItem & data_node = data_list[uid];
		data_node.uid = uid;

		ret = instream.Pop(&data_node.pos) &&
			instream.Pop(&data_node.change_state) &&
			instream.Pop(&data_node.avatar_type) &&
			instream.Pop(&data_node.headshot_id) &&
			instream.Pop(&data_node.prof) &&
			instream.Pop(&data_node.star) &&
			instream.Pop(&data_node.layer_level) &&
			instream.Pop(&data_node.get_timestamp);

		if (!ret)
		{
			return false;
		}
		const char * role_name = NULL;
		ret = instream.Pop(&role_name);
		if (!ret)
		{
			return false;
		}
		STRNCPY((char *)data_node.user_name, role_name, sizeof(data_node.user_name));
	}
	return ret;
}

void CourageChallengeRankItem::SetData(SynCourageChallengeRankInfo & role_info)
{
	uid = role_info.uid;
	avatar_type = role_info.avatar_type;
	headshot_id = role_info.headshot_id;
	prof = role_info.prof;
	reserve_sh = role_info.reserve_sh;
	star = role_info.star;
	layer_level = role_info.layer_level;
	STRNCPY(user_name, role_info.user_name, sizeof(user_name));
	get_timestamp = role_info.get_timestamp;
}
