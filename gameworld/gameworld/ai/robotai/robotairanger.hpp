#ifndef __ROBOT_AI_RANGER_HPP__
#define __ROBOT_AI_RANGER_HPP__

#include "ai/aibase.hpp"
#include "obj/character/character.h"
#include "robotaibase.hpp"



class Role;

class RobotAIRanger : public RobotAIBase
{
public:
	RobotAIRanger(Character *me);
	virtual ~RobotAIRanger();

	virtual void AI(time_t now_second);


protected:
	virtual int OnActionEnd(time_t now_second);

	time_t m_next_idle_check_time;
};


#endif