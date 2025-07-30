#ifndef __RAND_ACTIVITY_ZONG_XIANG_MAN_CHENG_HPP__
#define __RAND_ACTIVITY_ZONG_XIANG_MAN_CHENG_HPP__

#include "global/randactivity/randactivity.hpp"
#include "protocol/msgrandactivity.hpp"

class RandActivityZongXiangManCheng :public RandActivity
{
public:
	RandActivityZongXiangManCheng(RandActivityManager *activity_manager);
	~RandActivityZongXiangManCheng();

	virtual void OnUserLogin(Role *user);
	virtual void Update(unsigned long interval, time_t now_second);
	virtual void OnSpecialActivityStatusChange(int from_status, int to_status);

	

	void RASendGameInfo(Role * role = NULL);
private:

	
};


#endif