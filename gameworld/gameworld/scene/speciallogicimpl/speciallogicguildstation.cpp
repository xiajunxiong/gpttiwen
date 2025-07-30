#include "speciallogicguildstation.hpp"
#include "global/guild/guildmanager.hpp"
#include "scene/scene.h"
#include "obj/character/role.h"

SpecialLogicGuildStation::SpecialLogicGuildStation(Scene * scene) : SpecialLogic(scene)
{
}

SpecialLogicGuildStation::~SpecialLogicGuildStation()
{
}

bool SpecialLogicGuildStation::CanDestroy()
{
	Guild * guild = GuildManager::Instance().GetGuild(m_scene->GetSceneKey());
	return NULL == guild;	// 如果家族解散了，就可以删除驻地了。
}

void SpecialLogicGuildStation::OnRoleEnterScene(Role * role)
{
	int guild_id = m_scene->GetSceneKey();
	Guild * guild = GuildManager::Instance().GetGuild(guild_id);
	if (NULL == guild || guild_id != role->GetGuildID()) this->DelayKickOutRole(role);
}
