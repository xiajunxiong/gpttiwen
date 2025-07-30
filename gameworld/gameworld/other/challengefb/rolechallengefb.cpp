#include "rolechallengefb.hpp"
#include "protocol/msgchallengefb.h"
#include "other/rolemodulemanager.hpp"
#include "engineadapter.h"
#include "obj/character/role.h"
#include "servercommon/logdef.h"

RoleChallengeFb::RoleChallengeFb()
{
}

RoleChallengeFb::~RoleChallengeFb()
{
}

void RoleChallengeFb::InitParam(RoleModuleManager * mgr, const ChallengeFbParam & param)
{
	m_mgr = mgr;
	m_param = param;
}

void RoleChallengeFb::GetChallengeFbParam(ChallengeFbParam * param)
{
	*param = m_param;
}

void RoleChallengeFb::OnWeekChange()
{
	memset(m_param.week_highest_layer, 0, sizeof(m_param.week_highest_layer));
}

void RoleChallengeFb::OnRoleLogin()
{
	this->SendRoleChallengeFbInfo();
}

void RoleChallengeFb::OnPassChallengeFb(int fb_id, int chapter)
{
	if (fb_id < 0 || fb_id >= CHALLENGE_FB_ID_MAX || chapter < 0 || chapter >= MAX_UINT8) return;

	if (chapter > m_param.week_highest_layer[fb_id]) m_param.week_highest_layer[fb_id] = chapter;
	this->SendRoleChallengeFbInfo();
	m_mgr->GetRole()->LogActive(LOG_ACTIVE_TYPE_CHALLENGE_FB);
}

int RoleChallengeFb::GetHighestPass(int fb_id)
{
	if (fb_id < 0 || fb_id >= CHALLENGE_FB_ID_MAX) return 0;

	return m_param.week_highest_layer[fb_id];
}

void RoleChallengeFb::SendRoleChallengeFbInfo()
{
	Protocol::SCChallengeFbRoleInfor cfri;
	memcpy(cfri.highest_pass_chapter, m_param.week_highest_layer, sizeof(cfri.highest_pass_chapter));
	cfri.reserve_ush = 0;

	int tmp_interval = EngineAdapter::Instance().NextWeekInterval(1, 0, 0, 0);
	cfri.next_refresh_timestamp = (UInt32)(EngineAdapter::Instance().Time() + tmp_interval);

	m_mgr->NetSend((const void *)&cfri, sizeof(cfri));
}