#ifndef __RAND_ACTIVITY_SHAN_HAI_QUESTION_H__
#define __RAND_ACTIVITY_SHAN_HAI_QUESTION_H__

#include "global/randactivity/randactivity.hpp"
#include "other/roleactivity/impl/roleactivityshanhaiquestion.hpp"
#include "protocol/randactivity/msgshanhaiquestion.hpp"


class RoleActivityShanHaiQuestion;
class RandActivityShanHaiQuestion : public RandActivity
{
public:
	RandActivityShanHaiQuestion(RandActivityManager *activity_manager);
	~RandActivityShanHaiQuestion();

	virtual void Update(unsigned long interval, time_t now_second);
	virtual void OnSpecialActivityStatusChange(int from_status, int to_status);
	bool GetActivityStatus() { return is_open; }
	
private:
	time_t ForHourMinuteGetTimestamp(int hour_minute);
	void SetActivityTime();

	void SendActivityStatusAllInfo();
	


	time_t start_time;
	time_t end_time;
	bool is_open ;
};


#endif
