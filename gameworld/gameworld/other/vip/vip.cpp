#include "vip.hpp"

class Vip::VipImpl {
public:
	VipImpl() {}
	~VipImpl() {}

	int TestHotFix(Vip::VipImpl* this_ptr);
	int GetLevel(Vip::VipImpl* this_ptr);

	RoleModuleManager* m_mgr;
	VipParam m_param;
};

int Vip::VipImpl::TestHotFix(Vip::VipImpl* this_ptr)
{
	return 1;
}

int Vip::VipImpl::GetLevel(Vip::VipImpl* this_ptr)
{
	return m_param.vip_level;
}

Vip::Vip()
	: m_pImpl(new VipImpl())
{
}

Vip::~Vip()
{
	if (NULL != m_pImpl)
	{
		delete m_pImpl;
		m_pImpl = NULL;
	}
}

RoleModuleManager* Vip::GetRoleModuleManager()
{
	return m_pImpl->m_mgr;
}

void Vip::Init(RoleModuleManager *role_module_manager, const VipParam &vip_param)
{
	m_pImpl->m_mgr = role_module_manager;
	m_pImpl->m_param = vip_param;
}

void Vip::GetOtherInitParam(VipParam *vip_param)
{
	*vip_param = m_pImpl->m_param;
}

int Vip::GetVipLevel()
{
	return 0;
}
