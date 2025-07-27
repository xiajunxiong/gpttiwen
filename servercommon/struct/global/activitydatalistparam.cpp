#include "common/tlvprotocol.h"
#include "activitydatalistparam.hpp"

bool ActivityDataListParam::Serialize(TLVSerializer &outstream) const
{
	bool ret = outstream.Push(next_id);
	if (!ret) return false;

	ret = outstream.Push(count);
	if (!ret) return false;

	for (int i = 0; i < count && i < LIST_COUNT; ++i)
	{
		ret = outstream.Push(data_list[i].activity_type);
		if (!ret) return false;

		{
			TLVSerializer activity_data;
			activity_data.Reset((char*)&data_list[i].activity_data, sizeof(data_list[i].activity_data));
			activity_data.MoveCurPos(sizeof(data_list[i].activity_data));

			ret = outstream.Push(activity_data);
			if (!ret) return false;
		}
	}

	return true;
}

bool ActivityDataListParam::Unserialize(TLVUnserializer &instream)
{
	bool ret = instream.Pop(&next_id);
	if (!ret) return false;

	ret = instream.Pop(&count);
	if (!ret) return false;

	for (int i = 0; i < count && i < LIST_COUNT; ++i)
	{
		ret = instream.Pop(&data_list[i].activity_type);
		if (!ret) return false;

		{
			TLVUnserializer activity_data;
			instream.Pop(&activity_data);
			if (activity_data.Size() <= sizeof(data_list[i].activity_data))
			{
				memcpy(&data_list[i].activity_data, activity_data.Ptr(), activity_data.Size());
			}
		}
	}

	return true;
}


unsigned int GetActivityRoleDataLengthByType(int activity_type)
{
	unsigned int length = 0;
	switch (activity_type)
	{
		case ACTIVITY_TYPE_BRAVE_GROUND:
			{
				length = sizeof(ActivityRankBraveGroundRoleData);
			}
			break;
		case ACTIVITY_TYPE_CHIEF_ELECTION:
			{
				length = sizeof(ActivityChiefElectionRoleData);
			}
			break;
		case ACTIVITY_TYPE_CHIEF_PEAK_BATTLE:
			{
				length = sizeof(ActivityChiefPeakBattleRoleData);
			}
			break;
		case ACTIVITY_TYPE_HUAN_JIE_ZHAN_CHANG:
			{
				length = sizeof(ActivityHuanJieZhanChangRoleData);
			}
			break;

		default:
			{
				length = 0;
			}
			break;
	}


	return length;
}

bool ActivityRoleDataParam::Serialize(TLVSerializer & outstream) const
{
	bool ret = outstream.Push(count);
	if (!ret)
	{
		return false;
	}

	for (int i = 0; i < count; ++i)
	{
		const DBActivityRoleDataItem & rank_item = data_list[i];

		ret = outstream.Push(rank_item.change_state) && outstream.Push(rank_item.activity_type) && outstream.Push(rank_item.inc_id) && outstream.Push(rank_item.role_id);
		if (!ret)
		{
			return false;
		}

		if (rank_item.change_state == structcommon::CS_DELETE)
		{
			continue;
		}

		unsigned int length = GetActivityRoleDataLengthByType(rank_item.activity_type);
		if (length > 0)
		{
			TLVSerializer role_data;
			role_data.Reset((void *)&rank_item.data, sizeof(rank_item.data));
			role_data.MoveCurPos(length);

			ret = outstream.Push(role_data);

			if (!ret)
			{
				return false;
			}
		}
	}

	return true;
}

bool ActivityRoleDataParam::Unserialize(TLVUnserializer & instream)
{
	bool ret = instream.Pop(&count);
	if (!ret) return false;

	for (int i = 0; i < count; ++i)
	{
		DBActivityRoleDataItem & rank_item = data_list[i];
		ret = instream.Pop(&rank_item.change_state) && instream.Pop(&rank_item.activity_type) && instream.Pop(&rank_item.inc_id) && instream.Pop(&rank_item.role_id);

		if (!ret)
		{
			return false;
		}

		if (rank_item.change_state == structcommon::CS_DELETE)
		{
			continue;
		}

		unsigned int length = GetActivityRoleDataLengthByType(rank_item.activity_type);
		if (length > 0)
		{
			TLVUnserializer role_data;
			ret = instream.Pop(&role_data);

			if (role_data.Size() <= sizeof(rank_item.data))
			{
				memcpy(&rank_item.data, role_data.Ptr(), role_data.Size());
			}

			if (!ret)
			{
				return false;
			}
		}
	}

	return true;
}



bool IsRandActivity(int activity_type)
{
	if (activity_type < RAND_ACTIVITY_TYPE_BEGIN || activity_type >= RAND_ACTIVITY_TYPE_END)
	{
		return false;
	}

	return true;
}

bool IsNormalActivity(int activity_type)
{
	if (activity_type <= ACTIVITY_TYPE_INVALID || activity_type >= ACTIVITY_TYPE_MAX)
	{
		return false;
	}

	return true;
}



unsigned int GetActivityDataLength(int activity_type)
{
	unsigned int ret = 0;

	switch (activity_type)
	{
		case ACTIVITY_TYPE_HUAN_JIE_ZHAN_CHANG:
			{
				ret = sizeof(HuanJieZhanChangActivityData);
			}
			break;

		default:
			{
				if (IsRandActivity(activity_type))
				{
					ret = sizeof(RandActivityData);
				}
				else if (IsNormalActivity(activity_type))
				{
					ret = sizeof(NormalActivityData);
				}
			}
			break;
	}

	if (ret > sizeof(ActivityData))
	{
		ret = 0;
	}

	return ret;
}

void ActivityData::Reset(int act_type)
{
	switch (act_type)
	{
		case ACTIVITY_TYPE_HUAN_JIE_ZHAN_CHANG:
			{
				huan_jie_activity_data.Reset();
			}
			break;
		case ACTIVITY_TYPE_WORLD_TEAM_ARENA:
			{
				world_team_arena.Reset();
			}
			break;
		default:
			{
				if (IsRandActivity(act_type))
				{
					rand_activity_data.Reset();
				}
				else if (IsNormalActivity(act_type))
				{
					activity_data.Reset();
				}
				else
				{
					memset(this, 0, sizeof(*this));
				}
			}
			break;
	}
}
