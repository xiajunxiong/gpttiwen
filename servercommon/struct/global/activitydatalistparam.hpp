#ifndef __ACTIVITY_DATA_LIST_PARAM_HPP__
#define __ACTIVITY_DATA_LIST_PARAM_HPP__

#include "servercommon/activitydef.hpp"

class ActivityDataListParam
{
public:
	static const int LIST_COUNT = 300;

	UNSTD_STATIC_CHECK(LIST_COUNT > RAND_ACTIVITY_TYPE_MAX + ACTIVITY_TYPE_MAX + SPECIAL_ACTIVITY_TYPE_MAX);

public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	struct ActivityDataParam
	{
		ActivityDataParam()
		{
			this->Reset(0);
		}

		void Reset(int act_type)
		{	
			activity_type = act_type;
			activity_data.Reset(act_type);				
		}

		int activity_type;
		ActivityData activity_data;
	};

	long long next_id;
	int count;
	ActivityDataParam data_list[LIST_COUNT];
};

unsigned int GetActivityRoleDataLengthByType(int activity_type);

class ActivityRoleDataParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	struct DBActivityRoleDataItem
	{
		DBActivityRoleDataItem()
		{
			this->Reset(0);
		}

		void Reset(int act_type)
		{
			inc_id = 0;
			change_state = 0;
			role_id = 0;
			activity_type = ACTIVITY_TYPE_INVALID;
			data.Reset(act_type);
		}

		long long inc_id;		
		char change_state;
		int role_id;
		int activity_type;		
		ActivityRoleData data;
	};

	int count;
	DBActivityRoleDataItem data_list[MAX_ONCE_ACTIVITY_ROLE_DATA_COUNT];
};

#endif