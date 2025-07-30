#ifndef __AIBASE_HPP__
#define __AIBASE_HPP__

#include "gamedef.h"

class Character;

class AIBase
{
	friend class Monster;

public:
	AIBase(Character *me) : m_me(me) {}
	virtual ~AIBase() {}


	virtual void AI(time_t now_second) = 0;
	virtual void Update(unsigned long interval, time_t now_second) = 0;

protected:

	Character *m_me;
};
#endif  // __AIBASE_HPP__