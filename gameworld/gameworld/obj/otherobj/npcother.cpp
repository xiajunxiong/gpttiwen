#include "npcother.h"
#include "npc/npcmanager.h"
#include "scene/scene.h"
#include "protocol/msgnpc.h"

NPCOther::NPCOther() :NPC(Obj::OBJ_TYPE_NPC_FUNC)
{
}

NPCOther::~NPCOther()
{
}

void NPCOther::NotifyInfo()
{
	Protocol::SCNPCInfoNotify ngn;
	ngn.obj_id = this->GetId();
	ngn.param1 = m_extra_info.param1;
	ngn.param2 = m_extra_info.param2;
	ngn.param3 = m_extra_info.param3;

	m_scene->GetZoneMatrix()->NotifyAreaMsg(m_obser_handle, (void *)&ngn, sizeof(ngn));
}

NPCExtraInfo & NPCOther::GetNpcExtraInfo()
{
	return m_extra_info;
}
