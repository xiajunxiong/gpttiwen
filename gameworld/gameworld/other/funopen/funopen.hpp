#ifndef __FUN_OPEN_HPP__
#define __FUN_OPEN_HPP__

#include "funopendef.hpp"

class Role;
class FunOpen
{
public:
	static bool IsFunOpen(Role* role, int fun_type, bool is_notice = false);
	static int GetFunLimitLevel(int fun_type);
};

#endif