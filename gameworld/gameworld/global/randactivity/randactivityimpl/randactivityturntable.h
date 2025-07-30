#ifndef __RANDACTIVITY_TURN_TABLE_H__
#define __RANDACTIVITY_TURN_TABLE_H__


#include "global/randactivity/randactivity.hpp"

class RandActivityTurnTable : public RandActivity
{
public:
	RandActivityTurnTable(RandActivityManager *activity_manager);
	virtual ~RandActivityTurnTable();

};

#endif