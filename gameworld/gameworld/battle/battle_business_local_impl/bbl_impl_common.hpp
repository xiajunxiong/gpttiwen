#ifndef __BBL_IMPL_NORMAL_MONSTER_HPP__
#define __BBL_IMPL_NORMAL_MONSTER_HPP__

#include "battle/battle_business_local.hpp"

class BBLImplCommon : public BattleBusinessLocal
{
public:
	BBLImplCommon();
	virtual ~BBLImplCommon();

	void* operator new(size_t c);
	void operator delete(void* m);
};

#endif