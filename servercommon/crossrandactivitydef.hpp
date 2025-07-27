#ifndef __CROSS_RAND_ACTIVITY_DEF_HPP__
#define __CROSS_RAND_ACTIVITY_DEF_HPP__

#include "servercommon/activitydef.hpp"

#pragma pack(push, 4)
static const int CROSS_RAND_ACTIVITY_DATA_LEN = 1024;
struct CrossRandActivityParam
{
	CrossRandActivityParam() : change_state(0), activity_type(0), open_time(0), end_time(0), activity_status(0), data_len(0)
	{
		memset(activity_data, 0, sizeof(activity_data));
	}

	int change_state;

	int activity_type;
	unsigned int open_time;
	unsigned int end_time;
	int activity_status;

	int data_len;
	char activity_data[CROSS_RAND_ACTIVITY_DATA_LEN];
};

class CrossActivityDataListParam
{
public:
	CrossActivityDataListParam() : count(0) {}

	bool Serialize(TLVSerializer &outstream) const
	{
		bool ret = outstream.Push(next_id);
		if (!ret) return false;

		ret = outstream.Push(count);
		if (!ret) return false;

		for (int i = 0; i < count && i < CROSS_RAND_ACTIVITY_COUNT; ++i)
		{
			TLVSerializer cross_rand_activity_data;
			cross_rand_activity_data.Reset((char *)&param_list[i], sizeof(CrossRandActivityParam));
			cross_rand_activity_data.MoveCurPos(sizeof(param_list[i]));

			ret = outstream.Push(cross_rand_activity_data);
			if (!ret) return false;
		}

		return true;
	}

	bool Unserialize(TLVUnserializer &instream) const
	{
		bool ret = instream.Pop(&next_id);
		if (!ret) return false;

		ret = instream.Pop(&count);
		if (!ret) return false;

		for (int i = 0; i < count && i < CROSS_RAND_ACTIVITY_COUNT; ++i)
		{
			TLVUnserializer cross_rand_activity_data;
			instream.Pop(&cross_rand_activity_data);
			if (cross_rand_activity_data.Size() <= sizeof(CrossRandActivityParam))
			{
				memcpy((void *)&param_list[i], cross_rand_activity_data.Ptr(), cross_rand_activity_data.Size());
			}
		}

		return true;
	}

	long long next_id;

	int count;
	CrossRandActivityParam param_list[CROSS_RAND_ACTIVITY_COUNT];
};


#pragma pack(pop)

#endif // !__CROSS_ACTIVITY_DEF_HPP__
