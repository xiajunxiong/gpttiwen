#ifndef __RAND_ACTIVITY_LUCKY_BLIND_BOX_HPP__
#define __RAND_ACTIVITY_LUCKY_BLIND_BOX_HPP__

#include "global/randactivity/randactivity.hpp"
#include "protocol/msgrandactivity.hpp"
#include "servercommon/roleactivity/raluckyblindboxparam.hpp"

class RandActivityLuckyBlindBox :public RandActivity
{
public:
	RandActivityLuckyBlindBox(RandActivityManager *activity_manager);
	~RandActivityLuckyBlindBox();
private:
};


#endif