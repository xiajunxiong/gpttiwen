#ifndef __ACTIVITY_GUILD_ANSWER_HPP__
#define __ACTIVITY_GUILD_ANSWER_HPP__

#include "global/activity/activity.hpp"

class ActivityGuildAnswer : public Activity
{
public:
	ActivityGuildAnswer(ActivityManager *activity_manager);
	~ActivityGuildAnswer();

	virtual void OnActivityStatusChange(int from_status, int to_status);

	const std::vector<int>& GetRoomKeyVec() { return m_room_key_vec; }
private:
	std::vector<int> m_room_key_vec;
};

#endif