#include "activitymazeracer.hpp"
#include "scene/speciallogicimpl/mazeracermanager.hpp"

ActivityMazeRacer::ActivityMazeRacer(ActivityManager * activity_manager) : Activity(activity_manager, ACTIVITY_TYPE_MAZE_RACER)
{
}

ActivityMazeRacer::~ActivityMazeRacer()
{
}

void ActivityMazeRacer::OnActivityStatusChange(int from_status, int to_status)
{
	Activity::OnActivityStatusChange(from_status, to_status);

	switch (to_status)
	{
	case ACTIVITY_STATUS_STANDY:
		MazeRacerManager::Instance().OnActivityStandy();
		break;
	case ACTIVITY_STATUS_OPEN:
		MazeRacerManager::Instance().OnActivityOpen();
		break;
	case ACTIVITY_STATUS_CLOSE:
		MazeRacerManager::Instance().OnActivityEnd();
		break;
	}
}

void ActivityMazeRacer::Update(unsigned long interval, time_t now_second)
{
	MazeRacerManager::Instance().Update(now_second);  
	Activity::Update(interval, now_second);
	MazeRacerManager::Instance().CheckIssueActivityAward(now_second, m_next_open_wday);
}

void ActivityMazeRacer::OnUserLogin(Role * role)
{
	if (NULL == role) return;

	Activity::OnUserLogin(role);
	MazeRacerManager::Instance().SendAllRankInfo(role);
}