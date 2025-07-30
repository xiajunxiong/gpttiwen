#ifndef __ROLE_ACTIVITY_MI_JING_QI_WEN_2_HPP__
#define __ROLE_ACTIVITY_MI_JING_QI_WEN_2_HPP__

#include "other/roleactivity/roleactivity.hpp"

class RoleActivityMiJingQiWen2 : public RoleActivity
{
public:
	RoleActivityMiJingQiWen2(int type);
	virtual ~RoleActivityMiJingQiWen2();
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);
	void* operator new(size_t c);
	void operator delete(void* m);
private:
	void OnRoleEnterSpecialFb();
};


#endif
