#include "scene/scene.h"
#include "scene/speciallogic.hpp"
#include "obj/character/role.h"

#include "specialtransport.h"

SpecialTransport::SpecialTransport():Obj(OBJ_TYPE_SPECIAL_TRANSPORT)
{
}

SpecialTransport::~SpecialTransport()
{
}

void SpecialTransport::Transport(Role * role)
{
	if (role == NULL)
	{
		return;
	}

	if ((m_posi- role->GetPos()).IsLongerThan(30))
	{
		return;
	}

	SpecialLogic * special_lobgic = m_scene->GetSpecialLogic();
	if (special_lobgic == NULL)
	{
		return;
	}

	special_lobgic->OnTransport(role, this);
}

SpecialTransportExtraInfo & SpecialTransport::GetExtraInfo()
{
	return m_extra_info;
}
