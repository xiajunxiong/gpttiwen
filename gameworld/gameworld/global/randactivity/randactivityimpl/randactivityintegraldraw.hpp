#ifndef __RAND_ACTIVITY_INTEGRAL_DRAW_HPP__
#define __RAND_ACTIVITY_INTEGRAL_DRAW_HPP__

#include "global/randactivity/randactivity.hpp"
#include "servercommon/roleactivity/rahorcruxtianchengparam.hpp"

class RandActivityIntegralDraw : public RandActivity
{
public:
	RandActivityIntegralDraw(RandActivityManager *activity_manager);
	~RandActivityIntegralDraw();

	virtual void OnUserLogin(Role * user);
	virtual void OnSpecialActivityStatusChange(int from_status, int to_status);
	virtual void Init(const ActivityData & arg_data);
	virtual void GetInitParam(ActivityData & arg_data);

	virtual void Update(unsigned long interval, time_t now_second);

	void RASendGameInfo(Role * role = NULL);
	bool IsGameOpen() { return m_is_game_open != 0; }

private:
	int m_is_game_open;
};

#endif
