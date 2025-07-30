#include "specialappearance.hpp"
#include "obj/character/role.h"
#include "scene/scene.h"
#include "engineadapter.h"
#include "other/event/eventhandler.hpp"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/errornum.h"
#include "config/logicconfigmanager.hpp"
#include "other/rolemodulemanager.hpp"
#include "protocol/msgscene.h"

SpecialAppearance::SpecialAppearance() : m_mgr(NULL), m_show_appearance_type(0), m_show_appearance_param(0)
{

}

SpecialAppearance::~SpecialAppearance()
{

}

void SpecialAppearance::Init(RoleModuleManager* mgr, const SpecialAppearanceParam& param)
{
	m_mgr = mgr;
	m_param = param;
	this->CalcAppearance();
}

void SpecialAppearance::GetInitParam(SpecialAppearanceParam* out_param)
{
	if (NULL != out_param) *out_param = m_param;
}

void SpecialAppearance::Update(unsigned long interval, unsigned int now_second)
{
	short old_type = m_show_appearance_type;
	int old_id = m_show_appearance_param;

	// 在此处判断特殊形象是否都已经用完时间了
	
	this->CalcAppearance();
	if (old_type != m_show_appearance_type || old_id != m_show_appearance_param)
	{
		this->SendAppearance(true);
	}

}

void SpecialAppearance::SendAppearance(bool is_broadcast)
{
	/*
	static Protocol::SCSpecialAppearanceChangeNotice rsac;
	rsac.obj_id = m_mgr->GetRole()->GetId();
	rsac.special_appearance_type = m_show_appearance_type;
	rsac.special_appearance_param = m_show_appearance_param;

	if (is_broadcast)
	{
		if (NULL != m_mgr->GetRole()->GetScene())
		{
			m_mgr->GetRole()->GetScene()->GetZoneMatrix()->NotifyAreaMsg(m_mgr->GetRole()->GetObserHandle(), (void *)&rsac, sizeof(rsac));
		}
	}
	else
	{
		EngineAdapter::Instance().NetSend(m_mgr->GetRole()->GetNetId(), (const char*)&rsac, sizeof(rsac));
	}
	*/
}

void SpecialAppearance::CalcAppearance()
{
	m_show_appearance_type = 0;
	m_show_appearance_param = 0;
}
