#include "originbusinessregister.hpp"
#include "servercommon/servercommon.h"
#include "engineadapter.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "global/team/team.hpp"
#include "global/team/teammanager.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "obj/character/role.h"
#include "internalcomm.h"

static OriginBusinessRegister* g_OBR_instance = NULL;
OriginBusinessRegister& OriginBusinessRegister::Instance()
{
	if (NULL == g_OBR_instance)
	{
		g_OBR_instance = new OriginBusinessRegister();
	}

	return *g_OBR_instance;
}

void OriginBusinessRegister::Update(unsigned long interval, unsigned int now)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	OriginBusinessInfoMap::iterator it = m_role_businiess_map.begin();
	std::vector<int> erase_uid_list;
	for (; it != m_role_businiess_map.end(); ++it)
	{
		if (now > it->second.register_timestamp + 10)
		{
			erase_uid_list.push_back(it->first);
		}
	}

	for (size_t i = 0; i < erase_uid_list.size(); ++i)
	{
		m_role_businiess_map.erase(erase_uid_list[i]);
	}
}

void OriginBusinessRegister::OnUserLogin(const UserID& user_id)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	OriginBusinessInfoMap::iterator it = m_role_businiess_map.find(UidToInt(user_id));
	if (it == m_role_businiess_map.end()) return;

	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
	if (NULL == role) return;

	OriginBusinessInfo& info = it->second;
	switch (info.business.business_type)
	{
	case ReturnOriginData_Business::BUSINESS_TYPE_TEAM_SWITCH_TO_ORIGIN:
		{
			Team* team = role->GetMyTeam();
			if (NULL == team)
			{
				team = TeamManager::Instance().CreateTeam(role, info.business.level_limit_low, info.business.level_limit_high, info.business.team_type, false);
			}

			if (NULL != team)
			{
				static crossgameprotocal::GameHiddenTeammateJoinOriginServerTeam ghtjost;
				ghtjost.team_index = team->GetTeamIndex();
				ghtjost.business_data = info.business;

				InternalComm::Instance().SendToHiddenThroughCross((const char *)&ghtjost, sizeof(ghtjost));
			}
		}
		break;

	case ReturnOriginData_Business::BUSINESS_TYPE_JOIN_ORIGIN_TEAM:
		{
			TeamManager::Instance().JoinTeamDirectly(role, info.business.join_team_id);
		}
		break;
	}

	m_role_businiess_map.erase(UidToInt(user_id));
}

void OriginBusinessRegister::RegisterBusiness(int role_id, const ReturnOriginData_Business& business)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	OriginBusinessInfo info;
	info.role_id = role_id;
	info.register_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	info.business = business;

	m_role_businiess_map.insert(OriginBusinessInfoMap::value_type(role_id, info));
}

OriginBusinessRegister::OriginBusinessRegister()
{

}

OriginBusinessRegister::~OriginBusinessRegister()
{

}

