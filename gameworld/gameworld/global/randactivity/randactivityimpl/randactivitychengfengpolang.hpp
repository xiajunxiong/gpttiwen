#ifndef __RAND_ACTIVITY_CHENG_FENG_PO_LANG_HPP__
#define __RAND_ACTIVITY_CHENG_FENG_PO_LANG_HPP__

#include "global/randactivity/randactivity.hpp"
#include "protocol/msgrandactivity.hpp"

class RandActivityChengFengPoLang :public RandActivity
{
public:
	RandActivityChengFengPoLang(RandActivityManager *activity_manager);
	~RandActivityChengFengPoLang();

	virtual void OnUserLogin(Role *user);
	virtual void Update(unsigned long interval, time_t now_second);
	virtual void OnSpecialActivityStatusChange(int from_status, int to_status);

	bool GetIsOpen() { return !!m_is_open; }

	void RASendGameInfo(Role * role = NULL);
private:

	int m_is_open;
};


#endif