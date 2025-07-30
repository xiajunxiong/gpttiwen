#ifndef __ROLE_ACTIVITY_MI_JING_QI_WEN_HPP__
#define __ROLE_ACTIVITY_MI_JING_QI_WEN_HPP__

#include "other/roleactivity/roleactivity.hpp"

class RoleActivityMiJingQiWen : public RoleActivity
{
public:
	RoleActivityMiJingQiWen(int type);
	virtual ~RoleActivityMiJingQiWen();
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);
	void* operator new(size_t c);
	void operator delete(void* m);
private:
	void OnRoleEnterSpecialFb();
};


#endif
