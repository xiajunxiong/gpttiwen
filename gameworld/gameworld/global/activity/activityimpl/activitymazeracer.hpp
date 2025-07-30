#ifndef __ACTIVITY_MAZE_RACER_HPP__
#define __ACTIVITY_MAZE_RACER_HPP__

#include "global/activity/activity.hpp"

#include <map>
#include <vector>

class ActivityMazeRacer : public Activity
{
public:
	ActivityMazeRacer(ActivityManager *activity_manager);
	~ActivityMazeRacer();

	virtual void OnActivityStatusChange(int from_status, int to_status);
	virtual void Update(unsigned long interval, time_t now_second);
	virtual void OnUserLogin(Role *role);

private:
};

#endif